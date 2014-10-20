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

#include "config.h"

/* int Iabs(int n); Return absolute value of given int */
EXTERN int Iabs();

/* long Labs(long n); Return absolute value of given long */
EXTERN long Labs();

/* unsigned Umin(unsigned a,unsigned b); Return the smaller unsigned integer */
EXTERN unsigned Umin();

/* unsigned Umax(unsigned a,unsigned b); Return the larger unsigned integer */
EXTERN unsigned Umax();

/* int Imin(int a,int b); Return the lower integer */
EXTERN int Imin();

/* int Imax(int a,int b); Return the higher integer */
EXTERN int Imax();

/* long Lmax(long a,long b); Return the higher long */
EXTERN long Lmax();

/* long Lmin(long a,long b); Return the smaller long */
EXTERN long Lmin();

#endif
