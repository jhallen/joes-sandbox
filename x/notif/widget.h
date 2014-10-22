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

#define WIDGETFUNCS \
 LITHFUNCS \
 void (*stborder)();	/* Set border widget */				\
 Border *(*gtborder)();	/* Get border widget */				\
 void (*sthelpmgr)();	/* Set help manager */				\
 void (*stlofst)(); 	/* Set scrolling left offset */			\
 void (*sttofst)();	/* Set scrolling top offset */			\
 int (*gtlofst)();	/* Get scrolling left offset */			\
 int (*gttofst)();	/* Get scrolling top offset */			\
 void (*stscroll)();	/* Make window scrollable */			\
 void (*stsizenotifyfn)();	/* Set callback */			\
 void (*stkmap)();	/* Set keymap */				\
 KMAP *(*gtkmap)();	/* Get keymap */				\
 /* Event handlers */							\
 int (*ungrab)();	/* Ungrab the pointer */			\
 KMAP *kmap;		/* Default kmap */				\

/* Widget variables */

#define WIDGETVARS \
 LITHVARS \
 Wind *main;		/* Primary drawing and event source window */	\
 Wind *mask;		/* Scroll-mask window */			\
 Border *border;	/* Border window */				\
 Wind *outer;		/* Outermost window */				\
 struct hints *hints;	/* Window manager hint list */			\
 Help *help;		/* Help manager */				\
 int focusn;		/* Next child no. for focus traversal */	\
 Widget *trap;		/* Destination of keyboard events */		\
 Widget *traprtn;	/* Destination of return key */			\
 TASK *sizenotifyfn;	/* Notify someone that we changed size */	\
 KMAP *kmap;		/* keybinding map to use for this widget */	\
 KBD *kbd;		/* Keyboard event handler */			\

extern struct widgetfuncs { WIDGETFUNCS } widgetfuncs;
struct widgetvars { struct widgetfuncs *funcs; WIDGETVARS };

Widget *mkWidget();	/* Create a widget */
Widget *gtfocus();
void clfocus();
void stfocus();
void stfocusrtn();

extern Widget *root;
