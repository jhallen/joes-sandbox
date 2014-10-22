/* Border windows 
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

/* Simple border window */

static void borderstmargins(w,a,b,c,d)
Border *w;
 {
 w->lmargin=a;
 w->rmargin=b;
 w->tmargin=c;
 w->bmargin=d;
 }

static int bordergtlmargin(w)
Border *w;
 {
 return w->lmargin;
 }

static int bordergtrmargin(w)
Border *w;
 {
 return w->rmargin;
 }

static int bordergttmargin(w)
Border *w;
 {
 return w->tmargin;
 }

static int bordergtbmargin(w)
Border *w;
 {
 return w->bmargin;
 }

static int bordergtvisual(w)
Border *w;
 {
 return w->visual;
 }

static void borderstvisual(w,n)
Border *w;
 {
 w->visual=n;
 }

static int borderenter(w,ev)
Border *w;
XEvent *ev;
 {
 int x=ev->xcrossing.x;
 int y=ev->xcrossing.y;
 int mode=ev->xcrossing.mode;
 int detail=ev->xcrossing.detail;
 if(w->target->help && w->target->help->funcs->enter && mode!=NotifyUngrab)
  w->target->help->funcs->enter(w->target->help,x,y,mode,detail);
 doevent(w,ev);
 return 0;
 }

static int borderleave(w,ev)
Border *w;
XEvent *ev;
 {
 int x=ev->xcrossing.x;
 int y=ev->xcrossing.y;
 int mode=ev->xcrossing.mode;
 int detail=ev->xcrossing.detail;
 if(w->target->help && w->target->help->funcs->leave && detail!=NotifyInferior)
  w->target->help->funcs->leave(w->target->help,x,y,mode,detail);
 doevent(w,ev);
 return 0;
 }

struct borderfuncs borderfuncs;

Border *mkBorder(w)
Border *w;
 {
 mkWind(w);
 if(!borderfuncs.on)
  {
  mcpy(&borderfuncs,&windfuncs,sizeof(struct windfuncs));
  borderfuncs.gtlmargin=bordergtlmargin;
  borderfuncs.gtrmargin=bordergtrmargin;
  borderfuncs.gttmargin=bordergttmargin;
  borderfuncs.gtbmargin=bordergtbmargin;
  borderfuncs.stmargins=borderstmargins;
  borderfuncs.stvisual=borderstvisual;
  borderfuncs.gtvisual=bordergtvisual;
  borderfuncs.buttonpress=0;
  borderfuncs.buttonrelease=0;
  borderfuncs.keypress=0;
  borderfuncs.motionnotify=0;
  borderfuncs.expose=0;
  borderfuncs.enternotify=borderenter;
  borderfuncs.leavenotify=borderleave;
  }
 w->funcs= &borderfuncs;
 w->target=0;
 w->visual=0;
 stmargins(w,2,2,2,2);
 stbknd(w,stdborderbknd);
 return w;
 }

/* Shadow window */

static int shadowdraw(w)
Shadow *w;
 {
 drawclr(w);

 if(w->raised)
  draw3dbox(w,w->lmargin-w->raised,w->tmargin-w->raised,
            gtw(w)-w->lmargin-w->rmargin+2*w->raised,
            gth(w)-w->tmargin-w->bmargin+2*w->raised,
            w->raised,w->tgc,w->bgc);

 if(w->lowered)
  draw3dbox(w,w->outline_margin,w->outline_margin,
            gtw(w)-2*w->outline_margin,gth(w)-2*w->outline_margin,
            w->lowered,w->bgc,w->tgc);

 if(w->outlined)
  drawbox(w,0,0,gtw(w),gth(w),w->outlined,w->ogc);

 return 0;
 }

static void shadowstfixed(w,smargin,omargin,tgc,bgc,ogc)
Shadow *w;
GC tgc, bgc, ogc;
 {
 int margin=smargin+omargin;

 w->shadow_margin=smargin;
 w->outline_margin=omargin;
 w->tgc=tgc;
 w->bgc=bgc;
 w->ogc=ogc;

 stmargins(w,margin,margin,margin,margin);
 }

static void shadowststyle(w,raised,lowered,outlined)
Shadow *w;
 {
 int flg=0;

 if(w->raised!=raised) flg=1;
 if(w->lowered!=lowered) flg=1;
 if(w->outlined!=outlined) flg=1;

 w->raised=raised;
 w->lowered=lowered;
 w->outlined=outlined;

 if(flg) redraw(w);
 }

struct shadowfuncs shadowfuncs;

Shadow *mkShadow(w)
Shadow *w;
 {
 mkBorder(w);
 if(!shadowfuncs.on)
  {
  mcpy(&shadowfuncs,&borderfuncs,sizeof(struct borderfuncs));
  shadowfuncs.expose=shadowdraw;
  shadowfuncs.ststyle=shadowststyle;
  shadowfuncs.stfixed=shadowstfixed;
  }
 w->funcs= &shadowfuncs;
 stfixed(w,2,0,stdshadowtop,stdshadowbot,stdoutline);
 ststyle(w,2,0,0);
 stbknd(w,stdshadowbknd);
 return w;
 }

/* Intelligent border for buttons */

static void iborderststate(w,n)
Iborder *w;
 {
 int outlined=0;
 int raised=2;
 int lowered=0;

 w->state=n;
 if(n&vpress) raised=0;
 if(n&vfocus) outlined=2;
 if(n&vfocusrtn) lowered=1;
 
 ststyle(w,raised,lowered,outlined);
 }

static int iborderfocus(w)
Iborder *w;
 {
 if(gtvisual(w)&vfocus) ststate(w,w->state|vfocus);
 }

static int iborderunfocus(w)
Iborder *w;
 {
 ststate(w,w->state&~vfocus);
 }

static int ibordertrigger(w)
Iborder *w;
 {
 if(gtvisual(w)&vpress) ststate(w,w->state|vpress);
 }

static int iborderreset(w)
Iborder *w;
 {
 ststate(w,w->state&~vpress);
 }

static int iborderselect(w)
Iborder *w;
 {
 if(gtvisual(w)&vselect) ststate(w,w->state|vselect);
 }

static int iborderunselect(w)
Iborder *w;
 {
 ststate(w,w->state&~vselect);
 }

static int iborderfocusrtn(w)
Iborder *w;
 {
 if(gtvisual(w)&vfocusrtn) ststate(w,w->state|vfocusrtn);
 }

static int iborderunfocusrtn(w)
Iborder *w;
 {
 ststate(w,w->state&~vfocusrtn);
 }

static int iborderactivate(w)
Iborder *w;
 {
 if(gtvisual(w)&vactive) ststate(w,w->state|vactive);
 }

static int iborderdeactivate(w)
Iborder *w;
 {
 ststate(w,w->state&~vactive);
 }

static int iborderenter(w,ev)
Iborder *w;
XEvent *ev;
 {
 if(gtvisual(w)&venter) ststate(w,w->state|venter);
 (borderfuncs.enternotify)(w,ev);
 }

static int iborderleave(w,ev)
Iborder *w;
XEvent *ev;
 {
 ststate(w,w->state&~venter);
 (borderfuncs.leavenotify)(w,ev);
 }

struct iborderfuncs iborderfuncs;

Iborder *mkIborder(w)
Iborder *w;
 {
 mkShadow(w);
 if(!iborderfuncs.on)
  {
  mcpy(&iborderfuncs,&shadowfuncs,sizeof(shadowfuncs));
  iborderfuncs.focus=iborderfocus;
  iborderfuncs.unfocus=iborderunfocus;
  iborderfuncs.trigger=ibordertrigger;
  iborderfuncs.reset=iborderreset;
  iborderfuncs.select=iborderselect;
  iborderfuncs.unselect=iborderunselect;
  iborderfuncs.ststate=iborderststate;
  iborderfuncs.enternotify=iborderenter;
  iborderfuncs.leavenotify=iborderleave;
  iborderfuncs.focusrtn=iborderfocusrtn;
  iborderfuncs.unfocusrtn=iborderunfocusrtn;
  iborderfuncs.activate=iborderactivate;
  iborderfuncs.deactivate=iborderdeactivate;
  }
 w->funcs= &iborderfuncs;
 w->state=0;
 stfixed(w,5,3,stdshadowtop,stdshadowbot,stdoutline);
 stvisual(w,vfocus|vpress|vselect|vfocusrtn);
 return w;
 }

/* Iborder, but with no focus effects.  For keypad pushbuttons */

Iborder *mkIkborder(w)
Iborder *w;
 {
 w=mkIborder(w);
 stfixed(w,2,0,stdshadowtop,stdshadowbot,stdoutline);
 stvisual(w,vpress|vselect);
 return w;
 }

/* Intelligent border for edit window */

static void eborderststate(w,n)
Eborder *w;
 {
 int outlined=0;
 int raised=0;
 int lowered=2;

 w->state=n;
 if(n&vfocus) outlined=2;
 
 ststyle(w,raised,lowered,outlined);
 }

struct eborderfuncs eborderfuncs;

Eborder *mkEborder(w)
Eborder *w;
 {
 mkIborder(w);
 if(!eborderfuncs.on)
  {
  mcpy(&eborderfuncs,&iborderfuncs,sizeof(iborderfuncs));
  eborderfuncs.ststate=eborderststate;
  }
 w->funcs= &eborderfuncs;
 stfixed(w,2,6,stdshadowtop,stdshadowbot,stdoutline);
 ststyle(w,0,2,0);
 return w;
 }

/* Eborder, but with no focus effects.  For touch screens */

Eborder *mkEkborder(w)
Eborder *w;
 {
 mkEborder(w);
 stfixed(w,2,0,stdshadowtop,stdshadowbot,stdoutline);
 ststyle(w,0,2,0);
 stvisual(w,vpress|vselect);
 return w;
 }

/* Intelligent border for menu buttons */

static void mborderststate(w,n)
Mborder *w;
 {
 int outlined=0;
 int raised=0;
 int lowered=0;

 w->state=n;
 if(n&vfocus) raised=2;
 if(n&vselect) raised=2;
 
 ststyle(w,raised,lowered,outlined);
 }

struct mborderfuncs mborderfuncs;

Mborder *mkMborder(w)
Mborder *w;
 {
 mkIborder(w);
 if(!mborderfuncs.on)
  {
  mcpy(&mborderfuncs,&iborderfuncs,sizeof(iborderfuncs));
  mborderfuncs.ststate=mborderststate;
  }
 w->funcs= &mborderfuncs;
 stfixed(w,2,0,stdshadowtop,stdshadowbot,stdoutline);
 ststyle(w,0,0,0);
 return w;
 }

/* Intelligent border for toggle buttons */

static void tborderststate(w,n)
Tborder *w;
 {
 int outlined=0;
 int raised=0;
 int lowered=0;

 w->state=n;
 if(n&vfocus) outlined=2;
 if(n&vfocusrtn) lowered=1;
 
 ststyle(w,raised,lowered,outlined);
 }

struct tborderfuncs tborderfuncs;

Tborder *mkTborder(w)
Tborder *w;
 {
 mkIborder(w);
 if(!tborderfuncs.on)
  {
  mcpy(&tborderfuncs,&iborderfuncs,sizeof(iborderfuncs));
  tborderfuncs.ststate=tborderststate;
  }
 w->funcs= &tborderfuncs;
 stfixed(w,5,3,stdshadowtop,stdshadowbot,stdoutline);
 ststyle(w,0,0,0);
 return w;
 }

/* Titled border */

static int titleddraw(w)
Titled *w;
 {
 int amnt=gtlmargin(w);
 draw3dbox(w,amnt/2,amnt/2,gtw(w)-amnt,gth(w)-amnt,1,stdshadowbot,stdshadowtop);
 draw3dbox(w,amnt/2+1,amnt/2+1,gtw(w)-amnt-2,gth(w)-amnt-2,1,stdshadowtop,stdshadowbot);
 return 0;
 }

static void titledst(w,t)
Titled *w;
Widget *t;
 {
 if(w->msg) rm(w->msg);
 w->msg=0;
 if(t)
  {
  w->msg=t;
  stborder(t,NULL);
  stbknd(t,stdtopbknd);
  stx(t,gtlmargin(w));
  sty(t,0);
  add(w,t);
  }
 }

struct titledfuncs titledfuncs;

Titled *mkTitled(w)
Titled *w;
 {
 mkBorder(w);
 if(!titledfuncs.on)
  {
  mcpy(&titledfuncs,&borderfuncs,sizeof(struct borderfuncs));
  titledfuncs.expose=titleddraw;
  titledfuncs.st=titledst;
  }
 w->funcs= &titledfuncs;
 stmargins(w,16,16,16,16);
 stbknd(w,stdtopbknd);
 w->msg=0;
 return w;
 }

/* Dialog titlebar window */

static int titlebardraw(w)
Titlebar *w;
 {
 draw3dbox(w,0,0,
           gtw(w),
           gth(w),
           2,stdshadowtop,stdshadowbot);
 drawfilled(w,getgc("8x13","grey50"),2,2,gtw(w)-4,16);
 if(w->text)
  drawtxt(w,stdshadowtop,2+txtw(gtfgnd(w),"M")/2,2+txtb(gtfgnd(w),"M"),w->text);
 return 0;
 }

static void titlebarst(w,t)
Titlebar *w;
char *t;
 {
 w->text=t;
 }

struct titlebarfuncs titlebarfuncs;

Titlebar *mkTitlebar(w)
Titlebar *w;
 {
 mkBorder(w);
 if(!titlebarfuncs.on)
  {
  mcpy(&titlebarfuncs,&borderfuncs,sizeof(struct borderfuncs));
  titlebarfuncs.expose=titlebardraw;
  titlebarfuncs.st=titlebarst;
  }
 w->funcs= &titlebarfuncs;
 stmargins(w,4,4,16+4,4);
 stbknd(w,stdtopbknd);
 w->text=0;
 return w;
 }
