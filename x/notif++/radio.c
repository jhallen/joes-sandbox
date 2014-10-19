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

int Radio::gt()
  {
  return state;
  }

void Radio::st(int n)
  {
  List<Toggle *>::Iterator l;
  int x;
  Toggle *t;
  state=n;
  for(x=0,list.begin(l);list.test(l);list.next(l))
    {
    t=list.indirect(l);
    if(n==x && !t->gt()) t->st(1);
    if(n!=x && t->gt()) t->st(0);
    ++x;
    }
  }

void Radio::stfn(Fn_1<void,int> *c)
  {
  fn=c;
  }

static void radioevt(Radio *r,Toggle *w,int n)
  {
  w->stfn(new Fn2_1<void,Radio *,Toggle *,int>(radioevt,r,w));
  if(!n) w->st(1);
  else
    {
    int x;
    List<Toggle *>::Iterator l;
    Toggle *t;
    for(x=0, r->list.begin(l); r->list.indirect(l) != w; r->list.next(l), ++x);
    r->st(x);
    if (r->fn) r->fn->cont(r->fn,x);
    }
  }

void Radio::groupadd(Toggle *w)
  {
  list.insert(w);
  w->stfn(new Fn2_1<void,Radio *,Toggle *,int>(radioevt,this,w));
  add(w);
  }

Radio::Radio()
  {
  state= -1;
  fn=0;
  ttob(NULL,0);
  ledge(0);
  }

Radio::~Radio()
  {
  if (fn) fn->cancel();
  }
