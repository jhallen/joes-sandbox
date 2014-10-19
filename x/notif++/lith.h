/* Base class for windows and widgets
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

/* Virtual functions */

struct Pmgr
  {
  // Object which controls  placement needs these
  // These could be freed after placement is complete.
  int lrel, rrel, wid, hmode, hofst;		// Horz. positioning mode
  int alrel, arrel, awid, ahmode, ahofst;	// Aux horz. positioning mode
  int trel, brel, hgt, vmode, vofst;		// Vert. positioning mode
  int atrel, abrel, ahgt, avmode, avofst;	// Aux Vert. positioning mode
  Pmgr();
  };

class Lith
  {
  public:

  Link<Lith> link;	/* Our siblings */
  Lith *in;		/* Widget we're in */
  Window win;		/* X window we're associated with */
  int x, y;		/* Position of window */
  int w, h;		/* Size of window */
  Link<Lith> kids;	/* Child widgets */
  int lofst, rofst, tofst, bofst;
                        // Relative position for placement manager
  Pmgr *pmgr;		// Set if this Lith controls placement of its kids

  /* Flags */
  bool is_menu;
  bool is_cascade;
  bool on_flag;
  bool enabled_flag;
  bool focusable_flag;
  bool override_flag;
  bool transient_flag;
  bool activated_flag;

  GC fgnd;		/* Foreground gc */

  virtual Window gtwin();	/* Return X window for drawing */
  virtual Lith *gtmain();	/* Get drawing widget of this widget */
  virtual Lith *gt2nd();	/* Get scroll mask or if none, main */
  virtual Lith *gtouter();	/* Get outermost componant of this widget */
  virtual void stw(int n);	/* Set outside width */
  virtual int gtw();		/* Query outside width */
  virtual void sth(int n);	/* Set outside height */
  virtual int gth();		/* Query outside height */
  virtual void stx(int n);	/* Set X position of window */
  virtual int gtx();		/* Query X position */
  virtual void sty(int n);	/* Set Y position of window */
  virtual int gty();		/* Query Y position */
  virtual void stiw(int n);	/* Set inside width */
  virtual int gtiw();		/* Query inside width */
  virtual void stih(int n);	/* Set inside height */
  virtual int gtih();		/* Query inside height */
  virtual void stwidth(int n);	/* Set drawing area width */
  virtual int gtwidth();	/* Get drawing area width */
  virtual void stheight(int n);	/* Set drawing area height */
  virtual int gtheight();	/* Get drawing area height */
  virtual int ckx();		/* Check if x position has been set */
  virtual int cky();		/* Check if y position has been set */
  virtual int ckw();		/* Check if width has been set */
  virtual int ckh();		/* Check if height has been set */
  virtual void on();		/* Turn lith on if it is enabled */
  virtual void off();		/* Turn lith off */
  virtual void localon();	/* Local on routine */
  virtual void localoff();	/* Local off routine */
  virtual void enable();	/* Allow widget to go on */
  virtual void disable();	/* Disallow widget from going on */

  virtual void stmom(Lith *n);	/* Set widget's parent */
  virtual Lith *gtmom();	/* Get widget's parent */
  virtual void clmom();		/* Remove widget's parent */
  virtual void stbknd(GC n);	/* Set background GC */
  virtual GC gtbknd();		/* Get background GC */
  virtual void stpixmap(Pixmap n);	/* Set background pixmap */
  virtual void stcursor(Cursor n);	/* Set background cursor */
  virtual void sttitle(char *n);/* Set background title */
  virtual void stfgnd(GC n);	/* Set foreground GC */
  virtual GC gtfgnd();		/* Get foreground GC */

  virtual void add(Lith *n);	/* Add widget to this widget */
  virtual void ins(int n,Lith *x);		/* Insert widget into this widget */
  virtual Lith *nth(int n);	/* Get nth widget */
  virtual int no();		/* Get no. widgets */
  virtual int find(Lith *x);	/* Find pos no. for widget */
  virtual void rmv(Lith *x);	/* Remove a widget */
  virtual void *apply(Fn_1<void *,Lith *> *func);	/* Apply func to all kids */
  virtual void *xyapply(int x,int y,Fn_1<void *,Lith *> *func);	/* Apply func to all kids */

  virtual void stoverride();
  virtual void cloverride();
  virtual void sttransient();
  virtual void cltransient();
  virtual void stfocusable();
  virtual void clfocusable();

  /* Notif events */
  virtual int user(unsigned button,unsigned state,int x,int y,Time time,Widget *org);		/* User action event */
  virtual int focus();		/* Attained focus event */
  virtual int unfocus();	/* Lost focus event */
  virtual int trigger();	/* Button trigger */
  virtual int reset();		/* Button reset */
  virtual int select();		/* Item selected */
  virtual int unselect();	/* Item unselected */
  virtual int activate();	/* Item activated */
  virtual int deactivate();	/* Item deactivated */
  virtual int focusrtn();	/* Attained return key focus */
  virtual int unfocusrtn();	/* Lost return-key focus */
  virtual int drop();		/* Something got dropped on us */

  /* X events */
  virtual int keypress(XEvent *ev);
  virtual int keyrelease(XEvent *ev);
  virtual int buttonpress(XEvent *ev);
  virtual int buttonrelease(XEvent *ev);
  virtual int motionnotify(XEvent *ev);
  virtual int enternotify(XEvent *ev);
  virtual int leavenotify(XEvent *ev);
  virtual int focusin(XEvent *ev);
  virtual int focusout(XEvent *ev);
  virtual int keymapnotify(XEvent *ev);
  virtual int expose(XEvent *ev);
  virtual int graphicsexpose(XEvent *ev);
  virtual int noexpose(XEvent *ev);
  virtual int visibilitynotify(XEvent *ev);
  virtual int createnotify(XEvent *ev);
  virtual int destroynotify(XEvent *ev);
  virtual int unmapnotify(XEvent *ev);
  virtual int mapnotify(XEvent *ev);
  virtual int maprequest(XEvent *ev);
  virtual int reparentnotify(XEvent *ev);
  virtual int configurenotify(XEvent *ev);
  virtual int configurerequest(XEvent *ev);
  virtual int gravitynotify(XEvent *ev);
  virtual int resizerequest(XEvent *ev);
  virtual int circulatenotify(XEvent *ev);
  virtual int circulaterequest(XEvent *ev);
  virtual int propertynotify(XEvent *ev);
  virtual int selectionclear(XEvent *ev);
  virtual int selectionrequest(XEvent *ev);
  virtual int selectionnotify(XEvent *ev);
  virtual int colormapnotify(XEvent *ev);
  virtual int clientmessage(XEvent *ev);
  virtual int mappingnotify(XEvent *ev);

  // Placement manager calls

  // Set positioning mode

  // Horz modes
  void ltor(Lith *rel,int ofst);	// Left to right
  void ledge(int ofst);			// Left edge
  void lsame(Lith *rel,int ofst);	// Left same as another
  void rtol(Lith *rel,int ofst);	// Right to left
  void redge(int ofst);			// Right edge
  void rsame(Lith *rel,int ofst);	// Right same as another
  void hcenter();			// Centered

  // Aux horz modes
  void auxltor(Lith *rel,int ofst);
  void auxledge(int ofst);
  void auxlsame(Lith *rel,int ofst);
  void auxrtol(Lith *rel,int ofst);
  void auxredge(int ofst);
  void auxrsame(Lith *rel,int ofst);

  // Vert modes
  void ttob(Lith *rel,int ofst);	// Top to bottom
  void tedge(int ofst);			// Top edge
  void tsame(Lith *rel,int ofst);	// Top same as another
  void btot(Lith *rel,int ofst);	// Bottom to top
  void bedge(int ofst);			// Bottom edge
  void bsame(Lith *rel,int ofst);	// Bottom same as another
  void vcenter();			// Centered

  // Aux vert modes
  void auxttob(Lith *rel,int ofst);
  void auxtedge(int ofst);
  void auxtsame(Lith *rel,int ofst);
  void auxbtot(Lith *rel,int ofst);
  void auxbedge(int ofst);
  void auxbsame(Lith *rel,int ofst);

  // Placement manager subroutines
  void dox(Lith *w);
  void doy(Lith *w);

  void calcx();				// Calculate X positions of kids
  void calcy();				// Calculate Y positions of kids
  void calcw();				// Calculate our width
  void calch();				// Calculate our height
  void place(Lith *w);			// Place a widget

  void *apply(void *(*func)(),void *arg);

  Lith();
  virtual ~Lith();
  };
