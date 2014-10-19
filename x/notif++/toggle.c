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

void Toggle::disp()
 {
 if(state)
  {
  if(dispoff) dispoff->disable();
  if(dispon) dispon->enable();
  }
 else
  {
  if(dispon) dispon->disable();
  if(dispoff) dispoff->enable();
  }
 }

void Toggle::stfn(Fn_1<void,int> *new_fn)
 {
 fn=new_fn;
 }

void Toggle::stdispon(Lith *w)
 {
 if(dispon) delete dispon;
 dispon=w;
 tedge(0);
 ltor(NULL,0);
 w->disable();
 add(w);
 disp();
 }

void Toggle::stdispoff(Lith *w)
 {
 if(dispoff) delete dispoff;
 dispoff=w;
 tedge(0);
 ltor(NULL,0);
 w->disable();
 add(w);
 disp();
 }

void Toggle::st(int n)
 {
 if(state!=n)
  {
  state=n;
  disp();
  }
 }

int Toggle::gt()
 {
 return state;
 }

void Toggle::press()
 {
 state= !state;
 stfocus(this);
 disp();
 grabunwind();
 if (fn) fn->cont(fn,state);
 }

int Toggle::user(unsigned c,unsigned state,int x,int y,Time time,Widget *org)
 {
 switch(c)
  {
  case XK_Press1:
   press();
   return 0;
  case 13: case 10: case 32:
   press();
   return 0;
  }
 return 1;
 }

int Toggle::enternotify(XEvent *ev)
 {
 int x=ev->xcrossing.x;
 int y=ev->xcrossing.y;
 if(gtmom()->is_menu && grabber && !greedy && x>=0 && y>=0 && x<gtw() && y<gth())
  {
  zapto((Widget *)gtmom());
  stfocus(this);
  }
 return 0;
 }

Toggle::Toggle()
 {
 Icon *i;
 fn=0;
 state=0;
 dispon=0;
 dispoff=0;
 stborder(new Tborder());
 i=new Icon();
  i->stborder(new Shadow());
  i->stwidth(12);
  i->stheight(12);
  i->st(piccheck);
 stdispon(i);
 i=new Icon();
  i->stborder(new Shadow());
  i->stwidth(12);
  i->stheight(12);
 stdispoff(i);
 stfocusable();
 }

Toggle::~Toggle()
  {
  if (fn) fn->cancel();
  }

Mtoggle::Mtoggle()
 {
 Icon *i;
 fn=0;
 state=0;
 dispon=0;
 dispoff=0;
 stborder(new Mborder());
 i=new Icon();
  i->st(picarc);
 stdispon(i);
 stfocusable();
 }

Mtoggle::~Mtoggle()
  {
  if (fn) fn->cancel();
  }
