#include "./dasm/tokenizer.h"

Token token_init(Token_Type type, Token_Value value) {
    return (Token) {
        .type = type,
        .value = value,
    };
}

Token_Node *token_node_init(Token_Type type, Token_Value value) {
    Token_Node *node = (Token_Node *)malloc(sizeof(Token_Node));
    node->token = token_init(type, value);
    node->next = NULL;
    return node; 
}



int is_inst(char *str) {
    for (size_t i = 0; i < ARR_SIZE(inst_set); ++i) {
        if (isequal(str, inst_set[i])) return 1;
    }
    return 0;
}

int is_reg(char *str) {
    for (size_t i = 0; i < ARR_SIZE(regs_str); ++i) {
        if (isequal(str, regs_str[i])) return 1;
    }
    return 0;
}

Token_Type get_token_type(char *str) {
    if (is_inst(str)) return TT_INST;
    if (is_reg(str)) return TT_REG;
    return TT_OPERAND;
}

Token_Node *read_next(View *line) {
    View view = sv_init(line->content + line->current);
    view.size = 0;

    Token_Type type = TT_NONE;
    Token_Value value;
    
    char ch =  line->content[line->current];

    if (ch == QUOTES) {
        type = TT_OPERAND;
    }

    view.size++;
    line->current++;

    if (type == TT_OPERAND) {
        // read a string "demo string"
        while (line->current != line->size) {
            ch = line->content[line->current++];
            view.size++;
            if (ch == QUOTES) break;
        }
    }

    if (type == TT_NONE) {
        while (line->current != line->size) {
            ch = line->content[line->current++];

            if (ch == WHITE_SPACE || ch == NEW_LINE || ch == COMMA) { 
                break; 
            }

            if (ch == COLON) { type = TT_LABEL; break; }
            view.size++;
        }
    }

    if (type == TT_NONE) {
        char *str = sv_to_cstr(&view);
        type = get_token_type(str);
        free(str);
    }

    value = view;
    Token_Node *node = token_node_init(type, value);
    return node;
}
