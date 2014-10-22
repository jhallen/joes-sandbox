/* Menu 
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

struct menufuncs menufuncs;

static void menuon(m)
Menu *m;
 {
 (lithfuncs.on)(m);
 if(gtflg(m)&flgon)
  {
  stfocus(nth(m,0));
  grab(m,greedy);
  }
 }

static int menuungrab(m)
Menu *m;
 {
 disable(m);
 return 0;
 }

int umenupress1(stat,m,junk,key,state,x,y,org)
Menu *m;
 {
 if(!stat)
  {
  if(x<0 || x>=gtwidth(m) || y<0 || y>=gtheight(m))
   { /* Mouse press is out of our window */
   popgrab();
   }
  else
   { /* Mouse press is in our window */
   calcpointer();
   /* Event is for one of our kids maybe */
   doevent(gtmain(m),&ev);
   }
  }
 return stat;
 }

int umenurelease1(stat,m,junk,key,state,x,y,org)
Menu *m;
 {
 if(!stat)
  {
/*  printf("Menu release %d,%d greedy=%d\n",x,y,greedy); */
  greedygrab(1);
  stfocus(nth(m,0));
  if(x>=0 && x<gtw(m) && y>=0 && y<gth(m))
   {
   ev.type=ButtonPress;
   calcpointer();
   doevent(gtmain(m),&ev);
   calcpointer();
   ev.type=ButtonRelease;
   doevent(gtmain(m),&ev);
   }
  }
 return stat;
 }

int umenuleft(stat,m,junk,key,state,x,y,org)
Menu *m;
 {
 if(!stat)
  {
  grabunwind();
  if(whofocus->funcs->user)
   {
   whofocus->funcs->user(whofocus,XK_Left,0,0,0,0,0);
   if(whofocus->trap && whofocus->trap->funcs==&cascadefuncs)
    whofocus->funcs->user(whofocus,32,0,0,0,0,0);
   }
  }
 return stat;
 }

int umenuright(stat,m,junk,key,state,x,y,org)
Menu *m;
 {
 if(!stat)
  {
  grabunwind();
  if(whofocus->funcs->user)
   {
   whofocus->funcs->user(whofocus,XK_Right,0,0,0,0,0);
   if(whofocus->trap && whofocus->trap->funcs==&cascadefuncs)
    whofocus->funcs->user(whofocus,32,0,0,0,0,0);
   }
  }
 return stat;
 }

int umenuesc(stat,m,junk,key,state,x,y,org)
Menu *m;
 {
 if(!stat)
  {
  zapgrab();
  zapgrab();
  }
 return stat;
 }

Menu *mkMenu(m)
Menu *m;
 {
 mkWidget(m);
 if(!menufuncs.on)
  {
  mcpy(&menufuncs,&widgetfuncs,sizeof(widgetfuncs));
  menufuncs.on=menuon;
  menufuncs.ungrab=menuungrab;
  menufuncs.kmap=mkkmap(NULL,NULL,NULL);
  kcpy(menufuncs.kmap,widgetfuncs.kmap);
  kadd(menufuncs.kmap,NULL,"Left",umenuleft,NULL);
  kadd(menufuncs.kmap,NULL,"Right",umenuright,NULL);
  kadd(menufuncs.kmap,NULL,"^[",umenuesc,NULL);
  kadd(menufuncs.kmap,NULL,"Press1",umenupress1,NULL);
  kadd(menufuncs.kmap,NULL,"Release1",umenurelease1,NULL);
  }
 m->funcs= &menufuncs;
 stkmap(m,menufuncs.kmap);
 stborder(m,mk(Shadow));
 stflg(m,gtflg(m)|flgoverride);
 ttob(m,NULL,0);
 ledge(m,NULL,0);
 auxredge(m,NULL,0);
 disable(m);
 return m;
 }
