#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

static char *strdup_safe(const char *s){
    if(!s) return NULL;
    char *d=malloc(strlen(s)+1);
    strcpy(d,s);
    return d;
}

Program *ast_program(Block *block){
    Program *p = malloc(sizeof(Program));
    p->block = block;
    return p;
}

Block *ast_block(Stmt **stmts, size_t count){
    Block *b = malloc(sizeof(Block));
    b->stmts = stmts;
    b->stmt_count = count;
    return b;
}

Stmt *ast_stmt_decl(int line, int is_vec, const char *name, int size){
    Stmt *s = malloc(sizeof(Stmt));
    s->kind = STMT_DECL;
    s->line = line;
    s->u.decl.name = strdup_safe(name);
    s->u.decl.is_vec = is_vec;
    s->u.decl.size = size;
    return s;
}

Stmt *ast_stmt_assign(int line, const char *name, Exp *value){
    Stmt *s = malloc(sizeof(Stmt));
    s->kind = STMT_ASSIGN;
    s->line = line;
    s->u.assign.name = strdup_safe(name);
    s->u.assign.value = value;
    return s;
}

Stmt *ast_stmt_print(int line, const char *str, Exp **args, size_t count){
    Stmt *s = malloc(sizeof(Stmt));
    s->kind = STMT_PRINT;
    s->line = line;
    s->u.print.str = strdup_safe(str);
    s->u.print.args = args;
    s->u.print.arg_count = count;
    return s;
}

Stmt *ast_stmt_if(int line, Exp *cond, Block *then_block){
    Stmt *s = malloc(sizeof(Stmt));
    s->kind = STMT_IF;
    s->line = line;
    s->u.if_stmt.cond = cond;
    s->u.if_stmt.then_block = then_block;
    return s;
}

Stmt *ast_stmt_loop(int line, Exp *cond, Block *body){
    Stmt *s = malloc(sizeof(Stmt));
    s->kind = STMT_LOOP;
    s->line = line;
    s->u.loop.cond = cond;
    s->u.loop.body = body;
    return s;
}

Exp *ast_exp_ident(int line, const char *name){
    Exp *e = malloc(sizeof(Exp));
    e->kind = EXP_IDENT;
    e->line = line;
    e->u.ident.name = strdup_safe(name);
    e->type.kind = TYPE_SCL; // default; real type set in semantics
    e->type.size = 0;
    return e;
}

Exp *ast_exp_int(int line, int value){
    Exp *e = malloc(sizeof(Exp));
    e->kind = EXP_INT_LIT;
    e->line = line;
    e->u.int_lit.value = value;
    e->type.kind = TYPE_SCL;
    e->type.size = 0;
    return e;
}

Exp *ast_exp_vec_lit(int line, int *values, size_t count){
    Exp *e = malloc(sizeof(Exp));
    e->kind = EXP_VEC_LIT;
    e->line = line;
    e->u.vec_lit.values = values;
    e->u.vec_lit.count = count;
    e->type.kind = TYPE_VEC;
    e->type.size = (int)count;
    return e;
}

Exp *ast_exp_binop(int line, char op, Exp *left, Exp *right){
    Exp *e = malloc(sizeof(Exp));
    e->kind = EXP_BINOP;
    e->line = line;
    e->u.binop.op = op;
    e->u.binop.left = left;
    e->u.binop.right = right;
    e->type.kind = TYPE_SCL;
    e->type.size = 0;
    return e;
}

Exp *ast_exp_dot(int line, Exp *left, Exp *right){
    Exp *e = malloc(sizeof(Exp));
    e->kind = EXP_DOT;
    e->line = line;
    e->u.dot.left = left;
    e->u.dot.right = right;
    e->type.kind = TYPE_SCL;
    return e;
}

Exp *ast_exp_index(int line, Exp *left, Exp *right){
    Exp *e = malloc(sizeof(Exp));
    e->kind = EXP_INDEX;
    e->line = line;
    e->u.index.left = left;
    e->u.index.right = right;
    e->type.kind = TYPE_SCL;
    return e;
}

static void indent(int n){ for(int i=0;i<n;i++) putchar(' '); }

static void print_exp(Exp *e, int ind);
static void print_stmt(Stmt *s, int ind){
    indent(ind);
    switch(s->kind){
    case STMT_DECL:
        printf("Decl %s %s", s->u.decl.is_vec?"vec":"scl", s->u.decl.name);
        if(s->u.decl.is_vec) printf("{%d}", s->u.decl.size);
        printf("\n");
        break;
    case STMT_ASSIGN:
        printf("Assign %s =\n", s->u.assign.name);
        print_exp(s->u.assign.value, ind+2);
        break;
    case STMT_PRINT:
        printf("Print \"%s\"\n", s->u.print.str);
        for(size_t i=0;i<s->u.print.arg_count;i++){
            print_exp(s->u.print.args[i], ind+2);
        }
        break;
    case STMT_IF:
        printf("If\n");
        indent(ind+2); printf("Cond:\n");
        print_exp(s->u.if_stmt.cond, ind+4);
        indent(ind+2); printf("Then:\n");
        for(size_t i=0;i<s->u.if_stmt.then_block->stmt_count;i++)
            print_stmt(s->u.if_stmt.then_block->stmts[i], ind+4);
        break;
    case STMT_LOOP:
        printf("Loop\n");
        indent(ind+2); printf("Cond:\n");
        print_exp(s->u.loop.cond, ind+4);
        indent(ind+2); printf("Body:\n");
        for(size_t i=0;i<s->u.loop.body->stmt_count;i++)
            print_stmt(s->u.loop.body->stmts[i], ind+4);
        break;
    }
}

static void print_exp(Exp *e, int ind){
    indent(ind);
    switch(e->kind){
    case EXP_IDENT:
        printf("Ident %s\n", e->u.ident.name);
        break;
    case EXP_INT_LIT:
        printf("Int %d\n", e->u.int_lit.value);
        break;
    case EXP_VEC_LIT:
        printf("VecLit");
        for(size_t i=0;i<e->u.vec_lit.count;i++)
            printf(" %d", e->u.vec_lit.values[i]);
        printf("\n");
        break;
    case EXP_BINOP:
        printf("BinOp %c\n", e->u.binop.op);
        print_exp(e->u.binop.left, ind+2);
        print_exp(e->u.binop.right, ind+2);
        break;
    case EXP_DOT:
        printf("Dot\n");
        print_exp(e->u.dot.left, ind+2);
        print_exp(e->u.dot.right, ind+2);
        break;
    case EXP_INDEX:
        printf("Index\n");
        print_exp(e->u.index.left, ind+2);
        print_exp(e->u.index.right, ind+2);
        break;
    }
}

void ast_print(Program *prog){
    printf("Program\n");
    for(size_t i=0;i<prog->block->stmt_count;i++)
        print_stmt(prog->block->stmts[i],2);
}

static void free_exp(Exp *e);
static void free_stmt(Stmt *s){
    if(!s) return;
    switch(s->kind){
    case STMT_DECL:
        free(s->u.decl.name);
        break;
    case STMT_ASSIGN:
        free(s->u.assign.name);
        free_exp(s->u.assign.value);
        break;
    case STMT_PRINT:
        free(s->u.print.str);
        for(size_t i=0;i<s->u.print.arg_count;i++)
            free_exp(s->u.print.args[i]);
        free(s->u.print.args);
        break;
    case STMT_IF:
        free_exp(s->u.if_stmt.cond);
        for(size_t i=0;i<s->u.if_stmt.then_block->stmt_count;i++)
            free_stmt(s->u.if_stmt.then_block->stmts[i]);
        free(s->u.if_stmt.then_block->stmts);
        free(s->u.if_stmt.then_block);
        break;
    case STMT_LOOP:
        free_exp(s->u.loop.cond);
        for(size_t i=0;i<s->u.loop.body->stmt_count;i++)
            free_stmt(s->u.loop.body->stmts[i]);
        free(s->u.loop.body->stmts);
        free(s->u.loop.body);
        break;
    }
    free(s);
}

static void free_exp(Exp *e){
    if(!e) return;
    switch(e->kind){
    case EXP_IDENT:
        free(e->u.ident.name);
        break;
    case EXP_INT_LIT:
        break;
    case EXP_VEC_LIT:
        free(e->u.vec_lit.values);
        break;
    case EXP_BINOP:
        free_exp(e->u.binop.left);
        free_exp(e->u.binop.right);
        break;
    case EXP_DOT:
        free_exp(e->u.dot.left);
        free_exp(e->u.dot.right);
        break;
    case EXP_INDEX:
        free_exp(e->u.index.left);
        free_exp(e->u.index.right);
        break;
    }
    free(e);
}

void ast_free(Program *prog){
    if(!prog) return;
    for(size_t i=0;i<prog->block->stmt_count;i++)
        free_stmt(prog->block->stmts[i]);
    free(prog->block->stmts);
    free(prog->block);
    free(prog);
}
