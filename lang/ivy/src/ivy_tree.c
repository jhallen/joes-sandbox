/* IVY abstract syntax trees

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
#include "ivy_tree.h"
#include "ivy_free_list.h"
#include "ivy.h"

/* Trees are short-lived in Ivy: they exist between the parser and the
   code generator.  A single tree is never larger than a single top-level
   block. */

/* Construct a two operand node: note that NULL is returned if any
   of the arguments are NULL. */

Ivy_node *ivy_cons2(Ivy_loc *loc, int what, Ivy_node *left, Ivy_node *right)
{
	if (!left || !right) {
		return 0;
	} else if (what == ivy_nSEMI && left->what == ivy_nSEMI) {
		/* Build a list */
		Ivy_node *n;
		for (n = left; n->r && n->r->what == ivy_nSEMI; n = n->r);
		n->r = ivy_cons2(loc, ivy_nSEMI, n->r, right);
		return left;
	} else {
		Ivy_node *n = (Ivy_node *)ivy_alloc_item(loc->free_list);
		n->what = what;
		n->l = left;
		n->r = right;
		n->s = 0;
		*n->loc = *loc;
		return n;
	}
}

/* Get first node of list */

Ivy_node *ivy_first(Ivy_node *list)
{
	if (list && list->what == ivy_nSEMI)
		return list->l;
	else
		return list;
}

/* Get rest of list */

Ivy_node *ivy_rest(Ivy_node *list)
{
	if (list && list->what == ivy_nSEMI)
		return list->r;
	else
		return 0;
}

/* Construct a single operand node: note that NULL is returned if any
   of the arguments are NULL. */

Ivy_node *ivy_cons1(Ivy_loc *loc,int what, Ivy_node *right)
{
	if (!right)
		return 0;
	else {
		Ivy_node *n = (Ivy_node *)ivy_alloc_item(loc->free_list);
		n->what = what;
		n->l = 0;
		n->r = right;
		n->s = 0;
		*n->loc = *loc;
		return n;
	}
}

/* Construct an integer ivy_constant */

Ivy_node *ivy_consnum(Ivy_loc *loc,long long v)
{
	Ivy_node *n = (Ivy_node *)ivy_alloc_item(loc->free_list);
	n->what = ivy_nNUM;
	n->l = 0;
	n->r = 0;
	n->n = v;
	n->s = 0;
	*n->loc = *loc;
	return n;
}

/* Construct a floating point ivy_constant */

Ivy_node *ivy_consfp(Ivy_loc *loc,double v)
{
	Ivy_node *n = (Ivy_node *)ivy_alloc_item(loc->free_list);
	n->what = ivy_nFP;
	n->l = 0;
	n->fp = v;
	n->r = 0;
	n->n = 0;
	n->s = 0;
	*n->loc = *loc;
	return n;
}

/* Construct a string family node */

Ivy_node *ivy_conss(Ivy_loc *loc,int a, char *v, int l)
{
	Ivy_node *n = (Ivy_node *)ivy_alloc_item(loc->free_list);
	n->what = a;
	n->l = 0;
	n->r = 0;
	n->s = v;
	n->n = l;
	*n->loc = *loc;
	return n;
}

/* Construct a string constant */

Ivy_node *ivy_consstr(Ivy_loc *loc, char *v, int len)
{
	return ivy_conss(loc, ivy_nSTR, v, len);
}

/* Construct an identifier */

Ivy_node *ivy_consnam(Ivy_loc *loc, char *v)
{
	return ivy_conss(loc, ivy_nNAM, v, strlen(v));
}

/* Construct a label */

Ivy_node *ivy_conslabel(Ivy_loc *loc, char *v)
{
	return ivy_conss(loc, ivy_nLABEL, v, strlen(v));
}

/* Construct a void */

Ivy_node *ivy_consvoid(Ivy_loc *loc)
{
	Ivy_node *n = (Ivy_node *)ivy_alloc_item(loc->free_list);
	n->what = ivy_nVOID;
	n->l = 0;
	n->r = 0;
	n->s = 0;
	n->n = 0;
	*n->loc = *loc;
	return n;
}

Ivy_node *ivy_consthis(Ivy_loc *loc)
{
	Ivy_node *n = (Ivy_node *)ivy_alloc_item(loc->free_list);
	n->what = ivy_nTHIS;
	n->l = 0;
	n->r = 0;
	n->s = 0;
	n->n = 0;
	*n->loc = *loc;
	return n;
}

/* Construct empty */

Ivy_node *ivy_consempty(Ivy_loc *loc)
{
	Ivy_node *n = (Ivy_node *)ivy_alloc_item(loc->free_list);
	n->what = ivy_nEMPTY;
	n->l = 0;
	n->r = 0;
	n->s = 0;
	n->n = 0;
	*n->loc = *loc;
	return n;
}

/* Construct an optional node */

Ivy_node *ivy_opt(Ivy_loc *loc, Ivy_node *n)
{
	if (n)
		return n;
	else
		return ivy_consempty(loc);
}

/* Duplicate a tree */

Ivy_node *ivy_dup_tree(Ivy_loc *loc, Ivy_node * o)
{
	Ivy_node *n;
	if (!o)
		return 0;
	n = (Ivy_node *)ivy_alloc_item(loc->free_list);
	n->what = o->what;
	if (o->s && o->what == ivy_nNAM) {
		n->s = o->s;
	} else if (o->s) {
		int x;
		n->s = (char *) malloc(o->n + 1);
		for (x = 0; x != o->n; ++x)
			n->s[x] = o->s[x];
		n->s[x] = 0;
	} else
		n->s = 0;
	n->n = o->n;
	n->r = ivy_dup_tree(loc, o->r);
	n->l = ivy_dup_tree(loc, o->l);
	*n->loc = *o->loc;
	return n;
}

/* Tree printer */

void ivy_indent(FILE *out, int x)
{
	while (x--)
		fputc(' ', out);
}

void ivy_prtree(FILE *out, Ivy_node *n, int lvl)
{
	switch (n->what) {
		case ivy_nNUM: {
			ivy_indent(out, lvl), fprintf(out, "%lld", n->n);
			break;
		} case ivy_nFP: {
			ivy_indent(out, lvl), fprintf(out, "%g", n->fp);
			break;
		} case ivy_nSTR: {
			ivy_indent(out, lvl), fprintf(out, "\"%s\"", n->s);
			break;
		} case ivy_nVOID: {
			ivy_indent(out, lvl), fprintf(out, "void");
			break;
		} default: {
			if (n->s)
				ivy_indent(out, lvl), fprintf(out, "%s", n->s);
			else {
				ivy_indent(out, lvl), fprintf(out, "(%s\n", ivy_what_tab[n->what].name);
				if (n->l)
					ivy_prtree(out, n->l, lvl + 2);
				if (n->r)
					ivy_prtree(out, n->r, lvl + 2);
				ivy_indent(out, lvl), fprintf(out, ")");
			}
		}
	}
	fprintf(out, "\n");
}
