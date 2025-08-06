#ifndef AST_H
#define AST_H

#include <stddef.h>
#include "types.h"

// Forward declarations
struct Stmt;
struct Exp;
struct Block;

typedef struct {
    struct Block *block;
} Program;

typedef struct Block {
    struct Stmt **stmts;
    size_t stmt_count;
} Block;

typedef enum {
    STMT_DECL,
    STMT_ASSIGN,
    STMT_PRINT,
    STMT_IF,
    STMT_LOOP
} StmtKind;

typedef struct Stmt {
    StmtKind kind;
    int line;
    union {
        struct { char *name; int is_vec; int size; } decl;
        struct { char *name; struct Exp *value; } assign;
        struct { char *str; struct Exp **args; size_t arg_count; } print;
        struct { struct Exp *cond; struct Block *then_block; } if_stmt;
        struct { struct Exp *cond; struct Block *body; } loop;
    } u;
} Stmt;

typedef enum {
    EXP_IDENT,
    EXP_INT_LIT,
    EXP_VEC_LIT,
    EXP_BINOP,
    EXP_DOT,
    EXP_INDEX
} ExpKind;

typedef struct Exp {
    ExpKind kind;
    int line;
    Type type; // filled by semantics
    union {
        struct { char *name; } ident;
        struct { int value; } int_lit;
        struct { int *values; size_t count; } vec_lit;
        struct { char op; struct Exp *left; struct Exp *right; } binop;
        struct { struct Exp *left; struct Exp *right; } dot;
        struct { struct Exp *left; struct Exp *right; } index;
    } u;
} Exp;

Program *ast_program(Block *block);
Block *ast_block(struct Stmt **stmts, size_t count);
Stmt *ast_stmt_decl(int line, int is_vec, const char *name, int size);
Stmt *ast_stmt_assign(int line, const char *name, Exp *value);
Stmt *ast_stmt_print(int line, const char *str, Exp **args, size_t count);
Stmt *ast_stmt_if(int line, Exp *cond, Block *then_block);
Stmt *ast_stmt_loop(int line, Exp *cond, Block *body);

Exp *ast_exp_ident(int line, const char *name);
Exp *ast_exp_int(int line, int value);
Exp *ast_exp_vec_lit(int line, int *values, size_t count);
Exp *ast_exp_binop(int line, char op, Exp *left, Exp *right);
Exp *ast_exp_dot(int line, Exp *left, Exp *right);
Exp *ast_exp_index(int line, Exp *left, Exp *right);

void ast_print(Program *prog);
void ast_free(Program *prog);

#endif // AST_H
