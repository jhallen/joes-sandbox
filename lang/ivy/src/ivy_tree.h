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

typedef struct node Node;
typedef struct what What;

/* Node type codes: must be in same order as corresponding structures in what_tab */

enum {
	nNAM, nNUM, nFP, nSTR, nQUOTE, nCALL1, nCALL, /* nCALL2, */ nPRINC, nPRDEC,
	nPOINC, nPODEC, nCOM, nNEG, nNOT, nAT, nADDR, nSHR, nSHL, nMUL, nDIV, nMOD,
	nAND, nADD, nSUB, nOR, nXOR, nEQ, nNE, nLT, nGT, nLE, nGE, nLAND,
	nLOR, nCOND, nSWITCH, nSET, nDOTTO, nSHLTO, nSHRTO, nMULTO, nDIVTO, nMODTO, nANDTO,
	nADDTO, nSUBTO, nXORTO, nORTO, nPOST, nDOTPO, nSHLPO, nSHRPO, nMULPO,
        nDIVPO, nMODPO, nANDPO, nADDPO, nSUBPO, nXORPO, nORPO, nCOMMA, nSEMI,
        nLAMBDA, nDEFUN, nLIST, nPAREN, nVOID, nTHIS, nEMPTY, nLABEL, nIF, nFOR, nWHILE,
        nLOCAL, nELSE, nLOOP, nUNTIL, nBREAK, nCONT, nRETURN, nFOREACH, nFORINDEX
};

/* Parse tree nodes */

struct node {
	int what;		/* What type of node this is */
	Node *l;		/* Left operand pointer */
	Node *r;		/* Right (single) operand pointer */
	char *s;		/* If node is a string */
	long long n;		/* If node is an integer or size of string */
	double fp;		/* If node is floating point constant */

	Loc loc[1];		/* Location */
};

/* Node type structure */

struct what {
	int what;		/* Code for this node type */
	char *name;		/* How it's scanned/printed */
	int prefix;		/* Context dependent alternatives */
	int infix;
	int postfix;
	int prec;		/* Precidence */
	int assoc;		/* Set for right-associative */
	int meth;		/* Method to build parse tree out of it */
	int i;			/* Which instruction to use */
	int inst;		/* Which nodes type to use (for some meths) */
	What *next;		/* Next node type structure with same hash value */
};

/* Table of node type structures */

extern What what_tab[];

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

Node *cons1(Loc *, int, Node *);

Node *cons2(Loc *, int, Node *, Node *);

Node *consnum(Loc *, long long);

Node *consfp(Loc *, double);

Node *conss(Loc *, int, char *s, int len);

Node *consstr(Loc *, char *s, int len);

Node *consnam(Loc *, char *s);

Node *conslabel(Loc *, char *s);

Node *consvoid(Loc *);
Node *consthis(Loc *);

Node *consempty(Loc *);

Node *opt(Loc *, Node * a);		// If a is NULL, return consempty() otherwise return a */

/* Tree duplicator */
Node *dup(Loc *, Node *);

/* Tree eliminator */
void rm(Loc *, Node *);

/* Print tree */
void indent(FILE *out, int lvl);
void prtree(FILE *out, Node *n, int lvl);

Node *first(Node *);
Node *rest(Node *);

#endif
