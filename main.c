#include "./dm.h"

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

const char *inst_type_to_cstr(Inst_Type type) {
    switch(type) {
        case INST_PUSH:
            return "INST_PUSH";
        case INST_ADD:
            return "INST_ADD";
        case INST_SUB:
            return "INST_SUB";
        case INST_MUL:
            return "INST_MUL";
        case INST_DIV:
            return "INST_DIV";
        case INST_DUP:
            return "INST_DUP";
        case INST_JMP:
            return "INST_JMP";
        case INST_HALT:
            return "INST_HALT";
        case INST_CMP:
            return "INST_CMP";
        case INST_JE:
            return "INST_JE";
        case INST_JNE:
            return "INST_JNE";
        case INST_SYSCALL:  
            return "INST_SYSCALL";
        case INST_PUSH_STR:
            return "INST_PUSH_STR";
        case INST_DEC:
            return "INST_DEC";
        case INST_INC:
            return "INST_INC";
        case INST_POP:
            return "INST_POP";
        default:
            assert(false && "Error: 'inst_type_to_cstr', type not implemented yet\n");
    }
}

const char *machine_error_to_cstr(Machine_Error err) {
    switch(err) {       
        case No_Error:
            return "No_Error";
        case Stack_OverFlow_Error:
            return "Stack_OverFlow_Error";
        case Stack_UnderFlow_Error:
            return "Stack_UnderFlow_Error";
        case Division_By_Zero_Error:
            return "Division_By_Zero_Error";
        case Label_Not_Found_Error:
            return "Label_Not_Found_Error";
        case Syscall_Error:
            return "Syscall_Error";
        case Invalid_Memory_Location_Error:
            return "Invalid_Memory_Location_Error";
        default:
            assert(false && "Error: 'machine_error_to_cstr', error not implemented yet\n");
    }
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
    push(dm, inst.operand.as_word);
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

    push(dm, op1 == op2);
    dm->ip++;
}

void handle_je_inst(Machine *dm, Inst inst) {
    if (isstackempty(dm)) {
        dm->err = Stack_OverFlow_Error;
        return;
    }

    Word top = pop(dm);
    if(top == 0) {
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
    if(top == 1) {
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

    char c = fgetc(stdout);

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
    (void)inst;
    if (isstackempty(dm)) {
        dm->err = Stack_UnderFlow_Error;
        return;
    }

    Word op = pop(dm);
    op++;
    push(dm, op);

    dm->ip++;
}
void handle_dec_inst(Machine *dm, Inst inst) {
    (void)inst;
    if (isstackempty(dm)) {
        dm->err = Stack_UnderFlow_Error;
        return;
    }

    Word op = pop(dm);
    op--;
    push(dm, op);

    dm->ip++;
}
void handle_pop_inst(Machine *dm, Inst inst) {
    size_t op = (size_t) inst.operand.as_word; 
    if (op > dm->sp) {
        dm->err = Invalid_Memory_Location_Error;
        return;
    }

    for (size_t i = dm->sp - op; i < dm->sp - 1; ++i) {
        dm->stack[i] = dm->stack[i + 1];
    }    

    dm->sp--;
    dm->ip++;
}

void print_inst(Machine *dm, Inst inst) {
    fprintf(dm->debug_file, "%s\n", inst_type_to_cstr(inst.type));
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


void execute(Machine *dm) {
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

Inst inst_word_init(Inst_Type type, Word operand) {
    return (Inst) {
        .type = type,
        .operand.as_word = operand,
    };
}

Inst inst_str_init(Inst_Type type, char *name) {
    return (Inst) {
        .type = type,
        .operand.as_str = name,
    };
}

Label label_init(char *name, size_t addr) {
    return (Label) {
        .name = name,
        .address = addr,
    };
}

Directive dir_init(Directive_Type type, Directive_Value value) {
    return (Directive) {
        .type = type,
        .value = value,
    };
}

Directive dir_label_init(Label label) {
    return (Directive) {
        .type = LABEL_DEFINITION,
        .value.as_label = label,
    };
}

Prog_Statement prog_statement_init(Statement_Type type, Statement_Value value) {
    return (Prog_Statement) {
        .type = type,
        .value = value,
    };
}

Machine dm;

void write_instruction(Machine *dm, Inst inst) {
    if (indent != 0)
        putc(indent, dm->asm_file);

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

#define MAKE_INST_PUSH(value)           inst_word_init(INST_PUSH, value)
#define MAKE_INST_ADD                   inst_word_init(INST_ADD, 0)
#define MAKE_INST_SUB                   inst_word_init(INST_SUB, 0)
#define MAKE_INST_MUL                   inst_word_init(INST_MUL, 0)
#define MAKE_INST_DIV                   inst_word_init(INST_DIV, 0)
#define MAKE_INST_DUP(offset)           inst_word_init(INST_DUP, offset)
#define MAKE_INST_HALT                  inst_word_init(INST_HALT, 0)
#define MAKE_INST_CMP                   inst_word_init(INST_CMP, 0)
#define MAKE_INST_SYSCALL               inst_word_init(INST_SYSCALL, 0)
#define MAKE_INST_DEC                   inst_word_init(INST_DEC, 0)
#define MAKE_INST_INC                   inst_word_init(INST_INC, 0)
#define MAKE_INST_POP(offset)           inst_word_init(INST_POP, offset)

#define MAKE_INST_JMP(name)             inst_str_init(INST_JMP, name)
#define MAKE_INST_JE(name)              inst_str_init(INST_JE, name)
#define MAKE_INST_JNE(name)             inst_str_init(INST_JNE, name)
#define MAKE_INST_PUSH_STR(str)         inst_str_init(INST_PUSH_STR, str)

#define MAKE_LABEL(name)                dir_label_init(label_init(name, dm->prog_size))


#define PUSH_INST(inst)                                                 \
    do {                                                                \
        Statement_Value value;                                          \
        value.as_inst = inst;                                           \
        Statement_Type type = INST_TYPE;                                \
        Prog_Statement statement = prog_statement_init(type, value);    \
        dm_push_prog_statement(dm, statement);                          \
    } while(0)

#define PUSH_DIR(label)                                                 \
    do {                                                                \
        Statement_Value value;                                          \
        value.as_dir = label;                                           \
        Statement_Type type = DIRECTIVE_TYPE;                           \
        Prog_Statement statement = prog_statement_init(type, value);    \
        dm_push_prog_statement(dm, statement);                          \
    } while(0)

void fibo_prog(Machine *dm) {
    PUSH_INST(MAKE_INST_JMP("main"));

    PUSH_DIR(MAKE_LABEL("loop"));
    PUSH_INST(MAKE_INST_DUP(1));
    PUSH_INST(MAKE_INST_DUP(3));
    PUSH_INST(MAKE_INST_ADD);
    PUSH_INST(MAKE_INST_DUP(1));
    PUSH_INST(MAKE_INST_PUSH(8));
    PUSH_INST(MAKE_INST_CMP);
    PUSH_INST(MAKE_INST_JE("break"));
    PUSH_INST(MAKE_INST_JMP("loop"));

    PUSH_DIR(MAKE_LABEL("main"));
    PUSH_INST(MAKE_INST_PUSH(1));
    PUSH_INST(MAKE_INST_PUSH(1));
    PUSH_INST(MAKE_INST_JMP("loop"));

    PUSH_DIR(MAKE_LABEL("break"));
    PUSH_INST(MAKE_INST_HALT);
}

void write_string_prog(Machine *dm) {
    PUSH_INST(MAKE_INST_JMP("main"));

    PUSH_DIR(MAKE_LABEL("done"));
    PUSH_INST(MAKE_INST_HALT);

    PUSH_DIR(MAKE_LABEL("print"));
    PUSH_INST(MAKE_INST_PUSH(0));
    PUSH_INST(MAKE_INST_DUP(2));
    PUSH_INST(MAKE_INST_CMP);
    PUSH_INST(MAKE_INST_JE("done"));
    PUSH_INST(MAKE_INST_DUP(2));
    PUSH_INST(MAKE_INST_PUSH(WRITE));
    PUSH_INST(MAKE_INST_SYSCALL);
    PUSH_INST(MAKE_INST_DEC);
    PUSH_INST(MAKE_INST_POP(2));
    // PUSH_INST(MAKE_INST_HALT);
    PUSH_INST(MAKE_INST_JMP("print"));

    PUSH_DIR(MAKE_LABEL("main"));
    PUSH_INST(MAKE_INST_PUSH_STR("ABCD"));
    PUSH_INST(MAKE_INST_PUSH(4));          // push the string size
    PUSH_INST(MAKE_INST_JMP("print"));
}

void read_string_prog(Machine *dm) {
    PUSH_INST(MAKE_INST_JMP("main"));

    PUSH_DIR(MAKE_LABEL("main"));
    PUSH_INST(MAKE_INST_PUSH(4));      // pushing the string size
    PUSH_INST(MAKE_INST_PUSH(READ));   // pushing the syscall type
    PUSH_INST(MAKE_INST_SYSCALL);
    PUSH_INST(MAKE_INST_HALT);
}

void prog_init(Machine *dm) {
    write_string_prog(dm);
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
    // prog_init(dm);
}

void dm_clean(Machine *dm) {
    free(dm->Labels.items);
    fclose(dm->asm_file);
    fclose(dm->output_file);
}

// return the token value if the current node has the type 'type' else raise error
Token expect_token(Token_Node *current, Token_Type type) {
    if (current == NULL) goto error;
    if (current->token.type != type) goto error;
    return current->token;

error:
    assert(false && "ERROR: 'expect_token' failed, expected token of type TYPE_GIVEN, but not found\n");
}

Inst_Type cstr_to_inst_type(char *cstr) {
    for (size_t i = 0; i < ARR_SIZE(inst_set); ++i) {
        if (isequal(cstr, inst_set[i])) {
            return i;
        }
    }

    assert("ERROR: 'cstr_to_inst_type' failed to get the instruction type\n");
    return 0;
}

void dm_parse_label_def(Machine *dm, Tokens tokens) {
    char *value;
    Token_Node *current = tokens;

    Statement_Type st_type;
    Statement_Value st_value;

    if (current->token.type == TT_LABEL) {
        value = sv_to_cstr(&current->token.value);

        st_type = DIRECTIVE_TYPE;
        st_value.as_dir = MAKE_LABEL(value);
        
        current = current->next;

        if (current != NULL) {
            assert("ERROR: Expected no tokens but found more\n");
        }
    } else {
        assert("Expected Label definition but else found\n");
    }

    dm_push_prog_statement(dm, prog_statement_init(st_type, st_value));
}

void dm_parse_instruction(Machine *dm, Tokens tokens) {
    char *value;
    Token_Node *current = tokens;

    Statement_Type st_type;
    Statement_Value st_value;

    if(current->token.type == TT_INST) {
        Inst inst;
        
        value = sv_to_cstr(&current->token.value);
        
        Inst_Type type = cstr_to_inst_type(value);
        inst.type = type;

        current = current->next;
        
        if (inst_num_operands[type] == 1) {
            Token token = expect_token(current, TT_OPERAND);
            value = sv_to_cstr(&token.value);

            if (inst_op_type[type] == STRING_TYPE) {
                inst.operand.as_str = value;
            } else {
                inst.operand.as_word = atoll(value);
            }
        }

        st_type = INST_TYPE;
        st_value.as_inst = inst;
    } else {
        assert("Expected an instruction but else found\n");
    }
    
    dm_push_prog_statement(dm, prog_statement_init(st_type, st_value));
}

void dm_assemble_line(Machine *dm, Tokens tokens) {
    if (tokens == NULL) return;
    if (tokens->token.type == TT_LABEL) dm_parse_label_def(dm, tokens);
    else if (tokens->token.type == TT_INST) dm_parse_instruction(dm, tokens);
    else 
        assert("Expected an instruction or a label definition but else found\n");
}

void free_tokens(Tokens tokens) {
    Token_Node *current = tokens;
    while (current) {
        Token_Node *next = current->next;
        free(current);
        current = next;
    }
}

void free_lines(Lines lines) {
    Line_Node *current = lines;
    while (current) {
        Line_Node *next = current->next;
        free_tokens(current->tokens);
        free(current);
        current = next;
    }
}

void dm_assemble(Machine *dm, const char *filename) {
    char *file_content = fcontent(filename);

    Lexer lexer = lexer_init(file_content);
    Lines lines = lex(&lexer);
    Line_Node *line = lines;

    while (line) {
        dm_assemble_line(dm, line->tokens);
        line = line->next;
    }

    free_lines(lines);
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

int main(void) {
    const char *filename = "./main.asm";
    dm_init(&dm);
    dm_assemble(&dm, filename);
    // execute(&dm);
    dm_write_prog(&dm);
    dm_clean(&dm);
    return 0;
}