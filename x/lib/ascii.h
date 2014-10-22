/* ASCII character functions
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

#ifndef _Iascii
#define _Iascii 1

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

extern char _ctaB[], _ctaA[], _hval[];

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

/* Value of hex digit */
#define hexval(c) (_hval[(unsigned char)(c)])

/* int toup(int c); Convert character to uppercase if it was lowercase */
extern int toup();

/* int todn(int c); Convert character to lowercase if it was uppercase */
extern int todn();

#endif
