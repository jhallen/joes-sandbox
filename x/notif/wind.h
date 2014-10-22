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

#define WINDFUNCS \
 LITHFUNCS \
 void (*sttarget)();	/* Set target widget */				\

/* Window private variables */

#define WINDVARS \
 LITHVARS \
 Widget *target;	/* Widget we belong to */			\
 GC bknd;		/* Background pixel */				\
 Pixmap pixmap;		/* Background pixmap */				\
 Cursor cursor;		/* Cursor */					\
 char *title;		/* Title */					\

extern struct windfuncs { WINDFUNCS } windfuncs;
struct windvars { struct windfuncs *funcs; WINDVARS };

Wind *mkWind();		/* Create a window */

/* Hints */
void hintwindowname();
void hinticonname();
void hintposition();
void hintminsize();
void hintmaxsize();
void hintresizeinc();
void hintaspect();
void hintgravity();
void hintargs();
void hintinput();
void hinticonified();
void hinticonpixmap();
void hinticonwindow();
void hinticonposition();
void hinticonmask();
void hintgroup();
void hinturgent();
void hintclass();
