/* Scroll bars 
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

/* Generic scroll bar */

class Bar : public Widget
  {
  public:

  Button *dec;		/* Decrement button */
  void bardec();	/* Decrement callback */

  Button *inc;		/* Increment button */
  void barinc();	/* Increment callback */

  Slider *slider;	/* Slider widget */
  void barmove(int n);	/* Slider callback */

  int pos;		/* Scroll bar position */
  int wsize;		/* Window size */
  int dsize;		/* Data size */
  int step;		/* Step */

  Fn_1<void,int> *fn;	/* Callback function */


  void stpos(int new_pos);	/* Set position */
  int gtpos();

  void stdsize(int new_dsize);	/* Set data size */
  int gtdsize();

  void stwsize(int new_wsize);	/* Set window size */
  int gtwsize();

  void ststep(int new_step);	/* Set step */
  int gtstep();

  void stfn(Fn_1<void,int> *new_fn);/* Set/get call-back function */
  Fn_1<void,int> *gtfn();
  };

/* Horizontal scroll bar */

class Hbar : public Bar
  {
  public:

  virtual void on();
  Hbar();
  virtual ~Hbar();
  };

/* Vertical scroll bar */

class Vbar : public Bar
  {
  public:

  virtual void on();
  Vbar();
  virtual ~Vbar();
  };
