%{
#include <stdio.h>
void yyerror(const char *s);
int yylex(void);
extern int yylineno;
%}

%token SCL VEC IF LOOP PRINT IDENT INT_LIT

%left '+' '-'
%left '*' '/'
%left '@' ':'

%%
Program : Block ;

Block : '{' StmtListOpt '}' ;

StmtListOpt : /* empty */
            | StmtList ;

StmtList : Stmt
         | StmtList Stmt ;

Stmt : Decl ';'
     | Assign ';'
     | If
     | Loop
     | Print ';' ;

Decl : SCL IDENT
     | VEC IDENT '{' INT_LIT '}' ;

Assign : IDENT '=' Exp ;

If : IF Exp Block ;

Loop : LOOP Exp Block ;

Print : PRINT ExpList ;

ExpList : Exp
        | ExpList ',' Exp ;

Exp : INT_LIT
    | IDENT
    | '(' Exp ')'
    | Exp '+' Exp
    | Exp '-' Exp
    | Exp '*' Exp
    | Exp '/' Exp
    | Exp '@' Exp
    | Exp ':' Exp ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Parse error: %s at line %d\n", s, yylineno);
}
