
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

typedef struct KeyValue {
    char key[128];
    char value[128];
    struct KeyValue *next;
} KeyValue;

typedef struct Section {
    char name[128];
    KeyValue *pairs;
    struct Section *next;
} Section;

static Section *sections = NULL;   // head of section list
static Section *last_section = NULL; // tail pointer
static int valid = 1;

// Append section at end
static Section* addSection(const char *name) {
    Section *s = malloc(sizeof(Section));
    strncpy(s->name, name, sizeof(s->name) - 1);
    s->name[sizeof(s->name)-1] = '\0';
    s->pairs = NULL;
    s->next = NULL;

    if (!sections) {
        sections = s;
    } else {
        last_section->next = s;
    }
    last_section = s;
    return s;
}

// Append key-value at end of section
static void addKeyValue(Section *s, const char *key, const char *value) {
    KeyValue *kv = malloc(sizeof(KeyValue));
    strncpy(kv->key, key, sizeof(kv->key) - 1);
    kv->key[sizeof(kv->key)-1] = '\0';
    strncpy(kv->value, value, sizeof(kv->value) - 1);
    kv->value[sizeof(kv->value)-1] = '\0';
    kv->next = NULL;

    if (!s->pairs) {
        s->pairs = kv;
    } else {
        KeyValue *cur = s->pairs;
        while (cur->next) cur = cur->next;
        cur->next = kv;
    }
}

int parseFile(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("fopen");
        return 0;
    }

    char line[256];
    Section *current = NULL;

    while (fgets(line, sizeof(line), fp)) {
        Token tok = lexLine(line);
        switch (tok.type) {
            case TOKEN_SECTION:
                current = addSection(tok.key);
                break;
            case TOKEN_KEYVALUE:
                if (current) {
                    addKeyValue(current, tok.key, tok.value);
                } else {
                    fprintf(stderr, "Error: key-value outside section\n");
                    valid = 0;
                }
                break;
            case TOKEN_COMMENT:
            case TOKEN_EMPTY:
                break;
            case TOKEN_INVALID:
            default:
                fprintf(stderr, "Invalid line: %s\n", line);
                valid = 0;
                break;
        }
    }

    fclose(fp);
    return valid;
}

void printINI() {
    for (Section *s = sections; s; s = s->next) {
        for (KeyValue *kv = s->pairs; kv; kv = kv->next) {
            printf("%s.%s = %s\n", s->name, kv->key, kv->value);
        }
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <ini_file>\n", argv[0]);
        return 1;
    }

    if (parseFile(argv[1])) {
        printf("✅ INI file is valid\n");
        printINI();
    } else {
        printf("❌ INI file is invalid\n");
    }

    return 0;
}
