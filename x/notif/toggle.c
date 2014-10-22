/* Toggle buttons
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

/* Toggle button */

static void togglerm(t)
Toggle *t;
 {
 cancel(t->fn); t->fn=0;
 (widgetfuncs.rm)(t);
 }

static void toggledisp(t)
Toggle *t;
 {
 if(t->state)
  {
  if(t->dispoff) disable(t->dispoff);
  if(t->dispon) enable(t->dispon);
  }
 else
  {
  if(t->dispon) disable(t->dispon);
  if(t->dispoff) enable(t->dispoff);
  }
 }

static void togglestfn(t,c)
Toggle *t;
TASK *c;
 {
 t->fn=c;
 }

static void togglestdispon(t,w)
Toggle *t;
Lith *w;
 {
 if(t->dispon) rm(t->dispon);
 t->dispon=w;
 tedge(t,0);
 ltor(t,NULL,0);
 disable(w);
 add(t,w);
 toggledisp(t);
 }

static void togglestdispoff(t,w)
Toggle *t;
Lith *w;
 {
 if(t->dispoff) rm(t->dispoff);
 t->dispoff=w;
 tedge(t,0);
 ltor(t,NULL,0);
 disable(w);
 add(t,w);
 toggledisp(t);
 }

static void togglest(t,n)
Toggle *t;
 {
 if(t->state!=n)
  {
  t->state=n;
  toggledisp(t);
  }
 }

static int togglegt(t)
Toggle *t;
 {
 return t->state;
 }

static int togglepress(t)
Toggle *t;
 {
 t->state= !t->state;
 stfocus(t);
 toggledisp(t);
 grabunwind();
 cont1(t->fn,t->state); t->fn=0;
 return 0;
 }

static int toggleuser(w,key)
Toggle *w;
 {
 switch(key)
  {
  case XK_Press1:
   togglepress(w);
   return 0;
  case 13: case 10: case 32:
   togglepress(w);
   return 0;
  }
 return 1;
 }

static void toggleenter(b,ev)
Button *b;
XEvent *ev;
 {
 int x=ev->xcrossing.x;
 int y=ev->xcrossing.y;
 if(gtmom(b)->funcs==&menufuncs && grabber && !greedy &&
    x>=0 && y>=0 && x<gtw(b) && y<gth(b))
  {
  zapto(gtmom(b));
  stfocus(b);
  }
 }

struct togglefuncs togglefuncs;

Toggle *mkToggle(t)
Toggle *t;
 {
 Icon *i;
 mkWidget(t);
 if(!togglefuncs.on)
  {
  mcpy(&togglefuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  togglefuncs.stfn=togglestfn;
  togglefuncs.stdispon=togglestdispon;
  togglefuncs.stdispoff=togglestdispoff;
  togglefuncs.st=togglest;
  togglefuncs.gt=togglegt;
  togglefuncs.user=toggleuser;
  togglefuncs.enternotify=toggleenter;
  togglefuncs.rm=togglerm;
  }
 t->funcs= &togglefuncs;
 t->fn=0;
 t->state=0;
 t->dispon=0;
 t->dispoff=0;
 stborder(t,mk(Tborder));
 i=mk(Icon);
  stborder(i,mk(Shadow));
  stwidth(i,12);
  stheight(i,12);
  st(i,piccheck);
 stdispon(t,i);
 i=mk(Icon);
  stborder(i,mk(Shadow));
  stwidth(i,12);
  stheight(i,12);
 stdispoff(t,i);
 stflg(t,gtflg(t)|flgfocus);
 return t;
 }

Toggle *mkMtoggle(t)
Toggle *t;
 {
 Icon *i;
 mkWidget(t);
 if(!togglefuncs.on)
  {
  mcpy(&togglefuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  togglefuncs.stfn=togglestfn;
  togglefuncs.stdispon=togglestdispon;
  togglefuncs.stdispoff=togglestdispoff;
  togglefuncs.st=togglest;
  togglefuncs.gt=togglegt;
  togglefuncs.user=toggleuser;
  togglefuncs.enternotify=toggleenter;
  }
 t->funcs= &togglefuncs;
 t->fn=0;
 t->state=0;
 t->dispon=0;
 t->dispoff=0;
 stborder(t,mk(Mborder));
 i=mk(Icon);
  st(i,picarc);
 stdispon(t,i);
 stflg(t,gtflg(t)|flgfocus);
 return t;
 }
