/* Radio buttons 
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

class Radio : public Widget
  {
  public:
  int state;
  List<Toggle *> list;
  Fn_1<void,int> *fn;

  virtual void stfn(Fn_1<void,int> *new_fn);
  virtual void st(int new_state);
  virtual int gt();
  virtual void groupadd(Toggle *w);

  Radio();
  virtual ~Radio();
  };
