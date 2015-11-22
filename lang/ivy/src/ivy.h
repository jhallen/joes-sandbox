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

#define REF_DEBUG 0

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
typedef struct pos Pos;		/* A string position */

typedef struct entry Entry;	/* A hash table entry */
typedef unsigned char Pseudo;	/* Byte code */

#if REF_DEBUG
typedef struct ref_list Ref;
#else
typedef int Ref;
#endif

/* A value */

struct val {
	union {
		long long num;	/* An integer */
		double fp;	/* Floating point */
		Var *var;	/* A variable */
		Str *str;	/* A string */
		Fun *fun;	/* A function */
		Obj *obj;	/* An object */
		Pos *pos;	/* A string position */
		struct callfunc *callfunc; /* Only in tRET_IVY */
		void (*func)();	/* Some kind of function address */
		char *name;	/* An atom */
		Entry *iter;
	} u;
	int type;		/* What type this thing is */
	Var *var;		/* Variable where value came from */
};

/* Value types */

enum {
	tNUM,			/* Integer */
	tSTR,			/* String */
	tNAM,			/* A name (an atom) */
	tOBJ,			/* Object */
	tVAR,			/* A variable */
	tFUN,			/* A function in its context */
	tFUNC,			/* A function */
	tLST,			/* List count (only on stack) */
	tNARG,			/* Named argument (only on stack) */
	tPOS,			/* A string position (only on stack) */
	tVOID,			/* Nothing */
	tFP,			/* Floating point */
	tRET_IVY,		/* Normal function return */
	tRET_SIMPLE		/* Return from simple call (for initializers and quoting) */
};

/* An interpreter */

struct ivy {
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

/* A reference list (for debugging) */

struct ref_list {
	struct ref_list *next;
	int type;
	void *who;
	int line;
};

#if REF_DEBUG
int _dec_ref(Ref *ref, void *who, int line);
void _inc_ref(Ref *ref, void *who, int type, int line);
void _init_ref(Ref *ref, void *who, int type, int line);
void show_ref(Ref *ref);
#else
#define _dec_ref(ref, who, line) --*(ref)
#define _inc_ref(ref, who, type, line) ++*(ref)
#define _init_ref(ref, who, type, line) (*(ref)=1)
#endif

#define dec_ref(ref, who) _dec_ref(ref, who, __LINE__)
#define inc_ref(ref, who, type) _inc_ref(ref, who, type, __LINE__)
#define init_ref(ref, who, type) _init_ref(ref, who, type, __LINE__)

/* Reference type */

enum {
	rOBJ,	/* Objs own variables and other OBJs */
	rFUN,	/* Closers have a ref on the scope OBJ */
	rVAL,	/* Value hold a ref on variable it most recently came from */
	rSCOPE
};

/* A variable */

struct var {
	Ref ref;		/* Reference count or mark */
	Val val;		/* The value of the variable */
};

/* String */

struct str {
	Ref ref;		/* Reference count or mark */
	char *s;		/* Pointer to string */
	int len;		/* Size of string */
};

/* A string position */

struct pos {
	Ref ref;		/* Reference count or mark */
	Val var;		/* The string variable */
	int pos;		/* Offset into string */
};

/* A function */

struct func {
	Ref ref;		/* Reference count or mark */
	Pseudo *code;		/* Code address */
	void (*cfunc)(Ivy *);	/* C function address */
	char **args;	/* Arguments names */
	Pseudo **inits;		/* Argument initializer code */
	char *quote;	/* Set to quote arg */
	int nargs;		/* No. args */
};

/* A function in its context */

struct fun {
	Ref ref;		/* Reference count or mark */
	Func *f;		/* Actual function */
	Obj *scope;		/* Context function was created in */
	Val *init_vals;		/* Initialization values */
	int x;			/* The init value we're up to */
};

/* An object (a hash table) */

struct obj {
	Ref ref;		/* Reference count or mark */
#ifdef ONEXT
	Obj *next;		/* Next outer scoping level or NULL for root */
#else
				/* Next outer scoping level is in mom */
#endif

	Entry **tab;		/* Hash table of 'ENTRY' pointers */
	int size;		/* No. of ENTRY pointers in 'tab' array */

	Var **ary;		/* Automatic array of variables */
	int arysiz;		/* Size of malloc block array is in */
	int nitems;		/* One plus highest numbered member in array */

	int visit;		/* Visit flag */

	int objno;
};

Obj *get_mom(Obj *o);

/* A hash table entry: for variables and structure members */

struct entry {
	char *name;		/* Member name (an atom) */
	Entry *next;		/* next entry with same hash value */
	Var *var;		/* Variable containing Value assigned to this member */
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
	iGET,			/* iGET                 Get named variable's value */
	iGET_ATOM,
	iGETF,			/* iGETF		Same as above, but force current scope */
	iGETF_ATOM,
	iAT,			/* iAT                  Get value's address */

	/* Assignment */
	iSET,			/* iSET                 Assign value to variable */

	/* Functions / Arrays / Structures */
	iCALL,			/* iCALL                Call or get member/element */
	iSTASH,			/* iSTACH		Pop and stash return value */
	iRTS,			/* iRTS                 Return from subroutine (PUSH stashed return value) */

	/* Stack */
	iPOP,			/* iPOP                 Kill 1st */

	iPSH_NARG,		/* iPSH                 Push various things */
	iPSH_VOID,
	iPSH_THIS,
	iPSH_NUM,
	iPSH_LST,
	iPSH_FP,
	iPSH_STR,
	iPSH_NAM,
	iPSH_FUNC,

	iFOREACH,		/* iFOREACH             Iterate a list values */
	iFORINDEX,		/* iFORINDEX		Iterate a list keys */

	iFIX			/* iFIX                 Stack list -> object */
};

/* Member functions... */
Obj *mkobj(int size, void *ref_who, int ref_type, int line);	/* Create an object */
void rmobj(Obj *);		/* Delete an object */
Var *get(Obj *, char *);	/* Get named member from an object */
Var *getn(Obj *, int);		/* Get numbered member from an object */
Var *set(Obj *, char *);	/* Set named member of an an object */
Var *setn(Obj *, int);		/* Set numbered member of an object */
Obj *dupobj(Obj *, void *, int, int);		/* Duplicate an object */

Var *mkvar(void *ref_who, int ref_type);		/* Create a variable */
void rmvar(Var *, int line);		/* Delete a variable */

Str *mkstr(char *, int len, void *ref_who, int ref_type, int line);	/* Create a string */
void rmstr(Str *);		/* Delete a string */

Pos *mkpos(Val var, int, void *ref_who, int ref_type);	/* Create string position */
void rmpos(Pos *);		/* Delete string position */

Func *mkfunc(Pseudo *, int, char **, Pseudo **, char *);	/* Create a function */

Fun *mkfun(Func *, Obj *, void *ref_who, int ref_type);	/* Create a function in context */

/* Other functions */

Var *getv(Ivy *,char *);	/* Get a variable - check all scope levels */

void addlvl(Ivy *ivy, Obj *dyn);		/* Add a scope level */
void rmvlvl(Ivy *ivy);		/* Remove a scope level */

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

#endif
