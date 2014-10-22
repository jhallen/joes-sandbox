/* Option widget 
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

struct optionfuncs optionfuncs;

Option *mkOption(o)
Option *o;
 {
 mkWidget(o);
 if(!optionfuncs.on)
  {
  mcpy(&optionfuncs,&widgetfuncs,sizeof(widgetfuncs));
  }
 o->funcs= &optionfuncs;
 o->item=0;
 o->button=0;
 o->menu=0;
 o->container=0;
 return o;
 }
