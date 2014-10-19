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

class Slider : public Widget
  {
  public:

  int rept;	/* Autorepeat state */
  int rate;	/* Autorepeat rate */
  int delay;	/* Autorepeat starting delay */
  int dir;	/* Last direction for repeat */

  int dsize;		/* Data size */
  int wsize;		/* Window size */
  int pos;		/* Pointer position */
  int start;		/* Starting position when pressed */
  int min;		/* Min. slider size */
  Fn_1<void,int> *fn;	/* Function to call when slider moved */
  Fn_0<void> *tevent;	/* Timer event for auto-repeat */

  virtual void stpos(int pos);
  virtual void stdsize(int dsize);
  virtual void stwsize(int wsize);
  virtual void stfn(Fn_1<void,int> *new_fn);
  virtual Fn_1<void,int> *gtfn();

  virtual void strate(int n);
  virtual void stdelay(int n);

  virtual void sliderrept();
  virtual void sliderpgup();
  virtual void sliderpgdn();

  virtual ~Slider();
  };

class Hslider : public Slider
  {
  public:
  Hslider();
  virtual int expose(XEvent *ev);
  virtual int user(unsigned c,unsigned state,int x,int y,Time t,Widget *org);
  virtual ~Hslider();
  };

class Vslider : public Slider
  {
  public:
  Vslider();
  virtual int expose(XEvent *ev);
  virtual int user(unsigned c,unsigned state,int x,int y,Time t,Widget *org);
  virtual ~Vslider();
  };
