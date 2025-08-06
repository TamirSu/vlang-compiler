#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "symtab.h"
#include "types.h"

static int check_block(Block *b, SymTab *st);
static Type check_exp(Exp *e, SymTab *st, int *err);

static int check_id(const char *name){
    return strlen(name) <= 32;
}

int semantic_check(Program *prog){
    SymTab *st = symtab_create();
    symtab_push(st);
    int rc = check_block(prog->block, st);
    symtab_free(st);
    return rc;
}

static int check_block(Block *b, SymTab *st){
    symtab_push(st);
    int rc = 0;
    for(size_t i=0;i<b->stmt_count && rc==0;i++){
        Stmt *s = b->stmts[i];
        switch(s->kind){
        case STMT_DECL: {
            if(!check_id(s->u.decl.name)){
                fprintf(stderr,"line %d: identifier too long\n", s->line);
                rc=1; break;
            }
            Type t = s->u.decl.is_vec ? (Type){TYPE_VEC,s->u.decl.size} : (Type){TYPE_SCL,0};
            if(symtab_define(st, s->u.decl.name, t)!=0){
                fprintf(stderr,"line %d: duplicate identifier %s\n", s->line,s->u.decl.name);
                rc=1;
            }
            break; }
        case STMT_ASSIGN: {
            Type var;
            if(symtab_lookup(st, s->u.assign.name, &var)!=0){
                fprintf(stderr,"line %d: undefined identifier %s\n", s->line,s->u.assign.name);
                rc=1; break;
            }
            int err=0;
            Type val = check_exp(s->u.assign.value, st, &err);
            if(err){ rc=1; break; }
            if(var.kind!=val.kind || (var.kind==TYPE_VEC && var.size!=val.size)){
                fprintf(stderr,"line %d: type mismatch in assignment to %s\n", s->line,s->u.assign.name);
                rc=1;
            }
            break; }
        case STMT_PRINT: {
            for(size_t a=0;a<s->u.print.arg_count && rc==0;a++){
                int err=0; check_exp(s->u.print.args[a], st, &err); if(err) rc=1; }
            break; }
        case STMT_IF: {
            int err=0; Type t = check_exp(s->u.if_stmt.cond, st, &err);
            if(err){ rc=1; break; }
            if(t.kind!=TYPE_SCL){
                fprintf(stderr,"line %d: if condition must be scalar\n", s->line); rc=1; break; }
            rc = check_block(s->u.if_stmt.then_block, st); break; }
        case STMT_LOOP: {
            int err=0; Type t = check_exp(s->u.loop.cond, st, &err);
            if(err){ rc=1; break; }
            if(t.kind!=TYPE_SCL){
                fprintf(stderr,"line %d: loop condition must be scalar\n", s->line); rc=1; break; }
            rc = check_block(s->u.loop.body, st); break; }
        }
    }
    symtab_pop(st);
    return rc;
}

static Type check_vec_bin(Type l, Type r, char op, int line, int *err){
    if(l.kind==TYPE_SCL && r.kind==TYPE_SCL)
        return (Type){TYPE_SCL,0};
    if(l.kind==TYPE_VEC && r.kind==TYPE_VEC){
        if(l.size!=r.size){
            fprintf(stderr,"line %d: vector size mismatch\n", line);
            *err=1; return (Type){TYPE_SCL,0};
        }
        return (Type){TYPE_VEC,l.size};
    }
    if(l.kind==TYPE_VEC && r.kind==TYPE_SCL)
        return (Type){TYPE_VEC,l.size};
    if(l.kind==TYPE_SCL && r.kind==TYPE_VEC)
        return (Type){TYPE_VEC,r.size};
    fprintf(stderr,"line %d: invalid operands for %c\n", line, op);
    *err=1; return (Type){TYPE_SCL,0};
}

static Type check_exp(Exp *e, SymTab *st, int *err){
    switch(e->kind){
    case EXP_IDENT:{
        if(symtab_lookup(st, e->u.ident.name, &e->type)!=0){
            fprintf(stderr,"line %d: undefined identifier %s\n", e->line, e->u.ident.name);
            *err=1; return (Type){TYPE_SCL,0};
        }
        return e->type; }
    case EXP_INT_LIT:
        return (Type){TYPE_SCL,0};
    case EXP_VEC_LIT:
        return (Type){TYPE_VEC,(int)e->u.vec_lit.count};
    case EXP_BINOP:{
        Type lt = check_exp(e->u.binop.left, st, err);
        if(*err) return lt;
        Type rt = check_exp(e->u.binop.right, st, err);
        if(*err) return rt;
        Type t = check_vec_bin(lt, rt, e->u.binop.op, e->line, err);
        e->type = t;
        return t; }
    case EXP_DOT:{
        Type lt = check_exp(e->u.dot.left, st, err);
        if(*err) return lt;
        Type rt = check_exp(e->u.dot.right, st, err);
        if(*err) return rt;
        if(lt.kind!=TYPE_VEC || rt.kind!=TYPE_VEC || lt.size!=rt.size){
            fprintf(stderr,"line %d: dot requires equal vectors\n", e->line);
            *err=1; return (Type){TYPE_SCL,0};
        }
        e->type = (Type){TYPE_SCL,0};
        return e->type; }
    case EXP_INDEX:{
        Type lt = check_exp(e->u.index.left, st, err);
        if(*err) return lt;
        Type rt = check_exp(e->u.index.right, st, err);
        if(*err) return rt;
        if(lt.kind!=TYPE_VEC){
            fprintf(stderr,"line %d: index left must be vector\n", e->line);
            *err=1; return (Type){TYPE_SCL,0};
        }
        if(rt.kind==TYPE_SCL){
            e->type = (Type){TYPE_SCL,0};
            return e->type;
        } else if(rt.kind==TYPE_VEC){
            if(lt.size!=rt.size){
                fprintf(stderr,"line %d: index vectors size mismatch\n", e->line);
                *err=1; return (Type){TYPE_SCL,0};
            }
            e->type = (Type){TYPE_VEC, lt.size};
            return e->type;
        }
        fprintf(stderr,"line %d: invalid right operand for index\n", e->line);
        *err=1; return (Type){TYPE_SCL,0}; }
    }
    return (Type){TYPE_SCL,0};
}
