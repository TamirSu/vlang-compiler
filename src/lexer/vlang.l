%{
#define _POSIX_C_SOURCE 200809L
#include "vlang.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
extern int fileno(FILE *);

#define YY_NO_UNPUT
#define YY_NO_INPUT

void yyerror(const char *s);
%}

%option noyywrap nodefault yylineno

%%
"scl"           { return SCL; }
"vec"           { return VEC; }
"if"            { return IF; }
"loop"          { return LOOP; }
"print"         { return PRINT; }

[0-9]+          { yylval = atoi(yytext); return INT_LIT; }
[A-Za-z_][A-Za-z0-9_]* {
                    if (yyleng > 32) {
                        fprintf(stderr, "error: identifier '%s' too long at line %d\n", yytext, yylineno);
                        exit(1);
                    }
                    yylval = 0; /* value unused for now */
                    return IDENT;
                  }

"//"[^\n]*     ;
[ \t\r\n]+     ;

"{"            { return '{'; }
"}"            { return '}'; }
"("            { return '('; }
")"            { return ')'; }
";"            { return ';'; }
","            { return ','; }
"="            { return '='; }
"@"            { return '@'; }
":"            { return ':'; }
"+"            { return '+'; }
"-"            { return '-'; }
"*"            { return '*'; }
"/"            { return '/'; }

.               { yyerror("unknown character"); }

%%
