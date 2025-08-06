%{
#include <stdio.h>
#include <stdlib.h>
int yylex(void);
void yyerror(const char *s);
extern int yylineno;
%}

/* tokens */
%token SCL VEC PRINT IF LOOP STRING_LIT
%token IDENT INT_LIT

%left '+' '-'
%left '*' '/'
%left '@' ':'   /* dot product and index, highest precedence */

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
          | Print ';'
          | If
          | Loop
          ;

Print     : PRINT STRING_LIT ':' PrintArgsOpt ;

PrintArgsOpt
          : /* empty */
          | PrintArgs
          ;

PrintArgs : Exp
          | PrintArgs ',' Exp
          ;

If        : IF Exp Block ;

Loop      : LOOP Exp Block ;

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
           | Exp '@' Exp   /* dot product */
           | Exp ':' Exp   /* index */
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
