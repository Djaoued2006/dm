#include "./machine/dm.h"

char indent = 0;

void dm_get_address_from_label_name(Machine *dm, char *label_name, size_t *address) {
    for (size_t i = 0; i < dm->Labels.count; i++) {
        // GET THE LABEL
        Label label = (Label) {0};
        arr_get_item(&dm->Labels, &label, i);
        if (strcmp(label.name, label_name) == 0) {
            *address = label.address;
            return;
        }
    }
    dm->err = Label_Not_Found_Error;
}

int isstackempty(Machine *dm) {
    return (dm->sp == 0);
}
int isstackfull(Machine *dm) {
    return (dm->sp == STACK_CAPACITY);
}

void push(Machine *dm, Word value) {
    dm->stack[dm->sp++] = value;
}

Word pop(Machine *dm) {
    return dm->stack[--dm->sp];
}

void handle_push_inst(Machine *dm, Inst inst) {
    if(isstackfull(dm)) {
        dm->err = Stack_OverFlow_Error;
        return;
    }

    if (inst.val_type == REG) {
        push(dm, dm->regs[inst.operand.as_reg]);
    } else {
        push(dm, inst.operand.as_word);
    }

    dm->ip++;
}

void handle_arithmetic_inst(Machine *dm, Inst inst) {
    switch(inst.type) {
        case INST_ADD:
            HANDLE_ARITHMETIC_INSTRUCTION(+);
            break;
        case INST_SUB:
            HANDLE_ARITHMETIC_INSTRUCTION(-);
            break;
        case INST_MUL:
            HANDLE_ARITHMETIC_INSTRUCTION(*);
            break;
        case INST_DIV:
            HANDLE_ARITHMETIC_INSTRUCTION(/);
            break;
        default:    
            assert(false && "ERROR: 'handle_arithmetic_inst' inst.type not found\n");
    }
    dm->ip++;
}

void handle_dup_inst(Machine *dm, Inst inst) {
    (void)inst;

    if (isstackfull(dm)) {
        dm->err = Stack_OverFlow_Error;
        return;
    }
    if (isstackempty(dm)) {
        dm->err = Stack_UnderFlow_Error;
        return;
    }
    if ((size_t) inst.operand.as_word > dm->sp) {
        dm->err = Stack_UnderFlow_Error;
        return;
    }

    push(dm, dm->stack[dm->sp - inst.operand.as_word]);
    dm->ip++;
}

void handle_jmp_inst(Machine *dm, Inst inst) {
    size_t address = 0;
    dm_get_address_from_label_name(dm, inst.operand.as_str, &address);
    dm_handle_error(dm);
    dm->ip = address;
}

void handle_halt_inst(Machine *dm, Inst inst) {
    (void)inst;
    dm->state = HALTED;
}


void handle_cmp_inst(Machine *dm, Inst inst) {
    (void)inst;
    if (dm->sp < 2) {
        dm->err = Stack_UnderFlow_Error;
        return;
    }

    Word op1 = pop(dm);
    Word op2 = pop(dm);

    push(dm, op2 - op1);
    dm->ip++;
}

void handle_je_inst(Machine *dm, Inst inst) {
    if (isstackempty(dm)) {
        dm->err = Stack_OverFlow_Error;
        return;
    }

    Word top = pop(dm);
    if (top != 0) {
        dm->ip++;
        return;
    }

    size_t address = 0;
    dm_get_address_from_label_name(dm, inst.operand.as_str, &address);
    dm_handle_error(dm);
    dm->ip = address;
}

void handle_jne_inst(Machine *dm, Inst inst) {
    if (isstackempty(dm)) {
        dm->err = Stack_OverFlow_Error;
        return;
    }

    Word top = pop(dm);
    if (top == 0) {
        dm->ip++;
        return;
    }
    
    size_t address = 0;
    dm_get_address_from_label_name(dm, inst.operand.as_str, &address);
    dm_handle_error(dm);
    dm->ip = address;
}


// WRITE A SINGLE CHAR TO THE STDOUT
void handle_write_syscall(Machine *dm) {
    if (isstackempty(dm)) {
        dm->err = Stack_UnderFlow_Error;
        return;
    }

    char c = (char) pop(dm);
    
    fputc(c, dm->output_file);
}

// GET A SINGLE CHAR FROM THE STDIN
void handle_read_syscall(Machine *dm) {    
    if (isstackfull(dm)) {
        dm->err = Stack_OverFlow_Error;
        return;
    }
    
    char c = getchar();

    push(dm, c);
}

void handle_syscall(Machine *dm, Inst inst) {
    (void)inst;

    if (isstackempty(dm)) {
        dm->err = Stack_UnderFlow_Error;
        return;
    }

    Word call = pop(dm);

    switch(call) {
        case WRITE:
            handle_write_syscall(dm);
            break;
        case READ:
            handle_read_syscall(dm);
            break;
        default:
            assert(false && "ERROR: 'handle_syscall', call not implemented yet\n");
    }

    dm->ip++;
}

void handle_push_str_inst(Machine *dm, Inst inst) {
    char *str = inst.operand.as_str;
    size_t length = strlen(str);

    for (size_t i = 0; i < length; ++i) {
        push(dm, (Word) str[length - 1 - i]);
    }

    dm->ip++;
}

void handle_inc_inst(Machine *dm, Inst inst) {
    if (inst.val_type == REG) {
        dm->regs[inst.operand.as_reg]++;
    }
    
    dm->ip++;
}
void handle_dec_inst(Machine *dm, Inst inst) {
    if (inst.val_type == REG) {
        dm->regs[inst.operand.as_reg]--;
    }

    dm->ip++;
}
void handle_pop_inst(Machine *dm, Inst inst) {
    if (dm->sp < 1) {
        dm->err = Stack_UnderFlow_Error;
        return;
    }

    dm->regs[inst.operand.as_reg] = pop(dm);

    dm->ip++;
}

void handle_jl_inst(Machine *dm, Inst inst){
    if (isstackempty(dm)) {
        dm->err = Stack_OverFlow_Error;
        return;
    }

    Word top = pop(dm);
    if(top >= 0) {
        dm->ip++;
        return;
    }

    size_t address = 0;
    dm_get_address_from_label_name(dm, inst.operand.as_str, &address);
    dm_handle_error(dm);
    dm->ip = address;
}

void handle_jg_inst(Machine *dm, Inst inst){
    if (isstackempty(dm)) {
        dm->err = Stack_OverFlow_Error;
        return;
    }

    Word top = pop(dm);
    if(top <= 0) {
        dm->ip++;
        return;
    }
    
    size_t address = 0;
    dm_get_address_from_label_name(dm, inst.operand.as_str, &address);
    dm_handle_error(dm);
    dm->ip = address;
}

void handle_jle_inst(Machine *dm, Inst inst){
    if (isstackempty(dm)) {
        dm->err = Stack_OverFlow_Error;
        return;
    }

    Word top = pop(dm);
    if(top > 0) {
        dm->ip++;
        return;
    }

    size_t address = 0;
    dm_get_address_from_label_name(dm, inst.operand.as_str, &address);
    dm_handle_error(dm);
    dm->ip = address;
}

void handle_jge_inst(Machine *dm, Inst inst){
    if (isstackempty(dm)) {
        dm->err = Stack_OverFlow_Error;
        return;
    }

    Word top = pop(dm);
    if(top < 0) {
        dm->ip++;
        return;
    }
    
    size_t address = 0;
    dm_get_address_from_label_name(dm, inst.operand.as_str, &address);
    dm_handle_error(dm);
    dm->ip = address;
}

void print_inst(Machine *dm, Inst inst) {
    fprintf(stdout, "%s ", inst_set[inst.type]);
    if (inst_num_operands[inst.type] == 1)
        if (inst.val_type == REG) 
            fprintf(stdout, "%s", regs_str[inst.operand.as_reg]);
        else if (inst.val_type == STR) 
            fprintf(stdout, "%s", inst.operand.as_str);
        else fprintf(stdout, "%zu", inst.operand.as_word);
        
    fprintf(stdout, "\n");
}

void execute_instruction(Machine *dm, Inst inst) {
    print_inst(dm, inst);
    switch(inst.type) {
        case INST_PUSH:
            handle_push_inst(dm, inst);
            break;
        case INST_ADD: 
        case INST_SUB:
        case INST_MUL:
        case INST_DIV:
            handle_arithmetic_inst(dm, inst);
            break;
        case INST_DUP:
            handle_dup_inst(dm, inst);
            break;
        case INST_JMP:
            handle_jmp_inst(dm, inst);
            break;
        case INST_HALT:
            handle_halt_inst(dm, inst);
            break;
        case INST_CMP:
            handle_cmp_inst(dm, inst);
            break;
        case INST_JE:
            handle_je_inst(dm, inst);
            break;
        case INST_JNE:
            handle_jne_inst(dm, inst);
            break;
        case INST_SYSCALL:
            handle_syscall(dm, inst);
            break;
        case INST_PUSH_STR:
            handle_push_str_inst(dm, inst);
            break;
        case INST_INC:
            handle_inc_inst(dm, inst);
            break;
        case INST_DEC:
            handle_dec_inst(dm, inst);
            break;
        case INST_POP:
            handle_pop_inst(dm, inst);
            break;
        case INST_JL:
            handle_jl_inst(dm, inst);
            break;
        case INST_JG:
            handle_jg_inst(dm, inst);
            break;
        case INST_JLE:
            handle_jle_inst(dm, inst);
            break;
        case INST_JGE:
            handle_jge_inst(dm, inst);
            break;
        default:
            assert(false && "Error: 'execute_instruction', instruction not implemented yet\n");
    }
}

void dump_stack(Machine *dm) {
    fprintf(dm->debug_file, "\tStack:\n");
    for (size_t i = 0; i < dm->sp; ++i) {
        fprintf(dm->debug_file, "\t\t%ld\t%c\n", dm->stack[i], (char)dm->stack[i]);
    }
    putchar('\n');
}

void dm_handle_error(Machine *dm) {
    if (dm->err == No_Error) return;
    fprintf(stderr, "\nERROR: %s\n\n", machine_error_to_cstr(dm->err));
    exit(1);
}

void write_directive(Machine *dm, Directive dir) {
    if (dir.type == LABEL_DEFINITION) {
        fprintf(dm->asm_file, "\n%s:\n", dir.value.as_label.name);
    } else {
        assert(false && "ERROR: 'write_directive', dir.type not implemented yet\n");
    }
}

void handle_directive(Machine *dm, Directive dir) {
    if (dir.type == LABEL_DEFINITION) {
        // PUSH THE LABEL TO THE LABELS
        arr_append(&dm->Labels, &dir.value.as_label);
    } else 
        assert(false && "ERROR: 'handle_directive' dir.type not implemented yet\n");
}

void execute_statement(Machine *dm, Prog_Statement statement) {
    switch(statement.type) {
        case INST_TYPE:
            execute_instruction(dm, statement.value.as_inst);
            break;
        case DIRECTIVE_TYPE:
            dm->ip++;
            break;
        default:
            assert(false && "ERROR: 'execute_statement', statement.type not implemented yet\n");
    }
}

void dump_labels(Machine *dm) {
    fprintf(dm->debug_file, "Labels:\n");

    for (size_t i = 0; i < dm->Labels.count; i++) {
        Label label = {0};
        arr_get_item(&dm->Labels, &label, i);
        fprintf(stdout, "\tLabel Name: '%s', Label Address: %zu\n", label.name, label.address);
    }
    putchar('\n');
}


void dm_execute(Machine *dm) {
    size_t count = 0;
    do {    
        execute_statement(dm, dm->prog[dm->ip]);
        if (dm->err != No_Error) {
            dm_handle_error(dm);
            break;
        }
        dump_stack(dm);
        // dump_labels(dm);

        // if (count == 30) break;
        // count++;
    } while (dm->state != HALTED);
    (void)count;
}

void write_instruction(Machine *dm, Inst inst) {
    if (indent != 0)
        putc(indent, dm->asm_file);
    
    fprintf(dm->asm_file, "%s\n", inst_set[inst.type]);

    return;
    
    switch(inst.type) {
        case INST_PUSH:
            fprintf(dm->asm_file, "push %ld\n", inst.operand.as_word);
            break;
        case INST_DUP:
            fprintf(dm->asm_file, "dup %zu\n", (size_t) inst.operand.as_word);
            break; 
        case INST_ADD:
            fprintf(dm->asm_file, "add\n");
            break; 
        case INST_SUB:
            fprintf(dm->asm_file, "sub\n");
            break; 
        case INST_MUL:
            fprintf(dm->asm_file, "mul\n");
            break; 
        case INST_DIV:
            fprintf(dm->asm_file, "div\n");
            break;
        case INST_JMP:
            fprintf(dm->asm_file, "jmp %s\n", inst.operand.as_str);
            break;
        case INST_HALT:
            fprintf(dm->asm_file, "halt\n");
            break;
        case INST_CMP:
            fprintf(dm->asm_file, "cmp\n");
            break;
        case INST_JE:
            fprintf(dm->asm_file, "je %s\n", inst.operand.as_str);
            break;
        case INST_JNE:
            fprintf(dm->asm_file, "jne %s\n", inst.operand.as_str);
            break;
        case INST_SYSCALL:
            fprintf(dm->asm_file, "syscall\n");
            break;
        case INST_PUSH_STR:
            fprintf(dm->asm_file, "push_str '%s'\n", inst.operand.as_str);
            break;
        case INST_INC:
            fprintf(dm->asm_file, "inc\n");
            break;
        case INST_DEC:
            fprintf(dm->asm_file, "dec\n");
            break;
        case INST_POP:
            fprintf(dm->asm_file, "pop %zu\n", (size_t) inst.operand.as_word);
            break;
        case INST_JLE:
            fprintf(dm->asm_file, "jle %s\n", inst.operand.as_str);
            break;
        case INST_JGE:
            fprintf(dm->asm_file, "jge %s\n", inst.operand.as_str);
            break;
        default:
            assert(false && "ERROR: 'write_instruction', inst.type not implemented yet\n");
            break;
    }
}

void dm_push_prog_statement(Machine *dm, Prog_Statement prog_statement) {
    if (dm->prog_size == PROG_CAPACITY) 
        assert(false && "ERROR: 'dm_push_prog_statement' can not push more statements\n");

    if (prog_statement.type == DIRECTIVE_TYPE) {
        indent = '\t';
        handle_directive(dm, prog_statement.value.as_dir);
    }

    dm->prog[dm->prog_size++] = prog_statement;
}

void dm_init(Machine *dm) {
    dm->Labels = arr_init(sizeof(Label));
    dm->err = No_Error;
    dm->sp = 0;
    dm->prog_size = 0;
    dm->ip = 0;
    dm->debug_file = stdout;

    FILE *asm_file = fopen("./disasm.asm", "w");
    FILE *out_file = fopen("./out.txt", "w");

    dm->asm_file = asm_file;
    dm->output_file = out_file;

    for (size_t i = 0; i < REGS_COUNT; ++i) dm->regs[i] = 0;
}

void dm_clean(Machine *dm) {
    free(dm->Labels.items);
    fclose(dm->asm_file);
    fclose(dm->output_file);
}

void write_statement(Machine *dm, size_t index) {
    Prog_Statement statement = dm->prog[index];
    switch(statement.type) {
        case INST_TYPE:
            write_instruction(dm, statement.value.as_inst);
            break;
        case DIRECTIVE_TYPE:
            write_directive(dm, statement.value.as_dir);
            break;
    }
}

void dm_write_prog(Machine *dm) {
    for (size_t i = 0; i < dm->prog_size; ++i) {
        write_statement(dm, i);
    }
}