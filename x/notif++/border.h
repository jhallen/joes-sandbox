/* Generic border window 
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

/* Border window */

class Border : public Wind
  {
  public:
  int lmargin, rmargin, tmargin, bmargin;
  int visual;

  virtual int gtlmargin();
  virtual int gtrmargin();
  virtual int gttmargin();
  virtual int gtbmargin();
  virtual void stmargins(int l,int r,int t,int b);
  virtual void stvisual(int n);
  virtual int gtvisual();

  virtual int enternotify(XEvent *ev);
  virtual int leavenotify(XEvent *ev);

  Border();
  };

/* Silly shadow effects */

class Shadow : public Border
  {
  public:
  GC tgc, bgc, ogc;
  int lowered;
  int raised;
  int outlined;
  int shadow_margin;
  int outline_margin;
  virtual void ststyle(int raised,int lowered,int outlined);
  virtual void stfixed(int smargin,int omargin,GC tgc,GC bgc,GC ogc);
  virtual int expose(XEvent *ev);
  Shadow();
  };

/* Visual event flags */
#define vfocus 1	/* Button has focus */
#define vpress 2	/* Button is pressed */
#define vselect 4	/* Item is selected */
#define venter 8	/* Pointer has entered widget */
#define vactive 16	/* Item is activated */
#define vfocusrtn 32	/* Item has return key focus */

/* Intelligent border for buttons */

class Iborder : public Shadow
  {
  public:
  int state;
  virtual void ststate(int n);
  virtual int unfocus();
  virtual int focus();
  virtual int trigger();
  virtual int reset();
  virtual int select();
  virtual int unselect();
  virtual int focusrtn();
  virtual int unfocusrtn();
  virtual int activate();
  virtual int deactivate();
  virtual int enternotify(XEvent *ev);
  virtual int leavenotify(XEvent *ev);
  Iborder();
  };

/* Intelligent border for menu buttons */

class Mborder : public Iborder
  {
  public:
  Mborder();
  virtual void ststate(int n);
  };

/* Intelligent border for menu buttons */

class Tborder : public Iborder
  {
  public:
  Tborder();
  virtual void ststate(int n);
  };

/* Intelligent border for edit boxes */

class Eborder : public Iborder
  {
  public:
  Eborder();
  virtual void ststate(int n);
  };

/* Border composed of a line with a title in the upper left side */

class Titled : public Border
  {
  public:
  Widget *msg;
  void st(Widget *m);
  virtual int expose(XEvent *ev);
  Titled();
  };

/* Border which looks like a subwindow */

class Titlebar : public Border
  {
  public:
  char *text;
  void st(char *t);
  virtual int expose(XEvent *ev);
  Titlebar();
  };
