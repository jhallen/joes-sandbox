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

#define EDITFUNCS \
 WIDGETFUNCS \
 void (*st)();		/* Set text buffer */			\
 char *(*gt)();		/* Get text buffer */			\
 void (*stfn)();	/* Set return-key call-back function */	\
 void (*strtn)();	/* Set return-key focus */		\

#define EDITVARS \
 WIDGETVARS \
 int xpos, ypos;	/* Position of text */			\
 int havefocus;		/* Set if we have focus */		\
 int bksize;		/* Malloc size of buf */		\
 int len;		/* String length of buf */		\
 char *buf;		/* Edit buffer */			\
 int cur;		/* Byte offset to cursor */		\
 int sel;		/* Byte offset to selected text */	\
 int to;		/* Byte offset to end of selected text */\
 int start;		/* Selected text starting point */	\
 int editing;		/* Set if we're editing, clr if we just got focus */\
 int ofst;		/* Scroll offset */			\
 int curpos;		/* Screen position of cursor */		\
 char *org;		/* Original text */			\
 int update;		/* Set if we need update */		\
 TASK blink[1];		/* Cursor blink timer */		\
 Widget *focusrtn;	/* Who should get focus on return */	\
 TASK *fn;		/* Function to call for return key */

extern struct editfuncs { EDITFUNCS } editfuncs;
struct editvars { struct editfuncs *funcs; EDITVARS };

Edit *mkEdit();
