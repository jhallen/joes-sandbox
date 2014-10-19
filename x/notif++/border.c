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

void Border::stmargins(int a,int b,int c,int d)
  {
  lmargin=a;
  rmargin=b;
  tmargin=c;
  bmargin=d;
  }

int Border::gtlmargin()
  {
  return lmargin;
  }

int Border::gtrmargin()
  {
  return rmargin;
  }

int Border::gttmargin()
  {
  return tmargin;
  }

int Border::gtbmargin()
  {
  return bmargin;
  }

int Border::gtvisual()
  {
  return visual;
  }

void Border::stvisual(int n)
  {
  visual=n;
  }

int Border::enternotify(XEvent *ev)
  {
  int x=ev->xcrossing.x;
  int y=ev->xcrossing.y;
  int mode=ev->xcrossing.mode;
  int detail=ev->xcrossing.detail;
  if(target->help && mode!=NotifyUngrab)
    target->help->enter(x,y,mode,detail);
  doevent(this,ev);
  return 0;
  }

int Border::leavenotify(XEvent *ev)
  {
  int x=ev->xcrossing.x;
  int y=ev->xcrossing.y;
  int mode=ev->xcrossing.mode;
  int detail=ev->xcrossing.detail;
  if(target->help && detail!=NotifyInferior)
    target->help->leave(x,y,mode,detail);
  doevent(this,ev);
  return 0;
  }

Border::Border()
  {
  target=0;
  visual=0;
  stmargins(2,2,2,2);
  stbknd(stdborderbknd);
  }

/* Shadow window */

int Shadow::expose(XEvent *ev)
  {
  drawclr(this);

  if(raised)
    draw3dbox(this,lmargin-raised,tmargin-raised,
              gtw()-lmargin-rmargin+2*raised,
              gth()-tmargin-bmargin+2*raised,
              raised,tgc,bgc);

  if(lowered)
    draw3dbox(this,outline_margin,outline_margin,
              gtw()-2*outline_margin,gth()-2*outline_margin,
              lowered,bgc,tgc);

  if(outlined)
    drawbox(this,0,0,gtw(),gth(),outlined,ogc);

  return 0;
  }

void Shadow::stfixed(int smargin,int omargin,GC ntgc,GC nbgc,GC nogc)
  {
  int margin=smargin+omargin;

  shadow_margin=smargin;
  outline_margin=omargin;
  tgc=ntgc;
  bgc=nbgc;
  ogc=nogc;

  stmargins(margin,margin,margin,margin);
  }

void Shadow::ststyle(int nraised,int nlowered,int noutlined)
  {
  int flg=0;

  if(raised!=nraised) flg=1;
  if(lowered!=nlowered) flg=1;
  if(outlined!=noutlined) flg=1;

  raised=nraised;
  lowered=nlowered;
  outlined=noutlined;

  if(flg) redraw(this);
  }

Shadow::Shadow()
  {
  stfixed(2,0,stdshadowtop,stdshadowbot,stdoutline);
  ststyle(2,0,0);
  stbknd(stdshadowbknd);
  }

/* Intelligent border for buttons */

void Iborder::ststate(int n)
  {
  int outlined=0;
  int raised=2;
  int lowered=0;

  state=n;
  if(n&vpress) raised=0;
  if(n&vfocus) outlined=2;
  if(n&vfocusrtn) lowered=1;
  
  ststyle(raised,lowered,outlined);
  }

int Iborder::focus()
  {
  if(gtvisual()&vfocus) ststate(state|vfocus);
  return 0;
  }

int Iborder::unfocus()
  {
  ststate(state&~vfocus);
  return 0;
  }

int Iborder::trigger()
  {
  if(gtvisual()&vpress) ststate(state|vpress);
  return 0;
  }

int Iborder::reset()
  {
  ststate(state&~vpress);
  return 0;
  }

int Iborder::select()
  {
  if(gtvisual()&vselect) ststate(state|vselect);
  return 0;
  }

int Iborder::unselect()
  {
  ststate(state&~vselect);
  return 0;
  }

int Iborder::focusrtn()
  {
  if(gtvisual()&vfocusrtn) ststate(state|vfocusrtn);
  return 0;
  }

int Iborder::unfocusrtn()
  {
  ststate(state&~vfocusrtn);
  return 0;
  }

int Iborder::activate()
  {
  if(gtvisual()&vactive) ststate(state|vactive);
  return 0;
  }

int Iborder::deactivate()
  {
  ststate(state&~vactive);
  return 0;
  }

int Iborder::enternotify(XEvent *ev)
 {
 if(gtvisual()&venter) ststate(state|venter);
 Border::enternotify(ev);
 return 0;
 }

int Iborder::leavenotify(XEvent *ev)
  {
  ststate(state&~venter);
  Border::leavenotify(ev);
  return 0;
  }

Iborder::Iborder()
  {
  state=0;
  stfixed(5,3,stdshadowtop,stdshadowbot,stdoutline);
  stvisual(vfocus|vpress|vselect|vfocusrtn);
  }

/* Iborder, but with no focus effects.  For keypad pushbuttons */

//Iborder *mkIkborder(w)
//Iborder *w;
// {
// w=mkIborder(w);
// stfixed(w,2,0,stdshadowtop,stdshadowbot,stdoutline);
// stvisual(w,vpress|vselect);
// return w;
// }

/* Intelligent border for edit window */

void Eborder::ststate(int n)
  {
  int outlined=0;
  int raised=0;
  int lowered=2;

  state=n;
  if(n&vfocus) outlined=2;
 
  ststyle(raised,lowered,outlined);
  }

Eborder::Eborder()
  {
  stfixed(2,6,stdshadowtop,stdshadowbot,stdoutline);
  ststyle(0,2,0);
  }

/* Eborder, but with no focus effects.  For touch screens */

//Eborder *mkEkborder(w)
//Eborder *w;
// {
// mkEborder(w);
// stfixed(w,2,0,stdshadowtop,stdshadowbot,stdoutline);
// ststyle(w,0,2,0);
// stvisual(w,vpress|vselect);
// return w;
// }

/* Intelligent border for menu buttons */

void Mborder::ststate(int n)
  {
  int outlined=0;
  int raised=0;
  int lowered=0;

  state=n;
  if(n&vfocus) raised=2;
  if(n&vselect) raised=2;

  ststyle(raised,lowered,outlined);
  }

Mborder::Mborder()
  {
  stfixed(2,0,stdshadowtop,stdshadowbot,stdoutline);
  ststyle(0,0,0);
  }

/* Intelligent border for toggle buttons */

void Tborder::ststate(int n)
  {
  int outlined=0;
  int raised=0;
  int lowered=0;

  state=n;
  if(n&vfocus) outlined=2;
  if(n&vfocusrtn) lowered=1;
  
  ststyle(raised,lowered,outlined);
  }

Tborder::Tborder()
  {
  stfixed(5,3,stdshadowtop,stdshadowbot,stdoutline);
  ststyle(0,0,0);
  }

/* Titled border */

int Titled::expose(XEvent *ev)
  {
  int amnt=gtlmargin();
  draw3dbox(this,amnt/2,amnt/2,gtw()-amnt,gth()-amnt,1,stdshadowbot,stdshadowtop);
  draw3dbox(this,amnt/2+1,amnt/2+1,gtw()-amnt-2,gth()-amnt-2,1,stdshadowtop,stdshadowbot);
  return 0;
  }

void Titled::st(Widget *t)
  {
  if(msg) delete msg;
  msg=0;
  if(t)
    {
    msg=t;
    t->stborder(NULL);
    t->stbknd(stdtopbknd);
    t->stx(gtlmargin());
    t->sty(0);
    add(t);
    }
  }

Titled::Titled()
  {
  stmargins(16,16,16,16);
  stbknd(stdtopbknd);
  msg=0;
  }

/* Dialog titlebar window */

int Titlebar::expose(XEvent *ev)
  {
  draw3dbox(this,0,0,
            gtw(),
            gth(),
            2,stdshadowtop,stdshadowbot);
  drawfilled(this,getgc("8x13","grey50"),2,2,gtw()-4,16);
  if(text)
    drawtxt(this,stdshadowtop,2+txtw(gtfgnd(),"M")/2,2+txtb(gtfgnd()),text);
  return 0;
  }

void Titlebar::st(char *t)
  {
  text=t;
  }

Titlebar::Titlebar()
  {
  stmargins(4,4,16+4,4);
  stbknd(stdtopbknd);
  text=0;
  }
