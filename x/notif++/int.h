/* Integer functions
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

#ifndef _Iint
#define _Iint 1

#ifndef MAXINT
#define MAXINT (((unsigned)~0) >> 1)
#endif

#ifndef MAXLONG
#define MAXLONG (((unsigned long)~0L) >> 1)
#endif

/* int Iabs(int n); Return absolute value of given int */
// This is in stdlib.h
// extern int abs(int a);

/* long Labs(long n); Return absolute value of given long */
// extern long abs(long a);

/* unsigned Umin(unsigned a,unsigned b); Return the smaller unsigned integer */
extern unsigned min(unsigned a,unsigned b);

/* unsigned Umax(unsigned a,unsigned b); Return the larger unsigned integer */
extern unsigned max(unsigned a,unsigned b);

/* int Imin(int a,int b); Return the lower integer */
extern int min(int a,int b);

/* int Imax(int a,int b); Return the higher integer */
extern int max(int a,int b);

/* long Lmax(long a,long b); Return the higher long */
extern long max(long a,long b);

/* long Lmin(long a,long b); Return the smaller long */
extern long min(long a,long b);

#endif
