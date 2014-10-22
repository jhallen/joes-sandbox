/* Index buttons  
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

static int indexdraw(r)
Index *r;
 {
 int q;
 int hgt=gth((Button *)r->list->next->data)+
         gty((Button *)r->list->next->data)+2;
 int a=gtx((Button *)qnth(r->list->next,r->state)->data)-2;
 int b=a+2+gtw((Button *)qnth(r->list->next,r->state)->data)+2;

 int xpos=3;
 int ypos=hgt;
 int w=gtwidth(r)-6;
 int h=gtheight(r)-hgt-3;

 /* a is left x cord of tab.  b is right x cord of tab.  hgt is y cord of
  * bottom of tab. */

 for(q=0;q!=2;++q)
  {
  drawline(r,stdshadowbot,a+q,3+q,b-1-q,3+q);
  drawline(r,stdshadowbot,a+q,3+q,a+q,hgt+q);
  drawline(r,stdshadowtop,b-1-q,3+q,b-1-q,hgt+q);

  drawline(r,stdshadowbot,xpos+q,ypos+q,a+q,ypos+q);
  drawline(r,stdshadowbot,b-q,ypos+q,xpos+w-1-q,ypos+q);
  drawline(r,stdshadowbot,xpos+q,ypos+q,xpos+q,ypos+h-1-q);
  drawline(r,stdshadowtop,xpos+w-1-q,ypos+q,xpos+w-1-q,ypos+h-1-q);
  drawline(r,stdshadowtop,xpos+q,ypos+h-1-q,xpos+w-1-q,ypos+h-1-q);
  }
 }

static int indexgt(r)
Index *r;
 {
 return r->state;
 }

static void indexst(r,n)
Index *r;
 {
 disable((Widget *)qnth(r->widgets->next,r->state)->data);
 r->state=n;
 enable((Widget *)qnth(r->widgets->next,r->state)->data);
 redraw(r);
 }

static void indexstfn(r,c)
Index *r;
TASK *c;
 {
 }

static int indexevt(stat,r,w)
Index *r;
Button *w;
 {
 if(!stat)
  {
  int x;
  LINK *l;
  Button *t=qnth(r->list->next,r->state)->data;
  TASK *task=qnth(r->fns->next,r->state)->data;
  if(!t->fn) stfn(t,fn2(task,indexevt,r,t));
  x=qfindn(r->list,w);
  indexst(r,x);
  }
 return stat;
 }

static void indexadd(r,a,b)
Index *r;
Button *a;
Widget *b;
 {
 TASK *task=malloc(sizeof(TASK));
 iztask(task);
 enqueb(r->list,a);
 enqueb(r->widgets,b);
 enqueb(r->fns,task);
 if(r->list->next->data!=a) disable(b);
 add(r,a);
 stx(b,6);
 sty(b,44);
 add(r,b);
 tedge(r,5);
 ltor(r,a,4);
 stfn(a,fn2(task,indexevt,r,a));
 }

static void indexrm(r)
Index *r;
 {
 while(!qempty(r->list)) cancel(((Button *)deque(r->list->next))->fn);
 deque(r->list);
 while(!qempty(r->widgets)) deque(r->widgets->next);
 deque(r->widgets);
 while(!qempty(r->fns)) free(deque(r->fns->next));
 deque(r->fns);
 (widgetfuncs.rm)(r);
 }

struct indexfuncs indexfuncs;

Index *mkIndex(r)
Index *r;
 {
 mkWidget(r);
 if(!indexfuncs.on)
  {
  mcpy(&indexfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  indexfuncs.st=indexst;
  indexfuncs.gt=indexgt;
  indexfuncs.addpair=indexadd;
  indexfuncs.expose=indexdraw;
  indexfuncs.rm=indexrm;
  }
 r->funcs= &indexfuncs;
 r->state= 0;
 r->list=newque(NULL);
 r->widgets=newque(NULL);
 r->fns=newque(NULL);
 ledge(r,5);
 tedge(r,5);
 return r;
 }
