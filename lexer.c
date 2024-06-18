#include "./dasm/lexer.h"

Lexer lexer_init(char *input) {
    return (Lexer) {
        .input = sv_init(input),
        .pos = (Location) {
            .row = 1,
            .col = 1,
        },
    };
}

char lexer_peek(Lexer *lexer) {
    return (lexer->input.content[lexer->input.current]);
}

void lexer_consume(Lexer *lexer) {
    lexer->input.current++;
}

int lexer_end(Lexer *lexer) {
    return (lexer->input.current == lexer->input.size);
}

View lexer_get_line(Lexer *lexer) {
    View line = (View) {
        .content = lexer->input.content + lexer->input.current,
        .size = 0,
    };

    while (!lexer_end(lexer)) {
        char ch = lexer_peek(lexer);
        lexer_consume(lexer);
        if (ch == NEW_LINE) { lexer->pos.row++; break; }
        line.size++;
    }

    return line;
}

Line_Node *line_node_init() {
    Line_Node *node = (Line_Node *)malloc(sizeof(Line_Node));
    node->tokens = NULL;
    node->next = NULL;
    return node;
}

Line_Node *tokenize_line(View *line) {
    Line_Node *node = line_node_init();
    Token_Node *current = NULL;

    while (line->size != line->current) {
        sv_skip_spaces(line);

        if (line->size == line->current) break;
        
        Token_Node *token = read_next(line);

        char *value = sv_to_cstr(&token->token.value);

        printf("value: %s, type: %d\n", value, token->token.type);

        free(value);

        if (current == NULL) {
            node->tokens = token;
            current = token;
        } else {
            current->next = token;
            current = token;
        }
    }

    return node;
}


// RETURNS A LINKED LIST OF ROWS WHERE EACH ROW CONTAINS A TOKENIZED ASSEMBLY INSTRUCTION
Lines lex(Lexer *lexer) {
    Lines lines = NULL;
    Line_Node *current = NULL;

    while (!lexer_end(lexer)) {
        if (lexer_peek(lexer) == NEW_LINE) {
            lexer->pos.row++;
            lexer_consume(lexer);
        } else {
            View line = lexer_get_line(lexer);
            Line_Node *tokens = tokenize_line(&line);

            if (current == NULL) {
                current = tokens;
                lines = tokens;
            } else {
                current->next = tokens;
                current = tokens;
            }

        }
    }

    return lines;
}
