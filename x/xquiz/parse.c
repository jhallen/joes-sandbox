/* Parser */

#include <stdio.h>
#include <stddef.h>
#include "types.h"
#include "box.h"
#include "scan.h"
#include "builtin.h"
#include "parse.h"

I err;

LST *parse(int prec)
{
	LST *n;
	TOKEN *t = gettok();
	if (!t) {
		err = 1;
		return 0;
	}
	if (t->type == tEOF) {
		rmtok(t);
		return 0;
	}
	if (t->type == tNUM) {
		n = (LST *)t->num;
		rmtok(t);
	} else if (t->type == tSYM) {
		n = (LST *)t->sym;
		rmtok(t);
		t = gettok();
		if (t->type == tLPAREN) {
			LST *g = newlst(), *h;
			rmtok(t);
			g->d = n;
			n = g;
 comma:
			if (h = parse(0)) {
				g = g->r = newlst();
				g->d = h;
				t = gettok();
				if (t->type == tCOMMA) {
					rmtok(t);
					goto comma;
				}
				if (t->type == tEOF) {
					if (!err)
						err = 2;
					rmtok(t);
					return n;
				}
				if (t->type != tRPAREN) {
					err = 1;
					rmtok(t);
					return n;
				}
				rmtok(t);
			} else {
				g->r = newlst();
				if (!err)
					err = 2;
			}
		} else
			ungettok(t);
	} else if (t->type == tLPAREN) {
		rmtok(t);
		n = parse(0);
		t = gettok();
		if (t->type == tEOF) {
			if (!err)
				err = 2;
			rmtok(t);
			return n;
		}
		if (t->type != tRPAREN) {
			err = 1;
			rmtok(t);
			return n;
		}
		if (!n) {
			err = 1;
			return 0;
		}
		rmtok(t);
	} else if (t->type == tINFIX && t->sym == ySUB) {
		n = ncons(2, yNEG, parse(yNEG->prec));
		rmtok(t);
	} else if (t->type == tPREFIX) {
		n = ncons(2, t->sym, parse(t->prec));
		rmtok(t);
	} else if (t->type == tINFIX && t->sym == yADD) {
		n = parse(yNEG->prec);
		rmtok(t);
		if (!n) {
			if (!err)
				err = 2;
			return 0;
		}
	} else {
		ungettok(t);
		err = 1;
		return 0;
	}
	if (!n)
		return 0;

 up:
	t = gettok();
	if (!t)
		return 0;
	if (t->type == tINFIX) {
		if (prec > t->prec || prec == t->prec && !t->ass) {
			ungettok(t);
			return n;
		}
		n = ncons(3, t->sym, n, parse(t->prec));
		rmtok(t);
		goto up;
	} else if (t->type == tEOF || t->type == tRPAREN || t->type == tCOMMA) {
		ungettok(t);
		return n;
	} else if (t->type == tPOSTFIX) {
		if (prec >= t->prec) {
			ungettok(t);
			return n;
		}
		n = ncons(2, t->sym, n);
		rmtok(t);
		goto up;
	} else {
		rmtok(t);
		err = 1;
		return n;
	}
}
