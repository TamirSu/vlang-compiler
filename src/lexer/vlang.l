%option noyywrap nodefault yylineno

%{
#include "vlang.tab.h"
#include <stdio.h>
%}

%%
[ \t\r\n]+             { /* skip spaces and newlines */ }
"//"[^\n]*            { /* skip // comment */ }

"scl"                   { return SCL; }
"vec"                   { return VEC; }
"if"                    { return IF; }
"loop"                  { return LOOP; }
"print"                 { return PRINT; }

[A-Za-z_][A-Za-z0-9_]*  { if (yyleng > 32) { fprintf(stderr, "Identifier too long at line %d\n", yylineno); } return IDENT; }
[0-9]+                  { return INT_LIT; }

"{" { return '{'; }
"}" { return '}'; }
";" { return ';'; }
"=" { return '='; }
"(" { return '('; }
")" { return ')'; }
"+" { return '+'; }
"-" { return '-'; }
"*" { return '*'; }
"/" { return '/'; }
"[" { return '['; }
"]" { return ']'; }
"," { return ','; }
"@" { return '@'; }
":" { return ':'; }

.   { fprintf(stderr, "Unexpected character '%s' at line %d\n", yytext, yylineno); }
%%
