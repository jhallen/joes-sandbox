/* Ivy header file

   Copyright (C) 1993 Joseph H. Allen

This file is part of IVY

IVY is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation; either version 1, or (at your option) any later version.  

IVY is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.  


IVY; see the file COPYING.  If not, write to the Free Software Foundation, 
675 Mass Ave, Cambridge, MA 02139, USA.  */

#ifndef _Iivy
#define _Iivy 1

#include "error.h"

typedef struct ivy Ivy;		/* An interpreter */
typedef struct parser Parser;	/* Parser */
typedef struct parse_state Parse_state;
				/* Parser state stack entry */

typedef struct func Func;	/* A function */
typedef struct fun Fun;		/* A function in context (a closure) */
typedef struct val Val;		/* A value */
 
typedef struct str Str;		/* A string */
typedef struct obj Obj;		/* An object */
typedef struct var Var;		/* A variable */

typedef struct entry Entry;	/* A hash table entry */
typedef unsigned char Pseudo;	/* Byte code */

#include "ivy_obj.h"
#include "ivy_str.h"
#include "ivy_fun.h"
#include "ivy_gc.h"
#include "ivy_tree.h"
#include "ivy_symbols.h"
#include "ivy_frag.h"

/* Value types */

enum valtype {
	tNUM,			/* Integer */
	tSTR,			/* String */
	tNAM,			/* A symbol (a name) */
	tOBJ,			/* Object */
	tFUN,			/* A function in its context */
	tLST,			/* List count (only on stack) */
	tVOID,			/* Nothing */
	tFP,			/* Floating point */
	tRET_IVY,		/* Normal function return */
	tRET_NEXT_INIT,		/* Call next initializer */
	tRET_SIMPLE,
	tPAIR			/* A name value pair */
};

/* A value */

struct val {
	enum valtype type;	/* What type this thing is */
	union {
		long long num;	/* An integer */
		double fp;	/* Floating point */
		Str *str;	/* A string */
		Fun *fun;	/* A function */
		Obj *obj;	/* An object */
		struct callfunc *callfunc; /* Only in tRET_IVY */
		void (*func)(Ivy *ivy, struct callfunc *t);
		char *name;	/* An atom */
		Entry *iter;
	} u;
	Var *var;		/* Variable where value came from */
};

#include "ivy_var.h"
#include "ivy_gc.h"

/* Call a function closure */

struct callfunc {
	Var *argv;	/* Argument vector */
	Var *a;		/* Where arg result goes... */
	Val *q;		/* Arg pointer */
	int argn;	/* Argument vector index */
	int x;		/* Stack index */
	Obj *ovars;	/* Save caller's scope */
	Fun *o;		/* Function we're calling */
	Val val;	/* String or object we're calling */
};

/* An interpreter */

struct ivy {
	Ivy *next;
	Error_printer errprn[1];	/* Error printer */
	Val stashed;	/* Stashed return value */
	Val *sptop;	/* Base of stack */
	Val *sp;	/* Stack */
	int spsize;	/* Stack size */
	Obj *glblvars;	/* Outer-most scoping level: Global variables */
	Obj *vars;	/* Current deepest scoping level */
	jmp_buf err;	/* Error return point */
	Pseudo *pc;	/* Current program counter */
	void (*call_me)(Ivy *);
			/* C function to call */
	void *call_me_obj;

	FILE *out;	/* Standard output */
	FILE *in;	/* Standard input */
};

/* Parser state */

struct parse_state {
	Parse_state *next;
	int (*state)(Parser *);
	int prec;
	int last;
	Node *n;
	Node *cmd;
	Node *args;
	Node *blk;
	What *op;
	int blvl;	/* Indentation level of start of command */
	int line;	/* Line number of last argument of command */
};

/* A parser */

struct parser {
	Ivy *ivy;		/* Interpreter */
	Error_printer *err;	/* Error printer (taken from 'ivy') */
	// Error_printer err[1];	/* Error printer */

	Free_list free_list[1];	/* Free list */

	Loc loc[1];		/* Current input location */

	char *str_buf;	/* String buffer */
	int str_siz;		/* Malloc size of string buffer */
	int str_len;		/* Current length of string in buffer */

	int paren_level;	/* Parenthesis depth */
	int need_more;		/* Set for continuation line */

	Parse_state state;	/* Current state */
	Node *rtn;
};


/* Node *compargs(char *s); Compile argument string into a tree (for creating built-in functions) */
Node *compargs(Ivy *ivy, char *);

/* Main functions */

/* Interpret and execute a line immediately */
Parser *mkparser(Ivy *ivy, char *file_name);
void rmparser();
Val parse(Ivy *ivy, Parser *parser, char *buf, int unasm, int ptree, int ptop, int norun, int trace);
void parse_done(Ivy *ivy, Parser *parser, int unasm, int ptree, int ptop, int norun, int trace);

/* Convert a parse-tree into pseudo-machine code */
Pseudo *codegen(Error_printer *err,Node *n);

/* A a C-function to the global table */
void addfunc(Error_printer *err, char *name, char *argstr, void (*cfunc) ());

#ifndef _Isetjmp
#define _Isetjmp 1
#include <setjmp.h>
#endif

/* Hash iterator for strings */

#define hnext(accu,c) (((accu) << 4) + ((accu) >> 28) + (c))

/* Compute hash value of atom address */

#define ahash(s) (((unsigned long)(s)>>3) ^ ((unsigned long)(s)>>12))

/* A function */

struct func {
	Pseudo *code;		/* Code address */
	void (*cfunc)(Ivy *);	/* C function address */
	char **args;	/* Arguments names */
	Pseudo **inits;		/* Argument initializer code */
	char *quote;	/* Set to quote arg */
	int nargs;		/* No. args */
};

/* Pseudo-Instruction Set */

/* 1st refers to top of stack */
/* 2nd refers to second element on stack */

enum {
	/* Flow */
	iBRA,			/* iBRA <offset>        Add offset to PC */
	iBEQ,			/* iBEQ <offset>        POP, BRA if ==0 */
	iBNE,			/* iBNE <offset>        POP, BRA if !=0 */
	iBGT,			/* iBGT <offset>        POP, BRA if >0 */
	iBLT,			/* iBLT <offset>        POP, BRA if <0 */
	iBGE,			/* iBGE <offset>        POP, BRA if >=0 */
	iBLE,			/* iBLE <offset>        POP, BRA if <=0 */

	/* Basic operators */
	iCOM,			/* iCOM                 1's complement 1st */
	iNEG,			/* iNEG                 2's complement 1st */
	iSHL,			/* iSHL                 Shift 2nd left by 1st, POP */
	iSHR,			/* iSHR                 Shift 2nd right by 1st, POP */
	iMUL,			/* iMUL                 2nd*=1st, POP */
	iDIV,			/* iDIV                 2nd/=1st, POP */
	iMOD,			/* iMOD                 2nd%=1st, POP */
	iAND,			/* iAND                 2nd*=1st, POP */
	iADD,			/* iADD                 2nd+=1st, POP */
	/* iADD also concatenates strings */
	/* iADD also appends objects */
	iSUB,			/* iSUB                 2nd-=1st, POP */
	iOR,			/* iOR                  2nd|=1st, POP */
	/* iOR also unions objects */
	iXOR,			/* iXOR                 2nd^=1st, POP */
	iCMP,			/* iCMP                 2nd compared with 1st, POP */
	/* iCMP also works on strings */

	/* Block structuring */
	iBEG,			/* iBEG                 Make new block level */
	iEND,			/* iEND                 Remove 1 level of local vars */
	iLOC,			/* iLOC                 Create local variable */

	/* Variable lookup */
	iGET,			/* iGET_ATOM            Get named variable's value */
	iGETF,			/* iGETF_ATOM           Same as above, but force current scope */
	iAT,			/* iAT                  Get value's address */

	/* Assignment */
	iSET,			/* iSET                 Assign value to variable */

	/* Functions / Arrays / Structures */
	iCALL,			/* iCALL                Call or get member/element */
	iSTASH,			/* iSTACH		Pop and stash return value */
	iRTS,			/* iRTS                 Return from subroutine (PUSH stashed return value) */

	/* Stack */
	iPOP,			/* iPOP                 Kill 1st */

	iPSH_VOID,
	iPSH_THIS,
	iPSH_NUM,
	iPSH_LST,
	iPSH_FP,
	iPSH_STR,
	iPSH_NAM,
	iPSH_FUNC,
	iPSH_PAIR,

	iFOREACH,		/* iFOREACH             Iterate a list values */
	iFORINDEX,		/* iFORINDEX		Iterate a list keys */

	iFIX			/* iFIX                 Stack list -> object */
};

/* Member functions... */

Func *mkfunc(Pseudo *, int, char **, Pseudo **, char *);	/* Create a function */

Fun *mkfun(Func *, Obj *, void *ref_who, int ref_type);	/* Create a function in context */

/* Other functions */

Var *getv_by_string(Ivy *,char *);	/* Get a variable - check all scope levels */
Var *getv_by_symbol(Ivy *,char *);	/* Same as above, but only with interned strings */

void addlvl(Ivy *ivy, Obj *dyn);		/* Add a scope level */
void rmvlvl(Ivy *ivy);		/* Remove a scope level */
Obj *get_mom(Obj *o);

Val mkpval(int, void *);
void mkval(Val *,int);
Val mkival(int, long long);
Val mkdval(int, double);
Val *rmval(Val *, int);
Val *dupval(Val *, Val *);
Val *pr(FILE *out,Val *,int lvl);

Val *psh(Ivy *);
Val popval(Ivy *);

void simple_call(Ivy *, Pseudo *, void (*func)(Ivy *,void *), void *obj);
void callfunc(Ivy *, Fun *);

/* Initialize an interpreter */
void mk_ivy(Ivy *ivy, void (*err_print)(void *obj, char *), void *err_obj, FILE *in, FILE *out);

/* Set global scope for interpreter */
void set_globals(Ivy *ivy, Obj *globals);

/* Create global scope: initialize it with built-in functions */
Obj *mk_globals(Ivy *ivy);

/* Execute a function */
Val run(Ivy *, Pseudo *, int ptop, int trace);

/* Table of built-in functions */

extern struct builtin {
	char *name; /* Function name */
	void (*cfunc) (); /* Function address */
	char *args; /* Argument list */
} builtins[];

void rthelp();

void disasm(FILE *out, Pseudo *code, int ind, int oneline);
int cntlst(Node *args);
int genlst(Error_printer *err, char **argv, Pseudo ** initv, char *quote, Node * n);

/* Atoms */
extern char *a_symbol;
extern char *b_symbol;
extern char *mom_symbol;
extern char *dynamic_symbol;
extern char *argv_symbol;

#endif
