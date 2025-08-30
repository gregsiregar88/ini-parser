
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

static char *trim(char *str) {
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;

    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end+1) = '\0';
    return str;
}

Token lexLine(char *line) {
    Token tok;
    memset(&tok, 0, sizeof(tok));

    line[strcspn(line, "\r\n")] = 0; // remove newline
    char *trimmed = trim(line);

    if (strlen(trimmed) == 0) {
        tok.type = TOKEN_EMPTY;
        return tok;
    }
    if (trimmed[0] == ';' || trimmed[0] == '#') {
        tok.type = TOKEN_COMMENT;
        return tok;
    }
    if (trimmed[0] == '[') {
        char *end = strchr(trimmed, ']');
        if (end && end[1] == '\0') {
            tok.type = TOKEN_SECTION;
            strncpy(tok.key, trimmed + 1, end - trimmed - 1);
            tok.key[end - trimmed - 1] = '\0';
            return tok;
        }
        tok.type = TOKEN_INVALID;
        return tok;
    }

    char *eq = strchr(trimmed, '=');
    if (!eq) eq = strchr(trimmed, ':');
    if (eq) {
        *eq = '\0';
        char *key = trim(trimmed);
        char *val = trim(eq + 1);

        if (strlen(key) > 0 && strlen(val) > 0) {
            tok.type = TOKEN_KEYVALUE;
            strncpy(tok.key, key, sizeof(tok.key) - 1);
            strncpy(tok.value, val, sizeof(tok.value) - 1);
            return tok;
        }
    }

    tok.type = TOKEN_INVALID;
    return tok;
}
