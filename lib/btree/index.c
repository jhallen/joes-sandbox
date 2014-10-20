/* Dynamic hash index file manager
   Copyright (C) 1992 Joseph H. Allen

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 1, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
more details.

You should have received a copy of the GNU General Public License along with
this program; see the file COPYING.  If not, write to the Free Software
Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "vfile.h"
#include "index.h"

int ssplit=0;
int sappend=0;

#define shl(a,b) ((b)>=32?0:(a)<<(b))

INDEX *iopen(char *name)
{
INDEX *i=malloc(sizeof(INDEX));
char *s=malloc(strlen(name)+5);
strcpy(s,name);
strcat(s,".ndx");
if(!(i->fd=vopen(s)))
 {
 free(s);
 free(i);
 return 0;
 }
free(s);
vseek(i->fd,vsize(i->fd)-2*sizeof(long));
i->bits=vgetl(i->fd);
i->start=vgetl(i->fd);
i->size=shl(1,32-i->bits);
i->base=vsize(i->fd)-i->size*sizeof(long)-2*sizeof(long);
return i;
}

void iclose(INDEX *ndx)
{
vseek(ndx->fd,ndx->base+ndx->size*sizeof(long));
vputl(ndx->fd,ndx->bits);
vputl(ndx->fd,ndx->start);
vclose(ndx->fd);
free(ndx);
}

/* Allocate a new page */

static unsigned long new(INDEX *ndx)
{
unsigned long rtval=ndx->base;
char buf[IPGSIZE];
unsigned long tt;
ndx->base+=IPGSIZE;
tt=vtell(ndx->fd);
valloc(ndx->fd,IPGSIZE);
if(ndx->size*sizeof(long)>IPGSIZE)
 {
 ndx->start=ndx->start-IPGSIZE/sizeof(long)&(ndx->size-1);
 vseek(ndx->fd,rtval); vread(ndx->fd,buf,IPGSIZE);
 vseek(ndx->fd,rtval+ndx->size*sizeof(long)); vwrite(ndx->fd,buf,IPGSIZE);
 }
else
 {
 vseek(ndx->fd,rtval); vread(ndx->fd,buf,ndx->size*sizeof(long));
 vseek(ndx->fd,rtval+IPGSIZE); vwrite(ndx->fd,buf,ndx->size*sizeof(long));
 }
vseek(ndx->fd,tt);
return rtval;
}

/* Double size of hash table */

static void expandhash(INDEX *ndx)
{
unsigned long from,to,tmp;
unsigned long sz=ndx->size;
valloc(ndx->fd,ndx->size*sizeof(long));
from=ndx->base+ndx->size*sizeof(long);
to=from+ndx->size*sizeof(long);
do
 {
 from-=sizeof(long);
 to-=sizeof(long);
 tmp=rl(ndx->fd,from);
 wl(ndx->fd,to,tmp);
 to-=sizeof(long);
 tmp=rl(ndx->fd,from);
 wl(ndx->fd,to,tmp);
 }
 while(--sz);
ndx->start<<=1;
--ndx->bits;
ndx->size<<=1;
}

/* Add entry to index */

void iadd(INDEX *ndx,unsigned long hash,unsigned long addr)
{
unsigned long first, a, b, middle, newfirst, c;
int x, y, total, above, bits;

if(hash==0) hash=1;
if(hash== ~0L) hash= ~1L;

first=rl(ndx->fd,
         ((hash>>ndx->bits)+ndx->start&ndx->size-1)*sizeof(long)+ndx->base);

a=first;
do
 {
 vseek(ndx->fd,a+sizeof(long));
 a=vgetl(ndx->fd);
 for(x=0;x!=IENTRIES;vgetl(ndx->fd), x++)
  if(b=vgetl(ndx->fd), b==0 || b== ~0L)
   {
   vseek(ndx->fd,vtell(ndx->fd)-sizeof(long));
   vputl(ndx->fd,hash);
   vputl(ndx->fd,addr);
   if((x+1!=IENTRIES) && !b) vputl(ndx->fd,0);
   return;
   }
 }
 while(a);

a=first;
total=0;
above=0;

bits=rl(ndx->fd,a);
middle=(shl(~0UL,bits)&hash)+(1<<(bits-1));

do
 {
 vseek(ndx->fd,a+sizeof(long));
 a=vgetl(ndx->fd);
 total+=IENTRIES;
 for(x=0;x!=IENTRIES;vgetl(ndx->fd), x++) if(vgetl(ndx->fd)>=middle) ++above;
 }
 while(a);

if(hash>=middle)
 if(above+1>total) goto append;
 else goto split;
else
 if(total-above+1>total) goto append;
 else goto split;

split:
ssplit++;
newfirst=new(ndx);

vseek(ndx->fd,newfirst);
vputl(ndx->fd,bits-1);
vputl(ndx->fd,0);
vseek(ndx->fd,first);
vputl(ndx->fd,bits-1);

if(bits==ndx->bits) expandhash(ndx);


vseek(ndx->fd,
      (ndx->size-1&(middle>>ndx->bits)+ndx->start)*sizeof(long)+ndx->base);
for(x=0;x!=(1<<(bits-ndx->bits-1));x++) vputl(ndx->fd,newfirst);

a=first;
c=newfirst;
y=2*sizeof(long);
do
 {
 vseek(ndx->fd,a+sizeof(long));
 a=vgetl(ndx->fd);
 for(x=0;x!=IENTRIES;x++)
  if(b=vgetl(ndx->fd), b>=middle)
   {
   if(y==IPGSIZE)
    {
    unsigned long p=c;
    y=2*sizeof(long);
    c=new(ndx);
    wl(ndx->fd,c+sizeof(long),0);
    wl(ndx->fd,p+sizeof(long),c);
    }
   wl(ndx->fd,c+y,b); y+=4;
   wl(ndx->fd,c+y,vgetl(ndx->fd)); y+=4;
   vseek(ndx->fd,vtell(ndx->fd)-2*sizeof(long));
   vputl(ndx->fd,~0L);
   vputl(ndx->fd,~0L);
   }
  else vgetl(ndx->fd);
 }
 while(a);  
if(y!=IPGSIZE) wl(ndx->fd,c+y,0);

if(hash>=middle) first=newfirst;
a=first;
do
 {
 vseek(ndx->fd,a+sizeof(long));
 a=vgetl(ndx->fd);
 for(x=0;x!=IENTRIES;vgetl(ndx->fd), x++)
  if(b=vgetl(ndx->fd), b==0 || b== ~0L)
   {
   vseek(ndx->fd,vtell(ndx->fd)-sizeof(long));
   vputl(ndx->fd,hash);
   vputl(ndx->fd,addr);
   if((x+1!=IENTRIES) && !b) vputl(ndx->fd,0);
   return;
   }
 }
 while(a);

append:
sappend++;
b=new(ndx);
a=rl(ndx->fd,first+sizeof(long));
wl(ndx->fd,first+sizeof(long),b);
vseek(ndx->fd,b+sizeof(long));
vputl(ndx->fd,a);
vputl(ndx->fd,hash);
vputl(ndx->fd,addr);
vputl(ndx->fd,0);
return;
}

void idel(INDEX *ndx,unsigned long hash,unsigned long addr)
{
unsigned long a, b;
int x;

if(hash==0) hash=1;
if(hash== ~0L) hash= ~1L;

a=rl(ndx->fd,
     ((hash>>ndx->bits)+ndx->start&ndx->size-1)*sizeof(long)+ndx->base);
do
 {
 vseek(ndx->fd,a+sizeof(long));
 a=vgetl(ndx->fd);
 for(x=0;x!=IENTRIES;x++)
  if((b=vgetl(ndx->fd))==hash)
   {
   if(vgetl(ndx->fd)==addr)
    {
    vseek(ndx->fd,vtell(ndx->fd)-2*sizeof(long));
    vputl(ndx->fd,~0L);
    return;
    }
   }
  else if(!b) break;
  else vgetl(ndx->fd);
 }
 while(a);
}

unsigned long *ifind(INDEX *ndx,unsigned long hash,
                     unsigned long *buf,int *bufsiz,int *buflen)
{
unsigned long a, b;
int x;
*buflen=0;
if(hash==0) hash=1;
if(hash== ~0L) hash= ~1L;

a=rl(ndx->fd,
     ((hash>>ndx->bits)+ndx->start&ndx->size-1)*sizeof(long)+ndx->base);
do
 {
 vseek(ndx->fd,a+sizeof(long));
 a=vgetl(ndx->fd);
 for(x=0;x!=IENTRIES;x++)
  if((b=vgetl(ndx->fd))==hash)
   {
   if(bufsiz==buflen)
    if(buf) buf=realloc(buf,sizeof(long)*(bufsiz+=32));
    else buf=malloc(sizeof(long)*(bufsiz=32));
   buf[buflen++]=vgetl(ndx->fd);
   }
  else if(!b) break;
  else vgetl(ndx->fd);
 }
 while(a);
return buf;
}

void imk(char *name)
{
char *s=malloc(strlen(name)+5);
VFILE *fd;
strcpy(s,name);
strcat(s,".ndx");
mscreat(s);
fd=vopen(s);
valloc(fd,IPGSIZE+3*sizeof(long));
wl(fd,0,32);
wl(fd,sizeof(long),0);
wl(fd,sizeof(long)*2,0);
wl(fd,IPGSIZE,0);
wl(fd,IPGSIZE+sizeof(long),32);
wl(fd,IPGSIZE+2*sizeof(long),0);
vclose(fd);
free(s);
}

void irm(char *name)
{
char *s=malloc(strlen(name)+5);
strcpy(s,name);
strcat(s,".ndx");
unlink(s);
free(s);
}
