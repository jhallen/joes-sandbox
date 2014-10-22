/* Scroll bar widgets 
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

/* Vertical scroll bar */

static void barinc(stat,v)
Bar *v;
 {
 if(!stat)
  {
  stfn(v->inc,fn1(v->fninc,barinc,v));
  if(v->pos<v->dsize-v->wsize-1)
   {
   v->pos+=v->step;
   if(v->pos>v->dsize-v->wsize-1) v->pos=v->dsize-v->wsize-1;
   stpos(v->slider,v->pos);
   cont1(v->fn,v->pos); v->fn=0;
   }
  }
 }

static void bardec(stat,v)
Bar *v;
 {
 if(!stat)
  {
  stfn(v->dec,fn1(v->fndec,bardec,v));
  if(v->pos>0)
   {
   v->pos-=v->step;
   if(v->pos<0) v->pos=0;
   stpos(v->slider,v->pos);
   cont1(v->fn,v->pos); v->fn=0;
   }
  }
 }

static void barmove(stat,v,n)
Bar *v;
 {
 if(!stat)
  {
  v->pos=n;
  stfn(v->slider,fn1(v->fnslide,barmove,v));
  cont1(v->fn,n); v->fn=0;
  }
 }

static void vbaron(v)
Bar *v;
 {
 if(!v->inc)
  {
  Icon *i;
  ttob(v,NULL,0);
  ledge(v,0);
  v->dec=mk(Button);
   stflg(v->dec,gtflg(v->dec)&~flgfocus);
   stmargins(gtborder(v->dec),2,2,2,2);
   strate(v->dec,33000);
   stdelay(v->dec,250000);
   stfn(v->dec,fn1(v->fndec,bardec,v));
   i=mk(Icon);
    st(i,picuparw);
    stwidth(i,gtwidth(v)-4);
    stheight(i,gtwidth(v)-4);
   add(v->dec,i);
  add(v,v->dec);
  v->slider=mk(Vslider);
   stw(v->slider,gtwidth(v));
   sth(v->slider,gtheight(v)-2*gtwidth(v));
   stfn(v->slider,fn1(v->fnslide,barmove,v));
   stdsize(v->slider,v->dsize);
   stwsize(v->slider,v->wsize);
   stpos(v->slider,v->pos); 
  add(v,v->slider);
  v->inc=mk(Button);
   stflg(v->inc,gtflg(v->inc)&~flgfocus);
   stmargins(gtborder(v->inc),2,2,2,2);
   strate(v->inc,33000);
   stdelay(v->inc,250000);
   stfn(v->inc,fn1(v->fninc,barinc,v));
   i=mk(Icon);
    st(i,picdnarw);
    stwidth(i,gtwidth(v)-4);
    stheight(i,gtwidth(v)-4);
   add(v->inc,i);
  add(v,v->inc);
  }
 (lithfuncs.on)(v);
 }

static void barstpos(v,n)
Bar *v;
 {
 v->pos=n;
 if(v->slider) stpos(v->slider,n);
 }

static void barstdsize(v,n)
Bar *v;
 {
 v->dsize=n;
 if(v->slider) stdsize(v->slider,n);
 }

static void barstwsize(v,n)
Bar *v;
 {
 v->wsize=n;
 if(v->slider) stwsize(v->slider,n);
 }

static void barstfn(v,c)
Bar *v;
TASK *c;
 {
 v->fn=c;
 }

static void barststep(v,n)
Bar *v;
 {
 v->step=n;
 }

static int bargtstep(v)
Bar *v;
 {
 return v->step;
 }

static void barrm(v)
Bar *v;
 {
 cancel(v->fn); v->fn=0;
 (widgetfuncs.rm)(v);
 }

struct barfuncs vbarfuncs;

Bar *mkVbar(v)
Bar *v;
 {
 mkWidget(v);
 if(!vbarfuncs.on)
  {
  mcpy(&vbarfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  vbarfuncs.on=vbaron;
  vbarfuncs.stpos=barstpos;
  vbarfuncs.stdsize=barstdsize;
  vbarfuncs.stwsize=barstwsize;
  vbarfuncs.stfn=barstfn;
  vbarfuncs.ststep=barststep;
  vbarfuncs.gtstep=bargtstep;
  vbarfuncs.rm=barrm;
  }
 v->funcs= &vbarfuncs;
 v->inc=0;
 iztask(v->fninc);
 v->dec=0;
 iztask(v->fndec);
 v->slider=0;
 iztask(v->fnslide);
 v->pos=0;
 v->wsize=50;
 v->dsize=100;
 v->fn=0;
 v->step=1;
 stborder(v,mk(Shadow));
 ststyle(((Shadow *)gtborder(v)),0,2,0);
 stw(v,24);
 return v;
 }

/* Horz scroll bar */

static void hbaron(v)
Bar *v;
 {
 if(!v->inc)
  {
  Icon *i;
  ltor(v,NULL,0);
  tedge(v,0);
  v->dec=mk(Button);
   stflg(v->dec,gtflg(v->dec)&~flgfocus);
   stmargins(gtborder(v->dec),2,2,2,2);
   strate(v->dec,33000);
   stdelay(v->dec,250000);
   stfn(v->dec,fn1(v->fndec,bardec,v));
   i=mk(Icon);
    st(i,picltarw);
    stwidth(i,gtheight(v)-4);
    stheight(i,gtheight(v)-4);
   add(v->dec,i);
  add(v,v->dec);
  v->slider=mk(Hslider);
   stw(v->slider,gtwidth(v)-2*gtheight(v));
   sth(v->slider,gtheight(v));
   stfn(v->slider,fn1(v->fnslide,barmove,v));
   stdsize(v->slider,v->dsize);
   stwsize(v->slider,v->wsize);
   stpos(v->slider,v->pos); 
  add(v,v->slider);
  v->inc=mk(Button);
   stflg(v->inc,gtflg(v->inc)&~flgfocus);
   stmargins(gtborder(v->inc),2,2,2,2);
   strate(v->inc,33000);
   stdelay(v->inc,250000);
   stfn(v->inc,fn1(v->fninc,barinc,v));
   i=mk(Icon);
    st(i,picrtarw);
    stwidth(i,gtheight(v)-4);
    stheight(i,gtheight(v)-4);
   add(v->inc,i);
  add(v,v->inc);
  }
 (lithfuncs.on)(v);
 }

struct barfuncs hbarfuncs;

Bar *mkHbar(v)
Bar *v;
 {
 mkWidget(v);
 if(!hbarfuncs.on)
  {
  mcpy(&hbarfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  hbarfuncs.on=hbaron;
  hbarfuncs.stpos=barstpos;
  hbarfuncs.stdsize=barstdsize;
  hbarfuncs.stwsize=barstwsize;
  hbarfuncs.stfn=barstfn;
  hbarfuncs.ststep=barststep;
  hbarfuncs.gtstep=bargtstep;
  hbarfuncs.rm=barrm;
  }
 v->funcs= &hbarfuncs;
 v->inc=0;
 iztask(v->fninc);
 v->dec=0;
 iztask(v->fndec);
 v->slider=0;
 iztask(v->fnslide);
 v->pos=0;
 v->wsize=50;
 v->dsize=100;
 v->fn=0;
 v->step=1;
 stborder(v,mk(Shadow));
 ststyle(((Shadow *)gtborder(v)),0,2,0);
 sth(v,24);
 return v;
 }
