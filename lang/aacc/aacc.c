/* AACC: Ack! Another Compiler Compiler!
   An LALR(1) parser generator with full control over conflict resolution
   Copyright (C) 1994 Joseph H. Allen

This is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation; either version 1, or (at your option) any later version.  

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
more details.

You should have received a copy of the GNU General Public License along with
this program; see the file COPYING.  If not, write to the Free Software
Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *prefix = "yy";
char *type = "int";

typedef struct production Production;
typedef struct symbol Symbol;
typedef struct conf Conf;

/********************************/
/* Production and symbol tables */
/********************************/

/* A production (translation rule)
 *
 * The conventional notation for a production is "A ==> B C D...", where
 * A is some non-terminal symbol and B, C, D, etc. are terminal or non-terminal
 * symbols.  When the parser recognizes the right hand side of the production
 * on its stack, it may replace the recognized string with the non-terminal of
 * the left hand side of the production (a "reduce" action).
 *
 * Each production is stored in an instance of the following structure
 * allocated from the heap.  'left' points to the symbol table entry for
 * the left side of the production (A above) and 'str' is an array of
 * pointers to symbol table entries for the symbols of the right side of the
 * production (B, C, D etc. above).
 */

struct production {
	Production *next;	/* Next production with same left side */
	Symbol *left;		/* Non-terminal symbol on left side of production */
	Symbol **str;		/* The right side of the production */
	int len;		/* Number of symbols in 'str' */
	int line;		/* Source file line number */
	int prodno;		/* Production number (index into rules) */
	char *func;		/* Name of function to call when this prod. is found */
	char *label;		/* The name of this production */
};

/* 'rules' is an array of pointers to all of the productions found in the
 * source file */

Production **rules;	/* Array of ptrs to production structures */
int nrules = 0;		/* No. of productions we have */
int srules;		/* Amount of space allocated in 'rules' */

/* Each terminal (token) and non-terminal symbol encountered in the source
 * file is stored in an instance of one of the following structures allocated
 * from the heap.
 *
 * After the source file has been read in, a pass is made over all of the
 * symbols to number the non-terminals (those symbols appearing on the left
 * side of a production).  This number (stored in 'ntermn') is used as an index
 * into the goto part of the generated Action/Goto table.
 */

struct symbol {
	char *name;		/* Name of this symbol */
	Symbol *next;	/* Next symbol having name with same hash */
	Production *right;	/* This is 0 for terminal symbols; otherwise
					   it points to a linked list of productions
					   for which this symbol is on the left */
	int ntermn;		/* Non-terminal number */
};

/* Conflict resolution command */

struct conf {
	int *prod;		/* Possible productions to reduce */
	int nprods;
	int siz;
	int resolve;		/* How to resolve this conflict, -1 for shift,
				   0..nrules for a reduce by a specific rule */
	char *func;		/* Function name- conflict will be resolved run time */
	int shift;		/* State number if 'shift' is chosen resolution */
	Symbol *e;	/* Lookahead symbol */
};

/* Table of conflict resolution commands */

Conf *conf;
int nconfs = 0;
int confsiz;

/* Hash table of symbols */

#define HTSIZE 128
Symbol *htab[HTSIZE];

/* Lookup a symbol with the given name.  If no such symbol exists, it is
 * created and is assumed to be a terminal symbol.
 */

Symbol *find(char *name)
{
	unsigned long accu = 0;
	Symbol *e;
	int x;
	for (x = 0; name[x]; ++x)
		accu = (accu << 4) + (accu >> 28) + name[x];
	for (e = htab[accu % HTSIZE]; e; e = e->next)
		if (!strcmp(e->name, name))
			return e;
	e = (Symbol *) malloc(sizeof(Symbol));
	e->name = strcpy((char *) malloc(strlen(name) + 1), name);
	e->next = htab[accu % HTSIZE];
	htab[accu % HTSIZE] = e;
	e->right = 0;
	return e;
}

/*********************/
/* Input file reader */
/*********************/

char *ptr;			/* Input line pointer */
int line = 0;			/* Current line number */

/* Skip over whitespace */
void skipws()
{
	while (*ptr == ' ' || *ptr == '\t')
		++ptr;
	if (*ptr == '#')
		*ptr = 0;
	if (*ptr == '\n')
		*ptr = 0;
}

/* Skip over a symbol (stop at next whitespace or end of line) */
void skipwd()
{
	while (*ptr && *ptr != '\n' && *ptr != ' ' && *ptr != '\t'
	       && *ptr != '#' && *ptr != ':' && *ptr != ')')
		++ptr;
	if (*ptr == '#')
		*ptr = 0;
	if (*ptr == '\n')
		*ptr = 0;
}

/* Collect symbols remaining on line into an array.  Returns array with
 * one or more symbols or NULL if there were no symbols.  'nwords' points
 * to an integer which is set to the number of symbols found */
Symbol **getwords(int *nwords)
{
	int siz = 16;		/* Amount allocated for symbol array */
	int len = 0;
	char *p, c;
	Symbol **array =
	    (Symbol **) malloc(sizeof(Symbol *) * siz);

      loop:

	skipws();
	p = ptr;
	skipwd();		/* p points to next symbol */
	if (p != ptr) {		/* If we're not at the end of the line */
		c = *ptr;
		*ptr = 0;	/* Zero-terminate the word */
		if (len == siz)	/* Allocate more space in array if needed */
			array =
			    (Symbol **) realloc(array, sizeof(Symbol *) * (siz += 4));
		array[len++] = find(p);	/* Add symbol to array */
		*ptr = c;
		goto loop;	/* Try for next symbol */
	}
	if (len) {		/* We found some symbols: return them and set nwords with no. we found */
		*nwords = len;
		return array;
	} else {		/* No symbols were found.  Free the array and return 0 */
		free(array);
		return 0;
	}
}

/* Process a line of input (a single production or conflict resolution cmd) */
void process()
{
	char *p, c;
	char *label = 0;
	skipws();
	p = ptr;
	skipwd();		/* p points to first symbol on line- the left
				   side of the production */

	if (*p == ':') {	/* Found a label */
		ptr = p + 1;
		skipws();
		p = ptr;
		skipwd();
		if (p != ptr) {
			c = *ptr;
			*ptr = 0;
			label = strcpy((char *) malloc(strlen(p) + 1), p);
			*ptr = c;
			skipws();
			p = ptr;
			skipwd();
		} else {
			fprintf(stderr, "%d: missing label\n", line);
			return;
		}
	}

	if (ptr != p && *p == '(') {	/* Found a conflict resolution hint */
		int siz = 16;
		if (nconfs == confsiz)
			conf =
			    (Conf *) realloc(conf,
						    sizeof(Conf) *
						    (confsiz += 8));
		conf[nconfs].nprods = 0;
		conf[nconfs].e = 0;
		conf[nconfs].func = 0;
		conf[nconfs].prod = (int *) malloc(sizeof(int) * siz);
		ptr = p + 1;
	      loop:
		skipws();
		p = ptr;
		skipwd();
		if (p != ptr) {
			c = *ptr;
			*ptr = 0;
			if (!strcmp(p, "shift")) {
				if (conf[nconfs].nprods == siz)
					conf[nconfs].prod =
					    (int *) realloc(conf[nconfs].prod, sizeof(int) * (siz += 2));
				conf[nconfs].prod[conf[nconfs].nprods++] = -1;
			} else {
				int z;
				for (z = 0; z != nrules; ++z)
					if (rules[z]->label
					    && !strcmp(rules[z]->label, p))
						break;
				if (z == nrules)
					fprintf(stderr,
						"%d: Unknown label: %s\n",
						line, p);
				else {
					if (conf[nconfs].nprods == siz)
						conf[nconfs].prod =
						    (int *)
						    realloc(conf[nconfs].
							    prod,
							    sizeof(int) *
							    (siz += 2));
					conf[nconfs].prod[conf[nconfs].
							  nprods++] = z;
				}
			}
			*ptr = c;
			goto loop;
		}
		if (*p == ':') {
			ptr = p + 1;
			skipws();
			p = ptr;
			skipwd();
			if (p != ptr) {
				c = *ptr;
				*ptr = 0;
				conf[nconfs].e = find(p);
				*ptr = c;
				goto loop;
			}
		}
		if (*p == ')') {
			ptr = p + 1;
			skipws();
			p = ptr;
			skipwd();
			if (p != ptr) {
				c = *ptr;
				*ptr = 0;
				if (!strcmp(p, "shift"))
					conf[nconfs].resolve =
					    -1, ++nconfs;
				else {
					int z;
					for (z = 0; z != nrules; ++z)
						if (rules[z]->label
						    && !strcmp(rules[z]->
							       label, p))
							break;
					if (z != nrules)
						conf[nconfs].resolve =
						    z, ++nconfs;
					else
						fprintf(stderr,
							"%d: Unknown label %s\n",
							line, p);
				}
				*ptr = c;
			} else if (*p == ':') {
				ptr = p + 1;
				skipws();
				p = ptr;
				skipwd();
				if (p != ptr) {
					c = *ptr;
					*ptr = 0;
					conf[nconfs].func =
					    strcpy((char *)
						   malloc(strlen(p) + 1),
						   p);
					++nconfs;
				} else
					fprintf(stderr,
						"%d: Missing resolution function\n",
						line);
			} else
				fprintf(stderr, "%d: Missing resolution\n",
					line);
		} else
			fprintf(stderr,
				"%d: Missing ) in conflict resolution line\n",
				line);
	} else if (ptr != p) {	/* If this is not a blank line */
		Symbol *e;	/* Symbol for left side */
		Symbol **words;	/* Array of symbols on right side */
		int nwords;
		c = *ptr;
		*ptr = 0;
		e = find(p);
		*ptr = c;
		/* Collect words into an array */
		words = getwords(&nwords);
		if (!words)
			fprintf(stderr, "%d: Incomplete production\n",
				line);
		else {		/* Create production */
			Production *rule =
			    (Production *)
			    malloc(sizeof(Production));
			rule->func = 0;
			if (*ptr == ':') {
				++ptr;
				skipws();
				p = ptr;
				skipwd();
				if (p != ptr) {
					c = *ptr;
					*ptr = 0;
					rule->func =
					    strcpy((char *)
						   malloc(strlen(p) + 1),
						   p);
					*ptr = c;
				}
			}
			rule->next = e->right;
			rule->left = e;
			rule->len = nwords;
			rule->str = words;
			rule->line = line;
			rule->prodno = nrules;
			rule->label = label;
			label = 0;
			e->right = rule;

			/* Store production in production array */
			if (srules == nrules)
				rules =
				    (Production **) realloc(rules, sizeof (struct production *) * (srules += 8));
			rules[nrules++] = rule;
		}
	}
	if (label)
		free(label);
}

/* Process a file */
void readin(FILE *f)
{
	char buf[1024];

	confsiz = 128;
	conf = (Conf *) malloc(sizeof(Conf) * confsiz);

	/* Allocate production table */
	srules = 16;
	rules =
	    (Production **) malloc(sizeof(Production *) *
					  srules);

	rules[0] = (Production *) malloc(sizeof(Production));
	rules[0]->func = 0;
	rules[0]->next = 0;
	rules[0]->left = find("THEEND");
	rules[0]->left->right = rules[0];
	rules[0]->len = 1;
	rules[0]->str = (Symbol **) malloc(sizeof(Symbol *));
	rules[0]->line = line;
	rules[0]->prodno = nrules++;
	rules[0]->label = 0;

	/* Process input file */
	while ((ptr = fgets(buf, 1023, f)) != 0)
		++line, process();

	if (nrules == 1) {
		fprintf(stderr, "No productions\nNo output generated\n");
		exit(1);
	} else
		rules[0]->str[0] = rules[1]->left;
}

/*************/
/* item sets */
/*************/

/* Sets of items operations */

typedef struct item Item;
typedef struct items Items;

struct item {
	Production *rule;
	int ofst;
};

struct items {
	Item *items;	/* Set of items */
	int nitems, siz;
	Items **to;	/* Item sets which produced this item set */
	int nto, tosiz;
	Symbol **visited;	/* visited flags */
	int nvisited;
	int vissiz;
};

/* Generate an empty set of items */

Items *newitems()
{
	Items *items =
	    (Items *) malloc(sizeof(Items));;
	items->nitems = 0;
	items->siz = 64;
	items->items =
	    (Item *) malloc(sizeof(Item) * items->siz);
	items->nto = 0;
	items->tosiz = 64;
	items->to =
	    (Items **) malloc(sizeof(Items *) *
				     items->tosiz);
	items->nvisited = 0;
	items->vissiz = 64;
	items->visited =
	    (Symbol **) malloc(sizeof(Symbol *) *
				      items->vissiz);
	return items;
}

/* Add an item to an item-set */

void add(Items *items, Production *rule, int ofst)
{
	if (items->nitems == items->siz)
		items->items =
		    (Item *) realloc(items->items,
					    sizeof(Item) *
					    (items->siz += 64));
	items->items[items->nitems].rule = rule;
	items->items[items->nitems].ofst = ofst;
	++items->nitems;
}

/* Add a reference to an item-set */

void addref(Items *items, Items *ref)
{
	if (items->nto == items->tosiz)
		items->to =
		    (Items **) realloc(items->to,
					      sizeof(Items *) *
					      (items->tosiz += 64));
	items->to[items->nto++] = ref;
}

/* Check if we visited an item */

int chkvisited(Items *i, Symbol *s)
{
	int x;
	for (x = 0; x != i->nvisited; ++x)
		if (i->visited[x] == s)
			return 1;
	return 0;
}

void addvisited(Items *i, Symbol *s)
{
	if (i->nvisited == i->vissiz)
		i->visited =
		    (Symbol **) realloc(i->visited,
					       sizeof(Symbol *) *
					       (i->vissiz += 64));
	i->visited[i->nvisited++] = s;
}

/***********************/
/* Item-set operations */
/***********************/

/* Close an item set */

void closure(Items *items)
{
	int x;
	Production *r;
	/* For each item in the set */
	for (x = 0; x != items->nitems; ++x)
		/* If we found an item with a nonterminal at the end */
		if (items->items[x].ofst != items->items[x].rule->len &&
		    items->items[x].rule->str[items->items[x].ofst]->right)
			/* Then for each production with that nonterminal on the left */
			for (r =
			     items->items[x].rule->str[items->items[x].
						       ofst]->right; r;
			     r = r->next) {
				int y;
				/* If that production in the form ". rightside" is not already in set */
				for (y = 0; y != items->nitems; ++y)
					if (items->items[y].ofst == 0
					    && items->items[y].rule == r)
						break;
				/* then we add ". rightside" to the item-set */
				if (y == items->nitems)
					add(items, r, 0);
			}
}

/* Generate goto set */

Items *used = 0;

Items *go_to(Items *i, Symbol *e)
{
	Items *h;
	int x;
	if (!used)
		used = newitems();
	h = used;
	h->nitems = 0;
	h->nto = 0;
	h->nvisited = 0;
	for (x = 0; x != i->nitems; ++x)
		if (i->items[x].ofst != i->items[x].rule->len &&
		    i->items[x].rule->str[i->items[x].ofst] == e)
			add(h, i->items[x].rule, i->items[x].ofst + 1);
	closure(h);
	if (h->nitems)
		return h;
	else
		return 0;
}

/***************************/
/* Collection of item sets */
/***************************/

typedef struct set Set;

struct set {
	Items **set;
	int nsets;
	int siz;
};

/* Generate an empty set of item sets */

Set *newset()
{
	Set *set = (Set *) malloc(sizeof(Set));
	set->nsets = 0;
	set->siz = 64;
	set->set =
	    (Items **) malloc(sizeof(Items *) * set->siz);
	return set;
}

/* Add an item set to a collection */

void addset(Set *set, Items *items)
{
	if (set->nsets == set->siz)
		set->set =
		    (Items **) realloc(set->set,
					      sizeof(Items *) *
					      (set->siz += 64));
	set->set[set->nsets++] = items;
}

/* Return true if two item sets match */

int match(Items *a, Items *b)
{
	int x;
	if (a->nitems != b->nitems)
		return 0;
	for (x = 0; x != a->nitems; ++x) {
		int y;
		for (y = 0; y != b->nitems; ++y)
			if (a->items[x].rule == b->items[y].rule
			    && a->items[x].ofst == b->items[y].ofst)
				break;
		if (y == b->nitems)
			return 0;
	}
	return 1;
}

/* Find an item set in a collection */

int findset(Set *set, Items *i)
{
	int x;
	for (x = 0; x != set->nsets; ++x)
		if (match(set->set[x], i))
			return x;
	return -1;
}

/* Generate lr0 states (collection of item sets) */

Set *lr0()
{
	Items *items = newitems();
	Set *set = newset();
	int x, y;
	Symbol *e;
	add(items, rules[0], 0);
	closure(items);
	addset(set, items);
	for (x = 0; x != set->nsets; ++x)
		for (y = 0; y != HTSIZE; ++y)
			for (e = htab[y]; e; e = e->next) {
				Items *i = go_to(set->set[x], e);
				if (i) {
					int idx = findset(set, i);
					if (idx == -1)
						addset(set, i), addref(i,
								       set->
								       set
								       [x]),
						    used = 0;
					else
						addref(set->set[idx],
						       set->set[x]);
				}
			}
	return set;
}

/* Check if terminal can follow non-terminal in the context of i */
/* This would be an SLR parser if follows was not limited to context */

int check(Set *set, int x, Symbol *nonterm, Symbol *term)
{
	int y;
	Items *i = set->set[x];
	for (y = 0; y != i->nitems; ++y) {	/* Look for ". nonterm term" */
		Item *j = &i->items[y];
		if (j->ofst == j->rule->len - 1 &&
		    j->rule->str[j->ofst] == nonterm && !term) {
			return 1;
		} else if (j->ofst < j->rule->len - 1 &&
			   j->rule->str[j->ofst] == nonterm) {
			if (j->rule->str[j->ofst + 1] == term) {
				/* X is 'term' */
				return 1;
			} else if (j->rule->str[j->ofst + 1]->right) {	/* X is no 'term', but is a nonterminal.  Check if first
									 * symbol of the nonterminal can be 'term' */
				Items *ii = go_to(i, nonterm);
				if (ii) {
					int z = findset(set, ii);
					if (z != -1) {
						int q;
						for (q = 0;
						     q !=
						     set->set[z]->nitems;
						     ++q)
							if ((set->set[z]->items[q].ofst != set->set[z]->items[q].rule->len &&
							    set->set[z]->items[q].rule->str[set->set[z]->items[q].ofst] == term) ||
							    (set->set[z]->items[q].ofst == set->set[z]->items[q].rule->len && !term)) {
								return 1;
							}
					} else
						printf("Huh?\n");
				}
			}
		}
	}
	return 0;
}

int lookahead(Set *set, int setidx, int itmidx, Symbol *term, int len);

int check1(Set *set, int x, Symbol *nonterm, Symbol *term)
{
	int y;
	Items *i = set->set[x];
	for (y = 0; y != i->nitems; ++y) {
		Item *j = &i->items[y];
		if (j->ofst == j->rule->len - 1
		    && j->rule->str[j->ofst] == nonterm) {
			if (lookahead(set, x, y, term, j->ofst))
				return 1;
		}
	}
	return 0;
}

int look(Set *set, int setidx, Symbol *nonterm, Symbol *term, 
         int len, int (*check) (Set *set, int x, Symbol *nonterm, Symbol *term))
{
	int x;
	if (len)
		for (x = 0; x != set->set[setidx]->nto; ++x) {
			int idx;
			for (idx = 0;
			     set->set[idx] != set->set[setidx]->to[x];
			     ++idx);
			if (look(set, idx, nonterm, term, len - 1, check))
				return 1;
	} else if (check(set, setidx, nonterm, term))
		return 1;
	return 0;
}

/* Check if 'term' is in the FOLLOWS set of the reduction
 * specified by the item at 'itmidx' in the item set for state 'setidx'
 */

int lookahead(Set *set, int setidx, int itmidx, Symbol *term, int len)
{
	Items *i = set->set[setidx];
	Symbol *nonterm = i->items[itmidx].rule->left;

	if (chkvisited(i, nonterm))
		return 0;
	addvisited(i, nonterm);

	if (look(set, setidx, nonterm, term, len, check))
		return 1;
	if (look(set, setidx, nonterm, term, len, check1))
		return 1;

	return 0;
}

void clrvisited(Set *set)
{
	int x;
	for (x = 0; x != set->nsets; ++x)
		set->set[x]->nvisited = 0;
}

void prprod(FILE *f, Production *p)
{
	int y;
	fprintf(f, "%s ==>", p->left->name);
	for (y = 0; y != p->len; ++y)
		fprintf(f, " %s", p->str[y]->name);
}

/* Create action/goto table */

Conf confl;

int conmatch(Conf *z)
{
	int x;
	if (confl.e != z->e)
		return 0;
	if (confl.nprods != z->nprods)
		return 0;
	for (x = 0; x != confl.nprods; ++x) {
		int y;
		for (y = 0; y != z->nprods && confl.prod[x] != z->prod[y];
		     ++y);
		if (y == z->nprods)
			return 0;
	}
	return 1;
}

Conf *resolve()
{
	int z;
	for (z = 0; z != nconfs && !conmatch(z + conf); ++z);
	if (z != nconfs) {
		conf[z].shift = confl.shift;
		return conf + z;
	} else
		return 0;
}

int confnum = 0;
int commaflg;

void addconf(int act, int stat)
{
	if (act == -1)
		confl.shift = stat;
	if (confl.nprods == confl.siz)
		confl.prod =
		    (int *) realloc(confl.prod,
				    sizeof(int) * (confl.siz += 64));
	confl.prod[confl.nprods++] = act;
}

void genaction(FILE *f, Set *set, int x, Symbol *e)
{
	Conf *conf = 0;
	Items *ii;
	char buf[10];
	int res;
	int z;

	if (!confl.siz)
		confl.prod =
		    (int *) malloc(sizeof(int) * (confl.siz = 64));
	confl.nprods = 0;
	confl.e = e;

	/* Check for shift */
	if (e && (ii = go_to(set->set[x], e))) {
		z = findset(set, ii);
		if (z != -1)
			addconf(-1, z);
		else
			printf("Huh?\n");
	}

	/* Check for reduce */
	for (z = 0; z != set->set[x]->nitems; ++z)
		if (set->set[x]->items[z].ofst ==
		    set->set[x]->items[z].rule->len)
			if (clrvisited(set),
			    lookahead(set, x, z, e,
				      set->set[x]->items[z].rule->len))
				addconf(set->set[x]->items[z].rule->prodno,
					0);

	/* Check accept */
	if (!e)
		for (z = 0; z != set->set[x]->nitems; ++z)
			if (set->set[x]->items[z].ofst ==
			    set->set[x]->items[z].rule->len)
				if (set->set[x]->items[z].rule == rules[0])
					addconf(0, 0);

	/* Check for conflicts */
	if (confl.nprods > 1) {
		conf = resolve();
		if (conf && !conf->func) {
			if (conf->resolve == -1)
				res = 0;	/* Shift */
			else {	/* Reduce */
				for (z = 0; z != confl.nprods; ++z)
					if (confl.prod[z] == conf->resolve)
						break;
				if (z != confl.nprods)
					res = z;
				else
					fprintf(stderr,
						"invalid resolution for conflict:\n"),
					    res = -2;
			}
		} else
			res = -2;
		if (res < 0 && (!conf || !conf->func)) {
			printf("Conflict:");
			for (z = 0; z != confl.nprods; ++z)
				if (confl.prod[z] == -1)
					printf(" shift");
				else if (confl.prod[z] == 0)
					printf(" accept");
				else
					printf(" reduce("),
					    prprod(stdout,
						   rules[confl.prod[z]]),
					    printf(")");
			if (e)
				printf(" : %s\n", e->name);
			else
				printf(" : %sEND\n", prefix);
		}
	} else
		res = 0;

	if (res < 0)
		res = 0;

	/* Output result */
	if (confl.nprods) {
		if (confl.nprods > 1 && conf && conf->func)
			sprintf(buf, " C+%d", confnum++);
		else if (confl.prod[res] == -1)
			sprintf(buf, "S+%d", confl.shift);
		else if (confl.prod[res] == 0)
			sprintf(buf, "ACC");
		else
			sprintf(buf, "R+%d", confl.prod[res] - 1);
	}
	if (commaflg++)
		fprintf(f, ",");
	if (confl.nprods)
		fprintf(f, " %5s", buf);
	else
		fprintf(f, "    er");
}

void lalr(FILE *f, Set *set)
{
	int x;
	Symbol *e;
	for (x = 0; x != set->nsets; ++x) {
		int y;
		commaflg = 0;
		for (y = 0; y != HTSIZE; ++y)
			for (e = htab[y]; e; e = e->next)
				if (!e->right)
					genaction(f, set, x, e);
		genaction(f, set, x, (Symbol *) 0);
		for (y = 0; y != HTSIZE; ++y)
			for (e = htab[y]; e; e = e->next)
				if (e->right && e != rules[0]->left) {
					Items *ii =
					    go_to(set->set[x], e);
					int n;
					if (ii
					    && (n =
						findset(set, ii)) != -1)
						fprintf(f, ", %5d", n);
					else
						fprintf(f, ",    er");
				}
		if (x + 1 != set->nsets)
			fprintf(f, ",\n");
		else
			fprintf(f, "\n");
	}
}

/* Create production table */

void prodtable(FILE *f)
{
	int x;
	int y;
	int col = 0;
	int nterms;
	Symbol *e;
	fprintf(f, " ");

	for (x = y = nterms = 0; x != HTSIZE; ++x)
		for (e = htab[x]; e; e = e->next)
			if (!e->right)
				++nterms;
			else if (e->right != rules[0])
				e->ntermn = y++;
	++nterms;

	for (x = 1; x != nrules; ++x) {
		char buf[64];
		sprintf(buf, " { %s, %d, %d }",
			rules[x]->func ? rules[x]->func : "0",
			rules[x]->left->ntermn + nterms, rules[x]->len);
		if (col + strlen(buf) >= 78)
			fprintf(f, ",\n %s", buf), col = strlen(buf) + 1;
		else {
			if (!col)
				fprintf(f, "%s", buf);
			else
				fprintf(f, ",%s", buf), ++col;
			col += strlen(buf);
		}
	}
	if (col)
		fprintf(f, "\n");
	else
		fprintf(f, " { 0, 0, 0 }\n");
}

void prdefs(FILE *f)
{
	int x;
	Symbol *e;
	int y = 0;
	for (x = 0; x != HTSIZE; ++x)
		for (e = htab[x]; e; e = e->next)
			if (!e->right)
				fprintf(f, "#define %s %d\n", e->name,
					y++);
	fprintf(f, "#define %sEND %d\n", prefix, y++);
	for (x = 0; x != HTSIZE; ++x)
		for (e = htab[x]; e; e = e->next)
			if (e->right && e->right != rules[0])
				fprintf(f, "#define %s %d\n", e->name,
					y++);
	fprintf(f,
		"\n#define %sNSYMS %d	/* Total no. of symbols */\n",
		prefix, y);
}

void prfuncs(FILE *f)
{
	int x;
	for (x = 0; x != nrules; ++x)
		if (rules[x]->func) {
			fprintf(f, "extern %sVAL %s();	/* ", prefix,
				rules[x]->func);
			prprod(f, rules[x]);
			fprintf(f, " */\n");
		}
	for (x = 0; x != nconfs; ++x)
		if (conf[x].func) {
			int y;
			int flg = 0;
			fprintf(f, "extern int %s();	/* (",
				conf[x].func);
			for (y = 0; y != conf[x].nprods; ++y) {
				if (flg)
					fprintf(f, " / ");
				if (conf[x].prod[y] == -1)
					fprintf(f, "shift");
				else
					prprod(f, rules[conf[x].prod[y]]);
				flg = 1;
			}
			fprintf(f, " :%s) */\n", conf[x].e->name);
		}
}

void prconf(FILE *f)
{
	int x;
	int col = 0;
	int idx = 0;
	fprintf(f, " ");
	for (x = 0; x != nconfs; ++x)
		if (conf[x].func) {
			char buf[64];
			sprintf(buf, " { %s, %d }", conf[x].func, idx);
			idx += conf[x].nprods;
			if (col + strlen(buf) >= 78)
				fprintf(f, ",\n  %s", buf), col =
				    strlen(buf) + 2;
			else {
				if (!col)
					fprintf(f, "%s", buf);
				else
					fprintf(f, ",%s", buf);
				col += strlen(buf);
			}
		}
	if (col)
		fprintf(f, "\n");
	else
		fprintf(f, " { 0, 0 }\n");	/* Blank conflict table */
}

void prconf1(FILE *f)
{
	int x;
	int col = 0;
	for (x = 0; x != nconfs; ++x)
		if (conf[x].func) {
			int y;
			for (y = 0; y != conf[x].nprods; ++y) {
				char buf0[10], buf[10];
				if (conf[x].prod[y] == -1)
					sprintf(buf0, "S+%d",
						conf[x].shift);
				else
					sprintf(buf0, "R+%d",
						conf[x].prod[y] - 1);
				sprintf(buf, " %5s", buf0);
				if (col + strlen(buf) >= 78)
					fprintf(f, ",\n %s", buf), col =
					    strlen(buf) + 1;
				else {
					if (!col)
						fprintf(f, "%s", buf);
					else
						fprintf(f, ",%s", buf);
					col += strlen(buf);
				}
			}
		}
	if (col)
		fprintf(f, "\n");
	else
		fprintf(f, " 0\n");	/* In case table is blank */
}

void pritems(Items *items)
{
	int x;
	for (x = 0; x != items->nitems; ++x) {
		int y;
		printf("%s ==> ", items->items[x].rule->left->name);
		for (y = 0; y != items->items[x].rule->len; ++y) {
			if (y == items->items[x].ofst)
				printf(". ");
			printf("%s ", items->items[x].rule->str[y]->name);
		}
		if (y == items->items[x].ofst)
			printf(".");
		printf("\n");
	}
}

void prset(Set *set)
{
	int x;
	for (x = 0; x != set->nsets; ++x) {
		int y;
		printf("%d: ", x);
		for (y = 0; y != set->set[x]->nto; ++y) {
			int z;
			for (z = 0; z != set->nsets; ++z)
				if (set->set[z] == set->set[x]->to[y])
					printf("%d ", z);
			printf(",");
		}
		printf("\n");
		pritems(set->set[x]);
		printf("\n");
	}
}

int main(int argc, char *argv[])
{
	Set *set;
	FILE *f;
	char buf[64];
	int x;
	char *name = 0;
	for (x = 1; x != argc; ++x)
		if (!strcmp(argv[x], "-prefix") && x + 1 != argc)
			prefix = argv[++x];
		else if (!strcmp(argv[x], "-type") && x + 1 != argc)
			type = argv[++x];
		else if (!strcmp(argv[x], "-h") || !strcmp(argv[x], "-") ||
			 !strcmp(argv[x], "-help")) {
			fprintf(stderr,
				"aacc [-prefix prefix] [-type \'type\'] [-help] grammar-file\n");
			return 0;
		} else if (!name)
			name = argv[x];
		else {
			fprintf(stderr,
				"aacc: unknown option or too many arguments\n");
			return 1;
		}
	if (!name) {
		fprintf(stderr, "aacc: grammar-file name required\n");
		return 1;
	}

	f = fopen(name, "r");
	if (!f) {
		fprintf(stderr, "aacc: Couldn\'t open file %s\n", name);
		exit(1);
	}
	readin(f);
	fclose(f);

	set = lr0();
  /* prset(set); */
	set->set[0]->nto = 0;

	sprintf(buf, "%stab.h", prefix);
	printf("writing %s\n", buf);
	f = fopen(buf, "w");
	if (!f) {
		fprintf(stderr, "aacc: Couldn't open file %s\n", buf);
		exit(1);
	}

	fprintf(f, "/* Parser header file */\n\n");
	fprintf(f, "#ifndef _I%stab\n", prefix);
	fprintf(f, "#define _I%stab 1\n", prefix);
	fprintf(f, "\n");
	fprintf(f, "/* Value associated with each token */\n");
	fprintf(f, "typedef %s %sVAL;\n", type, prefix);
	fprintf(f, "\n");
	fprintf(f, "#ifndef _Iaacc\n");
	fprintf(f, "#define _Iaacc 1\n");
	fprintf(f, "\n");
	fprintf(f, "/* Action table flags */\n");
	fprintf(f, "#define S   0x0000	/* Shift */\n");
	fprintf(f, "#define R   0x1000	/* Reduce */\n");
	fprintf(f, "#define ACC 0x2000	/* Accept */\n");
	fprintf(f, "#define er  0x3000	/* Error */\n");
	fprintf(f, "#define C   0x4000	/* Conflict */\n");
	fprintf(f, "\n");
	fprintf(f, "/* Conflict table entry */\n");
	fprintf(f, "struct conf\n");
	fprintf(f, " {\n");
	fprintf(f,
		" int (*func)(%sVAL value);	/* Conflict resolution function */\n", prefix);
	fprintf(f, " int idx;	/* Conflict resolution table index */\n");
	fprintf(f, " };\n");
	fprintf(f, "\n");
	fprintf(f, "#endif\n");
	fprintf(f, "\n");
	fprintf(f, "/* Production table entry */\n");
	fprintf(f, "struct %sprod\n", prefix);
	fprintf(f, " {\n");
	fprintf(f,
		" %sVAL (*func)();	/* Function to execute when reducing this production */\n",
		prefix);
	fprintf(f,
		" short token;		/* Symbol number for left side of production */\n");
	fprintf(f,
		" short len;		/* Length of right side of production */\n");
	fprintf(f, " };\n");
	fprintf(f, "\n");
	fprintf(f, "/* Stack entry */\n");
	fprintf(f, "struct %sstkentry\n", prefix);
	fprintf(f, " {\n");
	fprintf(f, " int state;\n");
	fprintf(f, " %sVAL value;\n", prefix);
	fprintf(f, " };\n");
	fprintf(f, "\n");
	fprintf(f, "/* A stack */\n");
	fprintf(f, "struct %sstack\n", prefix);
	fprintf(f, " {\n");
	fprintf(f, " struct %sstkentry *stack;\n", prefix);
	fprintf(f, " int sp;\n");
	fprintf(f, " int siz;\n");
	fprintf(f, " };\n");
	fprintf(f, "\n");
	fprintf(f,
		"/* Main parser function.  Pass it a token and a value associated\n");
	fprintf(f,
		" * with the token and it returns 1 if the token was accepted, 0\n");
	fprintf(f,
		" * if parsing is complete, or -1 if there was an error.\n");
	fprintf(f, " */\n");
	fprintf(f, "extern int %sparse(struct %sstack *stack, int token, %sVAL value);\n", prefix, prefix, prefix);
	fprintf(f, "\n");
	fprintf(f,
		"extern struct %sstack *%smkstk();	/* Create a stack */\n",
		prefix, prefix);
	fprintf(f,
		"extern void %srmstk(struct %sstack *stack);	/* Eliminate a stack */\n",
		prefix, prefix);
	fprintf(f, "\n");
	fprintf(f, "extern short %saction[];	/* Action/Goto table */\n",
		prefix);
	fprintf(f,
		"extern struct %sprod %sprod[];	/* Production table */\n",
		prefix, prefix);
	fprintf(f,
		"extern struct conf %sconf[];	/* Conflict table */\n",
		prefix);
	fprintf(f,
		"extern short %sres[];		/* Conflict resolution table */\n",
		prefix);
	fprintf(f, "\n");
	fprintf(f, "/* Symbols */\n");

	prdefs(f);

	fprintf(f, "\n");
	fprintf(f, "/* User provided Functions */\n");

	prfuncs(f);

	fprintf(f, "\n");
	fprintf(f, "#endif\n");
	fclose(f);

	sprintf(buf, "%stab.c", prefix);
	printf("writing %s\n", buf);
	f = fopen(buf, "w");
	if (!f) {
		fprintf(stderr, "Couldn't open %s\n", buf);
		exit(1);
	}

	fprintf(f, "/* Parser - this is unlicensed freeware */\n");
	fprintf(f, "\n");
	fprintf(f, "#include <stdlib.h>\n");
	fprintf(f, "#include \"%stab.h\"\n", prefix);
	fprintf(f, "\n");
	fprintf(f, "/* Action/Goto table */\n");
	fprintf(f, "short %saction[]=\n {\n", prefix);

	lalr(f, set);

	fprintf(f, " };\n");
	fprintf(f, "\n");
	fprintf(f, "/* Production table */\n");
	fprintf(f, "struct %sprod %sprod[]=\n", prefix, prefix);
	fprintf(f, " {\n");

	prodtable(f);

	fprintf(f, " };\n");
	fprintf(f, "\n");
	fprintf(f, "/* Conflict table */\n");
	fprintf(f, "struct conf %sconf[]=\n", prefix);
	fprintf(f, " {\n");

	prconf(f);

	fprintf(f, " };\n");
	fprintf(f, "\n");
	fprintf(f, "/* Conflict resolution table */\n");
	fprintf(f, "short %sres[]=\n", prefix);
	fprintf(f, " {\n");

	prconf1(f);

	fprintf(f, " };\n");

	fprintf(f, "\n");
	fprintf(f, "/* Create a stack */\n");
	fprintf(f, "\n");
	fprintf(f, "struct %sstack *%smkstk()\n", prefix, prefix);
	fprintf(f, " {\n");
	fprintf(f,
		" struct %sstack *stack=(struct %sstack *)malloc(sizeof(struct %sstack));\n",
		prefix, prefix, prefix);
	fprintf(f, " stack->siz=128;\n");
	fprintf(f, " stack->sp=0;\n");
	fprintf(f, " stack->stack=(struct %sstkentry *)malloc(\n", prefix);
	fprintf(f, "  sizeof(struct %sstkentry)*stack->siz);\n", prefix);
	fprintf(f, " stack->stack[0].state=0;\n");
	fprintf(f, " return stack;\n");
	fprintf(f, " }\n");
	fprintf(f, "\n");
	fprintf(f, "/* Eliminate a stack */\n");
	fprintf(f, "\n");
	fprintf(f, "void %srmstk(struct %sstack *stack)\n", prefix, prefix);
	fprintf(f, " {\n");
	fprintf(f, " free(stack->stack);\n");
	fprintf(f, " free(stack);\n");
	fprintf(f, " }\n");
	fprintf(f, "\n");
	fprintf(f,
		"/* Main parser function.  Pass it a token and a value associated\n");
	fprintf(f,
		" * with the token and it returns 1 if the token was accepted, 0\n");
	fprintf(f,
		" * if parsing is complete or -1 if there's an error.\n");
	fprintf(f, " */\n");
	fprintf(f, "\n");
	fprintf(f, "int %sparse(struct %sstack *stack,int token,%sVAL value)\n", prefix, prefix, prefix);
	fprintf(f, " {\n");
	fprintf(f, " while(1)\n");
	fprintf(f, "  {\n");
	fprintf(f, "  /* Get table entry */\n");
	fprintf(f,
		"  int ent=%saction[stack->stack[stack->sp].state*%sNSYMS+token];\n",
		prefix, prefix);
	fprintf(f, "  %sVAL v;\n", prefix);
	fprintf(f, "  loop: switch(ent&0xF000)\n");
	fprintf(f, "   {\n");
	fprintf(f, "  case S: /* Shift */\n");
	fprintf(f, "          if(++stack->sp==stack->siz)\n");
	fprintf(f,
		"           stack->stack=(struct %sstkentry *)realloc(stack->stack,\n",
		prefix);
	fprintf(f,
		"            sizeof(struct %sstkentry)*(stack->siz+=128));\n",
		prefix);
	fprintf(f,
		"          stack->stack[stack->sp].state=(ent&0x0FFF);\n");
	fprintf(f, "          stack->stack[stack->sp].value=value;\n");
	fprintf(f, "          return 1;\n");
	fprintf(f, "\n");
	fprintf(f, "  case R: /* Reduce */\n");
	fprintf(f, "          if(%sprod[ent&0x0FFF].func)\n", prefix);
	fprintf(f, "           switch(%sprod[ent&0x0FFF].len)\n", prefix);
	fprintf(f, "            {\n");
	fprintf(f,
		"            case 1: v=%sprod[ent&0x0FFF].func(stack->stack[stack->sp-0].value); break;\n",
		prefix);
	fprintf(f,
		"            case 2: v=%sprod[ent&0x0FFF].func(stack->stack[stack->sp-1].value,stack->stack[stack->sp-0].value); break;\n",
		prefix);
	fprintf(f,
		"            case 3: v=%sprod[ent&0x0FFF].func(stack->stack[stack->sp-2].value,stack->stack[stack->sp-1].value,stack->stack[stack->sp-0].value); break;\n",
		prefix);
	fprintf(f,
		"            case 4: v=%sprod[ent&0x0FFF].func(stack->stack[stack->sp-3].value,stack->stack[stack->sp-2].value,stack->stack[stack->sp-1].value,stack->stack[stack->sp-0].value); break;\n",
		prefix);
	fprintf(f,
		"            case 5: v=%sprod[ent&0x0FFF].func(stack->stack[stack->sp-4].value,stack->stack[stack->sp-3].value,stack->stack[stack->sp-2].value,stack->stack[stack->sp-1].value,stack->stack[stack->sp-0].value); break;\n",
		prefix);
	fprintf(f,
		"            case 6: v=%sprod[ent&0x0FFF].func(stack->stack[stack->sp-5].value,stack->stack[stack->sp-4].value,stack->stack[stack->sp-3].value,stack->stack[stack->sp-2].value,stack->stack[stack->sp-1].value,stack->stack[stack->sp-0].value); break;\n",
		prefix);
	fprintf(f, "            }\n");
	fprintf(f, "          stack->sp-=%sprod[ent&0x0FFF].len-1;\n",
		prefix);
	fprintf(f, "          stack->stack[stack->sp].state=\n");
	fprintf(f,
		"           %saction[stack->stack[stack->sp-1].state*%sNSYMS+\n",
		prefix, prefix);
	fprintf(f, "                    %sprod[ent&0x0FFF].token];\n",
		prefix);
	fprintf(f, "          stack->stack[stack->sp].value=v;\n");
	fprintf(f, "          break;\n");
	fprintf(f, "\n");
	fprintf(f, "  case ACC: /* Done */\n");
	fprintf(f, "          return 0;\n");
	fprintf(f, "\n");
	fprintf(f, "  case C: /* Conflict */\n");
	fprintf(f,
		"          ent=%sres[%sconf[ent&0x0FFF].func(value)+%sconf[ent&0x0FFF].idx];\n",
		prefix, prefix, prefix);
	fprintf(f, "          goto loop;\n");
	fprintf(f, "\n");
	fprintf(f, "  case er: /* Error */\n");
	fprintf(f, "          return -1;\n");
	fprintf(f, "   }\n");
	fprintf(f, "  }\n");
	fprintf(f, " }\n");

	fclose(f);
	return 0;
}
