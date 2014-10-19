/* Toggle button 
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

class Toggle : public Widget
  {
  public:
  Fn_1<void,int> *fn;
  int state;
  Lith *dispon;
  Lith *dispoff;
  
  void disp();
  void press();
  virtual int user(unsigned c,unsigned state,int x,int y,Time time,Widget *org);
  virtual int enternotify(XEvent *ev);

  virtual void stfn(Fn_1<void,int> *fn);
  virtual void stdispon(Lith *w);
  virtual void stdispoff(Lith *w);
  virtual void st(int);
  virtual int gt();

  Toggle();
  virtual ~Toggle();
  };

class Mtoggle : public Toggle
  {
  public:
  Mtoggle();
  virtual ~Mtoggle();
  };
