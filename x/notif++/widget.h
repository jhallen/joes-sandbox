/* Widget 
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

/* Widget member virtual functions */

// typedef int (Widget::*Key)(int c,int state,int x,int y,Time time,Widget *org);
typedef Fn_6<int,int,int,int,int,Time,Widget *> *Key;

extern Kmap<Key> stdkmap;

class Widget : public Lith
  {
  public:

  Wind *main;
  Wind *mask;
  Border *border;
  Wind *outer;
  struct hints *hints;
  Help *help;
  int focusn;
  Widget *trap;
  Widget *traprtn;

  Fn_0<void> *sizenotifyfn;
  // Keymap manegement
  Kmap<Key> *kmap;	// Keymap to use for this widget
  Kbd<Key> kbd;		// Keyboard handler

  virtual Wind *gtmain();
  virtual void stw(int n);
  virtual int gtw();
  virtual void sth(int n);
  virtual int gth();
  virtual void stiw(int n);
  virtual int gtiw();
  virtual void stwidth(int n);
  virtual void stih(int n);
  virtual int gtih();
  virtual void stheight(int n);
  virtual int gtlofst();
  virtual int gttofst();
  virtual void stlofst(int n);
  virtual void sttofst(int n);
  virtual void stscroll();
  virtual void localon();
  virtual void localoff();
  virtual Border *gtborder();
  virtual void stborder(Border *n);
  virtual Wind *gt2nd();
  virtual Wind *gtouter();
  virtual void sthelpmgr(Help *x);
  virtual int user(unsigned c,unsigned state,int x,int y,Time t,Widget *org);
  virtual int configurenotify(XEvent *ev);
  virtual int trigger();
  virtual int reset();
  virtual int focus();
  virtual int unfocus();
  virtual int select();
  virtual int unselect();
  virtual int activate();
  virtual int deactivate();
  virtual int focusrtn();
  virtual int unfocusrtn();
  virtual void stkmap(Kmap<Key> *n);
  virtual Kmap<Key> *gtkmap();
  virtual void stsizenotifyfn(Fn_0<void> *n);
  virtual void ungrab();
  virtual int keypress(XEvent *);
  virtual int keyrelease(XEvent *);
  virtual int buttonpress(XEvent *);
  virtual int buttonrelease(XEvent *);
  virtual int motionnotify(XEvent *);
  int ufocusnext(int c,int state,int x,int y,Time time,Widget *org);
  int ufocusprev(int c,int state,int x,int y,Time time,Widget *org);
  
  Widget();
  virtual ~Widget();
  };

Widget *gtfocus(Widget *w);
void clfocus(Widget *w);
void stfocus(Widget *w);
void stfocusrtn(Widget *w);

extern Widget *root;
