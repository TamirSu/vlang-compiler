#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "codegen.h"
#include "runtime.h"

static char* dupstr(const char* s){ char* d=malloc(strlen(s)+1); strcpy(d,s); return d; }

static FILE *out;
static int tmp_id;

static char* tmpname(void){
    char buf[32];
    snprintf(buf,sizeof(buf),"t%d",tmp_id++);
    return dupstr(buf);
}

static char* gen_exp(Exp *e);

static void gen_stmt(Stmt *s){
    switch(s->kind){
    case STMT_DECL:
        if(s->u.decl.is_vec)
            fprintf(out,"vec %s = vec_create(%d);\n", s->u.decl.name, s->u.decl.size);
        else
            fprintf(out,"int %s;\n", s->u.decl.name);
        break;
    case STMT_ASSIGN:{
        char *rhs = gen_exp(s->u.assign.value);
        if(s->u.assign.value->type.kind==TYPE_VEC)
            fprintf(out,"%s = %s;\n", s->u.assign.name, rhs);
        else
            fprintf(out,"%s = %s;\n", s->u.assign.name, rhs);
        free(rhs);
        break; }
    case STMT_PRINT:{
        fprintf(out,"printf(\"%s\"", s->u.print.str);
        for(size_t i=0;i<s->u.print.arg_count;i++){
            Exp *e = s->u.print.args[i];
            char *v = gen_exp(e);
            if(e->type.kind==TYPE_VEC)
                fprintf(out,", vec_repr(%s)", v);
            else
                fprintf(out,", %s", v);
            free(v);
        }
        fprintf(out,");\n");
        break; }
    case STMT_IF:{
        char *cond = gen_exp(s->u.if_stmt.cond);
        fprintf(out,"if(%s){\n", cond);
        free(cond);
        for(size_t i=0;i<s->u.if_stmt.then_block->stmt_count;i++)
            gen_stmt(s->u.if_stmt.then_block->stmts[i]);
        fprintf(out,"}\n");
        break; }
    case STMT_LOOP:{
        char *cond = gen_exp(s->u.loop.cond);
        fprintf(out,"while(%s){\n", cond);
        free(cond);
        for(size_t i=0;i<s->u.loop.body->stmt_count;i++)
            gen_stmt(s->u.loop.body->stmts[i]);
        fprintf(out,"}\n");
        break; }
    }
}

static char* gen_exp(Exp *e){
    switch(e->kind){
    case EXP_INT_LIT:{
        char *t=tmpname();
        fprintf(out,"int %s = %d;\n", t, e->u.int_lit.value);
        return t; }
    case EXP_IDENT:
        return dupstr(e->u.ident.name);
    case EXP_VEC_LIT:{
        char *t=tmpname();
        fprintf(out,"vec %s = vec_from_array(%zu, (int[]){", t, e->u.vec_lit.count);
        for(size_t i=0;i<e->u.vec_lit.count;i++){
            if(i) fprintf(out,",");
            fprintf(out,"%d", e->u.vec_lit.values[i]);
        }
        fprintf(out,"});\n");
        return t; }
    case EXP_BINOP:{
        char *l = gen_exp(e->u.binop.left);
        char *r = gen_exp(e->u.binop.right);
        char *t=tmpname();
        if(e->u.binop.left->type.kind==TYPE_SCL && e->u.binop.right->type.kind==TYPE_SCL)
            fprintf(out,"int %s = %s %c %s;\n", t,l,e->u.binop.op,r);
        else if(e->u.binop.left->type.kind==TYPE_VEC && e->u.binop.right->type.kind==TYPE_VEC)
            fprintf(out,"vec %s = vec_binop(%s,%s,'%c');\n", t,l,r,e->u.binop.op);
        else if(e->u.binop.left->type.kind==TYPE_VEC && e->u.binop.right->type.kind==TYPE_SCL)
            fprintf(out,"vec %s = vec_binop_scl(%s,%s,'%c');\n", t,l,r,e->u.binop.op);
        else if(e->u.binop.left->type.kind==TYPE_SCL && e->u.binop.right->type.kind==TYPE_VEC)
            fprintf(out,"vec %s = vec_binop_scl_rev(%s,%s,'%c');\n", t,l,r,e->u.binop.op);
        free(l); free(r);
        return t; }
    case EXP_DOT:{
        char *l = gen_exp(e->u.dot.left);
        char *r = gen_exp(e->u.dot.right);
        char *t=tmpname();
        fprintf(out,"int %s = vec_dot(%s,%s);\n", t,l,r);
        free(l); free(r);
        return t; }
    case EXP_INDEX:{
        char *l = gen_exp(e->u.index.left);
        char *r = gen_exp(e->u.index.right);
        char *t=tmpname();
        if(e->u.index.right->type.kind==TYPE_SCL)
            fprintf(out,"int %s = vec_index(%s,%s);\n", t,l,r);
        else
            fprintf(out,"vec %s = vec_gather(%s,%s);\n", t,l,r);
        free(l); free(r);
        return t; }
    }
    return dupstr("0");
}

static void gen_block(Block *b){
    for(size_t i=0;i<b->stmt_count;i++)
        gen_stmt(b->stmts[i]);
}

int codegen_program(Program *prog, const char *src_path){
    mkdir("generated/c",0777);
    const char *base = strrchr(src_path,'/');
    base = base?base+1:src_path;
    char basecpy[128];
    strncpy(basecpy, base, sizeof(basecpy));
    char *dot = strrchr(basecpy,'.');
    if(dot) *dot='\0';
    char fname[256];
    snprintf(fname,sizeof(fname),"generated/c/%s.c",basecpy);
    out = fopen(fname,"w");
    if(!out) return 1;
    fprintf(out,"#include <stdio.h>\n#include \"runtime.h\"\nint main(){\n");
    tmp_id=0;
    gen_block(prog->block);
    fprintf(out,"return 0;\n}\n");
    fclose(out);
    return 0;
}
