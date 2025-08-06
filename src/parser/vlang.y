%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
int yylex(void);
void yyerror(const char *s);
extern int yylineno;

typedef struct { Stmt **items; size_t count; } StmtList;
static StmtList *stmtlist_new(void){ StmtList *l=malloc(sizeof(StmtList)); l->items=NULL; l->count=0; return l; }
static StmtList *stmtlist_add(StmtList *l, Stmt *s){ l->items=realloc(l->items,sizeof(Stmt*)*(l->count+1)); l->items[l->count++]=s; return l; }

typedef struct { Exp **items; size_t count; } ExpList;
static ExpList *explist_new(void){ ExpList *l=malloc(sizeof(ExpList)); l->items=NULL; l->count=0; return l; }
static ExpList *explist_add(ExpList *l, Exp *e){ l->items=realloc(l->items,sizeof(Exp*)*(l->count+1)); l->items[l->count++]=e; return l; }

typedef struct { int *items; size_t count; } IntList;
static IntList *intlist_new(void){ IntList *l=malloc(sizeof(IntList)); l->items=NULL; l->count=0; return l; }
static IntList *intlist_add(IntList *l, int v){ l->items=realloc(l->items,sizeof(int)*(l->count+1)); l->items[l->count++]=v; return l; }

Program *gProgram = NULL;
%}

%define api.value.type {void*}

/* tokens */
%token SCL VEC PRINT IF LOOP STRING_LIT
%token IDENT INT_LIT

%left '+' '-'
%left '*' '/'
%left '@' ':'   /* dot product and index, highest precedence */

%start Program

%%
Program    : Block { gProgram = ast_program((Block*)$1); };

Block      : '{' OptStmtList '}' { $$ = ast_block(((StmtList*)$2)->items, ((StmtList*)$2)->count); free($2); };

OptStmtList
          : /* empty */       { $$ = stmtlist_new(); }
          | StmtList          { $$ = $1; }
          ;

StmtList  : Stmt              { StmtList* l=stmtlist_new(); if($1) stmtlist_add(l,(Stmt*)$1); $$=l; }
          | StmtList Stmt     { if($2) stmtlist_add((StmtList*)$1,(Stmt*)$2); $$=$1; }
          ;

/* statements */
Stmt      : ';'                      { $$ = NULL; }
          | Decl ';'                 { $$ = $1; }
          | Assign ';'               { $$ = $1; }
          | Print ';'                { $$ = $1; }
          | If                       { $$ = $1; }
          | Loop                     { $$ = $1; }
          ;

Print     : PRINT STRING_LIT ':' PrintArgsOpt { $$ = ast_stmt_print(yylineno, (char*)$2, ((ExpList*)$4)->items, ((ExpList*)$4)->count); free($2); free($4); };

PrintArgsOpt
          : /* empty */       { $$ = explist_new(); }
          | PrintArgs         { $$ = $1; }
          ;

PrintArgs : Exp                       { ExpList* l=explist_new(); explist_add(l,(Exp*)$1); $$=l; }
          | PrintArgs ',' Exp         { $$ = explist_add((ExpList*)$1,(Exp*)$3); }
          ;

If        : IF Exp Block { $$ = ast_stmt_if(yylineno, (Exp*)$2, (Block*)$3); };

Loop      : LOOP Exp Block { $$ = ast_stmt_loop(yylineno, (Exp*)$2, (Block*)$3); };

/* declarations */
Decl      : SCL IDENT                      { $$ = ast_stmt_decl(yylineno,0,(char*)$2,0); free($2); }
          | VEC IDENT '{' INT_LIT '}'      { $$ = ast_stmt_decl(yylineno,1,(char*)$2, *(int*)$4); free($2); free($4); }
          ;

/* assignment */
Assign    : IDENT '=' Exp { $$ = ast_stmt_assign(yylineno,(char*)$1,(Exp*)$3); free($1); };

/* expressions */
Exp       : INT_LIT                      { $$ = ast_exp_int(yylineno, *(int*)$1); free($1); }
           | IDENT                        { $$ = ast_exp_ident(yylineno,(char*)$1); free($1); }
           | '(' Exp ')'                  { $$ = $2; }
           | Exp '+' Exp                  { $$ = ast_exp_binop(yylineno,'+',(Exp*)$1,(Exp*)$3); }
           | Exp '-' Exp                  { $$ = ast_exp_binop(yylineno,'-',(Exp*)$1,(Exp*)$3); }
           | Exp '*' Exp                  { $$ = ast_exp_binop(yylineno,'*',(Exp*)$1,(Exp*)$3); }
           | Exp '/' Exp                  { $$ = ast_exp_binop(yylineno,'/',(Exp*)$1,(Exp*)$3); }
           | Exp '@' Exp                  { $$ = ast_exp_dot(yylineno,(Exp*)$1,(Exp*)$3); }
           | Exp ':' Exp                  { $$ = ast_exp_index(yylineno,(Exp*)$1,(Exp*)$3); }
           | VecLit                       { $$ = $1; }
           ;

VecLit    : '[' IntListOpt ']'            { $$ = ast_exp_vec_lit(yylineno, ((IntList*)$2)->items, ((IntList*)$2)->count); free($2); };

IntListOpt
          : /* empty */       { $$ = intlist_new(); }
          | IntList          { $$ = $1; }
          ;

IntList   : INT_LIT                    { IntList* l=intlist_new(); int v=*(int*)$1; free($1); intlist_add(l,v); $$=l; }
          | IntList ',' INT_LIT        { int v=*(int*)$3; free($3); $$=intlist_add((IntList*)$1,v); }
          ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "syntax error: %s at line %d\n", s, yylineno);
}
