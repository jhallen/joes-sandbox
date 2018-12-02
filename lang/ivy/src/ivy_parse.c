/* IVY Parser

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include "ivy_symbols.h"
#include "ivy_free_list.h"
#include "ivy_tree.h"
#include "ivy.h"

/* Skip over whitespace */

static void skipws(Ivy_loc *loc)
{
	for (;;) switch (*loc->ptr) {
		case ' ': {
			++loc->ptr;
			++loc->col;
			break;
		} case '\t': {
			++loc->ptr;
			loc->col += 8 - loc->col % 8;
			break;
		} case '\n': case '#': {
			loc->ptr = "";
		} default: {
			if (loc->lvl == -1)
				loc->lvl = loc->col;
			return;
		}
	}
}

/* Skip whitespace including ; */

static void skipwss(Ivy_loc *loc)
{
	while(skipws(loc), (*loc->ptr == ';')) {
		++loc->ptr;
		++loc->col;
	}
}

/* Skip whitespace including ; and , */

static void skipwssc(Ivy_loc *loc)
{
	while(skipws(loc), (*loc->ptr == ';' || *loc->ptr == ',')) {
		++loc->ptr;
		++loc->col;
	}
}

/* Skip whitespace including , */

static void skipwsc(Ivy_loc *loc)
{
	while(skipws(loc), (*loc->ptr == ',')) {
		++loc->ptr;
		++loc->col;
	}
}

/* Table of operators and parse-tree node types */

/*               NAME    PREFIX  INFIX   POSTFIX PREC    ASSOC   METH    INST    Symbol */
Ivy_what ivy_what_tab[]=
{
	/* Constants */
	{ ivy_nNAM, "nam", 0, 0, 0, 120, 0, 0, 0, 0 },
	{ ivy_nNUM, "num", 0, 0, 0, 120, 0, 0, 0, 0 },
	{ ivy_nFP, "fp", 0, 0, 0, 120, 0, 0, 0, 0 },
	{ ivy_nSTR, "str", 0, 0, 0, 120, 0, 0, 0, 0 },

	/* Operators */
	{ ivy_nQUOTE, "`", ivy_nQUOTE, 0, 0, 120, 0, 1, 0, ivy_nQUOTE },
	{ ivy_nELLIPSIS, "...", 0, 0, ivy_nELLIPSIS, 120, 0, 1, 0, 0 },

	{ ivy_nCALL1, ".", 0, ivy_nCALL1, 0, 110, 0, 2, 0, 0 },

	{ ivy_nENV, "::", 0, ivy_nENV, 0, 105, 0, 2, ivy_iENV, 0 },

	{ ivy_nCALL, "(", 0, ivy_nCALL, 0, 100, 0, 66, 0, 0 },

	{ ivy_nPRINC, "++", ivy_nPRINC, 0, ivy_nPOINC, 90, 0, 5, 0, ivy_nADD },
	{ ivy_nPRDEC, "--", ivy_nPRDEC, 0, ivy_nPODEC, 90, 0, 5, 0, ivy_nSUB },
	{ ivy_nPOINC, "++", ivy_nPRINC, 0, ivy_nPOINC, 90, 0, 13, 0, ivy_nADD },
	{ ivy_nPODEC, "--", ivy_nPRDEC, 0, ivy_nPODEC, 90, 0, 13, 0, ivy_nSUB },
	{ ivy_nCOM, "~", ivy_nCOM, 0, 0, 90, 0, 1, ivy_iCOM, 0 },
	{ ivy_nNEG, "neg", ivy_nNEG, ivy_nSUB, 0, 90, 0, 1, ivy_iNEG, 0 },
	{ ivy_nNOT, "!", ivy_nNOT, 0, 0, 90, 0, 1, 0, 0 },
	{ ivy_nAT, "*", ivy_nAT, ivy_nMUL, 0, 90, 0, 1, ivy_iAT, 0 },
	{ ivy_nADDR, "&", ivy_nADDR, ivy_nAND, 0, 90, 0, 1, 0, 0 },

	{ ivy_nSHR, ">>", 0, ivy_nSHR, 0, 80, 0, 2, ivy_iSHR, 0 },
	{ ivy_nSHL, "<<", 0, ivy_nSHL, 0, 80, 0, 2, ivy_iSHL, 0 },

	{ ivy_nMUL, "*", ivy_nAT, ivy_nMUL, 0, 70, 0, 2, ivy_iMUL, 0 },
	{ ivy_nDIV, "/", 0, ivy_nDIV, 0, 70, 0, 2, ivy_iDIV, 0 },
	{ ivy_nMOD, "%", ivy_nMOD, ivy_nMOD, 0, 70, 0, 2, ivy_iMOD, 0 },
	{ ivy_nAND, "&", ivy_nADDR, ivy_nAND, 0, 70, 0, 2, ivy_iAND, 0 },

	{ ivy_nADD, "+", 0, ivy_nADD, 0, 60, 0, 2, ivy_iADD, 0 },
	{ ivy_nSUB, "-", ivy_nNEG, ivy_nSUB, 0, 60, 0, 2, ivy_iSUB, 0 },
	{ ivy_nOR, "|", 0, ivy_nOR, 0, 60, 0, 2, ivy_iOR, 0 },
	{ ivy_nXOR, "^", 0, ivy_nXOR, 0, 60, 0, 2, ivy_iXOR, 0 },

	{ ivy_nEQ, "==", 0, ivy_nEQ, 0, 50, 0, 2, ivy_iBEQ, 0 },
	{ ivy_nNE, "!=", 0, ivy_nNE, 0, 50, 0, 2, ivy_iBNE, 0 },
	{ ivy_nLT, "<", 0, ivy_nLT, 0, 50, 0, 2, ivy_iBLT, 0 },
	{ ivy_nGT, ">", 0, ivy_nGT, 0, 50, 0, 2, ivy_iBGT, 0 },
	{ ivy_nLE, "<=", 0, ivy_nLE, 0, 50, 0, 2, ivy_iBLE, 0 },
	{ ivy_nGE, ">=", 0, ivy_nGE, 0, 50, 0, 2, ivy_iBGE, 0 },

	{ ivy_nLAND, "&&", 0, ivy_nLAND, 0, 40, 0, 2, 0, 0 },

	{ ivy_nLOR, "||", 0, ivy_nLOR, 0, 30, 0, 2, 0, 0 },

	{ ivy_nSET, "=", 0, ivy_nSET, 0, 20, 1, 2, ivy_iSET, 0 },
	{ ivy_nDOTTO, ".=", 0, ivy_nDOTTO, 0, 20, 1, 6, 0, ivy_nCALL1 },
	{ ivy_nSHLTO, "<<=", 0, ivy_nSHLTO, 0, 20, 1, 6, 0, ivy_nSHL },
	{ ivy_nSHRTO, ">>=", 0, ivy_nSHRTO, 0, 20, 1, 6, 0, ivy_nSHR },
	{ ivy_nMULTO, "*=", 0, ivy_nMULTO, 0, 20, 1, 6, 0, ivy_nMUL },
	{ ivy_nDIVTO, "/=", 0, ivy_nDIVTO, 0, 20, 1, 6, 0, ivy_nDIV },
	{ ivy_nMODTO, "%=", 0, ivy_nMODTO, 0, 20, 1, 6, 0, ivy_nMOD },
	{ ivy_nANDTO, "&=", 0, ivy_nANDTO, 0, 20, 1, 6, 0, ivy_nAND },
	{ ivy_nADDTO, "+=", 0, ivy_nADDTO, 0, 20, 1, 6, 0, ivy_nADD },
	{ ivy_nSUBTO, "-=", 0, ivy_nSUBTO, 0, 20, 1, 6, 0, ivy_nSUB },
	{ ivy_nXORTO, "^=", 0, ivy_nXORTO, 0, 20, 1, 6, 0, ivy_nXOR },
	{ ivy_nORTO, "|=", 0, ivy_nORTO, 0, 20, 1, 6, 0, ivy_nOR },
	{ ivy_nPOST, ":", 0, ivy_nPOST, 0, 20, 1, 10, 0, 0 },
	{ ivy_nDOTPO, ".:", 0, ivy_nDOTPO, 0, 20, 1, 14, 0, ivy_nCALL1 },
	{ ivy_nSHLPO, "<<:", 0, ivy_nSHLPO, 0, 20, 1, 14, 0, ivy_nSHL },
	{ ivy_nSHRPO, ">>:", 0, ivy_nSHRPO, 0, 20, 1, 14, 0, ivy_nSHR },
	{ ivy_nMULPO, "*:", 0, ivy_nMULPO, 0, 20, 1, 14, 0, ivy_nMUL },
	{ ivy_nDIVPO, "/:", 0, ivy_nDIVPO, 0, 20, 1, 14, 0, ivy_nDIV },
	{ ivy_nMODPO, "%:", 0, ivy_nMODPO, 0, 20, 1, 14, 0, ivy_nMOD },
	{ ivy_nANDPO, "&:", 0, ivy_nANDPO, 0, 20, 1, 14, 0, ivy_nAND },
	{ ivy_nADDPO, "+:", 0, ivy_nADDPO, 0, 20, 1, 14, 0, ivy_nADD },
	{ ivy_nSUBPO, "-:", 0, ivy_nSUBPO, 0, 20, 1, 14, 0, ivy_nSUB },
	{ ivy_nXORPO, "^:", 0, ivy_nXORPO, 0, 20, 1, 14, 0, ivy_nXOR },
	{ ivy_nORPO, "|:", 0, ivy_nORPO, 0, 20, 1, 14, 0, ivy_nOR },

	{ ivy_nCOMMA, "\\", 0, ivy_nCOMMA, 0, 15, 0, 2, 0, 0 },

	{ ivy_nSEMI, ";", 0, ivy_nSEMI, 0, 10, 0, 2, 0, 0 },

	/* A function definition */
	{ ivy_nDEFUN, "fn", 0, 0, 0, 0, 0, 0, 0, 0 },

	/* A list [...] */
	{ ivy_nLIST, "LST", 0, 0, 0, 0, 0, 1, 0, 0 },

	/* Parenthasis */
	{ ivy_nPAREN, "paren", 0, 0, 0, 0, 0, 1, 0, 0 },

	/* Nothing */
	{ ivy_nVOID, "void", 0, 0, 0, 0, 0, 1, 0, 0 },

	/* Current scope */
	{ ivy_nTHIS, "this", 0, 0, 0, 0, 0, 1, 0, 0 },

	/* Empty list */
	{ ivy_nEMPTY, "empty", 0, 0, 0, 0, 0, 1, 0, 0 },

	/* A label */
	{ ivy_nLABEL, "label", 0, 0, 0, 0, 0, 1, 0, 0 },

	/* Statements */
	{ ivy_nIF, "if", 0, 0, 0, 0, 0, 1, 0, 0 },
	{ ivy_nFOR, "for", 0, 0, 0, 0, 0, 1, 0, 0 },
	{ ivy_nWHILE, "while", 0, 0, 0, 0, 0, 1, 0, 0 },
	{ ivy_nVAR, "var", 0, 0, 0, 0, 0, 1, 0, 0 },
	{ ivy_nLOOP, "loop", 0, 0, 0, 0, 0, 2, 0, 0 },
	{ ivy_nUNTIL, "until", 0, 0, 0, 0, 0, 3, 0, 0 },
	{ ivy_nBREAK, "break", 0, 0, 0, 0, 0, 4, 0, 0 },
	{ ivy_nCONT, "continue", 0, 0, 0, 0, 0, 4, 0, 0 },
	{ ivy_nRETURN, "return", 0, 0, 0, 0, 0, 4, 0, 0 },
	{ ivy_nFOREACH, "foreach", 0, 0, 0, 0, 0, 5, 0, 0 },
	{ ivy_nFORINDEX, "forindex", 0, 0, 0, 0, 0, 5, 0, 0 },
	{ ivy_nSCOPE, "scope", 0, 0, 0, 0, 0, 1, 0, 0 }
};

/* Operator scanner */

#define OPRTLEN 64
#define OPRLTLEN 128
static char oprltab[OPRLTLEN];		/* Hash table of operator left-substrings */
static Ivy_what *oprtab[OPRTLEN];		/* Hash table of operators */
static int opr_isinit;			/* Set if table has been initialized */

static int ioprtab[] =
{
	ivy_nCALL1, ivy_nENV, ivy_nCALL, ivy_nPRINC, ivy_nPRDEC, ivy_nCOM, ivy_nSUB, ivy_nNOT, ivy_nSHR, ivy_nSHL,
	ivy_nMUL, ivy_nDIV, ivy_nMOD, ivy_nAND, ivy_nADD, ivy_nOR, ivy_nXOR, ivy_nEQ, ivy_nNE, ivy_nGE, ivy_nLE,
	ivy_nGT, ivy_nLT, ivy_nLAND, ivy_nLOR, ivy_nSET, ivy_nDOTTO, ivy_nSHLTO, ivy_nSHRTO, ivy_nMULTO,
	ivy_nDIVTO, ivy_nMODTO, ivy_nANDTO, ivy_nADDTO, ivy_nSUBTO, ivy_nXORTO, ivy_nORTO, ivy_nPOST,
	ivy_nDOTPO, ivy_nSHLPO, ivy_nSHRPO, ivy_nMULPO, ivy_nDIVPO, ivy_nMODPO, ivy_nANDPO, ivy_nADDPO,
	ivy_nSUBPO, ivy_nXORPO, ivy_nORPO, ivy_nQUOTE, ivy_nELLIPSIS, ivy_nCOMMA, -1
};

static void izoprtab(void)
{
	int x;
	int t;
	opr_isinit = 1;
	for (x = 0; (t = ioprtab[x]) != -1; ++x) {
		Ivy_what *tt = &ivy_what_tab[t];
		const char *s = tt->name;
		unsigned long accu = 0;
		while (*s) {
			accu = ivy_hnext(accu, *s++);
			if (*s)
				++oprltab[accu % OPRLTLEN];
		}
		tt->next = oprtab[accu % OPRTLEN];
		oprtab[accu % OPRTLEN] = tt;
	}
}

static int strcmpn(const char *blk, int len, const char *s)
{
	while (len && *s && *blk == *s) {
		++blk;
		++s;
		--len;
	}
	if (!len && !*s)
		return 0;
	else
		return 1;
}

static Ivy_what *doopr(Ivy_loc *loc, unsigned long accu, const char *start)
{
	unsigned long oaccu = accu;
	const char *oops = loc->ptr;
	int oopscol = loc->col;
	Ivy_what *t;
	do {
		if (!*loc->ptr)
			break;
		accu = ivy_hnext(accu, *loc->ptr++);
		for (t = oprtab[accu % OPRTLEN]; t; t = t->next)
			if (!strcmpn(start, loc->ptr - start, t->name)) {
				Ivy_what *u;
				if ((u = doopr(loc, accu, start)))
					return u;
				else
					return t;
			}
	} while (oprltab[accu % OPRLTLEN]);
	accu = oaccu;
	loc->ptr = oops;
	loc->col = oopscol;
	return 0;
}

/* Look up an operator */

static Ivy_what *opr(Ivy_loc *loc)
{
	if (!opr_isinit)
		izoprtab();
	return doopr(loc, 0, loc->ptr);
}

/* Get a character which might be an escape sequence */

static int escape(Ivy_loc *loc)
{
	if (*loc->ptr == '\\') {
		++loc->ptr;
		++loc->col;
		switch (*loc->ptr) {
			case '^': {
				++loc->ptr;
				++loc->col;
				if (*loc->ptr >= '@' && *loc->ptr <= '_')
					return ++loc->col, *loc->ptr++ - '@';
				else if (*loc->ptr == '?')
					return ++loc->col, ++loc->ptr, 127;
				else
					return escape(loc);
			} case '8': case '9': {
				return ++loc->col, *loc->ptr++ - '0';
			} case 'x': case 'X': {
				int num = 0;
				++loc->col;
				++loc->ptr;
				if (*loc->ptr >= '0' && *loc->ptr <= '9')
					num = (++loc->col, *loc->ptr++ - '0');
				else if (*loc->ptr >= 'a' && *loc->ptr <= 'f')
					num = (++loc->col, *loc->ptr++ - 'a' + 10);
				else if (*loc->ptr >= 'A' && *loc->ptr <= 'F')
					num = (++loc->col, *loc->ptr++ - 'A' + 10);
				if (*loc->ptr >= '0' && *loc->ptr <= '9')
					num =
					    num * 16 + (++loc->col,
							*loc->ptr++ - '0');
				else if (*loc->ptr >= 'a' && *loc->ptr <= 'f')
					num =
					    num * 16 + (++loc->col,
							*loc->ptr++ - 'a' + 10);
				else if (*loc->ptr >= 'A' && *loc->ptr <= 'F')
					num =
					    num * 16 + (++loc->col,
							*loc->ptr++ - 'A' + 10);
				return num;
			} case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': {
				int num = *loc->ptr++ - '0';
				++loc->col;
				if (*loc->ptr >= '0' && *loc->ptr <= '7')
					num =
					    num * 8 + *loc->ptr++ - '0', ++loc->col;
				if (*loc->ptr >= '0' && *loc->ptr <= '7')
					num =
					    num * 8 + *loc->ptr++ - '0', ++loc->col;
				return num;
			} case 'a': {
				return ++loc->ptr, ++loc->col, 7;
			} case 'b': {
				return ++loc->ptr, ++loc->col, 8;
			} case 'e': {
				return ++loc->ptr, ++loc->col, 27;
			} case 'f': {
				return ++loc->ptr, ++loc->col, 12;
			} case 'n': {
				return ++loc->ptr, ++loc->col, 10;
			} case 'r': {
				return ++loc->ptr, ++loc->col, 13;
			} case 't': {
				return ++loc->ptr, ++loc->col, 9;
			} case '\\': {
				return ++loc->col, *loc->ptr++;
			} default: {
				return escape(loc);
			}
		}
	} else if (*loc->ptr != '\t') {
		return ++loc->col, *loc->ptr++;
	} else {
		return loc->col += 8 - loc->col % 8, *loc->ptr++;
	}
}

/* Keyword (statement) table */

static int ikwtab[] =
    { ivy_nIF, ivy_nFOR, ivy_nWHILE, ivy_nLOOP, ivy_nVAR, ivy_nUNTIL, ivy_nBREAK, ivy_nCONT, ivy_nRETURN, ivy_nFOREACH, ivy_nFORINDEX, ivy_nDEFUN, ivy_nSCOPE, -1 };

#define KWHTLEN 32
static Ivy_what *kwhtab[KWHTLEN];
static int kw_isinit = 0;

static void izkwtab(void)
{
	int x;
	kw_isinit = 1;
	for (x = 0; ikwtab[x] != -1; ++x) {
		char *s = ivy_symbol_add(ivy_what_tab[ikwtab[x]].name);
		unsigned long hval = ivy_fib_hash(s, (64-5));
		ivy_what_tab[ikwtab[x]].name = s; /* Replace name with symbol */
		ivy_what_tab[ikwtab[x]].next = kwhtab[hval % KWHTLEN];
		kwhtab[hval % KWHTLEN] = &ivy_what_tab[ikwtab[x]];
	}
}

/* Look up a keyword by symbol */

static Ivy_what *kw(char *symbol)
{
	Ivy_what *a;
	if (!symbol)
		return 0;
	if (!kw_isinit)
		izkwtab();
	for (a = kwhtab[ivy_fib_hash(symbol, (64 - 5)) % KWHTLEN]; a; a = a->next)
		if (a->name == symbol)
			return a;
	return 0;
}

/* Parser transition macros */

/* #define PTRACE(x) printf(x); printf(" %s\n", parser->loc->ptr); */
#define PTRACE(x)

/* Parser helper functions:
     Call 'subr' with precedence 'prec'.  Subr returns to 'retn'.
*/

#define pcall(subr,retn,nprec) do { \
	Ivy_parse_state *stk; \
	stk = (Ivy_parse_state *)malloc(sizeof(Ivy_parse_state)); \
	parser->state.state = (retn); \
	*stk = parser->state; \
	parser->state.next = stk; \
	parser->state.prec = (nprec); \
	parser->state.state = (subr); \
} while(0)

/* Return with a node */

#define pret(n) do { \
	Ivy_parse_state *stk = parser->state.next; \
	parser->rtn = (n); \
        parser->state = *stk; \
        free(stk); \
} while(0)

/* Loop jump */

#define pjump(next_state) do { \
	parser->state.state = (next_state); \
} while(0)

/* Check size of string buffer: expand if necessary */

#define check_str_buf() do { \
	if (parser->str_len == parser->str_siz) \
		parser->str_siz *= 2; \
		parser->str_buf = (char *)realloc(parser->str_buf,parser->str_siz); \
} while(0)

static int parse_rest(Ivy_parser *);

static int parse_rest_done_infix(Ivy_parser *parser)
{
	PTRACE("parse_rest_done_infix");
	parser->state.n = ivy_cons2(parser->loc,parser->state.op->what, parser->state.n, parser->rtn);
	pjump(parse_rest);
	return 0;
}

/* Make op= or op: assignment */

static int parse_rest_done_infix1(Ivy_parser *parser)
{
	PTRACE("parse_rest_done_infix1");
	parser->state.n = ivy_cons2(parser->loc,(parser->state.op->meth & 8 ? ivy_nPOST : ivy_nSET), parser->state.n, ivy_cons2(parser->loc,parser->state.op->inst, ivy_dup_tree(parser->loc, parser->state.n), parser->rtn));
	pjump(parse_rest);
	return 0;
}

static int parse_rest_done_call(Ivy_parser *parser)
{
	Ivy_what *k;
	PTRACE("parse_rest_done_call");
	if (parser->state.op->what == ivy_nCALL && parser->state.n->what == ivy_nNAM && (k = kw(parser->state.n->s))) {
		/* rm(parser->state.n); */
		parser->state.n = ivy_cons1(parser->loc, k->what, ivy_opt(parser->loc, parser->rtn));
	} else
		parser->state.n = ivy_cons2(parser->loc, parser->state.op->what, parser->state.n, ivy_opt(parser->loc,parser->rtn));
	if (*parser->loc->ptr == '}')
		++parser->loc->ptr, ++parser->loc->col;
	else
		ivy_error_2(parser->err,"\"%s\" %d: Error: missing ]",parser->loc->name,parser->loc->line);
	--parser->paren_level;
	pjump(parse_rest);
	return 0;
}

static int parse_rest_done_call1(Ivy_parser *parser)
{
	Ivy_what *k;
	PTRACE("parse_rest_done_call1");
	if (parser->state.op->what == ivy_nCALL && parser->state.n->what == ivy_nNAM && (k = kw(parser->state.n->s))) {
		/* rm(parser->state.n); */
		parser->state.n = ivy_cons1(parser->loc, k->what, ivy_opt(parser->loc, parser->rtn));
	} else
		parser->state.n = ivy_cons2(parser->loc, parser->state.op->what, parser->state.n, ivy_opt(parser->loc,parser->rtn));
	if (*parser->loc->ptr == ')')
		++parser->loc->ptr, ++parser->loc->col;
	else
		ivy_error_2(parser->err,"\"%s\" %d: Error: missing )",parser->loc->name,parser->loc->line);
	--parser->paren_level;
	pjump(parse_rest);
	return 0;
}

/* Try to parse an infix or postfix operator */

static int parse_lst(Ivy_parser *);
static int parse_expr(Ivy_parser *);

static int parse_rest(Ivy_parser *parser)
{
	const char *oops = parser->loc->ptr;
	int oopscol = parser->loc->col;
	int left;
	int right;
	PTRACE("parse_rest");

	if (!parser->state.n) {
		pret(NULL);
		return 0;
	}

	/* Measure left side */

	left = parser->loc->col;

	skipws(parser->loc);

	left = parser->loc->col - left;

	/* Get operator */

	parser->state.op = opr(parser->loc);

	/* Measure right side */

	right = parser->loc->col;

	skipws(parser->loc);

	if (*parser->loc->ptr && *parser->loc->ptr != ';')
		right = parser->loc->col - right;
	else
		right = 32767;

	if (parser->state.op && parser->state.op->infix && left <= right &&
	    (ivy_what_tab[parser->state.op->infix].prec > parser->state.prec ||
	     (ivy_what_tab[parser->state.op->infix].prec == parser->state.prec && ivy_what_tab[parser->state.op->infix].assoc))) {
		if (parser->state.op->meth & 128) {	/* Function call? */
			parser->state.op = &ivy_what_tab[parser->state.op->infix];
			++parser->paren_level;
			PTRACE("parse_rest pcall parse_lst");
			pcall(parse_lst,parse_rest_done_call,0);
			return 0;
		} else if (parser->state.op->meth & 64) {	/* Function call? */
			parser->state.op = &ivy_what_tab[parser->state.op->infix];
			++parser->paren_level;
			PTRACE("parse_rest pcall parse_lst1");
			pcall(parse_lst,parse_rest_done_call1,0);
			return 0;
		} else {
			parser->state.op = &ivy_what_tab[parser->state.op->infix];
			if (parser->state.op->meth & 4) {	/* Make into assignment? */
				PTRACE("parse_rest pcall parse_expr");
				pcall(parse_expr,parse_rest_done_infix1,parser->state.op->prec);
				return 2;
			} else {
				PTRACE("parse_rest pcall parse_expr1");
				pcall(parse_expr,parse_rest_done_infix,parser->state.op->prec);
				return 2;
			}
		}
	} else if (parser->state.op && parser->state.op->postfix && (!parser->state.op->prefix || left <= right) &&
	           (ivy_what_tab[parser->state.op->postfix].prec > parser->state.prec ||
	            (ivy_what_tab[parser->state.op->postfix].prec == parser->state.prec && ivy_what_tab[parser->state.op->postfix].assoc))) {
		parser->state.op = &ivy_what_tab[parser->state.op->postfix];
		if (parser->state.op->meth & 4)	/* Make into an assignment? */
			parser->state.n = ivy_cons2(parser->loc, ivy_nPOST, parser->state.n,
			              ivy_cons2(parser->loc,parser->state.op->inst, ivy_dup_tree(parser->loc, parser->state.n), ivy_consnum(parser->loc,1)));
		else
			parser->state.n = ivy_cons1(parser->loc,parser->state.op->what, parser->state.n);	/* Normal case */
		if (right == 32767) {
			PTRACE("parse_rest pret parse_rest");
			pret(parser->state.n);
		} else {
			PTRACE("parse_rest pjump parse_rest");
			pjump(parse_rest);
		}
	} else {
		parser->loc->ptr = oops;
		parser->loc->col = oopscol;

		pret(parser->state.n);
	}

	return 0;
}

static int parse_expr_done_prefix(Ivy_parser *parser)
{
	PTRACE("parse_expr_done_prefix");
	parser->state.n = ivy_cons1(parser->loc,parser->state.op->what, parser->rtn);
	pjump(parse_rest);
	return 0;
}

static int parse_expr_done_prefix1(Ivy_parser *parser)
{
	PTRACE("parse_expr_done_prefix1");
	parser->state.n = ivy_cons2(parser->loc,ivy_nSET, parser->rtn, ivy_cons2(parser->loc,parser->state.op->inst, ivy_dup_tree(parser->loc, parser->rtn), ivy_consnum(parser->loc,1)));
	pjump(parse_rest);
	return 0;
}

static int parse_string(Ivy_parser *parser)
{
	PTRACE("parse_string");
	/* Copy character into str_buf */
	while (*parser->loc->ptr && *parser->loc->ptr != '"' && !(parser->loc->ptr[0] == '\\' && !parser->loc->ptr[1])) {
		check_str_buf();
		parser->str_buf[parser->str_len++] = escape(parser->loc);
	}

	/* Done? */
	if (!*parser->loc->ptr || *parser->loc->ptr == '\\') {
		if (*parser->loc->ptr == '\\') {
			++parser->loc->ptr;
		} else {
			check_str_buf();
			parser->str_buf[parser->str_len++] = '\n';
		}
		if (parser->loc->eof) {
			ivy_error_2(parser->err,"\"%s\" %d: Missing closing \"",parser->loc->name,parser->loc->line);
		} else {
			/* Get more input */
			return 3;
		}
	} else {
		/* Skip over terminating " */
		++parser->loc->ptr, ++parser->loc->col;
	}

	/* Make sure there's space for the NUL */
	check_str_buf();
	parser->str_buf[parser->str_len] = 0;
	parser->state.n = ivy_consstr(parser->loc,(char *)memcpy(malloc(parser->str_len+1),parser->str_buf,parser->str_len+1),parser->str_len);
	pjump(parse_rest);
	return 0;
}

static int parse_expr_done_paren(Ivy_parser *parser)
{
	PTRACE("parse_expr_done_paren");
	parser->state.n = ivy_cons1(parser->loc,ivy_nPAREN, ivy_opt(parser->loc, parser->rtn));
	if (*parser->loc->ptr != ')')
		ivy_error_2(parser->err,"\"%s\" %d: Error: missing )",parser->loc->name,parser->loc->line);
	else
		++parser->loc->ptr, ++parser->loc->col;
	--parser->paren_level;
	pjump(parse_rest);
	return 0;
}

static int parse_expr_done_square(Ivy_parser *parser)
{
	PTRACE("parse_expr_done_square");
	parser->state.n = ivy_cons1(parser->loc,ivy_nPAREN, parser->rtn);
	if (*parser->loc->ptr != '}')
		ivy_error_2(parser->err,"\"%s\" %d: Error: missing }",parser->loc->name,parser->loc->line);
	else
		++parser->loc->ptr, ++parser->loc->col;
	--parser->paren_level;
	pjump(parse_rest);
	return 0;
}

static int parse_expr_done_list(Ivy_parser *parser)
{
	PTRACE("parse_expr_done_list");
	parser->state.n = ivy_cons1(parser->loc,ivy_nLIST, ivy_opt(parser->loc,parser->rtn));
	if (*parser->loc->ptr != ']')
		ivy_error_2(parser->err,"\"%s\" %d: Error: missing ]",parser->loc->name,parser->loc->line);
	else
		++parser->loc->ptr, ++parser->loc->col;
	--parser->paren_level;
	pjump(parse_rest);
	return 0;
}

/* Parse an expression: parse prefix operators and first term, then jump to parse_rest  */

/* opt() business.  cons1() and cons2() return NULL if any of their args are NULL.  opt()
   can be used on optional arguments: NULL is replaces with consempty().
   
   () [] give return NULL (and not an empty nPAREN).
   {} returns an empty nLIST.
*/

static int parse_paren(Ivy_parser *parser);

static int parse_expr(Ivy_parser *parser)
{
	PTRACE("parse_expr");
	/* Skip leading whitespace */
	skipws(parser->loc);

	switch (*parser->loc->ptr) {
		case 0:	{ /* Need more data */
			if (parser->loc->eof)
				pret(NULL);
			return 0;
		} case '0': { /* octal or hex */
			++parser->loc->ptr;
			++parser->loc->col;
			if (*parser->loc->ptr == 'x' || *parser->loc->ptr == 'X') { /* Hex */
				long long num = 0;
				++parser->loc->ptr, ++parser->loc->col;
				while ((*parser->loc->ptr >= '0' && *parser->loc->ptr <= '9') ||
				       (*parser->loc->ptr >= 'a' && *parser->loc->ptr <= 'f') ||
				       (*parser->loc->ptr >= 'A' && *parser->loc->ptr <= 'F') ||
				       *parser->loc->ptr == '_')
					if (*parser->loc->ptr >= '0' && *parser->loc->ptr <= '9')
						num = num * 16 + *parser->loc->ptr++ - '0', ++parser->loc->col;
					else if (*parser->loc->ptr >= 'a' && *parser->loc->ptr <= 'f')
						num = num * 16 + *parser->loc->ptr++ - 'a' + 10, ++parser->loc->col;
					else if (*parser->loc->ptr >= 'A' && *parser->loc->ptr <= 'F')
						num = num * 16 + *parser->loc->ptr++ - 'A' + 10, ++parser->loc->col;
					else if (*parser->loc->ptr == '_')
						parser->loc->ptr++, parser->loc->col++;
				parser->state.n = ivy_consnum(parser->loc,num);
				break;
			} else if (*parser->loc->ptr == 'b' || *parser->loc->ptr == 'B') { /* Binary */
				long long num = 0;
				++parser->loc->ptr;
				++parser->loc->col;
				while(*parser->loc->ptr == '0' || *parser->loc->ptr == '1' || *parser->loc->ptr == '_') {
					if (*parser->loc->ptr != '_')
						num = (num << 1) + *parser->loc->ptr - '0';
					++parser->loc->ptr;
					++parser->loc->col;
				}
				parser->state.n = ivy_consnum(parser->loc,num);
				break;
			} else if (*parser->loc->ptr == 'o' || *parser->loc->ptr == 'O') { /* Octal */
				long long num = 0;
				++parser->loc->ptr;
				++parser->loc->col;
				while((*parser->loc->ptr >= '0' && *parser->loc->ptr <= '7') || *parser->loc->ptr == '_') {
					if (*parser->loc->ptr != '_')
						num = (num << 3) + *parser->loc->ptr - '0';
					++parser->loc->ptr;
					++parser->loc->col;
				}
				parser->state.n = ivy_consnum(parser->loc,num);
				break;
			} else if (*parser->loc->ptr == '.' || *parser->loc->ptr == 'e' || *parser->loc->ptr == 'E')
				goto parsefloat;
			/* Fall into decimal */
		} case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
			int x;
			long long num = 0;
			for (x = 0; parser->loc->ptr[x] >= '0' && parser->loc->ptr[x] <= '9'; ++x)
				num = num * 10 + parser->loc->ptr[x] - '0';
			if (parser->loc->ptr[x] == '.' || parser->loc->ptr[x] == 'e' || parser->loc->ptr[x] == 'E')
				goto parsefloat;
			else {
				parser->loc->ptr += x;
				parser->loc->col += x;
				parser->state.n = ivy_consnum(parser->loc,num);
			}
			break;
		} case '\'': { /* Character */
			int num = 0;
			++parser->loc->ptr, ++parser->loc->col;
			num = escape(parser->loc);
			if (*parser->loc->ptr == '\'')
				++parser->loc->ptr, ++parser->loc->col;
			else
				ivy_error_2(parser->err,"\"%s\" %d: Error missing '",parser->loc->name,parser->loc->line);
			parser->state.n = ivy_consnum(parser->loc,num);
			break;
		} case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':		/* Name */
		case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
		case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
		case 's': case 't': case 'u': case 'v': case 'w': case 'x':
		case 'y': case 'z':

		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
		case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
		case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
		case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
		case 'Y': case 'Z': case '_': {
			parser->str_len = 0;
			while ((*parser->loc->ptr >= 'a' && *parser->loc->ptr <= 'z') || (*parser->loc->ptr >= 'A' && *parser->loc->ptr <= 'Z') || (*parser->loc->ptr >= '0' && *parser->loc->ptr <= '9') || *parser->loc->ptr == '_') {
				check_str_buf();
				parser->str_buf[parser->str_len++] = *parser->loc->ptr;
				++parser->loc->ptr;
				++parser->loc->col;
			}
			check_str_buf();
			parser->str_buf[parser->str_len] = 0;
			if (!strcmp(parser->str_buf, "void"))
				parser->state.n = ivy_consvoid(parser->loc);
			else if (!strcmp(parser->str_buf, "this"))
				parser->state.n = ivy_consthis(parser->loc);
			else
				parser->state.n = ivy_consnam(parser->loc, ivy_symbol_add(parser->str_buf));
			break;
		} case '"': { /* String */
			++parser->loc->ptr;
			++parser->loc->col;
			parser->str_len = 0;
			pjump(parse_string);
			return parse_string(parser); /* Don't get more input yet */
			break;
		} case '(': {		/* Prec. */
			++parser->loc->ptr, ++parser->loc->col;
			++parser->paren_level;
			pcall(/* parse_paren */ parse_lst,parse_expr_done_paren,0);
			return 0;
			break;
		} case '{': {		/* Prec. */
			++parser->loc->ptr, ++parser->loc->col;
			++parser->paren_level;
			pcall(parse_paren,parse_expr_done_square,0);
			return 0;
			break;
		} case '[': { /* List. */
			++parser->loc->ptr, ++parser->loc->col;
			++parser->paren_level;
			pcall(parse_lst,parse_expr_done_list,0);
			return 0;
			break;
		} case '.': { /* Float */
			if (parser->loc->ptr[1] >= '0' && parser->loc->ptr[1] <= '9') {
				int x;
				char buf[1024];
				double fp;
			      parsefloat:
				x = 0;
				while (parser->loc->ptr[x] >= '0' && parser->loc->ptr[x] <= '9')
					++x;
				if (parser->loc->ptr[x] == '.')
					++x;
				while (parser->loc->ptr[x] >= '0' && parser->loc->ptr[x] <= '9')
					++x;
				if (parser->loc->ptr[x] == 'e' || parser->loc->ptr[x] == 'E') {
					++x;
					if (parser->loc->ptr[x] == '+' || parser->loc->ptr[x] == '-')
						++x;
					while (parser->loc->ptr[x] >= '0' && parser->loc->ptr[x] <= '9')
						++x;
				}
				memcpy(buf, parser->loc->ptr, x);
				buf[x] = 0;
				fp = strtod(buf,NULL);
				parser->loc->ptr += x;
				parser->loc->col += x;
				parser->state.n = ivy_consfp(parser->loc,fp);
				break;
			}
			/* Fall into operator */
		} default: {		/* Operator? */
			const char *oops = parser->loc->ptr;
			int oopscol = parser->loc->col;
			parser->state.op = opr(parser->loc);
			if (parser->state.op && parser->state.op->prefix) {
				parser->state.op = &ivy_what_tab[parser->state.op->prefix];
				if (parser->state.op->meth & 4)	{ /* Make operator into an assignment (++ --) */
					pcall(parse_expr,parse_expr_done_prefix1,parser->state.op->prec);
					return 2;
				} else {
					pcall(parse_expr,parse_expr_done_prefix,parser->state.op->prec);
					return 2;
				}
			} else {
				parser->state.n = 0;
				parser->loc->ptr = oops;
				parser->loc->col = oopscol;
			}
			break;
		}
	}
	pjump(parse_rest);
	return 0;
}

/* Parse a command */

/* Construct command and return */

static int is_simple(Ivy_node *n)
{
	if (n->what == ivy_nNAM)
		return 1;
	else if (n->what == ivy_nCALL1)
		return is_simple(n->l) && is_simple(n->r);
	else
		return 0;
}

static int parse_cmd_done(Ivy_parser *parser)
{
	Ivy_what *k;
	PTRACE("parse_cmd_done");
	// Add block arg to arg
	if (parser->state.blk) {
		parser->state.blk = ivy_cons1(parser->loc,ivy_nPAREN, parser->state.blk);
		if (parser->state.args)
			parser->state.args = ivy_cons2(parser->loc,ivy_nSEMI, parser->state.args, parser->state.blk);
		else
			parser->state.args = parser->state.blk;
	}
	// Construct command: construct statement node if we're calling a keyword
	if (parser->state.cmd->what == ivy_nNAM && (k = kw(parser->state.cmd->s))) {
		parser->rtn = ivy_cons1(parser->loc, k->what, ivy_opt(parser->loc, parser->state.args));
		/* rm(parser->state.cmd); */
	} else if (is_simple(parser->state.cmd))
		/* Only treat simple names as commands */
		parser->rtn = ivy_cons2(parser->loc, ivy_nCALL, parser->state.cmd, ivy_opt(parser->loc, parser->state.args));
	else if (parser->state.args)
		parser->rtn = ivy_cons2(parser->loc, ivy_nSEMI, parser->state.cmd, parser->state.args);
	else
		parser->rtn = parser->state.cmd;
	// We're done!
	pret(parser->rtn);
	return 0;
}

/* Add parsed command to block */

static int parse_cmd_4(Ivy_parser *);

static int parse_cmd_5(Ivy_parser *parser)
{
	PTRACE("parse_cmd_5");
	if (parser->rtn) {
		if (parser->state.blk)
			parser->state.blk = ivy_cons2(parser->loc,ivy_nSEMI, parser->state.blk, parser->rtn);
		else
			parser->state.blk = parser->rtn;
		pjump(parse_cmd_4);
	} else {
		// Something is there... it must be an error
		/* rm(parser->loc, parser->state.blk); */
		/* rm(parser->loc, parser->state.args); */
		/* rm(parser->loc, parser->state.cmd); */
		pret(NULL);
	}
	return 0;
}

/* Parse next command in block */

static int parse_cmd(Ivy_parser *parser);

static int parse_cmd_2(Ivy_parser *);

static int parse_cmd_4(Ivy_parser *parser)
{
	PTRACE("parse_cmd_4");
	skipws(parser->loc);
	if (*parser->loc->ptr) {
		if (parser->loc->lvl > parser->state.blvl) {
			PTRACE("parse_cmd_4 more indent, starting block")
			pcall(parse_cmd,parse_cmd_5,0);
		} else {
			PTRACE("parse_cmd_4 same or less indent, block done")
			return parse_cmd_done(parser);
		}
	} else if (parser->loc->eof) {
		return parse_cmd_done(parser);
	}
	return 0;
}

/* If we parsed an expression, add it to argument list and look for another. */

static int parse_cmd_3(Ivy_parser *parser)
{
	PTRACE("parse_cmd_3");
	if (parser->rtn) {
		/* Note line number expression ended on */
		if (parser->loc->col == parser->loc->lvl || !parser->loc->col) {
			/* We didn't take anything from this line, so assume it ended on previous one */
			parser->state.line = parser->loc->line - 1;
		} else {
			// printf("take %d %d\n",parser->loc->col, parser->loc->lvl);
			/* Expression took from this line so it's the last */
			parser->state.line = parser->loc->line;
		}
		if (parser->state.args)
			parser->state.args = ivy_cons2(parser->loc,ivy_nSEMI, parser->state.args, parser->rtn);
		else
			parser->state.args = parser->rtn;
		pjump(parse_cmd_2);
	} else {
		/* Bad expression or ; finish with this command */
		parser->state.blk = 0;
		return parse_cmd_done(parser);
	}
	return 0;
}

/* Parse next argument.  If line empty, jump into parse block loop. */

static int parse_cmd_2(Ivy_parser *parser)
{
	PTRACE("parse_cmd_2");
	skipwsc(parser->loc);
	// printf("(%d %d)\n",parser->state.line, parser->loc->line);
	if (*parser->loc->ptr && parser->state.line == parser->loc->line) {
		/* Something else is on the same line that we got previous arg or command */
		pcall(parse_expr, parse_cmd_3, 0);
	} else {
		/* Maybe we have a block.. */
		parser->state.blk = 0;
/* Allow indentation to signal blocks */
#if 0
		pjump(parse_cmd_4);
#else
		return parse_cmd_done(parser);
#endif
	}
	return 0;
}

/* If we parsed an expression, jump into parse arguments loop.  Otherwise
   return 0 */

static int parse_cmd_1(Ivy_parser *parser)
{
	PTRACE("parse_cmd_1");
	if (parser->rtn) {
		/* Note line number expression ended on */
		if (parser->loc->col == parser->loc->lvl || !parser->loc->col) {
			/* We didn't take anything from this line, so assume it ended on previous one */
			parser->state.line = parser->loc->line - 1;
		} else {
			/* Expression took from this line so it's the last */
			// printf("take\n");
			parser->state.line = parser->loc->line;
		}
		/* Assume expression is the command.. */
		parser->state.cmd = parser->rtn;
		parser->state.args = 0;
		parser->state.last = 0;
		pjump(parse_cmd_2);
	} else {
		pret(NULL);
	}
	return 0;
}

/* Skip whitespace and ;s.  Record indentation level.  Parse an expression. */

static int parse_cmd(Ivy_parser *parser)
{
	PTRACE("parse_cmd");
	skipwss(parser->loc);
	if (*parser->loc->ptr) {
		parser->state.blvl = parser->loc->lvl;
		pcall(parse_expr,parse_cmd_1,0);
	} else {
		// No data, return nothing.
		pret(NULL);
	}
	return 0;
}

static int parse_paren_2(Ivy_parser *);

static int parse_paren_1(Ivy_parser *parser)
{
	PTRACE("parse_paren_1");
	skipwss(parser->loc);
	if (!*parser->loc->ptr) {
		if (parser->loc->eof)
			pret(parser->state.n);
		return 0;
	}
	pcall(parse_cmd,parse_paren_2,0);
	return 0;
}

static int parse_paren_2(Ivy_parser *parser)
{
	PTRACE("parse_paren_2");
	if (parser->rtn) {
		if (parser->state.n)
			parser->state.n = ivy_cons2(parser->loc,ivy_nSEMI, parser->state.n, parser->rtn);
		else
			parser->state.n = parser->rtn;
		pjump(parse_paren_1);
		return 0;
	} else {
		pret(parser->state.n);
		return 0;
	}
}

/* Parse a list of commands.
 * This is used in: (commands), [commands]
 * - including the argument list in 'fn (commands) body'
 */

static int parse_paren(Ivy_parser *parser)
{
	PTRACE("parse_paren");
	parser->state.n = 0;
	pjump(parse_paren_1);
	return 0;
}

static int parse_lst_2(Ivy_parser *);

static int parse_lst_1(Ivy_parser *parser)
{
	PTRACE("parse_lst_1");

	/* skipws(parser->loc); */
	skipwssc(parser->loc);
	if (!*parser->loc->ptr) {
		if (parser->loc->eof)
			pret(parser->state.n);
		return 0;
	}
	pcall(parse_expr,parse_lst_2,0);
	return 0;
}

static int parse_lst_2(Ivy_parser *parser)
{
	PTRACE("parse_lst_2");
	if (parser->rtn) {
		if (parser->state.n)
			parser->state.n = ivy_cons2(parser->loc, ivy_nSEMI, parser->state.n, parser->rtn);
		else
			parser->state.n = parser->rtn;
		pjump(parse_lst_1);
		return 0;
	} else {
		pret(parser->state.n);
		return 0;
	}
}

/* Parse a list of expressions
 * This is used in func(list), func[list], and { list }
 */

static int parse_lst(Ivy_parser *parser)
{
	PTRACE("parse_lst");
	parser->state.n = 0;
	pjump(parse_lst_1);
	return 0;
}

/* Parse list idle */

static int parse_lst_idle_1(Ivy_parser *parser);

static int parse_lst_idle(Ivy_parser *parser)
{
	PTRACE("parse_lst_idle");
	pcall(parse_lst,parse_lst_idle_1,0);
	return 0;
}

static int parse_lst_idle_1(Ivy_parser *parser)
{
	PTRACE("parse_lst_idle_1");
	if (parser->rtn) {
		pjump(parse_lst_idle);
		return 1;
	} else if (*parser->loc->ptr) {
		ivy_error_3(parser->err,"\"%s\" %d: Unexpected character '%c'",parser->loc->name,parser->loc->line,*parser->loc->ptr);
		++parser->loc->ptr;
		++parser->loc->col;
		pjump(parse_lst_idle);
	} else {
		/* We did nothing */
		pjump(parse_lst_idle);
	}
	return 0;
}

/* Idle state */

static int parse_idle_1(Ivy_parser *);

static int parse_idle(Ivy_parser *parser)
{
	PTRACE("parse_idle");
	skipws(parser->loc);
	if (*parser->loc->ptr)
		pcall(parse_cmd,parse_idle_1,0);
	return 0;
}

static int parse_idle_1(Ivy_parser *parser)
{
	PTRACE("parse_idle_1");
	if (parser->rtn) {
		pjump(parse_idle);
		return 1; /* Top level should take return value */
	} else {
		if (*parser->loc->ptr) {
			ivy_error_3(parser->err,"\"%s\" %d: Unexpected character '%c'",parser->loc->name,parser->loc->line,*parser->loc->ptr);
			++parser->loc->ptr;
			++parser->loc->col;
			pjump(parse_idle);
			return 1; // Clear error before proceeding
		} else {
			/* We did nothing */
			pjump(parse_idle);
		}
	}
	return 0;
}

/* Parse a line */

Ivy_val ivy_parse(Ivy *ivy, Ivy_parser *parser, const char *text, int unasm, int ptree, int ptop, int norun, int trace)
{
	Ivy_val rtn_val;
	rtn_val.type = ivy_tERROR;
	parser->loc->ptr = text;
	parser->loc->col = 0;
	parser->loc->lvl = -1;
	++parser->loc->line;
	PTRACE("Parse: giving a line");
	/* Skip blank lines if we are waiting for more input, keep need_more set */
	if (parser->need_more == 2) {
		skipws(parser->loc);
		if (!*parser->loc->ptr)
			return rtn_val;
	}
	parser->need_more = 0;
	/* Keep feeding parser as long as we have data */
	while (*parser->loc->ptr || (parser->loc->eof && parser->state.state != parse_idle)) {
		int sts = parser->state.state(parser);
		if (sts == 1) { /* We completed parsing something */
			Ivy_pseudo *code;
			if (ptree && parser->rtn) ivy_prtree(ivy->out, parser->rtn, 0);
			if (!parser->err->error_flag) {
				code = ivy_codegen(parser->err, parser->rtn);
				if (unasm) ivy_disasm(ivy->out, code, 0, 0);
				if (!parser->err->error_flag) {
					if (!norun)
						rtn_val = ivy_run(ivy, code, ptop, trace);
				} else {
					ivy_error_0(parser->err, "There were code generator errors- not executing");
					parser->err->error_flag = 0;
				}
			} else {
				// printf("Syntax errors...\n");
				ivy_error_0(parser->err, "There were syntax errors- not executing");
				parser->err->error_flag = 0;
			}
			/* rm(parser->loc, parser->rtn); */
			/* Fast free: */
			ivy_free_all(parser->free_list);
		} else if (sts == 2 || sts == 3) { /* Get more input if nothing else is on the line */
			skipws(parser->loc);
			if (!*parser->loc->ptr) {
				parser->need_more = sts;
			}
		}
	}
	if (parser->paren_level)
		parser->need_more = 2;
	return rtn_val;
}

Ivy_val ivy_parse_done(Ivy *ivy, Ivy_parser *parser, int unasm, int ptree, int ptop, int norun, int trace)
{
	Ivy_val rtn_val;
	rtn_val.type = ivy_tERROR;
	if (!parser->need_more) {
		parser->loc->eof = 1;
		rtn_val = ivy_parse(ivy, parser, "", unasm, ptree, ptop, norun, trace);
		parser->loc->eof = 0;
		parser->loc->line = 0;
		parser->err->error_flag = 0; /* All errors printed at this point */
	}
	return rtn_val;
}

/* Create a parser */

Ivy_parser *ivy_create_parser(Ivy *ivy, const char *file_name)
{
	Ivy_parser *parser = (Ivy_parser *)malloc(sizeof(Ivy_parser));
	parser->ivy = ivy;
	parser->err = ivy->errprn;

	ivy_create_allocator(parser->free_list, sizeof(Ivy_node));

	parser->loc->ptr = "";
	parser->loc->col = 0;
	parser->loc->lvl = -1;
	parser->loc->line = 0;
	parser->loc->name = file_name;
	parser->loc->eof = 0;
	parser->loc->free_list = parser->free_list;

	parser->paren_level = 0;
	parser->need_more = 0;

	parser->state.state = parse_idle;
	parser->state.next = 0;
	parser->str_siz = 80;
	parser->str_buf = (char *)malloc(parser->str_siz);
	return parser;
}

/* Delete a parser */

void ivy_free_parser(Ivy_parser *parser)
{
	ivy_free_allocator(parser->free_list);
	free(parser->str_buf);
	free(parser);
}

/* Compile argument string into a tree */

Ivy_node *ivy_compargs(Ivy *ivy, const char *buf)
{
	Ivy_node *rtn = 0;
	Ivy_parser *parser = ivy_create_parser(ivy, "builtins");
	parser->loc->ptr = buf;
	parser->loc->col = 0;
	parser->loc->lvl = -1;
	++parser->loc->line;
	parser->state.state = parse_lst_idle;
	parser->loc->eof = 1;
	/* Keep feeding parser as long as we have data */
	while (*parser->loc->ptr || parser->state.state != parse_lst_idle) {
		if (1 == parser->state.state(parser)) {
			rtn = parser->rtn;
			break;
		}
	}
	
	rtn = ivy_opt(parser->loc,rtn);
	ivy_free_parser(parser);
	return rtn;
}
