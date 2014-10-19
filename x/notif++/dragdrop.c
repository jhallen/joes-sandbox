/* Drag & drop thingy 
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

extern XContext wins;

static int draguser(b,key)
Drag *b;
 {
 switch(key)
  {
  case XK_Press1:
   {
   if(!(gtflg(b)&flgactivated)) return 0;
   if(b->help && b->help->funcs->deactivate)
    b->help->funcs->deactivate(b->help);
   grabunwind();
   return 0;
   }
  case XK_Release1:
   {
   int yofst, xofst, xtmp, ytmp;
   Wind *x, *y;
   calcpointer();		/* Set new pointer destination */
   if(XFindContext(dsp,ev.xany.window,wins,((XPointer *)&x))) return 0;
   xofst=yofst=xtmp=ytmp=0;
   for(y=x;y && y!=root;y=gtmom(y))
    if(y->target && y->target->funcs->drop) { x=y; xofst=xtmp; yofst=ytmp; break; }
    else xtmp+=gtx(y), ytmp+=gty(y);
   if(x->target && x->target->funcs->drop)
    x->target->funcs->drop(x->target,gtmom(b),ev.xbutton.x+xofst,ev.xbutton.y+yofst);
   return 0;
   }
  }
 return 1;
 }

struct dragfuncs dragfuncs;

Drag *mkdrag(d)
Drag *d;
 {
 mkWidget(d);
 if(!dragfuncs.on)
  {
  mcpy(&dragfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  dragfuncs.user=draguser;
  }
 d->funcs= &dragfuncs;
 stborder(d,mk(Iborder));
 vcenter(d);
 hcenter(d);
 return d;
 }
