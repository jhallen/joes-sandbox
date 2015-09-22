/* Symbol table functions */

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "types.h"
#include "box.h"
#include "builtin.h"

#define STSIZE 1024

SYM *htab[STSIZE];

void clrbinds()
{
	int x;
	SYM *s;
	for (x = 0; x != STSIZE; x++)
		for (s = htab[x]; s; s = s->r)
			if (s->bind && s != yE && s != yPI)
				discard(s->bind), s->bind = 0;
}

SYM *lookup(C *s)
{
	C *ss = s;
	SYM *n;
	U hsh;
	for (hsh = 0; *s; ++s)
		hsh = hsh * 34857 + *s;
	hsh &= STSIZE - 1;
	for (n = htab[hsh]; n; n = n->r)
		if (!strcmp(ss, n->s))
			break;
	return n;
}

SYM *add(C *s)
{
	SYM *n = newsym();
	U hsh;
	n->s = strcpy((C *) malloc(strlen(s) + 1), s);
	for (hsh = 0; *s; ++s)
		hsh = hsh * 34857 + *s;
	hsh &= STSIZE - 1;
	n->r = htab[hsh];
	htab[hsh] = n;
	return n;
}
