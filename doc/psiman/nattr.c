/* Character attribute parsing */

#include <stdio.h>
#include <stdlib.h>
#include "attr.h"

int okbeg(int *text, int x)
{
	int left;		/* 0=space, 1=letter, 2=non-letter */
	int right;

	if (!x)
		left = 0;
	else if ((text[x - 1] & 255) == ' ')
		left = 0;
	else if (isletter(text[x - 1]))
		left = 1;
	else
		left = 2;

	if (!text[x + 1])
		right = 0;
	else if ((text[x + 1] & 255) == ' ' || (text[x + 1] & 255) == '.'
		 || (text[x + 1] & 255) == ','
		 || (text[x + 1] & 255) == ':'
		 || (text[x + 1] & 255) == ';'
		 || (text[x + 1] & 255) == '?')
		right = 0;
	else if (isletter(text[x + 1]))
		right = 1;
	else
		right = 2;

	if (left != right)
		return 1;
	else
		return 0;
}

int thereisokend(int *text)
{
	int x;
	for (x = 1; text[x]; ++x)
		if ((255 & text[x]) == '_' && okbeg(text, x))
			return 1;
	return 0;
}

/* Italicize text */
int *italicize(int *text, int *siz, int *len)
{
	int attr = 0;
	int *ary;
	int newlen = 0;
	int x;
	ary = (int *) malloc(sizeof(int) ** siz);
	for (x = 0; text[x]; ++x)
		if ((255 & text[x]) == '_' && okbeg(text, x)
		    && (attr || thereisokend(text + x)))
			attr ^= ITAL;
		else
			ary[newlen++] = (text[x] | attr);
	ary[newlen] = 0;
	if (newlen >= *siz) {
		fprintf(stderr, "??? %d %d %d %d\n", newlen, x, *len,
			*siz);
		exit(1);
	}
	*len = newlen;
	return ary;
}

int nontext(int *text)
{
	if (nwords(text) == 1)
		return 1;
	if (len(text) < 8)
		return 1;
	return 0;
}

int punc(int *text)
{
	int l = len(text);
	int c;
	if (!l)
		return 0;
	c = (text[l - 1] & 255);
	if (c == '.' || c == ',' || c == ':' || c == ';' || c == '!'
	    || c == '?')
		return 1;
	return 0;
}

OBJ *attr(OBJ * obj)
{
	int x;
	if (obj->n == 1 && nwords(obj->glob[0]) < 6 && !punc(obj->glob[0]))
		setattr(obj->glob[0], BOLD);
	if (obj->n > 1 && nontext(obj->glob[0]))
		setattr(obj->glob[0], BOLD);
	for (x = 0; x != obj->n; ++x) {
		int *prev;
		obj->glob[x] = italicize(prev =
					 obj->glob[x], obj->siz + x,
					 obj->len + x);
		free(prev);
	}
	return obj;
}
