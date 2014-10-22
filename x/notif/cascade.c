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

static void cascadest(b,m)
Cascade *b;
Menu *m;
 {
 b->m=m;
 }

static int cascadepress(b,x,y)
Cascade *b;
 {
 int xpos, ypos;
 if(b->help && b->help->funcs->deactivate)
  b->help->funcs->deactivate(b->help);

 if(b->state)
  { /* Already pressed */
  if(x<0 || x>=gtwidth(b)+2 || y<0 || y>=gtheight(b)+2)
   { /* Mouse is outside our window */
   popgrab();
   }
  else
   {
   greedygrab(0);
   }
  return 0;
  }

 if(gtflg(b)&flgfocus) stfocus(b);
 if(b->funcs->trigger) b->funcs->trigger(b);

 if(gtmom(b)->funcs!=(struct lithfuncs *)&menufuncs)
  {
  calcpos(gtwin(gtouter(b)),&xpos,&ypos);
  stx(b->m,xpos);
  sty(b->m,ypos+gth(b)+2);
  }
 else
  {
  calcpos(gtwin(gtouter(b)),&xpos,&ypos);
  stx(b->m,xpos+gtw(b)+2);
  sty(b->m,ypos);
  }
 grab(b,0);
 b->state=1;
 enable(b->m);
 }

static int cascaderelease(b)
Cascade *b;
 {
 b->state=0;
 zapgrab();
 greedygrab(1);
 }

static int cascadeuser(b,key,state,x,y)
Cascade *b;
 {
 if(!(gtflg(b)&flgactivated)) return 0;
 switch(key)
  {
  case XK_Press1:
   cascadepress(b,x,y);
   return 0;
  case XK_Release1:
   cascaderelease(b);
   return 0;
  case 13: case 10: case 32:
   cascadepress(b,0,0);
   if(grabber)
    {
    if(grabber->funcs->user)
     (grabber->funcs->user)(grabber,XK_Release1,0,-1,-1,0,0);
    }
   else cascaderelease(b);
   return 0;
  }
 return 1;
 }

static int cascadeungrab(b)
Cascade *b;
 {
 if(b->funcs->reset) b->funcs->reset(b);
 b->state=0;
 return 0;
 }

static void cascadeenter(b,ev)
Cascade *b;
XEvent *ev;
 {
 int x=ev->xcrossing.x;
 int y=ev->xcrossing.y;
 if(/* gtmom(b)->funcs==&menufuncs && */ grabber && !greedy &&
    x>=0 && y>=0 && x<gtw(b) && y<=gth(b) && !b->state)
  {
  stfocus(b);
  zapto(gtmom(b));
  cascadepress(b,0,0);
  }
 }

struct cascadefuncs cascadefuncs;

Cascade *mkCascade(b)
Cascade *b;
 {
 mkButton(b);
 if(!cascadefuncs.on)
  {
  mcpy(&cascadefuncs,&buttonfuncs,sizeof(buttonfuncs));
  cascadefuncs.st=cascadest;
  cascadefuncs.user=cascadeuser;
  cascadefuncs.ungrab=cascadeungrab;
  cascadefuncs.enternotify=cascadeenter;
  }
 b->funcs= &cascadefuncs;
 b->m=0;
 b->state=0;
 return b;
 }

Cascade *mkMcascade(b)
Cascade *b;
 {
 Icon *i;
 mkCascade(b);
 stborder(b,mk(Mborder));
 redge(b,0);
 i=mk(Icon);
  st(i,picrtarw);
 add(b,i);
 ledge(b,0);
 auxrtol(b,i,0);
 return b;
 }

Cascade *mkMbcascade(b)
Cascade *b;
 {
 mkCascade(b);
 stborder(b,mk(Mborder));
 return b;
 }
