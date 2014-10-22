/* Button widget 
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

static void buttonpress(b)
Button *b;
 {
 if(b->help && b->help->funcs->deactivate)
  b->help->funcs->deactivate(b->help);
 if(b->rept==0) b->rept=1;		/* User pressed button */
 else if(b->rept==1) b->rept=2;		/* Button started repeating */
 if(gtflg(b)&flgfocus) stfocus(b);
 if(b->funcs->trigger) b->funcs->trigger(b);
 grabunwind();
 cont0(b->fn); b->fn=0;
 }

static void buttonrept(stat,b)
Button *b;
 {
 if(!stat)
  {
  if(b->rept) buttonpress(b);
  }
 }

static void buttonrelease(b)
Button *b;
 {
 b->rept=0;
 cancel(b->tevent);
 }

static int buttonuser(b,key)
Button *b;
 {
 if(!(gtflg(b)&flgactivated)) return 1;
 switch(key)
  {
  case XK_Press1:
   buttonpress(b);
   return 0;
  case XK_Release1:
   buttonrelease(b);
   return 0;
  case 13: case 10: case 32:
   buttonpress(b);
   if(grabber)
    {
    if(grabber->funcs->user)
     grabber->funcs->user(grabber,XK_Release1,0,-1,-1,0,0);
    }
   else buttonrelease(b);
   return 0;
  }
 return 1;
 }

static void buttonstfn(b,c)
Button *b;
TASK *c;
 {
 if(b->funcs->reset) b->funcs->reset(b);
 b->fn=c;
 if(b->rept==1 && b->delay)
  /* After button was first pressed */
  submit(b->delay,fn1(b->tevent,buttonrept,b));
 else if(b->rept==2 && b->rate)
  /* After button repeated */
  submit(b->rate,fn1(b->tevent,buttonrept,b));
 }

static TASK *buttongtfn(b)
Button *b;
 {
 return b->fn;
 }

static void buttonstdelay(b,n)
Button *b;
 {
 b->delay=n;
 }

static void buttonstrate(b,n)
Button *b;
 {
 b->rate=n;
 }

static void buttonenter(b,ev)
Button *b;
XEvent *ev;
 {
 int x=ev->xcrossing.x;
 int y=ev->xcrossing.y;
 if(gtmom(b)->funcs==&menufuncs && grabber && !greedy &&
    x>=0 && y>=0 && x<gtw(b) && y<gth(b))
  {
  zapto(gtmom(b));
  stfocus(b);
  }
 }

static void buttonsttext(b,s)
Button *b;
char *s;
 {
 Text *t=mk(Text);
 st(t,s);
 add(b,t);
 }

static void buttonsticon(b,s)
Button *b;
int *s;
 {
 Icon *t=mk(Icon);
 st(t,s);
 add(b,t);
 }

static void buttonrm(b)
Button *b;
 {
 b->rept=0;
 cancel(b->tevent);
 cancel(b->fn); b->fn=0;
 (widgetfuncs.rm)(b);
 }

struct buttonfuncs buttonfuncs;

Button *mkButton(b)
Button *b;
 {
 mkWidget(b);
 if(!buttonfuncs.on)
  {
  mcpy(&buttonfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  buttonfuncs.user=buttonuser;
  buttonfuncs.stfn=buttonstfn;
  buttonfuncs.stdelay=buttonstdelay;
  buttonfuncs.strate=buttonstrate;
  buttonfuncs.enternotify=buttonenter;
  buttonfuncs.sttext=buttonsttext;
  buttonfuncs.sticon=buttonsticon;
  buttonfuncs.rm=buttonrm;
  }
 b->funcs= &buttonfuncs;
 b->fn=0;
 b->rept= 0;
 b->rate=0;
 b->delay=0;
 iztask(b->tevent);
 stborder(b,mk(Iborder));
 stflg(b,gtflg(b)|flgfocus);
 vcenter(b);
 hcenter(b);
 return b;
 }

Button *mkMbutton(b)
Button *b;
 {
 mkButton(b);
 stborder(b,mk(Mborder));
 ledge(b,0);
 return b;
 }
