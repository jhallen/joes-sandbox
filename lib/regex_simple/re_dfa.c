/* Convert NFA into DFA */

#include <stdlib.h>
#include <stdio.h>
#include "re.h"

/* Closure */

void show_list(struct list *l)
{
	while (l) {
		printf("	%p\n",l->st);
		l = l->next;
	}
}

/* Add st to l */

struct list *add(struct list *l, struct state *st)
{
	struct list *n = (struct list *)malloc(sizeof(struct list));
	n->st = st;
	n->next = l;
	return n;
}

/* Return true if st is in l */

int is_in(struct list *l, struct state *st)
{
	while (l)
		if (l->st == st)
			return 1;
		else
			l = l->next;
	return 0;
}

/* Add st to l if it's not already in it.  If we added st,
 * follow its pointers. */

struct list *scan(struct list *l,struct state *st)
{
	if (!is_in(l, st)) {
		l = add(l, st);
		if (st->ch==-2) {
			if (st->f) l = scan(l, st->f);
			if (st->e) l = scan(l, st->e);
		}
	}
	return l;
}

/* Compute closure of a list: generate new list of states with no duplicates */

struct list *closure(struct list *l)
{
	struct list *new_list = 0;
	while (l)
	{
		new_list = scan(new_list,l->st);
		l=l->next;
	}
	return new_list;
}

/* Add ->f of all matching states to a new list */

struct list *move(struct list *l, int c)
{
	struct list *new_list = 0;
	while (l)
	{
		if (l->st->ch == c)
			new_list = add(new_list,l->st->f);
		l=l->next;
	}
	return new_list;
}

/* Convert NFA to DFA */

struct dfa eof;
int next_no;

struct dfa *new_dfa()
{
	struct dfa *dfa = calloc(sizeof(struct dfa), 1);
	dfa->no = next_no++;
	return dfa;
}

int list_size(struct list *l)
{
	int n = 0;
	while (l) {
		++n;
		l = l->next;
	}
	return n;
}

void unlist(struct list **array, struct list *l)
{
	int n = 0;
	while (l) {
		array[n++] = l;
		l = l->next;
	}
}

int cmp(const struct list **a, const struct list **b)
{
	if ((*a)->st > (*b)->st)
		return 1;
	else if ((*a)->st < (*b)->st)
		return -1;
	else
		return 0;
}

struct list *relist(struct list **array, int n)
{
	struct list *first = array[0];
	struct list *l = first;
	int x;
	for (x = 1; x != n; ++x) {
		l->next = array[x];
		l = l->next;
	}
	l->next = 0;
	return first;
}

struct list *sort_list(struct list *l)
{
	if (l) {
		int n = list_size(l);
		struct list **array = (struct list **)malloc(sizeof(struct list *)*n);
		unlist(array, l);
		qsort(array, n, sizeof(struct list *), (int (*)(const void *,const void *))cmp);
		l = relist(array, n);
	}
	return l;
}

struct dfa *find(struct dfa *d, struct list *l)
{
	while (d) {
		struct list *a = l;
		struct list *b = d->nfa;
		while (a && a->st->ch == -2) a = a->next;
		while (b && b->st->ch == -2) b = b->next;
		while(a && b && a->st == b->st) {
			a = a->next;
			b = b->next;
			while (a && a->st->ch == -2) a = a->next;
			while (b && b->st->ch == -2) b = b->next;
		}
		if (!a && !b)
			return d;


		d = d->next;
	}
	return 0;
}

struct dfa *all_dfa_states;
int next_no;

struct dfa *do_nfa_to_dfa(struct list *nfa)
{
	struct list *list;
	struct dfa *dfa;
//	printf("nfa_to_dfa...\n");
	/* This list of NFA states becomes our DFA state */
	list = sort_list(closure(nfa));
//	show_list(list);
	/* Does this DFA state already exist? */
	dfa = find(all_dfa_states, list);
//	if (dfa)
//		printf("Found existing %x\n",dfa);
	if (!dfa) {
		int x;
		/* Create it */
		dfa = new_dfa();
//		printf("Create new %x\n",dfa);
		dfa->next = all_dfa_states;
		all_dfa_states = dfa;
		dfa->nfa = list;
		for (x = -1; x != 256; ++x) {
			struct list *l = move(list, x);
			if (l) {
				if (x == -1)
					dfa->eof = &eof;
				else
					dfa->nxt[x] = do_nfa_to_dfa(l);
			}
		}
	}
	return dfa;
}

struct dfa *nfa_to_dfa(struct state *st)
{
	return do_nfa_to_dfa(add(NULL, st));
}

void show_dfa(struct dfa *dfa)
{
	int x;
	if (dfa->flg)
		return;
	printf("state %d:\n",dfa->no);
	for (x=0; x!=256; ++x)
		if (dfa->nxt[x])
			printf("  '%c'(%d): next-state=%d\n",x,x,dfa->nxt[x]->no);
	if (dfa->eof)
		printf("  end-of-string: match!\n");
	dfa->flg = 1;
	printf("\n");
	for (x=0; x!=256; ++x)
		if (dfa->nxt[x])
			show_dfa(dfa->nxt[x]);
}
