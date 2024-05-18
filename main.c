#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/*
    I'LL ADD A NEW FIELD IN THE OPERAND TYPE, isvalid
    if (isvalid == false) => no operand expected
    else 
        more operands expected


    THE OPERAND VALUE CAN BE A WORD (ANY IMMEDIATE OR REG VALUE) 
    OR A LABEL : {
        NAME,
        ADDRESS
    }

    ADD LABEL INSTRUCTION WILL ADD A NEW LABEL IN THE LABELS IN THE MACHINE 

*/

#define STACK_CAPACITY 100
#define RAM_CAPACITY 100
#define PROG_CAPACITY 100
#define LABELS_CAPACITY 100
#define ARRAY_SIZE(arr) sizeof(arr) / sizeof((arr)[0])

typedef uint16_t word;

typedef enum {
    AX = 0,
    BX, 
    CX, 
    DX, 
    SI, 
    DI, 
    BP, 
    SP, 
    PC, 
    FLAGS,
    REG_COUNT,
} REG;

char *REGS_NAME[REG_COUNT] = {
    "AX",
    "BX", 
    "CX", 
    "DX", 
    "SI", 
    "DI", 
    "BP", 
    "SP", 
    "PC", 
    "FLAGS",
};

typedef enum {
    INST_MOVE = 0,  // MOV REG, IMM
    INST_ADD,       // ADD REG, REG
    INST_SUB,       // SUB REG, REG
    INST_MUL,       // MUL REG, REG
    INST_DIV,       // DIV REG, REG
    INST_PUSH,      // PUSH REG
    INST_POP,       // POP REG
    INST_JUMP,      // JUMP LABEL
    MAKE_LABEL,     // LABEL
    INST_COUNT,
} INST_TYPE;

char *INST_TYPE_NAME[INST_COUNT] = {
    "MOV", 
    "ADD",       
    "SUB",       
    "MUL",       
    "DIV",       
    "PUSH",       
    "POP",
    "JMP",
    "ADD_LAB",
};

typedef enum {
    REGISTER = 0,
    VARIABLE,
    IMMEDIATE,
    NO_TYPE,
    LABEL_TYPE,
    OPERAND_TYPE_COUNT,
} OPERAND_TYPE;

typedef struct {
    char name[30];
    word address;
} LABEL;

typedef union {
    word value;
    LABEL label;
} OPERAND_VALUE;

typedef struct {
    OPERAND_TYPE type;
    OPERAND_VALUE op_value;
    bool isvalid;
} OPERAND;

typedef struct {
    INST_TYPE type;
    OPERAND ops[2];    // MAX NUMBER OF OPERANDS IS 2
} INST;

typedef struct {
    word stack[STACK_CAPACITY];
    word ram[RAM_CAPACITY];
    word regs[REG_COUNT];

    INST prog[PROG_CAPACITY];
    size_t prog_size;


    LABEL labels[LABELS_CAPACITY];
    size_t labels_size;
} machine;

machine dm = {0};

// INITIALIZATION
OPERAND operand_init(OPERAND_TYPE type, word value, bool isvalid);
LABEL label_init(char *label_name, word address);
INST inst_init(INST_TYPE type, OPERAND first_op, OPERAND second_op);
void add_label(machine *dm, char *label_name, word address);
OPERAND operand_label_init(char *name, word address);

// GETTERS
word get_operand_value(machine *dm, OPERAND op);
int get_label(machine *dm, char *label_name);

// INSTRUCTIONS
void handle_move_instruction(machine *dm, INST curr_inst);
void handle_add_inst(machine *dm, INST curr_inst);
void handle_sub_inst(machine *dm, INST curr_inst);
void handle_mul_inst(machine *dm, INST curr_inst);
void handle_div_inst(machine *dm, INST curr_inst);
void handle_arithmetic_inst(machine *dm, INST curr_inst);
void handle_push_inst(machine *dm, INST curr_inst);
void handle_pop_inst(machine *dm, INST curr_inst);
void handle_jump_inst(machine *dm, INST curr_inst);

// DUMPING
void dump_regs(machine *dm);
void write_operand(OPERAND op);
void write_inst(INST inst);
void dump_stack(machine *dm);
void print_program(machine *dm);

void execute_inst(machine *dm) {
    INST current_inst = dm->prog[dm->regs[PC]];

    // update the program counter:
    dm->regs[PC]++;

    switch (current_inst.type) {
        case INST_MOVE:
            handle_move_instruction(dm, current_inst);
            dump_regs(dm);
            break;
        case INST_ADD:
        case INST_SUB:
        case INST_MUL:
        case INST_DIV:
            handle_arithmetic_inst(dm, current_inst);
            dump_regs(dm);
            break;
        case INST_PUSH:
            handle_push_inst(dm, current_inst);
            break;
        case INST_POP:
            handle_pop_inst(dm, current_inst);
            break;
        case MAKE_LABEL:
            add_label(dm, current_inst.ops[0].op_value.label.name, current_inst.ops[0].op_value.label.address);
            break;
        case INST_JUMP:
            handle_jump_inst(dm, current_inst);
            break;
        default:
            break;
    }
}

INST prog[PROG_CAPACITY] = {0};
size_t curr_size = 0;

void push_inst(INST_TYPE type, OPERAND op1, OPERAND op2) {
    prog[curr_size++] = inst_init(type, op1, op2);
}

void make_prog() {
    // INITIALIZING A TESTING PROGRAM
    OPERAND op1 = {0};
    OPERAND op2 = {0};

    
    op1 = operand_init(REGISTER, AX, true);
    op2 = operand_init(IMMEDIATE, 0, true);
    push_inst(INST_MOVE, op1, op2); // mov ax, 0

    op1 = operand_init(REGISTER, BX, true);
    op2 = operand_init(IMMEDIATE, 1, true);   
    push_inst(INST_MOVE, op1, op2); // mov bx, 1
    
    op2.isvalid = false;
    op1 = operand_label_init("loop", curr_size);
    push_inst(MAKE_LABEL, op1, op2); // loop:

    op1 = operand_init(REGISTER, CX, true);
    op2 = operand_init(REGISTER, AX, true);
    push_inst(INST_MOVE, op1, op2);  // mov cx, ax

    op1 = operand_init(REGISTER, AX, true);
    op2 = operand_init(REGISTER, BX, true);
    push_inst(INST_ADD, op1, op2);  // add ax, bx
    
    
    op1 = operand_init(REGISTER, BX, true);
    op2 = operand_init(REGISTER, CX, true);
    push_inst(INST_MOVE, op1, op2);  // mov bx, cx
    
    // op1 = operand_init(REGISTER, AX, true);
    // op2 = operand_init(REGISTER, BX, true);
    // push_inst(INST_DIV, op1, op2);
    
    // op1 = operand_init(REGISTER, AX, true);
    // op2.isvalid = false;
    // push_inst(INST_PUSH, op1, op2);
    
    

    // op1 = operand_init(REGISTER, BX, true);
    // push_inst(INST_PUSH, op1, op2);

    // op1 = operand_init(REGISTER, CX, true);
    // push_inst(INST_PUSH, op1, op2);

    // op1 = operand_init(REGISTER, AX, true);
    // push_inst(INST_POP, op1, op2);

    op2.isvalid = false;
    op1 = operand_label_init("loop", 0);
    push_inst(INST_JUMP, op1, op2);   // jmp loop
}

void load_prog(machine *dm) {
    // ITERATE OVER ALL THE INSTRUCITIONS IN THE prog AND PUT THEM IN dm->prog
    for (size_t i = 0; i < curr_size; i++) 
        dm->prog[i] = prog[i];
    
    // UPDATE THE PROGRAM SIZE TO BE THE CURR_SIZE
    dm->prog_size = curr_size;
}

void execute(machine *dm) {
    size_t count = 0;
    size_t limit = curr_size * 20; // execution limit 
    while (dm->regs[PC] < dm->prog_size) {
        if (count == limit)
            break;
        count++;
        write_inst(dm->prog[dm->regs[PC]]);
        execute_inst(dm);
    }
}

void machine_init(machine *dm) {
    // MACHINE IS ALREADY INITIALIZED WITH ZERO
    // ALL I HAVE TO DO IS TO LOAD THE PROGRAM
    load_prog(dm);
}

int main(void) {
    // ADD INSTRUCTIONS
    make_prog();

    // INITIALIZE THE MACHINE
    machine_init(&dm);

    // PRINT THE PROGRAM
    print_program(&dm);
    printf("\n");

    // EXECUTE INSTRUCTIONS
    execute(&dm);

    // for (size_t i = 0; i < curr_size; i++) 
    //     write_inst(prog[i]);
    return 0;
}



// INITIALIZE AN OPERAND OF TYPE != LABEL_TYPE
OPERAND operand_init(OPERAND_TYPE type, word value, bool isvalid) {
    return (OPERAND) {
        .type = type,
        .op_value.value = value,
        .isvalid = isvalid,
    };   
}

// INITIALIZE AN OPERAND OF TYPE == LABEL_TYPE
OPERAND operand_label_init(char *name, word address) {
    return (OPERAND) {
        .type = LABEL_TYPE,
        .op_value.label = label_init(name, address),
        .isvalid = true,
    };
}

// INITIALIZE AN INSTRUCTION
INST inst_init(INST_TYPE type, OPERAND first_op, OPERAND second_op) { 
    return (INST) {
        .type = type,
        .ops[0] = first_op,
        .ops[1] = second_op,
    };
}

// GET THE OPERAND VALUE 
// REGISTER -> REGISTER VALUE
// VARIABLE -> VARIABLE VALUE
// IMMEDIATE -> THE VALUE ITSELF
// LABEL    -> ADDRESS

word get_operand_value(machine *dm, OPERAND op) {
    switch(op.type) {
        case LABEL_TYPE:
            // returning the label address
            return get_label(dm, op.op_value.label.name);
        case REGISTER:
            // getting the reg value
            return dm->regs[op.op_value.value]; 
        case IMMEDIATE:
            return op.op_value.value;
        case VARIABLE:
            // getting the variable value from the ram
            return dm->ram[op.op_value.value];
        default:
            fprintf(stderr, "INVALID TYPE\n");
            exit(1);
            break;
    }   
}

// SETS THE OPERAND VALUE 
// REGISTER => REG VALUE 
// VARIABLE => VARIABLE VALUE
void set_operand(machine *dm, OPERAND op, word value) {
    switch(op.type) {
        case REGISTER:
            dm->regs[op.op_value.value] = value;
            break;
        case VARIABLE:
            dm->ram[op.op_value.value] = value;
            break;
        default:
            fprintf(stderr, "INVALID OPERAND TYPE\n");
            exit(1);
    }
}

void handle_move_instruction(machine *dm, INST curr_inst) {
    // getting the operands
    OPERAND op1 = curr_inst.ops[0];
    OPERAND op2 = curr_inst.ops[1];

    if (!op1.isvalid || !op2.isvalid) {
        fprintf(stderr, "MORE OPERANDS EXPECTED\n");
        exit(1);
    }

    // invalid operand type (for the first operand)
    if (op1.type == IMMEDIATE) {
        fprintf(stderr, "INVALID FIRST OPERAND, FOUND IMMEDIATE, EXPECTED OTHERS\n");
        exit(1);
    }

    word op2_value = get_operand_value(dm, op2);

    // making the move instruction
    set_operand(dm, op1, op2_value);
}

void handle_add_inst(machine *dm, INST curr_inst) {
    // getting the operands
    OPERAND op1 = curr_inst.ops[0];
    OPERAND op2 = curr_inst.ops[1];

    if (!op1.isvalid || !op2.isvalid) {
        fprintf(stderr, "MORE OPERANDS EXPECTED\n");
        exit(1);
    }

    // invalid operand type (for the first operand)
    if (op1.type == IMMEDIATE) {
        fprintf(stderr, "INVALID FIRST OPERAND, FOUND IMMEDIATE, EXPECTED OTHERS\n");
        exit(1);
    }

    word op1_value = get_operand_value(dm, op1);
    word op2_value = get_operand_value(dm, op2);

    word result = op1_value + op2_value;

    set_operand(dm, op1, result);
}


void handle_sub_inst(machine *dm, INST curr_inst) {
    // getting the operands
    OPERAND op1 = curr_inst.ops[0];
    OPERAND op2 = curr_inst.ops[1];

    if (!op1.isvalid || !op2.isvalid) {
        fprintf(stderr, "MORE OPERANDS EXPECTED\n");
        exit(1);
    }

    // invalid operand type (for the first operand)
    if (op1.type == IMMEDIATE) {
        fprintf(stderr, "INVALID FIRST OPERAND, FOUND IMMEDIATE, EXPECTED OTHERS\n");
        exit(1);
    }

    word op1_value = get_operand_value(dm, op1);
    word op2_value = get_operand_value(dm, op2);

    word result = op1_value - op2_value;

    set_operand(dm, op1, result);
}

void handle_mul_inst(machine *dm, INST curr_inst) {
    // getting the operands
    OPERAND op1 = curr_inst.ops[0];
    OPERAND op2 = curr_inst.ops[1];

    if (!op1.isvalid || !op2.isvalid) {
        fprintf(stderr, "MORE OPERANDS EXPECTED\n");
        exit(1);
    }

    // invalid operand type (for the first operand)
    if (op1.type == IMMEDIATE) {
        fprintf(stderr, "INVALID FIRST OPERAND, FOUND IMMEDIATE, EXPECTED OTHERS\n");
        exit(1);
    }

    word op1_value = get_operand_value(dm, op1);
    word op2_value = get_operand_value(dm, op2);

    word result = op1_value * op2_value;

    set_operand(dm, op1, result);
}


void handle_div_inst(machine *dm, INST curr_inst) {
    // getting the operands
    OPERAND op1 = curr_inst.ops[0];
    OPERAND op2 = curr_inst.ops[1];

    if (!op1.isvalid || !op2.isvalid) {
        fprintf(stderr, "MORE OPERANDS EXPECTED\n");
        exit(1);
    }

    // invalid operand type (for the first operand)
    if (op1.type == IMMEDIATE) {
        fprintf(stderr, "INVALID FIRST OPERAND, FOUND IMMEDIATE, EXPECTED OTHERS\n");
        exit(1);
    }

    word op1_value = get_operand_value(dm, op1);
    word op2_value = get_operand_value(dm, op2);

    if (op2_value == 0) {
        fprintf(stderr, "DIVISION BY ZERO ISN'T ALLOWED\n");
        exit(1);
    }

    word result = op1_value / op2_value;

    set_operand(dm, op1, result);
}

void handle_arithmetic_inst(machine *dm, INST curr_inst) {
    switch (curr_inst.type) {
        case INST_ADD:
            handle_add_inst(dm, curr_inst);
            break;
        case INST_SUB: 
            handle_sub_inst(dm, curr_inst);
            break;
        case INST_MUL:
            handle_mul_inst(dm, curr_inst);
            break;
        case INST_DIV:
            handle_div_inst(dm, curr_inst);
            break;
        default:
            break;
    }
}

void handle_push_inst(machine *dm, INST curr_inst) {
    if (dm->regs[SP] == STACK_CAPACITY) {
        fprintf(stderr, "CAN NOT EXECUTE THE PUSH INSTRUCTION, STACK OVERFLOW ERROR\n");
        exit(1);
    }

    OPERAND op = curr_inst.ops[0];
    
    if (!op.isvalid) {
        fprintf(stderr, "NO OPERAND TO PUSH\n");
        exit(1);
    }

    word operand_value = get_operand_value(dm, op);
    dm->stack[dm->regs[SP]++] = operand_value;
    dump_stack(dm);             // for testing purposes!
} 

void handle_pop_inst(machine *dm, INST curr_inst) {
    if (dm->regs[SP] == 0) {
        fprintf(stderr, "CAN NOT EXECUTE THE POP INSTRUCTION, STACK UNDERFLOW ERROR\n");
        exit(1);
    }

    OPERAND op = curr_inst.ops[0];
    
    if (!op.isvalid) {
        fprintf(stderr, "NO OPERAND TO POP\n");
        exit(1);
    }

    if (op.type == IMMEDIATE) {
        fprintf(stderr, "INVALID FIRST OPERAND, FOUND IMMEDIATE, EXPECTED OTHERS\n");
        exit(1);
    }

    word poped_value = dm->stack[dm->regs[SP]--];
    set_operand(dm, op, poped_value);
    dump_stack(dm);             // for testing purposes!
} 

LABEL label_init(char *label_name, word address) {
    LABEL result;

    // COPY THE LABEL NAME
    for (size_t i = 0; i < 30; i++) {
        if (*label_name == '\0')
            break;
        result.name[i] = *label_name;
        label_name++;
    }

    result.address = address;

    return result;
}

// ADD LABEL TO THE MACHINE LABELS ARRAY
void add_label(machine *dm, char *label_name, word address) {
    if (dm->labels_size == LABELS_CAPACITY) {
        fprintf(stderr, "CAN NOT ADD A NEW LABEL\n");
        exit(1);
    }

    dm->labels[dm->labels_size++] = label_init(label_name, address);
}

// return -1 if the label isn't found
int get_label(machine *dm, char *label_name) {
    for (size_t i = 0; i < dm->labels_size; i++) {
        if (strcmp(dm->labels[i].name, label_name) == 0)
            return dm->labels[i].address;
    }
    return -1;
}

void handle_jump_inst(machine *dm, INST curr_inst) {
    if (dm->labels_size == 0) {
        fprintf(stderr, "NO LABELS PROVIDED\n");
        exit(1);
    } 

    OPERAND op = curr_inst.ops[0];
    int addr = get_label(dm, op.op_value.label.name);

    if (addr == -1) {
        fprintf(stderr, "LABEL %s IS NOT FOUND\n", op.op_value.label.name);
        exit(1);
    }

    dm->regs[PC] = addr + 1; // jump to the instruction after the add label instruction
}

void dump_regs(machine *dm) {
    printf("REGISTERS:\n");
    for (size_t i = 0; i < REG_COUNT; i++) 
        printf("\t%s: %d\n", REGS_NAME[i], dm->regs[i]);
    printf("\n");
}

void write_operand(OPERAND op) {
    switch (op.type) {
        case LABEL_TYPE:
            printf("%s", op.op_value.label.name);
            break;
        case REGISTER:
            printf("%s", REGS_NAME[op.op_value.value]);
            break;
        default:
            printf("%d", op.op_value.value);
            break;
    }
}

void write_inst(INST inst) {
    printf("%s ", INST_TYPE_NAME[inst.type]);

    if (inst.ops[0].isvalid) {
        write_operand(inst.ops[0]);
    }

    if (inst.ops[1].isvalid) {
        printf(", ");
        write_operand(inst.ops[1]);
    }
    
    printf("\n");
}

void dump_stack(machine *dm) {
    size_t i;
    printf("STACK:\n");
    for (i = dm->regs[SP]; i > 0; i--) {
        printf("\t%d\n", dm->stack[i - 1]);
    }
    printf("\n");
}

void print_program(machine *dm) {
    for (size_t i = 0; i < dm->prog_size; i++) {
        write_inst(dm->prog[i]);
    }
}