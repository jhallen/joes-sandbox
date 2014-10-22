/* Button widget 
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

#define BUTTONFUNCS \
 WIDGETFUNCS \
 void (*stfn)(); \
 void (*strate)(); \
 void (*stdelay)(); \
 void (*sttext)(); \
 void (*sticon)();

#define BUTTONVARS \
 WIDGETVARS \
 TASK *fn; /* Set if button is armed.  Clear if not. */ \
 int rept; /* 0=not-pressed, 1=pressed, 2=repeating */ \
 int rate; /* Repeat rate */ \
 int delay; /* Delay to first repeat */ \
 TASK tevent[1]; /* Repeat timer */

extern struct buttonfuncs { BUTTONFUNCS } buttonfuncs;
struct buttonvars { struct buttonfuncs *funcs; BUTTONVARS };

Button *mkButton();		/* Create a button widget */
Button *mkMbutton();		/* Create a button with border for menu */
