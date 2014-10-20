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

#include "zstr.h"

char _ctaB[]=
 {
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,_flo,0,_bin,_bin,_oct,_oct,_oct,_oct,_oct,_oct,_dec,_dec,0,0,0,0,0,0,
 0,_upp|_hex,_upp|_hex,_upp|_hex,_upp|_hex,_upp|_hex|_flo,_upp|_hex,_upp,_upp,_upp,_upp,_upp,_upp,_upp,_upp,_upp,_upp,_upp,_upp,_upp,_upp,_upp,_upp,_upp,_upp,_upp,_upp,0,0,0,0,_und,
 0,_low|_hex,_low+_hex,_low|_hex,_low|_hex,_low|_hex|_flo,_low|_hex,_low,_low,_low,_low,_low,_low,_low,_low,_low,_low,_low,_low,_low,_low,_low,_low,_low,_low,_low,_low,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
 };

char _ctaA[]=
 {
 _eos,0,0,0,0,0,0,0,0,_whi,_eol,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 _whi,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
 };

int toup(a) { return clower(a) ? a+'A'-'a' : a; }
int todn(a) { return cupper(a) ? a+'a'-'A' : a; }
unsigned Umin(a,b) unsigned a,b; { return a<b?a:b; }
unsigned Umax(a,b) unsigned a,b; { return a>b?a:b; }
int Imin(a,b) { return a<b?a:b; }
int Imax(a,b) { return a>b?a:b; }
long Lmax(a,b) long a,b; { return a>b?a:b; }
long Lmin(a,b) long a,b; { return a<b?a:b; }
int Iabs(a) { return a>=0?a:-a; }
long Labs(a) long a; { return a>=0?a:-a; }

int zlen(s)
char *s;
 {
 char *os=s;
 loop:
  if(!s[0]) return s-os;
  if(!s[1]) return s-os+1;
  if(!s[2]) return s-os+2;
  if(!s[3]) return s-os+3;
  if(!s[4]) return s-os+4;
  if(!s[5]) return s-os+5;
  if(!s[6]) return s-os+6;
  if(!s[7]) return s-os+7;
  if(!s[8]) return s-os+8;
  if(!s[9]) return s-os+9;
  if(!s[10]) return s-os+10;
  if(!s[11]) return s-os+11;
  if(!s[12]) return s-os+12;
  if(!s[13]) return s-os+13;
  if(!s[14]) return s-os+14;
  if(!s[15]) return s-os+15;
 s+=15; goto loop;
 }

char *zchr(s,c)
char *s, c;
 {
 loop:
 if(!s[0]) return 0; else if(s[0]==c) return s;
 if(!s[1]) return 0; else if(s[1]==c) return s+1;
 if(!s[2]) return 0; else if(s[2]==c) return s+2;
 if(!s[3]) return 0; else if(s[3]==c) return s+3;
 if(!s[4]) return 0; else if(s[4]==c) return s+4;
 if(!s[5]) return 0; else if(s[5]==c) return s+5;
 if(!s[6]) return 0; else if(s[6]==c) return s+6;
 if(!s[7]) return 0; else if(s[7]==c) return s+7;
 if(!s[8]) return 0; else if(s[8]==c) return s+8;
 if(!s[9]) return 0; else if(s[9]==c) return s+9;
 if(!s[10]) return 0; else if(s[10]==c) return s+10;
 if(!s[11]) return 0; else if(s[11]==c) return s+11;
 if(!s[12]) return 0; else if(s[12]==c) return s+12;
 if(!s[13]) return 0; else if(s[13]==c) return s+13;
 if(!s[14]) return 0; else if(s[14]==c) return s+14;
 if(!s[15]) return 0; else if(s[15]==c) return s+15;
 s+=16; goto loop;
 }

char *zrchr(s,c)
char *s, c;
 {
 char *p=0;
 while(s=zchr(s,c)) p=s;
 return p;
 }

char *zcpy(d,s)
char *d,*s;
 {
 char *od=d;
 loop:
  if(!(d[0]=s[0])) return od;
  if(!(d[1]=s[1])) return od;
  if(!(d[2]=s[2])) return od;
  if(!(d[3]=s[3])) return od;
  if(!(d[4]=s[4])) return od;
  if(!(d[5]=s[5])) return od;
  if(!(d[6]=s[6])) return od;
  if(!(d[7]=s[7])) return od;
  if(!(d[8]=s[8])) return od;
  if(!(d[9]=s[9])) return od;
  if(!(d[10]=s[10])) return od;
  if(!(d[11]=s[11])) return od;
  if(!(d[12]=s[12])) return od;
  if(!(d[13]=s[13])) return od;
  if(!(d[14]=s[14])) return od;
  if(!(d[15]=s[15])) return od;
 s+=16; d+=16; goto loop;
 }

char *zcat(d,s)
char *d,*s;
 {
 char *od=d;
 zcpy(d+zlen(s),s);
 return od;
 }

char *zdup(s)
char *s;
 {
 return zcpy((char *)malloc(zlen(s)+1),s);
 }

int zcmp(l,r)
char *l, *r;
 {
 int c;
 loop:
  if(!l[0] || l[0]!=r[0]) { c=0; goto done; }
  if(!l[1] || l[1]!=r[1]) { c=1; goto done; }
  if(!l[2] || l[2]!=r[2]) { c=2; goto done; }
  if(!l[3] || l[3]!=r[3]) { c=3; goto done; }
  if(!l[4] || l[4]!=r[4]) { c=4; goto done; }
  if(!l[5] || l[5]!=r[5]) { c=5; goto done; }
  if(!l[6] || l[6]!=r[6]) { c=6; goto done; }
  if(!l[7] || l[7]!=r[7]) { c=7; goto done; }
  if(!l[8] || l[8]!=r[8]) { c=8; goto done; }
  if(!l[9] || l[9]!=r[9]) { c=9; goto done; }
  if(!l[10] || l[10]!=r[10]) { c=10; goto done; }
  if(!l[11] || l[11]!=r[11]) { c=11; goto done; }
  if(!l[12] || l[12]!=r[12]) { c=12; goto done; }
  if(!l[13] || l[13]!=r[13]) { c=13; goto done; }
  if(!l[14] || l[14]!=r[14]) { c=14; goto done; }
  if(!l[15] || l[15]!=r[15]) { c=15; goto done; }
 l+=16; r+=16; goto loop;
 done:
 if(l[c]>r[c]) return 1;
 if(l[c]<r[c]) return -1;
 return 0;
 }

int zicmp(l,r)
char *l, *r;
 {
 int c;
 loop:
  if(!toup(l[0]) || toup(l[0])!=toup(r[0])) { c=0; goto done; }
  if(!toup(l[1]) || toup(l[1])!=toup(r[1])) { c=1; goto done; }
  if(!toup(l[2]) || toup(l[2])!=toup(r[2])) { c=2; goto done; }
  if(!toup(l[3]) || toup(l[3])!=toup(r[3])) { c=3; goto done; }
  if(!toup(l[4]) || toup(l[4])!=toup(r[4])) { c=4; goto done; }
  if(!toup(l[5]) || toup(l[5])!=toup(r[5])) { c=5; goto done; }
  if(!toup(l[6]) || toup(l[6])!=toup(r[6])) { c=6; goto done; }
  if(!toup(l[7]) || toup(l[7])!=toup(r[7])) { c=7; goto done; }
  if(!toup(l[8]) || toup(l[8])!=toup(r[8])) { c=8; goto done; }
  if(!toup(l[9]) || toup(l[9])!=toup(r[9])) { c=9; goto done; }
  if(!toup(l[10]) || toup(l[10])!=toup(r[10])) { c=10; goto done; }
  if(!toup(l[11]) || toup(l[11])!=toup(r[11])) { c=11; goto done; }
  if(!toup(l[12]) || toup(l[12])!=toup(r[12])) { c=12; goto done; }
  if(!toup(l[13]) || toup(l[13])!=toup(r[13])) { c=13; goto done; }
  if(!toup(l[14]) || toup(l[14])!=toup(r[14])) { c=14; goto done; }
  if(!toup(l[15]) || toup(l[15])!=toup(r[15])) { c=15; goto done; }
 l+=16; r+=16; goto loop;
 done:
 if(toup(l[c])>toup(r[c])) return 1;
 if(toup(l[c])<toup(r[c])) return -1;
 return 0;
 }

int fields(s,fld,sep)
char *s, **fld, sep;
 {
 int y=1;
 for(fld[0]=s;s=zchr(s,sep);*s=0, fld[y++]= ++s);
 return y;
 }

int nfields(s,sep)
char *s, sep;
 {
 int y=1;
 while(s=zchr(s,sep)) ++y, ++s;
 return y;
 }
