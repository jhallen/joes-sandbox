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

#define TEDITFUNCS \
 WIDGETFUNCS \
 void (*st)();		/* Set pattern buffer */		\
 void (*stfield)();	/* Set field */				\
 char **(*gt)();	/* Get fields array */			\
 void (*stfn)();	/* Set return-key call-back function */	\
 void (*strtn)();	/* Set return-key focus */		\

#define TEDITVARS \
 WIDGETVARS \
 int xpos, ypos;	/* Position of text */			\
 int havefocus;		/* Set if we have focus */		\
\
 int nfields;		/* No. of fields */			\
 char **fields;		/* Field buffers */			\
 int *lens;		/* Field buffer lengths */		\
 int *sizs;		/* Field buffer malloc sizes */		\
 int *wids;		/* Field widths */			\
 int *poss;		/* Field positions */			\
 int *flgs;		/* Field formatting flags */		\
 int *data0;		/* Field formatting data */		\
 int *data1;		/* Field formatting data */		\
 int *data2;		/* Field formatting data */		\
\
 int cur;		/* Byte offset to cursor with current field */	\
 int curfield;		/* Current field of cursor */		\
 int tediting;		/* Set if we're tediting, clr if we just got focus */\
 int ofst;		/* Scroll offset */			\
 int curpos;		/* Screen position of cursor */		\
 int update;		/* Set if we need screen update */	\
 TASK blink[1];		/* Cursor blink timer */		\
 Widget *focusrtn;	/* Who should get focus on return */	\
 int pushmode;		/* Push edit mode */			\
 TASK *fn;		/* Function to call for return key */

extern struct teditfuncs { TEDITFUNCS } teditfuncs;
struct teditvars { struct teditfuncs *funcs; TEDITVARS };

Tedit *mkTedit();
