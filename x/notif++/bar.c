/* Scroll bar widgets
   Copyright (C) 1999 Joseph H. Allen

This file is part of Notif++

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

// User hit increment button

void Bar::barinc()
  {
  // Re-install self
  inc->stfn(new Mfn0_0<void,Bar>(&Bar::barinc,this));

  // If we're not at the end
  if (pos < dsize - wsize - 1)
    {
    // Update position
    pos += step;
    if (pos > dsize - wsize - 1)
      pos = dsize - wsize - 1;

    // Move slider
    slider->stpos(pos);

    // Call callback function with new position
    if (fn) fn->cont(fn,pos);
    }
  }

// User hit decrement button

void Bar::bardec()
  {
  // Re-install self
  dec->stfn(new Mfn0_0<void,Bar>(&Bar::bardec,this));

  // If we're not at beginning
  if (pos > 0)
    {
    pos -= step;
    if (pos < 0)
      pos = 0;

    // Move slider
    slider->stpos(pos);

    // Call callback function with new position
    if (fn) fn->cont(fn,pos);
    }
  }

// User moved slider

void Bar::barmove(int n)
  {
  // Re-install self
  slider->stfn(new Mfn0_1<void,Bar,int>(&Bar::barmove,this));

  if (pos != n)
    {
    // Update position
    pos = n;

    // Call callback function with new position
    if (fn) fn->cont(fn,pos);
    }
  }

void Bar::stpos(int n)
  {
  pos = n;
  if (slider) slider->stpos(n);
  }

int Bar::gtpos()
  {
  return pos;
  }

void Bar::stdsize(int n)
  {
  dsize = n;
  if (slider) slider->stdsize(n);
  }

int Bar::gtdsize()
  {
  return dsize;
  }

void Bar::stwsize(int n)
  {
  wsize = n;
  if (slider) slider->stwsize(n);
  }

int Bar::gtwsize()
  {
  return wsize;
  }

void Bar::stfn(Fn_1<void,int> *new_fn)
  {
  fn = new_fn;
  }

Fn_1<void,int> *Bar::gtfn()
  {
  return fn;
  }

void Bar::ststep(int n)
  {
  step = n;
  }

int Bar::gtstep()
  {
  return step;
  }

void Vbar::on()
  {
  // If this is the first time...
  if(!inc)
    {
    Icon *i;
    ledge(0);
    tedge(0);

    dec = new Button();
     dec->clfocusable();
     dec->gtborder()->stmargins(2,2,2,2);
     dec->strate(33000);
     dec->stdelay(250000);
     dec->stfn(new Mfn0_0<void,Vbar>(&Bar::bardec,this));
     i = new Icon();
      i->st(picuparw);
      i->stwidth(gtwidth()-4);
      i->stheight(gtwidth()-4);
     dec->add(i);
    add(dec);

    bedge(0);

    inc = new Button();
     inc->clfocusable();
     inc->gtborder()->stmargins(2,2,2,2);
     inc->strate(33000);
     inc->stdelay(250000);
     inc->stfn(new Mfn0_0<void,Vbar>(&Bar::barinc,this));
     i = new Icon();
      i->st(picdnarw);
      i->stwidth(gtwidth()-4);
      i->stheight(gtwidth()-4);
     inc->add(i);
    add(inc);

    ttob(dec,0);
    auxbtot(inc,0);
    slider = new Vslider();
     slider->stw(gtwidth());
     slider->sth(gtheight()-2*gtwidth());
     slider->stfn(new Mfn0_1<void,Vbar,int>(&Bar::barmove,this));
     slider->stdsize(dsize);
     slider->stwsize(wsize);
     slider->stpos(pos); 
     slider->strate(33000);
     slider->stdelay(250000);
    add(slider);

    }
  Lith::on();
  }

Vbar::~Vbar()
  {
  if (fn) fn->cancel();
  }

Vbar::Vbar()
  {
  inc = 0;
  dec = 0;
  slider = 0;
  pos = 0;
  wsize = 50;
  dsize = 100;
  fn = 0;
  step = 1;
  stborder(new Shadow());
  ((Shadow *)gtborder())->ststyle(0,2,0);
  stw(24);
  }

/* Horz scroll bar */

void Hbar::on()
  {
  if (!inc)
    {
    Icon *i;
    tedge(0);
    ledge(0);

    dec = new Button();
      dec->clfocusable();
      dec->gtborder()->stmargins(2,2,2,2);
      dec->strate(33000);
      dec->stdelay(250000);
      dec->stfn(new Mfn0_0<void,Hbar>(&Bar::bardec,this));
      i = new Icon();
        i->st(picltarw);
        i->stwidth(gtheight()-4);
        i->stheight(gtheight()-4);
      dec->add(i);
    add(dec);

    redge(0);
    inc = new Button();
      inc->clfocusable();
      inc->gtborder()->stmargins(2,2,2,2);
      inc->strate(33000);
      inc->stdelay(250000);
      inc->stfn(new Mfn0_0<void,Hbar>(&Bar::barinc,this));
      i = new Icon();
        i->st(picrtarw);
        i->stwidth(gtheight()-4);
        i->stheight(gtheight()-4);
      inc->add(i);
    add(inc);

    ltor(dec,0);
    auxrtol(inc,0);
    slider = new Hslider();
      slider->stw(gtwidth()-2*gtheight());
      slider->sth(gtheight());
      slider->stfn(new Mfn0_1<void,Hbar,int>(&Bar::barmove,this));
      slider->stdsize(dsize);
      slider->stwsize(wsize);
      slider->stpos(pos); 
      slider->strate(33000);
      slider->stdelay(250000);
    add(slider);
    }
  Lith::on();
  }

Hbar::Hbar()
  {
  inc = 0;
  dec = 0;
  slider = 0;
  pos = 0;
  wsize = 50;
  dsize = 100;
  fn = 0;
  step = 1;
  stborder(new Shadow());
  ((Shadow *)gtborder())->ststyle(0,2,0);
  sth(24);
  }

Hbar::~Hbar()
  {
  printf("Deleting\n");
  if (fn) fn->cancel();
  }
