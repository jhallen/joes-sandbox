/* Scroll bars 
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

#define BARFUNCS \
 WIDGETFUNCS \
 void (*stpos)(); \
 void (*stdsize)(); \
 void (*stwsize)(); \
 void (*ststep)(); \
 int (*gtstep)(); \
 void (*stfn)(); \

#define BARVARS \
 WIDGETVARS \
 Button *dec; \
 TASK fndec[1]; \
 Button *inc; \
 TASK fninc[1]; \
 Slider *slider; \
 TASK fnslide[1]; \
 int pos; \
 int wsize; \
 int dsize; \
 int step; \
 TASK *fn;

struct barfuncs { BARFUNCS };
extern struct barfuncs vbarfuncs;
extern struct barfuncs hbarfuncs;
struct barvars { struct barfuncs *funcs; BARVARS };

Bar *mkVbar();
Bar *mkHbar();
