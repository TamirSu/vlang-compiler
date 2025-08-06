// src/main.c
#include <stdio.h>
#include <stdlib.h>

int yyparse(void);
extern FILE *yyin;

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
        printf("Parse OK\n");
    } else {
        fprintf(stderr, "Parse failed\n");
    }

    fclose(yyin);
    return rc;
}
