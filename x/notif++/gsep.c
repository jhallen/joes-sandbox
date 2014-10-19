/* Graphical separator for menus 
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

int Gsep::expose(XEvent *ev)
  {
  int q;
  drawclr(this);
  if(gtwidth()>gtheight())
    {
    int wid=gtwidth()-1;
    int y=gtheight()/2-1;
    for(q=0;q!=thick;++q)
      drawline(this,bgc,0,y-q,wid,y-q);
    for(q=0;q!=thick;++q)
      drawline(this,tgc,0,y+q+1,wid,y+q+1);
    }
  else
    {
    int x=gtwidth()/2-1;
    int hgt=gtheight()-1;
    for(q=0;q!=thick;++q)
      drawline(this,bgc,x-q,0,x-q,hgt);
    for(q=0;q!=thick;++q)
      drawline(this,tgc,x+q+1,0,x+q+1,hgt);
    }
  return 0;
  }

Gsep::Gsep()
  {
  thick=1;
  tgc=stdshadowtop;
  bgc=stdshadowbot;
  stwidth(6);
  stheight(6);
  }
