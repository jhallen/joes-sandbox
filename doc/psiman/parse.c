/* Parse next input object */

#include <stdio.h>
#include "parse.h"

OBJ *parse0()
{
	OBJ *obj;
	int flg;
	int col;		/* Current column number */
	int x;
	char buf[1024];

	if (!gets(buf))
		return 0;
	obj = mkobj();

	x = 0;
	col = 0;

      loop:
	while (buf[x] == ' ' || buf[x] == '\t') {
		if (buf[x] == '\t')
			col += 8 - (col & 7);
		else
			++col;
		++x;
	}
	obj->start[obj->n] = col;
	flg = 0;
	while (buf[x] && buf[x] != '\t'
	       && !(buf[x] == ' ' && buf[x + 1] == ' '
		    && buf[x + 2] == ' ')) {
		append(obj->glob + obj->n, obj->len + obj->n,
		       obj->siz + obj->n, buf[x]);
		++col;
		flg = 1;
		++x;
	}
	if (flg)
		++obj->n;
	if (buf[x])
		goto loop;

	if (obj->n)
		obj->ind = obj->start[0];
	else
		obj->ind = 0;

	return obj;
}

/* Figure out if b should be merged into a */

int ymerge(OBJ * a, OBJ * b)
{
	if (!a->n || !b->n)
		return 0;
	if (b->n > 1)
		return 0;
	if (a->n == 1 && a->len[0] < 30)
		return 0;
	if (a->n == 1 && b->n == 1 && b->ind > a->ind)
		return 0;
	if (b->n == 1 && b->start[0] == 0)
		b->start[0] = a->start[a->n - 1];
	return 1;
}

static OBJ *newobj = 0;

OBJ *parse()
{
	OBJ *obj;
	if (newobj)
		obj = newobj, newobj = 0;
	else
		obj = parse0();
	if (!obj)
		return 0;

	while ((newobj = parse0()) && ymerge(obj, newobj)) {
		obj = merge(obj, newobj);
		newobj = 0;
	}
	return obj;
}
