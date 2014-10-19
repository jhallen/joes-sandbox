/* Cascading menus button 
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

void Cascade::st(Menu *new_m)
  {
  m=new_m;
  }

void Cascade::press(int x,int y)
  {
  int xpos, ypos;
  if(help)
    help->deactivate();

  if(state)
    { /* Already pressed */
    if(x<0 || x>=gtwidth()+2 || y<0 || y>=gtheight()+2)
      { /* Mouse is outside our window */
      popgrab();
      }
    else
      {
      greedygrab(0);
      }
    return;
    }


  trigger();

  if(!gtmom()->is_menu)
    {
    calcpos(gtouter()->gtwin(),&xpos,&ypos);
    m->stx(xpos);
    m->sty(ypos+gth()+2);
    }
  else
    {
    calcpos(gtouter()->gtwin(),&xpos,&ypos);
    m->stx(xpos+gtw()+2);
    m->sty(ypos);
    }
  grab(this,0);
  state=1;
  m->enable();
  }

void Cascade::release()
  {
  state=0;
  zapgrab();
  greedygrab(1);
  }

int Cascade::user(unsigned key,unsigned state,int x,int y,Time t,Widget *org)
  {
  if (!activated_flag) return 0;
  switch(key)
    {
    case XK_Press1:
      press(x,y);
      return 0;
    case XK_Release1:
      release();
      return 0;
    case 13: case 10: case 32:
      press(0,0);
      if(grabber)
        {
        (grabber->user)(XK_Release1,0,-1,-1,0,0);
        }
     else release();
     return 0;
    }
  return 1;
  }

void Cascade::ungrab()
  {
  reset();
  state=0;
  }

int Cascade::enternotify(XEvent *ev)
 {
 int x=ev->xcrossing.x;
 int y=ev->xcrossing.y;
 if(/* gtmom(b)->funcs==&menufuncs && */ grabber && !greedy &&
    x>=0 && y>=0 && x<gtw() && y<=gth() && !state)
   {
   stfocus(this);
   zapto((Widget *)gtmom());
   press(0,0);
   }
 return 0;
 }

Cascade::Cascade()
  {
  m=0;
  state=0;
  is_cascade=1;
  }

Mcascade::Mcascade()
  {
  Icon *i;
  stborder(new Mborder());
  redge(0);
  i=new Icon();
   i->st(picrtarw);
  add(i);
  ledge(0);
  auxrtol(i,0);
  }

Mbcascade::Mbcascade()
  {
  stborder(new Mborder());
  }
