/* Symbol table management functions
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
#include "hash.h"

extern void *malloc();

unsigned long hash(s)
char *s;
 {
 unsigned long accu=0;
 while(*s) accu=hnext(accu,*s++);
 return accu;
 }

unsigned long hashn(s,n)
char *s;
 {
 unsigned long accu=0;
 while(n--) accu=hnext(accu,*s++);
 return accu;
 }

HASH *htmk(len)
 {
 HASH *t=(HASH *)malloc(sizeof(HASH));
 t->len=len-1;
 t->llen=len*4-1;
 t->tab=(HENTRY **)calloc(sizeof(HENTRY *),len);
 t->next=0;
 t->ltab=0;
 t->btab=0;
 return t;
 }

void htrm(ht)
HASH *ht;
 {
 HASH *nxt;
 do
  {
  nxt=ht->next;
  free(ht->tab);
  if(ht->btab) free(ht->btab);
  free(ht);
  } while(ht=nxt);
 }

void *htaddn(ht,name,len,val)
HASH *ht;
char *name;
int len;
void *val;
 {
 unsigned long accu=0;
 int x;
 int idx;
 HENTRY *entry;
 if(!ht->btab)
  {
  ht->btab=calloc(ht->llen+1,1);
  if(ht->ltab) memcpy(ht->btab,ht->ltab,ht->llen+1);
  ht->ltab=ht->btab;
  }
 x=0;
 if(x!=len) accu=hnext(accu,name[x++]);
 while(x!=len)
  {
  ++ht->btab[accu&ht->llen];
  accu=hnext(accu,name[x++]);
  }
 idx=(accu&ht->len);

 entry=malloc(sizeof(HENTRY));

 entry->next=ht->tab[idx];
 ht->tab[idx]=entry;
 entry->name=name;
 entry->len=len;
 return entry->val=val;
 }

void *htadd(ht,name,val)
HASH *ht;
char *name;
void *val;
 {
 return htaddn(ht,name,zlen(name),val);
 }

void *htfind(ht,name)
HASH *ht;
char *name;
 {
 HENTRY *e;
 int hh=hash(name);
 do
  {
  for(e=ht->tab[hh&ht->len];e;e=e->next)
   if(!zcmp(e->name,name)) return e->val;
  } while(ht=ht->next);
 return 0;
 }

void *htfindn(ht,hval,name,len)
HASH *ht;
char *name;
 {
 HENTRY *e;
 do
  {
  for(e=ht->tab[hval&ht->len];e;e=e->next)
   if(len==e->len && !strncmp(e->name,name,len)) return e->val;
  } while(ht=ht->next);
 return 0;
 }

static void *dofind(ht,accu,start,iptr,func)
HASH *ht;
unsigned long accu;
char *start;
char **iptr;
void *(*func)();
 {
 char *ptr= *iptr;
 void *t;
 if(!ht->ltab) return 0;
 do
  {
  if(!*ptr) break;
  accu=hnext(accu,*ptr++);
  if(t=htfindn(ht,accu,start,ptr-start))
   {
   void *u;
   *iptr=ptr;
   if(u=dofind(ht,accu,start,iptr,func)) return u;
   else
    if(func) return func(t,ptr);
    else return t;
   }
  } while(ht->ltab[accu&ht->llen]);
 return 0;
 }

void *htlfind(ht,ptr)
HASH *ht;
char **ptr;
 {
 return dofind(ht,0L,*ptr,ptr,NULL);
 }

void *htlfindx(ht,ptr,func)
HASH *ht;
char **ptr;
void *(*func)();
 {
 return dofind(ht,0L,*ptr,ptr,func);
 }

HASH *htpsh(old)
HASH *old;
 {
 HASH *n=htmk(old->len+1);
 n->next= old;
 n->ltab= old->ltab;
 return n;
 }

void htall(ht,func,obj)
HASH *ht;
void (*func)();
void *obj;
 {
 int n;
 HENTRY *e;
 for(n=0;n!=ht->len+1;++n)
  for(e=ht->tab[n];e;e=e->next)
   func(obj,e->name,e->val);
 }
