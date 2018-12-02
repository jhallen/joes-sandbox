/* Code generator

   Copyright (C) 1993 Joseph H. Allen

This file is part of IVY

IVY is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software
Foundation; either version 1, or (at your option) any later version.  

IVY is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.  

You should have received a copy of the GNU General Public License along with 
IVY; see the file COPYING.  If not, write to the Free Software Foundation, 
675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include "ivy.h"

// #define AUTOSCOPE 1

Ivy_func *ivy_create_func(Ivy_pseudo *code, int nargs, char **args, Ivy_pseudo **inits, char *quote, int thunk,
			char *argv, Ivy_pseudo *argv_init, char argv_quote)
{
	Ivy_func *func = (Ivy_func *)malloc(sizeof(Ivy_func));
	func->code = code;
	func->nargs = nargs;
	func->args = args;
	func->inits = inits;
	func->quote = quote;
	func->cfunc = 0;
	func->thunk = thunk;
	func->argv = argv;
	func->argv_init = argv_init;
	func->argv_quote = argv_quote;
	return func;
}

/* Add branch list 'b' to end of branch list 'a' */

static void addlist(Ivy_frag *frag, int a, int b)
{
	while (ivy_fragn(frag, a))
		a = ivy_fragn(frag, a);
	ivy_fragn(frag, a) = b;
}

/* Set each value in branch list 'a' to destination 'b' */

static void setlist(Ivy_frag *frag, int a, int b)
{
	while (a) {
		int c = ivy_fragn(frag, a);
		ivy_fragn(frag, a) = b - a;
		a = c;
	}
}

/* Track contents of the stack.  Also track loops.  */

#define lvlSCOPE 0
#define lvlLOOP 1
#define lvlVALUE 2

struct ivy_looplvl {
	struct ivy_looplvl *next;	/* Next level */
	int what;		/* 2 = value, 1 = loop, 0 = scope */
	int cont;		/* List of continue destinations */
	int brk;		/* List of brk destinations */
	char *name;		/* Named level */
};

static void push_looplvl(Ivy_frag *frag, int what, int cont, int brk)
{
	struct ivy_looplvl *ll =
	    (struct ivy_looplvl *) malloc(sizeof(struct ivy_looplvl));
	ll->what = what;
	ll->cont = cont;
	ll->brk = brk;
	ll->next = frag->looplvls;
	ll->name = 0;
	frag->looplvls = ll;
	if (ll->what == lvlSCOPE)
		ivy_emitc(frag, ivy_iBEG);
}

static char *looplvl_name(int what)
{
	switch (what) {
		case 0: return "scope";
		case 1: return "loop";
		case 2: return "value";
		default: return "unknown";
	}
}

static void show_looplvls(Ivy_frag *frag)
{
	struct ivy_looplvl *ll = frag->looplvls;
	while (ll) {
		printf("  %s\n", looplvl_name(ll->what));
		ll = ll->next;
	}
}

static void pop_looplvl(Ivy_frag *frag, int what, int cont, int brk)
{
	struct ivy_looplvl *ll = frag->looplvls;
	frag->looplvls = ll->next;
	if (what != ll->what) {
		printf("Expected level of type %s\n", looplvl_name(what));
		printf("But we have this:\n");
		show_looplvls(frag);
		abort();
	}
	if (ll->what == lvlLOOP) {
		setlist(frag, ll->cont, cont);
		setlist(frag, ll->brk, brk);
	}
	if (ll->what == lvlSCOPE)
		ivy_emitc(frag, ivy_iEND);
	if (ll->name)
		free(ll->name);
	free(ll);
}

/* Find named loop, or with NULL, find innermost loop */

static struct ivy_looplvl *findlvl(Ivy_frag *frag, char *name)
{
	struct ivy_looplvl *ll;
	if (name)
		for (ll = frag->looplvls; ll && (!ll->name || strcmp(ll->name, name)); ll = ll->next);
	else
		for (ll = frag->looplvls; ll && ll->what != lvlLOOP; ll = ll->next);
	return ll;
}

/* Convert stack list to object */

static void fixlooplvl(Ivy_frag *frag, int amnt)
{
	int x;
	for (x = 0; x != amnt; ++x)
		pop_looplvl(frag, lvlVALUE, 0, 0);
}

/* Pop for premature exit */

static void poploops(Ivy_frag *frag, struct ivy_looplvl *target)
{
	struct ivy_looplvl *ll;
	for (ll = frag->looplvls; ll && ll != target; ll = ll->next) {
		if (ll->what == lvlSCOPE)
			ivy_emitc(frag, ivy_iEND);
		else if (ll->what == lvlVALUE)
			ivy_emitc(frag, ivy_iPOP);
	}
}

static void poploops1(Ivy_frag *frag, struct ivy_looplvl *target)
{
	struct ivy_looplvl *ll;
	for (ll = frag->looplvls->next; ll && ll != target; ll = ll->next) {
		if (ll->what == lvlSCOPE)
			ivy_emitc(frag, ivy_iEND);
		else if (ll->what == lvlVALUE)
			ivy_emitc(frag, ivy_iPOP);
	}
}

/* Count values we would have to pop */

static int countvalues(Ivy_frag *frag, struct ivy_looplvl *target)
{
	int count = 0;
	struct ivy_looplvl *ll;
	for (ll = frag->looplvls; ll && ll != target; ll = ll->next) {
		if (ll->what == lvlVALUE)
			++count;
	}
	return count;
}

/* Push something */

static void push_str(Ivy_frag *frag)
{
	push_looplvl(frag, lvlVALUE, 0, 0);
	ivy_emitc(frag, ivy_iPSH_STR);
}

static void push_nam(Ivy_frag *frag)
{
	push_looplvl(frag, lvlVALUE, 0, 0);
	ivy_emitc(frag, ivy_iPSH_NAM);
}

static void push_narg(Ivy_frag *frag)
{
	// push_looplvl(frag, lvlVALUE, 0, 0); /* It's counted with the value */
	ivy_emitc(frag, ivy_iPSH_NAM);
}

static void push_pair(Ivy_frag *frag)
{
	ivy_emitc(frag, ivy_iPSH_PAIR);
}

static void push_func(Ivy_frag *frag)
{
	push_looplvl(frag, lvlVALUE, 0, 0);
	ivy_emitc(frag, ivy_iPSH_FUNC);
}

static void push_lst(Ivy_frag *frag)
{
	push_looplvl(frag, lvlVALUE, 0, 0);
	ivy_emitc(frag, ivy_iPSH_LST);
}

static void push_void(Ivy_frag *frag)
{
	push_looplvl(frag, lvlVALUE, 0, 0);
	ivy_emitc(frag, ivy_iPSH_VOID);
}

static void push_this(Ivy_frag *frag)
{
	push_looplvl(frag, lvlVALUE, 0, 0);
	ivy_emitc(frag, ivy_iPSH_THIS);
}

static void push_num(Ivy_frag *frag)
{
	push_looplvl(frag, lvlVALUE, 0, 0);
	ivy_emitc(frag, ivy_iPSH_NUM);
}

static void push_fp(Ivy_frag *frag)
{
	push_looplvl(frag, lvlVALUE, 0, 0);
	ivy_emitc(frag, ivy_iPSH_FP);
}

/* Disassembler */

void ivy_disasm(FILE *out, Ivy_pseudo * c, int ind, int oneline)
{
	Ivy_pseudo *start = c;
	if (ind < 0) {
		fprintf(out, "Bad indent value\n");
		return;
	}
	ivy_indent(out, ind); fprintf(out, "%p:\n", start);
	for (;;) {
		ivy_indent(out, ind); fprintf(out, "%d ", (int)(c - start));
		switch (*c++) {
			case ivy_iBRA: {
				c += ivy_align_o(c, sizeof(int));
				ivy_indent(out, ind); fprintf(out, "	bra %d\n", *(int *)c + (int)(c - start));
				c += sizeof(int);
				break;
			} case ivy_iBEQ: {
				c += ivy_align_o(c, sizeof(int));
				ivy_indent(out, ind); fprintf(out, "	beq %d\n", *(int *)c + (int)(c - start));
				c += sizeof(int);
				break;
			} case ivy_iBNE: {
				c += ivy_align_o(c, sizeof(int));
				ivy_indent(out, ind); fprintf(out, "	bne %d\n", *(int *)c + (int)(c - start));
				c += sizeof(int);
				break;
			} case ivy_iBGT: {
				c += ivy_align_o(c, sizeof(int));
				ivy_indent(out, ind); fprintf(out, "	bgt %d\n", *(int *)c + (int)(c - start));
				c += sizeof(int);
				break;
			} case ivy_iBLT: {
				c += ivy_align_o(c, sizeof(int));
				ivy_indent(out, ind); fprintf(out, "	blt %d\n", *(int *)c + (int)(c - start));
				c += sizeof(int);
				break;
			} case ivy_iBGE: {
				c += ivy_align_o(c, sizeof(int));
				ivy_indent(out, ind); fprintf(out, "	bge %d\n", *(int *)c + (int)(c - start));
				c += sizeof(int);
				break;
			} case ivy_iBLE: {
				c += ivy_align_o(c, sizeof(int));
				ivy_indent(out, ind); fprintf(out, "	ble %d\n", *(int *)c + (int)(c - start));
				c += sizeof(int);
				break;
			} case ivy_iCOM: {
				ivy_indent(out, ind); fprintf(out, "	com\n");
				break;
			} case ivy_iNEG: {
				ivy_indent(out, ind); fprintf(out, "	neg\n");
				break;
			} case ivy_iSHL: {
				ivy_indent(out, ind); fprintf(out, "	shl\n");
				break;
			} case ivy_iSHR: {
				ivy_indent(out, ind); fprintf(out, "	shr\n");
				break;
			} case ivy_iMUL: {
				ivy_indent(out, ind); fprintf(out, "	mul\n");
				break;
			} case ivy_iDIV: {
				ivy_indent(out, ind); fprintf(out, "	div\n");
				break;
			} case ivy_iMOD: {
				ivy_indent(out, ind); fprintf(out, "	mod\n");
				break;
			} case ivy_iAND: {
				ivy_indent(out, ind); fprintf(out, "	and\n");
				break;
			} case ivy_iADD: {
				ivy_indent(out, ind); fprintf(out, "	add\n");
				break;
			} case ivy_iSUB: {
				ivy_indent(out, ind); fprintf(out, "	sub\n");
				break;
			} case ivy_iOR: {
				ivy_indent(out, ind); fprintf(out, "	or\n");
				break;
			} case ivy_iXOR: {
				ivy_indent(out, ind); fprintf(out, "	xor\n");
				break;
			} case ivy_iCMP: {
				ivy_indent(out, ind); fprintf(out, "	cmp\n");
				break;
			} case ivy_iBEG: {
				ivy_indent(out, ind); fprintf(out, "	beg\n");
				break;
			} case ivy_iEND: {
				ivy_indent(out, ind); fprintf(out, "	end\n");
				break;
			} case ivy_iLOC: {
				ivy_indent(out, ind); fprintf(out, "	loc\n");
				break;
			} case ivy_iGET: {
				ivy_indent(out, ind); fprintf(out, "	get\n");
				break;
			} case ivy_iGETF: {
				ivy_indent(out, ind); fprintf(out, "	getf\n");
				break;
			} case ivy_iAT: {
				ivy_indent(out, ind); fprintf(out, "	at\n");
				break;
			} case ivy_iENV: {
				ivy_indent(out, ind); fprintf(out, "	env\n");
				break;
			} case ivy_iSET: {
				ivy_indent(out, ind); fprintf(out, "	set\n");
				break;
			} case ivy_iCALL: {
				ivy_indent(out, ind); fprintf(out, "	call\n");
				break;
			} case ivy_iRTS: {
				ivy_indent(out, ind); fprintf(out, "	rts\n");
				return;
			} case ivy_iSTASH: {
				ivy_indent(out, ind); fprintf(out, "	stash\n");
				break;
			} case ivy_iUNSTASH: {
				ivy_indent(out, ind); fprintf(out, "	unstash\n");
				break;
			} case ivy_iPOP: {
				ivy_indent(out, ind); fprintf(out, "	pop\n");
				break;
			} case ivy_iFOREACH: {
				c += ivy_align_o(c, sizeof(int));
				ivy_indent(out, ind); fprintf(out, "	foreach %d\n", *(int *)c + (int)(c - start));
				c += sizeof(int);
				break;
			} case ivy_iFORINDEX: {
				c += ivy_align_o(c, sizeof(int));
				ivy_indent(out, ind); fprintf(out, "	forindex %d\n", *(int *)c + (int)(c - start));
				c += sizeof(int);
				break;
			} case ivy_iFIX: {
				ivy_indent(out, ind); fprintf(out, "	fix\n");
				break;
			} case ivy_iPSH_NUM: {
				c += ivy_align_o(c, sizeof(long long));
				ivy_indent(out, ind); fprintf(out, "	psh_num %lld\n", *(long long *)c);
				c += sizeof(long long);
				break;
			} case ivy_iPSH_FP: {
				c += ivy_align_o(c, sizeof(double));
				ivy_indent(out, ind); fprintf(out, "	psh_fp %g\n", *(double *)c);
				c += sizeof(double);
				break;
			} case ivy_iPSH_STR: {
				c += ivy_align_o(c, sizeof(int));
				ivy_indent(out, ind); fprintf(out, "	psh_str \"%s\"\n", c + sizeof(int));
				c += sizeof(int) + *(int *)c + 1;
				break;
			} case ivy_iPSH_NAM: {
				c += ivy_align_o(c, sizeof(char *));
				ivy_indent(out, ind); fprintf(out, "	psh_nam %s\n", *(char **)c);
				c += sizeof(char *);
				break;
			} case ivy_iPSH_VOID: {
				ivy_indent(out, ind); fprintf(out, "	psh_void\n");
				break;
			} case ivy_iPSH_THIS: {
				ivy_indent(out, ind); fprintf(out, "	psh_this\n");
				break;
			} case ivy_iPSH_LST: {
				c += ivy_align_o(c, sizeof(int));
				ivy_indent(out, ind); fprintf(out, "	psh_lst %d\n", *(int *)c);
				c += sizeof(int);
				break;
			} case ivy_iPSH_FUNC: {
				c += ivy_align_o(c, sizeof(void *));
				ivy_indent(out, ind); fprintf(out, "	psh_func %p\n", *(void **)c);
				ivy_disasm(out, ((Ivy_func *)*(void **)c)->code, ind + 4, 0);
				c += sizeof(void *);
				break;
			} case ivy_iPSH_PAIR: {
				ivy_indent(out, ind); fprintf(out, "	psh_pair\n");
				break;
			} default: {
				ivy_indent(out, ind); fprintf(out, "	unknown???\n");
				break;
			}
		}
		if (oneline)
			break;
	}
}

static int genl(Ivy_error_printer *, Ivy_frag *, Ivy_node *);
static int genbra(Ivy_error_printer *, Ivy_frag *, Ivy_node *, int);
static void gen(Ivy_error_printer *, Ivy_frag *, Ivy_node *);
static void genn(Ivy_error_printer *, Ivy_frag *, Ivy_node *);

/* Count no. of comma seperated elements.  Use for args and 1st expr of FOR */

int ivy_cntlst(Ivy_node * n)
{
	switch(n->what) {
		case ivy_nEMPTY: {
			return 0;
		} case ivy_nCALL: case ivy_nCOMMA: case ivy_nSEMI: {
			return ivy_cntlst(n->l) + ivy_cntlst(n->r);
		} case ivy_nPAREN: {
			return ivy_cntlst(n->r);
		} default: {
			return 1;
		}
	}
}

/* Generate list of comma seperated names (for arg list in function declarations) */

int ivy_genlst(Ivy_error_printer *err, char **argv, Ivy_pseudo ** initv, char *quote, Ivy_node *n, int *ellipsis)
{
	switch (n->what) {
		case ivy_nEMPTY: {
			return 0;
		} case ivy_nELLIPSIS: {
			if (*ellipsis) {
				ivy_error_2(err, "\"%s\" %d: ellipsis already given", n->loc->name, n->loc->line);
			}
			*ellipsis = 1;
			return ivy_genlst(err, argv, initv, quote, n->r, ellipsis);
		} case ivy_nADDR: {
			quote[0] = 1;
			return ivy_genlst(err, argv, initv, quote, n->r, ellipsis);
		} case ivy_nPAREN: {
			return ivy_genlst(err, argv, initv, quote, n->r, ellipsis);
		} case ivy_nSEMI: case ivy_nCOMMA: case ivy_nCALL: {
			int x = ivy_genlst(err, argv, initv, quote, n->l, ellipsis);
			return x + ivy_genlst(err, argv + x, initv + x, quote + x, n->r, ellipsis);
		} case ivy_nNAM: {
			return (argv[0] = n->s), (initv[0] = 0), 1;
		} case ivy_nSET: {
			if (n->l->what == ivy_nNAM)
				return (argv[0] = n->l->s), (initv[0] = ivy_codegen(err, n->r)), 1;
			else if (n->l->what == ivy_nADDR && n->l->r->what == ivy_nNAM) {
				quote[0] = 1;
				return (argv[0] = n->l->r->s), (initv[0] = ivy_codegen(err, n->r)), 1;
			}
			break;
		}
	}
	ivy_error_3(err, "\"%s\" %d: incorrect argument list %s", n->loc->name, n->loc->line, ivy_what_tab[n->what].name);
	return 0;
}

/* Generate and count list (used to generate inside of objects) */

static int genl(Ivy_error_printer *err, Ivy_frag *frag, Ivy_node * n)
{
	int result;
	switch (n->what) {
		case ivy_nEMPTY: {
			return 0;
		} case ivy_nSEMI: {
			result = genl(err, frag, n->r);
			return result + genl(err, frag, n->l);
		} case ivy_nSET: {
			if (n->l->what == ivy_nQUOTE && n->l->r->what == ivy_nNAM) {
				gen(err, frag, n->r);
				push_narg(frag);
				ivy_emitp(frag, n->l->r->s);
				push_pair(frag);
				return 1;
			} else if (n->l->what == ivy_nQUOTE && n->l->r->what == ivy_nSTR) {
				gen(err, frag, n->r);
				push_str(frag);
				ivy_emits(frag, n->l->r->s, n->l->r->n);
				push_pair(frag);
				return 1;
			} else if (n->l->what == ivy_nQUOTE && n->l->r->what == ivy_nNUM) {
				gen(err, frag, n->r);
				push_num(frag);
				ivy_emitl(frag, n->l->r->n);
				push_pair(frag);
				return 1;
			}
			break;
		}
	}
	gen(err, frag, n);
	return 1;
}

/* Generate a function */

static void genfunc(Ivy_error_printer *err, Ivy_frag *frag, Ivy_node *args, Ivy_node *body, int thunk)
{
	Ivy_pseudo *cod = ivy_codegen(err, body);
	Ivy_func *o;
	int argc = ivy_cntlst(args);
	char *quote;
	int ellipsis = 0;
	/* FIXME: argc can be zero, don't trust malloc to work with zero size */
	char **argv = (char **)malloc(argc * sizeof(char *));
	Ivy_pseudo **initv = (Ivy_pseudo **)malloc(argc * sizeof(Ivy_pseudo *));
	quote = (char *)calloc(argc,1);
	ivy_genlst(err, argv, initv, quote, args, &ellipsis);
	if (!ellipsis)
		o = ivy_create_func(cod, argc, argv, initv, quote, thunk, 0, 0, 0);
	else
		o = ivy_create_func(cod, argc - 1, argv, initv, quote, thunk, argv[argc-1], initv[argc-1], quote[argc-1]);
	push_func(frag);
	ivy_emitp(frag, o);
}

/* Generate and count list (used to generate function call arg lists) */

static int gencl(Ivy_error_printer *err, Ivy_frag *frag, Ivy_node * n)
{
	int result;
	switch (n->what) {
		case ivy_nEMPTY: {
			return 0;
		} case ivy_nSEMI: {
			result = gencl(err, frag, n->r);
			return result + gencl(err, frag, n->l);
		} case ivy_nSET: {
			if (n->l->what == ivy_nQUOTE && n->l->r->what == ivy_nNAM) {
				genfunc(err, frag, ivy_consempty(n->loc), n->r, 1);
				push_narg(frag);
				ivy_emitp(frag, n->l->r->s);
				push_pair(frag);
				return 1;
			} else if (n->l->what == ivy_nQUOTE && n->l->r->what == ivy_nSTR) {
				genfunc(err, frag, ivy_consempty(n->loc), n->r, 1);
				push_str(frag);
				ivy_emits(frag, n->l->r->s, n->l->r->n);
				push_pair(frag);
				return 1;
			} else if (n->l->what == ivy_nQUOTE && n->l->r->what == ivy_nNUM) {
				genfunc(err, frag, ivy_consempty(n->loc), n->r, 1);
				push_num(frag);
				ivy_emitl(frag, n->l->r->n);
				push_pair(frag);
				return 1;
			}
			break;
		}
	}
	genfunc(err, frag, ivy_consempty(n->loc), n, 1);
	return 1;
}

/* Generate and count list for 'var' */

static int genll(Ivy_error_printer *err, Ivy_frag *frag, Ivy_node * n)
{
	switch (n->what) {
		case ivy_nEMPTY: {
			return 0;
		} case ivy_nSEMI: {
			int result = genll(err, frag, n->r);
			return result + genll(err, frag, n->l);
		} case ivy_nNAM: {
			push_nam(frag);
			ivy_emitp(frag, n->s);
			return 1;
		} case ivy_nSET: {
			if (n->l->what == ivy_nNAM) {
				push_nam(frag);
				ivy_emitp(frag, n->l->s);
				return 1;
			} else {
				ivy_error_2(err, "\"%s\" %d: incorrect list for vars", n->loc->name, n->loc->line);
				return 0;
			}
		}
	}
	ivy_error_2(err, "\"%s\" %d: incorrect list for vars", n->loc->name, n->loc->line);
	return 0;
}

/* Generate initializers for 'var': no need to check, genll was always called previously */

static int genla(Ivy_error_printer *err, Ivy_frag *frag, Ivy_node * n)
{
	switch (n->what) {
		case ivy_nEMPTY: {
			return 0;
		} case ivy_nSEMI: {
			int result = genla(err, frag, n->r);
			return result + genla(err, frag, n->l);
		} case ivy_nNAM: {
			return 1;
		} case ivy_nSET: {
			genn(err, frag, n);
			return 1;
		}
	}
	return 0;
}

/* Generate cond */

static void gencond(Ivy_error_printer *err, Ivy_frag *frag, Ivy_node *n, int v)
{
	Ivy_node *f, *r;
	int end = 0;

	loop:	

	f = ivy_first(n);
	r = ivy_first(ivy_rest(n));
	n = ivy_rest(ivy_rest(n));

	if (f && r) { /* elif */
		int els = genbra(err, frag, f, 1); /* Branch if false */
#ifdef AUTOSCOPE
		push_looplvl(frag, lvlSCOPE, 0, 0);
#endif
		if (v) {
			gen(err, frag, r);
			/* Value is still here, but we need to pop the lvlSCOPE */
			/* Value is put back at the branch target point: XXX */
			pop_looplvl(frag, lvlVALUE, 0, 0);
		} else
			genn(err, frag, r);
#ifdef AUTOSCOPE
		pop_looplvl(frag, lvlSCOPE, 0, 0);
#endif
		ivy_emitc(frag, ivy_iBRA);
		if (end)
			addlist(frag, end, ivy_emitn(frag, 0));
		else
			end = ivy_emitn(frag, 0);
		/* Value is gone after the branch */
		setlist(frag, els, frag->code);
		goto loop;
	} else if (f) { /* else */
#ifdef AUTOSCOPE
		push_looplvl(frag, lvlSCOPE, 0, 0);
#endif
		if (v) {
			gen(err, frag, f);
			pop_looplvl(frag, lvlVALUE, 0, 0);
		} else
			genn(err, frag, f);
#ifdef AUTOSCOPE
		pop_looplvl(frag, lvlSCOPE, 0, 0);
#endif
		if (v) {
			push_looplvl(frag, lvlVALUE, 0, 0); /* here XXX */
		}
		setlist(frag, end, frag->code);
		return;
	} else { /* no else */
		if (v) {
			push_void(frag); /* or here XXX */
		}
		setlist(frag, end, frag->code);
		return;
	}
}


/* Generate a value: it's left on the stack */

static void gen(Ivy_error_printer *err, Ivy_frag *frag, Ivy_node * n)
{
	switch(n->what) {
		case ivy_nCOMMA: {
			genn(err, frag, n->l), gen(err, frag, n->r);
			break;
		} case ivy_nPAREN: {
			gen(err, frag, n->r);
			break;
		} case ivy_nLIST: {
			int amnt = genl(err, frag, n->r);
			push_lst(frag);
			ivy_emitn(frag, amnt);
			ivy_emitc(frag, ivy_iFIX);
			fixlooplvl(frag, amnt);
			break;
		} case ivy_nVOID: {
			push_void(frag);
			break;
		} case ivy_nTHIS: {
			push_this(frag);
			break;
		} case ivy_nNUM: {
			push_num(frag);
			ivy_emitl(frag, n->n);
			break;
		} case ivy_nFP: {
			push_fp(frag);
			ivy_emitd(frag, n->fp);
			break;
		} case ivy_nSTR: {
			push_str(frag);
			ivy_emits(frag, n->s, n->n);
			break;
		} case ivy_nNAM: {
			push_nam(frag);
			ivy_emitp(frag, n->s);
			ivy_emitc(frag, ivy_iGET);
			break;
		} case ivy_nQUOTE: {
			push_nam(frag);
			ivy_emitp(frag, n->r->s);
			break;
		} case ivy_nSET: {
			gen(err, frag, n->r);
			gen(err, frag, n->l);
			ivy_emitc(frag, ivy_iSET);
			pop_looplvl(frag, lvlVALUE, 0, 0);
			break;
		} case ivy_nIF: {
			gencond(err, frag, n->r, 1);
			break;
		} case ivy_nPOST: {
			gen(err, frag, n->l);
			gen(err, frag, n->r);
			gen(err, frag, n->l);
			ivy_emitc(frag, ivy_iSET);
			pop_looplvl(frag, lvlVALUE, 0, 0);
			ivy_emitc(frag, ivy_iPOP);
			pop_looplvl(frag, lvlVALUE, 0, 0);
			break;
		} case ivy_nADDR: {
			/* Generate a code snippet */
			genfunc(err, frag, ivy_consempty(n->loc), n->r, 1);
			break;
		} case ivy_nDEFUN: {
			if (n->r->what==ivy_nSEMI) {
				if (n->r->l->what==ivy_nCALL /* && n->r->l->l->what==nNAM */) { /* fn sq(x) x*x */
					genfunc(err, frag, n->r->l->r, n->r->r, 0);
					if (n->r->l->l->what == ivy_nNAM) {
						push_nam(frag);
						ivy_emitp(frag, n->r->l->l->s);
						ivy_emitc(frag, ivy_iGETF);
					} else
						gen(err, frag, n->r->l->l);
					ivy_emitc(frag, ivy_iSET);
					pop_looplvl(frag, lvlVALUE, 0, 0);
				} else if (n->r->l->what != ivy_nPAREN && n->r->r->what==ivy_nSEMI && n->r->r->l->what==ivy_nPAREN) { /* fn sq (x) x*x */
					genfunc(err, frag, n->r->r->l, n->r->r->r, 0);
					if (n->r->l->what == ivy_nNAM) {
						push_nam(frag);
						ivy_emitp(frag, n->r->l->s);
						ivy_emitc(frag, ivy_iGETF);
					} else
						gen(err, frag, n->r->l);
					ivy_emitc(frag, ivy_iSET);
					pop_looplvl(frag, lvlVALUE, 0, 0);
				} else if (n->r->l->what != ivy_nPAREN && n->r->r->what==ivy_nPAREN) { /* fn sq (x) */
					genfunc(err, frag, n->r->r, ivy_consempty(n->loc), 0);
					if (n->r->l->what == ivy_nNAM) { /* it looks like "a", use getf */
						push_nam(frag);
						ivy_emitp(frag, n->r->l->s);
						ivy_emitc(frag, ivy_iGETF);
					} else
						gen(err, frag, n->r->l); /* it looks like "a.b" */
					ivy_emitc(frag, ivy_iSET);
					pop_looplvl(frag, lvlVALUE, 0, 0);
				} else if (n->r->l->what == ivy_nPAREN) { /* fn (x) x*x */
					genfunc(err, frag, n->r->l, n->r->r, 0);
				} else {
					ivy_error_2(err, "\"%s\" %d: ill-formed fn", n->r->loc->name, n->r->loc->line);
					push_void(frag);
				}
			} else if(n->r->what==ivy_nCALL /* && n->r->l->what==nNAM */) { /* fn sq(x) */
				genfunc(err, frag, n->r->r, ivy_consempty(n->loc), 0);
				if (n->r->l->what == ivy_nNAM) {
					push_nam(frag);
					ivy_emitp(frag, n->r->l->s);
					ivy_emitc(frag, ivy_iGETF);
				} else
					gen(err, frag, n->r->l);
				ivy_emitc(frag, ivy_iSET);
				pop_looplvl(frag, lvlVALUE, 0, 0);
			} else if(n->r->what == ivy_nPAREN) { /* fn () */
				genfunc(err,frag, n->r, ivy_consempty(n->loc), 0);
			} else {
				ivy_error_2(err, "\"%s\" %d: ill-formed fn", n->r->loc->name, n->r->loc->line);
				push_void(frag);
			}
			break;
		} case ivy_nSEMI: {
			genn(err, frag, n->l);
			gen(err, frag, n->r);
			break;
		} case ivy_nEQ: case ivy_nNE: case ivy_nGT: case ivy_nLT: case ivy_nGE: case ivy_nLE: case ivy_nLAND: case ivy_nLOR: case ivy_nNOT: {
			int b = genbra(err, frag, n, 1);
			int link;
			push_num(frag);
			ivy_emitl(frag, 1);
			ivy_emitc(frag, ivy_iBRA);
			link=ivy_emitn(frag, 0);
			setlist(frag, b, frag->code);
			push_num(frag);
			ivy_emitl(frag, 0);
			*(int *)(frag->begcode+link)=frag->code-link;
			break;
		} case ivy_nCALL: {
			int nargs = gencl(err, frag, n->r); /* Ivy_functionalize */
			push_lst(frag);
			ivy_emitn(frag, nargs);
			gen(err, frag, n->l);
			ivy_emitc(frag, ivy_iCALL);
			fixlooplvl(frag, nargs + 1);
			break;
		} case ivy_nCALL1: { /* Ends up being the same as above */
			if (n->r->what == ivy_nNAM) { // In "a.b", avoid variable lookup on "b"
				n->r = ivy_cons1(n->loc, ivy_nQUOTE, n->r);
			}
			int nargs = gencl(err, frag, n->r);
			push_lst(frag);
			ivy_emitn(frag, nargs);
			gen(err, frag, n->l);
			ivy_emitc(frag, ivy_iCALL);
			fixlooplvl(frag, nargs + 1);
			break;
		} case ivy_nCOM: case ivy_nNEG: case ivy_nSHL: case ivy_nSHR: case ivy_nMUL: case ivy_nDIV: case ivy_nMOD: case ivy_nAND:
		  case ivy_nADD: case ivy_nSUB: case ivy_nOR: case ivy_nXOR: case ivy_nAT: case ivy_nENV: {
			if (n->l)
				gen(err, frag, n->l);
			if (n->r)
				gen(err, frag, n->r);
			ivy_emitc(frag, ivy_what_tab[n->what].i);
			if (n->r && n->l)
				pop_looplvl(frag, lvlVALUE, 0, 0);
			break;
		} case ivy_nSCOPE: {
			push_looplvl(frag, lvlSCOPE, 0, 0);
			gen(err, frag, n->r);
			ivy_emitc(frag, ivy_iSTASH);
			pop_looplvl(frag, lvlVALUE, 0, 0);

			pop_looplvl(frag, lvlSCOPE, 0, 0);

			ivy_emitc(frag, ivy_iUNSTASH);
			push_looplvl(frag, lvlVALUE, 0, 0);
			break;
		} case ivy_nELLIPSIS: {
			ivy_error_2(err, "\"%s\" %d: invalid use of ...", n->loc->name, n->loc->line);
			push_void(frag);
			break;
		} default: {
			genn(err, frag, n);
			push_void(frag);
		}
	}
}

/* Generate nothing (returns true if guarenteed to branch) */

static int last_is_paren(Ivy_node *n)
{
	if (n->what == ivy_nPAREN)
		return 1;
	else if (n->what == ivy_nSEMI)
		return last_is_paren(n->r);
	else
		return 0;
}

static Ivy_node *extract_last_is_paren(Ivy_node *n, Ivy_node **r)
{
	if (n->what == ivy_nPAREN) {
		*r = n;
		return ivy_consempty(n->loc);
	} else if (n->what == ivy_nSEMI) {
		n->r = extract_last_is_paren(n->r, r);
		return n;
	} else {
		return n;
	}
}

static Ivy_node *extract_loop_name(Ivy_node *n, Ivy_node **r)
{
	if (n->what == ivy_nSEMI && n->l->what == ivy_nQUOTE && n->l->r->what == ivy_nNAM) {
		*r = n->l;
		return n->r;
	} else if (n->what == ivy_nQUOTE) {
		*r = n;
		return ivy_consempty(n->loc);
	} else {
		*r = NULL;
		return n;
	}
}

/* Generate nothing: the code is executed but with no result left on the stack */

static void genn(Ivy_error_printer *err, Ivy_frag *frag, Ivy_node * n)
{
	switch(n->what) {
		case ivy_nPAREN: {
			genn(err, frag, n->r);
			break;
		} case ivy_nQUOTE: {
			ivy_error_2(err, "\"%s\" %d: `used incorrectly", n->loc->name, n->loc->line);
			break;
		} case ivy_nLABEL: {
			frag->looplvls->name = strdup(n->s);
			break;
		} case ivy_nVAR: {
			int amnt = genll(err, frag, n->r); /* Create variables */
			push_lst(frag);
			ivy_emitn(frag, amnt);
			ivy_emitc(frag, ivy_iLOC);
			fixlooplvl(frag, amnt + 1);
			genla(err, frag, n->r); /* Initialize them */
			break;
		} case ivy_nFOR: {
			int top, cont;
			Ivy_node *name;
			Ivy_node *args = extract_loop_name(n->r, &name);
			if (args->what != ivy_nSEMI) { /* One arg */
				genn(err, frag, args); /* Initializer */
			} else if (args->r->what != ivy_nSEMI) { /* Two args */
				genn(err, frag, args->l); /* Initializer */
				ivy_emitc(frag, ivy_iBRA);
				ivy_emitn(frag, 0);
				push_looplvl(frag, lvlLOOP, frag->code-sizeof(int), 0);
				if (name)
					frag->looplvls->name = strdup(name->r->s);
				top = frag->code;
				cont = frag->code;
				setlist(frag,genbra(err, frag, args->r, 0), top); /* Test */
				pop_looplvl(frag, lvlLOOP, cont, frag->code);
			} else if (args->r->r->what != ivy_nSEMI) { /* Three args */
				genn(err, frag, args->l); /* Initializer */
				ivy_emitc(frag, ivy_iBRA);
				ivy_emitn(frag, 0);
				push_looplvl(frag, lvlLOOP, frag->code-sizeof(int), 0);
				if (name)
					frag->looplvls->name = strdup(name->r->s);
				top = frag->code;
				genn(err, frag, args->r->r); /* Increment */
				cont = frag->code;
				setlist(frag,genbra(err, frag, args->r->l, 0), top); /* Test */
				pop_looplvl(frag, lvlLOOP, cont, frag->code);
			} else { /* Four args */
				genn(err, frag, args->l); /* Initializer */
				ivy_emitc(frag, ivy_iBRA);
				ivy_emitn(frag, 0);
				push_looplvl(frag, lvlLOOP, frag->code-sizeof(int), 0);
				if (name)
					frag->looplvls->name = strdup(name->r->s);
				top = frag->code;
#ifdef AUTOSCOPE
				push_looplvl(frag, lvlSCOPE, 0, 0);
#endif
				genn(err, frag, args->r->r->r); /* Body */
#ifdef AUTOSCOPE
				pop_looplvl(frag, lvlSCOPE, 0, 0);
#endif
				genn(err, frag, args->r->r->l); /* Increment */
				cont = frag->code;
				setlist(frag,genbra(err, frag, args->r->l, 0), top); /* Test */
				pop_looplvl(frag, lvlLOOP, cont, frag->code);
			}
			break;
		} case ivy_nFOREACH: case ivy_nFORINDEX: {
			int top, cont;
			Ivy_node *name;
			Ivy_node *args = extract_loop_name(n->r, &name);
			if (args->what != ivy_nSEMI) { /* One arg */
				ivy_error_2(err,"\"%s\" %d: No args for foreach?", n->loc->name, n->loc->line);
			} else if (args->r->what != ivy_nSEMI) { /* Two args */
				ivy_error_2(err,"\"%s\" %d: Only two args for foreach?", n->loc->name, n->loc->line);
			} else { /* Three args */
				if (args->l->what != ivy_nNAM) {
					ivy_error_2(err, "\"%s\" %d: First arg to foreach must be a variable", n->loc->name, n->loc->line);
				}
#ifdef AUTOSCOPE
				push_looplvl(frag, lvlSCOPE, 0, 0); /* Scope for args */
#endif
				gen(err, frag, args->l);	/* Variable (check that it really is at runtime) */
				gen(err, frag, args->r->l);	/* Array/object */
				push_num(frag);
				ivy_emitl(frag, 0);	/* Temp vars for iFOREACH */
				push_num(frag);
				ivy_emitl(frag, -1);
				ivy_emitc(frag, ivy_iBRA);
				ivy_emitn(frag, 0);
				push_looplvl(frag, lvlLOOP, frag->code-sizeof(int), 0); /* Start loop */
				if (name)
					frag->looplvls->name = strdup(name->r->s);
				top = frag->code;
#ifdef AUTOSCOPE
				push_looplvl(frag, lvlSCOPE, 0, 0); /* Scope for body */
#endif
				genn(err, frag, args->r->r);
#ifdef AUTOSCOPE
				pop_looplvl(frag, lvlSCOPE, 0, 0); /* Body scope done */
#endif
				cont = frag->code;
				if (n->what == ivy_nFOREACH)
					ivy_emitc(frag, ivy_iFOREACH);
				else
					ivy_emitc(frag, ivy_iFORINDEX);
				ivy_align_frag(frag, sizeof(int));
				ivy_emitn(frag, top - (frag->code));
				pop_looplvl(frag, lvlLOOP, cont, frag->code); /* Complete loop */
				ivy_emitc(frag, ivy_iPOP);
				ivy_emitc(frag, ivy_iPOP);
				ivy_emitc(frag, ivy_iPOP);
				ivy_emitc(frag, ivy_iPOP);
				fixlooplvl(frag, 4); /* POP temp vars */
#ifdef AUTOSCOPE
				pop_looplvl(frag, lvlSCOPE, 0, 0); /* POP args scope */
#endif
			}
			break;
		} case ivy_nWHILE: {
			int top, cont;
			Ivy_node *name;
			Ivy_node *args = extract_loop_name(n->r, &name);
			if (args->what==ivy_nEMPTY) {
				ivy_error_2(err,"\"%s\" %d: No args for while", n->loc->name, n->loc->line);
				break;
			}
			ivy_emitc(frag, ivy_iBRA);
			ivy_emitn(frag, 0);
			push_looplvl(frag, lvlLOOP, frag->code-sizeof(int), 0);
			if (name)
				frag->looplvls->name = strdup(name->r->s);
			top = frag->code;
#ifdef AUTOSCOPE
			push_looplvl(frag, lvlSCOPE, 0, 0);
#endif
			if (args->what==ivy_nSEMI)
				genn(err, frag, args->r);
#ifdef AUTOSCOPE
			pop_looplvl(frag, lvlSCOPE, 0, 0);
#endif
			cont = frag->code;
			if (args->what==ivy_nSEMI)
				setlist(frag, genbra(err, frag, args->l, 0), top);
			else
				setlist(frag, genbra(err, frag, args, 0), top);
			pop_looplvl(frag, lvlLOOP, cont, frag->code);
			break;
		} case ivy_nRETURN: {
			int z;
			if (n->r)
				gen(err, frag, n->r);
			else
				push_void(frag);

			pop_looplvl(frag, lvlVALUE, 0, 0);

			if (countvalues(frag, NULL)) {
				ivy_emitc(frag, ivy_iSTASH);
				poploops(frag, NULL);
				ivy_emitc(frag, ivy_iUNSTASH);
			} else {
				poploops(frag, NULL);
			}
			ivy_emitc(frag, ivy_iBRA);
			z = ivy_emitn(frag, 0);
			if (frag->rtn)
				addlist(frag, frag->rtn, z);
			else
				frag->rtn = z;
			break;
		} case ivy_nLOOP: {
			int cont;
			Ivy_node *name;
			Ivy_node *args = extract_loop_name(n->r, &name);
			cont = frag->code;
			push_looplvl(frag, lvlLOOP, 0, 0);
			if (name)
				frag->looplvls->name = strdup(name->r->s);
#ifdef AUTOSCOPE
			push_looplvl(frag, lvlSCOPE, 0, 0);
#endif
			genn(err, frag, args);
#ifdef AUTOSCOPE
			pop_looplvl(frag, lvlSCOPE, 0, 0);
#endif
			ivy_emitc(frag, ivy_iBRA);
			ivy_align_frag(frag, sizeof(int));
			ivy_emitn(frag, cont - (frag->code));
			pop_looplvl(frag, lvlLOOP, cont, frag->code);
			break;
		} case ivy_nBREAK: {
			struct ivy_looplvl *ll = findlvl(frag, NULL);
			if (n->r) {
				// printf("looking... %s %p %p\n",n->r->s,frag,ll);
				if (n->r->what == ivy_nNAM)
					ll = findlvl(frag, n->r->s);
				else if (n->r->what != ivy_nEMPTY)
					ivy_error_2(err, "\"%s\" %d: Invalid argument to break", n->r->loc->name, n->r->loc->line);
			}
			if (ll) {
				int z;
				// printf("break %d %d\n", ll->scopelvl, frag->scopelvl);
				poploops(frag, ll);
				ivy_emitc(frag, ivy_iBRA);
				z = ivy_emitn(frag, 0);
				if (ll->brk)
					addlist(frag, ll->brk, z);
				else
					ll->brk = z;
			} else
				ivy_error_2(err, "\"%s\" %d: break with no loop", n->loc->name, n->loc->line);
			break;
		} case ivy_nCONT: {
			struct ivy_looplvl *ll = findlvl(frag, NULL);
			if (n->r) {
				if (n->r->what == ivy_nNAM)
					ll = findlvl(frag, n->r->s);
				else if (n->r->what != ivy_nEMPTY)
					ivy_error_2(err, "\"%s\" %d: Invalid argument to continue", n->r->loc->name, n->r->loc->line);
			}
			if (ll) {
				int z;
				poploops(frag, ll);
				ivy_emitc(frag, ivy_iBRA);
				z = ivy_emitn(frag, 0);
				if (ll->cont)
					addlist(frag, ll->cont, z);
				else
					ll->cont = z;
			} else
				ivy_error_2(err, "\"%s\" %d: continue with no loop", n->loc->name, n->loc->line);
			break;
		} case ivy_nUNTIL: {
			int els = genbra(err, frag, n->r, 1);
			struct ivy_looplvl *ll = findlvl(frag, NULL);
			if (ll) {
				int z;
				// printf("break %d %d\n", ll->scopelvl, frag->scopelvl);
				poploops(frag, ll);
				ivy_emitc(frag, ivy_iBRA);
				z = ivy_emitn(frag, 0);
				if (ll->brk)
					addlist(frag, ll->brk, z);
				else
					ll->brk = z;
			} else {
				ivy_error_2(err, "\"%s\" %d: until with no loop", n->loc->name, n->loc->line);
			}
			setlist(frag, els, frag->code);
			break;
		} case ivy_nIF: {
			gencond(err, frag, n->r, 0);
			break;
		} case ivy_nSEMI: {
			genn(err, frag, n->l);
			genn(err, frag, n->r);
			break;
		} case ivy_nEMPTY: {
			break;
		} default: {
			gen(err, frag, n);
			ivy_emitc(frag, ivy_iPOP);
			pop_looplvl(frag, lvlVALUE, 0, 0);
			break;
		}
	}
}

/* Generate a branch. If 't' is set, then the block falls through if the result
 * is true and takes the branch if the result is false.
 *
 * If 't' is clear, then the block falls through if the result is false and
 * takes the branch if the result is true.
 *
 * Return the address of the value which should be set to the branch offset
 */

static int genbra(Ivy_error_printer *err, Ivy_frag *frag, Ivy_node * n, int t)
{
	switch(n->what) {
		case ivy_nEQ: {
			gen(err, frag, n->l);
			gen(err, frag, n->r);
			ivy_emitc(frag, ivy_iCMP);
			pop_looplvl(frag, lvlVALUE, 0, 0);
			if (t)
				ivy_emitc(frag, ivy_iBNE);
			else
				ivy_emitc(frag, ivy_iBEQ);
			pop_looplvl(frag, lvlVALUE, 0, 0);
			return ivy_emitn(frag, 0);
		} case ivy_nNE: {
			gen(err, frag, n->l);
			gen(err, frag, n->r);
			ivy_emitc(frag, ivy_iCMP);
			pop_looplvl(frag, lvlVALUE, 0, 0);
			if (t)
				ivy_emitc(frag, ivy_iBEQ);
			else
				ivy_emitc(frag, ivy_iBNE);
			pop_looplvl(frag, lvlVALUE, 0, 0);
			return ivy_emitn(frag, 0);
		} case ivy_nGT: {
			gen(err, frag, n->l);
			gen(err, frag, n->r);
			ivy_emitc(frag, ivy_iCMP);
			pop_looplvl(frag, lvlVALUE, 0, 0);
			if (t)
				ivy_emitc(frag, ivy_iBLE);
			else
				ivy_emitc(frag, ivy_iBGT);
			pop_looplvl(frag, lvlVALUE, 0, 0);
			return ivy_emitn(frag, 0);
		} case ivy_nGE: {
			gen(err, frag, n->l);
			gen(err, frag, n->r);
			ivy_emitc(frag, ivy_iCMP);
			pop_looplvl(frag, lvlVALUE, 0, 0);
			if (t)
				ivy_emitc(frag, ivy_iBLT);
			else
				ivy_emitc(frag, ivy_iBGE);
			pop_looplvl(frag, lvlVALUE, 0, 0);
			return ivy_emitn(frag, 0);
		} case ivy_nLT: {
			gen(err, frag, n->l);
			gen(err, frag, n->r);
			ivy_emitc(frag, ivy_iCMP);
			pop_looplvl(frag, lvlVALUE, 0, 0);
			if (t)
				ivy_emitc(frag, ivy_iBGE);
			else
				ivy_emitc(frag, ivy_iBLT);
			pop_looplvl(frag, lvlVALUE, 0, 0);
			return ivy_emitn(frag, 0);
		} case ivy_nLE: {
			gen(err, frag, n->l);
			gen(err, frag, n->r);
			ivy_emitc(frag, ivy_iCMP);
			pop_looplvl(frag, lvlVALUE, 0, 0);
			if (t)
				ivy_emitc(frag, ivy_iBGT);
			else
				ivy_emitc(frag, ivy_iBLE);
			pop_looplvl(frag, lvlVALUE, 0, 0);
			return ivy_emitn(frag, 0);
		} case ivy_nNOT: {
			return genbra(err, frag, n->r, !t);
		} case ivy_nLAND: {
			int b1 = genbra(err, frag, n->l, 1);
			int b2;
			if (t)
				b2 = genbra(err, frag, n->r, 1), addlist(frag, b2, b1);
			else
				b2 = genbra(err, frag, n->r, 0), setlist(frag, b1, frag->code);
			return b2;
		} case ivy_nLOR: {
			int b1 = genbra(err, frag, n->l, 0);
			int b2;
			if (t)
				b2 = genbra(err, frag, n->r, 1), setlist(frag, b1, frag->code);
			else
				b2 = genbra(err, frag, n->r, 0), addlist(frag, b2, b1);
			return b2;
		} default: {
			gen(err, frag, n);
			if (t)
				ivy_emitc(frag, ivy_iBEQ);
			else
				ivy_emitc(frag, ivy_iBNE);
			pop_looplvl(frag, lvlVALUE, 0, 0);
			return ivy_emitn(frag, 0);
		}
	}
}

/* Code generator: convert a parse-tree into pseudo-machine code */

Ivy_pseudo *ivy_codegen(Ivy_error_printer *err, Ivy_node *n)
{
	Ivy_frag frag[1];

	ivy_setup_frag(frag);

	gen(err, frag, n);

	//ivy_emitc(frag, ivy_iSTASH);
	//pop_looplvl(frag, lvlVALUE, 0, 0);

	if (frag->rtn)
		setlist(frag, frag->rtn, frag->code);

	ivy_emitc(frag, ivy_iRTS);
	pop_looplvl(frag, lvlVALUE, 0, 0);

	if (frag->looplvls) {
		printf("Expected no looplvls at end of code\n");
		printf("But we have this:\n");
		show_looplvls(frag);
		// abort();
	}

	return frag->begcode;
}
