#include <stdlib.h>
#include "types.h"
#include "box.h"
#include "builtin.h"
#include "scan.h"

C *ptr = 0;

TOKEN *tokenstack = 0;

void setscan(C *s)
{
	TOKEN *t;
	ptr = s;
	while (t = tokenstack) {
		tokenstack = t->next;
		free(t);
	}
}

void rmtok(TOKEN *t)
{
	if (t)
		free(t);
}

void ungettok(TOKEN *t)
{
	if (!t)
		return;
	if (t->type == tEOF) {
		free(t);
		return;
	}
	t->next = tokenstack;
	tokenstack = t;
}

TOKEN *gettok()
{
	TOKEN *t;
	if (t = tokenstack) {
		tokenstack = t->next;
		return t;
	}
	t = (TOKEN *) malloc(sizeof(TOKEN));
	while (*ptr == ' ' || *ptr == '\t')
		++ptr;
	if (!*ptr) {
		t->type = tEOF;
		return t;
	}
	if (ptr[0] == '^') {
		++ptr;
		t->type = tINFIX;
		t->sym = yEXP;
		t->ass = t->sym->ass;
		t->prec = t->sym->prec;
		return t;
	}
	if (ptr[0] == '*' && ptr[1] == '*') {
		++ptr;
		++ptr;
		t->type = tINFIX;
		t->sym = yEXP;
		t->ass = t->sym->ass;
		t->prec = t->sym->prec;
		return t;
	}
	if (ptr[0] == '*') {
		++ptr;
		t->type = tINFIX;
		t->sym = yMUL;
		t->ass = t->sym->ass;
		t->prec = t->sym->prec;
		return t;
	}
	if (ptr[0] == '/') {
		++ptr;
		t->type = tINFIX;
		t->sym = yDIV;
		t->ass = t->sym->ass;
		t->prec = t->sym->prec;
		return t;
	}
	if (ptr[0] == '+') {
		++ptr;
		t->type = tINFIX;
		t->sym = yADD;
		t->ass = t->sym->ass;
		t->prec = t->sym->prec;
		return t;
	}
	if (ptr[0] == '=') {
		++ptr;
		t->type = tINFIX;
		t->sym = yEQ;
		t->ass = t->sym->ass;
		t->prec = t->sym->prec;
		return t;
	}
	if (ptr[0] == '-') {
		++ptr;
		t->type = tINFIX;
		t->sym = ySUB;
		t->ass = t->sym->ass;
		t->prec = t->sym->prec;
		return t;
	}
	if (ptr[0] == '(') {
		++ptr;
		t->type = tLPAREN;
		return t;
	}
	if (ptr[0] == ')') {
		++ptr;
		t->type = tRPAREN;
		return t;
	}
	if (ptr[0] == '!') {
		++ptr;
		t->type = tPOSTFIX;
		t->sym = yFACT;
		t->prec = t->sym->prec;
		return t;
	}
	if (ptr[0] == ',') {
		++ptr;
		t->type = tCOMMA;
		return t;
	}
	if (ptr[0] == '\'') {
		++ptr;
		t->type = tPOSTFIX;
		t->sym = yDERV;
		t->ass = t->sym->ass;
		t->prec = t->sym->prec;
		return t;
	}
	if (ptr[0] == 'D') {
		++ptr;
		t->type = tPREFIX;
		t->sym = yD;
		t->ass = t->sym->ass;
		t->prec = t->sym->prec;
		return t;
	}
	if (*ptr >= '0' && *ptr <= '9' || *ptr == '.') {
		t->type = tNUM;
		t->num = newnum();
		sscanf(ptr, "%lf", &t->num->n);
		while (*ptr >= '0' && *ptr <= '9' || *ptr == '.' || *ptr == 'e'
		       || *ptr == 'E')
			++ptr;
		return t;
	}
	if (*ptr >= 'a' && *ptr <= 'z' || *ptr >= 'A' && *ptr <= 'Z'
	    || *ptr == '_' || *ptr == '%') {
		C *s = ptr, o;
		while (*ptr >= 'a' && *ptr <= 'z' || *ptr >= 'A' && *ptr <= 'Z'
		       || *ptr == '_' || *ptr == '%' || *ptr >= '0'
		       && *ptr <= '9')
			++ptr;
		o = *ptr;
		*ptr = 0;
		t->type = tSYM;
		if (!(t->sym = lookup(s)))
			t->sym = add(s);
		*ptr = o;
		return t;
	}
	t->type = tUNKNOWN;
	return t;
}
