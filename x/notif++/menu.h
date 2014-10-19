/* Menu 
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

class Menu : public Widget
  {
  public:
  Menu();
  virtual void on();
  virtual void ungrab();
  int umenupress1(int key,int state,int x,int y,Time time,Widget *org);
  int umenurelease1(int key,int state,int x,int y,Time time,Widget *org);
  int umenuleft(int key,int state,int x,int y,Time time,Widget *org);
  int umenuright(int key,int state,int x,int y,Time time,Widget *org);
  int umenuesc(int key,int state,int x,int y,Time time,Widget *org);
  };
