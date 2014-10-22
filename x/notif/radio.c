/* Radio buttons 
   Copyright (C) 1999 Joseph H. Allen

This file is part of Notif

Notif is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation; either version 1, or (at your option) any later version.  

Notif is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.  

You should have received a copy of the GNU General Public License along with 
Notif; see the file COPYING.  If not, write to the Free Software Foundation, 
675 Mass Ave, Cambridge, MA 02139, USA.  */ 

#include "notif.h"

static int radiogt(r)
Radio *r;
 {
 return r->state;
 }

static void radiost(r,n)
Radio *r;
 {
 LINK *l;
 int x;
 Toggle *t;
 r->state=n;
 for(x=0,l=r->list->next;t=(Toggle *)l->data, l!=r->list;l=l->next)
  {
  if(n==x && !gt(t)) st(t,1);
  if(n!=x && gt(t)) st(t,0);
  ++x;
  }
 }

static void radiostfn(r,c)
Radio *r;
TASK *c;
 {
 r->fn=c;
 }

static int radioevt(stat,r,w,n)
Radio *r;
Toggle *w;
 {
 if(!stat)
  {
  int x;
  LINK *l;
  TASK *task;
  task=qnth(r->fns->next,qfindn(r->list,w))->data;
  stfn(w,fn2(task,radioevt,r,w));
  if(!n) st(w,1);
  else
   {
   Toggle *t;
   for(x=0,l=r->list->next;t=(Toggle *)l->data, t!=w;l=l->next, ++x);
   radiost(r,x);
   if(r->fn) cont1(r->fn,x), r->fn=0;
   }
  }
 }

static void radioadd(r,w)
Radio *r;
Toggle *w;
 {
 TASK *task=malloc(sizeof(TASK));
 iztask(task);
 enqueb(r->list,w);
 enqueb(r->fns,task);
 stfn(w,fn2(task,radioevt,r,w));
 }

static void radiorm(r)
Radio *r;
 {
 cancel(r->fn); r->fn=0;
 while(!qempty(r->list)) cancel(((Toggle *)deque(r->list->next))->fn);
 deque(r->list);
 while(!qempty(r->fns)) free(deque(r->fns->next));
 (widgetfuncs.rm)(r);
 }

struct radiofuncs radiofuncs;

Radio *mkRadio(r)
Radio *r;
 {
 mkWidget(r);
 if(!radiofuncs.on)
  {
  mcpy(&radiofuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  radiofuncs.stfn=radiostfn;
  radiofuncs.st=radiost;
  radiofuncs.gt=radiogt;
  radiofuncs.groupadd=radioadd;
  radiofuncs.rm=radiorm;
  }
 r->funcs= &radiofuncs;
 r->state= -1;
 r->fn=0;
 r->list=newque(NULL);
 r->fns=newque(NULL);
 ttob(r,NULL,0);
 ledge(r,0);
 return r;
 }
