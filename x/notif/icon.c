/* Icon widget 
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

static void iconst(t,pic)
Icon *t;
int *pic;
 {
 t->pic=pic;
 }

static int icondraw(t)
Icon *t;
 {
 drawclr(t);
 if(t->pic) drawpic(t,0,0,gtwidth(t),gtheight(t),gtfgnd(t),t->pic);
 return 0;
 }

struct iconfuncs iconfuncs;

Icon *mkIcon(t)
Icon *t;
 {
 mkWidget(t);
 if(!iconfuncs.on)
  {
  mcpy(&iconfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  iconfuncs.st=iconst;
  iconfuncs.expose=icondraw;
  }
 t->funcs= &iconfuncs;
 t->pic=0;
 stwidth(t,16);
 stheight(t,16);
 return t;
 }
