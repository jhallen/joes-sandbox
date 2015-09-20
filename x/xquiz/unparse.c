#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "types.h"
#include "queue.h"
#include "xjunk.h"
#include "display.h"
#include "main.h"
#include "box.h"
#include "builtin.h"
#include "unparse.h"

int mousex, mousey;
int basex, basey;

/* Write character at x/y position */
void wrt(DSPOBJ *obj, I x, I y, char c)
{
	XDrawString(dsp, obj->win, obj->gc, x, y - sdescent, &c, 1);
}

int wrtmouse(I x, I y, C c)
{
	if (mousex < x)
		return 0;
	if (mousex > x + XTextWidth(sfs, &c, 1))
		return 0;
	if (mousey < y - sheight)
		return 0;
	if (mousey > y)
		return 0;
	return 1;
}

void wrts(DSPOBJ *obj, I x, I y, C *s)
{
	XDrawString(dsp, obj->win, obj->gc, x, y - sdescent, s, strlen(s));
}

int wrtsmouse(I x, I y, C *s)
{
	if (mousex < x)
		return 0;
	if (mousex > x + XTextWidth(sfs, s, strlen(s)))
		return 0;
	if (mousey < y - sheight)
		return 0;
	if (mousey > y)
		return 0;
	return 1;
}

int wwidth(C *s)
{
	int width = XTextWidth(sfs, s, strlen(s));
	return width;
}

char *NTOS(NUM *n)
{
	char buf[128];
	sprintf(buf, "%G", n->n);
	return strdup(buf);
}

int unwidth(LST *n, int prec)
{
	SYM *sy = n;
	I w = 0;
	C *s;
	if (!n)
		return wwidth("?");
	switch (typ(n)) {
	case tSYM:
		if (sy->bind && sy != yE && sy != yPI && sy != yI) {
			s = NTOS(sy->bind);
			w = wwidth(s);
			free(s);
			return w;
		} else
			return wwidth(((SYM *) n)->s);
	case tNUM:
		s = NTOS(n);
		w = wwidth(s);
		free(s);
		return w;
	case tLST:
		switch (sy = n->d, sy->type) {
		case tSYM:
			if (sy == ySQRT) {
				w = 21;
				w += unwidth(n->r->d, 0);
			} else {
				w += wwidth(sy->s);
				w += wwidth("()");
				while (n = n->r) {
					w += unwidth(n->d, 0);
					if (n->r)
						w += wwidth(",");
				}
			}
			return w;
		case tINFIX:
			if (sy == yDIV) {
				I a = unwidth(n->r->d, 0);
				I b = unwidth(n->r->r->d, 0);
				if (n->r->d)
					if (typ(n->r->d) == tLST)
						if (n->r->d->d == yDIV) {
							w += wwidth("()");
							goto ovr;
						}
				if (n->r->r->d)
					if (typ(n->r->r->d) == tLST)
						if (n->r->r->d->d == yDIV)
							w += wwidth("()");
 ovr:
				if (a > b)
					w += a;
				else
					w += b;
			} else if (sy == yEXP) {
				w += unwidth(n->r->d, sy->prec);
				w += unwidth(n->r->r->d, 0);
			} else {
				n = n->r;
				w += unwidth(n->d, sy->prec);
				n = n->r;
				if (sy == ySUB || sy == yDIV)
					do {
						w += unwidth(sy, sy->prec + 1);
						w += unwidth(n->d,
							     sy->prec + 1);
						w += wwidth(" ");
					} while (n = n->r);
				else
					do {
						w += unwidth(sy, sy->prec);
						w += unwidth(n->d, sy->prec);
						w += wwidth(" ");
					} while (n = n->r);
			}
			if (sy->prec < prec || sy == yEXP && sy->prec <= prec)
				w += wwidth("()");
			return w;
		case tPREFIX:
		case tPOSTFIX:
			if (sy == ySQRT)
				w += 21;
			if (sy->prec < prec)
				w += wwidth("()");
			w += unwidth(sy, sy->prec);
			w += unwidth(n->r->d, sy->prec);
			return w + wwidth(" ") / 2;
		}
	}
}

int unheight(LST *n, int prec)
{
	SYM *sy;
	I w = 0;
	if (!n)
		return sheight;
	switch (typ(n)) {
	case tSYM:
		return sheight;
	case tNUM:
		return sheight;
	case tLST:
		switch (sy = n->d, sy->type) {
		case tSYM:
			if (sy == ySQRT)
				w = sheight + unheight(n->r->d, 0);
			else
				while (n = n->r) {
					I z = unheight(n->d, 0);
					if (z > w)
						w = z;
				}
			return w;
		case tINFIX:
			if (sy == yDIV) {
				w += unheight(n->r->d, 0);
				w += unheight(n->r->r->d, 0);
				w += sheight;
			} else if (sy == yEXP) {
				I a = unheight(n->r->d, sy->prec);
				I b = unheight(n->r->r->d, 0) + sheight;
				if (a / 2 + (a & 1) > b)
					return a;
				else
					return b + a / 2;
			} else {
				I a, b;
				n = n->r;
				a = unheight(n->d, sy->prec);
				n = n->r;
				do {
					b = unheight(n->d, sy->prec);
					if (b > a)
						a = b;
				}
				while (n = n->r);
				w += a;
			}
			return w;
		case tPREFIX:
		case tPOSTFIX:
			return unheight(n->r->d, sy->prec);
		}
	}
}

int unbase(LST *n, int prec)
{
	SYM *sy;
	I w = 0, z;
	if (!n)
		return 0;
	switch (typ(n)) {
	case tSYM:
		return 0;
	case tNUM:
		return 0;
	case tLST:
		switch (sy = n->d, sy->type) {
		case tSYM:
			while (n = n->r) {
				I z = unbase(n->d, 0);
				if (z > w)
					w = z;
			}
			return w;
		case tINFIX:
			if (sy == yDIV)
				return unheight(n->r->r->d, 0);
			else if (sy == yEXP)
				return unbase(n->r->d, sy->prec);
			else {
				I a, b;
				n = n->r;
				a = unbase(n->d, sy->prec);
				n = n->r;
				do {
					b = unbase(n->d, sy->prec);
					if (b > a)
						a = b;
				}
				while (n = n->r);
				return a;
			}
		case tPREFIX:
		case tPOSTFIX:
			return unbase(n->r->d, sy->prec);
		}
	}
}

void unparse(DSPOBJ *obj, int x, int y, LST **l, LST **sel, int prec)
{
	LST *n = *l;
	SYM *sy = n;
	C *s;
	if (l == sel)
		XSetForeground(dsp, obj->gc, magenta.pixel);
	if (!n) {
		wrt(obj, x, y, '?');
		goto done;
	}
	switch (typ(n)) {
	case tSYM:
		if (n == yNEG)
			wrts(obj, x, y, "-");
		else if (sy->bind && sy != yE && sy != yPI && sy != yI) {
			s = NTOS(sy->bind);
			wrts(obj, x, y, s);
			free(s);
		} else
			wrts(obj, x, y, ((SYM *) n)->s);
		goto done;
	case tNUM:
		s = NTOS(n);
		wrts(obj, x, y, s);
		free(s);
		goto done;
	case tLST:
		switch (sy = n->d, sy->type) {
		case tSYM:
			if (sy == ySQRT) {
				int h = unheight(n->r->d, 0);
				int b = unbase(n->r->d, 0);
				int w = unwidth(n->r->d, 0);
				XDrawLine(dsp, obj->win, obj->gc, x,
					  y + b - h / 2, x + 7, y + b - h / 2);
				XDrawLine(dsp, obj->win, obj->gc, x + 7,
					  y + b - h / 2, x + sheight, y + b);
				XDrawLine(dsp, obj->win, obj->gc, x + sheight,
					  y + b, x + 21,
					  y - (h - b) - sdescent - sascent / 2);
				XDrawLine(dsp, obj->win, obj->gc, x + 21,
					  y - (h - b) - sdescent - sascent / 2,
					  x + 21 + w,
					  y - (h - b) - sdescent - sascent / 2);
				unparse(obj, x + 21, y, &n->r->d, sel, 0);
			} else {
				wrts(obj, x, y, sy->s), x += wwidth(sy->s);
				wrt(obj, x, y, '(');
				x += wwidth("(");
				while (n = n->r) {
					unparse(obj, x, y, &n->d, sel, 0);
					x += unwidth(n->d, 0);
					if (n->r)
						wrt(obj, x, y, ','), x +=
						    wwidth(",");
				}
				wrt(obj, x, y, ')');
				x += wwidth(")");
			}
			goto done;
		case tINFIX:
			if (sy->prec < prec || sy == yEXP && sy->prec <= prec)
				wrt(obj, x, y, '('), x += wwidth("(");
			if (sy == yDIV) {
				I w = 0;
				I a = unwidth(n->r->d, 0);
				I b = unwidth(n->r->r->d, 0);
				if (n->r->d)
					if (typ(n->r->d) == tLST)
						if (n->r->d->d == yDIV) {
							w += wwidth("()");
							goto ovr;
						}
				if (n->r->r->d)
					if (typ(n->r->r->d) == tLST)
						if (n->r->r->d->d == yDIV)
							w += wwidth("()");
 ovr:
				if (a > b)
					w += a;
				else
					w += b;
/*               for(a=0;a<w;a+=wwidth("-")) wrt(obj,x+a,y,'-'); */
				XDrawLine(dsp, obj->win, obj->gc, x,
					  y - sdescent - sascent / 2, x + w,
					  y - sdescent - sascent / 2);
				b = unwidth(n->r->d, 0);
				unparse(obj, x + w / 2 - b / 2,
					y - unbase(n->r->d, 0) - sheight,
					&n->r->d, sel, 0);
				a = unheight(n->r->r->d, 0);
				b = unwidth(n->r->r->d, 0);
				unparse(obj, x + w / 2 - b / 2,
					y + a - unbase(n->r->r->d, 0),
					&n->r->r->d, sel, 0);
				x += w;
			} else if (sy == yEXP) {
				I a;
				unparse(obj, x, y, &n->r->d, sel, sy->prec);
				x += unwidth(n->r->d, sy->prec);
				unparse(obj, x,
					y - unbase(n->r->r->d, 0) - sheight,
					&n->r->r->d, sel, 0);
				x += unwidth(n->r->r->d, 0);
			} else {
				n = n->r;
				unparse(obj, x, y, &n->d, sel, sy->prec);
				x += unwidth(n->d, sy->prec);
				n = n->r;
				if (sy == ySUB || sy == yDIV)
					do {
						x += wwidth(" ") / 2;
						unparse(obj, x, y, &sy, 0,
							sy->prec + 1);
						x += unwidth(sy,
							     sy->prec + 1) +
						    wwidth(" ") / 2;
						unparse(obj, x, y, &n->d, sel,
							sy->prec + 1);
						x += unwidth(n->d,
							     sy->prec + 1);
					}
					while (n = n->r);
				else
					do {
						x += wwidth(" ") / 2;
						unparse(obj, x, y, &sy, 0,
							sy->prec);
						x += unwidth(sy,
							     sy->prec) +
						    wwidth(" ") / 2;
						unparse(obj, x, y, &n->d, sel,
							sy->prec);
						x += unwidth(n->d, sy->prec);
					}
					while (n = n->r);
			}
			if (sy->prec < prec || sy == yEXP && sy->prec <= prec)
				wrt(obj, x, y, ')');
			goto done;
		case tPREFIX:
			if (sy->prec < prec)
				wrt(obj, x, y, '('), x += wwidth("(");
			unparse(obj, x, y, &sy, 0, sy->prec);
			x += unwidth(sy, sy->prec) + wwidth(" ") / 2;
			unparse(obj, x, y, &n->r->d, sel, sy->prec);
			x += unwidth(n->r->d, sy->prec);
			if (sy->prec < prec)
				wrt(obj, x, y, ')');
			goto done;
		case tPOSTFIX:
			if (sy->prec < prec)
				wrt(obj, x, y, '('), x += wwidth("(");
			unparse(obj, x, y, &n->r->d, sel, sy->prec);
			x += unwidth(n->r->d, sy->prec) + wwidth(" ") / 2;
			unparse(obj, x, y, &sy, 0, sy->prec);
			x += unwidth(sy, sy->prec);
			if (sy->prec < prec)
				wrt(obj, x, y, ')');
			goto done;
		}
	}
 done:
	if (sel == l)
		XSetForeground(dsp, obj->gc, yellow.pixel);
}

/* Find part of tree which mouse is pointing to */
/* Returns address of pointer to object */

LST **findmouse(int x, int y, LST **lst, int prec)
{
	int orgx = x, orgy = y;
	LST **t;
	LST *n = *lst;
	SYM *sy = n;
	C *s;
	basex = x;
	basey = y;
	if (!n) {
		if (wrtmouse(x, y, '?'))
			return lst;
		else
			return 0;
	}
	switch (typ(n)) {
	case tSYM:
		if (n == yNEG)
			if (wrtsmouse(x, y, "-"))
				return lst;
			else
				return 0;
		else if (sy->bind && sy != yE && sy != yPI && sy != yI) {
			s = NTOS(sy->bind);
			if (wrtsmouse(x, y, s)) {
				free(s);
				return lst;
			}
			free(s);
			return 0;
		} else if (wrtsmouse(x, y, ((SYM *) n)->s))
			return lst;
		else
			return 0;
	case tNUM:
		s = NTOS(n);
		if (wrtsmouse(x, y, s)) {
			free(s);
			return lst;
		}
		free(s);
		return 0;
	case tLST:
		switch (sy = n->d, sy->type) {
		case tSYM:
			if (sy == ySQRT) {
				int h = unheight(n->r->d, 0);
				int b = unbase(n->r->d, 0);
				int w = unwidth(n->r->d, 0);
				if (mousex >= x && mousex < x + 21 &&
				    mousey < y
				    && mousey >=
				    y - (h - b) - sdescent - sascent / 2)
					return lst;
				return findmouse(x + 21, y, &n->r->d, 0);
			} else {
				if (wrtsmouse(x, y, sy->s))
					return lst;
				x += wwidth(sy->s);
				x += wwidth("(");
				while (n = n->r) {
					if (t = findmouse(x, y, &n->d, 0))
						return t;
					x += unwidth(n->d, 0);
					if (n->r)
						x += wwidth(",");
				}
			}
			return 0;
		case tINFIX:
			if (sy->prec < prec || sy == yEXP && sy->prec <= prec)
				x += wwidth("(");
			if (sy == yDIV) {
				I w = 0;
				I a = unwidth(n->r->d, 0);
				I b = unwidth(n->r->r->d, 0);
				if (n->r->d)
					if (typ(n->r->d) == tLST)
						if (n->r->d->d == yDIV) {
							w += wwidth("()");
							goto ovr;
						}
				if (n->r->r->d)
					if (typ(n->r->r->d) == tLST)
						if (n->r->r->d->d == yDIV)
							w += wwidth("()");
 ovr:
				if (a > b)
					w += a;
				else
					w += b;
				if (mousex >= x && mousex < x + w && mousey < y
				    && mousey >= y - sheight)
					return lst;
				b = unwidth(n->r->d, 0);
				if (t =
				    findmouse(x + w / 2 - b / 2,
					      y - unbase(n->r->d, 0) - sheight,
					      &n->r->d, 0))
					return t;
				a = unheight(n->r->r->d, 0);
				b = unwidth(n->r->r->d, 0);
				return findmouse(x + w / 2 - b / 2,
						 y + a - unbase(n->r->r->d, 0),
						 &n->r->r->d, 0);
			} else if (sy == yEXP) {
				I a = unwidth(n->r->d, sy->prec);
				if (t = findmouse(x, y, &n->r->d, sy->prec))
					return t;
				if (mousex >= x && mousex < x + a &&
				    mousey < y - sheight
				    && mousey >=
				    y - sheight - unheight(n->r->r->d, 0)) {
					basex = orgx;
					basey = orgy;
					return lst;
				}
				x += a;
				if (t =
				    findmouse(x,
					      y - unbase(n->r->r->d,
							 0) - sheight,
					      &n->r->r->d, 0))
					return t;
				if (mousex >= x
				    && mousex < x + unwidth(n->r->r->d, 0)
				    && mousey < y && mousey >= y - sheight) {
					basex = orgx;
					basey = orgy;
					return lst;
				}
				return 0;
			} else {
				n = n->r;
				if (t = findmouse(x, y, &n->d, sy->prec))
					return t;
				x += unwidth(n->d, sy->prec);
				n = n->r;
				if (sy == ySUB || sy == yDIV)
					do {
						x += wwidth(" ") / 2;
						if (t =
						    findmouse(x, y, &sy,
							      sy->prec + 1)) {
							basex = orgx;
							basey = orgy;
							return lst;
						}
						x += unwidth(sy,
							     sy->prec + 1) +
						    wwidth(" ") / 2;
						if (t =
						    findmouse(x, y, &n->d,
							      sy->prec + 1))
							return t;
						x += unwidth(n->d,
							     sy->prec + 1);
					}
					while (n = n->r);
				else
					do {
						x += wwidth(" ") / 2;
						if (t =
						    findmouse(x, y, &sy,
							      sy->prec)) {
							basex = orgx;
							basey = orgy;
							return lst;
						}
						x += unwidth(sy,
							     sy->prec) +
						    wwidth(" ") / 2;
						if (t =
						    findmouse(x, y, &n->d,
							      sy->prec))
							return t;
						x += unwidth(n->d, sy->prec);
					}
					while (n = n->r);
			}
			return 0;
		case tPREFIX:
			if (sy->prec < prec)
				x += wwidth("(");
			if (t = findmouse(x, y, &n->d, sy->prec)) {
				basex = orgx;
				basey = orgy;
				return lst;
			}
			x += unwidth(sy, sy->prec) + wwidth(" ") / 2;
			return findmouse(x, y, &n->r->d, sy->prec);
		case tPOSTFIX:
			if (sy->prec < prec)
				x += wwidth("(");
			if (t = findmouse(x, y, &n->r->d, sy->prec))
				return t;
			x += unwidth(n->r->d, sy->prec) + wwidth(" ") / 2;
			if (t = findmouse(x, y, &n->d, sy->prec)) {
				basex = orgx;
				basey = orgy;
				return lst;
			}
			return 0;
		}
	}
}

void apnd(C **str, C *s)
{
	while (*s)
		*(*str)++ = *s++;
}

void unparsetext(C **str, LST *n, int prec)
{
	SYM *sy = n;
	C *s;
	if (!n) {
		*(*str)++ = '?';
		return;
	}
	switch (typ(n)) {
	case tSYM:
		if (n == yNEG)
			*(*str)++ = '-';
		else if (sy->bind && sy != yE && sy != yPI && sy != yI) {
			s = NTOS(sy->bind);
			apnd(str, s);
			free(s);
		} else
			apnd(str, ((SYM *) n)->s);
		return;
	case tNUM:
		s = NTOS(n);
		apnd(str, s);
		free(s);
		return;
	case tLST:
		switch (sy = n->d, sy->type) {
		case tSYM:
			apnd(str, sy->s);
			*(*str)++ = '(';
			while (n = n->r) {
				unparsetext(str, n->d, 0);
				if (n->r)
					*(*str)++ = ',';
			}
			*(*str)++ = ')';
			return;
		case tINFIX:
			if (sy->prec < prec || sy == yEXP && sy->prec <= prec)
				*(*str)++ = '(';
			n = n->r;
			unparsetext(str, n->d, sy->prec);
			n = n->r;
			if (sy == ySUB || sy == yDIV)
				do {
					unparsetext(str, sy, sy->prec + 1);
					unparsetext(str, n->d, sy->prec + 1);
				}
				while (n = n->r);
			else
				do {
					unparsetext(str, sy, sy->prec);
					unparsetext(str, n->d, sy->prec);
				}
				while (n = n->r);
			if (sy->prec < prec || sy == yEXP && sy->prec <= prec)
				*(*str)++ = ')';
			return;
		case tPREFIX:
			if (sy->prec < prec)
				*(*str)++ = '(';
			unparsetext(str, sy, sy->prec);
			unparsetext(str, n->r->d, sy->prec);
			if (sy->prec < prec)
				*(*str)++ = ')';
			return;
		case tPOSTFIX:
			if (sy->prec < prec)
				*(*str)++ = '(';
			unparsetext(str, n->r->d, sy->prec);
			unparsetext(str, sy, sy->prec);
			if (sy->prec < prec)
				*(*str)++ = ')';
			return;
		}
	}
}

void show(LST *n)
{
	C *s;
	int c;
	if (n) {
		switch (typ(n)) {
		case tSYM:
			fputs(((SYM *) n)->s, stdout);
			break;
		case tNUM:
			s = NTOS(n);
			fputs(s, stdout);
			free(s);
			break;
		case tLST:
			fputc('(', stdout);
			do {
				show(n->d);
				if (n->r)
					fputc(' ', stdout);
			}
			while (n = n->r);
			fputc(')', stdout);
		}
	} else
		fputc('?', stdout);
}
