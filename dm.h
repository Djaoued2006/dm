#ifndef DM_H
#define DM_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include "./defs.h"
#include "./utils.h"
#include "./init.h"


#define RAISE_ERROR(_err)                           \
    do {                                            \
        dm->state = ERROR;                          \
        dm->err = _err;                             \
        return;                                     \
    } while(0)


void dm_init(Machine *dm) {
    dm->ip = 0;
    dm->sp = 0;
    dm->prog.prog_size = 0;
    dm->prog.label_count = 0;
    dm->state = RUNNING;
    dm->code = EXIT_OKEY;
    for (int i = 0; i < R_COUNT; ++i) dm->registers[i] = 0;
}


void dm_push_instruction(Machine *dm, Inst inst) {
    if (dm->prog.prog_size >= PROGRAM_CAPACITY) {
        fprintf(stderr, "ERROR: too much instructions\n");
        exit(1);
    }

    dm->prog.instructions[dm->prog.prog_size++] = inst;
}


// Helper Functions
void push(Machine *dm, Word value) {
    dm->stack[dm->sp++] = value;
}

Word pop(Machine *dm) {
    return (dm->stack[--dm->sp]);
}

// Implementation of the instruction set
void handle_push_inst(Machine *dm, Inst inst) {
    if (dm->sp >= STACK_CAPACITY) {
        RAISE_ERROR(STACK_OVERFLOW);
    }

    Word value;

    switch(inst.ops.op1.type) {
        case IMMEDIATE: 
            value = inst.ops.op1.value.as_imm;
            break;
        case REGISTER:
            Register reg = inst.ops.op1.value.as_reg;
            value = dm->registers[reg];
            break;
        default:
            RAISE_ERROR(WRONG_OPERAND_TYPE);
    }

    push(dm, value);
    dm->ip++;
}

void handle_pop_inst(Machine *dm, Inst inst) {
    if (dm->sp == 0) {
        RAISE_ERROR(STACK_UNDERFLOW);
    }

    Word value = pop(dm);

    switch(inst.ops.op1.type) {
        case REGISTER:
            Register reg = inst.ops.op1.value.as_reg;
            dm->registers[reg] = value;
            break;
        default:
            RAISE_ERROR(WRONG_OPERAND_TYPE);
    }

    dm->ip++;
}

void handle_arithemetic_inst(Machine *dm, Inst inst) {
    Word op1, op2;
    
    if (inst.ops.op1.type == NO_TYPE) {
        if (dm->sp < 2) { RAISE_ERROR(STACK_UNDERFLOW); }
        op2 = pop(dm);
        op1 = pop(dm);
    } else if (inst.ops.op1.type == REGISTER) {
        Register reg1 = inst.ops.op1.value.as_reg;
        op1 = dm->registers[reg1];
        
        if (inst.ops.op2.type == NO_TYPE) { RAISE_ERROR(FEW_OPERANDS); }
        
        if (inst.ops.op2.type == REGISTER) {
            Register reg2 = inst.ops.op2.value.as_reg;
            op2 = dm->registers[reg2];
        } else if (inst.ops.op2.type == IMMEDIATE) {
            op2 = inst.ops.op2.value.as_imm;
        } else { RAISE_ERROR(WRONG_OPERAND_TYPE); }


    } else { RAISE_ERROR(WRONG_OPERAND_TYPE); }

    switch (inst.type) {
        case INST_ADD:
            op1 += op2;
            break;
        case INST_SUB:
            op1 -= op2;
            break;
        case INST_MUL:
            op1 *= op2;
            break;
        case INST_DIV:
            if (op2 == 0) { RAISE_ERROR(DIVISION_BY_ZERO); }
            op1 /= op2;
            break;
    }

    if (inst.ops.op1.type == NO_TYPE) { push(dm, op1); }
    else dm->registers[inst.ops.op1.value.as_reg] = op1;

    dm->ip++;
}

void handle_inc_inst(Machine *dm, Inst inst) {
    switch (inst.ops.op1.type) {
        case REGISTER:
            Register reg = inst.ops.op1.value.as_reg;
            Word value = dm->registers[reg];
            value++;
            dm->registers[reg] = value;
            break;
        case NO_TYPE:
            if (dm->sp < 1) { RAISE_ERROR(STACK_UNDERFLOW); }
            Word op = pop(dm);
            push(dm, op + 1);
            break;
        default:
            RAISE_ERROR(WRONG_OPERAND_TYPE);
    }
    dm->ip++;
}

void handle_dec_inst(Machine *dm, Inst inst) {
    switch (inst.ops.op1.type) {
        case REGISTER:
            Register reg = inst.ops.op1.value.as_reg;
            Word value = dm->registers[reg];
            value--;
            dm->registers[reg] = value;
            break;
        case NO_TYPE:
            if (dm->sp < 1) { RAISE_ERROR(STACK_UNDERFLOW); }
            Word op = pop(dm);
            push(dm, op - 1);
            break;
        default:
            RAISE_ERROR(WRONG_OPERAND_TYPE);
    }
    dm->ip++;
}

void handle_cmp_inst(Machine *dm, Inst inst) {
    Word op1, op2;
    
    if (inst.ops.op1.type == NO_TYPE) {
        if (dm->sp < 2) { RAISE_ERROR(STACK_UNDERFLOW); }
        op2 = pop(dm);
        op1 = pop(dm);
    } else if (inst.ops.op1.type == REGISTER) {
        Register reg1 = inst.ops.op1.value.as_reg;
        op1 = dm->registers[reg1];
        
        if (inst.ops.op2.type == NO_TYPE) { RAISE_ERROR(FEW_OPERANDS); }
        
        if (inst.ops.op2.type == REGISTER) {
            Register reg2 = inst.ops.op2.value.as_reg;
            op2 = dm->registers[reg2];
        } else if (inst.ops.op2.type == IMMEDIATE) {
            op2 = inst.ops.op2.value.as_imm;
        } else { RAISE_ERROR(WRONG_OPERAND_TYPE); }


    } else { RAISE_ERROR(WRONG_OPERAND_TYPE); }

    op1 -= op2;
    
    push(dm, op1);

    dm->ip++;
}

size_t get_label_adress(Machine *dm, char *name) {
    for (size_t i = 0; i < dm->prog.label_count; ++i) {
        if (strcmp(name, dm->prog.labels[i].name) == 0)
            return dm->prog.labels[i].address;
    }

    fprintf(stderr, "ERROR: Label Not Found\n");
    exit(1);
}

void handle_jz_inst(Machine *dm, Inst inst) {
    if (dm->sp < 1) { RAISE_ERROR(STACK_UNDERFLOW); }
    Word value = pop(dm);
    if (value == 0) {
        char *label_name = inst.ops.op1.value.as_label;
        size_t address = get_label_adress(dm, label_name);
        dm->ip = address;
    } else { dm->ip++; };
}

void handle_jmp_inst(Machine *dm, Inst inst) {
    char *label_name = inst.ops.op1.value.as_label;
    size_t address = get_label_adress(dm, label_name);
    dm->ip = address;
}

void handle_jge_inst(Machine *dm, Inst inst) {
    if (dm->sp < 1) { RAISE_ERROR(STACK_UNDERFLOW); }
    Word value = pop(dm);
    if (value >= 0) {
        char *label_name = inst.ops.op1.value.as_label;
        size_t address = get_label_adress(dm, label_name);
        dm->ip = address;
    } else { dm->ip++; };
}

void handle_jle_inst(Machine *dm, Inst inst) {
    if (dm->sp < 1) { RAISE_ERROR(STACK_UNDERFLOW); }
    Word value = pop(dm);
    if (value <= 0) {
        char *label_name = inst.ops.op1.value.as_label;
        size_t address = get_label_adress(dm, label_name);
        dm->ip = address;
    } else { dm->ip++; };
}

void handle_jl_inst(Machine *dm, Inst inst) {
    if (dm->sp < 1) { RAISE_ERROR(STACK_UNDERFLOW); }
    Word value = pop(dm);
    if (value < 0) {
        char *label_name = inst.ops.op1.value.as_label;
        size_t address = get_label_adress(dm, label_name);
        dm->ip = address;
    } else { dm->ip++; };
}

void handle_jg_inst(Machine *dm, Inst inst) {
    if (dm->sp < 1) { RAISE_ERROR(STACK_UNDERFLOW); }
    Word value = pop(dm);
    if (value > 0) {
        char *label_name = inst.ops.op1.value.as_label;
        size_t address = get_label_adress(dm, label_name);
        dm->ip = address;
    } else { dm->ip++; };
}


void handle_halt_inst(Machine *dm, Inst inst) {
    (void)inst;
    dm->state = HALTED;
}

void handle_mov_inst(Machine *dm, Inst inst) {
    Word op;
    
    if (inst.ops.op1.type == REGISTER) {
        Register reg1 = inst.ops.op1.value.as_reg;
        
        if (inst.ops.op2.type == NO_TYPE) { RAISE_ERROR(FEW_OPERANDS); }
        
        if (inst.ops.op2.type == REGISTER) {
            Register reg2 = inst.ops.op2.value.as_reg;
            op = dm->registers[reg2];
        } else if (inst.ops.op2.type == IMMEDIATE) {
            op = inst.ops.op2.value.as_imm;
        } else { RAISE_ERROR(WRONG_OPERAND_TYPE); }     

        dm->registers[reg1] = op;
    } else { RAISE_ERROR(WRONG_OPERAND_TYPE); }

    dm->ip++;

}

void handle_call_inst(Machine *dm, Inst inst) {
    char *label_name = inst.ops.op1.value.as_label;
    size_t address = get_label_adress(dm, label_name);
    push(dm, (Word) (dm->ip + 1));
    dm->ip = address;
}

void handle_ret_inst(Machine *dm, Inst inst) {
    if (dm->sp < 1) {
        dm->err = STACK_UNDERFLOW;
        return;
    }

    dm->ip = pop(dm);
}

void handle_write_syscall(Machine *dm) {
    Word value = dm->registers[RAX];
    fprintf(stdout, "%c", (char)value);
}

void handle_read_syscall(Machine *dm) {
    char ch = getchar();
    dm->registers[RAX] = ch;
}

void handle_syscall_inst(Machine *dm, Inst inst) {
    // rbx contains the syscall you wanna perform
    // the input or output in rax

    switch(dm->registers[RBX]) {
        case SYS_WRITE:
            handle_write_syscall(dm);
            break;
        case SYS_READ:
            handle_read_syscall(dm);
            break;
        default:
            fprintf(stderr, "SYSCALL NOT IMPLEMENTED YET\n");
            exit(1);            
    }

    dm->ip++;
}


// executes the current instruction pointed by the ip
void dm_execute_inst(Machine *dm) {
    if (dm->ip >= dm->prog.prog_size) {
        fprintf(stderr, "ERROR: IP out of bounds\n");
        exit(1);
    }

    Inst current = dm->prog.instructions[dm->ip];
    switch(current.type) {
        case INST_PUSH:
            handle_push_inst(dm, current);
            break;
        case INST_POP:
            handle_pop_inst(dm, current);
            break;
        case INST_HALT:
            handle_halt_inst(dm, current);
            break;
        case INST_ADD:
        case INST_SUB:
        case INST_MUL:
        case INST_DIV:
            handle_arithemetic_inst(dm, current);
            break;
        case INST_INC:  
            handle_inc_inst(dm, current);
            break;
        case INST_DEC:
            handle_dec_inst(dm, current);
            break;
        case INST_CMP:
            handle_cmp_inst(dm, current);
            break;
        case INST_JZ:
            handle_jz_inst(dm, current);
            break;
        case INST_JMP:
            handle_jmp_inst(dm, current);
            break; 
        case INST_JGE:
            handle_jge_inst(dm, current);
            break;
        case INST_JLE:
            handle_jle_inst(dm, current);
            break;
        case INST_JG:
            handle_jg_inst(dm, current);
            break;
        case INST_JL:
            handle_jl_inst(dm, current);
            break;     
        case INST_MOV:
            handle_mov_inst(dm, current);
            break;  
        case INST_CALL:
            handle_call_inst(dm, current);
            break;
        case INST_RET:
            handle_ret_inst(dm, current);
            break;
        case INST_SYSCALL:
            handle_syscall_inst(dm, current);
            break;
        default:
            assert(false && "ERROR: 'dm_execute_inst' failed, Instruction type not implemented yet\n");
            break;
    }
}


void dm_execute(Machine *dm) {
    while (dm->state != HALTED) {
        dm_execute_inst(dm);
        if (dm->state == ERROR) {
            // fprintf(stderr, "ERROR: %s\n", err_to_cstr(dm->err));
            dm->state = HALTED;
            dm->code = EXIT_FAIL;
        }
    }
}

#endif