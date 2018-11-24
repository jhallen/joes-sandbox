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
typedef struct closure Closure;	/* A function in context (a closure) */
typedef struct val Val;		/* A value */
 
typedef struct str Str;		/* A string */
typedef struct obj Obj;		/* An object */

typedef struct entry Entry;	/* A hash table entry */
typedef unsigned char Pseudo;	/* Byte code */

#include "ivy_str.h"
#include "ivy_gc.h"
#include "ivy_tree.h"
#include "ivy_symbols.h"
#include "ivy_frag.h"

/* Value types */

enum valtype {
	tVOID,			/* Nothing */
	tNUM,			/* Integer */
	tSTR,			/* String */
	tNAM,			/* A symbol (a name) */
	tOBJ,			/* Object */
	tCLOSURE,		/* A closure (function in its context) */
	tLST,			/* List count (only on stack) */
	tFP,			/* Floating point */
	tRET_IVY,		/* Normal function return */
	tRET_IVY_THUNK,		/* Normal function return for a thunk */
	tRET_NEXT_INIT,		/* Call next initializer */
	tRET_SIMPLE,		/* Call argument */
	tRET_SIMPLE_THUNK,	/* Call argument thunk */
	tPAIR,			/* A name value pair */
	tERROR = -1
};

/* A function closure */

struct closure {
	Func *func;	/* Code for function */
	Obj *env;	/* Environment to run it in */
};

/* A value: (5 words.. wasteful..) */

struct callfunc;

struct val {
	enum valtype type;	/* What type this thing is */
	enum valtype idx_type;
	union {
		long long num;	/* An integer */
		double fp;	/* Floating point */
		Str *str;	/* A string */
		Closure closure;/* A function */
		Obj *obj;	/* An object */
		char *name;	/* A symbol */
		Pseudo *pc;	/* Return address when type is tRET_IVY */
	} u;
	Obj *origin;		/* Where value is from */
	union {
		long long num;
		char *name;
		Str *str;
		void (*func)(Ivy *ivy, struct callfunc *t); /* Continuation function after tRET_SIMPLE */
		struct callfunc *callfunc; /* Only in tRET_IVY */
	} idx;			/* Index within origin */
};

#include "ivy_obj.h"
#include "ivy_gc.h"

/* State of function call in progress.  This tracks argument evaluations and initializer evaluations */

struct callfunc {
	Obj *argv;	/* Argument vector (no need to mark, it's in scoping level for function) */
	Val *argv_result;	/* Where arg result goes in argv */
	Val *scope_result;	/* Where arg result goes in scope */
	Val *q;		/* Arg pointer on stack */
	int argn;	/* Argument vector index */
	int x;		/* Provided arg list counter */
	Obj *ovars;	/* Save caller's scope */
	Closure o;	/* Function we're calling */
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
Parser *mkparser(Ivy *ivy, const char *file_name);
void rmparser(Parser *parser);
Val parse(Ivy *ivy, Parser *parser, const char *text, int unasm, int ptree, int ptop, int norun, int trace);
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

/* Compute hash value of symbol address */

#define ahash(s) (((unsigned long)(s)>>3) ^ ((unsigned long)(s)>>12))

/* A function */

struct func {
	Pseudo *code;		/* Code address */
	void (*cfunc)(Ivy *);	/* C function address */
	char **args;		/* Arguments names */
	Pseudo **inits;		/* Argument initializer code */
	char *quote;		/* Set to quote arg */
	int nargs;		/* No. args */
	int thunk;		/* Set if this is a thunk, which means it does not get its own scope level */
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
	iAT,

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

Func *mkfunc(Pseudo *, int, char **, Pseudo **, char *, int thunk);	/* Create a function */

Closure *mkclosure(Func *, Obj *, void *ref_who, int ref_type);	/* Create a function in context */

/* Other functions */

Val *getv_by_string(Ivy *,char *);	/* Get a variable - check all scope levels */
Val *getv_by_symbol(Ivy *,char *);	/* Same as above, but only with interned strings */

Val *get_origin(Val *v);
Val *set_origin(Val *v);

void addlvl(Ivy *ivy, Obj *dyn);		/* Add a scope level */
void rmvlvl(Ivy *ivy);		/* Remove a scope level */
Obj *get_mom(Obj *o);

Val mkpval(enum valtype, void *);
void mkval(Val *,enum valtype);
Val mkival(enum valtype, long long);
Val mkdval(enum valtype, double);
Val *rmval(Val *, int);
Val *dupval(Val *, Val *);
Val *pr(Ivy *ivy, FILE *out,Val *,int lvl);

Val *psh(Ivy *);
Val popval(Ivy *);

void simple_call(Ivy *, Pseudo *, void (*func)(Ivy *,void *), void *obj);
void callfunc(Ivy *, Closure);

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
	const char *name; /* Function name */
	void (*cfunc) (Ivy *); /* Function address */
	const char *args; /* Argument list */
} builtins[];

void rthelp(Ivy *);

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
