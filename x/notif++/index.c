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

int Index::expose(XEvent *ev)
 {
 int q;
 Widget *f = list.nth(0);

 int hgt=f->gth()+f->gty()+2;

 f = list.nth(state);

 int a=f->gtx()-2;
 int b=a+2+f->gtw()+2;

 int xpos=3;
 int ypos=hgt;
 int w=gtwidth()-6;
 int h=gtheight()-hgt-3;

 /* a is left x cord of tab.  b is right x cord of tab.  hgt is y cord of
  * bottom of tab. */

 for(q=0;q!=2;++q)
  {
  drawline(this,stdshadowbot,a+q,3+q,b-1-q,3+q);
  drawline(this,stdshadowbot,a+q,3+q,a+q,hgt+q);
  drawline(this,stdshadowtop,b-1-q,3+q,b-1-q,hgt+q);

  drawline(this,stdshadowbot,xpos+q,ypos+q,a+q,ypos+q);
  drawline(this,stdshadowbot,b-q,ypos+q,xpos+w-1-q,ypos+q);
  drawline(this,stdshadowbot,xpos+q,ypos+q,xpos+q,ypos+h-1-q);
  drawline(this,stdshadowtop,xpos+w-1-q,ypos+q,xpos+w-1-q,ypos+h-1-q);
  drawline(this,stdshadowtop,xpos+q,ypos+h-1-q,xpos+w-1-q,ypos+h-1-q);
  }
 }

int Index::gt()
 {
 return state;
 }

void Index::st(int n)
 {
 Widget *f = widgets.nth(state);
 f->disable();
 state=n;
 f = widgets.nth(state);
 f->enable();
 redraw(this);
 }

void indexevt(Index *r,Button *w)
 {
  int x;
  // t is currently set one
  Button *t=r->list.nth(r->state);
  if(!t->gtfn()) t->stfn(new Fn2_0<void,Index *,Button *>(indexevt,r,t));
  x=r->list.find(w);
  printf("Going to index=%d\n",x);
  r->st(x);
 }

void Index::addpair(Button *a,Widget *b)
 {
 if (list.no()) ltor(list.nth(list.no()-1),4);
 else ledge(5);
 tedge(6);
 add(a);

 list.insert(a);
 widgets.insert(b);
 if(list.no() != 1) b->disable();
 ledge(6);
 tedge(44);
 auxredge(6);
 auxbedge(6);
 add(b);
 a->stfn(new Fn2_0<void,Index *,Button *>(indexevt,this,a));
 }

Index::Index()
 {
 state= 0;
 ledge(5);
 tedge(5);
 }

Index::~Index()
 {
 }
