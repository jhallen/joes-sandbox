#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stddef.h>
#include <string.h>
#include "types.h"
#include "box.h"
#include "sym.h"
#include "builtin.h"
#include "quiz.h"
#include "unparse.h"
#include "simplify.h"

/* Evaluate */

double ev(LST *n)
{
	SYM *sy = (SYM *)n;
	if (n)
		switch (typ(n)) {
		case tNUM:
			return ((NUM *) n)->n;
		case tSYM:
			if (sy == yPI)
				return M_PI;
			if (sy == yE)
				return M_E;
			if (sy->bind)
				return sy->bind->n;
			return 0.0;
		case tLST:
			sy = (SYM *)n->d;
			if (sy == yADD)
				return ev(n->r->d) + ev(n->r->r->d);
			if (sy == ySUB)
				return ev(n->r->d) - ev(n->r->r->d);
			if (sy == yNEG)
				return -ev(n->r->d);
			if (sy == yEXP)
				return pow(ev(n->r->d), ev(n->r->r->d));
			if (sy == yMUL)
				return ev(n->r->d) * ev(n->r->r->d);
			if (sy == yDIV)
				return ev(n->r->d) / ev(n->r->r->d);
			if (sy == ySQRT)
				return sqrt(ev(n->r->d));
			if (sy == yABS)
				return fabs(ev(n->r->d));
			if (sy == ySIN)
				return sin(ev(n->r->d));
			if (sy == yCOS)
				return cos(ev(n->r->d));
			if (sy == yTAN)
				return tan(ev(n->r->d));
			if (sy == yASIN)
				return asin(ev(n->r->d));
			if (sy == yACOS)
				return acos(ev(n->r->d));
			if (sy == yATAN)
				return atan(ev(n->r->d));
			if (sy == yLOG)
				return log(ev(n->r->d));
		}
	return 9999.0;
}

void genlist(LST *n, SYM **ll, I *nn)
{
	I x;
	if (n)
		switch (typ(n)) {
		case tSYM:
			if (((SYM *) n)->bind)
				return;
			for (x = 0; x != *nn; x++)
				if ((SYM *)n == ll[x])
					return;
			ll[*nn] = (SYM *)n;
			*nn = *nn + 1;
			return;
		case tLST:
			do {
				n = n->r;
				if (n)
					genlist(n->d, ll, nn);
			}
			while (n);
			return;
		}
}

I isconst(LST *n)
{
	if (typ(n) == tNUM)
		return 1;
	if (typ(n) == tSYM) {
		SYM *sy = (SYM *)n;
		if (sy->bind)
			return 1;
	}
	return 0;
}

I isfconst(LST *n)
{
	if (n)
		if (typ(n) == tLST)
			if ((SYM *)n->d == yDIV)
				if (n->r->d)
					if (typ(n->r->d) == tNUM)
						if (n->r->r->d)
							if (typ(n->r->r->d) ==
							    tNUM)
								return 1;
	return 0;
}

/* Extract a term from the top level of tree.  A term is defined as a subtree
 * seperated by other subtrees by the infix operators over0 and over1.  *f is
 * incremented each time over1 is found.
 */

LST *extract(SYM *over0, SYM *over1, LST **tree, I *f)
{
	LST *rtn;
	LST *subtree;
	if (!*tree)
		return 0;
	if (typ(*tree) != tLST) {
		rtn = *tree;
		*tree = 0;
		return rtn;
	}
	if ((SYM *)(*tree)->d != over0 && (SYM *)(*tree)->d != over1) {
		rtn = *tree;
		*tree = 0;
		return rtn;
	}
	subtree = (*tree)->r->r->d;
	if ((SYM *)(*tree)->d == over1)
		(*f)++;
	rtn = extract(over0, over1, &subtree, f);
	if (subtree)
		(*tree)->r->r->d = subtree;
	else {
		(*tree)->r->r->d = 0;
		subtree = (*tree)->r->d;
		(*tree)->r->d = 0;
		discard(*tree);
		*tree = subtree;
	}
	return rtn;
}

/* Return greatest common divisor of u and v */

long gcd(long u, long v)
{
	long k, t;
	if (u < 0)
		u = -u;
	if (v < 0)
		v = -v;
	if (u == v)
		return u;
	if (!u)
		return v;
	if (!v)
		return u;

	k = 0;
	while (!(u & 1) && !(v & 1))
		++k, u >>= 1, v >>= 1;

	if (u & 1) {
		t = -v;
		goto b4;
	} else
		t = u;

 b3:	t >>= 1;
 b4:	if (!(t & 1))
		goto b3;
	if (t > 0)
		u = t;
	else
		v = -t;
	t = u - v;
	if (t)
		goto b3;

	return u * (1 << k);
}

/* Reduce a fraction to lowest terms */

void reduce(double *t, double *b)
{
	if (*b != 0.0)
		if (((double)(long)*b) == *b && ((double)(long)*t) == *t) {
			long g = gcd(abs((long)*t), abs((long)*b));
			*b /= (double)g;
			*t /= (double)g;
		} else
			*t /= *b, *b = 1.0;
}

/* Take derivative of list */
/* if flg==0, take partial derivative.  if flg==1, take
   derivative with respect to wrt */

LST *deriv(LST *n, int flg, SYM *wrt)
{
	SYM *sy;
	if (!n)
		return n;
	if (typ(n) == tNUM)
		return (LST *)newnum(0.0);

	if (typ(n) == tSYM)
		if (flg)
			if ((SYM *)n == wrt)
				return (LST *)dupnum(yONE);
			else
				return (LST *)dupnum(yZERO);
		else
			return ncons(2, yD, n);

	sy = (SYM *)n->d;

	if (sy == yNEG)
		return simplify(ncons(2, yNEG, deriv(n->r->d, flg, wrt)), 0, 0);

	else if (sy == yEXP) {
		return simplify(ncons(3, yADD,
				      ncons(3, yMUL,
					    ncons(3, yMUL,
						  ncons(3, yEXP,
							dup(n->r->d),
							ncons(3, ySUB,
							      dup(n->r->r->d),
							      newnum(1.0)
							)
						  ), dup(n->r->r->d)
					    ), deriv(n->r->d, flg, wrt)
				      ),
				      ncons(3, yMUL,
					    ncons(3, yMUL,
						  dup(n),
						  ncons(2, yLOG, dup(n->r->d))
					    ), deriv(n->r->r->d, flg, wrt)
				      )
				), 0, 0);
	} else if (sy == ySQRT)
		return simplify(ncons(3, yMUL,
				      ncons(3, yDIV,
					    dupnum(yONE),
					    ncons(3, yMUL, dupnum(yTWO), dup(n)
					    )
				      ), deriv(n->r->d, flg, wrt)
				), 0, 0);

	else if (sy == yMUL)
		return
		    simplify(ncons
			     (3, yADD,
			      ncons(3, yMUL, dup(n->r->d),
				    deriv(n->r->r->d, flg, wrt)), ncons(3, yMUL,
									dup(n->
									    r->
									    r->
									    d),
									deriv
									(n->r->
									 d, flg,
									 wrt))),
			     0, 0);

	else if (sy == yDIV) {
		NUM *num = newnum(2.0);
		return simplify(ncons(3, yDIV,
				      ncons(3, ySUB,
					    ncons(3, yMUL,
						  dup(n->r->r->d),
						  deriv(n->r->d, flg, wrt)
					    ),
					    ncons(3, yMUL,
						  dup(n->r->d),
						  deriv(n->r->r->d, flg, wrt)
					    )
				      ), ncons(3, yEXP, dup(n->r->r->d), num)
				), 0, 0);
	}

	else if (sy == yADD)
		return
		    simplify(ncons
			     (3, yADD, deriv(n->r->d, flg, wrt),
			      deriv(n->r->r->d, flg, wrt)), 0, 0);

	else if (sy == ySUB)
		return
		    simplify(ncons
			     (3, ySUB, deriv(n->r->d, flg, wrt),
			      deriv(n->r->r->d, flg, wrt)), 0, 0);

	else if (sy == yEQ)
		return ncons(3, yEQ, deriv(n->r->d, flg, wrt),
			     deriv(n->r->r->d, flg, wrt));

	else if (sy == ySIN)
		return
		    simplify(ncons
			     (3, yMUL, ncons(2, yCOS, dup(n->r->d)),
			      deriv(n->r->d, flg, wrt)), 0, 0);

	else if (sy == yCOS)
		return
		    simplify(ncons
			     (3, yMUL,
			      ncons(2, yNEG, ncons(2, ySIN, dup(n->r->d))),
			      deriv(n->r->d, flg, wrt)), 0, 0);

	if (flg)
		return ncons(2, yDERV, dup(n));
	else
		return ncons(2, yD, dup(n));
}

I match(LST *a, LST *b)
{
	if (!a)
		if (!b)
			return 1;
		else
			return 0;
	if (!b)
		return 0;
	if (typ(a) == tNUM) {
		if (typ(b) == tNUM)
			if (((NUM *) a)->n == ((NUM *) b)->n)
				return 1;
		return 0;
	}
	if (typ(a) == tSYM)
		if (a == b)
			return 1;
		else
			return 0;
	if ((SYM *)a->d == yADD || (SYM *)a->d == ySUB) {
		LST *q;
		I f, nt = 0, x;
		struct term {
			LST *term;
			I n;
		} *terms = calloc(100, sizeof(struct term));
		a = dup(a);
		b = dup(b);
		while (f = 0, q = extract(yADD, ySUB, &a, &f))
			terms[nt].n = (f & 1), terms[nt++].term = q;
		while (f = 0, q = extract(yADD, ySUB, &b, &f)) {
			for (x = 0; x != nt; x++)
				if (match(q, terms[x].term)
				    && (f & 1) == terms[x].n) {
					discard(q);
					discard(terms[x].term);
					terms[x].term = 0;
					goto okk;
				}
			goto matchno;
 okk:			;
		}
		for (x = 0; x != nt; x++)
			if (terms[x].term)
				goto matchno;
		free(terms);
		return 1;
 matchno:
		for (x = 0; x != nt; x++)
			if (terms[x].term)
				discard(terms[x].term);
		free(terms);
		return 0;
	}
	if ((SYM *)a->d == yMUL || (SYM *)a->d == yDIV) {
		LST *q;
		I f, nt = 0, x;
		struct term {
			LST *term;
			I n;
		} *terms = calloc(100, sizeof(struct term));
		a = dup(a);
		b = dup(b);
		while (f = 0, q = extract(yMUL, yDIV, &a, &f))
			terms[nt].n = (f & 1), terms[nt++].term = q;
		while (f = 0, q = extract(yMUL, yDIV, &b, &f)) {
			for (x = 0; x != nt; x++)
				if (match(q, terms[x].term)
				    && (f & 1) == terms[x].n) {
					discard(q);
					discard(terms[x].term);
					terms[x].term = 0;
					goto ok;
				}
			goto nomatch;
 ok:			;
		}
		for (x = 0; x != nt; x++)
			if (terms[x].term)
				goto nomatch;
		free(terms);
		return 1;
 nomatch:
		for (x = 0; x != nt; x++)
			if (terms[x].term)
				discard(terms[x].term);
		free(terms);
		return 0;
	}
	while (match(a->d, b->d)) {
		a = a->r;
		b = b->r;
		if (!a && !b)
			return 1;
		if (!a || !b)
			break;
	}
	return 0;
}

/* Remove constant multiplied by a term.  Only works after it has been
   simplified. */

LST *getconst(LST *q, double *t, double *b)
{
	*t = 1.0;
	*b = 1.0;
	if (q)
		if (typ(q) == tLST) {
			if ((SYM *)q->d == yMUL) {
				if (q->r->d && typ(q->r->d) == tNUM) {
					LST *l;
					*t = ((NUM *) q->r->d)->n;
					l = q;
					q = q->r->r->d;
					l->r->r->d = 0;
					discard(l);
				}
			} else if ((SYM *)q->d == yDIV) {
				LST *qq = q->r->d;
				if (qq && typ(qq) == tNUM)
					*t = ((NUM *) qq)->n, ((NUM *) qq)->n =
					    1.0;
				else if (qq && typ(qq) == tLST && (SYM *)qq->d == yMUL
					 && qq->r->d && typ(qq->r->d) == tNUM) {
					*t = ((NUM *) qq->r->d)->n;
					q->r->d = qq->r->r->d;
					qq->r->r->d = 0;
					discard(qq);
				}
				qq = q->r->r->d;
				if (qq && typ(qq) == tNUM) {
					*b = ((NUM *) qq)->n;
					qq = q;
					q = q->r->d;
					qq->r->d = 0;
					discard(qq);
				} else if (qq && typ(qq) == tLST
					   && (SYM *)qq->d == yMUL && qq->r->d
					   && typ(qq->r->d) == tNUM) {
					*b = ((NUM *) qq->r->d)->n;
					q->r->r->d = qq->r->r->d;
					qq->r->r->d = 0;
					discard(qq);
				}
			}
		}
	return q;
}

void indent(int ind)
{
	while  (ind--)
		printf(" ");
}

LST *simplify(LST *n, int r, int ind)
{
	SYM *sy;
	indent(ind); printf("Simplify %d:", r); show(n); printf("\n");

	ind += 4;

	if (!n) {
		indent(ind); printf("NULL list\n");
		return 0;
	}

	if (typ(n) != tLST) {
		indent(ind); printf("not a list\n");
		return n;
	}

	switch (sy = (SYM *)n->d, sy->type) {
	case tINFIX:
		n->r->d = simplify(n->r->d, r, ind);
		n->r->r->d = simplify(n->r->r->d, r, ind);
		if (sy == yEXP && isconst(n->r->d) && isconst(n->r->r->d)) {
			double d1, d2;
			if (typ(n->r->d) == tNUM)
				d1 = ((NUM *) n->r->d)->n;
			else
				d1 = ((SYM *) n->r->d)->bind->n;
			if (typ(n->r->r->d) == tNUM)
				d2 = ((NUM *) n->r->r->d)->n;
			else
				d2 = ((SYM *) n->r->r->d)->bind->n;
			if (d2 == 0.0
			    || ((d2 >= -1.0 && d2 <= 1.0) ? d1 >= 0.0 : 1))
				if (d1 = pow(d1, d2), d1 == (double)(I) d1) {
					discard(n);
					n = (LST *)newnum(d1);
				}
			return n;
		}
		if (sy == yEXP && isconst(n->r->r->d)) {
			double d;
			if (typ(n->r->r->d) == tNUM)
				d = ((NUM *) n->r->r->d)->n;
			else
				d = ((SYM *) n->r->r->d)->bind->n;
			if (d == 0.0) {
				discard(n);
				n = (LST *)newnum(1.0);
				return n;
			}
			if (d == 1.0) {
				LST *a = n->r->d;
				n->r->d = 0;
				discard(n);
				return a;
			}
		}
		if (sy == yEXP) {
			if (n->r->d)
				if (typ(n->r->d) == tLST)
					if ((SYM *)n->r->d->d == yEXP) {
						LST *q = n->r->d;
						q->r->r->d =
						    simplify(ncons
							     (3, yMUL,
							      q->r->r->d,
							      n->r->r->d), r, ind);
						n->r->r->d = 0;
						n->r->d = 0;
						discard(n);
						return q;
					}
		}
		if (sy == yADD || sy == ySUB) {
			I f;	/* Odd for subtract terms */
			LST *q;	/* Extracted term */
			double t = 0.0, b = 1.0;	/* Current number */
			LST *nn = 0, *tt = 0, *nu;
			I fneg = 0, j, tadd = 0;
			struct th {
				LST *term;
				LST *n;
			} *terms = calloc(100, sizeof(struct th));
			I nt = 0, x;

			/* Extract terms, accumulate constant */
			while (f = 0, q = extract(yADD, ySUB, &n, &f))
				if (isconst(q)) {
					double d;
					if (typ(q) == tNUM)
						d = ((NUM *) q)->n;
					else
						d = ((SYM *) q)->bind->n;
					discard(q);
					if (f & 1)
						t -= d * b;
					else
						t += d * b;
				} else if (isfconst(q)) {
					double ob = b;
					double a = ((NUM *) q->r->d)->n;
					double u = ((NUM *) q->r->r->d)->n;
					t *= u;
					b *= u;
					t += (f & 1 ? -ob * a : ob * a);
					reduce(&t, &b);
					discard(q);
				} else {
					/* Remove constant from term if possible */
					double t, b;
					LST *new;
					q = getconst(q, &t, &b);
					if (f & 1)
						t = -t;
					if (b == 1.0)
						new = (LST *)newnum(t);
					else if (t == 0.0 && b != 0.0)
						new = (LST *)newnum(t);
					else
						new =
						    ncons(3, yDIV, newnum(t),
							  newnum(b));
					/* Search for term */
					for (j = 0; j != nt; ++j)
						if (match(q, terms[j].term)) {
							terms[j].n =
							    ncons(3, yADD,
								  terms[j].n,
								  new);
							discard(q);
							goto ov;
						}
					terms[nt].n = new;
					terms[nt++].term = q;
 ov:					;
				}

			for (x = 0; x != nt; ++x) {
				terms[x].n = simplify(terms[x].n, r, ind);
				if (isconst(terms[x].n))
					if (((NUM *) terms[x].n)->n < 0.0) {
						((NUM *) terms[x].n)->n =
						    -((NUM *) terms[x].n)->n;
						if (tt) {
							nn = nn->r->d =
							    ncons(3, ySUB, 0,
								  simplify(ncons
									   (3,
									    yMUL,
									    terms
									    [x].
									    n,
									    terms
									    [x].
									    term),
									   r, ind));
							if (x == nt - 1)
								fneg = 1;
							goto or;
						} else {
							tt = nn =
							    ncons(3, ySUB, 0,
								  simplify(ncons
									   (3,
									    yMUL,
									    terms
									    [x].
									    n,
									    terms
									    [x].
									    term),
									   r, ind));
							if (x == nt - 1)
								fneg = 1;
							goto or;
						}
					}
				if (tt)
					if (x != nt - 1)
						nn = nn->r->d =
						    ncons(3, yADD, 0,
							  simplify(ncons
								   (3, yMUL,
								    terms[x].n,
								    terms[x].
								    term), r, ind));
					else
						nn->r->d =
						    simplify(ncons
							     (3, yMUL,
							      terms[x].n,
							      terms[x].term),
							     r, ind);
				else if (x != nt - 1)
					tt = nn =
					    ncons(3, yADD, 0,
						  simplify(ncons
							   (3, yMUL, terms[x].n,
							    terms[x].term), r, ind));
				else
					tt = nn =
					    simplify(ncons
						     (3, yMUL, terms[x].n,
						      terms[x].term), r, ind);
 or:				;
			}

			if (b < 0.0)
				b = -b, t = -t;
			if (!fneg && t < 0.0)
				t = -t, tadd = 1;

			if (b == 1.0)
				nu = (LST *)newnum(t);
			else
				nu = ncons(3, yDIV, newnum(t), newnum(b));

			if (!tt) {
				if (tadd)
					n = ncons(2, yNEG, nu);
				else
					n = nu;
				goto out;
			}

			if (t != 0.0)
				if (fneg) {
					if (tadd)
						nu = ncons(2, yNEG, nu);
					nn->r->d = nu;
					n = tt;
					goto out;
				} else {
					if (!tadd) {
						n = ncons(3, yADD, tt, nu);
						goto out;
					} else {
						n = ncons(3, ySUB, tt, nu);
						goto out;
					}
				}

			discard(nu);

			if (fneg) {
				nn->d = (LST *)yNEG;
				nn->r->d = nn->r->r->d;
				nn->r->r->d = 0;
				discard(nn->r->r);
				nn->r->r = 0;
			}

			n = tt;
 out:			;
			if ((SYM *)n->d == yADD) {
				LST *q;
				if (n->r->r->d && typ(n->r->r->d) == tLST
				    && (SYM *)n->r->r->d->d == yNEG) {
 up:
					q = n->r->r->d;
					n->r->r->d = q->r->d;
					q->r->d = 0;
					discard(q);
					n->d = (LST *)ySUB;
				} else if (n->r->d && typ(n->r->d) == tLST
					   && (SYM *)n->r->d->d == yNEG) {
					q = n->r->d;
					n->r->d = n->r->r->d;
					n->r->r->d = q;
					goto up;
				}
			}
			if ((SYM *)n->d == ySUB) {
				LST *q;
				if (n->r->r->d && typ(n->r->r->d) == tLST
				    && (SYM *)n->r->r->d->d == yNEG) {
					q = n->r->r->d;
					n->r->r->d = q->r->d;
					q->r->d = 0;
					discard(q);
					n->d = (LST *)yADD;
				}
			}
			return n;
		}
		if (sy == yMUL || sy == yDIV) {
			I f;	/* Odd for divide term */
			I tadd = 0;
			NUM *tc = newnum(1.0);	/* Constant part of numerator */
			struct th {
				LST *term;
				LST *n;
			} *t = calloc(100, sizeof(struct th));
			I nt = 0;
			I j;
			LST *nn;
			NUM *bc = newnum(1.0);	/* Constant part of denominator */
			LST *tt = 0, *bb = 0;
			LST *q;	/* Returned term */

			/* Extract terms */
			while (f = 0, q = extract(yMUL, yDIV, &n, &f))
				if (isconst(q)) {
					double d;
					if (typ(q) == tNUM)
						d = ((NUM *) q)->n;
					else
						d = ((SYM *) q)->bind->n;
					discard(q);
					if (f & 1)
						bc->n *= d;
					else
						tc->n *= d;
				} else {
					LST *p, *tmp;
					for (j = 0; j != nt; ++j)
						if (match(q, t[j].term)) {
							if (f & 1)
								t[j].n =
								    ncons(3,
									  yADD,
									  t[j].
									  n,
									  newnum(-1.0));
							else
								t[j].n =
								    ncons(3,
									  yADD,
									  t[j].
									  n,
									  newnum(1.0));
							discard(q);
							goto ovr;
						}
					if (typ(q) == tLST)
						if ((SYM *)q->d == yEXP) {
							tmp = q;
							p = q->r->r->d;
							q = q->r->d;
							tmp->r->d = 0;
							tmp->r->r->d = 0;
							discard(tmp);
							for (j = 0; j != nt;
							     ++j)
								if (match
								    (q,
								     t[j].
								     term)) {
									if (f &
									    1)
										t[j].n = ncons(3, yADD, t[j].n, ncons(2, yNEG, p));
									else
										t[j].n = ncons(3, yADD, t[j].n, p);
									discard
									    (q);
									goto ovr;
								}
							goto ovr1;
						}
					p = (LST *)newnum(1.0);
 ovr1:
					t[nt].term = q;
					t[nt++].n =
					    ((f & 1) ? ncons(2, yNEG, p) : p);
 ovr:					;
				}

			/* Test for zero return */
			if (tc->n == 0.0) {
				for (j = 0; j != nt; ++j)
					discard(t[j].term);
				free(t);
				discardnum(bc);
				return (LST *)tc;
			}

			/* Build */
			for (j = nt - 1; j >= 0; --j) {
				t[j].n = simplify(t[j].n, r, ind);
				if (!isconst(t[j].n)) {
					t[j].term =
					    ncons(3, yEXP, t[j].term, t[j].n);
					if (!tt)
						tt = t[j].term;
					else
						tt = ncons(3, yMUL, tt,
							   t[j].term);
				} else if (((NUM *) t[j].n)->n > 0) {
					if (((NUM *) t[j].n)->n != 1.0)
						t[j].term =
						    ncons(3, yEXP, t[j].term,
							  t[j].n);
					else
						discard(t[j].n);
					if (!tt)
						tt = t[j].term;
					else
						tt = ncons(3, yMUL, tt,
							   t[j].term);
				} else if (((NUM *) t[j].n)->n < 0) {
					if (((NUM *) t[j].n)->n != -1.0)
						t[j].term =
						    ncons(3, yEXP, t[j].term,
							  ncons(2, yNEG,
								t[j].n));
					else
						discard(t[j].n);
					if (!bb)
						bb = t[j].term;
					else
						bb = ncons(3, yMUL, bb,
							   t[j].term);
				} else
					discard(t[j].term), discard(t[j].n);
			}
			free(t);

			/* Reduce */
			reduce(&tc->n, &bc->n);
			if (bc->n < 0.0)
				bc->n = -bc->n, tc->n = -tc->n;
			if (tt && tc->n < 0.0)
				tc->n = -tc->n, tadd = 1;

			/* Multiply constants by other terms */
			if (!tt)
				tt = (LST *)tc;
			else if (tc->n == 1.0)
				discardnum(tc);
			else {
				nn = newlst();
				nn->d = (LST *)yMUL;
				nn->r = newlst();
				nn->r->r = newlst();
				nn->r->d = (LST *)tc;
				nn->r->r->d = tt;
				tt = nn;
			}
			if (!bb) {
				bb = (LST *)bc;
				/* If only thing on bottom is 1.0 */
				if (bc->n == 1.0) {
					if (tadd)
						return ncons(2, yNEG, tt);
					return tt;
				}
			} else if (bc->n == 1.0)
				discardnum(bc);
			else {
				nn = newlst();
				nn->d = (LST *)yMUL;
				nn->r = newlst();
				nn->r->r = newlst();
				nn->r->d = (LST *)bc;
				nn->r->r->d = bb;
				bb = nn;
			}

			/* Divide top by bottom */
			n = newlst();
			n->r = newlst();
			n->r->r = newlst();
			n->d = (LST *)yDIV;
			n->r->d = tt;
			n->r->r->d = bb;
			if (tadd)
				return ncons(2, yNEG, n);
			return n;
		}
		return n;
	case tPREFIX:
	case tPOSTFIX:
	case tSYM:
		n->r->d = simplify(n->r->d, r, ind);
		if (isconst(n->r->d)) {
			double d;
			if (typ(n->r->d) == tNUM)
				d = ((NUM *) n->r->d)->n;
			else
				d = ((SYM *) n->r->d)->bind->n;
			if (sy == yNEG) {
				NUM *nn = newnum(-d);
				discard(n);
				n = (LST *)nn;
			} else if (sy == ySQRT && d >= 0.0) {
				if (d == (double)((I) sqrt(d) * (I) sqrt(d))) {
					NUM *nn = newnum(sqrt(d));
					discard(n);
					n = (LST *)nn;
				}
			} else if (sy == yLOG && d == 1.0) {
				NUM *nn = newnum(0.0);
				discard(n);
				n = (LST *)nn;
			} else if (sy == yLOG && d == M_E) {
				NUM *nn = newnum(1.0);
				discard(n);
				n = (LST *)nn;
			} else if (sy == yD || sy == yDERV) {
				NUM *nn = newnum(0.0);
				discard(n);
				n = (LST *)nn;
			}
		} else if (sy == yNEG) {
			LST *nn;
			if (n->r->d)
				if (typ(n->r->d) == tLST)
					if (n->r->d->d == (LST *)yNEG) {
						nn = n->r->d->r->d;
						n->r->d->r->d = 0;
						discard(n);
						return nn;
					}
		} else if (sy == yLOG) {
			LST *nn, *q;
			if (n->r->d)
				if (typ(n->r->d) == tLST)
					if (n->r->d->d == (LST *)yEXP) {
						nn = n->r->d->r->r->d;
						q = n->r->d;
						n->r->d = q->r->d;
						q->r->d = 0;
						q->r->r->d = 0;
						discard(q);
						n = simplify(ncons
							     (3, yMUL, nn, n),
							     r, ind);
					}
		} else if (sy == yD && r) {
			LST *nn = deriv(n->r->d, 0, 0);
			discard(n);
			n = nn;
		} else if (sy == yDERV && r) {
			LST *nn = deriv(n->r->d, 1, lookup("x"));
			discard(n);
			n = nn;
		}
		return n;
	}
	return n;
}

LST *distribute(LST *n)
{
	LST *l;
	if (!n)
		return 0;
	if (typ(n) != tLST)
		return n;
	for (l = n->r; l; l = l->r)
		l->d = distribute(l->d);
	if ((SYM *)n->d == yMUL) {
		if (n->r->d && typ(n->r->d) == tLST
		    && ((SYM *)n->r->d->d == yADD || (SYM *)n->r->d->d == ySUB)) {
			LST *z = n->r->d;
			I f;
			LST *q;
			LST *nn = 0, *uu;
			while (f = 0, q = extract(yADD, ySUB, &z, &f))
				if (nn)
					if (!z)
						uu->r->d =
						    ncons(3, yMUL, q,
							  dup(n->r->r->d));
					else if (f & 1)
						uu = uu->r->d =
						    ncons(3, ySUB, 0,
							  ncons(3, yMUL, q,
								dup(n->r->r->
								    d)));
					else
						uu = uu->r->d =
						    ncons(3, yADD, 0,
							  ncons(3, yMUL, q,
								dup(n->r->r->
								    d)));
				else if (!z)
					nn = uu =
					    ncons(3, yMUL, q, dup(n->r->r->d));
				else if (f & 1)
					nn = uu =
					    ncons(3, ySUB, 0,
						  ncons(3, yMUL, q,
							dup(n->r->r->d)));
				else
					nn = uu =
					    ncons(3, yADD, 0,
						  ncons(3, yMUL, q,
							dup(n->r->r->d)));
			n->r->d = 0;
			discard(n);
			n = nn;
			n = distribute(n);
		} else if (n->r->r->d && typ(n->r->r->d) == tLST
			   && ((SYM *)n->r->r->d->d == yADD
			       || (SYM *)n->r->r->d->d == ySUB)) {
			LST *z = n->r->r->d;
			I f;
			LST *q;
			LST *nn = 0, *uu;
			while (f = 0, q = extract(yADD, ySUB, &z, &f))
				if (nn)
					if (!z)
						uu->r->d =
						    ncons(3, yMUL, dup(n->r->d),
							  q);
					else if (f & 1)
						uu = uu->r->d =
						    ncons(3, ySUB, 0,
							  ncons(3, yMUL,
								dup(n->r->d),
								q));
					else
						uu = uu->r->d =
						    ncons(3, yADD, 0,
							  ncons(3, yMUL,
								dup(n->r->d),
								q));
				else if (!z)
					nn = uu =
					    ncons(3, yMUL, dup(n->r->d), q);
				else if (f & 1)
					nn = uu =
					    ncons(3, ySUB, 0,
						  ncons(3, yMUL, dup(n->r->d),
							q));
				else
					nn = uu =
					    ncons(3, yADD, 0,
						  ncons(3, yMUL, dup(n->r->d),
							q));
			n->r->r->d = 0;
			discard(n);
			n = nn;
			n = distribute(n);
		}
	}
	return n;
}

I isin(LST *n, LST *q)
{
	LST *z;
	I f;
	I yes = 0;
	n = dup(n);
	while (z = extract(yMUL, 0, &n, &f)) {
		if (match(z, q))
			yes = 1;
		discard(z);
	}
	return yes;
}

LST *rmv(LST *n, LST *q)
{
	if (n && typ(n) == tLST && ((SYM *)n->d == yADD || (SYM *)n->d == ySUB)) {
		if (match(n->r->d, q)) {
			discard(n->r->d);
			n->r->d = (LST *)newnum(1.0);
		} else
			n->r->d = rmv(n->r->d, q);
		if (match(n->r->r->d, q)) {
			discard(n->r->r->d);
			n->r->r->d = (LST *)newnum(1.0);
		} else
			n->r->r->d = rmv(n->r->r->d, q);
	} else if (n && typ(n) == tLST && (SYM *)n->d == yMUL) {
		if (match(n->r->d, q)) {
			discard(n->r->d);
			n->r->d = (LST *)newnum(1.0);
		} else if (n->r->d && typ(n->r->d) == tLST
			   && (SYM *)n->r->d->d == yMUL)
			n->r->d = rmv(n->r->d, q);
		if (match(n->r->r->d, q)) {
			discard(n->r->r->d);
			n->r->r->d = (LST *)newnum(1.0);
		} else if (n->r->r->d && typ(n->r->r->d) == tLST
			   && (SYM *)n->r->r->d->d == yMUL)
			n->r->r->d = rmv(n->r->r->d, q);
	}
	return n;
}

LST *qp0, *qp1, *qp2, *qp3, *qp4, *qp5;

void izsimp()
{
	char buf[80];
	qp0 = parseeqn(strcpy(buf, "%1*%0^2+%2*%0+%3"));
	qp1 = parseeqn(strcpy(buf, "   %0^2+%2*%0+%3"));
	qp2 = parseeqn(strcpy(buf, "%1*%0^2+   %0+%3"));
	qp3 = parseeqn(strcpy(buf, "   %0^2+   %0+%3"));
	qp4 = parseeqn(strcpy(buf, "%1*%0^2+      %3"));
	qp5 = parseeqn(strcpy(buf, "   %0^2+      %3"));
}

void clrmatch()
{
	SYM *q;
	q = lookup("%0");
	if (!q)
		q = add("%0");
	discard((LST *)q->bind), q->bind = 0;
	q = lookup("%1");
	if (!q)
		q = add("%1");
	discard((LST *)q->bind), q->bind = 0;
	q = lookup("%2");
	if (!q)
		q = add("%2");
	discard((LST *)q->bind), q->bind = 0;
	q = lookup("%3");
	if (!q)
		q = add("%3");
	discard((LST *)q->bind), q->bind = 0;
	q = lookup("%4");
	if (!q)
		q = add("%4");
	discard((LST *)q->bind), q->bind = 0;
}

I patternmatch(LST *a, LST *b)
{
	if (!a)
		if (!b)
			return 1;
		else
			return 0;
	if (typ(a) == tSYM)
		if (((SYM *) a)->s[0] == '%')
			if (!((SYM *) a)->bind) {
				((SYM *) a)->bind = (NUM *)dup(b);
				return 1;
			} else
				return match((LST *)((SYM *) a)->bind, b);
		else if (a == b)
			return 1;
		else
			return 0;
	if (!b)
		return 0;
	if (typ(a) == tNUM) {
		if (typ(b) == tNUM)
			if (((NUM *) a)->n == ((NUM *) b)->n)
				return 1;
		return 0;
	}
	while (patternmatch(a->d, b->d)) {
		a = a->r;
		b = b->r;
		if (!a && !b)
			return 1;
		if (!a || !b)
			break;
	}
	return 0;
}

I shufflematch(LST *a, LST *b, LST *c)
{
	LST *t;
	SYM *sy;
	if (!a) {
		return clrmatch(), patternmatch(b, c);
	}
	if (typ(a) != tLST) {
		return clrmatch(), patternmatch(b, c);
	}
	sy = (SYM *)a->d;
	if (sy->type != tINFIX) {
		return clrmatch(), patternmatch(b, c);
	}

	if (shufflematch(a->r->d, b, c))
		return 1;
	if (shufflematch(a->r->r->d, b, c))
		return 1;
	if (sy == yMUL || sy == yADD) {
		t = a->r->d, a->r->d = a->r->r->d, a->r->r->d = t;
		if (shufflematch(a->r->d, b, c))
			return 1;
		if (shufflematch(a->r->r->d, b, c))
			return 1;
	}
	return 0;
}

LST *factor(LST *n)
{
	LST *l;
	if (!n)
		return 0;
	if (typ(n) != tLST)
		return n;
	l = dup(n);
	if (shufflematch(l, qp0, l))
		goto in;
	if (shufflematch(l, qp1, l)) {
		lookup("%1")->bind = newnum(1.0);
		goto in;
	}
	if (shufflematch(l, qp2, l)) {
		lookup("%2")->bind = newnum(2.0);
		goto in;
	}
	if (shufflematch(l, qp3, l)) {
		lookup("%1")->bind = newnum(1.0);
		lookup("%2")->bind = newnum(1.0);
		goto in;
	}
/*
if(shufflematch(l,qp4,l)) { lookup("%2")->bind=newn(0.0); goto in; }
if(shufflematch(l,qp5,l)) { lookup("%1")->bind=newn(1.0); lookup("%2")->bind=newn(0.0); goto in; }
*/
	goto ovr;
 in:
	discard(n);
	return ncons(3, yMUL,
		     ncons(3, ySUB, dup((LST *)lookup("%0")->bind),
			   ncons(3, yDIV,
				 ncons(3, yADD,
				       ncons(2, yNEG, dup((LST *)lookup("%2")->bind)),
				       ncons(2, ySQRT,
					     ncons(3, ySUB,
						   ncons(3, yEXP,
							 dup((LST *)lookup("%2")->
							     bind), newnum(2.0)),
						   ncons(3, yMUL, newnum(4.0),
							 ncons(3, yMUL,
							       dup((LST *)lookup
								   ("%1")->
								   bind),
							       dup((LST *)lookup
								   ("%3")->
								   bind))))
				       )
				 ),
				 ncons(3, yMUL, newnum(2.0),
				       dup((LST *)lookup("%1")->bind))
			   )
		     ),
		     ncons(3, ySUB, dup((LST *)lookup("%0")->bind),
			   ncons(3, yDIV,
				 ncons(3, ySUB,
				       ncons(2, yNEG, dup((LST *)lookup("%2")->bind)),
				       ncons(2, ySQRT,
					     ncons(3, ySUB,
						   ncons(3, yEXP,
							 dup((LST *)lookup("%2")->
							     bind), newnum(2.0)),
						   ncons(3, yMUL, newnum(4.0),
							 ncons(3, yMUL,
							       dup((LST *)lookup
								   ("%1")->
								   bind),
							       dup((LST *)lookup
								   ("%3")->
								   bind))))
				       )
				 ),
				 ncons(3, yMUL, newnum(2.0),
				       dup((LST *)lookup("%1")->bind))
			   )
		     )
	    );
 ovr:
	discard(l);
	for (l = n->r; l; l = l->r)
		l->d = factor(l->d);
	if ((SYM *)n->d == yADD || (SYM *)n->d == ySUB) {
		LST *z = dup(n);
		I f, j;
		LST *q, *r;
		LST **terms = calloc(100, sizeof(LST *));
		I nt = 0;
		while (q = extract(yADD, ySUB, &z, &f))
			while (r = extract(yMUL, 0, &q, &f)) {
				for (f = 0; f != nt; ++f)
					if (match(r, terms[f]))
						goto vo;
				terms[nt++] = r;
 vo:				;
			}
		r = 0;
		for (j = 0; j != nt; ++j) {
			I yes = 1;
			z = dup(n);
			while (q = extract(yADD, ySUB, &z, &f)) {
				if (!isin(q, terms[j]))
					yes = 0;
				discard(q);
			}
			if (yes) {
				if (r)
					r = ncons(3, yMUL, r, terms[j]);
				else
					r = terms[j];
				rmv(n, terms[j]);
			} else
				discard(terms[j]);
		}
		if (r)
			n = simplify(ncons(3, yMUL, r, n), 0, 0);
	}
	return n;
}
