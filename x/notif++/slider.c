/* Sliders 
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

void Slider::stdelay(int n)
  {
  delay=n;
  }

void Slider::strate(int n)
  {
  rate=n;
  }

void Slider::stfn(Fn_1<void,int> *new_fn)
 {
 fn=new_fn;
 printf("slider::stfn rept=%d delay=%d\n", rept, delay);
 if(rept==1 && delay)
   {
   printf("slider::stfn submit\n");
   /* After button was first pressed */
   submit(delay,tevent=new Mfn0_0<void,Slider>(&Slider::sliderrept,this));
   }
 else if(rept==2 && rate)
   {
   printf("slider::stfn submit\n");
   /* After button repeated */
   submit(rate,tevent=new Mfn0_0<void,Slider>(&Slider::sliderrept,this));
   }
 }

Fn_1<void,int> *Slider::gtfn()
 {
 return fn;
 }

void Slider::stpos(int new_pos)
 {
 pos=new_pos;
 redraw(this);
 }

void Slider::stdsize(int n)
 {
 dsize=n;
 redraw(this);
 }

void Slider::stwsize(int n)
 {
 wsize=n;
 redraw(this);
 }

/* Vertical slider */

int Vslider::expose(XEvent *ev)
  {
  drawclr(this);
  if(dsize >= wsize)
    {
    int hh=min+(gtheight()+1-min)*wsize/dsize;	/* Height of rect */
    int yy=((gtheight()+1)-hh)*pos/(dsize-wsize);	/* Y position of rect */
    drawfilled(this,gtbknd(),0,yy,gtwidth(),hh);
    draw3dbox(this,0,yy,gtwidth(),hh,2,stdshadowtop,stdshadowbot);
    }
  }

void Slider::sliderpgup()
  {
  if(pos>=wsize/2) pos-=wsize/2;
  else pos=0;
  redraw(this);
  dir = 0;
  printf("sliderpgup\n");
  if(rept==0) rept=1;		/* User pressed button */
  else if(rept==1) rept=2;		/* Button started repeating */
  if (fn) fn->cont(fn,pos);
  }

void Slider::sliderpgdn()
  {
  if(dsize-(pos+wsize+1)>=wsize/2) pos+=wsize/2;
  else pos=dsize-wsize-1;
  redraw(this);
  dir = 1;
  printf("sliderpgdn\n");
  if(rept==0) rept=1;		/* User pressed button */
  else if(rept==1) rept=2;		/* Button started repeating */
  if (fn) fn->cont(fn,pos);
  }

void Slider::sliderrept()
  {
  if(rept)
    {
    if (!dir)
      {
      sliderpgup();
      }
    else
      {
      sliderpgdn();
      }
    }
  }

Slider::~Slider()
 {
 printf("~Slider\n");
 rept=0;
 cancel(tevent);
 tevent=0;
 }

int Vslider::user(unsigned c,unsigned state,int x,int y,Time time,Widget *org)
 {
 switch(c)
  {
  case XK_Press1:
  if(dsize>=wsize)
   {
   int hh=min+(gtheight()-min+1)*wsize/dsize;	/* Height of rect */
   int yy=((gtheight()+1)-hh)*pos/(dsize-wsize);	/* Y position of rect */
   if(y>=yy && y<=yy+hh) start=y-yy;
   else if(y<yy)
    { /* Page up */
    sliderpgup();
    return 0;
    }
   else if(y>yy+hh)
    { /* Page down */
    sliderpgdn();
    return 0;
    }
   }
  return 0;

  case XK_Release1:
  start=-1;

  printf("Release\n");
  rept=0;
  cancel(tevent);
  tevent=0;

  return 0;

  case XK_Motion:
  if(start!=-1)
   {
   int hh=min+(gtheight()+1-min)*wsize/dsize;	/* Height of rect */
   int yy=(gtheight()+1)-hh;					/* Max travel of rect */
   int t=y-start;
   if(t<=0)
    {
    if(pos!=0)
     {
     pos=0;
     redraw(this);
     if (fn) fn->cont(fn,pos);
     return 0;
     }
    }
   else if(t>=yy)
    {
    if(pos!=dsize-wsize-1)
     {
     pos=dsize-wsize-1;
     redraw(this);
     if (fn) fn->cont(fn,pos);
     return 0;
     }
    }
   else
    {
    pos=t*(dsize-wsize)/yy;
    redraw(this);
    if (fn) fn->cont(fn,pos);
    return 0;
    }
   }
  return 0;
  }
 return 1;
 }

Vslider::Vslider()
 {
 rept=0;
 rate=0;
 delay=0;
 wsize=50;
 pos=0;
 dsize=100;
 fn=0;
 tevent=0;
 start=-1;
 min=8;
 stw(20);
 }

Vslider::~Vslider()
  {
  printf("~Vslider\n");
  if (fn) fn->cancel();
  }

/* Horz slider */

int Hslider::expose(XEvent *ev)
 {
 drawclr(this);
 if(dsize>=wsize)
  {
  int hh=min+(gtwidth()+1-min)*wsize/dsize;	/* Height of rect */
  int yy=((gtwidth()+1)-hh)*pos/(dsize-wsize);	/* Y position of rect */
  drawclr(this);
  drawfilled(this,gtbknd(),yy,0,hh,gtheight());
  draw3dbox(this,yy,0,hh,gtheight(),2,stdshadowtop,stdshadowbot);
  }
 }

int Hslider::user(unsigned c,unsigned state,int x,int y,Time time,Widget *org)
 {
 switch(c)
  {
  case XK_Press1:
  if(dsize>=wsize)
   {
   int hh=min+(gtwidth()+1-min)*wsize/dsize;	/* Height of rect */
   int yy=((gtwidth()+1)-hh)*pos/(dsize-wsize);	/* Y position of rect */
   if(x>=yy && x<=yy+hh) start=x-yy;
   else if(x<yy)
    { /* Page up */
    sliderpgup();
    return 0;
    }
   else if(x>yy+hh)
    { /* Page down */
    sliderpgdn();
    return 0;
    }
   }
  return 0;

  case XK_Release1:
  start= -1;
  printf("Release\n");
  rept=0;
  cancel(tevent);
  tevent=0;
  return 0;

  case XK_Motion:
  if(start!=-1)
   {
   int hh=min+(gtwidth()+1-min)*wsize/dsize;	/* Height of rect */
   int yy=(gtwidth()+1)-hh;					/* Max travel of rect */
   int t=x-start;
   if(t<=0)
    {
    if(pos!=0)
     {
     pos=0;
     redraw(this);
     if (fn) fn->cont(fn,pos);
     return 0;
     }
    }
   else if(t>=yy)
    {
    if(pos!=dsize-wsize-1)
     {
     pos=dsize-wsize-1;
     redraw(this);
     if (fn) fn->cont(fn,pos);
     return 0;
     }
    }
   else
    {
    pos=t*(dsize-wsize)/yy;
    redraw(this);
    if (fn) fn->cont(fn, pos);
    return 0;
    }
   }
  return 0;
  }
 return 1;
 }

Hslider::Hslider()
 {
 rept=0;
 rate=0;
 delay=0;
 wsize=50;
 pos=0;
 dsize=100;
 fn=0;
 tevent=0;
 start=-1;
 min=8;
 sth(20);
 }

Hslider::~Hslider()
  {
  if (fn) fn->cancel();
  }
