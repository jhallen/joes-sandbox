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

#include "int.h"

unsigned min(unsigned a,unsigned b) { return a<b?a:b; }
unsigned max(unsigned a,unsigned b) { return a>b?a:b; }
int min(int a,int b) { return a<b?a:b; }
int max(int a,int b) { return a>b?a:b; }
long max(long a,long b) { return a>b?a:b; }
long min(long a,long b) { return a<b?a:b; }
//This is in stdlib.h
//int abs(int a) { return a>=0?a:-a; }
//long abs(long a) { return a>=0?a:-a; }
