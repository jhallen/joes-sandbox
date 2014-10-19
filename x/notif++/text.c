/* Text field widget
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

/* Text widget */

void Text::st(char *t)
  {
  text=t;
  redraw(this);
  }

char *Text::gt()
  {
  return text;
  }

int Text::select()
  {
  if(!state)
    {
    stbknd(stdfgnd);
    stfgnd(getgc("8x13","white"));
    state=1;
    redraw(this);
    }
  return 0;
  }

int Text::unselect()
  {
  if(state)
    {
    stbknd(stdbknd);
    stfgnd(stdfgnd);
    state=0;
    redraw(this);
    }
  return 0;
  }

int Text::expose(XEvent *ev)
  {
  drawclr(this);
  if(text)
    {
    if(activated_flag) drawtxt(this,gtfgnd(),xpos,ypos,text);
    else drawtxt(this,getgc("8x13","Dim Grey"),xpos,ypos,text);
    }
  return 0;
  }

int Text::activate()
  {
  Widget::activate();
  redraw(this);
  return 0;
  }

int Text::deactivate()
  {
  Widget::deactivate();
  redraw(this);
  return 0;
  }

int Text::gtheight()
  {
  if(ypos== -1)
    {
    if(!ckh()) stheight(txth(gtfgnd()));
    ypos = Lith::gtheight() / 2 - txth(gtfgnd()) / 2 + txtb(gtfgnd());
    }
  return Lith::gtheight();
  }

int Text::gtwidth()
  {
  if(xpos== -1)
    {
    if(!ckw())
      if(text)
        stwidth(txtw(gtfgnd(),text) + txtw(gtfgnd(),"M"));
      else
        stwidth(64);
    if(text)
      xpos=Lith::gtwidth() / 2 - txtw(gtfgnd(), text) / 2;
    else
      xpos=txtw(gtfgnd(),"M") / 2;
    }
  return Lith::gtwidth();
  }

Text::Text()
  {
  text=0;
  xpos= -1;
  ypos= -1;
  state=0;
  }
