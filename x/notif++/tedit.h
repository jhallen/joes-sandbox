/* Tedit window 
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

class Tedit : public Widget
  {
  public:
  int xpos, ypos;	/* Position of text */
  int havefocus;	/* Set if we have focus */
  int nfields;		/* No. of fields */
  char **fields;	/* Field buffers */
  int *lens;		/* Field buffer lengths */
  int *sizs;		/* Field buffer malloc sizes */
  int *wids;		/* Field widths */
  int *poss;		/* Field positions */
  int *flgs;		/* Field formatting flags */
  int *data0;		/* Field formatting data */
  int *data1;		/* Field formatting data */
  int *data2;		/* Field formatting data */
  int cur;		/* Byte offset to cursor with current field */
  int curfield;		/* Current field of cursor */
  int tediting;		/* Set if we're tediting, clr if we just got focus */
  int ofst;		/* Scroll offset */
  int curpos;		/* Screen position of cursor */
  int update_flag;	/* Set if we need screen update */
  Fn_0<void> *blink;	/* Cursor blink timer */
  Widget *focusrtn;	/* Who should get focus on return */
  int pushmode;		/* Push edit mode */
  Fn_0<void> *fn;	/* Function to call for return key */

  int pos();
  virtual int expose(XEvent *ev);
  void curoff();
  virtual int focus();
  void update();
  virtual int unfocus();
  int ubacks(int key,int state,int xx,int y,Time time,Widget *org);
  int utype(int key,int state,int xx,int y,Time time,Widget *org);
  int urtn(int key,int state,int xx,int y,Time time,Widget *org);
  virtual int gtheight();
  int upress1(int key,int state,int xx,int y,Time time,Widget *org);


  void st(char *);			/* Set pattern buffer */
  void stfield(int x,char *val);	/* Set field */
  char **gt();				/* Get fields array */
  virtual void stfn(Fn_0<void> *fn);	/* Set return key callback function */
  void strtn(Widget *);			/* Set return-key focus */

  Tedit();
  virtual ~Tedit();
  };
