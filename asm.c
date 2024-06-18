#include "./dasm/asm.h"

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

// return the token value if the current node has the type 'type' else raise error
Token expect_token(Token_Node *current, Token_Type type) {
    if (current == NULL) goto error;
    if (current->token.type != type) goto error;
    return current->token;

error:
    assert(false && "ERROR: 'expect_token' failed, expected token of type TYPE_GIVEN, but not found\n");
}


Register cstr_to_reg_type(char *cstr) {
    for (size_t i = 0; i < ARR_SIZE(regs_str); ++i) {
        if (isequal(cstr, regs_str[i])) {
            return i;
        }
    }

    assert("ERROR: 'cstr_to_reg_type' failed to get the regsiter type\n");
    return 0;
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
            Token token = current->token;
            value = sv_to_cstr(&token.value);

            if (inst_op_type[type] == STRING_TYPE) {
                inst.val_type = STR;
                inst.operand.as_str = value;
            } else if (inst_op_type[type] == WORD_REG_TYPE) {
                if (token.type == TT_REG) {
                    inst.val_type = REG;
                    inst.operand.as_reg = cstr_to_reg_type(value);
                } else if (token.type == TT_OPERAND) {
                    inst.val_type = WORD;
                    inst.operand.as_word = atoll(value);
                } else {
                    assert("ERROR: 'dm_parse_instruction' failed to parse operand, expected reg or word type but else found\n");
                }
            } else if (inst_op_type[type] == REG_TYPE) {
                inst.val_type = REG;
                inst.operand.as_reg = cstr_to_reg_type(value);
            } else if (inst_op_type[type] == WORD_TYPE) {
                inst.val_type = WORD;
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
