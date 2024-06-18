#ifndef LEXER_H
#define LEXER_H

#include "./tokenizer.h"

typedef struct {
    size_t row; 
    size_t col;
} Location;

typedef struct {
    View input;
    Location pos;
} Lexer;

typedef struct Token_Node *Tokens;

typedef struct Line_Node {
    Tokens tokens;
    struct Line_Node *next;
} Line_Node;

typedef struct Line_Node *Lines;

Lexer lexer_init(char *input);

char lexer_peek(Lexer *lexer);
void lexer_consume(Lexer *lexer);
int lexer_end(Lexer *lexer);
View lexer_get_line(Lexer *lexer);

Lines lex(Lexer *lexer);

#endif