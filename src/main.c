// src/main.c
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "semantics.h"
#include "codegen.h"

int yyparse(void);
extern FILE *yyin;
extern Program *gProgram;

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <input.vl>\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        perror("fopen");
        return 1;
    }

    int rc = yyparse();
    if (rc == 0) {
        ast_print(gProgram);
        if (semantic_check(gProgram) == 0) {
            codegen_program(gProgram, argv[1]);
        } else {
            rc = 1;
        }
        ast_free(gProgram);
    } else {
        fprintf(stderr, "Parse failed\n");
    }

    fclose(yyin);
    return rc;
}
