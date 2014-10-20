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

#ifndef _Izstr
#define _Izstr 1

#include "config.h"

#define _upp 1
#define _low 2
#define _und 4
#define _bin 8
#define _oct 16
#define _dec 32
#define _hex 64
#define _flo 128

#define _whi 1
#define _eol 2
#define _eos 4

extern char _ctaB[], _ctaA[];

/* Character type test macros */

/* Upper case */
#define cupper(c) (_ctaB[(unsigned char)(c)]&_upp)

/* Lower case */
#define clower(c) (_ctaB[(unsigned char)(c)]&_low)

/* Letter */
#define calpha(c) (_ctaB[(unsigned char)(c)]&(_low|_upp))

/* First legal character of a C identifier */
#define cfirst(c) (_ctaB[(unsigned char)(c)]&(_low|_upp|_und))

/* Remaining legal characters of a C identifier */
#define crest(c)  (_ctaB[(unsigned char)(c)]&(_low|_upp|_und|_bin|_oct|_dec))

/* Binary digit */
#define cbin(c)   (_ctaB[(unsigned char)(c)]&(_bin))

/* Octal digit */
#define coct(c)   (_ctaB[(unsigned char)(c)]&(_bin|_oct))

/* Decimal digit */
#define cdec(c)   (_ctaB[(unsigned char)(c)]&(_bin|_oct|_dec))

/* Hexadecimal digit */
#define chex(c)   (_ctaB[(unsigned char)(c)]&(_bin|_oct|_dec|_hex))

/* Floating-point digit */
#define cfloat(c) (_ctaB[(unsigned char)(c)]&(_bin|_oct|_dec|_flo))

/* Whitespace: tab or space */
#define cwhite(c)  (_ctaA[(unsigned char)(c)]&(_whi))

/* Whitespace: tab, space or newline */
#define cwhitel(c) (_ctaA[(unsigned char)(c)]&(_whi|_eol))

/* Whitespace: tab, space, newline or nul */
#define cwhitef(c) (_ctaA[(unsigned char)(c)]&(_whi|_eol|_eos))

/* int Iabs(int n); Return absolute value of given int */
int Iabs();

/* long Labs(long n); Return absolute value of given long */
long Labs();

/* int toup(int c); Convert character to uppercase if it was lowercase */
int toup();

/* int todn(int c); Convert character to lowercase if it was uppercase */
int todn();

/* unsigned Umin(unsigned a,unsigned b); Return the smaller unsigned integer */
unsigned Umin();

/* unsigned Umax(unsigned a,unsigned b); Return the larger unsigned integer */
unsigned Umax();

/* int Imin(int a,int b); Return the lower integer */
int Imin();

/* int Imax(int a,int b); Return the higher integer */
int Imax();

/* long Lmax(long a,long b); Return the higher long */
long Lmax();

/* long Lmin(long a,long b); Return the smaller long */
long Lmin();

/* int zlen(char *s); Return length of z-string */
int zlen();

/* char *zchr(char *s,char c); Return address of first 'c' in 's', or NULL if
 * the end of 's' was found first */
char *zchr();

/* char *zrchr(char *s,char c); Return address of last 'c' in 's', or NULL if
 * there are none.
 */
char *zrchr();

/* char *zcpy(char *d,char *s); Copy z-string at s to d */
char *zcpy();

/* char *zcat(char *d,char *s); Append s onto d */
char *zcat();

/* char *zdup(char *s); Duplicate z-string into an malloc block */
char *zdup();

/* int zcmp(char *l,char *r); Compare the strings.  Return -1 if l is
 * less than r; return 0 if l equals r; and return 1 if l is greater than r.
 */
int zcmp();

/* int zicmp(char *l,char *r); Compare the strings, case insensitive.  Return
 * -1 if l is less than r; return 0 if l equals r; and return 1 if l is greater
 * than r.
 */
int zicmp();

/* int fields(char *s,char **fields,char sep); Break up z-string containing
 * fields into its componant fields.  This is done by setting the field
 * seperator characters to zero- thereby changing the fields into z-strings,
 * and by storing the starting address of each field in the array 'fields'
 * (which must be large enough to store the field pointers).
 *
 * The number of fields which were found in s is returned.
 */
int fields();

/* int nfields(char *s,char sep); Assuming 'sep' is the field seperator
 * character, count the number of fields in z-string s.  If no 'sep'
 * characters are in 's', the number of fields is 1.
 */
int nfields();

#endif
