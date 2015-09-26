/* Run-time library

   Copyright (C) 1993 Joseph H. Allen

This file is part of IVY

IVY is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 1, or (at your option) any later version.

IVY is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
details.

You should have received a copy of the GNU General Public License along with
IVY; see the file COPYING.  If not, write to the Free Software Foundation,
675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <setjmp.h>
#include "error.h"
#include "ivy_tree.h"
#include "ivy.h"

/* Functions for reading args */

int getintarg(Ivy *ivy, Obj * args, int n, long long *rtn)
{
	Var *v;
	if (n >= args->nitems) {
		error_0(ivy->errprn, "Incorrect number of args");
		return -1;
	}
	v = getn(args, n);
	if (v->val.type == tNUM) {
		*rtn = v->val.u.num;
		return 0;
	} else {
		error_0(ivy->errprn, "Incorrect arg type");
		return -1;
	}
}

int getstringarg(Ivy *ivy, Obj * args, int n, char **rtn)
{
	Var *v;
	if (n >= args->nitems) {
		error_0(ivy->errprn, "Incorrect number of args");
		return -1;
	}
	v = getn(args, n);
	if (v->val.type == tSTR) {
		*rtn = v->val.u.str->s;
		return 0;
	} else {
		error_0(ivy->errprn, "Incorrect arg type");
		return -1;
	}
}

int getdoublearg(Ivy *ivy, Obj * args, int n, double *rtn)
{
	Var *v;
	if (n >= args->nitems) {
		error_0(ivy->errprn, "Incorrect number of args");
		return -1;
	}
	v = getn(args, n);
	if (v->val.type == tFP) {
		*rtn = v->val.u.fp;
		return 0;
	} else {
		error_0(ivy->errprn, "Incorrect arg type");
		return -1;
	}
}

/* Write output */

void rtprint(Ivy *ivy)
{
	Obj *a = getv(ivy, "argv")->val.u.obj;
	int x;
	for (x = 0; x != a->nitems; ++x) {
		Var *v = getn(a, x);
		switch (v->val.type) {
			case tSTR: {
				fprintf(ivy->out, "%s", v->val.u.str->s);
				break;
			} case tNUM: {
				fprintf(ivy->out, "%lld", v->val.u.num);
				break;
			} case tFP: {
				fprintf(ivy->out, "%g", v->val.u.fp);
				break;
			} default: {
				pr(ivy->out, &v->val,0);
				break;
			}
		}
	}
	fprintf(ivy->out, "\n");
	mkval(psh(ivy), tVOID);
}

/* Printf the output */

void rtprintf(Ivy *ivy)
{
	Obj *a = getv(ivy, "argv")->val.u.obj;
	if (a->nitems == 0)
		error_0(ivy->errprn, "Incorrect number of args to printf");
	else {
		Var *v = getn(a, 0);
		if (v->val.type == tSTR) {
			char *s = v->val.u.str->s;
			int x, y;
			int n = 1;
			char buf[1024];
			while (*s) {
				/* Copy to first % or EOS */
				y = 0;
				for (x = 0; s[x] && s[x] != '%'; ++x)
					buf[y++] = s[x];
				if (s[x]) {	/* We have a %nnnnnX */
					buf[y++] = s[x];
					++x;
					if (s[x] == '%') {	/* Escape */
						buf[y++] = '%';
						++x;
						buf[y] = 0;
						fprintf(ivy->out, "%s", buf);
					} else {	/* Skip legal chars */
						while ((s[x] >= '0' && s[x] <= '9')
						       || s[x] == '.'
						       || s[x] == '+'
						       || s[x] == '-') {
							buf[y++] = s[x];
							++x;
						}
						switch (s[x]) {
						case 'd':
						case 'i':
						case 'o':
						case 'u':
						case 'x':
						case 'X':
							{	/* Arg has to be an integer */
								long long i;
								if (getintarg(ivy, a, n++, &i)) {
									goto bye;
								}
								buf[y++] = 'l';
								buf[y++] = 'l';
								buf[y++] = s[x];
								++x;
								buf[y] = 0;
								fprintf(ivy->out, buf, i);
								break;
							}
						case 'c':
							{	/* Arg has to be an integer */
								long long i;
								if (getintarg(ivy, a, n++, &i)) {
									goto bye;
								}
								buf[y++] =
								    s[x];
								++x;
								buf[y] = 0;
								fprintf(ivy->out, buf, (int)i);
								break;
							}

						case 'e':
						case 'E':
						case 'f':
						case 'F':
						case 'g':
						case 'G':
						case 'a':
						case 'A':
							{	/* Arg has to be a double */
								double d;
								if (getdoublearg(ivy, a, n++, &d)) {
									goto bye;
								}
								buf[y++] =
								    s[x];
								++x;
								buf[y] = 0;
								fprintf(ivy->out, buf, d);
								break;
							}

						case 's':
							{	/* Arg has to be a string */
								char *ss;
								if (getstringarg(ivy, a, n++, &ss)) {
									goto bye;
								}
								buf[y++] =
								    s[x];
								++x;
								buf[y] = 0;
								fprintf(ivy->out, buf, ss);
								break;
							}

						default:
							{
								error_0(ivy->errprn, "Illegal format string");
								goto bye;
							}
						}
					}
				} else {
					buf[y] = 0;
					fprintf(ivy->out, "%s", buf);
				}
				s += x;
			}
			if (n != a->nitems) {
				error_0(ivy->errprn, "Incorrect number of args to printf");
			}
		} else
			error_0(ivy->errprn, "First arg to printf must be a string");
	}
      bye:
	mkval(psh(ivy), tVOID);
}

/* Free variables */

void rtclr(Ivy *ivy)
{
	Obj *a = getv(ivy, "argv")->val.u.obj;
	int x;
	for (x = 0; x != a->nitems; ++x) {
		Var *v = getn(a, x);
		if (v->val.var) {
			rmval(&v->val.var->val, __LINE__);
			mkval(&v->val.var->val, tVOID);
		}
	}
	mkval(psh(ivy), tVOID);
}

/* Get input */

void rtget(Ivy *ivy)
{
	char buf[1024];
	if (fgets(buf, sizeof(buf), ivy->in)) {
		int l = strlen(buf);
		if (l) buf[--l]=0;
		*psh(ivy) = mkpval(tSTR, mkstr(strdup(buf), l, ivy->sp + 1, rVAL, __LINE__));
	} else
		mkval(psh(ivy), tVOID);
}

/* Convert string to integer */

void rtatoi(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tSTR) {
		long long num = atoll(a->val.u.str->s);
		*psh(ivy) = mkival(tNUM, num);
	} else
		longjmp(ivy->err, 1);
}

/* Convert integer to string */

void rtitoa(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tNUM) {
		char buf[30];
		sprintf(buf, "%lld", a->val.u.num);
		*psh(ivy) = mkpval(tSTR, mkstr(strdup(buf), strlen(buf), ivy->sp + 1, rVAL, __LINE__));
	} else
		longjmp(ivy->err, 1);
}

/* Get length of string or array */

void rtlen(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tSTR) {
		long long num = a->val.u.str->len;
		*psh(ivy) = mkival(tNUM, num);
	} else if (a->val.type == tOBJ) {
		*psh(ivy) = mkival(tNUM, a->val.u.obj->nitems);
	} else
		longjmp(ivy->err, 1);
}

/* Get current vars object */

void rtvars(Ivy *ivy)
{
#ifdef ONEXT
	Val x = mkpval(tOBJ, ivy->vars->next);
#else
	Val x = mkpval(tOBJ, get_mom(ivy->vars));
#endif
	pr(ivy->out, &x, 0);
	fprintf(ivy->out, "\n");
	mkval(psh(ivy), tVOID);
}

/* Duplicate an object */

void rtdup(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tOBJ)
		*psh(ivy) = mkpval(tOBJ, dupobj(a->val.u.obj, ivy->sp+1, rVAL, __LINE__));
	else
		dupval(psh(ivy), &a->val);
}

/* Include command */

void rtinc(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tSTR) {
		char *s = strdup(a->val.u.str->s);
		FILE *f = fopen(s, "r");
		rmvlvl(ivy);
		if (f)
			/* FIXME: compfile(s, f), */ fclose(f);
		else
			error_1(ivy->errprn, "Couldn\'t open file \'%s\'", s);
		addlvl(ivy,ivy->vars);
		mkval(psh(ivy), tVOID);
		free(s);
	} else
		longjmp(ivy->err, 1);
}

/* Do nothing */

void rtend(Ivy *ivy)
{
	mkval(psh(ivy),tVOID);
}

/* Simple (slow) regular expression parser.  Returns true if 'string'
 * matches 'pattern'.  Each substring which matches a spanning character ('+'
 * and '*') is placed in 'result' (which should be an array of string
 * buffers, each large enough to contain the longest expected substring).
 *
 * 'pattern' may be:
 *    .      matches any character.
 *    *      matches zero or more of the previous character.
 *    +      matches one or more of the previous character.
 *    [...]  matches one character in the list xx.
 *           ranges may be specified with the list, such as 0-9, a-z, etc.
 *    x      other characters match themselves only.
 */

int rmatch(char *string, char *pattern,
	   char **result)
{
	int q;
	char sense;
	char test[256];
	*result = 0;

	for (;;) {
		char *laststring;
		/* Build character match array */
		for (q = 0; q != 256; ++q)
			test[q] = 0;	/* Clear array */
		sense = 1;	/* Sense of match */
		if (!*pattern)	/* End of pattern */
			if (!*string)
				return 1;	/* Must match end of string */
			else
				return 0;
		else if (*pattern == '.') {	/* Match any character */
			++pattern;
			for (q = 0; q != 256; ++q)
				test[q] = 1;
		} else if (*pattern == '[') {	/* Match characters within brackets */
			++pattern;
			if (*pattern == '^')	/* Inverse sense of match */
				++pattern, sense = 0;
			if (*pattern == ']')	/* Match ] */
				++pattern, test[']'] = 1;
			while (*pattern && *pattern != ']') {
				if (*pattern == '-' && pattern[-1] != '[' && pattern[1] && pattern[1] != ']' && pattern[1] >= pattern[-1]) {	/* A Range of characters */
					for (q = pattern[-1];
					     q <= pattern[1]; ++q)
						test[q] = 1;
					++pattern;
				} else
					test[*(unsigned char *)pattern] = 1;
				++pattern;
			}
			if (*pattern)
				++pattern;
		} else		/* Match character itself */
			test[*(unsigned char *)pattern++] = 1;

		/* Check for spanning characters... */
		if (*pattern == '*' || *pattern == '+') {	/* Span... */
			char buf[1024];
			int x = 0;
			int lastx = -1;
			if (*pattern++ == '+') {
				if (test[*(unsigned char *)string] != sense)
					return 0;
				buf[x++] = *string++;
			}
			laststring = 0;
			for (;;) {
				if (rmatch(string, pattern, result + 1)) {	/* A possible match... try to find the longest one */
					lastx = x;
					laststring = string;
				}
				if (!*string || test[*(unsigned char *)string] != sense)
					break;
				buf[x++] = *string++;
			}
			if (lastx >= 0) {
				buf[lastx] = 0;
				*result = strdup(buf);
				return rmatch(laststring, pattern,
					      result + 1);
			} else
				return 0;
		} else {	/* Just one character */
			if (test[*(unsigned char *)string++] != sense)
				return 0;
		}
	}
}

/* Regex matching */
/* string, pattern, out... */
void rtmatch(Ivy *ivy)
{
	char *result[20];
	Obj *a = getv(ivy, "argv")->val.u.obj;
	Var *v;
	char *str;
	char *pat;
	int x;
	if (a->nitems < 2) {
		error_0(ivy->errprn, "Incorrect no. args to match");
		mkval(psh(ivy), tVOID);
		return;
	}
	v = getn(a, 0);
	if (v->val.type != tSTR) {
		error_1(ivy->errprn, "Incorrect arg 1 type for match %d",v->val.type);
		mkval(psh(ivy), tVOID);
		return;
	}
	str = v->val.u.str->s;
	v = getn(a, 1);
	if (v->val.type != tSTR) {
		error_1(ivy->errprn, "Incorrect arg 2 type for match %d",v->val.type);
		mkval(psh(ivy), tVOID);
		return;
	}
	pat = v->val.u.str->s;
	printf("string=%s\n", str);
	printf("pattern=%s\n",pat);
	if (rmatch(str, pat, result)) {
		int n = 2;
		for (x = 0; result[x]; ++x) {
			if (n >= a->nitems)
				free(result[x]);
			else {
				Var *dest = getn(a, n);
				if (!dest->val.var) {
					error_0(ivy->errprn, "Arg to match must be a variable\n");
					free(result[x]);
				} else {
					dest = dest->val.var;
					rmval(&dest->val, __LINE__);
					dest->val = mkpval(tSTR, mkstr(result[x], strlen(result[x]), &dest->val, rVAL, __LINE__));
				}
				++n;
			}
		}
		if (a->nitems - 2 > x) {
			error_0(ivy->errprn, "Incorrect no. args for match");
		}
		*psh(ivy) = mkival(tNUM, 1);
	} else {
		for (x = 0; result[x]; ++x)
			free(result[x]);
		mkval(psh(ivy), tVOID);
	}
}

/* Math */

void rtsin(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tFP)
		*psh(ivy) = mkdval(tFP, sin(a->val.u.fp));
	else if (a->val.type == tNUM)
		*psh(ivy) = mkdval(tFP, sin((double) a->val.u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for sin()");
		mkval(psh(ivy), tVOID);
	}
}

void rtcos(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tFP)
		*psh(ivy) = mkdval(tFP, cos(a->val.u.fp));
	else if (a->val.type == tNUM)
		*psh(ivy) = mkdval(tFP, cos((double) a->val.u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for cos()");
		mkval(psh(ivy), tVOID);
	}
}

void rttan(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tFP)
		*psh(ivy) = mkdval(tFP, tan(a->val.u.fp));
	else if (a->val.type == tNUM)
		*psh(ivy) = mkdval(tFP, tan((double) a->val.u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for tan()");
		mkval(psh(ivy), tVOID);
	}
}

void rtasin(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tFP)
		*psh(ivy) = mkdval(tFP, asin(a->val.u.fp));
	else if (a->val.type == tNUM)
		*psh(ivy) = mkdval(tFP, asin((double) a->val.u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for asin()");
		mkval(psh(ivy), tVOID);
	}
}

void rtacos(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tFP)
		*psh(ivy) = mkdval(tFP, acos(a->val.u.fp));
	else if (a->val.type == tNUM)
		*psh(ivy) = mkdval(tFP, acos((double) a->val.u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for acos()");
		mkval(psh(ivy), tVOID);
	}
}

void rtatan(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tFP)
		*psh(ivy) = mkdval(tFP, atan(a->val.u.fp));
	else if (a->val.type == tNUM)
		*psh(ivy) = mkdval(tFP, atan((double) a->val.u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for atan()");
		mkval(psh(ivy), tVOID);
	}
}

void rtexp(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tFP)
		*psh(ivy) = mkdval(tFP, exp(a->val.u.fp));
	else if (a->val.type == tNUM)
		*psh(ivy) = mkdval(tFP, exp((double) a->val.u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for exp()");
		mkval(psh(ivy), tVOID);
	}
}

void rtlog(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tFP)
		*psh(ivy) = mkdval(tFP, log(a->val.u.fp));
	else if (a->val.type == tNUM)
		*psh(ivy) = mkdval(tFP, log((double) a->val.u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for log()");
		mkval(psh(ivy), tVOID);
	}
}

void rtlog10(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tFP)
		*psh(ivy) = mkdval(tFP, log10(a->val.u.fp));
	else if (a->val.type == tNUM)
		*psh(ivy) = mkdval(tFP, log10((double) a->val.u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for log10()");
		mkval(psh(ivy), tVOID);
	}
}

void rtpow(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	Var *b = getv(ivy, "b");
	double l, r;
	if (a->val.type == tFP)
		l = a->val.u.fp;
	else if (a->val.type == tNUM)
		l = a->val.u.num;
	else {
		error_0(ivy->errprn, "Incorrect type for pow()");
		mkval(psh(ivy), tVOID);
		return;
	}
	if (b->val.type == tFP)
		r = b->val.u.fp;
	else if (b->val.type == tNUM)
		r = b->val.u.num;
	else {
		error_0(ivy->errprn, "Incorrect type for pow()");
		mkval(psh(ivy), tVOID);
		return;
	}
	*psh(ivy) = mkdval(tFP, pow(l, r));
}

void rtsqrt(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tFP)
		*psh(ivy) = mkdval(tFP, sqrt(a->val.u.fp));
	else if (a->val.type == tNUM)
		*psh(ivy) = mkdval(tFP, sqrt((double) a->val.u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for sqrt()");
		mkval(psh(ivy), tVOID);
	}
}

void rtatan2(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	Var *b = getv(ivy, "b");
	double l, r;
	if (a->val.type == tFP)
		l = a->val.u.fp;
	else if (a->val.type == tNUM)
		l = a->val.u.num;
	else {
		error_0(ivy->errprn, "Incorrect type for atan2()");
		mkval(psh(ivy), tVOID);
		return;
	}
	if (b->val.type == tFP)
		r = b->val.u.fp;
	else if (b->val.type == tNUM)
		r = b->val.u.num;
	else {
		error_0(ivy->errprn, "Incorrect type for atan2()");
		mkval(psh(ivy), tVOID);
		return;
	}
	*psh(ivy) = mkdval(tFP, atan2(l, r));
}

void rthypot(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	Var *b = getv(ivy, "b");
	double l, r;
	if (a->val.type == tFP)
		l = a->val.u.fp;
	else if (a->val.type == tNUM)
		l = a->val.u.num;
	else {
		error_0(ivy->errprn, "Incorrect type for hypot()");
		mkval(psh(ivy), tVOID);
		return;
	}
	if (b->val.type == tFP)
		r = b->val.u.fp;
	else if (b->val.type == tNUM)
		r = b->val.u.num;
	else {
		error_0(ivy->errprn, "Incorrect type for hypot()");
		mkval(psh(ivy), tVOID);
		return;
	}
	*psh(ivy) = mkdval(tFP, hypot(l, r));
}

void rtsinh(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tFP)
		*psh(ivy) = mkdval(tFP, sinh(a->val.u.fp));
	else if (a->val.type == tNUM)
		*psh(ivy) = mkdval(tFP, sinh((double) a->val.u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for sinh()");
		mkval(psh(ivy), tVOID);
	}
}

void rtcosh(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tFP)
		*psh(ivy) = mkdval(tFP, cosh(a->val.u.fp));
	else if (a->val.type == tNUM)
		*psh(ivy) = mkdval(tFP, cosh((double) a->val.u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for cosh()");
		mkval(psh(ivy), tVOID);
	}
}

void rttanh(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tFP)
		*psh(ivy) = mkdval(tFP, tanh(a->val.u.fp));
	else if (a->val.type == tNUM)
		*psh(ivy) = mkdval(tFP, tanh((double) a->val.u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for tanh()");
		mkval(psh(ivy), tVOID);
	}
}

void rtasinh(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tFP)
		*psh(ivy) = mkdval(tFP, asinh(a->val.u.fp));
	else if (a->val.type == tNUM)
		*psh(ivy) = mkdval(tFP, asinh((double) a->val.u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for asinh()");
		mkval(psh(ivy), tVOID);
	}
}

void rtacosh(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tFP)
		*psh(ivy) = mkdval(tFP, acosh(a->val.u.fp));
	else if (a->val.type == tNUM)
		*psh(ivy) = mkdval(tFP, acosh((double) a->val.u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for acosh()");
		mkval(psh(ivy), tVOID);
	}
}

void rtatanh(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tFP)
		*psh(ivy) = mkdval(tFP, atanh(a->val.u.fp));
	else if (a->val.type == tNUM)
		*psh(ivy) = mkdval(tFP, atanh((double) a->val.u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for atanh()");
		mkval(psh(ivy), tVOID);
	}
}

void rtfloor(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tFP)
		*psh(ivy) = mkdval(tFP, floor(a->val.u.fp));
	else if (a->val.type == tNUM)
		*psh(ivy) = mkdval(tFP, floor((double) a->val.u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for floor()");
		mkval(psh(ivy), tVOID);
	}
}

void rtceil(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tFP)
		*psh(ivy) = mkdval(tFP, ceil(a->val.u.fp));
	else if (a->val.type == tNUM)
		*psh(ivy) = mkdval(tFP, ceil((double) a->val.u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for ceil()");
		mkval(psh(ivy), tVOID);
	}
}

void rtint(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tFP)
		*psh(ivy) = mkival(tNUM, (long long) (a->val.u.fp));
	else if (a->val.type == tNUM)
		*psh(ivy) = mkival(tNUM, a->val.u.num);
	else {
		error_0(ivy->errprn, "Incorrect type for int()");
		mkval(psh(ivy), tVOID);
	}
}

void rtabs(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tFP)
		*psh(ivy) = mkdval(tFP, fabs(a->val.u.fp));
	else if (a->val.type == tNUM)
		if (a->val.u.num >= 0)
			*psh(ivy) = mkival(tNUM, a->val.u.num);
		else
			*psh(ivy) = mkival(tNUM, -a->val.u.num);
	else {
		error_0(ivy->errprn, "Incorrect type for abs()");
		mkval(psh(ivy), tVOID);
	}
}

void rterf(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tFP)
		*psh(ivy) = mkdval(tFP, erf(a->val.u.fp));
	else if (a->val.type == tNUM)
		*psh(ivy) = mkdval(tFP, erf((double) a->val.u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for erf()");
		mkval(psh(ivy), tVOID);
	}
}

void rterfc(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tFP)
		*psh(ivy) = mkdval(tFP, erfc(a->val.u.fp));
	else if (a->val.type == tNUM)
		*psh(ivy) = mkdval(tFP, erfc((double) a->val.u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for erfc()");
		mkval(psh(ivy), tVOID);
	}
}

void rtj0(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tFP)
		*psh(ivy) = mkdval(tFP, j0(a->val.u.fp));
	else if (a->val.type == tNUM)
		*psh(ivy) = mkdval(tFP, j0((double) a->val.u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for j0()");
		mkval(psh(ivy), tVOID);
	}
}

void rtj1(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tFP)
		*psh(ivy) = mkdval(tFP, j1(a->val.u.fp));
	else if (a->val.type == tNUM)
		*psh(ivy) = mkdval(tFP, j1((double) a->val.u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for j1()");
		mkval(psh(ivy), tVOID);
	}
}

void rty0(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tFP)
		*psh(ivy) = mkdval(tFP, y0(a->val.u.fp));
	else if (a->val.type == tNUM)
		*psh(ivy) = mkdval(tFP, y0((double) a->val.u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for y0()");
		mkval(psh(ivy), tVOID);
	}
}

void rty1(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	if (a->val.type == tFP)
		*psh(ivy) = mkdval(tFP, y1(a->val.u.fp));
	else if (a->val.type == tNUM)
		*psh(ivy) = mkdval(tFP, y1((double) a->val.u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for y1()");
		mkval(psh(ivy), tVOID);
	}
}

void rtjn(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	Var *b = getv(ivy, "b");
	double l, r;
	if (a->val.type == tFP)
		l = a->val.u.fp;
	else if (a->val.type == tNUM)
		l = a->val.u.num;
	else {
		error_0(ivy->errprn, "Incorrect type for jn()");
		mkval(psh(ivy), tVOID);
		return;
	}
	if (b->val.type == tFP)
		r = b->val.u.fp;
	else if (b->val.type == tNUM)
		r = b->val.u.num;
	else {
		error_0(ivy->errprn, "Incorrect type for jn()");
		mkval(psh(ivy), tVOID);
		return;
	}
	*psh(ivy) = mkdval(tFP, jn(l, r));
}

void rtyn(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	Var *b = getv(ivy, "b");
	double l, r;
	if (a->val.type == tFP)
		l = a->val.u.fp;
	else if (a->val.type == tNUM)
		l = a->val.u.num;
	else {
		error_0(ivy->errprn, "Incorrect type for yn()");
		mkval(psh(ivy), tVOID);
		return;
	}
	if (b->val.type == tFP)
		r = b->val.u.fp;
	else if (b->val.type == tNUM)
		r = b->val.u.num;
	else {
		error_0(ivy->errprn, "Incorrect type for yn()");
		mkval(psh(ivy), tVOID);
		return;
	}
	*psh(ivy) = mkdval(tFP, yn(l, r));
}

void rtmax(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	Var *b = getv(ivy, "b");
	double l, r;
	if (a->val.type == tFP)
		l = a->val.u.fp;
	else if (a->val.type == tNUM)
		l = a->val.u.num;
	else {
		error_0(ivy->errprn, "Incorrect type for max()");
		mkval(psh(ivy), tVOID);
		return;
	}
	if (b->val.type == tFP)
		r = b->val.u.fp;
	else if (b->val.type == tNUM)
		r = b->val.u.num;
	else {
		error_0(ivy->errprn, "Incorrect type for max()");
		mkval(psh(ivy), tVOID);
		return;
	}
	if (l >= r)
		*psh(ivy) = mkdval(tFP, l);
	else
		*psh(ivy) = mkdval(tFP, r);
}

void rtmin(Ivy *ivy)
{
	Var *a = getv(ivy, "a");
	Var *b = getv(ivy, "b");
	double l, r;
	if (a->val.type == tFP)
		l = a->val.u.fp;
	else if (a->val.type == tNUM)
		l = a->val.u.num;
	else {
		error_0(ivy->errprn, "Incorrect type for min()");
		mkval(psh(ivy), tVOID);
		return;
	}
	if (b->val.type == tFP)
		r = b->val.u.fp;
	else if (b->val.type == tNUM)
		r = b->val.u.num;
	else {
		error_0(ivy->errprn, "Incorrect type for min()");
		mkval(psh(ivy), tVOID);
		return;
	}
	if (l < r)
		*psh(ivy) = mkdval(tFP, l);
	else
		*psh(ivy) = mkdval(tFP, r);
}

void rtrandom(Ivy *ivy)
{
	*psh(ivy) = mkival(tNUM, random());
}

void rtdepth(Ivy *ivy)
{
	*psh(ivy) = mkival(tNUM, ivy->sp - ivy->sptop);
}

/* Table of built-in functions */

struct builtin builtins[] = {
	{"depth", rtdepth, ""},
	{"pr", rtprint, ""},
	{"vars", rtvars, ""},
	{"print", rtprint, ""},
	{"printf", rtprintf, ""},
	{"dup", rtdup, "a"},
	{"get", rtget, ""},
	{"atoi", rtatoi, "a"},
	{"itoa", rtitoa, "a"},
	{"len", rtlen, "a"},
	{"clear", rtclr, ""},
	{"include", rtinc, "a"},
	{"end", rtend, ""},
	{"match", rtmatch, ""},
	{"sin", rtsin, "a"},
	{"cos", rtcos, "a"},
	{"tan", rttan, "a"},
	{"asin", rtasin, "a"},
	{"acos", rtacos, "a"},
	{"atan", rtatan, "a"},
	{"exp", rtexp, "a"},
	{"log", rtlog, "a"},
	{"log10", rtlog10, "a"},
	{"pow", rtpow, "a;b"},
	{"sqrt", rtsqrt, "a"},
	{"atan2", rtatan2, "a;b"},
	{"hypot", rthypot, "a;b"},
	{"sinh", rtsinh, "a"},
	{"cosh", rtcosh, "a"},
	{"tanh", rttanh, "a"},
	{"asinh", rtasinh, "a"},
	{"acosh", rtacosh, "a"},
	{"atanh", rtatanh, "a"},
	{"floor", rtfloor, "a"},
	{"ceil", rtceil, "a"},
	{"int", rtint, "a"},
	{"abs", rtabs, "a"},
	{"erf", rterf, "a"},
	{"erfc", rterfc, "a"},
	{"j0", rtj0, "a"},
	{"j1", rtj1, "a"},
	{"jn", rtjn, "a;b"},
	{"y0", rty0, "a"},
	{"y1", rty1, "a"},
	{"yn", rtyn, "a;b"},
	{"max", rtmax, "a;b"},
	{"min", rtmin, "a;b"},
	{"random", rtrandom, ""},
	{"help", rthelp, "a"},
	{0, 0, 0}
};
