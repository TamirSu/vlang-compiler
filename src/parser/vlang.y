%{
#include <stdio.h>
#include <stdlib.h>
int yylex(void);
void yyerror(const char *s);
extern int yylineno;
%}

/* tokens */
%token SCL VEC IF LOOP PRINT
%token IDENT INT_LIT

%left '+' '-'
%left '*' '/'
%left '@' ':'   /* highest */

%start Program

%%
Program    : Block ;

Block      : '{' OptStmtList '}' ;

OptStmtList
          : /* empty */
          | StmtList
          ;

StmtList  : Stmt
          | StmtList Stmt
          ;

/* statements */
Stmt      : ';'
          | Decl ';'
          | Assign ';'
          ;

/* declarations */
Decl      : SCL IDENT
          | VEC IDENT '{' INT_LIT '}'
          ;

/* assignment */
Assign    : IDENT '=' Exp ;

/* expressions (minimal for now) */
Exp       : INT_LIT
           | IDENT
           | '(' Exp ')'
           | Exp '+' Exp
           | Exp '-' Exp
           | Exp '*' Exp
           | Exp '/' Exp
           | Exp '@' Exp
           | Exp ':' Exp
           | VecLit
           ;

VecLit    : '[' IntListOpt ']';

IntListOpt
          : /* empty */
          | IntList
          ;

IntList   : INT_LIT
          | IntList ',' INT_LIT
          ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "syntax error: %s at line %d\n", s, yylineno);
}
