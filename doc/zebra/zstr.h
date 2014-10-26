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

int cword(int c);
int cwhite(int c);
int cwhitel(int c);
int Iabs(int c);
int toup(int c);
unsigned Umin(unsigned a, unsigned b);
unsigned Umax(unsigned a, unsigned b);
int Imin(int a, int b);
int Imax(int a, int b);
char *zcpy(char *d, char *s);
char *zdup(char *s);
char *zcat();
int zcmp(char *l, char *r);
int zlen(char *s);
int fields(char *s, char **fields, int sep);
int nfields(char *s, int sep);

#endif
