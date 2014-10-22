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

static int gsepdraw(t)
Gsep *t;
 {
 int q;
 drawclr(t);
 if(gtwidth(t)>gtheight(t))
  {
  int wid=gtwidth(t)-1;
  int y=gtheight(t)/2-1;
  for(q=0;q!=t->thick;++q)
   drawline(t,t->bgc,0,y-q,wid,y-q);
  for(q=0;q!=t->thick;++q)
   drawline(t,t->tgc,0,y+q+1,wid,y+q+1);
  }
 else
  {
  int x=gtwidth(t)/2-1;
  int hgt=gtheight(t)-1;
  for(q=0;q!=t->thick;++q)
   drawline(t,t->bgc,x-q,0,x-q,hgt);
  for(q=0;q!=t->thick;++q)
   drawline(t,t->tgc,x+q+1,0,x+q+1,hgt);
  }
 return 0;
 }

struct gsepfuncs gsepfuncs;

Gsep *mkGsep(t)
Gsep *t;
 {
 mkWidget(t);
 if(!gsepfuncs.on)
  {
  mcpy(&gsepfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  gsepfuncs.expose=gsepdraw;
  }
 t->funcs= &gsepfuncs;
 t->thick=1;
 t->tgc=stdshadowtop;
 t->bgc=stdshadowbot;
 stwidth(t,6);
 stheight(t,6);
 return t;
 }
