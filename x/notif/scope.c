/* 'scope widget 
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

static void scopest(v,data)
Scope *v;
unsigned char *data;
 {
 v->data=data;
 redraw(v);
 }

static int scopedraw(v,ev)
Scope *v;
XExposeEvent *ev;
 {
 int x;
 int flg=0;
 int height=gth(v);
 if(v->data)
  for(x=ev->x;x!=ev->x+ev->width;++x)
   {
   if(flg) drawline(v,gtfgnd(v),x-1,height-v->data[x-1]-1,x,height-v->data[x]-1);
   drawdot(v,gtfgnd(v),x,height-v->data[x]-1);
   flg=1;
   }
 return 0;
 }

struct scopefuncs scopefuncs;

Scope *mkScope(v)
Scope *v;
 {
 int z;
 mkWidget(v);
 if(!scopefuncs.on)
  {
  mcpy(&scopefuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  scopefuncs.expose=scopedraw;
  scopefuncs.st=scopest;
  }
 v->funcs= &scopefuncs;
 v->data=0;
 stfgnd(v,getgc("8x13","red"));
 stw(v,32768);
 sth(v,256);
 return v;
 }
