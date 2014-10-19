/* Edit window 
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

class Edit : public Widget
  {
  public:
  int xpos, ypos;	/* Position of text */
  int havefocus;	/* Set if we have focus */
  int bksize;		/* Malloc size of buf */
  int len;		/* String length of buf */
  char *buf;		/* Edit buffer */
  int cur;		/* Byte offset to cursor */
  int sel;		/* Byte offset to selected text */
  int to;		/* Byte offset to end of selected text */
  int start;		/* Selected text starting point */
  int editing;		/* Set if we're editing, clr if we just got focus */
  int ofst;		/* Scroll offset */
  int curpos;		/* Screen position of cursor */
  char *org;		/* Original text */
  int update_flag;	/* Set if we need update */
  Fn_0<void> *blink;	/* Cursor blink timer */
  Widget *focusrtn;	/* Who should get focus on return */
  Fn_0<void> *fn;	/* Function to call for return key */

  int pos();
  virtual int expose(XEvent *);
  void curoff();
  virtual int focus();
  int findofst(int x);
  void update();
  void ins(int x,char *s,int s_len);
  void del(int x,int s_len);
  virtual int unfocus();
  void startediting();
  int ubacks(int key,int state,int x,int y,Time time,Widget *org);
  void killselection();
  int ubol(int key,int state,int x,int y,Time time,Widget *org);
  int ueol(int key,int state,int x,int y,Time time,Widget *org);
  int ultarw(int key,int state,int x,int y,Time time,Widget *org);
  int urtarw(int key,int state,int x,int y,Time time,Widget *org);
  int udelch(int key,int state,int x,int y,Time time,Widget *org);
  int udeleol(int key,int state,int x,int y,Time time,Widget *org);
  int udellin(int key,int state,int x,int y,Time time,Widget *org);
  int urelease(int key,int state,int x,int y,Time time,Widget *org);
  int upress1(int key,int state,int x,int y,Time time,Widget *org);
  int upress2(int key,int state,int x,int y,Time time,Widget *org);
  int umotion(int key,int state,int x,int y,Time time,Widget *org);
  int utype(int key,int state,int x,int y,Time time,Widget *org);
  int urtn(int key,int state,int x,int y,Time time,Widget *org);
  int urestore(int key,int state,int x,int y,Time time,Widget *org);
  virtual int gtheight();
  virtual int selectionclear(XEvent *ev);
  virtual int selectionnotify(XEvent *evv);
  virtual int selectionrequest(XEvent *evv);

  virtual void st(char *s);
  virtual char *gt();
  virtual void stfn(Fn_0<void> *fn);
  virtual void strtn(Widget *w);

  Edit();
  virtual ~Edit();
  };
