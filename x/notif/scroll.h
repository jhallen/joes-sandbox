/* Scrollable region w/ optional scroll-bars 
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

#define SCROLLFUNCS \
 WIDGETFUNCS \
 void (*stvcfg)(); \
 void (*sthcfg)(); \
 void (*stsize)(); \
 void (*stvstep)(); \
 void (*sthstep)(); \
 int (*gtvstep)(); \
 int (*gthstep)(); \
 void (*st)(); \
 Widget *(*gt)(); \

#define SCROLLVARS \
 WIDGETVARS \
 Widget *scroll; \
 Bar *vbar; \
 Bar *hbar; \
 int vcfg; \
 int hcfg; \
 int size; \
 int vstep; \
 int hstep; \
 TASK fnhorz[1]; \
 TASK fnvert[1]; \
 TASK fnresize[1]; \

#define cfgnone 0
#define cfgrsvd 1
#define cfgin 2
#define cfgout 3

extern struct scrollfuncs { SCROLLFUNCS } scrollfuncs;
struct scrollvars { struct scrollfuncs *funcs; SCROLLVARS };

Scroll *mkScroll();
