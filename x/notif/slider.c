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

static void sliderrm(v)
Slider *v;
 {
 cancel(v->fn); v->fn=0;
 (widgetfuncs.rm)(v);
 }

static void sliderstfn(v,c)
Slider *v;
TASK *c;
 {
 v->fn=c;
 }

static TASK *slidergtfn(v)
Slider *v;
 {
 return v->fn;
 }

static int sliderrelease(v)
Slider *v;
 {
 v->start=-1;
 }

static void sliderstpos(v,x)
Slider *v;
 {
 v->pos=x;
 redraw(v);
 }

static void sliderstdsize(v,n)
Slider *v;
 {
 v->dsize=n;
 redraw(v);
 }

static void sliderstwsize(v,n)
Slider *v;
 {
 v->wsize=n;
 redraw(v);
 }

/* Vertical slider */

static int vsliderdraw(v)
Slider *v;
 {
 drawclr(v);
 if(v->dsize>=v->wsize)
  {
  int hh=v->min+(gtheight(v)+1-v->min)*v->wsize/v->dsize;	/* Height of rect */
  int yy=((gtheight(v)+1)-hh)*v->pos/(v->dsize-v->wsize);	/* Y position of rect */
  drawfilled(v,gtbknd(v),0,yy,gtwidth(v),hh);
  draw3dbox(v,0,yy,gtwidth(v),hh,2,stdshadowtop,stdshadowbot);
  }
 }

static int vslideruser(v,key,state,x,y)
Slider *v;
 {
 switch(key)
  {
  case XK_Press1:
  if(v->dsize>=v->wsize)
   {
   int hh=v->min+(gtheight(v)-v->min+1)*v->wsize/v->dsize;	/* Height of rect */
   int yy=((gtheight(v)+1)-hh)*v->pos/(v->dsize-v->wsize);	/* Y position of rect */
   if(y>=yy && y<=yy+hh) v->start=y-yy;
   else if(y<yy)
    { /* Page up */
    if(v->pos>=v->wsize/2) v->pos-=v->wsize/2;
    else v->pos=0;
    redraw(v);
    cont1(v->fn,v->pos); v->fn=0;
    return 0;
    }
   else if(y>yy+hh)
    { /* Page down */
    if(v->dsize-(v->pos+v->wsize+1)>=v->wsize/2) v->pos+=v->wsize/2;
    else v->pos=v->dsize-v->wsize-1;
    redraw(v);
    cont1(v->fn,v->pos); v->fn=0;
    return 0;
    }
   }
  return 0;

  case XK_Release1:
  v->start=-1;
  return 0;

  case XK_Motion:
  if(v->start!=-1)
   {
   int hh=v->min+(gtheight(v)+1-v->min)*v->wsize/v->dsize;	/* Height of rect */
   int yy=(gtheight(v)+1)-hh;					/* Max travel of rect */
   int t=y-v->start;
   if(t<=0)
    {
    if(v->pos!=0)
     {
     v->pos=0;
     redraw(v);
     cont1(v->fn,v->pos); v->fn=0;
     return 0;
     }
    }
   else if(t>=yy)
    {
    if(v->pos!=v->dsize-v->wsize-1)
     {
     v->pos=v->dsize-v->wsize-1;
     redraw(v);
     cont1(v->fn,v->pos); v->fn=0;
     return 0;
     }
    }
   else
    {
    v->pos=t*(v->dsize-v->wsize)/yy;
    redraw(v);
    cont1(v->fn,v->pos); v->fn=0;
    return 0;
    }
   }
  return 0;
  }
 return 1;
 }

struct sliderfuncs vsliderfuncs;

Slider *mkVslider(v)
Slider *v;
 {
 mkWidget(v);
 if(!vsliderfuncs.on)
  {
  mcpy(&vsliderfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  vsliderfuncs.user=vslideruser;
  vsliderfuncs.expose=vsliderdraw;
  vsliderfuncs.stfn=sliderstfn;
  vsliderfuncs.gtfn=slidergtfn;
  vsliderfuncs.stpos=sliderstpos;
  vsliderfuncs.stdsize=sliderstdsize;
  vsliderfuncs.stwsize=sliderstwsize;
  vsliderfuncs.rm=sliderrm;
  }
 v->funcs= &vsliderfuncs;
 v->wsize=50;
 v->pos=0;
 v->dsize=100;
 v->fn=0;
 v->start=-1;
 v->min=8;
 stw(v,20);
 return v;
 }

/* Horz slider */

static int hsliderdraw(v)
Slider *v;
 {
 drawclr(v);
 if(v->dsize>=v->wsize)
  {
  int hh=v->min+(gtwidth(v)+1-v->min)*v->wsize/v->dsize;	/* Height of rect */
  int yy=((gtwidth(v)+1)-hh)*v->pos/(v->dsize-v->wsize);	/* Y position of rect */
  drawclr(v);
  drawfilled(v,gtbknd(v),yy,0,hh,gtheight(v));
  draw3dbox(v,yy,0,hh,gtheight(v),2,stdshadowtop,stdshadowbot);
  }
 }

static int hslideruser(v,key,state,x,y)
Slider *v;
 {
 switch(key)
  {
  case XK_Press1:
  if(v->dsize>=v->wsize)
   {
   int hh=v->min+(gtwidth(v)+1-v->min)*v->wsize/v->dsize;	/* Height of rect */
   int yy=((gtwidth(v)+1)-hh)*v->pos/(v->dsize-v->wsize);	/* Y position of rect */
   if(x>=yy && x<=yy+hh) v->start=x-yy;
   else if(x<yy)
    { /* Page up */
    if(v->pos>=v->wsize/2) v->pos-=v->wsize/2;
    else v->pos=0;
    redraw(v);
    cont1(v->fn,v->pos); v->fn=0;
    return 0;
    }
   else if(x>yy+hh)
    { /* Page down */
    if(v->dsize-(v->pos+v->wsize+1)>=v->wsize/2) v->pos+=v->wsize/2;
    else v->pos=v->dsize-v->wsize-1;
    redraw(v);
    cont1(v->fn,v->pos); v->fn=0;
    return 0;
    }
   }
  return 0;

  case XK_Release1:
  v->start= -1;
  return 0;

  case XK_Motion:
  if(v->start!=-1)
   {
   int hh=v->min+(gtwidth(v)+1-v->min)*v->wsize/v->dsize;	/* Height of rect */
   int yy=(gtwidth(v)+1)-hh;					/* Max travel of rect */
   int t=x-v->start;
   if(t<=0)
    {
    if(v->pos!=0)
     {
     v->pos=0;
     redraw(v);
     cont1(v->fn,v->pos); v->fn=0;
     return 0;
     }
    }
   else if(t>=yy)
    {
    if(v->pos!=v->dsize-v->wsize-1)
     {
     v->pos=v->dsize-v->wsize-1;
     redraw(v);
     cont1(v->fn,v->pos); v->fn=0;
     return 0;
     }
    }
   else
    {
    v->pos=t*(v->dsize-v->wsize)/yy;
    redraw(v);
    cont1(v->fn,v->pos); v->fn=0;
    return 0;
    }
   }
  return 0;
  }
 return 1;
 }

struct sliderfuncs hsliderfuncs;

Slider *mkHslider(v)
Slider *v;
 {
 mkWidget(v);
 if(!hsliderfuncs.on)
  {
  mcpy(&hsliderfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  hsliderfuncs.user=hslideruser;
  hsliderfuncs.expose=hsliderdraw;
  hsliderfuncs.stfn=sliderstfn;
  hsliderfuncs.gtfn=slidergtfn;
  hsliderfuncs.stpos=sliderstpos;
  hsliderfuncs.stdsize=sliderstdsize;
  hsliderfuncs.stwsize=sliderstwsize;
  hsliderfuncs.rm=sliderrm;
  }
 v->funcs= &hsliderfuncs;
 v->wsize=50;
 v->pos=0;
 v->dsize=100;
 v->fn=0;
 v->start=-1;
 v->min=8;
 sth(v,20);
 return v;
 }
