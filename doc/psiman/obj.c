#include <stdlib.h>
#include "obj.h"

/* Make an empty object */

OBJ *mkobj()
{
	OBJ *o = (OBJ *) malloc(sizeof(OBJ));
	int x;
	for (x = 0; x != 128; ++x)
		o->len[x] = o->siz[x] = 0, o->glob[x] = 0;
	o->n = 0;
	o->ind = 0;
	return o;
}

/* Free an obj */

void rmobj(OBJ * obj)
{
	int x;
	if (obj) {
		for (x = 0; x != obj->n; ++x)
			if (obj->glob[x])
				free(obj->glob[x]);
		free(obj);
	}
}

/* Count no. of words in text */

int nwords(int *text)
{
	int n = 0;
	int x;
	for (x = 0; text[x]; ++x)
		if ((text[x] & 255) == ' ' && x
		    && (text[x - 1] & 255) != ' ')
			++n;
	if (x && (text[x - 1] & 255) != ' ')
		++n;
	return n;
}

/* Set attribute on all of text */

void setattr(int *text, int attr)
{
	int x;
	for (x = 0; text[x]; ++x)
		text[x] |= attr;
}

/* Resize an array */
int *resize(int *ary, int len, int *siz)
{
	if (len == *siz)
		ary =
		    (int *) realloc(ary,
				    (*siz = len + 1024) * sizeof(int));
	return ary;
}

int isletter(int x)
{
	x &= 255;
	if ((x >= 'a' && x <= 'z') ||
	    (x >= 'A' && x <= 'Z') || (x >= '0' && x <= '9') || x == '_')
		return 1;
	else
		return 0;
}

int len(int *ary)
{
	int x;
	if (!ary)
		return 0;
	for (x = 0; ary[x]; ++x);
	return x;
}

void append(int **ary, int *len, int *siz, int c)
{
	if (*len + 1 >= *siz) {
		*siz = *len + 80;
		if (*ary)
			*ary = realloc(*ary, sizeof(int) ** siz);
		else
			*ary = malloc(sizeof(int) ** siz);
	}
	(*ary)[*len] = c;
	*len = *len + 1;
	(*ary)[*len] = 0;
}

OBJ *merge(OBJ * a, OBJ * b)
{
	OBJ *obj = mkobj();
	int an = 0, bn = 0;
	obj->ind = b->ind;
	while (an != a->n || bn != b->n) {
		if (an != a->n && bn != b->n)
			if (a->start[an] == b->start[bn]) {
				int x, z;
				obj->len[obj->n] =
				    a->len[an] + b->len[bn] + 1;
				obj->siz[obj->n] = obj->len[obj->n] + 8;
				obj->glob[obj->n] =
				    (int *) malloc(sizeof(int) *
						   obj->siz[obj->n]);
				obj->start[obj->n] = a->start[an];
				for (x = 0; x != a->len[an]; ++x)
					obj->glob[obj->n][x] =
					    a->glob[an][x];
				obj->glob[obj->n][x++] = ' ';
				for (z = 0; z != b->len[bn]; ++z)
					obj->glob[obj->n][x + z] =
					    b->glob[bn][z];
				obj->glob[obj->n][x + z] = 0;
				++an;
				++bn;
			} else if (a->start[an] < b->start[bn]) {	/* Take a */
				obj->glob[obj->n] = a->glob[an];
				a->glob[an] = 0;
				obj->len[obj->n] = a->len[an];
				obj->siz[obj->n] = a->siz[an];
				obj->start[obj->n] = a->start[an];
				++an;
			} else {	/* Take b */
				obj->glob[obj->n] = b->glob[bn];
				b->glob[bn] = 0;
				obj->len[obj->n] = b->len[bn];
				obj->siz[obj->n] = b->siz[bn];
				obj->start[obj->n] = b->start[bn];
				++bn;
		} else if (an != a->n) {	/* Take a */
			obj->glob[obj->n] = a->glob[an];
			a->glob[an] = 0;
			obj->len[obj->n] = a->len[an];
			obj->siz[obj->n] = a->siz[an];
			obj->start[obj->n] = a->start[an];
			++an;
		} else {	/* Take b */
			obj->glob[obj->n] = b->glob[bn];
			b->glob[bn] = 0;
			obj->len[obj->n] = b->len[bn];
			obj->siz[obj->n] = b->siz[bn];
			obj->start[obj->n] = b->start[bn];
			++bn;
		}
		++obj->n;
	}
	rmobj(a);
	rmobj(b);
	return obj;
}
