/* Window w/ smart scroll-bars
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

void scrollstsize(w,n)
Scroll *w;
 {
 w->size=n;
 }

void scrollstvcfg(w,n)
Scroll *w;
 {
 w->vcfg=n;
 }

void scrollsthcfg(w,n)
Scroll *w;
 {
 w->hcfg=n;
 }

static int dohbar(stat,w,pos)
Scroll *w;
 {
 if(!stat)
  {
  stlofst(w->scroll,pos);
  stfn(w->hbar,fn1(w->fnhorz,dohbar,w));
  }
 return stat;
 }

static int dovbar(stat,w,pos)
Scroll *w;
 {
 if(!stat)
  {
  sttofst(w->scroll,pos);
  stfn(w->vbar,fn1(w->fnvert,dovbar,w));
  }
 return stat;
 }

/* Deal with size change */

void scrollhv(w)
Scroll *w;
 {
 int besth, bestw;
 int needh, needv;
 /* First calculate what width and height of scroll window would be if no
    scroll-bars existed */
 switch(w->vcfg)
  {
  case cfgnone: bestw=gtiw(w); break;
  case cfgrsvd: bestw=gtiw(w)-w->size; break;
  case cfgin: bestw=gtiw(w); break;
  case cfgout: bestw=gtiw(w); if(w->vbar) bestw-=w->size; break;
  }
 switch(w->hcfg)
  {
  case cfgnone: besth=gtih(w); break;
  case cfgrsvd: besth=gtih(w)-w->size; break;
  case cfgin: besth=gtih(w); break;
  case cfgout: besth=gtih(w); if(w->hbar) besth-=w->size; break;
  }
 /* Take into account margins of scroll */
 if(gtborder(w->scroll))
  {
  bestw-=gtlmargin(gtborder(w->scroll))+gtrmargin(gtborder(w->scroll));
  besth-=gttmargin(gtborder(w->scroll))+gtbmargin(gtborder(w->scroll));
  }

 /* Determine which scroll-bars are needed */
 needh=0; needv=0;

 if(gtwidth(w->scroll)>bestw && w->hcfg!=cfgnone) needh=1;
 if(gtheight(w->scroll)>besth && w->vcfg!=cfgnone) needv=1;
 if(needv && w->vcfg==cfgin &&
    gtwidth(w->scroll)>bestw-w->size && w->hcfg!=cfgnone) needh=1;
 if(needh && w->hcfg==cfgin &&
    gtheight(w->scroll)>besth-w->size && w->vcfg!=cfgnone) needv=1;

 /* Size of scroll, taking into account scroll bars */
 if(needv && w->vcfg==cfgin) bestw-=w->size;
 if(needh && w->hcfg==cfgin) besth-=w->size;
 if(gtborder(w->scroll))
  {
  bestw+=gtlmargin(gtborder(w->scroll))+gtrmargin(gtborder(w->scroll));
  besth+=gttmargin(gtborder(w->scroll))+gtbmargin(gtborder(w->scroll));
  }

 /* Set width/height of scroll */
 stw(w->scroll,bestw);
 sth(w->scroll,besth);

 /* Set our own width/height */
 if(needv || w->vcfg==cfgrsvd) stiw(w,bestw+w->size);
 else stiw(w,bestw);

 if(needh || w->hcfg==cfgrsvd) stih(w,besth+w->size);
 else stih(w,besth);

 /* Turn on/off appropriate scroll-bars */
 if(!needh && w->hbar) rm(w->hbar), w->hbar=0;
 if(needh && !w->hbar)
  {
  w->hbar=mk(Hbar);
   ststep(w->hbar,16);
  stw(w->hbar,bestw);
  sth(w->hbar,w->size);
  stx(w->hbar,0);
  sty(w->hbar,besth);
  stfn(w->hbar,fn1(w->fnhorz,dohbar,w));
  add(w,w->hbar);
  }

 if(!needv && w->vbar) rm(w->vbar), w->vbar=0;
 if(needv && !w->vbar)
  {
  w->vbar=mk(Vbar);
   ststep(w->vbar,16);
  stw(w->vbar,w->size);
  sth(w->vbar,besth);
  stx(w->vbar,bestw);
  sty(w->vbar,0);
  stfn(w->vbar,fn1(w->fnvert,dovbar,w));
  add(w,w->vbar);
  }

 /* Set scroll-bar positions */
 if(w->hbar)
  {
  int wid=gtiw(w->scroll);
  stdsize(w->hbar,gtwidth(w->scroll));
  stwsize(w->hbar,wid);
  stpos(w->hbar,gtlofst(w->scroll));
  }
 else stlofst(w->scroll,0);

 if(w->vbar)
  {
  int hgt=gtih(w->scroll);
  stdsize(w->vbar,gtheight(w->scroll));
  stwsize(w->vbar,hgt);
  stpos(w->vbar,gttofst(w->scroll));
  }
 else sttofst(w->scroll,0);
 }

void scrollon(w)
Scroll *w;
 {
 if((gtflg(w)&flgenabled) && !(gtflg(w)&flgon)) scrollhv(w);
 (lithfuncs.on)(w);
 }

void scrollstw(w,n)
Scroll *w;
 {
 (widgetfuncs.stw)(w,n);
 if(gtflg(w)&flgon)
  {
  if(w->hbar) rm(w->hbar), w->hbar=0;
  if(w->vbar) rm(w->vbar), w->vbar=0;
  scrollhv(w);
  }
 }

void scrollsth(w,n)
Scroll *w;
 {
 (widgetfuncs.sth)(w,n);
 if(gtflg(w)&flgon)
  {
  if(w->hbar) rm(w->hbar), w->hbar=0;
  if(w->vbar) rm(w->vbar), w->vbar=0;
  scrollhv(w);
  }
 }

int scrollgtvstep(w)
Scroll *w;
 {
 return w->vstep;
 }

int scrollgthstep(w)
Scroll *w;
 {
 return w->hstep;
 }

void scrollstvstep(w,n)
Scroll *w;
 {
 w->vstep=n;
 if(w->vbar) ststep(w->vbar,n);
 }

void scrollsthstep(w,n)
Scroll *w;
 {
 w->hstep=n;
 if(w->hbar) ststep(w->hbar,n);
 }

int dosizenotify(stat,w)
Scroll *w;
 {
 if(!stat)
  {
  stsizenotifyfn(gt(w),fn1(w->fnresize,dosizenotify,w));
  if(gtflg(w)&flgon) scrollhv(w);
  }
 return stat;
 }

void scrollst(w,v)
Scroll *w;
Widget *v;
 {
 if(w->scroll) rm(w->scroll);
 w->scroll=v;
  stscroll(v);
  stborder(v,mk(Shadow));
  ststyle(((Shadow *)gtborder(v)),0,2,0);
  stx(v,0);
  sty(v,0);
  stsizenotifyfn(v,fn1(w->fnresize,dosizenotify,w));
 add(w,v);
 }

Widget *scrollgt(w)
Scroll *w;
 {
 return w->scroll;
 }

struct scrollfuncs scrollfuncs;

Scroll *mkScroll(r)
Scroll *r;
 {
 mkWidget(r);
 if(!scrollfuncs.on)
  {
  mcpy(&scrollfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  scrollfuncs.stw=scrollstw;
  scrollfuncs.sth=scrollsth;
  scrollfuncs.stsize=scrollstsize;
  scrollfuncs.stvcfg=scrollstvcfg;
  scrollfuncs.sthcfg=scrollsthcfg;
  scrollfuncs.on=scrollon;
  scrollfuncs.gtvstep=scrollgtvstep;
  scrollfuncs.gthstep=scrollgthstep;
  scrollfuncs.stvstep=scrollstvstep;
  scrollfuncs.sthstep=scrollsthstep;
  scrollfuncs.st=scrollst;
  scrollfuncs.gt=scrollgt;
  }
 r->funcs= &scrollfuncs;
 r->vstep=1;
 r->hstep=1;
 r->vbar=0;
 r->hbar=0;
 r->size=24;
 r->vcfg=cfgin;
 r->hcfg=cfgin;
 r->scroll=0;
 iztask(r->fnhorz);
 iztask(r->fnvert);
 iztask(r->fnresize);
 st(r,mk(Widget));
 return r;
 }
