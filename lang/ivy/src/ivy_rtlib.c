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
#include "ivy_gc.h"

/* Functions for reading args */

int getintarg(Ivy *ivy, Obj * args, int n, long long *rtn)
{
	Val *v;
	if (n >= args->ary_len) {
		error_0(ivy->errprn, "Incorrect number of args");
		return -1;
	}
	v = get_by_number(args, n);
	if (v->type == tNUM) {
		*rtn = v->u.num;
		return 0;
	} else {
		error_0(ivy->errprn, "Incorrect arg type");
		return -1;
	}
}

int getstringarg(Ivy *ivy, Obj * args, int n, char **rtn)
{
	Val *v;
	if (n >= args->ary_len) {
		error_0(ivy->errprn, "Incorrect number of args");
		return -1;
	}
	v = get_by_number(args, n);
	if (v->type == tSTR) {
		*rtn = v->u.str->s;
		return 0;
	} else {
		error_0(ivy->errprn, "Incorrect arg type");
		return -1;
	}
}

int getdoublearg(Ivy *ivy, Obj * args, int n, double *rtn)
{
	Val *v;
	if (n >= args->ary_len) {
		error_0(ivy->errprn, "Incorrect number of args");
		return -1;
	}
	v = get_by_number(args, n);
	if (v->type == tFP) {
		*rtn = v->u.fp;
		return 0;
	} else {
		error_0(ivy->errprn, "Incorrect arg type");
		return -1;
	}
}

/* Write output */

void rtprint(Ivy *ivy)
{
	Obj *a = getv_by_symbol(ivy, argv_symbol)->u.obj;
	int x;
	for (x = 0; x != a->ary_len; ++x) {
		Val *v = get_by_number(a, x);
		switch (v->type) {
			case tSTR: {
				fprintf(ivy->out, "%s", v->u.str->s);
				break;
			} case tNUM: {
				fprintf(ivy->out, "%lld", v->u.num);
				break;
			} case tFP: {
				fprintf(ivy->out, "%g", v->u.fp);
				break;
			} default: {
				pr(ivy, ivy->out, v, 0);
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
	Obj *a = getv_by_symbol(ivy, argv_symbol)->u.obj;
	if (a->ary_len == 0)
		error_0(ivy->errprn, "Incorrect number of args to printf");
	else {
		Val *v = get_by_number(a, 0);
		if (v->type == tSTR) {
			char *s = v->u.str->s;
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
			if (n != a->ary_len) {
				error_0(ivy->errprn, "Incorrect number of args to printf");
			}
		} else
			error_0(ivy->errprn, "First arg to printf must be a string");
	}
      bye:
	mkval(psh(ivy), tVOID);
}

/* Get pointer to origin of a value.  Returns NULL if there is none. */

Val *get_origin(Val *v)
{
	Val *l = 0;
	if (!v)
		return NULL;
	if (v->origin) {
		switch (v->idx_type) {
			case tNUM: {
				l = get_by_number(v->origin, v->idx.num);
				break;
			} case tNAM: {
				l = get_by_symbol(v->origin, v->idx.name);
				break;
			} case tSTR: {
				l = get_by_string(v->origin, v->idx.str->s);
				break;
			} default: {
				fprintf(stderr, "Invalid origin index type??\n");
				break;
			}
		}
	}
	return l;
}

/* Get pointer to origin of a value.  Create if it doesn't exist.
   Return NULL if there is no origin. */

Val *set_origin(Val *v)
{
	Val *l = 0;
	if (!v)
		return NULL;
	if (v->origin) {
		switch (v->idx_type) {
			case tNUM: {
				l = set_by_number(v->origin, v->idx.num);
				break;
			} case tNAM: {
				l = set_by_symbol(v->origin, v->idx.name);
				break;
			} case tSTR: {
				l = set_by_string(v->origin, v->idx.str->s);
				break;
			} default: {
				fprintf(stderr, "Invalid origin index??\n");
			}
		}
	}
	return l;
}

/* Free variables */

void rtclr(Ivy *ivy)
{
	Obj *a = getv_by_symbol(ivy, argv_symbol)->u.obj;
	int x;
	for (x = 0; x != a->ary_len; ++x) {
		Val *v = get_by_number(a, x);
		v = get_origin(v);
		if (v)
			mkval(v, tVOID);
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
		*psh(ivy) = mkpval(tSTR, alloc_str(strdup(buf), l));
	} else
		mkval(psh(ivy), tVOID);
}

/* Convert string to integer */

void rtatoi(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tSTR) {
		long long num = atoll(a->u.str->s);
		*psh(ivy) = mkival(tNUM, num);
	} else
		longjmp(ivy->err, 1);
}

/* Convert integer to string */

void rtitoa(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tNUM) {
		char buf[30];
		sprintf(buf, "%lld", a->u.num);
		*psh(ivy) = mkpval(tSTR, alloc_str(strdup(buf), strlen(buf)));
	} else
		longjmp(ivy->err, 1);
}

/* Get length of string or array */

void rtlen(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tSTR) {
		long long num = a->u.str->len;
		*psh(ivy) = mkival(tNUM, num);
	} else if (a->type == tOBJ) {
		*psh(ivy) = mkival(tNUM, a->u.obj->ary_len);
	} else
		longjmp(ivy->err, 1);
}

/* Get current vars object */

void rtvars(Ivy *ivy)
{
	Val x = mkpval(tOBJ, get_mom(ivy->vars));
	pr(ivy, ivy->out, &x, 0);
	fprintf(ivy->out, "\n");
	mkval(psh(ivy), tVOID);
}

/* Duplicate an object */

void rtdup(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tOBJ)
		*psh(ivy) = mkpval(tOBJ, dupobj(a->u.obj, ivy->sp+1, 0, __LINE__));
	else
		dupval(psh(ivy), a);
}

/* Include command */

void rtinc(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tSTR) {
		char *s = strdup(a->u.str->s);
		FILE *f = fopen(s, "r");
		rmvlvl(ivy);
		if (f)
			/* FIXME: compfile(s, f), */ fclose(f);
		else
			error_1(ivy->errprn, "Couldn\'t open file \'%s\'", s);
		addlvl(ivy, ivy->vars);
		mkval(psh(ivy), tVOID);
		free(s);
	} else
		longjmp(ivy->err, 1);
}

/* Do nothing */

void rtend(Ivy *ivy)
{
	mkval(psh(ivy), tVOID);
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
	Obj *a = getv_by_symbol(ivy, argv_symbol)->u.obj;
	Val *v;
	char *str;
	char *pat;
	int x;
	if (a->ary_len < 2) {
		error_0(ivy->errprn, "Incorrect no. args to match");
		mkval(psh(ivy), tVOID);
		return;
	}
	v = get_by_number(a, 0);
	if (v->type != tSTR) {
		error_1(ivy->errprn, "Incorrect arg 1 type for match %d",v->type);
		mkval(psh(ivy), tVOID);
		return;
	}
	str = v->u.str->s;
	v = get_by_number(a, 1);
	if (v->type != tSTR) {
		error_1(ivy->errprn, "Incorrect arg 2 type for match %d",v->type);
		mkval(psh(ivy), tVOID);
		return;
	}
	pat = v->u.str->s;
	printf("string=%s\n", str);
	printf("pattern=%s\n",pat);
	if (rmatch(str, pat, result)) {
		int n = 2;
		for (x = 0; result[x]; ++x) {
			if (n >= a->ary_len)
				free(result[x]);
			else {
				Val *dest = set_origin(get_by_number(a, n));
				if (!dest) {
					error_0(ivy->errprn, "Arg to match must be a variable\n");
					free(result[x]);
				} else {
					*dest = mkpval(tSTR, alloc_str(result[x], strlen(result[x])));
				}
				++n;
			}
		}
		if (a->ary_len - 2 > x) {
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
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tFP)
		*psh(ivy) = mkdval(tFP, sin(a->u.fp));
	else if (a->type == tNUM)
		*psh(ivy) = mkdval(tFP, sin((double) a->u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for sin()");
		mkval(psh(ivy), tVOID);
	}
}

void rtcos(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tFP)
		*psh(ivy) = mkdval(tFP, cos(a->u.fp));
	else if (a->type == tNUM)
		*psh(ivy) = mkdval(tFP, cos((double) a->u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for cos()");
		mkval(psh(ivy), tVOID);
	}
}

void rttan(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tFP)
		*psh(ivy) = mkdval(tFP, tan(a->u.fp));
	else if (a->type == tNUM)
		*psh(ivy) = mkdval(tFP, tan((double) a->u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for tan()");
		mkval(psh(ivy), tVOID);
	}
}

void rtasin(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tFP)
		*psh(ivy) = mkdval(tFP, asin(a->u.fp));
	else if (a->type == tNUM)
		*psh(ivy) = mkdval(tFP, asin((double) a->u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for asin()");
		mkval(psh(ivy), tVOID);
	}
}

void rtacos(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tFP)
		*psh(ivy) = mkdval(tFP, acos(a->u.fp));
	else if (a->type == tNUM)
		*psh(ivy) = mkdval(tFP, acos((double) a->u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for acos()");
		mkval(psh(ivy), tVOID);
	}
}

void rtatan(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tFP)
		*psh(ivy) = mkdval(tFP, atan(a->u.fp));
	else if (a->type == tNUM)
		*psh(ivy) = mkdval(tFP, atan((double) a->u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for atan()");
		mkval(psh(ivy), tVOID);
	}
}

void rtexp(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tFP)
		*psh(ivy) = mkdval(tFP, exp(a->u.fp));
	else if (a->type == tNUM)
		*psh(ivy) = mkdval(tFP, exp((double) a->u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for exp()");
		mkval(psh(ivy), tVOID);
	}
}

void rtlog(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tFP)
		*psh(ivy) = mkdval(tFP, log(a->u.fp));
	else if (a->type == tNUM)
		*psh(ivy) = mkdval(tFP, log((double) a->u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for log()");
		mkval(psh(ivy), tVOID);
	}
}

void rtlog10(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tFP)
		*psh(ivy) = mkdval(tFP, log10(a->u.fp));
	else if (a->type == tNUM)
		*psh(ivy) = mkdval(tFP, log10((double) a->u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for log10()");
		mkval(psh(ivy), tVOID);
	}
}

void rtpow(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	Val *b = getv_by_symbol(ivy, b_symbol);
	double l, r;
	if (a->type == tFP)
		l = a->u.fp;
	else if (a->type == tNUM)
		l = a->u.num;
	else {
		error_0(ivy->errprn, "Incorrect type for pow()");
		mkval(psh(ivy), tVOID);
		return;
	}
	if (b->type == tFP)
		r = b->u.fp;
	else if (b->type == tNUM)
		r = b->u.num;
	else {
		error_0(ivy->errprn, "Incorrect type for pow()");
		mkval(psh(ivy), tVOID);
		return;
	}
	*psh(ivy) = mkdval(tFP, pow(l, r));
}

void rtsqrt(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tFP)
		*psh(ivy) = mkdval(tFP, sqrt(a->u.fp));
	else if (a->type == tNUM)
		*psh(ivy) = mkdval(tFP, sqrt((double) a->u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for sqrt()");
		mkval(psh(ivy), tVOID);
	}
}

void rtatan2(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	Val *b = getv_by_symbol(ivy, b_symbol);
	double l, r;
	if (a->type == tFP)
		l = a->u.fp;
	else if (a->type == tNUM)
		l = a->u.num;
	else {
		error_0(ivy->errprn, "Incorrect type for atan2()");
		mkval(psh(ivy), tVOID);
		return;
	}
	if (b->type == tFP)
		r = b->u.fp;
	else if (b->type == tNUM)
		r = b->u.num;
	else {
		error_0(ivy->errprn, "Incorrect type for atan2()");
		mkval(psh(ivy), tVOID);
		return;
	}
	*psh(ivy) = mkdval(tFP, atan2(l, r));
}

void rthypot(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	Val *b = getv_by_symbol(ivy, b_symbol);
	double l, r;
	if (a->type == tFP)
		l = a->u.fp;
	else if (a->type == tNUM)
		l = a->u.num;
	else {
		error_0(ivy->errprn, "Incorrect type for hypot()");
		mkval(psh(ivy), tVOID);
		return;
	}
	if (b->type == tFP)
		r = b->u.fp;
	else if (b->type == tNUM)
		r = b->u.num;
	else {
		error_0(ivy->errprn, "Incorrect type for hypot()");
		mkval(psh(ivy), tVOID);
		return;
	}
	*psh(ivy) = mkdval(tFP, hypot(l, r));
}

void rtsinh(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tFP)
		*psh(ivy) = mkdval(tFP, sinh(a->u.fp));
	else if (a->type == tNUM)
		*psh(ivy) = mkdval(tFP, sinh((double) a->u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for sinh()");
		mkval(psh(ivy), tVOID);
	}
}

void rtcosh(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tFP)
		*psh(ivy) = mkdval(tFP, cosh(a->u.fp));
	else if (a->type == tNUM)
		*psh(ivy) = mkdval(tFP, cosh((double) a->u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for cosh()");
		mkval(psh(ivy), tVOID);
	}
}

void rttanh(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tFP)
		*psh(ivy) = mkdval(tFP, tanh(a->u.fp));
	else if (a->type == tNUM)
		*psh(ivy) = mkdval(tFP, tanh((double) a->u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for tanh()");
		mkval(psh(ivy), tVOID);
	}
}

void rtasinh(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tFP)
		*psh(ivy) = mkdval(tFP, asinh(a->u.fp));
	else if (a->type == tNUM)
		*psh(ivy) = mkdval(tFP, asinh((double) a->u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for asinh()");
		mkval(psh(ivy), tVOID);
	}
}

void rtacosh(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tFP)
		*psh(ivy) = mkdval(tFP, acosh(a->u.fp));
	else if (a->type == tNUM)
		*psh(ivy) = mkdval(tFP, acosh((double) a->u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for acosh()");
		mkval(psh(ivy), tVOID);
	}
}

void rtatanh(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tFP)
		*psh(ivy) = mkdval(tFP, atanh(a->u.fp));
	else if (a->type == tNUM)
		*psh(ivy) = mkdval(tFP, atanh((double) a->u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for atanh()");
		mkval(psh(ivy), tVOID);
	}
}

void rtfloor(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tFP)
		*psh(ivy) = mkdval(tFP, floor(a->u.fp));
	else if (a->type == tNUM)
		*psh(ivy) = mkdval(tFP, floor((double) a->u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for floor()");
		mkval(psh(ivy), tVOID);
	}
}

void rtceil(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tFP)
		*psh(ivy) = mkdval(tFP, ceil(a->u.fp));
	else if (a->type == tNUM)
		*psh(ivy) = mkdval(tFP, ceil((double) a->u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for ceil()");
		mkval(psh(ivy), tVOID);
	}
}

void rtint(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tFP)
		*psh(ivy) = mkival(tNUM, (long long) (a->u.fp));
	else if (a->type == tNUM)
		*psh(ivy) = mkival(tNUM, a->u.num);
	else {
		error_0(ivy->errprn, "Incorrect type for int()");
		mkval(psh(ivy), tVOID);
	}
}

extern int symbol_count;

void rtsymbolcount(Ivy *ivy)
{
	printf("Symbol count = %d\n", symbol_count);
	mkval(psh(ivy), tVOID);
}

void rtabs(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tFP)
		*psh(ivy) = mkdval(tFP, fabs(a->u.fp));
	else if (a->type == tNUM)
		if (a->u.num >= 0)
			*psh(ivy) = mkival(tNUM, a->u.num);
		else
			*psh(ivy) = mkival(tNUM, -a->u.num);
	else {
		error_0(ivy->errprn, "Incorrect type for abs()");
		mkval(psh(ivy), tVOID);
	}
}

void rterf(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tFP)
		*psh(ivy) = mkdval(tFP, erf(a->u.fp));
	else if (a->type == tNUM)
		*psh(ivy) = mkdval(tFP, erf((double) a->u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for erf()");
		mkval(psh(ivy), tVOID);
	}
}

void rterfc(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tFP)
		*psh(ivy) = mkdval(tFP, erfc(a->u.fp));
	else if (a->type == tNUM)
		*psh(ivy) = mkdval(tFP, erfc((double) a->u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for erfc()");
		mkval(psh(ivy), tVOID);
	}
}

void rtj0(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tFP)
		*psh(ivy) = mkdval(tFP, j0(a->u.fp));
	else if (a->type == tNUM)
		*psh(ivy) = mkdval(tFP, j0((double) a->u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for j0()");
		mkval(psh(ivy), tVOID);
	}
}

void rtj1(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tFP)
		*psh(ivy) = mkdval(tFP, j1(a->u.fp));
	else if (a->type == tNUM)
		*psh(ivy) = mkdval(tFP, j1((double) a->u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for j1()");
		mkval(psh(ivy), tVOID);
	}
}

void rty0(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tFP)
		*psh(ivy) = mkdval(tFP, y0(a->u.fp));
	else if (a->type == tNUM)
		*psh(ivy) = mkdval(tFP, y0((double) a->u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for y0()");
		mkval(psh(ivy), tVOID);
	}
}

void rty1(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	if (a->type == tFP)
		*psh(ivy) = mkdval(tFP, y1(a->u.fp));
	else if (a->type == tNUM)
		*psh(ivy) = mkdval(tFP, y1((double) a->u.num));
	else {
		error_0(ivy->errprn, "Incorrect type for y1()");
		mkval(psh(ivy), tVOID);
	}
}

void rtjn(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	Val *b = getv_by_symbol(ivy, b_symbol);
	double l, r;
	if (a->type == tFP)
		l = a->u.fp;
	else if (a->type == tNUM)
		l = a->u.num;
	else {
		error_0(ivy->errprn, "Incorrect type for jn()");
		mkval(psh(ivy), tVOID);
		return;
	}
	if (b->type == tFP)
		r = b->u.fp;
	else if (b->type == tNUM)
		r = b->u.num;
	else {
		error_0(ivy->errprn, "Incorrect type for jn()");
		mkval(psh(ivy), tVOID);
		return;
	}
	*psh(ivy) = mkdval(tFP, jn(l, r));
}

void rtyn(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	Val *b = getv_by_symbol(ivy, b_symbol);
	double l, r;
	if (a->type == tFP)
		l = a->u.fp;
	else if (a->type == tNUM)
		l = a->u.num;
	else {
		error_0(ivy->errprn, "Incorrect type for yn()");
		mkval(psh(ivy), tVOID);
		return;
	}
	if (b->type == tFP)
		r = b->u.fp;
	else if (b->type == tNUM)
		r = b->u.num;
	else {
		error_0(ivy->errprn, "Incorrect type for yn()");
		mkval(psh(ivy), tVOID);
		return;
	}
	*psh(ivy) = mkdval(tFP, yn(l, r));
}

void rtmax(Ivy *ivy)
{
	Val *a = getv_by_symbol(ivy, a_symbol);
	Val *b = getv_by_symbol(ivy, b_symbol);
	double l, r;
	if (a->type == tFP)
		l = a->u.fp;
	else if (a->type == tNUM)
		l = a->u.num;
	else {
		error_0(ivy->errprn, "Incorrect type for max()");
		mkval(psh(ivy), tVOID);
		return;
	}
	if (b->type == tFP)
		r = b->u.fp;
	else if (b->type == tNUM)
		r = b->u.num;
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
	Val *a = getv_by_symbol(ivy, a_symbol);
	Val *b = getv_by_symbol(ivy, b_symbol);
	double l, r;
	if (a->type == tFP)
		l = a->u.fp;
	else if (a->type == tNUM)
		l = a->u.num;
	else {
		error_0(ivy->errprn, "Incorrect type for min()");
		mkval(psh(ivy), tVOID);
		return;
	}
	if (b->type == tFP)
		r = b->u.fp;
	else if (b->type == tNUM)
		r = b->u.num;
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
	{"symbolcount", rtsymbolcount, ""},
	{0, 0, 0}
};
