/* Generic border window 
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

#define BORDERFUNCS \
 WINDFUNCS \
 int (*gtlmargin)(); \
 int (*gtrmargin)(); \
 int (*gttmargin)(); \
 int (*gtbmargin)(); \
 void (*stmargins)(); \
 void (*stvisual)(); \
 int (*gtvisual)();

#define BORDERVARS \
 WINDVARS \
 int lmargin, rmargin, tmargin, bmargin; \
 int visual;

extern struct borderfuncs { BORDERFUNCS } borderfuncs;
struct bordervars { struct borderfuncs *funcs; BORDERVARS };

Border *mkBorder();	/* Create a simple border window */

/* Visual event flags */
#define vfocus 1	/* Button has focus */
#define vpress 2	/* Button is pressed */
#define vselect 4	/* Item is selected */
#define venter 8	/* Pointer has entered widget */
#define vactive 16	/* Item is activated */
#define vfocusrtn 32	/* Item has return key focus */

/* Silly shadow effects */

#define SHADOWFUNCS \
 BORDERFUNCS \
 void (*ststyle)(); \
 void (*stfixed)();

#define SHADOWVARS \
 BORDERVARS \
 GC tgc, bgc, ogc; \
 int lowered; \
 int raised; \
 int outlined; \
 int shadow_margin; \
 int outline_margin;

extern struct shadowfuncs { SHADOWFUNCS } shadowfuncs;
struct shadowvars { struct shadowfuncs *funcs; SHADOWVARS };

Shadow *mkShadow();	/* Create a shadow border */

/* Intelligent border for buttons */

#define IBORDERFUNCS \
 SHADOWFUNCS \
 void (*ststate)();

#define IBORDERVARS \
 SHADOWVARS \
 int state; \

extern struct iborderfuncs { IBORDERFUNCS } iborderfuncs;
struct ibordervars { struct iborderfuncs *funcs; IBORDERVARS };

Iborder *mkIborder();	/* Create a shadow border */
Iborder *mkIkborder();

/* Intelligent border for menu buttons */

#define MBORDERFUNCS \
 IBORDERFUNCS

#define MBORDERVARS \
 IBORDERVARS

extern struct mborderfuncs { MBORDERFUNCS } mborderfuncs;
struct mbordervars { struct mborderfuncs *funcs; MBORDERVARS };

Mborder *mkMborder();	/* Create a shadow border */

/* Intelligent border for menu buttons */

#define TBORDERFUNCS \
 IBORDERFUNCS

#define TBORDERVARS \
 IBORDERVARS

extern struct tborderfuncs { TBORDERFUNCS } tborderfuncs;
struct tbordervars { struct tborderfuncs *funcs; TBORDERVARS };

Tborder *mkTborder();	/* Create a shadow border */

/* Intelligent border for edit boxes */

#define EBORDERFUNCS \
 IBORDERFUNCS

#define EBORDERVARS \
 IBORDERVARS

extern struct eborderfuncs { EBORDERFUNCS } eborderfuncs;
struct ebordervars { struct eborderfuncs *funcs; EBORDERVARS };

Eborder *mkEborder();	/* Create a shadow border */
Eborder *mkEkborder();

/* Border composed of a line with a title in the upper left side */

#define TITLEDFUNCS \
 BORDERFUNCS \
 void (*st)();

#define TITLEDVARS \
 BORDERVARS \
 Widget *msg; \

extern struct titledfuncs { TITLEDFUNCS } titledfuncs;
struct titledvars { struct titledfuncs *funcs; TITLEDVARS };

Titled *mkTitled();	/* Create a titled border */

/* Border which looks like a subwindow */

#define TITLEBARFUNCS \
 BORDERFUNCS \
 void (*st)();

#define TITLEBARVARS \
 BORDERVARS \
 char *text;

extern struct titlebarfuncs { TITLEBARFUNCS } titlebarfuncs;
struct titlebarvars { struct titlebarfuncs *funcs; TITLEBARVARS };

Titlebar *mkTitlebar();	/* Create a titlebar border */
