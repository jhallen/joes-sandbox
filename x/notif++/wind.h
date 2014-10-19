/* Primitive window
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

/* Member functions */

class Wind : public Lith
  {
  public:
  Widget *target;
  GC bknd;
  Pixmap pixmap;
  Cursor cursor;
  char *title;

  virtual void stwidth(int n);
  virtual void stheight(int n);
  virtual void stx(int n);
  virtual void sty(int n);
  virtual void localon();
  virtual void localoff();
  virtual void stbknd(GC gc);
  virtual GC gtbknd();
  virtual void stpixmap(Pixmap pix);
  virtual void sttitle(char *s);
  virtual void stcursor(Cursor c);
  virtual void sttarget(Widget *n);
  virtual int configurenotify(XEvent *ev);
  virtual int focusin();
  virtual int focusout();

  Wind();
  };

/* Window manager hints */

struct hints
  {
  int flg;
  XSizeHints normal_hints;
  XTextProperty window_name;
  XTextProperty icon_name;
  XWMHints wm_hints;
  char **argv;
  int argc;
  XClassHint class_hints;
  hints();
  };

/* Hints: these should be moved to Widget class */
void hintwindowname(char *s);
void hinticonname(char *s);
void hintposition(int a, int b);
void hintminsize(int a, int b);
void hintmaxsize(int a, int b);
void hintresizeinc(int a,int b);
void hintaspect(int a, int b, int c, int d);
void hintgravity(int a);
void hintargs(char **argv,int argc);
void hintinput(int n);
void hinticonified(int n);
void hinticonpixmap(Pixmap n);
void hinticonwindow(Window n);
void hinticonposition(int a,int b);
void hinticonmask(Pixmap n);
void hintgroup(XID n);
void hinturgent();
void hintclass(char *a,char *b);
