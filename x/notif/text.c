/* Text field widget
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

#include "notif.h"

/* Text widget */

static void textst(t,str)
Text *t;
char *str;
 {
 t->text=str;
 redraw(t);
 }

static char *textgt(t)
Text *t;
 {
 return t->text;
 }

static int textselect(t)
Text *t;
 {
 if(!t->state)
  {
  stbknd(t,stdfgnd);
  stfgnd(t,getgc("8x13","white"));
  t->state=1;
  redraw(t);
  }
 return 0;
 }

static int textunselect(t)
Text *t;
 {
 if(t->state)
  {
  stbknd(t,stdbknd);
  stfgnd(t,stdfgnd);
  t->state=0;
  redraw(t);
  }
 return 0;
 }

static int textdraw(t)
Text *t;
 {
 drawclr(t);
 if(t->text)
  {
  if(gtflg(t)&flgactivated) drawtxt(t,gtfgnd(t),t->xpos,t->ypos,t->text);
  else drawtxt(t,getgc("8x13","Dim Grey"),t->xpos,t->ypos,t->text);
  }
 return 0;
 }

static int textactivate(t)
Text *t;
 {
 (widgetfuncs.activate)(t);
 redraw(t);
 }

static int textdeactivate(t)
Text *t;
 {
 (widgetfuncs.deactivate)(t);
 redraw(t);
 }

static int textgtheight(t)
Text *t;
 {
 if(t->ypos== -1)
  {
  if(!ckh(t))
   stheight(t,txth(gtfgnd(t),"M"));
  t->ypos=(widgetfuncs.gtheight)(t)/2-
          txth(gtfgnd(t),"M")/2+txtb(gtfgnd(t),"M");
  }
 return (widgetfuncs.gtheight)(t);
 }

static int textgtwidth(t)
Text *t;
 {
 if(t->xpos== -1)
  {
  if(!ckw(t))
   if(t->text)
    stwidth(t,txtw(gtfgnd(t),t->text)+txtw(gtfgnd(t),"M"));
   else
    stwidth(t,64);
  if(t->text)
   t->xpos=(widgetfuncs.gtwidth)(t)/2-txtw(gtfgnd(t),t->text)/2;
  else
   t->xpos=txtw(gtfgnd(t),"M")/2;
  }
 return (widgetfuncs.gtwidth)(t);
 }

struct textfuncs textfuncs;

Text *mkText(t)
Text *t;
 {
 mkWidget(t);
 if(!textfuncs.on)
  {
  mcpy(&textfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  textfuncs.st=textst;
  textfuncs.gt=textgt;
  textfuncs.gtheight=textgtheight;
  textfuncs.gtwidth=textgtwidth;
  textfuncs.expose=textdraw;
  textfuncs.select=textselect;
  textfuncs.unselect=textunselect;
  textfuncs.activate=textactivate;
  textfuncs.deactivate=textdeactivate;
  }
 t->funcs= &textfuncs;
 t->text=0;
 t->xpos= -1;
 t->ypos= -1;
 t->state=0;
 return t;
 }
