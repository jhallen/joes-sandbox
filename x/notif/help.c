/* Pop-up help widget
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

static int helpflg=0;		/* Set if help is on */
static TASK thelp[1];		/* Timer for turning help on */
static Widget *helpwin=0;	/* If help is on: current help window */

void helpzap()
 {
 if(helpflg)
  {
  if(helpwin) clmom(helpwin), helpwin=0;
  helpflg=0;
  }
 else cancel(thelp);
 }

static void disphelp(b)
Help *b;
 {
 Widget *top=gttop(b->target);
 int xpos, ypos;
 Lith *d;
 helpflg=1;
 helpwin=b->help;
 xpos=5; ypos=gth(b->target);
 abspos(gtmain(b->target),&xpos,&ypos);
 stx(helpwin,xpos);
 sty(helpwin,ypos);
 stflg(helpwin,gtflg(helpwin)|flgoverride);
 add(root,helpwin);
 }

static int dohelp(stat,b)
Help *b;
 {
 if(!stat)
  {
  disphelp(b);
  }
 return stat;
 }

static void helpenter(b)
Help *b;
 {
 if(b->help && !thelp->state && !helpwin)
  if(!helpflg) submit(750000,fn1(thelp,dohelp,b));
  else disphelp(b);
 }

static void *helpchk(w,arg)
Widget *w;
void *arg;
 {
 if(w->help && w->help->help) return w;
 else return 0;
 }

static void helpleave(b,x,y)
Help *b;
 {
 if(helpflg)
  {
  if(helpwin) clmom(helpwin), helpwin=0;
  if(!xyapply(gttop(b->target),gtx(b->target)+x,gty(b->target)+y,helpchk,NULL))
   helpflg=0;
  }
 else cancel(thelp);
 }

static void helpdeactivate(b)
Help *b;
 {
 if(helpwin) clmom(helpwin), helpwin=0;
 helpflg=0;
 cancel(thelp);
 }

static void helprm(b)
Help *b;
 {
 helpdeactivate(b);
 if(b->help) rm(b->help);
 free(b);
 }

static void helpsthelp(b,s)
Help *b;
Widget *s;
 {
 if(b->help) rm(b->help);
 b->help=s;
 }

static void helpsttarget(b,t)
Help *b;
Widget *t;
 {
 b->target=t;
 }

struct helpfuncs helpfuncs=
 {
 helprm, helpdeactivate, helpenter, helpleave, helpsthelp, helpsttarget
 };

Help *mkHelp(f)
Help *f;
 {
 f->funcs= &helpfuncs;
 f->help=0;
 f->target=0;
 return f;
 }

/* Convenience functions */

void sthelptext(w,t)
Widget *w;
char *t;
 {
 Text *ht;
 sthelpmgr(w,mk(Help));
 ht=mk(Text);
  stbknd(ht,getgc("8x13","yellow"));
  stborder(ht,mk(Shadow));
  st(ht,t);
 sthelp(w,ht);
 }
