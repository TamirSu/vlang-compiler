%option noyywrap nodefault yylineno

%{
#include "vlang.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* dupstr(const char* s){ char* d=malloc(strlen(s)+1); strcpy(d,s); return d; }
%}

%%
[ \t\r\n]+             { /* skip spaces and newlines */ }
"//"[^\n]*            { /* skip // comment */ }

"scl"                   { return SCL; }
"vec"                   { return VEC; }
"if"                    { return IF; }
"loop"                  { return LOOP; }
"print"                 { return PRINT; }

"\"[^\n\"]*\""         { int len=yyleng-2; char* s=malloc(len+1); memcpy(s,yytext+1,len); s[len]='\0'; yylval=s; return STRING_LIT; }

[A-Za-z_][A-Za-z0-9_]*  { if (yyleng > 32) { fprintf(stderr, "Identifier too long at line %d\n", yylineno); } yylval=dupstr(yytext); return IDENT; }
[0-9]+                  { int *v=malloc(sizeof(int)); *v=atoi(yytext); yylval=v; return INT_LIT; }

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
"@" { return '@'; }  /* dot product */
":" { return ':'; }  /* index */

.   { fprintf(stderr, "Unexpected character '%s' at line %d\n", yytext, yylineno); }
%%
