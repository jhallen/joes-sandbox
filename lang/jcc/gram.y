/* These are returned by the lexer */

%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lex.h"

int yylex();
void yyerror(char *s);
%}

%union {
    struct num num;
    double fp;
    char *str;
}

%token tEOF 0
%token <num> tNUM
%token <str> tSTRING
%token <str> tWORD
%token <fp> tFP

%token tAUTO tBREAK tCASE tCHAR tCONST tCONTINUE
%token tDEFAULT tDO tDOUBLE tELSE tENUM tEXTERN tFLOAT tFOR tGOTO tIF tINT
%token tLONG tREGISTER tRETURN tSHORT tSIGNED tSIZEOF tSTATIC tSTRUCT tSWITCH
%token tTYPEDEF tUNION tUNSIGNED tVOID tVOLATILE tWHILE

/* These are not returned by the lexer */

%%

input: %empty | input line;

line : '{' tNUM '}' {
    printf("Value is %d\n", $2.num);
}

%%

int yylex()
{
    int t;
    t = get_tok(0);
    if (t == tFP)
        yylval.fp = float_val;
    else if (t == tNUM)
        yylval.num = num;
    else if (t == tWORD || t == tSTRING)
        yylval.str = strdup(word_buffer);
    return t;
}

void yyerror(char *s)
{
    printf("%s\n", s);
}
