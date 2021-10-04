/* C-language subset for introspection */

%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lex.h"
#include "tree.h"

int yylex();
void yyerror(char *s);

struct llvalterm {
    char *file_name;
    int line;
};

struct llvalnum {
    char *file_name;
    int line;
    struct num num;
};

struct llvalfp {
    char *file_name;
    int line;
    double fp;
};

struct llvals {
    char *file_name;
    int line;
    char *s;
};

struct llvaln {
    char *file_name;
    int line;
    Node *n;
};

%}

%union {
    struct llvalterm term;
    struct llvalnum num;
    struct llvalfp fp;
    struct llvals s;
    struct llvaln n;
}

%token tEOF 0

%token <num> tNUM
%token <s> tSTRING
%token <s> tWORD
%token <fp> tFP

%token <term> tAUTO tBREAK tCASE tCHAR tCONST tCONTINUE
%token <term> tDEFAULT tDO tDOUBLE tELSE tENUM tEXTERN tFLOAT tFOR tGOTO tIF tINT
%token <term> tLONG tREGISTER tRETURN tSHORT tSIGNED tSIZEOF tSTATIC tSTRUCT tSWITCH
%token <term> tTYPEDEF tUNION tUNSIGNED tVOID tVOLATILE tWHILE
%token <n> tTYPE

%type <num> const_expr

%type <n> input inside simple_type decl_name decl

%left <term> '-' '+'
%left <term> '*' '/'
%left <term> '['

%token <term> ';'

/* These are not returned by the lexer */

%%

top : input { print_tree($1.n); printf("\n"); };

input
  : %empty {
    $$.file_name = 0;
    $$.line = 0;
    $$.n = (void *)0;
} | input tTYPEDEF decl {
    $$ = $1;
    if ($3.n->s)
      add_typedef($3.n->s, $3.n->r);
} | input decl {
    if ($1.n) {
        $$.file_name = $1.file_name;
        $$.line = $1.line;
        $$.n = cons(.what = nLIST, .l = $1.n, .r = $2.n);
    } else
        $$ = $2;
} ;

inside
  : %empty {
    $$.file_name = 0;
    $$.line = 0;
    $$.n = (void *)0;
} | inside decl {
    if ($1.n) {
        $$.file_name = $1.file_name;
        $$.line = $1.line;
        $$.n = cons(.what = nLIST, .l = $1.n, .r = $2.n);
    } else
        $$ = $2;
} ;

simple_type
  : tVOID {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntVOID);
} | tCHAR {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntCHAR);
} | tSIGNED tCHAR {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntCHAR);
} | tUNSIGNED tCHAR {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntUCHAR);
} | tSHORT {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntSHORT);
} | tSIGNED tSHORT {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntSHORT);
} | tUNSIGNED tSHORT {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntUSHORT);
} | tINT {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntLONG);
} | tSIGNED tINT {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntLONG);
} | tUNSIGNED tINT {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntULONG);
} | tLONG {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntLONG);
} | tSIGNED tLONG {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntLONG);
} | tUNSIGNED tLONG {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntULONG);
} | tFLOAT {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntFLOAT);
} | tDOUBLE {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntDOUBLE);
} | tSTRUCT tWORD {
    Node *ty = lookup_struct($2.s);
    // printf("Find struct %s found %p\n", $2, ty);
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntSTRUCT, .s = $2.s, .r = ty);
} | tUNION tWORD {
    Node *ty = lookup_union($2.s);
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntUNION, .s = $2.s, .r = ty);
} | tSTRUCT tWORD '{' inside '}' {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntSTRUCT, .s = $2.s, .r = $4.n);
    if (add_struct($$.n->s, $$.n->r))
    {
        printf("duplicate struct definition\n");
    }
} | tUNION tWORD '{' inside '}' {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntUNION, .s = $2.s, .r = $4.n);
    if (add_union($$.n->s, $$.n->r))
    {
        printf("duplicate union definition\n");
    }
} | tSTRUCT '{' inside '}' {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntSTRUCT, .s = NULL, .r = $3.n);
} | tUNION '{' inside '}' {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = ntUNION, .s = NULL, .r = $3.n);
} | tTYPE {
    $$ = $1;
} ;

decl_name
 : tWORD {
   $$.file_name = $1.file_name;
   $$.line = $1.line;
   $$.n = cons(.what = nDECL, .s = $1.s, .r = NULL);
} | '(' decl_name ')' {
    $$ = $2;
} | '*' decl_name {
    $$ = $2;
    $$.n->r = list_append($$.n->r, cons(.what = ntPTR, .r = NULL));
} | decl_name '[' const_expr ']' {
    $$ = $1;
    $$.n->r = list_append($$.n->r, cons(.what = ntARRAY, .siz = $3.num.num, .r = NULL));
} ;

decl
  : error ';' {
    printf("%s %d: Syntax error\n", $2.file_name, $2.line);
    $$.file_name = $2.file_name;
    $$.line = $2.line;
    $$.n = cons(.what = nNOTHING);
}  | simple_type decl_name ';' {
    $$ = $2; $$.n->r = list_append($$.n->r, $1.n);
} | simple_type ';' {
    // FIXME: should complain if it's a type with no side-effects
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.n = cons(.what = nNOTHING);
} ;

const_expr
  : tNUM {
    $$ = $1;
} | '(' const_expr ')' {
    $$ = $2;
} | const_expr '+' const_expr {
    $$.file_name = $2.file_name;
    $$.line = $2.line;
    $$.num.num = $1.num.num + $3.num.num;
} | const_expr '-' const_expr {
    $$.file_name = $2.file_name;
    $$.line = $2.line;
    $$.num.num = $1.num.num - $3.num.num;
} | const_expr '*' const_expr {
    $$.file_name = $2.file_name;
    $$.line = $2.line;
    $$.num.num = $1.num.num * $3.num.num;
} | const_expr '/' const_expr {
    $$.file_name = $2.file_name;
    $$.line = $2.line;
    $$.num.num = $1.num.num / $3.num.num;
} | '-' const_expr %prec '*' {
    $$.file_name = $1.file_name;
    $$.line = $1.line;
    $$.num.num = -$2.num.num;
} ;

%%

int yylex()
{
    int t;
    Node *n;
    t = get_tok(0);
    if (t == tWORD && (n = lookup_typedef(word_buffer))) {
        t = tTYPE;
        yylval.n.file_name = file_name;
        yylval.n.line = line;
        yylval.n.n = n;
    } else if (t == tFP) {
        yylval.fp.file_name = file_name;
        yylval.fp.line = line;
        yylval.fp.fp = float_val;
    } else if (t == tNUM) {
        yylval.num.file_name = file_name;
        yylval.num.line = line;
        yylval.num.num = num;
    } else if (t == tWORD || t == tSTRING) {
        yylval.s.file_name = file_name;
        yylval.s.line = line;
        yylval.s.s = strdup(word_buffer);
    } else {
        yylval.term.file_name = file_name;
        yylval.term.line = line;
    }
    return t;
}

void yyerror(char *s)
{
    printf("%s\n", s);
}
