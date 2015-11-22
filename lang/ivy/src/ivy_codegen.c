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
#include "ivy_tree.h"
#include "ivy_frag.h"
#include "ivy.h"

Func *mkfunc(Pseudo * code, int nargs, char **args, Pseudo ** inits, char *quote)
{
	Func *func = (Func *) malloc(sizeof(Func));
	func->code = code;
	func->nargs = nargs;
	func->args = args;
	func->inits = inits;
	func->quote = quote;
	func->cfunc = 0;
	return func;
}

/* Generate a branch. If 't' is set, then the block falls through if the result
 * is true and takes the branch if the result is false.
 *
 * If 't' is clear, then the block falls through if the result is false and
 * takes the branch if the reuslt is true.
 *
 * Return the address of the value which should be set to the branch offset
 */

/* Add branch list 'b' to end of branch list 'a' */

static void addlist(Frag *frag, int a, int b)
{
	while (fragn(frag, a))
		a = fragn(frag, a);
	fragn(frag, a) = b;
}

/* Set each value in branch list 'a' to destination 'b' */

static void setlist(Frag *frag, int a, int b)
{
	while (a) {
		int c = fragn(frag, a);
		fragn(frag, a) = b - a;
		a = c;
	}
}

/* Track contents of the stack.  Also track loops.  */

#define lvlSCOPE 0
#define lvlLOOP 1
#define lvlVALUE 2

struct looplvl {
	struct looplvl *next;	/* Next level */
	int what;		/* 2 = value, 1 = loop, 0 = scope */
	int cont;		/* List of continue destinations */
	int brk;		/* List of brk destinations */
	char *name;		/* Named level */
};

void mklooplvl(Frag *frag, int what, int cont, int brk)
{
	struct looplvl *ll =
	    (struct looplvl *) malloc(sizeof(struct looplvl));
	ll->what = what;
	ll->cont = cont;
	ll->brk = brk;
	ll->next = frag->looplvls;
	ll->name = 0;
	frag->looplvls = ll;
	if (ll->what == lvlSCOPE)
		emitc(frag, iBEG);
}

void rmlooplvl(Frag *frag, int what, int cont, int brk)
{
	struct looplvl *ll = frag->looplvls;
	frag->looplvls = ll->next;
	if (what != ll->what) {
		printf("Expected level of type %d\n", what);
		printf("But we have this:\n");
		while (ll) {
			printf("  %d\n", ll->what);
			ll = ll->next;
		}
		abort();
	}
	if (ll->what == lvlLOOP) {
		setlist(frag, ll->cont, cont);
		setlist(frag, ll->brk, brk);
	}
	if (ll->what == lvlSCOPE)
		emitc(frag, iEND);
	if (ll->name)
		free(ll->name);
	free(ll);
}

/* Find named loop, or with NULL, find innermost loop */

struct looplvl *findlvl(Frag *frag, char *name)
{
	struct looplvl *ll;
	if (name)
		for (ll = frag->looplvls; ll && (!ll->name || strcmp(ll->name, name)); ll = ll->next);
	else
		for (ll = frag->looplvls; ll && ll->what != lvlLOOP; ll = ll->next);
	return ll;
}

/* Convert stack list to object */

void fixlooplvl(Frag *frag, int amnt)
{
	int x;
	for (x = 0; x != amnt; ++x)
		rmlooplvl(frag, lvlVALUE, 0, 0);
}

/* Pop for premature exit */

void poploops(Frag *frag, struct looplvl *target)
{
	struct looplvl *ll;
	for (ll = frag->looplvls; ll && ll != target; ll = ll->next) {
		if (ll->what == lvlSCOPE)
			emitc(frag, iEND);
		else if (ll->what == lvlVALUE)
			emitc(frag, iPOP);
	}
}

/* Push something */

void push_str(Frag *frag)
{
	mklooplvl(frag, lvlVALUE, 0, 0);
	emitc(frag, iPSH_STR);
}

void push_nam(Frag *frag)
{
	mklooplvl(frag, lvlVALUE, 0, 0);
	emitc(frag, iPSH_NAM);
}

void push_narg(Frag *frag)
{
	// mklooplvl(frag, lvlVALUE, 0, 0); /* It's counted with the value */
	emitc(frag, iPSH_NARG);
}

void push_func(Frag *frag)
{
	mklooplvl(frag, lvlVALUE, 0, 0);
	emitc(frag, iPSH_FUNC);
}

void push_lst(Frag *frag)
{
	mklooplvl(frag, lvlVALUE, 0, 0);
	emitc(frag, iPSH_LST);
}

void push_void(Frag *frag)
{
	mklooplvl(frag, lvlVALUE, 0, 0);
	emitc(frag, iPSH_VOID);
}

void push_this(Frag *frag)
{
	mklooplvl(frag, lvlVALUE, 0, 0);
	emitc(frag, iPSH_THIS);
}

void push_num(Frag *frag)
{
	mklooplvl(frag, lvlVALUE, 0, 0);
	emitc(frag, iPSH_NUM);
}

void push_fp(Frag *frag)
{
	mklooplvl(frag, lvlVALUE, 0, 0);
	emitc(frag, iPSH_FP);
}

/* Disassembler */

void disasm(FILE *out, Pseudo * c, int ind, int oneline)
{
	Pseudo *start = c;
	if (ind < 0) {
		fprintf(out, "Bad indent value\n");
		return;
	}
	indent(out, ind); fprintf(out, "%p:\n", start);
	for (;;) {
		indent(out, ind); fprintf(out, "%d ", (int)(c - start));
		switch (*c++) {
			case iBRA: {
				c += align_o(c, sizeof(int));
				indent(out, ind); fprintf(out, "	bra %d\n", *(int *)c + (int)(c - start));
				c += sizeof(int);
				break;
			} case iBEQ: {
				c += align_o(c, sizeof(int));
				indent(out, ind); fprintf(out, "	beq %d\n", *(int *)c + (int)(c - start));
				c += sizeof(int);
				break;
			} case iBNE: {
				c += align_o(c, sizeof(int));
				indent(out, ind); fprintf(out, "	bne %d\n", *(int *)c + (int)(c - start));
				c += sizeof(int);
				break;
			} case iBGT: {
				c += align_o(c, sizeof(int));
				indent(out, ind); fprintf(out, "	bgt %d\n", *(int *)c + (int)(c - start));
				c += sizeof(int);
				break;
			} case iBLT: {
				c += align_o(c, sizeof(int));
				indent(out, ind); fprintf(out, "	blt %d\n", *(int *)c + (int)(c - start));
				c += sizeof(int);
				break;
			} case iBGE: {
				c += align_o(c, sizeof(int));
				indent(out, ind); fprintf(out, "	bge %d\n", *(int *)c + (int)(c - start));
				c += sizeof(int);
				break;
			} case iBLE: {
				c += align_o(c, sizeof(int));
				indent(out, ind); fprintf(out, "	ble %d\n", *(int *)c + (int)(c - start));
				c += sizeof(int);
				break;
			} case iCOM: {
				indent(out, ind); fprintf(out, "	com\n");
				break;
			} case iNEG: {
				indent(out, ind); fprintf(out, "	neg\n");
				break;
			} case iSHL: {
				indent(out, ind); fprintf(out, "	shl\n");
				break;
			} case iSHR: {
				indent(out, ind); fprintf(out, "	shr\n");
				break;
			} case iMUL: {
				indent(out, ind); fprintf(out, "	mul\n");
				break;
			} case iDIV: {
				indent(out, ind); fprintf(out, "	div\n");
				break;
			} case iMOD: {
				indent(out, ind); fprintf(out, "	mod\n");
				break;
			} case iAND: {
				indent(out, ind); fprintf(out, "	and\n");
				break;
			} case iADD: {
				indent(out, ind); fprintf(out, "	add\n");
				break;
			} case iSUB: {
				indent(out, ind); fprintf(out, "	sub\n");
				break;
			} case iOR: {
				indent(out, ind); fprintf(out, "	or\n");
				break;
			} case iXOR: {
				indent(out, ind); fprintf(out, "	xor\n");
				break;
			} case iCMP: {
				indent(out, ind); fprintf(out, "	cmp\n");
				break;
			} case iBEG: {
				indent(out, ind); fprintf(out, "	beg\n");
				break;
			} case iEND: {
				indent(out, ind); fprintf(out, "	end\n");
				break;
			} case iLOC: {
				indent(out, ind); fprintf(out, "	loc\n");
				break;
			} case iGET: {
				indent(out, ind); fprintf(out, "	get\n");
				break;
			} case iGET_ATOM: {
				indent(out, ind); fprintf(out, "	get atom\n");
				break;
			} case iGETF: {
				indent(out, ind); fprintf(out, "	getf\n");
				break;
			} case iGETF_ATOM: {
				indent(out, ind); fprintf(out, "	getf atom\n");
				break;
			} case iAT: {
				indent(out, ind); fprintf(out, "	at\n");
				break;
			} case iSET: {
				indent(out, ind); fprintf(out, "	set\n");
				break;
			} case iCALL: {
				indent(out, ind); fprintf(out, "	call\n");
				break;
			} case iRTS: {
				indent(out, ind); fprintf(out, "	rts\n");
				return;
			} case iSTASH: {
				indent(out, ind); fprintf(out, "	stash\n");
				break;
			} case iPOP: {
				indent(out, ind); fprintf(out, "	pop\n");
				break;
			} case iFOREACH: {
				c += align_o(c, sizeof(int));
				indent(out, ind); fprintf(out, "	foreach %d\n", *(int *)c + (int)(c - start));
				c += sizeof(int);
				break;
			} case iFORINDEX: {
				c += align_o(c, sizeof(int));
				indent(out, ind); fprintf(out, "	forindex %d\n", *(int *)c + (int)(c - start));
				c += sizeof(int);
				break;
			} case iFIX: {
				indent(out, ind); fprintf(out, "	fix\n");
				break;
			} case iPSH_NUM: {
				c += align_o(c, sizeof(long long));
				indent(out, ind); fprintf(out, "	psh_num %lld\n", *(long long *)c);
				c += sizeof(long long);
				break;
			} case iPSH_FP: {
				c += align_o(c, sizeof(double));
				indent(out, ind); fprintf(out, "	psh_fp %g\n", *(double *)c);
				c += sizeof(double);
				break;
			} case iPSH_NARG: {
				c += align_o(c, sizeof(int));
				indent(out, ind); fprintf(out, "	psh_narg \"%s\"\n", c + sizeof(int));
				c += sizeof(int) + *(int *)c + 1;
				break;
			} case iPSH_STR: {
				c += align_o(c, sizeof(int));
				indent(out, ind); fprintf(out, "	psh_str \"%s\"\n", c + sizeof(int));
				c += sizeof(int) + *(int *)c + 1;
				break;
			} case iPSH_NAM: {
				c += align_o(c, sizeof(char *));
				indent(out, ind); fprintf(out, "	psh_nam %s\n", *(char **)c);
				c += sizeof(char *);
				break;
			} case iPSH_VOID: {
				indent(out, ind); fprintf(out, "	psh_void\n");
				break;
			} case iPSH_THIS: {
				indent(out, ind); fprintf(out, "	psh_this\n");
				break;
			} case iPSH_LST: {
				c += align_o(c, sizeof(int));
				indent(out, ind); fprintf(out, "	psh_lst %d\n", *(int *)c);
				c += sizeof(int);
				break;
			} case iPSH_FUNC: {
				c += align_o(c, sizeof(void *));
				indent(out, ind); fprintf(out, "	psh_func %p\n", *(void **)c);
				disasm(out, ((Func *)*(void **)c)->code, ind + 4, 0);
				c += sizeof(void *);
				break;
			}
		}
		if (oneline)
			break;
	}
}

static int genl(Error_printer *, Frag *, Node *);
static int genbra(Error_printer *, Frag *, Node *, int);
static void gen(Error_printer *, Frag *, Node *);
static void genn(Error_printer *, Frag *, Node *);

/* Count no. of comma seperated elements.  Use for args and 1st expr of FOR */

int cntlst(Node * n)
{
	switch(n->what) {
		case nEMPTY: {
			return 0;
		} case nCALL: case nCOMMA: case nSEMI: {
			return cntlst(n->l) + cntlst(n->r);
		} case nPAREN: {
			return cntlst(n->r);
		} default: {
			return 1;
		}
	}
}

/* Generate list of comma seperated names (for function args) */

int genlst(Error_printer *err, char **argv, Pseudo ** initv, char *quote, Node * n)
{
	switch (n->what) {
		case nEMPTY: {
			return 0;
		} case nADDR: {
			quote[0] = 1;
			return genlst(err, argv, initv, quote, n->r);
		} case nPAREN: {
			return genlst(err, argv, initv, quote, n->r);
		} case nSEMI: case nCOMMA: case nCALL: {
			int x = genlst(err, argv, initv, quote, n->l);
			return x + genlst(err, argv + x, initv + x, quote + x, n->r);
/*		} case nCALL: {
			if (n->l->what == nNAM && n->r->what == nEMPTY)
				return argv[0] = strdup(n->l->s), initv[0] = 0, 1;
			break;
*/
		} case nNAM: {
			return argv[0] = strdup(n->s), initv[0] = 0, 1;
		} case nSET: {
			if (n->l->what == nNAM)
				return argv[0] = strdup(n->l->s), initv[0] = codegen(err, n->r), 1;
			else if (n->l->what == nADDR && n->l->r->what == nNAM) {
				quote[0] = 1;
				return argv[0] = strdup(n->l->r->s), initv[0] = codegen(err, n->r), 1;
			}
			break;
		}
	}
	error_3(err, "\"%s\" %d: incorrect argument list %s", n->loc->name, n->loc->line, what_tab[n->what].name);
	return 0;
}

/* Generate and count list (used to generate arg lists) */

static int genl(Error_printer *err, Frag *frag, Node * n)
{
	int result;
	switch (n->what) {
		case nEMPTY: {
			return 0;
		} case nSEMI: {
			result = genl(err, frag, n->r);
			return result + genl(err, frag, n->l);
		} case nSET: {
			if (n->l->what == nQUOTE && (n->l->r->what == nNAM || n->l->r->what == nSTR)) {
				gen(err, frag, n->r);
				push_narg(frag);
				emits(frag, n->l->r->s, n->l->r->n);
				return 1;
			}
			break;
		}
	}
	gen(err, frag, n);
	return 1;
}

/* Generate a function */

void genfunc(Error_printer *err, Frag *frag, Node * args, Node * body)
{
	Pseudo *cod = codegen(err, body);
	Func *o;
	int argc = cntlst(args);
	char *quote;
	/* FIXME: argc can be zero */
	char **argv = (char **) malloc(argc * sizeof(char *));
	Pseudo **initv = (Pseudo **)malloc(argc * sizeof(Pseudo *));
	quote = (char *)calloc(argc,1);
	genlst(err, argv, initv, quote, args);
	o = mkfunc(cod, argc, argv, initv, quote);
	push_func(frag);
	emitp(frag, o);
}

/* Generate and count list (used to generate arg lists) */

static int gencl(Error_printer *err, Frag *frag, Node * n)
{
	int result;
	switch (n->what) {
		case nEMPTY: {
			return 0;
		} case nSEMI: {
			result = gencl(err, frag, n->r);
			return result + gencl(err, frag, n->l);
		} case nSET: {
			if (n->l->what == nQUOTE && (n->l->r->what == nNAM || n->l->r->what == nSTR)) {
				genfunc(err, frag, consempty(n->loc), n->r);
				push_narg(frag);
				emits(frag, n->l->r->s, n->l->r->n);
				return 1;
			}
			break;
		}
	}
	genfunc(err, frag, consempty(n->loc), n);
	return 1;
}

/* Generate and count local command, don't emit initializers */

static int genll(Error_printer *err, Frag *frag, Node * n)
{
	switch (n->what) {
		case nEMPTY: {
			return 0;
		} case nSEMI: {
			int result = genll(err, frag, n->r);
			return result + genll(err, frag, n->l);
		} case nNAM: {
			push_str(frag);
			emits(frag, n->s, n->n);
			return 1;
		} case nSET: {
			if (n->l->what == nNAM) {
				push_str(frag);
				emits(frag, n->l->s, n->l->n);
				return 1;
			}
		}
	}
	error_2(err, "\"%s\" %d: incorrect local list", n->loc->name, n->loc->line);
	return 0;
}

/* Generate initializers for local list */

static int genla(Error_printer *err, Frag *frag, Node * n)
{
	switch (n->what) {
		case nEMPTY: {
			return 0;
		} case nSEMI: {
			int result = genla(err, frag, n->r);
			return result + genla(err, frag, n->l);
		} case nNAM: {
			return 1;
		} case nSET: {
			if (n->l->what == nNAM) {
				genn(err, frag, n);
				return 1;
			}
		}
	}
	error_2(err, "\"%s\" %d: incorrect local list", n->loc->name, n->loc->line);
	return 0;
}

/* Generate if..elif..elif.. with no return value */
/* Returns list of branch offsets which should be set past final else */

int genelif(Error_printer *err, Frag *frag, Node * n, int v)
{
	switch (n->what) {
		case nEMPTY:
			return 0;
		case nIF: {
			int els = genbra(err, frag, n->l, 1);
			int rtval;
			mklooplvl(frag, lvlSCOPE, 0, 0);
			if (v)
				gen(err, frag, n->r);
			else
				genn(err, frag, n->r);
			rmlooplvl(frag, lvlSCOPE, 0, 0);
			emitc(frag, iBRA);
			rtval = emitn(frag, 0);
			setlist(frag, els, frag->code);
			return rtval;
		} case nSEMI: {
			if (n->r->what == nELSE && n->r->r->what == nIF) {
				int z = genelif(err, frag, n->l, v);
				if (!z)
					error_2(err, "\"%s\" %d: else without if", n->r->loc->name, n->r->loc->line);
				else
					addlist(frag, z, genelif(err, frag, n->r->r, v));
				return z;
			}
		}
	}
	if (v)
		gen(err, frag, n);
	else
		genn(err, frag, n);
	return 0;
}


/* Generate cond */

void gencond(Error_printer *err, Frag *frag, Node *n, int v)
{
	Node *f, *r;
	int end = 0;

	loop:	

	f = first(n);
	r = first(rest(n));
	n = rest(rest(n));

	if (f && r) { /* elif */
		int els = genbra(err, frag, f, 1); /* Branch if false */
		mklooplvl(frag, lvlSCOPE, 0, 0);
		if (v) {
			gen(err, frag, r);
			/* Value is still here, but we need to pop the lvlSCOPE */
			/* Value is put back at the branch target point: XXX */
			rmlooplvl(frag, lvlVALUE, 0, 0);
		} else
			genn(err, frag, r);
		rmlooplvl(frag, lvlSCOPE, 0, 0);
		emitc(frag, iBRA);
		if (end)
			addlist(frag, end, emitn(frag, 0));
		else
			end = emitn(frag, 0);
		/* Value is gone after the branch */
		setlist(frag, els, frag->code);
		goto loop;
	} else if (f) { /* else */
		mklooplvl(frag, lvlSCOPE, 0, 0);
		if (v) {
			gen(err, frag, f);
			rmlooplvl(frag, lvlVALUE, 0, 0);
		} else
			genn(err, frag, f);
		rmlooplvl(frag, lvlSCOPE, 0, 0);
		if (v) {
			mklooplvl(frag, lvlVALUE, 0, 0); /* here XXX */
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


/* Generate a value */

static void gen(Error_printer *err, Frag *frag, Node * n)
{
	switch(n->what) {
		case nCOMMA: {
			genn(err, frag, n->l), gen(err, frag, n->r);
			break;
		} case nPAREN: {
			gen(err, frag, n->r);
			break;
		} case nLIST: {
			int amnt = genl(err, frag, n->r);
			push_lst(frag);
			emitn(frag, amnt);
			emitc(frag, iFIX);
			fixlooplvl(frag, amnt);
			break;
		} case nVOID: {
			push_void(frag);
			break;
		} case nTHIS: {
			push_this(frag);
			break;
		} case nNUM: {
			push_num(frag);
			emitl(frag, n->n);
			break;
		} case nFP: {
			push_fp(frag);
			emitd(frag, n->fp);
			break;
		} case nSTR: {
			push_str(frag);
			emits(frag, n->s, n->n);
			break;
		} case nNAM: {
			push_nam(frag);
			emitp(frag, n->s);
			emitc(frag, iGET_ATOM);
			break;
		} case nSET: {
			gen(err, frag, n->r);
//			gena(err, frag, n->l);  (lvalue change)
			gen(err, frag, n->l);
			emitc(frag, iSET);
			rmlooplvl(frag, lvlVALUE, 0, 0);
			break;
		} case nIF: {
			gencond(err, frag, n->r, 1);
			break;
		} case nPOST: {
			gen(err, frag, n->l);
			gen(err, frag, n->r);
//			gena(err, frag, n->l); (lvalue change)
			gen(err, frag, n->l);
			emitc(frag, iSET);
			rmlooplvl(frag, lvlVALUE, 0, 0);
			emitc(frag, iPOP);
			rmlooplvl(frag, lvlVALUE, 0, 0);
			break;
		} case nADDR: {
			/* Generate a code snippet */
			genfunc(err, frag, consempty(n->loc), n->r);
			break;
		} case nDEFUN: {
			if (n->r->what==nSEMI) {
				if (n->r->l->what==nCALL && n->r->l->l->what==nNAM) { /* fn sq(x) x*x */
					genfunc(err, frag, n->r->l->r, n->r->r);
					push_nam(frag);
					emitp(frag, n->r->l->l->s);
					emitc(frag, iGETF_ATOM);
					emitc(frag, iSET);
					rmlooplvl(frag, lvlVALUE, 0, 0);
				} else if (n->r->l->what==nNAM && n->r->r->what==nSEMI && n->r->r->l->what==nPAREN) { /* fn sq (x) x*x */
					genfunc(err, frag, n->r->r->l, n->r->r->r);
					push_nam(frag);
					emitp(frag, n->r->l->s);
					emitc(frag, iGETF_ATOM);
					emitc(frag, iSET);
					rmlooplvl(frag, lvlVALUE, 0, 0);
				} else if (n->r->l->what==nNAM && n->r->r->what==nPAREN) { /* fn sq (x) */
					genfunc(err, frag, n->r->r, consempty(n->loc));
					push_nam(frag);
					emitp(frag, n->r->l->s);
					emitc(frag, iGETF_ATOM);
					emitc(frag, iSET);
					rmlooplvl(frag, lvlVALUE, 0, 0);
				} else if (n->r->l->what==nPAREN) { /* fn (x) x*x */
					genfunc(err, frag, n->r->l, n->r->r);
				} else {
					error_2(err, "\"%s\" %d: ill-formed fn", n->r->loc->name, n->r->loc->line);
					push_void(frag);
				}
			} else if(n->r->what==nCALL && n->r->l->what==nNAM) { /* fn sq(x) */
				genfunc(err, frag, n->r->r, consempty(n->loc));
				push_nam(frag);
				emitp(frag, n->r->l->s);
				emitc(frag, iGETF_ATOM);
				emitc(frag, iSET);
				rmlooplvl(frag, lvlVALUE, 0, 0);
			} else if(n->r->what==nPAREN) { /* fn () */
				genfunc(err,frag, n->r, consempty(n->loc));
			} else {
				error_2(err, "\"%s\" %d: ill-formed fn", n->r->loc->name, n->r->loc->line);
				push_void(frag);
			}
			break;
		} case nLAMBDA: {
			genfunc(err, frag, n->r->l, n->r->r);
			break;
		} case nSEMI: {
			if (n->r->what == nELSE) {
				int done = genelif(err, frag, n->l, 1);
				if (!done)
					error_2(err, "\"%s\" %d: else w/o if error", n->r->loc->name, n->r->loc->line);
				if (n->r->r->what == nIF) {
					addlist(frag, done, genbra(err, frag, n->r->r->l, 1));
					n = n->r;
				}
				mklooplvl(frag, lvlSCOPE, 0, 0);
				gen(err, frag, n->r->r);
				rmlooplvl(frag, lvlSCOPE, 0, 0);
				setlist(frag, done, frag->code);
			} else {
				genn(err, frag, n->l);
				gen(err, frag, n->r);
			}
			break;
		} case nEQ: case nNE: case nGT: case nLT: case nGE: case nLE: case nLAND: case nLOR: case nNOT: {
			int b = genbra(err, frag, n, 1);
			int link;
			push_num(frag);
			emitl(frag, 1);
			emitc(frag, iBRA);
			link=emitn(frag, 0);
			setlist(frag, b, frag->code);
			push_num(frag);
			emitl(frag, 0);
			*(int *)(frag->begcode+link)=frag->code-link;
			break;
		} case nCALL: {
//			int nargs = genl(err, frag, n->r); /* By value */
			int nargs = gencl(err, frag, n->r); /* Functionalize */
			push_lst(frag);
			emitn(frag, nargs);
//			gena(err, frag, n->l); (lvalue change)
			gen(err, frag, n->l);
			emitc(frag, iCALL);
			fixlooplvl(frag, nargs + 1);
			break;
		} case nCALL1: { /* Ends up being the same as above */
//			if (n->r->what != nNAM)
//				error_2(err, "\"%s\" %d: Invalid member name", n->r->loc->name, n->r->loc->line);
			if (n->r->what == nNAM) { /* Turn it into a string .x -> ."x" */
				n->r->what = nSTR;
			}
			int nargs = gencl(err, frag, n->r);
//			push_str(frag);
//			emits(frag, n->r->s, n->r->n);
			push_lst(frag);
			emitn(frag, nargs);
//			gena(err, frag, n->l); (lvalue change)
			gen(err, frag, n->l);
			emitc(frag, iCALL);
			fixlooplvl(frag, nargs + 1);
			break;
		} case nCOM: case nNEG: case nSHL: case nSHR: case nMUL: case nDIV: case nMOD: case nAND:
		  case nADD: case nSUB: case nOR: case nXOR: case nAT: {
			if (n->l)
				gen(err, frag, n->l);
			if (n->r)
				gen(err, frag, n->r);
			emitc(frag, what_tab[n->what].i);
			if (n->r && n->l)
				rmlooplvl(frag, lvlVALUE, 0, 0);
			break;
		} default: {
			genn(err, frag, n);
			push_void(frag);
		}
	}
}

/* Generate nothing (returns true if guarenteed to branch) */

static int last_is_paren(Node *n)
{
	if (n->what == nPAREN)
		return 1;
	else if (n->what == nSEMI)
		return last_is_paren(n->r);
	else
		return 0;
}

static Node *extract_last_is_paren(Node *n, Node **r)
{
	if (n->what == nPAREN) {
		*r = n;
		return consempty(n->loc);
	} else if (n->what == nSEMI) {
		n->r = extract_last_is_paren(n->r, r);
		return n;
	} else {
		return n;
	}
}

static Node *extract_loop_name(Node *n, Node **r)
{
	if (n->what == nSEMI && n->l->what == nQUOTE && n->l->r->what == nNAM) {
		*r = n->l;
		return n->r;
	} else if (n->what == nQUOTE) {
		*r = n;
		return consempty(n->loc);
	} else {
		*r = NULL;
		return n;
	}
}

static void genn(Error_printer *err, Frag *frag, Node * n)
{
	switch(n->what) {
		case nPAREN: {
			genn(err, frag, n->r);
			break;
		} case nQUOTE: {
			error_2(err, "\"%s\" %d: `used incorrectly", n->loc->name, n->loc->line);
			break;
		} case nLABEL: {
			frag->looplvls->name = strdup(n->s);
			break;
		} case nLOCAL: {
			if (n->r->what == nSEMI && n->r->l->what == nPAREN) {
				int amnt;
				mklooplvl(frag, lvlSCOPE, 0, 0);
				amnt = genll(err, frag, n->r->l->r);
				push_lst(frag);
				emitn(frag, amnt);
				emitc(frag, iLOC);
				fixlooplvl(frag, amnt + 1);
				genla(err, frag, n->r->l->r);
				genn(err, frag, n->r->r);
				rmlooplvl(frag, lvlSCOPE, 0, 0);
			} else if (n->r->what == nSEMI && last_is_paren(n->r)) {
				// Handles with a b [f]
				Node *r;
				int amnt;
				n->r = extract_last_is_paren(n->r, &r);
				mklooplvl(frag, lvlSCOPE, 0, 0);
				amnt = genll(err, frag, n->r);
				push_lst(frag);
				emitn(frag, amnt);
				emitc(frag, iLOC);
				fixlooplvl(frag, amnt + 1);
				genla(err, frag, n->r);
				genn(err, frag, r);
				rmlooplvl(frag, lvlSCOPE, 0, 0);
			} else {
				int amnt = genll(err, frag, n->r); /* Create variables */
				push_lst(frag);
				emitn(frag, amnt);
				emitc(frag, iLOC);
				fixlooplvl(frag, amnt + 1);
				genla(err, frag, n->r); /* Initialize them */
			}
			break;
		} case nFOR: {
			int top, cont;
			Node *name;
			Node *args = extract_loop_name(n->r, &name);
			if (args->what != nSEMI) { /* One arg */
				genn(err, frag, args); /* Initializer */
			} else if (args->r->what != nSEMI) { /* Two args */
				genn(err, frag, args->l); /* Initializer */
				emitc(frag, iBRA);
				emitn(frag, 0);
				mklooplvl(frag, lvlLOOP, frag->code-sizeof(int), 0);
				if (name)
					frag->looplvls->name = strdup(name->r->s);
				top = frag->code;
				cont = frag->code;
				setlist(frag,genbra(err, frag, args->r, 0), top); /* Test */
				rmlooplvl(frag, lvlLOOP, cont, frag->code);
			} else if (args->r->r->what != nSEMI) { /* Three args */
				genn(err, frag, args->l); /* Initializer */
				emitc(frag, iBRA);
				emitn(frag, 0);
				mklooplvl(frag, lvlLOOP, frag->code-sizeof(int), 0);
				if (name)
					frag->looplvls->name = strdup(name->r->s);
				top = frag->code;
				genn(err, frag, args->r->r); /* Increment */
				cont = frag->code;
				setlist(frag,genbra(err, frag, args->r->l, 0), top); /* Test */
				rmlooplvl(frag, lvlLOOP, cont, frag->code);
			} else { /* Four args */
				genn(err, frag, args->l); /* Initializer */
				emitc(frag, iBRA);
				emitn(frag, 0);
				mklooplvl(frag, lvlLOOP, frag->code-sizeof(int), 0);
				if (name)
					frag->looplvls->name = strdup(name->r->s);
				top = frag->code;
				mklooplvl(frag, lvlSCOPE, 0, 0);
				genn(err, frag, args->r->r->r); /* Body */
				rmlooplvl(frag, lvlSCOPE, 0, 0);
				genn(err, frag, args->r->r->l); /* Increment */
				cont = frag->code;
				setlist(frag,genbra(err, frag, args->r->l, 0), top); /* Test */
				rmlooplvl(frag, lvlLOOP, cont, frag->code);
			}
			break;
		} case nFOREACH: case nFORINDEX: {
			int top, cont;
			Node *name;
			Node *args = extract_loop_name(n->r, &name);
			if (args->what != nSEMI) { /* One arg */
				error_2(err,"\"%s\" %d: No args for foreach?", n->loc->name, n->loc->line);
			} else if (args->r->what != nSEMI) { /* Two args */
				error_2(err,"\"%s\" %d: Only two args for foreach?", n->loc->name, n->loc->line);
			} else { /* Three args */
				if (args->l->what != nNAM) {
					error_2(err, "\"%s\" %d: First arg to foreach must be a variable", n->loc->name, n->loc->line);
				}
				mklooplvl(frag, lvlSCOPE, 0, 0); /* Scope for args */
				gen(err, frag, args->l);	/* Variable (check that it really is at runtime) */
				gen(err, frag, args->r->l);	/* Array/object */
				push_num(frag);
				emitl(frag, 0);	/* Temp vars for iFOREACH */
				push_num(frag);
				emitl(frag, -1);
				emitc(frag, iBRA);
				emitn(frag, 0);
				mklooplvl(frag, lvlLOOP, frag->code-sizeof(int), 0); /* Start loop */
				if (name)
					frag->looplvls->name = strdup(name->r->s);
				top = frag->code;
				mklooplvl(frag, lvlSCOPE, 0, 0); /* Scope for body */
				genn(err, frag, args->r->r);
				rmlooplvl(frag, lvlSCOPE, 0, 0); /* Body scope done */
				cont = frag->code;
				if (n->what == nFOREACH)
					emitc(frag, iFOREACH);
				else
					emitc(frag, iFORINDEX);
				align_frag(frag, sizeof(int));
				emitn(frag, top - (frag->code));
				rmlooplvl(frag, lvlLOOP, cont, frag->code); /* Complete loop */
				emitc(frag, iPOP);
				emitc(frag, iPOP);
				emitc(frag, iPOP);
				emitc(frag, iPOP);
				fixlooplvl(frag, 4); /* POP temp vars */
				rmlooplvl(frag, lvlSCOPE, 0, 0); /* POP args scope */
			}
			break;
		} case nWHILE: {
			int top, cont;
			Node *name;
			Node *args = extract_loop_name(n->r, &name);
			if (args->what==nEMPTY) {
				error_2(err,"\"%s\" %d: No args for while", n->loc->name, n->loc->line);
				break;
			}
			emitc(frag, iBRA);
			emitn(frag, 0);
			mklooplvl(frag, lvlLOOP, frag->code-sizeof(int), 0);
			if (name)
				frag->looplvls->name = strdup(name->r->s);
			top = frag->code;
			mklooplvl(frag, lvlSCOPE, 0, 0);
			if (args->what==nSEMI)
				genn(err, frag, args->r);
			rmlooplvl(frag, lvlSCOPE, 0, 0);
			cont = frag->code;
			if (args->what==nSEMI)
				setlist(frag, genbra(err, frag, args->l, 0), top);
			else
				setlist(frag, genbra(err, frag, args, 0), top);
			rmlooplvl(frag, lvlLOOP, cont, frag->code);
			break;
		} case nRETURN: {
			int z;
			if (n->r)
				gen(err, frag, n->r);
			else
				push_void(frag);
			emitc(frag, iSTASH);
			rmlooplvl(frag, lvlVALUE, 0, 0);
			poploops(frag, NULL);
			emitc(frag, iBRA);
			z = emitn(frag, 0);
			if (frag->rtn)
				addlist(frag, frag->rtn, z);
			else
				frag->rtn = z;
			break;
		} case nLOOP: {
			int cont;
			Node *name;
			Node *args = extract_loop_name(n->r, &name);
			cont = frag->code;
			mklooplvl(frag, lvlLOOP, 0, 0);
			if (name)
				frag->looplvls->name = strdup(name->r->s);
			mklooplvl(frag, lvlSCOPE, 0, 0);
			genn(err, frag, args);
			rmlooplvl(frag, lvlSCOPE, 0, 0);
			emitc(frag, iBRA);
			align_frag(frag, sizeof(int));
			emitn(frag, cont - (frag->code));
			rmlooplvl(frag, lvlLOOP, cont, frag->code);
			break;
		} case nBREAK: {
			struct looplvl *ll = findlvl(frag, NULL);
			if (n->r) {
				// printf("looking... %s %p %p\n",n->r->s,frag,ll);
				if (n->r->what == nNAM)
					ll = findlvl(frag, n->r->s);
				else if (n->r->what != nEMPTY)
					error_2(err, "\"%s\" %d: Invalid argument to break", n->r->loc->name, n->r->loc->line);
			}
			if (ll) {
				int z;
				// printf("break %d %d\n", ll->scopelvl, frag->scopelvl);
				poploops(frag, ll);
				emitc(frag, iBRA);
				z = emitn(frag, 0);
				if (ll->brk)
					addlist(frag, ll->brk, z);
				else
					ll->brk = z;
			} else
				error_2(err, "\"%s\" %d: break with no loop", n->loc->name, n->loc->line);
			break;
		} case nCONT: {
			struct looplvl *ll = findlvl(frag, NULL);
			if (n->r) {
				if (n->r->what == nNAM)
					ll = findlvl(frag, n->r->s);
				else if (n->r->what != nEMPTY)
					error_2(err, "\"%s\" %d: Invalid argument to continue", n->r->loc->name, n->r->loc->line);
			}
			if (ll) {
				int z;
				poploops(frag, ll);
				emitc(frag, iBRA);
				z = emitn(frag, 0);
				if (ll->cont)
					addlist(frag, ll->cont, z);
				else
					ll->cont = z;
			} else
				error_2(err, "\"%s\" %d: continue with no loop", n->loc->name, n->loc->line);
			break;
		} case nUNTIL: {
			int els = genbra(err, frag, n->r, 1);
			struct looplvl *ll = findlvl(frag, NULL);
			if (ll) {
				int z;
				// printf("break %d %d\n", ll->scopelvl, frag->scopelvl);
				poploops(frag, ll);
				emitc(frag, iBRA);
				z = emitn(frag, 0);
				if (ll->brk)
					addlist(frag, ll->brk, z);
				else
					ll->brk = z;
			} else {
				error_2(err, "\"%s\" %d: until with no loop", n->loc->name, n->loc->line);
			}
			setlist(frag, els, frag->code);
			break;
		} case nIF: {
			gencond(err, frag, n->r, 0);
			break;
		} case nELSE: {
			error_2(err, "\"%s\" %d: else with no if", n->loc->name, n->loc->line);
			break;
		} case nSEMI: {
			if (n->r->what == nELSE) {
				int done = genelif(err, frag, n->l, 0);
				if (!done)
					error_2(err, "\"%s\" %d: else with no if", n->r->loc->name, n->r->loc->line);
				if (n->r->r->what == nIF) {
					addlist(frag, done, genbra(err, frag, n->r->r->l, 1));
					n = n->r;
				}
				mklooplvl(frag, lvlSCOPE, 0, 0);
				genn(err, frag, n->r->r);
				rmlooplvl(frag, lvlSCOPE, 0, 0);
				setlist(frag, done, frag->code);
			} else {
				genn(err, frag, n->l);
				genn(err, frag, n->r);
			}
			break;
		} case nEMPTY: {
			break;
		} default: {
			gen(err, frag, n);
			emitc(frag, iPOP);
			rmlooplvl(frag, lvlVALUE, 0, 0);
			break;
		}
	}
}

static int genbra(Error_printer *err, Frag *frag, Node * n, int t)
{
	switch(n->what) {
		case nEQ: {
			gen(err, frag, n->l);
			gen(err, frag, n->r);
			emitc(frag, iCMP);
			rmlooplvl(frag, lvlVALUE, 0, 0);
			if (t)
				emitc(frag, iBNE);
			else
				emitc(frag, iBEQ);
			rmlooplvl(frag, lvlVALUE, 0, 0);
			return emitn(frag, 0);
		} case nNE: {
			gen(err, frag, n->l);
			gen(err, frag, n->r);
			emitc(frag, iCMP);
			rmlooplvl(frag, lvlVALUE, 0, 0);
			if (t)
				emitc(frag, iBEQ);
			else
				emitc(frag, iBNE);
			rmlooplvl(frag, lvlVALUE, 0, 0);
			return emitn(frag, 0);
		} case nGT: {
			gen(err, frag, n->l);
			gen(err, frag, n->r);
			emitc(frag, iCMP);
			rmlooplvl(frag, lvlVALUE, 0, 0);
			if (t)
				emitc(frag, iBLE);
			else
				emitc(frag, iBGT);
			rmlooplvl(frag, lvlVALUE, 0, 0);
			return emitn(frag, 0);
		} case nGE: {
			gen(err, frag, n->l);
			gen(err, frag, n->r);
			emitc(frag, iCMP);
			rmlooplvl(frag, lvlVALUE, 0, 0);
			if (t)
				emitc(frag, iBLT);
			else
				emitc(frag, iBGE);
			rmlooplvl(frag, lvlVALUE, 0, 0);
			return emitn(frag, 0);
		} case nLT: {
			gen(err, frag, n->l);
			gen(err, frag, n->r);
			emitc(frag, iCMP);
			rmlooplvl(frag, lvlVALUE, 0, 0);
			if (t)
				emitc(frag, iBGE);
			else
				emitc(frag, iBLT);
			rmlooplvl(frag, lvlVALUE, 0, 0);
			return emitn(frag, 0);
		} case nLE: {
			gen(err, frag, n->l);
			gen(err, frag, n->r);
			emitc(frag, iCMP);
			rmlooplvl(frag, lvlVALUE, 0, 0);
			if (t)
				emitc(frag, iBGT);
			else
				emitc(frag, iBLE);
			rmlooplvl(frag, lvlVALUE, 0, 0);
			return emitn(frag, 0);
		} case nNOT: {
			return genbra(err, frag, n->r, !t);
		} case nLAND: {
			int b1 = genbra(err, frag, n->l, 1);
			int b2;
			if (t)
				b2 = genbra(err, frag, n->r, 1), addlist(frag, b2, b1);
			else
				b2 = genbra(err, frag, n->r, 0), setlist(frag, b1, frag->code);
			return b2;
		} case nLOR: {
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
				emitc(frag, iBEQ);
			else
				emitc(frag, iBNE);
			rmlooplvl(frag, lvlVALUE, 0, 0);
			return emitn(frag, 0);
		}
	}
}

/* Code generator: convert a parse-tree into pseudo-machine code */

Pseudo *codegen(Error_printer *err, Node *n)
{
	Frag frag[1];

	init_frag(frag);

	gen(err, frag, n);

	emitc(frag, iSTASH);

	if (frag->rtn)
		setlist(frag, frag->rtn, frag->code);

	emitc(frag, iRTS);

	return frag->begcode;
}
