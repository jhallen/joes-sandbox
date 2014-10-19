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

#include "notif.h"

void Menu::on()
  {
  Lith::on();
  if(on_flag)
    {
    stfocus((Widget *)nth(0));
    grab(this,greedy);
    }
  }

void Menu::ungrab()
  {
  disable();
  }

int Menu::umenupress1(int key,int state,int x,int y,Time time,Widget *org)
  {
  if(x<0 || x>=gtwidth() || y<0 || y>=gtheight())
    { /* Mouse press is out of our window */
    popgrab();
    }
  else
    { /* Mouse press is in our window */
    calcpointer();
    /* Event is for one of our kids maybe */
    doevent(gtmain(),&ev);
    }
  return 0;
  }

int Menu::umenurelease1(int key,int state,int x,int y,Time time,Widget *org)
  {
  /*  printf("Menu release %d,%d greedy=%d\n",x,y,greedy); */
  greedygrab(1);
  stfocus((Widget *)nth(0));
  if(x>=0 && x<gtw() && y>=0 && y<gth())
    {
    ev.type=ButtonPress;
    calcpointer();
    doevent(gtmain(),&ev);
    calcpointer();
    ev.type=ButtonRelease;
    doevent(gtmain(),&ev);
    }
  return 0;
  }

int Menu::umenuleft(int key,int state,int x,int y,Time time,Widget *org)
  {
  grabunwind();
  if(whofocus)
    {
    whofocus->user(XK_Left,0,0,0,0,0);
    if(whofocus->trap && whofocus->trap->is_cascade)
      whofocus->user(32,0,0,0,0,0);
    }
  return 0;
  }

int Menu::umenuright(int key,int state,int x,int y,Time time,Widget *org)
  {
  grabunwind();
  if(whofocus)
    {
    whofocus->user(XK_Right,0,0,0,0,0);
    if(whofocus->trap && whofocus->trap->is_cascade)
      whofocus->user(32,0,0,0,0,0);
    }
  return 0;
  }

int Menu::umenuesc(int key,int state,int x,int y,Time time,Widget *org)
  {
  zapgrab();
  zapgrab();
  return 0;
  }

int menukmapset=0;
Kmap<Key> menukmap;

Menu::Menu()
  {
  if(!menukmapset)
    {
    menukmapset=1;
    menukmap.add("Left",new Mfn0_6<int,Menu,int,int,int,int,Time,Widget *>(&Menu::umenuleft,NULL));
    menukmap.add("Right",new Mfn0_6<int,Menu,int,int,int,int,Time,Widget *>(&Menu::umenuright,NULL));
    menukmap.add("^[",new Mfn0_6<int,Menu,int,int,int,int,Time,Widget *>(&Menu::umenuesc,NULL));
    menukmap.add("Press1",new Mfn0_6<int,Menu,int,int,int,int,Time,Widget *>(&Menu::umenupress1,NULL));
    menukmap.add("Release1",new Mfn0_6<int,Menu,int,int,int,int,Time,Widget *>(&Menu::umenurelease1,NULL));
    menukmap.add("^I",new Mfn0_6<int,Menu,int,int,int,int,Time,Widget *>(&Widget::ufocusnext,NULL));
    menukmap.add("Down",new Mfn0_6<int,Menu,int,int,int,int,Time,Widget *>(&Widget::ufocusnext,NULL));
    menukmap.add("Up",new Mfn0_6<int,Menu,int,int,int,int,Time,Widget *>(&Widget::ufocusprev,NULL));
    }
  stkmap(&menukmap);
  stborder(new Shadow());
  stoverride();
  ttob(NULL,0);
  ledge(0);
  auxredge(0);
  disable();
  is_menu = 1;
  }
