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

#include "ivy_error.h"

typedef struct ivy Ivy;		/* An interpreter */
typedef struct ivy_parser Ivy_parser;	/* Parser */
typedef struct ivy_parse_state Ivy_parse_state;
				/* Parser state stack entry */

typedef struct ivy_func Ivy_func;	/* A function */
typedef struct ivy_closure Ivy_closure;	/* A function in context (a closure) */
typedef struct ivy_val Ivy_val;		/* A value */
 
typedef struct ivy_string Ivy_string;		/* A string */
typedef struct ivy_obj Ivy_obj;		/* An object */

typedef struct ivy_entry Ivy_entry;	/* A hash table entry */
typedef unsigned char Ivy_pseudo;	/* Byte code */

#include "ivy_str.h"
#include "ivy_tree.h"
#include "ivy_symbols.h"
#include "ivy_frag.h"

/* Value types */

enum ivy_valtype {
	ivy_tVOID,		/* Nothing */
	ivy_tNUM,		/* Integer */
	ivy_tSTR,		/* String */
	ivy_tNAM,		/* A symbol (a name) */
	ivy_tOBJ,		/* Object */
	ivy_tCLOSURE,		/* A closure (function in its context) */
	ivy_tLST,		/* List count (only on stack) */
	ivy_tFP,		/* Floating point */
	ivy_tRET_IVY,		/* Normal function return */
	ivy_tRET_IVY_THUNK,	/* Normal function return for a thunk */
	ivy_tRET_NEXT_INIT,	/* Call next initializer */
	ivy_tRET_SIMPLE,	/* Call argument */
	ivy_tRET_SIMPLE_THUNK,	/* Call argument thunk */
	ivy_tPAIR,		/* A name value pair */
	ivy_tSCOPE,		/* A scope */
	ivy_tERROR = -1
};

/* A function closure */

struct ivy_closure {
	Ivy_func *func;	/* Code for function */
	Ivy_obj *env;	/* Environment to run it in */
};

/* A value: (5 words.. wasteful..) */

struct ivy_callstate;

struct ivy_val {
	enum ivy_valtype type;	/* What type this thing is */
	enum ivy_valtype idx_type;
	union {
		long long num;	/* An integer */
		double fp;	/* Floating point */
		Ivy_string *str;	/* A string */
		Ivy_closure closure;/* A function */
		Ivy_obj *obj;	/* An object */
		char *name;	/* A symbol */
		Ivy_pseudo *pc;	/* Return address when type is tRET_IVY */
	} u;
	Ivy_obj *origin;		/* Where value is from */
	union {
		long long num;
		char *name;
		Ivy_string *str;
		void (*func)(Ivy *ivy, struct ivy_callstate *t); /* Continuation function after tRET_SIMPLE */
		struct ivy_callstate *callstate; /* Only in tRET_IVY */
	} idx;			/* Index within origin */
};

#include "ivy_obj.h"
#include "ivy_gc.h"

/* State of function call in progress.  This tracks argument evaluations and initializer evaluations */

struct ivy_callstate {
	Ivy_obj *argv;	/* Argument vector (no need to mark, it's in scoping level for function) */
	Ivy_val *result;	/* Where arg result should go */
	Ivy_val *q;		/* Arg pointer on stack */
	int argn;	/* Argument vector index */
	int x;		/* Provided arg list counter */
	Ivy_obj *ovars;	/* Save caller's scope */
	Ivy_closure o;	/* Function we're calling */
	Ivy_val val;	/* String or object we're calling */
	void (*cont_func)(Ivy *, struct ivy_callstate *);	/* What to do next after RET_SIMPLE */
};

/* An interpreter */

struct ivy {
	Ivy *next, *prev;	/* Doubly-linked list of all existing ivys */
	Ivy_val *sp;	/* Stack pointer sp[0] is most recently pushed value */
	Ivy_val *spend; /* End of stack */
	Ivy_obj *vars;	/* Current deepest scoping level */
	Ivy_val stashed;	/* Stashed return value */
	Ivy_pseudo *pc;	/* Current program counter */
	void (*call_me)(Ivy *);
			/* C function to call */
	void *call_me_obj;
	Ivy_val *sptop;	/* Base of stack */
	Ivy_obj *glblvars;	/* Outer-most scoping level: Global variables */
	jmp_buf err;	/* Error return point */
	Ivy_error_printer errprn[1];	/* Error printer */
	FILE *out;	/* Standard output */
	FILE *in;	/* Standard input */
};

/* Parser state */

struct ivy_parse_state {
	Ivy_parse_state *next;
	int (*state)(Ivy_parser *);
	int prec;
	int last;
	Ivy_node *n;
	Ivy_node *cmd;
	Ivy_node *args;
	Ivy_node *blk;
	Ivy_what *op;
	int blvl;	/* Indentation level of start of command */
	int line;	/* Line number of last argument of command */
};

/* A parser */

struct ivy_parser {
	Ivy *ivy;		/* Interpreter */
	Ivy_error_printer *err;	/* Error printer (taken from 'ivy') */
	// Error_printer err[1];	/* Error printer */

	Ivy_free_list free_list[1];	/* Free list */

	Ivy_loc loc[1];		/* Current input location */

	char *str_buf;	/* String buffer */
	int str_siz;		/* Malloc size of string buffer */
	int str_len;		/* Current length of string in buffer */

	int paren_level;	/* Parenthesis depth */
	int need_more;		/* Set for continuation line */

	Ivy_parse_state state;	/* Current state */
	Ivy_node *rtn;
};


/* Ivy_node *compargs(char *s); Compile argument string into a tree (for creating built-in functions) */
Ivy_node *ivy_compargs(Ivy *ivy, const char *);

/* Main functions */

/* Interpret and execute a line immediately */
Ivy_parser *ivy_create_parser(Ivy *ivy, const char *file_name);
void ivy_free_parser(Ivy_parser *parser);
Ivy_val ivy_parse(Ivy *ivy, Ivy_parser *parser, const char *text, int unasm, int ptree, int ptop, int norun, int trace);
Ivy_val ivy_parse_done(Ivy *ivy, Ivy_parser *parser, int unasm, int ptree, int ptop, int norun, int trace);

/* Convert a parse-tree into pseudo-machine code */
Ivy_pseudo *ivy_codegen(Ivy_error_printer *err,Ivy_node *n);

/* A a C-function to the global table */
void ivy_addfunc(Ivy_error_printer *err, char *name, char *argstr, void (*cfunc) ());

#ifndef _Isetjmp
#define _Isetjmp 1
#include <setjmp.h>
#endif

/* Hash iterator for strings */

#define ivy_hnext(accu,c) (((accu) << 4) + ((accu) >> 28) + (c))

/* Compute hash value of symbol address */

//#define ivy_ahash(s) (((unsigned long)(s)>>3) ^ ((unsigned long)(s)>>12))
//#define ivy_ahash(s) (((unsigned long)(s)>>3))
#define ivy_fib_hash(s, n) (((unsigned long long)(s) * 11400714819323198485llu) >> (n))

/* A function */

struct ivy_func {
	Ivy_pseudo *code;		/* Code address */
	void (*cfunc)(Ivy *);	/* C function address */
	char **args;		/* Arguments names */
	Ivy_pseudo **inits;		/* Argument initializer code */
	char *quote;		/* Set to quote arg */
	int nargs;		/* No. args */
	int thunk;		/* Set if this is a thunk, which means it does not get its own scope level */
	char *argv;		/* extra args name */
	Ivy_pseudo *argv_init;	/* initializer for argv */
	char argv_quote;	/* Set to quote argv */
};

/* Pseudo-Instruction Set */

/* 1st refers to top of stack */
/* 2nd refers to second element on stack */

enum {
	/* Flow */
	ivy_iBRA,			/* iBRA <offset>        Add offset to PC */
	ivy_iBEQ,			/* iBEQ <offset>        POP, BRA if ==0 */
	ivy_iBNE,			/* iBNE <offset>        POP, BRA if !=0 */
	ivy_iBGT,			/* iBGT <offset>        POP, BRA if >0 */
	ivy_iBLT,			/* iBLT <offset>        POP, BRA if <0 */
	ivy_iBGE,			/* iBGE <offset>        POP, BRA if >=0 */
	ivy_iBLE,			/* iBLE <offset>        POP, BRA if <=0 */

	/* Basic operators */
	ivy_iCOM,			/* iCOM                 1's complement 1st */
	ivy_iNEG,			/* iNEG                 2's complement 1st */
	ivy_iSHL,			/* iSHL                 Shift 2nd left by 1st, POP */
	ivy_iSHR,			/* iSHR                 Shift 2nd right by 1st, POP */
	ivy_iMUL,			/* iMUL                 2nd*=1st, POP */
	ivy_iDIV,			/* iDIV                 2nd/=1st, POP */
	ivy_iMOD,			/* iMOD                 2nd%=1st, POP */
	ivy_iAND,			/* iAND                 2nd*=1st, POP */
	ivy_iADD,			/* iADD                 2nd+=1st, POP */
	/* iADD also concatenates strings */
	/* iADD also appends objects */
	ivy_iSUB,			/* iSUB                 2nd-=1st, POP */
	ivy_iOR,			/* iOR                  2nd|=1st, POP */
	/* iOR also unions objects */
	ivy_iXOR,			/* iXOR                 2nd^=1st, POP */
	ivy_iCMP,			/* iCMP                 2nd compared with 1st, POP */
	/* iCMP also works on strings */

	/* Block structuring */
	ivy_iBEG,			/* iBEG                 Make new block level */
	ivy_iEND,			/* iEND                 Remove 1 level of local vars */
	ivy_iLOC,			/* iLOC                 Create local variable */

	/* Variable lookup */
	ivy_iGET,			/* iGET_ATOM            Get named variable's value */
	ivy_iGETF,			/* iGETF_ATOM           Same as above, but force current scope */
	ivy_iAT,			/* iAT			Call zero-arg function */
	ivy_iENV,			/* iENV			replace environment of closure */

	/* Assignment */
	ivy_iSET,			/* iSET                 Assign value to variable */

	/* Functions / Arrays / Structures */
	ivy_iCALL,			/* iCALL                Call or get member/element */
	ivy_iSTASH,			/* iSTASH		Pop and stash a value (used for returning a value when stack needs unwinding) */
	ivy_iUNSTASH,			/* iUNSTASH		Push stashed value */
	ivy_iRTS,			/* iRTS                 Return from subroutine (PUSH stashed return value) */

	/* Stack */
	ivy_iPOP,			/* iPOP                 Kill 1st */

	ivy_iPSH_VOID,
	ivy_iPSH_THIS,
	ivy_iPSH_NUM,
	ivy_iPSH_LST,
	ivy_iPSH_FP,
	ivy_iPSH_STR,
	ivy_iPSH_NAM,
	ivy_iPSH_FUNC,
	ivy_iPSH_PAIR,

	ivy_iFOREACH,		/* iFOREACH             Iterate a list values */
	ivy_iFORINDEX,		/* iFORINDEX		Iterate a list keys */

	ivy_iFIX			/* iFIX                 Stack list -> object */
};

/* Member functions... */

Ivy_func *ivy_create_func(Ivy_pseudo *code, int nargs, char **args, Ivy_pseudo **inits, char *quote, int thunk, char *argv, Ivy_pseudo *argv_init, char argv_quote);	/* Create a function */

/* Other functions */

Ivy_val *ivy_getv_by_string(Ivy *,char *);	/* Get a variable - check all scope levels */
Ivy_val *ivy_getv_by_symbol(Ivy *,char *);	/* Same as above, but only with interned strings */

Ivy_val *ivy_get_origin(Ivy_val *v);
Ivy_val *ivy_set_origin(Ivy_val *v);

void ivy_scope_push(Ivy *ivy, Ivy_obj *dyn);		/* Add a scope level */
void ivy_scope_pop(Ivy *ivy);		/* Remove a scope level */
Ivy_obj *ivy_get_mom(Ivy_obj *o);

/* Make space on stack for one value */

void ivy_expand_stack(Ivy *ivy);

static inline Ivy_val *ivy_push(Ivy *ivy)
{
	if (++ivy->sp == ivy->spend)
		ivy_expand_stack(ivy);
	return ivy->sp;
}

/* Make a value */

static inline void ivy_val(Ivy_val *v, enum ivy_valtype type)
{
	v->type = type;
	v->origin = 0;
	v->idx_type = ivy_tVOID;
	v->idx.num = 0;
	v->u.num = 0;
}

static inline void ivy_void_val(Ivy_val *v)
{
	v->type = ivy_tVOID;
	v->origin = 0;
	v->idx_type = ivy_tVOID;
	v->idx.num = 0;
	v->u.num = 0;
}

static inline void ivy_push_void(Ivy *ivy)
{
	ivy_void_val(ivy_push(ivy));
}

static inline void ivy_scope_val(Ivy_val *v, Ivy_obj *old)
{
	v->type = ivy_tSCOPE;
	v->origin = 0;
	v->idx_type = ivy_tVOID;
	v->idx.num = 0;
	v->u.obj = old;
}

static inline void ivy_push_scope(Ivy *ivy, Ivy_obj *old)
{
	ivy_scope_val(ivy_push(ivy), old);
}

static inline void ivy_int_val(Ivy_val *v, long long i)
{
	v->type = ivy_tNUM;
	v->origin = 0;
	v->idx_type = ivy_tVOID;
	v->idx.num = 0;
	v->u.num = i;
}

static inline void ivy_push_int(Ivy *ivy, long long i)
{
	ivy_int_val(ivy_push(ivy), i);
}

static inline void ivy_lst_val(Ivy_val *v, long long i)
{
	v->type = ivy_tLST;
	v->origin = 0;
	v->idx_type = ivy_tVOID;
	v->idx.num = 0;
	v->u.num = i;
}

static inline void ivy_double_val(Ivy_val *v, double d)
{
	v->type = ivy_tFP;
	v->origin = 0;
	v->idx_type = ivy_tVOID;
	v->idx.num = 0;
	v->u.fp = d;
}

static inline void ivy_push_double(Ivy *ivy, double d)
{
	ivy_double_val(ivy_push(ivy), d);
}

static inline void ivy_obj_val(Ivy_val *v, Ivy_obj *u)
{
	v->type = ivy_tOBJ;
	v->origin = 0;
	v->idx_type = ivy_tVOID;
	v->idx.num = 0;
	v->u.obj = u;
}

static inline void ivy_push_obj(Ivy *ivy, Ivy_obj *u)
{
	ivy_obj_val(ivy_push(ivy), u);
}

static inline void ivy_string_val(Ivy_val *v, Ivy_string *u)
{
	v->type = ivy_tSTR;
	v->origin = 0;
	v->idx_type = ivy_tVOID;
	v->idx.num = 0;
	v->u.str = u;
}

static inline void ivy_push_string(Ivy *ivy, Ivy_string *u)
{
	ivy_string_val(ivy_push(ivy), u);
}

static inline void ivy_symbol_val(Ivy_val *v, char *s)
{
	v->type = ivy_tNAM;
	v->origin = 0;
	v->idx_type = ivy_tVOID;
	v->idx.num = 0;
	v->u.name = s;
}

static inline void ivy_push_symbol(Ivy *ivy, char *name)
{
	ivy_symbol_val(ivy_push(ivy), name);
}

static inline void ivy_closure_val(Ivy_val *v, Ivy_func *f, Ivy_obj *env)
{
	v->type = ivy_tCLOSURE;
	v->origin = 0;
	v->idx_type = ivy_tVOID;
	v->idx.num = 0;
	v->u.closure.func = f;
	v->u.closure.env = env;
}

Ivy_val *ivy_rmval(Ivy_val *v, int line);
Ivy_val *ivy_dup_val(Ivy_val *, Ivy_val *);

static inline void ivy_pop(Ivy_val *v, Ivy *ivy)
{
	ivy_dup_val(v, ivy->sp);
	ivy->sp = ivy_rmval(ivy->sp, __LINE__);
}

Ivy_val *ivy_pr(Ivy *ivy, FILE *out,Ivy_val *,int lvl);

/* Initialize an interpreter */
void ivy_setup(Ivy *ivy, void (*err_print)(void *obj, char *), void *err_obj, FILE *in, FILE *out);
void ivy_shutdown(Ivy *ivy);

/* Execute a function */
Ivy_val ivy_run(Ivy *, Ivy_pseudo *, int ptop, int trace);

/* Table of built-in functions */

extern struct ivy_builtin {
	const char *name; /* Function name */
	void (*cfunc) (Ivy *); /* Function address */
	const char *args; /* Argument list */
} ivy_builtins[];

void ivy_rthelp(Ivy *);

void ivy_disasm(FILE *out, Ivy_pseudo *code, int ind, int oneline);
int ivy_cntlst(Ivy_node *args);
int ivy_genlst(Ivy_error_printer *err, char **argv, Ivy_pseudo ** initv, char *quote, Ivy_node *n, int *ellipsis);

/* Atoms */
extern char *ivy_a_symbol;
extern char *ivy_b_symbol;
extern char *ivy_mom_symbol;
extern char *ivy_dynamic_symbol;
extern char *ivy_argv_symbol;

/* Doubly-linked list of all Ivys */
extern Ivy ivy_list[1];

#endif
