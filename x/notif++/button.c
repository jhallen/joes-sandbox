/* Button widget 
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
#include <typeinfo>

void Button::buttonpress()
  {
  if(help)
    help->deactivate();
  if(rept==0) rept=1;		/* User pressed button */
  else if(rept==1) rept=2;		/* Button started repeating */
  if(focusable_flag) stfocus(this);
  trigger();
  grabunwind();
  if(fn) fn->cont(fn);
  }

void Button::buttonrept()
  {
  if(rept) buttonpress();
  }

void Button::buttonrelease()
  {
  rept=0;
  cancel(tevent);
  tevent=0;
  }

int Button::user(unsigned key,unsigned state,int x,int y,Time t,Widget *org)
  {
  if(!activated_flag) return 1;
  switch(key)
    {
    case XK_Press1:
      buttonpress();
      return 0;
    case XK_Release1:
      buttonrelease();
      return 0;
    case 13: case 10: case 32:
      buttonpress();
      if(grabber)
        {
        grabber->user(XK_Release1,0,-1,-1,0,0);
        }
      else buttonrelease();
      return 0;
    }
  return 1;
  }

void Button::stfn(Fn_0<void> *c)
  {
  reset();
  fn=c;
  if(rept==1 && delay)
    /* After button was first pressed */
    submit(delay,tevent=new Mfn0_0<void,Button>(&Button::buttonrept,this));
  else if(rept==2 && rate)
    /* After button repeated */
    submit(rate,tevent=new Mfn0_0<void,Button>(&Button::buttonrept,this));
  }

Fn_0<void> *Button::gtfn()
  {
  return fn;
  }

void Button::stdelay(int n)
  {
  delay=n;
  }

void Button::strate(int n)
  {
  rate=n;
  }

int Button::enternotify(XEvent *ev)
  {
  int x=ev->xcrossing.x;
  int y=ev->xcrossing.y;
  if(gtmom()->is_menu && grabber && !greedy &&
      x>=0 && y>=0 && x<gtw() && y<gth())
    {
    zapto((Widget *)gtmom());
    stfocus(this);
    }
  return 0;
  }

void Button::sttext(char *s)
  {
  Text *t=new Text();
    t->st(s);
  add(t);
  }

void Button::sticon(int *s)
  {
  Icon *t=new Icon();
    t->st(s);
  add(t);
  }

Button::~Button()
  {
  rept=0;
  cancel(tevent);
  if(fn) fn->cancel();
  }

Button::Button()
  {
  fn=0;
  rept= 0;
  rate=0;
  delay=0;
  tevent=0;
  stborder(new Iborder());
  focusable_flag = 1;
  vcenter();
  hcenter();
  }

Mbutton::Mbutton()
  {
  stborder(new Mborder());
  ledge(0);
  }
