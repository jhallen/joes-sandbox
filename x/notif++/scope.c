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

void Scope::st(unsigned char *s)
 {
 data=s;
 redraw(this);
 }

int Scope::expose(XEvent *evv)
 {
 XExposeEvent *ev = &evv->xexpose;
 int x;
 int flg=0;
 int height=gtheight();
 drawline(this,gtfgnd(),ev->x,127,ev->x+ev->width,127);
 for (x = ev->x - ev->x%20;x <ev->x + ev->width; x += 20)
   drawline(this,gtfgnd(),x,127-3,x,127+3);
 if(data)
  for(x=ev->x;x!=ev->x+ev->width;++x)
   {
   if(flg) drawline(this,gtfgnd(),x-1,height-data[x-1]-1,x,height-data[x]-1);
   drawdot(this,gtfgnd(),x,height-data[x]-1);
   flg=1;
   }
 return 0;
 }

Scope::Scope()
 {
 data=0;
 stfgnd(getgc("8x13","red"));
 stw(32768);
 sth(256);
 }

Scope::~Scope()
 {
 }
