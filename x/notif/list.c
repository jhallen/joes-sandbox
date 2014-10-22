/* List widget 
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

static void listrm(v)
List *v;
 {
 cancel(v->fn); v->fn=0;
 (widgetfuncs.rm)(v);
 }

static void listcalc(v)
List *v;
 {
 LINK *l;
 int updown=0;
 int y;

 /* Calculate height and direction */

 for(y=0,l=v->kids->next;l!=v->kids;l=l->next)
  {
  Widget *w=l->data;
  if(gty(w)>y) ++updown;
  else if(gty(w)<y) --updown;
  y+=gth(w);
  }
 v->height=y;
 if(!y) return;
 stheight(v,y+100);

 if(updown>=0)
  for(y=0,l=v->kids->next;l!=v->kids;l=l->next)
   {
   Widget *w=l->data;
   sty(w,y);
   y+=gth(w);
   }
 else
  for(y=v->height,l=v->kids->prev;l!=v->kids;l=l->prev)
   {
   Widget *w=l->data;
   y-=gth(w);
   sty(w,y);
   }
 }

static void listins(v,n,w)
List *v;
Widget *w;
 {
 (widgetfuncs.ins)(v,n,w);
 listcalc(v);
 }

static void listadd(v,w)
List *v;
Widget *w;
 {
 (widgetfuncs.add)(v,w);
 listcalc(v);
 }

static void listrmv(v,w)
List *v;
Widget *w;
 {
 (widgetfuncs.rmv)(v,w);
 listcalc(v);
 }

static void listdrop(v,w,x,y)
List *v;
Widget *w;
 {
 int yy;
 LINK *l;
 int n;
 if(!v->fn) return;
 for(yy=0,l=v->kids->next,n=0;l!=v->kids;l=l->next,++n)
  {
  Widget *wi=l->data;
  if(y>=yy && y<yy+gth(wi))
   { /* We found a spot for the window */
   if(wi!=w) cont2(v->fn,n,w), v->fn=0;
   return;
   }
  yy+=gth(wi);
  }
 if(y>=yy) cont2(v->fn,n,w), v->fn=0;
 }

static void liststfn(v,fn)
List *v;
TASK *fn;
 {
 v->fn=fn;
 }

struct listfuncs listfuncs;

List *mkVlist(v)
List *v;
 {
 mkWidget(v);
 if(!listfuncs.on)
  {
  mcpy(&listfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  listfuncs.ins=listins;
  listfuncs.add=listadd;
  listfuncs.rmv=listrmv;
  listfuncs.drop=listdrop;
  listfuncs.stfn=liststfn;
  listfuncs.rm=listrm;
  }
 v->funcs= &listfuncs;
 v->height=0;
 v->fn=0;
 ledge(v,0);
 ttob(v,NULL,0);
 return v;
 }
