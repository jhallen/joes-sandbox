/* Parse regular expression into NFA */

#include <stdlib.h>
#include "re.h"

/* Create an empty state */

struct state *mk_state()
{
	struct state *st = (struct state *)malloc(sizeof(struct state));
	st->f = 0;
	st->e = 0;
	st->ch = -1;
	st->flg = 0;
	return st;
}

/* Convert graph into list for freeing */

struct state *list = 0;

void to_list(struct state *st)
{
	if (st && st->ch!=-3) {
		st->ch = -3;
		to_list(st->f);
		to_list(st->e);
		st->f = list;
		list = st;
	}
}

/* Free an NFA... it may have loops */

struct state *rm_state(struct state *st)
{
	to_list (st);
	while (list) {
		st = list->f;
		free(list);
		list = st;
	}
	return 0;
}

/* Print an NFA */

void ind(int n)
{
	int x;
	for (x=0; x!=n; ++x)
		printf(" ");
}

void do_show(struct state *st,int n)
{
	if (!st)
		return;
	if (st->flg) {
		ind(n), printf("%x (link)\n",st);
		return ;
	}
	st->flg = 1;
	if (st->ch==-1) {
		ind(n), printf("%x done\n",st);
	} else if(st->ch==-2) {
		ind(n), printf("%x alt\n",st);
		do_show(st->f,n+2);
		do_show(st->e,n+2);
	} else {
		ind(n), printf("%x match '%c'\n",st,st->ch);
		do_show(st->f,n+2);
		do_show(st->e,n+2);
	}
}

void show(struct state *st)
{
	do_show(st, 0);
}

/* Parse regex into NFA */

unsigned char *ptr;	/* Input pointer */
struct state *last;	/* Address of done state */

/* Parse all items with precedence higher than prec */

struct state *do_parse(int prec)
{
	struct state *first, *next;
	/* Get first item */
	if (!*ptr || *ptr==')' || *ptr=='*' || *ptr=='|')
		return 0;
	else if (*ptr=='(') {
		++ptr;
		first = do_parse(0);
		next = last;
		if (!first)
			return 0;
		if (*ptr==')') {
			++ptr;
		} else
			return rm_state(first);
	} else {
		first = mk_state();
		first->ch = *ptr++;
		next = first->f = mk_state();
	}
	for(;;) {
		/* Try to extend it */
		if (*ptr=='*') {
			struct state *st;
			do ++ptr; while(*ptr=='*');
			st = mk_state();
			st->f = first;
			st->ch = -2;
			st->e = mk_state();
			next->f = st->e;
			next->ch = -2;
			next->e = first;
			first = st;
			next = st->e;
		} else if (*ptr=='|') {
			struct state *st;
			struct state *nf, *nl;
			if (prec) {
				last = next;
				return first;
			}
			++ptr;
			st = do_parse(0);
			if (!st)
				return rm_state(st);
			nf = mk_state();
			nf->ch = -2;
			nf->f = first;
			nf->e = st;
			nl = mk_state();
			next->ch = -2;
			next->f = nl;
			last->ch = -2;
			last->f = nl;
			first = nf;
			next = nl;
		} else if (!*ptr || *ptr==')') {
			last = next;
			return first;
		} else {
			struct state *st;
			st = do_parse(1);
			if (!st)
				return rm_state(first);
			next->ch = st->ch;
			next->f = st->f;
			next->e = st->e;
			next = last;
			st->f = 0;
			st->e = 0;
			rm_state(st);
		}
	}
}

/* Top level parser */

struct state *parse(unsigned char *s)
{
	struct state *st;
	ptr = s;
	st = do_parse(0);
	if (*ptr)
		return rm_state(st);
	else {
		return st;
	}
}
