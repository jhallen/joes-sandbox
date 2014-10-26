/* Zero terminated strings
   Copyright (C) 1992 Joseph H. Allen

This file is part of JOE (Joe's Own Editor)

JOE is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation; either version 1, or (at your option) any later version.  

JOE is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.  

You should have received a copy of the GNU General Public License along with 
JOE; see the file COPYING.  If not, write to the Free Software Foundation, 
675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <stdlib.h>
#include "zstr.h"

int toup(int a)
{
	return (a >= 'a' && a <= 'z') ? a + 'A' - 'a' : a;
}

unsigned Umin(unsigned a, unsigned b)
{
	return a < b ? a : b;
}

unsigned Umax(unsigned a, unsigned b)
{
	return a > b ? a : b;
}

int Imin(int a, int b)
{
	return a < b ? a : b;
}

int Imax(int a, int b)
{
	return a > b ? a : b;
}

int Iabs(int a)
{
	return a >= 0 ? a : -a;
}

int cword(int c)
{
	return c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c >= '0'
	    && c <= '9' || c == '_';
}

int cwhitel(int c)
{
	return c == ' ' || c == '\t' || c == '\n';
}

int cwhite(int c)
{
	return c == ' ' || c == '\t';
}

int zlen(char *s)
{
	char *os = s;
	while (*s)
		++s;
	return s - os;
}

char *zcpy(char *d, char *s)
{
	char *od = d;
	while (*d++ = *s++);
	return od;
}

char *zcat(char *d, char *s)
{
	char *od = d;
	while (*d)
		++d;
	while (*d++ = *s++);
	return od;
}

char *zdup(char *s)
{
	return zcpy((char *) malloc(zlen(s) + 1), s);
}

int zcmp(char *l, char *r)
{
	while (*l && *l == *r)
		++l, ++r;
	if (*l > *r)
		return 1;
	if (*l < *r)
		return -1;
	return 0;
}

int fields(char *s, char **fields, int sep)
{
	int y = 1;
	fields[0] = s;
	while (*s) {
		if (*s == sep)
			fields[y++] = s + 1, *s = 0;
		++s;
	}
	return y;
}

int nfields(char *s, int sep)
{
	int y = 1;
	while (*s)
		if (*s++ == sep)
			++y;
	return y;
}
