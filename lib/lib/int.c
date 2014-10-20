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

unsigned Umin(a,b) unsigned a,b; { return a<b?a:b; }
unsigned Umax(a,b) unsigned a,b; { return a>b?a:b; }
int Imin(a,b) { return a<b?a:b; }
int Imax(a,b) { return a>b?a:b; }
long Lmax(a,b) long a,b; { return a>b?a:b; }
long Lmin(a,b) long a,b; { return a<b?a:b; }
int Iabs(a) { return a>=0?a:-a; }
long Labs(a) long a; { return a>=0?a:-a; }
