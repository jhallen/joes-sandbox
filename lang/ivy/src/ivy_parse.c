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
#include "atom.h"
#include "free_list.h"
#include "ivy_tree.h"
#include "ivy.h"

/* Skip over whitespace */

void skipws(Loc *loc)
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

void skipwss(Loc *loc)
{
	while(skipws(loc), (*loc->ptr == ';')) {
		++loc->ptr;
		++loc->col;
	}
}

/* Skip whitespace including ; and , */

void skipwssc(Loc *loc)
{
	while(skipws(loc), (*loc->ptr == ';' || *loc->ptr == ',')) {
		++loc->ptr;
		++loc->col;
	}
}

/* Skip whitespace including , */

void skipwsc(Loc *loc)
{
	while(skipws(loc), (*loc->ptr == ',')) {
		++loc->ptr;
		++loc->col;
	}
}

/* Table of operators and parse-tree node types */

/*               NAME    PREFIX  INFIX   POSTFIX PREC    ASSOC   METH    INST    Atom */
What what_tab[]=
{
	/* Constants */
	{ nNAM, "nam", 0, 0, 0, 120, 0, 0, 0, 0 },
	{ nNUM, "num", 0, 0, 0, 120, 0, 0, 0, 0 },
	{ nFP, "fp", 0, 0, 0, 120, 0, 0, 0, 0 },
	{ nSTR, "str", 0, 0, 0, 120, 0, 0, 0, 0 },

	/* Operators */
	{ nQUOTE, "`", nQUOTE, 0, 0, 120, 0, 1, 0, nQUOTE },

	{ nCALL1, ".", 0, nCALL1, 0, 110, 0, 2, 0, 0 },

	{ nCALL, "(", 0, nCALL, 0, 100, 0, 66, 0, 0 },

//	{ nCALL2, "[", 0, nCALL, 0, 100, 0, 130, 0, 0 },

	{ nPRINC, "++", nPRINC, 0, nPOINC, 90, 0, 5, 0, nADD },
	{ nPRDEC, "--", nPRDEC, 0, nPODEC, 90, 0, 5, 0, nSUB },
	{ nPOINC, "++", nPRINC, 0, nPOINC, 90, 0, 13, 0, nADD },
	{ nPODEC, "--", nPRDEC, 0, nPODEC, 90, 0, 13, 0, nSUB },
	{ nCOM, "~", nCOM, 0, 0, 90, 0, 1, iCOM, 0 },
	{ nNEG, "neg", nNEG, nSUB, 0, 90, 0, 1, iNEG, 0 },
	{ nNOT, "!", nNOT, 0, 0, 90, 0, 1, 0, 0 },
	{ nAT, "*", nAT, nMUL, 0, 90, 0, 1, iAT, 0 },
	{ nADDR, "&", nADDR, nAND, 0, 90, 0, 1, 0, 0 },

	{ nSHR, ">>", 0, nSHR, 0, 80, 0, 2, iSHR, 0 },
	{ nSHL, "<<", 0, nSHL, 0, 80, 0, 2, iSHL, 0 },

	{ nMUL, "*", nAT, nMUL, 0, 70, 0, 2, iMUL, 0 },
	{ nDIV, "/", 0, nDIV, 0, 70, 0, 2, iDIV, 0 },
	{ nMOD, "%", nMOD, nMOD, 0, 70, 0, 2, iMOD, 0 },
	{ nAND, "&", nADDR, nAND, 0, 70, 0, 2, iAND, 0 },

	{ nADD, "+", 0, nADD, 0, 60, 0, 2, iADD, 0 },
	{ nSUB, "-", nNEG, nSUB, 0, 60, 0, 2, iSUB, 0 },
	{ nOR, "|", 0, nOR, 0, 60, 0, 2, iOR, 0 },
	{ nXOR, "^", 0, nXOR, 0, 60, 0, 2, iXOR, 0 },

	{ nEQ, "==", 0, nEQ, 0, 50, 0, 2, iBEQ, 0 },
	{ nNE, "!=", 0, nNE, 0, 50, 0, 2, iBNE, 0 },
	{ nLT, "<", 0, nLT, 0, 50, 0, 2, iBLT, 0 },
	{ nGT, ">", 0, nGT, 0, 50, 0, 2, iBGT, 0 },
	{ nLE, "<=", 0, nLE, 0, 50, 0, 2, iBLE, 0 },
	{ nGE, ">=", 0, nGE, 0, 50, 0, 2, iBGE, 0 },

	{ nLAND, "&&", 0, nLAND, 0, 40, 0, 2, 0, 0 },

	{ nLOR, "||", 0, nLOR, 0, 30, 0, 2, 0, 0 },

	{ nCOND, "?", nCOND, nSWITCH, 0, 25, 0, 17, 0, nIF },
	{ nSWITCH, "?", nCOND, nSWITCH, 0, 25, 0, 34, 0, nSWITCH },

	{ nSET, "=", 0, nSET, 0, 20, 1, 2, iSET, 0 },
	{ nDOTTO, ".=", 0, nDOTTO, 0, 20, 1, 6, 0, nCALL1 },
	{ nSHLTO, "<<=", 0, nSHLTO, 0, 20, 1, 6, 0, nSHL },
	{ nSHRTO, ">>=", 0, nSHRTO, 0, 20, 1, 6, 0, nSHR },
	{ nMULTO, "*=", 0, nMULTO, 0, 20, 1, 6, 0, nMUL },
	{ nDIVTO, "/=", 0, nDIVTO, 0, 20, 1, 6, 0, nDIV },
	{ nMODTO, "%=", 0, nMODTO, 0, 20, 1, 6, 0, nMOD },
	{ nANDTO, "&=", 0, nANDTO, 0, 20, 1, 6, 0, nAND },
	{ nADDTO, "+=", 0, nADDTO, 0, 20, 1, 6, 0, nADD },
	{ nSUBTO, "-=", 0, nSUBTO, 0, 20, 1, 6, 0, nSUB },
	{ nXORTO, "^=", 0, nXORTO, 0, 20, 1, 6, 0, nXOR },
	{ nORTO, "|=", 0, nORTO, 0, 20, 1, 6, 0, nOR },
	{ nPOST, ":", 0, nPOST, 0, 20, 1, 10, 0, 0 },
	{ nDOTPO, ".:", 0, nDOTPO, 0, 20, 1, 14, 0, nCALL1 },
	{ nSHLPO, "<<:", 0, nSHLPO, 0, 20, 1, 14, 0, nSHL },
	{ nSHRPO, ">>:", 0, nSHRPO, 0, 20, 1, 14, 0, nSHR },
	{ nMULPO, "*:", 0, nMULPO, 0, 20, 1, 14, 0, nMUL },
	{ nDIVPO, "/:", 0, nDIVPO, 0, 20, 1, 14, 0, nDIV },
	{ nMODPO, "%:", 0, nMODPO, 0, 20, 1, 14, 0, nMOD },
	{ nANDPO, "&:", 0, nANDPO, 0, 20, 1, 14, 0, nAND },
	{ nADDPO, "+:", 0, nADDPO, 0, 20, 1, 14, 0, nADD },
	{ nSUBPO, "-:", 0, nSUBPO, 0, 20, 1, 14, 0, nSUB },
	{ nXORPO, "^:", 0, nXORPO, 0, 20, 1, 14, 0, nXOR },
	{ nORPO, "|:", 0, nORPO, 0, 20, 1, 14, 0, nOR },

	{ nCOMMA, "\\", 0, nCOMMA, 0, 15, 0, 2, 0, 0 },

	{ nSEMI, ";", 0, nSEMI, 0, 10, 0, 2, 0, 0 },

	/* A function definition */
//	{ nDEFUN, "FUN", 0, nDEFUN, 0, 5, 0, 2, 0, nDEFUN },
	{ nLAMBDA, "lambda", 0, 0, 0, 0, 0, 0, 0, 0 },

	{ nDEFUN, "fn", 0, 0, 0, 0, 0, 0, 0, 0 },

	/* A list [...] */
	{ nLIST, "LST", 0, 0, 0, 0, 0, 1, 0, 0 },

	/* Parenthasis */
	{ nPAREN, "paren", 0, 0, 0, 0, 0, 1, 0, 0 },

	/* Nothing */
	{ nVOID, "void", 0, 0, 0, 0, 0, 1, 0, 0 },

	/* Current scope */
	{ nTHIS, "this", 0, 0, 0, 0, 0, 1, 0, 0 },

	/* Empty list */
	{ nEMPTY, "empty", 0, 0, 0, 0, 0, 1, 0, 0 },

	/* A label */
	{ nLABEL, "label", 0, 0, 0, 0, 0, 1, 0, 0 },

	/* Statements */
	{ nIF, "if", 0, 0, 0, 0, 0, 1, 0, 0 },
	{ nFOR, "for", 0, 0, 0, 0, 0, 1, 0, 0 },
	{ nWHILE, "while", 0, 0, 0, 0, 0, 1, 0, 0 },
	{ nLOCAL, "var", 0, 0, 0, 0, 0, 1, 0, 0 },
	{ nELSE, "else", 0, 0, 0, 0, 0, 2, 0, 0 },
	{ nLOOP, "loop", 0, 0, 0, 0, 0, 2, 0, 0 },
	{ nUNTIL, "until", 0, 0, 0, 0, 0, 3, 0, 0 },
	{ nBREAK, "break", 0, 0, 0, 0, 0, 4, 0, 0 },
	{ nCONT, "continue", 0, 0, 0, 0, 0, 4, 0, 0 },
	{ nRETURN, "return", 0, 0, 0, 0, 0, 4, 0, 0 },
	{ nFOREACH, "foreach", 0, 0, 0, 0, 0, 5, 0, 0 },
	{ nFORINDEX, "forindex", 0, 0, 0, 0, 0, 5, 0, 0 }
};

/* Operator scanner */

#define OPRTLEN 64
#define OPRLTLEN 128
char oprltab[OPRLTLEN];		/* Hash table of operator left-substrings */
What *oprtab[OPRTLEN];		/* Hash table of operators */
int opr_isinit;			/* Set if table has been initialized */

int ioprtab[] =
{
	nCALL1, nCALL, /* nCALL2, */ nPRINC, nPRDEC, nCOM, nSUB, nNOT, nSHR, nSHL,
	nMUL, nDIV, nMOD, nAND, nADD, nOR, nXOR, nEQ, nNE, nGE, nLE,
	nGT, nLT, nLAND, nLOR, nSET, nDOTTO, nSHLTO, nSHRTO, nMULTO,
	nDIVTO, nMODTO, nANDTO, nADDTO, nSUBTO, nXORTO, nORTO, nPOST,
	nDOTPO, nSHLPO, nSHRPO, nMULPO, nDIVPO, nMODPO, nANDPO, nADDPO,
	nSUBPO, nXORPO, nORPO, nQUOTE, nCOMMA, nCOND, -1
};

void izoprtab(void)
{
	int x;
	int t;
	opr_isinit = 1;
	for (x = 0; (t = ioprtab[x]) != -1; ++x) {
		What *tt = &what_tab[t];
		char *s = tt->name;
		unsigned long accu = 0;
		while (*s) {
			accu = hnext(accu, *s++);
			if (*s)
				++oprltab[accu % OPRLTLEN];
		}
		tt->next = oprtab[accu % OPRTLEN];
		oprtab[accu % OPRTLEN] = tt;
	}
}

int strcmpn(char *blk, int len, char *s)
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

What *doopr(Loc *loc, unsigned long accu, char *start)
{
	unsigned long oaccu = accu;
	char *oops = loc->ptr;
	int oopscol = loc->col;
	What *t;
	do {
		if (!*loc->ptr)
			break;
		accu = hnext(accu, *loc->ptr++);
		for (t = oprtab[accu % OPRTLEN]; t; t = t->next)
			if (!strcmpn(start, loc->ptr - start, t->name)) {
				What *u;
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

What *opr(Loc *loc)
{
	if (!opr_isinit)
		izoprtab();
	return doopr(loc, 0, loc->ptr);
}

/* Get a character which might be an escape sequence */

int escape(Loc *loc)
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

int ikwtab[] =
    { nIF, nELSE, nFOR, nWHILE, nLOOP, nLOCAL, nUNTIL, nBREAK, nCONT, nRETURN, nFOREACH, nFORINDEX, nDEFUN, nLAMBDA, -1 };

#define KWHTLEN 32
What *kwhtab[KWHTLEN];
int kw_isinit = 0;

void izkwtab(void)
{
	int x;
	kw_isinit = 1;
	for (x = 0; ikwtab[x] != -1; ++x) {
		char *s = atom_add(what_tab[ikwtab[x]].name);
		unsigned long hval = ahash(s);
		what_tab[ikwtab[x]].name = s; /* Replace name with atom */
		what_tab[ikwtab[x]].next = kwhtab[hval % KWHTLEN];
		kwhtab[hval % KWHTLEN] = &what_tab[ikwtab[x]];
	}
}

/* Look up a keyword by atom */

What *kw(char *atom)
{
	What *a;
	if (!atom)
		return 0;
	if (!kw_isinit)
		izkwtab();
	for (a = kwhtab[ahash(atom) % KWHTLEN]; a; a = a->next)
		if (a->name == atom)
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
	Parse_state *stk; \
	stk = (Parse_state *)malloc(sizeof(Parse_state)); \
	parser->state.state = (retn); \
	*stk = parser->state; \
	parser->state.next = stk; \
	parser->state.prec = (nprec); \
	parser->state.state = (subr); \
} while(0)

/* Return with a node */

#define pret(n) do { \
	Parse_state *stk = parser->state.next; \
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
		parser->str_buf = realloc(parser->str_buf,parser->str_siz); \
} while(0)

int parse_rest(Parser *);

int parse_rest_done_infix(Parser *parser)
{
	PTRACE("parse_rest_done_infix");
	parser->state.n = cons2(parser->loc,parser->state.op->what, parser->state.n, parser->rtn);
	pjump(parse_rest);
	return 0;
}

/* Make op= or op: assignment */

int parse_rest_done_infix1(Parser *parser)
{
	PTRACE("parse_rest_done_infix1");
	parser->state.n = cons2(parser->loc,(parser->state.op->meth & 8 ? nPOST : nSET), parser->state.n, cons2(parser->loc,parser->state.op->inst, dup(parser->loc, parser->state.n), parser->rtn));
	pjump(parse_rest);
	return 0;
}

int parse_rest_done_call(Parser *parser)
{
	What *k;
	PTRACE("parse_rest_done_call");
	if (parser->state.op->what == nCALL && parser->state.n->what == nNAM && (k = kw(parser->state.n->s))) {
		/* rm(parser->state.n); */
		parser->state.n = cons1(parser->loc, k->what, opt(parser->loc, parser->rtn));
	} else
		parser->state.n = cons2(parser->loc, parser->state.op->what, parser->state.n, opt(parser->loc,parser->rtn));
	if (*parser->loc->ptr == '}')
		++parser->loc->ptr, ++parser->loc->col;
	else
		error_2(parser->err,"\"%s\" %d: Error: missing ]",parser->loc->name,parser->loc->line);
	--parser->paren_level;
	pjump(parse_rest);
	return 0;
}

int parse_rest_done_call1(Parser *parser)
{
	What *k;
	PTRACE("parse_rest_done_call1");
	if (parser->state.op->what == nCALL && parser->state.n->what == nNAM && (k = kw(parser->state.n->s))) {
		/* rm(parser->state.n); */
		parser->state.n = cons1(parser->loc, k->what, opt(parser->loc, parser->rtn));
	} else
		parser->state.n = cons2(parser->loc, parser->state.op->what, parser->state.n, opt(parser->loc,parser->rtn));
	if (*parser->loc->ptr == ')')
		++parser->loc->ptr, ++parser->loc->col;
	else
		error_2(parser->err,"\"%s\" %d: Error: missing )",parser->loc->name,parser->loc->line);
	--parser->paren_level;
	pjump(parse_rest);
	return 0;
}

/* Try to parse an infix or postfix operator */

int parse_lst(Parser *);
int parse_expr(Parser *);

int parse_rest(Parser *parser)
{
	char *oops = parser->loc->ptr;
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
	    (what_tab[parser->state.op->infix].prec > parser->state.prec ||
	     (what_tab[parser->state.op->infix].prec == parser->state.prec && what_tab[parser->state.op->infix].assoc))) {
		if (parser->state.op->meth & 128) {	/* Function call? */
			parser->state.op = &what_tab[parser->state.op->infix];
			++parser->paren_level;
			PTRACE("parse_rest pcall parse_lst");
			pcall(parse_lst,parse_rest_done_call,0);
			return 0;
		} else if (parser->state.op->meth & 64) {	/* Function call? */
			parser->state.op = &what_tab[parser->state.op->infix];
			++parser->paren_level;
			PTRACE("parse_rest pcall parse_lst1");
			pcall(parse_lst,parse_rest_done_call1,0);
			return 0;
		} else {
			parser->state.op = &what_tab[parser->state.op->infix];
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
	           (what_tab[parser->state.op->postfix].prec > parser->state.prec ||
	            (what_tab[parser->state.op->postfix].prec == parser->state.prec && what_tab[parser->state.op->postfix].assoc))) {
		parser->state.op = &what_tab[parser->state.op->postfix];
		if (parser->state.op->meth & 4)	/* Make into an assignment? */
			parser->state.n = cons2(parser->loc,nPOST, parser->state.n,
			              cons2(parser->loc,parser->state.op->inst, dup(parser->loc, parser->state.n), consnum(parser->loc,1)));
		else
			parser->state.n = cons1(parser->loc,parser->state.op->what, parser->state.n);	/* Normal case */
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

int parse_expr_done_prefix(Parser *parser)
{
	PTRACE("parse_expr_done_prefix");
	parser->state.n = cons1(parser->loc,parser->state.op->what, parser->rtn);
	pjump(parse_rest);
	return 0;
}

int parse_expr_done_prefix1(Parser *parser)
{
	PTRACE("parse_expr_done_prefix1");
	parser->state.n = cons2(parser->loc,nSET, parser->rtn, cons2(parser->loc,parser->state.op->inst, dup(parser->loc, parser->rtn), consnum(parser->loc,1)));
	pjump(parse_rest);
	return 0;
}

int parse_string(Parser *parser)
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
			error_2(parser->err,"\"%s\" %d: Missing closing \"",parser->loc->name,parser->loc->line);
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
	parser->state.n = consstr(parser->loc,memcpy(malloc(parser->str_len+1),parser->str_buf,parser->str_len+1),parser->str_len);
	pjump(parse_rest);
	return 0;
}

int parse_expr_done_paren(Parser *parser)
{
	PTRACE("parse_expr_done_paren");
	parser->state.n = cons1(parser->loc,nPAREN, opt(parser->loc, parser->rtn));
	if (*parser->loc->ptr != ')')
		error_2(parser->err,"\"%s\" %d: Error: missing )",parser->loc->name,parser->loc->line);
	else
		++parser->loc->ptr, ++parser->loc->col;
	--parser->paren_level;
	pjump(parse_rest);
	return 0;
}

int parse_expr_done_square(Parser *parser)
{
	PTRACE("parse_expr_done_square");
	parser->state.n = cons1(parser->loc,nPAREN, parser->rtn);
	if (*parser->loc->ptr != '}')
		error_2(parser->err,"\"%s\" %d: Error: missing }",parser->loc->name,parser->loc->line);
	else
		++parser->loc->ptr, ++parser->loc->col;
	--parser->paren_level;
	pjump(parse_rest);
	return 0;
}

int parse_expr_done_list(Parser *parser)
{
	PTRACE("parse_expr_done_list");
	parser->state.n = cons1(parser->loc,nLIST, opt(parser->loc,parser->rtn));
	if (*parser->loc->ptr != ']')
		error_2(parser->err,"\"%s\" %d: Error: missing ]",parser->loc->name,parser->loc->line);
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

int parse_paren(Parser *parser);

int parse_expr(Parser *parser)
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
				parser->state.n = consnum(parser->loc,num);
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
				parser->state.n = consnum(parser->loc,num);
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
				parser->state.n = consnum(parser->loc,num);
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
				parser->state.n = consnum(parser->loc,num);
			}
			break;
		} case '\'': { /* Character */
			int num = 0;
			++parser->loc->ptr, ++parser->loc->col;
			num = escape(parser->loc);
			if (*parser->loc->ptr == '\'')
				++parser->loc->ptr, ++parser->loc->col;
			else
				error_2(parser->err,"\"%s\" %d: Error missing '",parser->loc->name,parser->loc->line);
			parser->state.n = consnum(parser->loc,num);
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
				parser->state.n = consvoid(parser->loc);
			else if (!strcmp(parser->str_buf, "this"))
				parser->state.n = consthis(parser->loc);
			else
				parser->state.n = consnam(parser->loc, atom_add(parser->str_buf));
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
				parser->state.n = consfp(parser->loc,fp);
				break;
			}
			/* Fall into operator */
		} default: {		/* Operator? */
			char *oops = parser->loc->ptr;
			int oopscol = parser->loc->col;
			parser->state.op = opr(parser->loc);
			if (parser->state.op && parser->state.op->prefix) {
				parser->state.op = &what_tab[parser->state.op->prefix];
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

int is_simple(Node *n)
{
	if (n->what == nNAM)
		return 1;
	else if (n->what == nCALL1)
		return is_simple(n->l) && is_simple(n->r);
	else
		return 0;
}

int parse_cmd_done(Parser *parser)
{
	What *k;
	PTRACE("parse_cmd_done");
	// Add block arg to arg
	if (parser->state.blk) {
		parser->state.blk = cons1(parser->loc,nPAREN, parser->state.blk);
		if (parser->state.args)
			parser->state.args = cons2(parser->loc,nSEMI, parser->state.args, parser->state.blk);
		else
			parser->state.args = parser->state.blk;
	}
	// Construct command: construct statement node if we're calling a keyword
	if (parser->state.cmd->what == nNAM && (k = kw(parser->state.cmd->s))) {
		parser->rtn = cons1(parser->loc, k->what, opt(parser->loc, parser->state.args));
		/* rm(parser->state.cmd); */
	} else if (is_simple(parser->state.cmd))
		/* Only treat simple names as commands */
		parser->rtn = cons2(parser->loc, nCALL, parser->state.cmd, opt(parser->loc, parser->state.args));
	else if (parser->state.args)
		parser->rtn = cons2(parser->loc, nSEMI, parser->state.cmd, parser->state.args);
	else
		parser->rtn = parser->state.cmd;
	// We're done!
	pret(parser->rtn);
	return 0;
}

/* Add parsed command to block */

int parse_cmd_4(Parser *);

int parse_cmd_5(Parser *parser)
{
	PTRACE("parse_cmd_5");
	if (parser->rtn) {
		if (parser->state.blk)
			parser->state.blk = cons2(parser->loc,nSEMI, parser->state.blk, parser->rtn);
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

int parse_cmd(Parser *parser);

int parse_cmd_2(Parser *);

int parse_cmd_4(Parser *parser)
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

int parse_cmd_3(Parser *parser)
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
			parser->state.args = cons2(parser->loc,nSEMI, parser->state.args, parser->rtn);
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

int parse_cmd_2(Parser *parser)
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

int parse_cmd_1(Parser *parser)
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

int parse_cmd(Parser *parser)
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

int parse_paren_2(Parser *);

int parse_paren_1(Parser *parser)
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

int parse_paren_2(Parser *parser)
{
	PTRACE("parse_paren_2");
	if (parser->rtn) {
		if (parser->state.n)
			parser->state.n = cons2(parser->loc,nSEMI, parser->state.n, parser->rtn);
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

int parse_paren(Parser *parser)
{
	PTRACE("parse_paren");
	parser->state.n = 0;
	pjump(parse_paren_1);
	return 0;
}

int parse_lst_2(Parser *);

int parse_lst_1(Parser *parser)
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

int parse_lst_2(Parser *parser)
{
	PTRACE("parse_lst_2");
	if (parser->rtn) {
		if (parser->state.n)
			parser->state.n = cons2(parser->loc, nSEMI, parser->state.n, parser->rtn);
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

int parse_lst(Parser *parser)
{
	PTRACE("parse_lst");
	parser->state.n = 0;
	pjump(parse_lst_1);
	return 0;
}

/* Parse list idle */

int parse_lst_idle_1(Parser *parser);

int parse_lst_idle(Parser *parser)
{
	PTRACE("parse_lst_idle");
	pcall(parse_lst,parse_lst_idle_1,0);
	return 0;
}

int parse_lst_idle_1(Parser *parser)
{
	PTRACE("parse_lst_idle_1");
	if (parser->rtn) {
		pjump(parse_lst_idle);
		return 1;
	} else if (*parser->loc->ptr) {
		error_3(parser->err,"\"%s\" %d: Unexpected character '%c'",parser->loc->name,parser->loc->line,*parser->loc->ptr);
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

int parse_idle_1(Parser *);

int parse_idle(Parser *parser)
{
	PTRACE("parse_idle");
	skipws(parser->loc);
	if (*parser->loc->ptr)
		pcall(parse_cmd,parse_idle_1,0);
	return 0;
}

int parse_idle_1(Parser *parser)
{
	PTRACE("parse_idle_1");
	if (parser->rtn) {
		pjump(parse_idle);
		return 1; /* Top level should take return value */
	} else {
		if (*parser->loc->ptr) {
			error_3(parser->err,"\"%s\" %d: Unexpected character '%c'",parser->loc->name,parser->loc->line,*parser->loc->ptr);
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

Val parse(Ivy *ivy, Parser *parser, char *text, int unasm, int ptree, int ptop, int norun, int trace)
{
	Val rtn_val;
	rtn_val.type = -1;
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
			Pseudo *code;
			if (ptree && parser->rtn) prtree(ivy->out, parser->rtn, 0);
			if (!parser->err->error_flag) {
				code = codegen(parser->err, parser->rtn);
				if (unasm) disasm(ivy->out, code, 0, 0);
				if (!parser->err->error_flag) {
					if (!norun) run(ivy, code, ptop, trace);
				} else {
					error_0(parser->err, "There were code generator errors- not executing");
					parser->err->error_flag = 0;
				}
			} else {
				// printf("Syntax errors...\n");
				error_0(parser->err, "There were syntax errors- not executing");
				parser->err->error_flag = 0;
			}
			/* rm(parser->loc, parser->rtn); */
			/* Fast free: */
			fr_all(parser->free_list);
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

void parse_done(Ivy *ivy, Parser *parser, int unasm, int ptree, int ptop, int norun, int trace)
{
	if (!parser->need_more) {
		parser->loc->eof = 1;
		parse(ivy, parser, "", unasm, ptree, ptop, norun, trace);
		parser->loc->eof = 0;
		parser->loc->line = 0;
		parser->err->error_flag = 0; /* All errors printed at this point */
	}
}

/* Create a parser */

Parser *mkparser(Ivy *ivy, char *file_name)
{
	Parser *parser = (Parser *)malloc(sizeof(Parser));
	parser->ivy = ivy;
	parser->err = ivy->errprn;

	mk_allocator(parser->free_list, sizeof(Node));

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
	parser->str_buf = malloc(parser->str_siz);
	return parser;
}

/* Delete a parser */

void rmparser(Parser *parser)
{
	rm_allocator(parser->free_list);
	free(parser->str_buf);
	free(parser);
}

/* Compile argument string into a tree */

Node *compargs(Ivy *ivy, char *buf)
{
	Node *rtn = 0;
	Parser *parser = mkparser(ivy, "builtins");
	parser->loc->ptr = buf;
	parser->loc->col = 0;
	parser->loc->lvl = -1;
	++parser->loc->line;
	parser->state.state = parse_lst_idle;
	parser->loc->eof = 1;
	/* Keep feeding parser as long as we have data */
	while (*parser->loc->ptr || parser->state.state != parse_lst_idle) {
		if (parser->state.state(parser)) {
			rtn = parser->rtn;
			break;
		}
	}
	
	rtn = opt(parser->loc,rtn);
	rmparser(parser);
	return rtn;
}
