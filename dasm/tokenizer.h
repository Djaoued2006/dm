#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "../sv.h"
#include "../utils.h"

typedef View Token_Value;

typedef enum {
    TT_NONE = 0,
    TT_INST,
    TT_LABEL,
    TT_OPERAND, 
} Token_Type;

typedef struct {
    Token_Type type;
    Token_Value value;
} Token;

typedef struct Token_Node {
    Token token;
    struct Token_Node *next;
} Token_Node;

Token_Node *read_next(View *line);

#endif