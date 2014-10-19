/* Event handler 
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

#ifndef _Ievent
#define _Ievent 1

void izws();			/* Initialize windowing system */
int ws();			/* Handle pending windowsing system events */
void wsflsh();			/* Flush windowing system output */
void doevent(Wind *root,XEvent *ev);	/* Send an event */

int userevent(Widget *x,XEvent *ev);

/* Global variabels */
extern Display *dsp;		/* Primary display */
extern XEvent ev;		/* Current event */
extern XContext wins;

/* Default GCs */
extern GC stdbknd;
extern GC stdfgnd;
extern GC stdtopbknd;
extern GC stdborderbknd;
extern GC stdshadowbknd;
extern GC stdshadowtop;
extern GC stdshadowbot;
extern GC stdoutline;

/* Our own events */
#define Trigger (LASTEvent)
#define Reset (LASTEvent+1)

Widget *gttop(Widget *x);
Window calcmom(Window win);
void calcpos(Window win,int *x,int *y);
void calcpointer();

extern Widget *grabber;	/* Current grabber */
extern int greedy;	/* Set for greedy grab */
void grab(Widget *w,int n);		/* Push grab */
void greedygrab(int n);
void popgrab();		/* Pop grab */
void zapgrab();		/* Zap grab */
void grabunwind();	/* Zap all grabs */
void zapto(Widget *);
void abspos(Lith *w,int *x,int *y);

/* Mouse state */
#define MouseLeft 256
#define MouseMiddle 512
#define MouseRight 1024
#define MouseShift 1
#define MouseCtrl 4
#define MouseAlt 8

/* Who owns focus */
extern Widget *whofocus;
extern int focusflg;

#define XK_Press1 0x8000
#define XK_Press2 0x8001
#define XK_Press3 0x8002
#define XK_Release1 0x8003
#define XK_Release2 0x8004
#define XK_Release3 0x8005
#define XK_Motion 0x8006

void placedialog(Widget *cur,Widget *new_w);

void modal(Widget *w);
void modalpop();

#endif
