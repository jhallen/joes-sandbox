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
#include "ivy_error.h"
#include "ivy_tree.h"
#include "ivy.h"

/* Functions for reading args */

int ivy_getintarg(Ivy *ivy, Ivy_obj * args, int n, long long *rtn)
{
	Ivy_val *v;
	if (n >= args->ary_len) {
		ivy_error_0(ivy->errprn, "Incorrect number of args");
		return -1;
	}
	v = ivy_get_by_number(args, n);
	if (v->type == ivy_tNUM) {
		*rtn = v->u.num;
		return 0;
	} else {
		ivy_error_0(ivy->errprn, "Incorrect arg type");
		return -1;
	}
}

int ivy_getstringarg(Ivy *ivy, Ivy_obj * args, int n, char **rtn)
{
	Ivy_val *v;
	if (n >= args->ary_len) {
		ivy_error_0(ivy->errprn, "Incorrect number of args");
		return -1;
	}
	v = ivy_get_by_number(args, n);
	if (v->type == ivy_tSTR) {
		*rtn = v->u.str->s;
		return 0;
	} else {
		ivy_error_0(ivy->errprn, "Incorrect arg type");
		return -1;
	}
}

int ivy_getdoublearg(Ivy *ivy, Ivy_obj * args, int n, double *rtn)
{
	Ivy_val *v;
	if (n >= args->ary_len) {
		ivy_error_0(ivy->errprn, "Incorrect number of args");
		return -1;
	}
	v = ivy_get_by_number(args, n);
	if (v->type == ivy_tFP) {
		*rtn = v->u.fp;
		return 0;
	} else {
		ivy_error_0(ivy->errprn, "Incorrect arg type");
		return -1;
	}
}

/* Write output */

static void rtprint(Ivy *ivy)
{
	Ivy_obj *a = ivy_getv_by_symbol(ivy, ivy_argv_symbol)->u.obj;
	int x;
	for (x = 0; x != a->ary_len; ++x) {
		Ivy_val *v = ivy_get_by_number(a, x);
		switch (v->type) {
			case ivy_tSTR: {
				fprintf(ivy->out, "%s", v->u.str->s);
				break;
			} case ivy_tNUM: {
				fprintf(ivy->out, "%lld", v->u.num);
				break;
			} case ivy_tFP: {
				fprintf(ivy->out, "%g", v->u.fp);
				break;
			} default: {
				ivy_pr(ivy, ivy->out, v, 0);
				break;
			}
		}
	}
	fprintf(ivy->out, "\n");
	ivy_push_void(ivy);
}

/* Printf the output */

static void rtprintf(Ivy *ivy)
{
	Ivy_obj *a = ivy_getv_by_symbol(ivy, ivy_argv_symbol)->u.obj;
	if (a->ary_len == 0)
		ivy_error_0(ivy->errprn, "Incorrect number of args to printf");
	else {
		Ivy_val *v = ivy_get_by_number(a, 0);
		if (v->type == ivy_tSTR) {
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
								if (ivy_getintarg(ivy, a, n++, &i)) {
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
								if (ivy_getintarg(ivy, a, n++, &i)) {
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
								if (ivy_getdoublearg(ivy, a, n++, &d)) {
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
								if (ivy_getstringarg(ivy, a, n++, &ss)) {
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
								ivy_error_0(ivy->errprn, "Illegal format string");
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
				ivy_error_0(ivy->errprn, "Incorrect number of args to printf");
			}
		} else
			ivy_error_0(ivy->errprn, "First arg to printf must be a string");
	}
      bye:
	ivy_push_void(ivy);
}

/* Get pointer to origin of a value.  Returns NULL if there is none. */

Ivy_val *ivy_get_origin(Ivy_val *v)
{
	Ivy_val *l = 0;
	if (!v)
		return NULL;
	if (v->origin) {
		switch (v->idx_type) {
			case ivy_tNUM: {
				l = ivy_get_by_number(v->origin, v->idx.num);
				break;
			} case ivy_tNAM: {
				l = ivy_get_by_symbol(v->origin, v->idx.name);
				break;
			} case ivy_tSTR: {
				l = ivy_get_by_string(v->origin, v->idx.str->s);
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

Ivy_val *ivy_set_origin(Ivy_val *v)
{
	Ivy_val *l = 0;
	if (!v)
		return NULL;
	if (v->origin) {
		switch (v->idx_type) {
			case ivy_tNUM: {
				l = ivy_set_by_number(v->origin, v->idx.num);
				break;
			} case ivy_tNAM: {
				l = ivy_set_by_symbol(v->origin, v->idx.name);
				break;
			} case ivy_tSTR: {
				l = ivy_set_by_string(v->origin, v->idx.str->s);
				break;
			} default: {
				fprintf(stderr, "Invalid origin index??\n");
			}
		}
	}
	return l;
}

/* Free variables */

static void rtclr(Ivy *ivy)
{
	Ivy_obj *a = ivy_getv_by_symbol(ivy, ivy_argv_symbol)->u.obj;
	int x;
	for (x = 0; x != a->ary_len; ++x) {
		Ivy_val *v = ivy_get_by_number(a, x);
		v = ivy_get_origin(v);
		if (v)
			ivy_void_val(v);
	}
	ivy_push_void(ivy);
}

/* Get input */

static void rtget(Ivy *ivy)
{
	char buf[1024];
	if (fgets(buf, sizeof(buf), ivy->in)) {
		int l = strlen(buf);
		if (l) buf[--l]=0;
		ivy_push_string(ivy, ivy_alloc_str(strdup(buf), l));
	} else
		ivy_push_void(ivy);
}

/* Convert string to integer */

static void rtatoi(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tSTR) {
		long long num = atoll(a->u.str->s);
		ivy_push_int(ivy, num);
	} else
		longjmp(ivy->err, 1);
}

/* Convert integer to string */

static void rtitoa(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tNUM) {
		char buf[30];
		sprintf(buf, "%lld", a->u.num);
		ivy_push_string(ivy, ivy_alloc_str(strdup(buf), strlen(buf)));
	} else
		longjmp(ivy->err, 1);
}

/* Get length of string or array */

static void rtlen(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tSTR) {
		long long num = a->u.str->len;
		ivy_push_int(ivy, num);
	} else if (a->type == ivy_tOBJ) {
		ivy_push_int(ivy, a->u.obj->ary_len);
	} else
		longjmp(ivy->err, 1);
}

/* Get current vars object */

static void rtvars(Ivy *ivy)
{
	Ivy_val x;
	ivy_obj_val(&x, ivy_get_mom(ivy->vars));
	ivy_pr(ivy, ivy->out, &x, 0);
	fprintf(ivy->out, "\n");
	ivy_push_void(ivy);
}

/* Duplicate an object */

static void rtdup(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tOBJ)
		ivy_push_obj(ivy, ivy_dup_obj(a->u.obj, ivy->sp+1, 0, __LINE__));
	else
		ivy_dup_val(ivy_push(ivy), a);
}

/* Load Ivy source code, run it and return its final value */

static void rtloadfile(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tSTR) {
		char *s = strdup(a->u.str->s);
		FILE *f = fopen(s, "r");
		if (f) {
			Ivy tmp[1];
			char buf[1024];
			Ivy_val rtn_val;
			Ivy_parser *parser;
			ivy_void_val(&rtn_val);
			ivy_setup(tmp, ivy->errprn->error_print, NULL, ivy->in, ivy->out);
			parser = ivy_create_parser(tmp, s);
			while (fgets(buf, sizeof(buf) - 1, f)) {
				Ivy_val rtn = ivy_parse(tmp, parser, buf, 0, 0, 0, 0, 0);
				if (rtn.type != ivy_tERROR)
					rtn_val = rtn;
			}
			ivy_parse_done(tmp, parser, 0, 0, 0, 0, 0);
			ivy_free_parser(parser);
			ivy_shutdown(tmp);
			fclose(f);
			*ivy_push(ivy) = rtn_val;
		} else {
			ivy_error_1(ivy->errprn, "Couldn\'t open file \'%s\'", s);
			ivy_push_void(ivy);
		}
		free(s);
	} else
		longjmp(ivy->err, 1);
}

/* Do nothing */

static void rtend(Ivy *ivy)
{
	ivy_push_void(ivy);
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

static int rmatch(char *string, char *pattern,
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
static void rtmatch(Ivy *ivy)
{
	char *result[20];
	Ivy_obj *a = ivy_getv_by_symbol(ivy, ivy_argv_symbol)->u.obj;
	Ivy_val *v;
	char *str;
	char *pat;
	int x;
	if (a->ary_len < 2) {
		ivy_error_0(ivy->errprn, "Incorrect no. args to match");
		ivy_push_void(ivy);
		return;
	}
	v = ivy_get_by_number(a, 0);
	if (v->type != ivy_tSTR) {
		ivy_error_1(ivy->errprn, "Incorrect arg 1 type for match %d",v->type);
		ivy_push_void(ivy);
		return;
	}
	str = v->u.str->s;
	v = ivy_get_by_number(a, 1);
	if (v->type != ivy_tSTR) {
		ivy_error_1(ivy->errprn, "Incorrect arg 2 type for match %d",v->type);
		ivy_push_void(ivy);
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
				Ivy_val *dest = ivy_set_origin(ivy_get_by_number(a, n));
				if (!dest) {
					ivy_error_0(ivy->errprn, "Arg to match must be a variable\n");
					free(result[x]);
				} else {
					ivy_string_val(dest, ivy_alloc_str(result[x], strlen(result[x])));
				}
				++n;
			}
		}
		if (a->ary_len - 2 > x) {
			ivy_error_0(ivy->errprn, "Incorrect no. args for match");
		}
		ivy_push_int(ivy, 1);
	} else {
		for (x = 0; result[x]; ++x)
			free(result[x]);
		ivy_push_void(ivy);
	}
}

/* Math */

static void rtsin(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tFP)
		ivy_push_double(ivy, sin(a->u.fp));
	else if (a->type == ivy_tNUM)
		ivy_push_double(ivy, sin((double) a->u.num));
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for sin()");
		ivy_push_void(ivy);
	}
}

static void rtcos(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tFP)
		ivy_push_double(ivy, cos(a->u.fp));
	else if (a->type == ivy_tNUM)
		ivy_push_double(ivy, cos((double) a->u.num));
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for cos()");
		ivy_push_void(ivy);
	}
}

static void rttan(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tFP)
		ivy_push_double(ivy, tan(a->u.fp));
	else if (a->type == ivy_tNUM)
		ivy_push_double(ivy, tan((double) a->u.num));
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for tan()");
		ivy_push_void(ivy);
	}
}

static void rtasin(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tFP)
		ivy_push_double(ivy, asin(a->u.fp));
	else if (a->type == ivy_tNUM)
		ivy_push_double(ivy, asin((double) a->u.num));
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for asin()");
		ivy_push_void(ivy);
	}
}

static void rtacos(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tFP)
		ivy_push_double(ivy, acos(a->u.fp));
	else if (a->type == ivy_tNUM)
		ivy_push_double(ivy, acos((double) a->u.num));
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for acos()");
		ivy_push_void(ivy);
	}
}

static void rtatan(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tFP)
		ivy_push_double(ivy, atan(a->u.fp));
	else if (a->type == ivy_tNUM)
		ivy_push_double(ivy, atan((double) a->u.num));
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for atan()");
		ivy_push_void(ivy);
	}
}

static void rtexp(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tFP)
		ivy_push_double(ivy, exp(a->u.fp));
	else if (a->type == ivy_tNUM)
		ivy_push_double(ivy, exp((double) a->u.num));
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for exp()");
		ivy_push_void(ivy);
	}
}

static void rtlog(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tFP)
		ivy_push_double(ivy, log(a->u.fp));
	else if (a->type == ivy_tNUM)
		ivy_push_double(ivy, log((double) a->u.num));
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for log()");
		ivy_push_void(ivy);
	}
}

static void rtlog10(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tFP)
		ivy_push_double(ivy, log10(a->u.fp));
	else if (a->type == ivy_tNUM)
		ivy_push_double(ivy, log10((double) a->u.num));
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for log10()");
		ivy_push_void(ivy);
	}
}

static void rtpow(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	Ivy_val *b = ivy_getv_by_symbol(ivy, ivy_b_symbol);
	double l, r;
	if (a->type == ivy_tFP)
		l = a->u.fp;
	else if (a->type == ivy_tNUM)
		l = a->u.num;
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for pow()");
		ivy_push_void(ivy);
		return;
	}
	if (b->type == ivy_tFP)
		r = b->u.fp;
	else if (b->type == ivy_tNUM)
		r = b->u.num;
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for pow()");
		ivy_push_void(ivy);
		return;
	}
	ivy_push_double(ivy, pow(l, r));
}

static void rtsqrt(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tFP)
		ivy_push_double(ivy, sqrt(a->u.fp));
	else if (a->type == ivy_tNUM)
		ivy_push_double(ivy, sqrt((double) a->u.num));
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for sqrt()");
		ivy_push_void(ivy);
	}
}

static void rtatan2(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	Ivy_val *b = ivy_getv_by_symbol(ivy, ivy_b_symbol);
	double l, r;
	if (a->type == ivy_tFP)
		l = a->u.fp;
	else if (a->type == ivy_tNUM)
		l = a->u.num;
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for atan2()");
		ivy_push_void(ivy);
		return;
	}
	if (b->type == ivy_tFP)
		r = b->u.fp;
	else if (b->type == ivy_tNUM)
		r = b->u.num;
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for atan2()");
		ivy_push_void(ivy);
		return;
	}
	ivy_push_double(ivy, atan2(l, r));
}

static void rthypot(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	Ivy_val *b = ivy_getv_by_symbol(ivy, ivy_b_symbol);
	double l, r;
	if (a->type == ivy_tFP)
		l = a->u.fp;
	else if (a->type == ivy_tNUM)
		l = a->u.num;
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for hypot()");
		ivy_push_void(ivy);
		return;
	}
	if (b->type == ivy_tFP)
		r = b->u.fp;
	else if (b->type == ivy_tNUM)
		r = b->u.num;
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for hypot()");
		ivy_push_void(ivy);
		return;
	}
	ivy_push_double(ivy, hypot(l, r));
}

static void rtsinh(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tFP)
		ivy_push_double(ivy, sinh(a->u.fp));
	else if (a->type == ivy_tNUM)
		ivy_push_double(ivy, sinh((double) a->u.num));
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for sinh()");
		ivy_push_void(ivy);
	}
}

static void rtcosh(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tFP)
		ivy_push_double(ivy, cosh(a->u.fp));
	else if (a->type == ivy_tNUM)
		ivy_push_double(ivy, cosh((double) a->u.num));
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for cosh()");
		ivy_push_void(ivy);
	}
}

static void rttanh(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tFP)
		ivy_push_double(ivy, tanh(a->u.fp));
	else if (a->type == ivy_tNUM)
		ivy_push_double(ivy, tanh((double) a->u.num));
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for tanh()");
		ivy_push_void(ivy);
	}
}

static void rtasinh(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tFP)
		ivy_push_double(ivy, asinh(a->u.fp));
	else if (a->type == ivy_tNUM)
		ivy_push_double(ivy, asinh((double) a->u.num));
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for asinh()");
		ivy_push_void(ivy);
	}
}

static void rtacosh(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tFP)
		ivy_push_double(ivy, acosh(a->u.fp));
	else if (a->type == ivy_tNUM)
		ivy_push_double(ivy, acosh((double) a->u.num));
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for acosh()");
		ivy_push_void(ivy);
	}
}

static void rtatanh(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tFP)
		ivy_push_double(ivy, atanh(a->u.fp));
	else if (a->type == ivy_tNUM)
		ivy_push_double(ivy, atanh((double) a->u.num));
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for atanh()");
		ivy_push_void(ivy);
	}
}

static void rtfloor(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tFP)
		ivy_push_double(ivy, floor(a->u.fp));
	else if (a->type == ivy_tNUM)
		ivy_push_double(ivy, floor((double) a->u.num));
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for floor()");
		ivy_push_void(ivy);
	}
}

static void rtceil(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tFP)
		ivy_push_double(ivy, ceil(a->u.fp));
	else if (a->type == ivy_tNUM)
		ivy_push_double(ivy, ceil((double) a->u.num));
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for ceil()");
		ivy_push_void(ivy);
	}
}

static void rtint(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tFP)
		ivy_push_int(ivy, (long long) (a->u.fp));
	else if (a->type == ivy_tNUM)
		ivy_push_int(ivy, a->u.num);
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for int()");
		ivy_push_void(ivy);
	}
}

extern int ivy_symbol_count;

static void rtsymbolcount(Ivy *ivy)
{
	printf("Symbol count = %d\n", ivy_symbol_count);
		ivy_push_void(ivy);
}

static void rtabs(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tFP)
		ivy_push_double(ivy, fabs(a->u.fp));
	else if (a->type == ivy_tNUM)
		if (a->u.num >= 0)
			ivy_push_int(ivy, a->u.num);
		else
			ivy_push_int(ivy, -a->u.num);
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for abs()");
		ivy_push_void(ivy);
	}
}

static void rterf(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tFP)
		ivy_push_double(ivy, erf(a->u.fp));
	else if (a->type == ivy_tNUM)
		ivy_push_double(ivy, erf((double) a->u.num));
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for erf()");
		ivy_push_void(ivy);
	}
}

static void rterfc(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tFP)
		ivy_push_double(ivy, erfc(a->u.fp));
	else if (a->type == ivy_tNUM)
		ivy_push_double(ivy, erfc((double) a->u.num));
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for erfc()");
		ivy_push_void(ivy);
	}
}

static void rtj0(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tFP)
		ivy_push_double(ivy, j0(a->u.fp));
	else if (a->type == ivy_tNUM)
		ivy_push_double(ivy, j0((double) a->u.num));
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for j0()");
		ivy_push_void(ivy);
	}
}

static void rtj1(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tFP)
		ivy_push_double(ivy, j1(a->u.fp));
	else if (a->type == ivy_tNUM)
		ivy_push_double(ivy, j1((double) a->u.num));
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for j1()");
		ivy_push_void(ivy);
	}
}

static void rty0(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tFP)
		ivy_push_double(ivy, y0(a->u.fp));
	else if (a->type == ivy_tNUM)
		ivy_push_double(ivy, y0((double) a->u.num));
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for y0()");
		ivy_push_void(ivy);
	}
}

static void rty1(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	if (a->type == ivy_tFP)
		ivy_push_double(ivy, y1(a->u.fp));
	else if (a->type == ivy_tNUM)
		ivy_push_double(ivy, y1((double) a->u.num));
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for y1()");
		ivy_push_void(ivy);
	}
}

static void rtjn(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	Ivy_val *b = ivy_getv_by_symbol(ivy, ivy_b_symbol);
	double l, r;
	if (a->type == ivy_tFP)
		l = a->u.fp;
	else if (a->type == ivy_tNUM)
		l = a->u.num;
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for jn()");
		ivy_push_void(ivy);
		return;
	}
	if (b->type == ivy_tFP)
		r = b->u.fp;
	else if (b->type == ivy_tNUM)
		r = b->u.num;
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for jn()");
		ivy_push_void(ivy);
		return;
	}
	ivy_push_double(ivy, jn(l, r));
}

static void rtyn(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	Ivy_val *b = ivy_getv_by_symbol(ivy, ivy_b_symbol);
	double l, r;
	if (a->type == ivy_tFP)
		l = a->u.fp;
	else if (a->type == ivy_tNUM)
		l = a->u.num;
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for yn()");
		ivy_push_void(ivy);
		return;
	}
	if (b->type == ivy_tFP)
		r = b->u.fp;
	else if (b->type == ivy_tNUM)
		r = b->u.num;
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for yn()");
		ivy_push_void(ivy);
		return;
	}
	ivy_push_double(ivy, yn(l, r));
}

static void rtmax(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	Ivy_val *b = ivy_getv_by_symbol(ivy, ivy_b_symbol);
	double l, r;
	if (a->type == ivy_tFP)
		l = a->u.fp;
	else if (a->type == ivy_tNUM)
		l = a->u.num;
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for max()");
		ivy_push_void(ivy);
		return;
	}
	if (b->type == ivy_tFP)
		r = b->u.fp;
	else if (b->type == ivy_tNUM)
		r = b->u.num;
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for max()");
		ivy_push_void(ivy);
		return;
	}
	if (l >= r)
		ivy_push_double(ivy, l);
	else
		ivy_push_double(ivy, r);
}

static void rtmin(Ivy *ivy)
{
	Ivy_val *a = ivy_getv_by_symbol(ivy, ivy_a_symbol);
	Ivy_val *b = ivy_getv_by_symbol(ivy, ivy_b_symbol);
	double l, r;
	if (a->type == ivy_tFP)
		l = a->u.fp;
	else if (a->type == ivy_tNUM)
		l = a->u.num;
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for min()");
		ivy_push_void(ivy);
		return;
	}
	if (b->type == ivy_tFP)
		r = b->u.fp;
	else if (b->type == ivy_tNUM)
		r = b->u.num;
	else {
		ivy_error_0(ivy->errprn, "Incorrect type for min()");
		ivy_push_void(ivy);
		return;
	}
	if (l < r)
		ivy_push_double(ivy, l);
	else
		ivy_push_double(ivy, r);
}

static void rtrandom(Ivy *ivy)
{
	ivy_push_int(ivy, random());
}

static void rtdepth(Ivy *ivy)
{
	ivy_push_int(ivy, ivy->sp - ivy->sptop);
}

/* Table of built-in functions */

struct ivy_builtin ivy_builtins[] = {
	{"help", ivy_rthelp, "a=void"},
	{"depth", rtdepth, ""},
	{"pr", rtprint, ""},
	{"vars", rtvars, ""},
	{"print", rtprint, "argv..."},
	{"printf", rtprintf, "argv..."},
	{"dup", rtdup, "a"},
	{"get", rtget, ""},
	{"atoi", rtatoi, "a"},
	{"itoa", rtitoa, "a"},
	{"len", rtlen, "a"},
	{"clear", rtclr, ""},
	{"loadfile", rtloadfile, "a"},
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
	{"symbolcount", rtsymbolcount, ""},
	{0, 0, 0}
};
