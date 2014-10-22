/* Program to generate termcap index file
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

#include <stdio.h>

gen(s,fd)
char *s;
FILE *fd;
{
int c,x;
long addr=0, oaddr;
loop:
while(c=getc(fd), c==' ' || c=='\t' || c=='#')
 do c=getc(fd); while(!(c== -1 || c=='\n'));
if(c== -1) return;
if(c=='\n') goto loop;
oaddr=addr;
addr=ftell(fd)-1;
ungetc(c,fd);
s[x=0]=0;
while(1)
 {
 c=getc(fd);
 if(c== -1 || c=='\n')
  {
  if(x!=0 && s[x-1]=='\\') --x;
  if(x)
   {
   int y,z,flg;
   s[x]=0; z=0; flg=0;
   do
    {
    for(y=z;s[y] && s[y]!='|' && s[y]!=':';++y);
    c=s[y]; s[y]=0;
    if(strlen(s+z)>2 && !strchr(s+z,' ') &&
       !strchr(s+z,'\t')) (flg && putchar(' ')), fputs(s+z,stdout), flg=1;
    s[y]=c;
    z=y+1;
    }
    while(c && c!=':');
   if(flg) printf(" %x\n",addr-oaddr);
   }
  goto loop;
  }
 else if(c=='\r') ;
 else s[x++]=c;
 }
}

main()
{
char array[65536];
gen(array,stdin);
}
