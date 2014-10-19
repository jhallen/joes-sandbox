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

void Scroll::stsize(int n)
  {
  size=n;
  }

void Scroll::stvcfg(int n)
  {
  vcfg=n;
  }

void Scroll::sthcfg(int n)
  {
  hcfg=n;
  }

static void dohbar(Scroll *w,int pos)
  {
  w->scroll->stlofst(pos);
  w->hbar->stfn(new Fn1_1<void,Scroll *,int>(dohbar,w));
  }

static void dovbar(Scroll *w,int pos)
  {
  w->scroll->sttofst(pos);
  w->vbar->stfn(new Fn1_1<void,Scroll *,int>(dovbar,w));
  }

extern void show_tree(int idx,Widget *l);

/* Deal with size change */

void Scroll::scrollhv()
 {
 int besth, bestw;
 int needh, needv;
 /* First calculate what width and height of scroll window would be if no
    scroll-bars existed */
 switch(vcfg)
  {
  case cfgnone: bestw=gtiw(); break;
  case cfgrsvd: bestw=gtiw()-size; break;
  case cfgin: bestw=gtiw(); break;
  case cfgout: bestw=gtiw(); if(vbar) bestw-=size; break;
  }
 switch(hcfg)
  {
  case cfgnone: besth=gtih(); break;
  case cfgrsvd: besth=gtih()-size; break;
  case cfgin: besth=gtih(); break;
  case cfgout: besth=gtih(); if(hbar) besth-=size; break;
  }
 /* Take into account margins of scroll */
 if(scroll->gtborder())
  {
  bestw-=scroll->gtborder()->gtlmargin()+scroll->gtborder()->gtrmargin();
  besth-=scroll->gtborder()->gttmargin()+scroll->gtborder()->gtbmargin();
  }

 /* Determine which scroll-bars are needed */
 needh=0; needv=0;

 if(scroll->gtwidth()>bestw && hcfg!=cfgnone) needh=1;
 if(scroll->gtheight()>besth && vcfg!=cfgnone) needv=1;
 if(needv && vcfg==cfgin &&
    scroll->gtwidth()>bestw-size && hcfg!=cfgnone) needh=1;
 if(needh && hcfg==cfgin &&
    scroll->gtheight()>besth-size && vcfg!=cfgnone) needv=1;

 /* Size of scroll, taking into account scroll bars */
 if(needv && vcfg==cfgin) bestw-=size;
 if(needh && hcfg==cfgin) besth-=size;
 if(scroll->gtborder())
  {
  bestw+=scroll->gtborder()->gtlmargin()+scroll->gtborder()->gtrmargin();
  besth+=scroll->gtborder()->gttmargin()+scroll->gtborder()->gtbmargin();
  }

 /* Set width/height of scroll */
 scroll->stw(bestw);
 scroll->sth(besth);

 /* Set our own width/height */
 if(needv || vcfg==cfgrsvd) stiw(bestw+size);
 else stiw(bestw);

 if(needh || hcfg==cfgrsvd) stih(besth+size);
 else stih(besth);

 /* Turn on/off appropriate scroll-bars */
 if(!needh && hbar)
   {
   delete hbar;
   hbar=0;
   }
 else if(needh && !hbar)
  {
  hbar=new Hbar();
   hbar->ststep(hstep);
   if (needv)
     hbar->stw(gtiw()-size);
   else
     hbar->stw(gtiw());
   hbar->sth(size);
   hbar->stx(0);
   hbar->sty(besth);
   hbar->stfn(new Fn1_1<void,Scroll *,int>(dohbar,this));
  add(hbar);
  }
 else if(needh && hbar)
  {
   if (needv)
     hbar->stw(gtiw()-size);
   else
     hbar->stw(gtiw());
   hbar->sth(size);
   hbar->stx(0);
   hbar->sty(besth);
  }

 if(!needv && vbar)
   {
   show_tree(0,vbar);
   delete vbar;
   vbar = 0;
   }
 else if(needv && !vbar)
  {
  vbar=new Vbar();
   vbar->ststep(vstep);
   vbar->stw(size);
   if (needh)
     vbar->sth(gtih()-size);
   else
     vbar->sth(gtih());
   vbar->stx(bestw);
   vbar->sty(0);
   vbar->stfn(new Fn1_1<void,Scroll *,int>(dovbar,this));
  add(vbar);
  }
 else if(needv && vbar)
  {
   if (needh)
     vbar->sth(gtih()-size);
   else
     vbar->sth(gtih());
   vbar->stw(size);
   vbar->stx(bestw);
   vbar->sty(0);
  }

 /* Set scroll-bar positions */
 if(hbar)
  {
  int wid=scroll->gtiw();
  if (vbar)
    wid -= size;
  hbar->stdsize(scroll->gtwidth());
  hbar->stwsize(wid);
  hbar->stpos(scroll->gtlofst());
  }
 else
  scroll->stlofst(0);

 if(vbar)
  {
  int hgt=scroll->gtih();
/*  if (hbar)
    hgt -= size; */
  vbar->stdsize(scroll->gtheight());
  vbar->stwsize(hgt);
  vbar->stpos(scroll->gttofst());
  }
 else scroll->sttofst(0);
 }

void Scroll::on()
  {
  if(enabled_flag && !on_flag) scrollhv();
  Lith::on();
  }

void Scroll::stw(int n)
  {
  Widget::stw(n);
  if(on_flag)
    {
    if(hbar) { delete hbar; hbar=0; }
    if(vbar) { delete vbar; vbar=0; }
    scrollhv();
    }
  }

void Scroll::sth(int n)
  {
  Widget::sth(n);
  if(on_flag)
    {
    if(hbar) { delete hbar; hbar=0; }
    if(vbar) { delete vbar; vbar=0; }
    scrollhv();
    }
  }

int Scroll::gtvstep()
  {
  return vstep;
  }

int Scroll::gthstep()
  {
  return hstep;
  }

void Scroll::stvstep(int n)
  {
  vstep=n;
  if(vbar) vbar->ststep(n);
  }

void Scroll::sthstep(int n)
  {
  hstep=n;
  if(hbar) hbar->ststep(n);
  }

static void dosizenotify(Scroll *w)
  {
  if(w->on_flag)
    w->scrollhv();
  w->scroll->stsizenotifyfn(new Fn1_0<void,Scroll *>(dosizenotify,w));
  }

static void doouternotify(Scroll *w)
  {
  if (w->on_flag)
    {
    Fn_0<void> *tmp = w->scroll->sizenotifyfn;
    w->scroll->sizenotifyfn = 0;
    w->scrollhv();
    w->scroll->sizenotifyfn = tmp;
    }
  w->stsizenotifyfn(new Fn1_0<void,Scroll *>(doouternotify,w));
  }

void Scroll::st(Widget *v)
  {
  Shadow *s;
  if(scroll) delete scroll;
  scroll=v;
    v->stscroll();
    v->stborder(s=new Shadow());
      s->ststyle(0,2,0);
    v->stx(0);
    v->sty(0);
    v->stsizenotifyfn(new Fn1_0<void,Scroll *>(dosizenotify,this));
  add(v);
  }

Widget *Scroll::gt()
  {
  return scroll;
  }

Scroll::Scroll()
  {
  vstep=1;
  hstep=1;
  vbar=0;
  hbar=0;
  size=20;
  vcfg=cfgin;
  hcfg=cfgin;
  scroll=0;
  st(new Widget());
  stsizenotifyfn(new Fn1_0<void,Scroll *>(doouternotify,this));
  }
