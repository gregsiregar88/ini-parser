
#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOKEN_SECTION,
    TOKEN_KEYVALUE,
    TOKEN_COMMENT,
    TOKEN_EMPTY,
    TOKEN_INVALID
} TokenType;

typedef struct {
    TokenType type;
    char key[256];
    char value[256];
} Token;

Token lexLine(char *line);

#endif
