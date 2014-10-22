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

#include "../lib/queue.h"

/* Virtual functions */

#define LITHFUNCS \
 int inhlvl;		/* Inheritance depth */				\
 void (*rm)();		/* Delete */					\
 Window (*gtwin)();	/* Return X window for drawing */		\
 Lith *(*gtmain)();	/* Get drawing widget of this widget */		\
 Lith *(*gt2nd)();	/* Get scroll mask or if none, main */		\
 Lith *(*gtouter)();	/* Get outermost componant of this widget */	\
 void (*stw)();		/* Set outside width */				\
 int (*gtw)();		/* Query outside width */			\
 void (*sth)();		/* Set outside height */			\
 int (*gth)();		/* Query outside height */			\
 void (*stx)();		/* Set X position of window */			\
 int (*gtx)();		/* Query X position */				\
 void (*sty)();		/* Set Y position of window */			\
 int (*gty)();		/* Query Y position */				\
 void (*stiw)();	/* Set inside width */				\
 int (*gtiw)();		/* Query inside width */			\
 void (*stih)();	/* Set inside height */				\
 int (*gtih)();		/* Query inside height */			\
 void (*stwidth)();	/* Set drawing area width */			\
 int (*gtwidth)();	/* Get drawing area width */			\
 void (*stheight)();	/* Set drawing area height */			\
 int (*gtheight)();	/* Get drawing area height */			\
 int (*ckx)();		/* Check if x position has been set */		\
 int (*cky)();		/* Check if y position has been set */		\
 int (*ckw)();		/* Check if width has been set */		\
 int (*ckh)();		/* Check if height has been set */		\
 void (*on)();		/* Turn lith on if it is enabled */		\
 void (*off)();		/* Turn lith off */				\
 void (*localon)();	/* Local on routine */				\
 void (*localoff)();	/* Local off routine */				\
 void (*enable)();	/* Allow widget to go on */			\
 void (*disable)();	/* Disallow widget from going on */		\
 void (*stflg)();	/* Set flag bits */				\
 int (*gtflg)();	/* Get flag bits */				\
 void (*stmom)();	/* Set widget's parent */			\
 Lith *(*gtmom)();	/* Get widget's parent */			\
 void (*clmom)();	/* Remove widget's parent */			\
 void (*stbknd)();	/* Set background GC */				\
 GC (*gtbknd)();	/* Get background GC */				\
 void (*stpixmap)();	/* Set background pixmap */			\
 void (*stcursor)();	/* Set background cursor */			\
 void (*sttitle)();	/* Set background title */			\
 void (*stfgnd)();	/* Set foreground GC */				\
 GC (*gtfgnd)();	/* Get foreground GC */				\
 void (*add)();		/* Add widget to this widget */			\
 void (*ins)();		/* Insert widget into this widget */		\
 Lith *(*nth)();	/* Get nth widget */				\
 int (*no)();		/* Get no. widgets */				\
 int (*find)();		/* Find pos no. for widget */			\
 void (*rmv)();		/* Remove a widget */				\
 void *(*apply)();	/* Apply func to all kids */			\
 void (*stpmgr)();	/* Set a placement manager */			\
 /* Notif events */							\
 int (*user)();		/* User action event */				\
 int (*focus)();	/* Attained focus event */			\
 int (*unfocus)();	/* Lost focus event */				\
 int (*trigger)();	/* Button trigger */				\
 int (*reset)();	/* Button reset */				\
 int (*select)();	/* Item selected */				\
 int (*unselect)();	/* Item unselected */				\
 int (*activate)();	/* Item activated */				\
 int (*deactivate)();	/* Item deactivated */				\
 int (*focusrtn)();	/* Attained return key focus */			\
 int (*unfocusrtn)();	/* Lost return-key focus */			\
 int (*drop)();		/* Something got dropped on us */		\
 /* X events */								\
 int (*dummy0)();							\
 int (*dummy1)();							\
 int (*keypress)();							\
 int (*keyrelease)();							\
 int (*buttonpress)();							\
 int (*buttonrelease)();						\
 int (*motionnotify)();							\
 int (*enternotify)();							\
 int (*leavenotify)();							\
 int (*focusin)();							\
 int (*focusout)();							\
 int (*keymapnotify)();							\
 int (*expose)();							\
 int (*graphicsexpose)();						\
 int (*noexpose)();							\
 int (*visibilitynotify)();						\
 int (*createnotify)();							\
 int (*destroynotify)();						\
 int (*unmapnotify)();							\
 int (*mapnotify)();							\
 int (*maprequest)();							\
 int (*reparentnotify)();						\
 int (*configurenotify)();						\
 int (*configurerequest)();						\
 int (*gravitynotify)();						\
 int (*resizerequest)();						\
 int (*circulatenotify)();						\
 int (*circulaterequest)();						\
 int (*propertynotify)();						\
 int (*selectionclear)();						\
 int (*selectionrequest)();						\
 int (*selectionnotify)();						\
 int (*colormapnotify)();						\
 int (*clientmessage)();						\
 int (*mappingnotify)();						\

/* Instance variables */

#define LITHVARS \
 Lith *in;		/* Widget we're in */				\
 Window win;		/* X window we're associated with */		\
 int x, y;		/* Position of window */			\
 int w, h;		/* Size of window */				\
 LINK *kids;		/* Child widgets */				\
 int flags;		/* Flag bits */					\
 GC fgnd;		/* Foreground gc */				\
 Pmgr *pmgr;		/* Placement manager */				\

extern struct lithfuncs { LITHFUNCS } lithfuncs;
struct lithvars { struct lithfuncs *funcs; LITHVARS };

Lith *mkLith();		/* Create a lith */

#define flgon		0x00000001	/* Set if lith is on */
#define flgenabled	0x00000002	/* Set if lith is enabled */
#define flgfocus	0x00000004	/* Set if lith is focusable */
#define flgoverride	0x00000008	/* Override redirect window */
#define flgtransient	0x00000010	/* Transient window */
#define flgactivated	0x00000020	/* Set if lith is activated */

void *xyapply();	/* Apply a function to all liths in at (x,y) coord */
