/* Ivy syntax trees

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

#ifndef _Iivy_tree
#define _Iivy_tree 1

#include "ivy_loc.h"

typedef struct ivy_node Ivy_node;
typedef struct ivy_what Ivy_what;

/* Node type codes: must be in same order as corresponding structures in what_tab */

enum {
	ivy_nNAM, ivy_nNUM, ivy_nFP, ivy_nSTR, ivy_nQUOTE, ivy_nELLIPSIS, ivy_nCALL1, ivy_nENV, ivy_nCALL, ivy_nPRINC, ivy_nPRDEC,
	ivy_nPOINC, ivy_nPODEC, ivy_nCOM, ivy_nNEG, ivy_nNOT, ivy_nAT, ivy_nADDR, ivy_nSHR, ivy_nSHL, ivy_nMUL, ivy_nDIV, ivy_nMOD,
	ivy_nAND, ivy_nADD, ivy_nSUB, ivy_nOR, ivy_nXOR, ivy_nEQ, ivy_nNE, ivy_nLT, ivy_nGT, ivy_nLE, ivy_nGE, ivy_nLAND,
	ivy_nLOR, ivy_nSET, ivy_nDOTTO, ivy_nSHLTO, ivy_nSHRTO, ivy_nMULTO, ivy_nDIVTO, ivy_nMODTO, ivy_nANDTO,
	ivy_nADDTO, ivy_nSUBTO, ivy_nXORTO, ivy_nORTO, ivy_nPOST, ivy_nDOTPO, ivy_nSHLPO, ivy_nSHRPO, ivy_nMULPO,
        ivy_nDIVPO, ivy_nMODPO, ivy_nANDPO, ivy_nADDPO, ivy_nSUBPO, ivy_nXORPO, ivy_nORPO, ivy_nCOMMA, ivy_nSEMI,
        ivy_nDEFUN, ivy_nLIST, ivy_nPAREN, ivy_nVOID, ivy_nTHIS, ivy_nEMPTY, ivy_nLABEL, ivy_nIF, ivy_nFOR, ivy_nWHILE,
        ivy_nVAR, ivy_nLOOP, ivy_nUNTIL, ivy_nBREAK, ivy_nCONT, ivy_nRETURN, ivy_nFOREACH, ivy_nFORINDEX, ivy_nSCOPE
};

/* Parse tree nodes */

struct ivy_node {
	int what;		/* What type of node this is */
	Ivy_node *l;		/* Left operand pointer */
	Ivy_node *r;		/* Right (single) operand pointer */
	char *s;		/* If node is a string */
	long long n;		/* If node is an integer or size of string */
	double fp;		/* If node is floating point constant */

	Ivy_loc loc[1];		/* Location */
};

/* Node type structure */

struct ivy_what {
	int what;		/* Code for this node type */
	const char *name;	/* How it's scanned/printed */
	int prefix;		/* Context dependent alternatives */
	int infix;
	int postfix;
	int prec;		/* Precidence */
	int assoc;		/* Set for right-associative */
	int meth;		/* Method to build parse tree out of it */
	int i;			/* Which instruction to use */
	int inst;		/* Which nodes type to use (for some meths) */
	Ivy_what *next;		/* Next node type structure with same hash value */
};

/* Table of node type structures */

extern Ivy_what ivy_what_tab[];

/* Methods for operators:
 *  1 2		No. args for operator.  Can be ORed with one of the following:
 *  4		Make operator into an assignment
 *               if operator is '*', it becomes '*='
 *  8		Post operation
 *               if operator is '*=' if becomes '*:'
 *  16          Cond
 *  32          Switch
 *  64		Parse second arg up to ), use precidence of 0 for right side
 *               I.E., arg is a function call
 *
 * Methods for statements:
 *  1 if expr [block]
 *  2 loop [block]
 *  3 until expr
 *  4 break [expr]
 *  5 foreach expr expr [block]
 *  6 include "string"
 */

/* Tree related functions */

/* Tree constructors */

Ivy_node *ivy_cons1(Ivy_loc *, int, Ivy_node *);

Ivy_node *ivy_cons2(Ivy_loc *, int, Ivy_node *, Ivy_node *);

Ivy_node *ivy_consnum(Ivy_loc *, long long);

Ivy_node *ivy_consfp(Ivy_loc *, double);

Ivy_node *ivy_conss(Ivy_loc *, int, char *s, int len);

Ivy_node *ivy_consstr(Ivy_loc *, char *s, int len);

Ivy_node *ivy_consnam(Ivy_loc *, char *s);

Ivy_node *ivy_conslabel(Ivy_loc *, char *s);

Ivy_node *ivy_consvoid(Ivy_loc *);
Ivy_node *ivy_consthis(Ivy_loc *);

Ivy_node *ivy_consempty(Ivy_loc *);

Ivy_node *ivy_opt(Ivy_loc *, Ivy_node * a);		// If a is NULL, return consempty() otherwise return a */

/* Tree duplicator */
Ivy_node *ivy_dup_tree(Ivy_loc *, Ivy_node *);

/* Print tree */
void ivy_indent(FILE *out, int lvl);
void ivy_prtree(FILE *out, Ivy_node *n, int lvl);

Ivy_node *ivy_first(Ivy_node *);
Ivy_node *ivy_rest(Ivy_node *);

#endif
