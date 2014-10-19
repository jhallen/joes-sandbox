/* Button widget 
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

class Button : public Widget
  {
  int rept;
  int rate;
  int delay;
  Fn_0<void> *fn;
  Fn_0<void> *tevent;

  public:

  virtual void buttonpress();
  virtual void buttonrept();
  virtual void buttonrelease();
  virtual int user(unsigned c,unsigned state,int x,int y,Time t,Widget *org);

  virtual void stfn(Fn_0<void> *new_fn);
  virtual Fn_0<void> *gtfn();
  virtual void strate(int n);
  virtual void stdelay(int n);
  virtual void sttext(char *s);
  virtual void sticon(int *i);
  virtual int enternotify(XEvent *ev);
  

  Button();
  virtual ~Button();
  };

class Mbutton : public Button
  {
  public:
  Mbutton();
  };
