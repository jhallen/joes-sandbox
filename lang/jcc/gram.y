/* These are returned by the lexer */

%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lex.h"

int yylex();
void yyerror(char *s);
%}

%token tEOF 0

%token tNUM tFP tSTRING tWORD tAUTO tBREAK tCASE tCHAR tCONST tCONTINUE
%token tDEFAULT tDO tDOUBLE tELSE tENUM tEXTERN tFLOAT tFOR tGOTO tIF tINT
%token tLONG tREGISTER tRETURN tSHORT tSIGNED tSIZEOF tSTATIC tSTRUCT tSWITCH
%token tTYPEDEF tUNION tUNSIGNED tVOID tVOLATILE tWHILE

/* These are not returned by the lexer */

%%

input: %empty | input line;

line : '{' tNUM '}' {
    printf("Value is %d\n", $2);
}

%%

int yylex()
{
    int t;
    t = get_tok(0);
    return t;
}

void yyerror(char *s)
{
    printf("%s\n", s);
}
