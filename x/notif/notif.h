/* Main Notif header file
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

#ifndef _Inotiff
#define _Inotiff 1

#ifndef _Ixincs
#define _Ixincs 1
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#endif

#include "../lib/blocks.h"
#include "../lib/task.h"
#include "../lib/queue.h"
#include "../lib/kbd.h"
#include "../lib/io.h"

/* Types */

typedef struct lithvars Lith;
typedef struct widgetvars Widget;
typedef struct windvars Wind;
typedef struct pmgrvars Pmgr;
typedef struct shadowvars Shadow;
typedef struct ibordervars Iborder;
typedef struct ebordervars Eborder;
typedef struct mbordervars Mborder;
typedef struct tbordervars Tborder;
typedef struct helpvars Help;
typedef struct textvars Text;
typedef struct iconvars Icon;
typedef struct buttonvars Button;
typedef struct buttonvars Mbutton;
typedef struct cascadevars Cascade;
typedef struct cascadevars Mcascade;
typedef struct cascadevars Mbcascade;
typedef struct togglevars Toggle;
typedef struct togglevars Mtoggle;
typedef struct radiovars Radio;
typedef struct editvars Edit;
typedef struct bordervars Border;
typedef struct titledvars Titled;
typedef struct slidervars Slider;
typedef struct slidervars Vslider;
typedef struct slidervars Hslider;
typedef struct barvars Bar;
typedef struct barvars Hbar;
typedef struct barvars Vbar;
typedef struct listvars List;
typedef struct imagevars Image;
typedef struct scopevars Scope;
typedef struct indexvars Index;
typedef struct listvars Vlist;
typedef struct listbarvars Listbar;
typedef struct menuvars Menu;
typedef struct scrollvars Scroll;
typedef struct optionvars Option;
typedef struct dragvars Drag;
typedef struct gsepvars Gsep;
typedef struct teditvars Tedit;
typedef struct titlebarvars Titlebar;

/* Polymorphic function dispatch macros */

#define rm(wind) ((wind)->funcs->rm(wind))
#define dup(wind) ((wind)->funcs->dup(wind))
#define gtwin(wind) ((wind)->funcs->gtwin(wind))
#define gtmain(wind) ((wind)->funcs->gtmain(wind))
#define gtouter(wind) ((wind)->funcs->gtouter(wind))
#define stw(wind,n) ((wind)->funcs->stw((wind),(n)))
#define gtw(wind) ((wind)->funcs->gtw(wind))
#define compw(wind) ((wind)->funcs->compw(wind))
#define sth(wind,n) ((wind)->funcs->sth((wind),(n)))
#define gth(wind) ((wind)->funcs->gth(wind))
#define comph(wind) ((wind)->funcs->comph(wind))
#define stx(wind,n) ((wind)->funcs->stx((wind),(n)))
#define gtx(wind) ((wind)->funcs->gtx(wind))
#define sty(wind,n) ((wind)->funcs->sty((wind),(n)))
#define gty(wind) ((wind)->funcs->gty(wind))
#define stiw(w,x) ((w)->funcs->stiw((w),(x)))
#define gtiw(w) ((w)->funcs->gtiw(w))
#define stih(w,h) ((w)->funcs->stih((w),(h)))
#define gtih(w) ((w)->funcs->gtih(w))
#define ckw(w) ((w)->funcs->ckw(w))
#define ckh(w) ((w)->funcs->ckh(w))
#define ckx(w) ((w)->funcs->ckx(w))
#define cky(w) ((w)->funcs->cky(w))
#define on(wind) ((wind)->funcs->on(wind))
#define off(wind) ((wind)->funcs->off(wind))
#define enable(w) ((w)->funcs->enable(w))
#define disable(w) ((w)->funcs->disable(w))
#define stflg(wind,n) ((wind)->funcs->stflg((wind),(n)))
#define gtflg(wind) ((wind)->funcs->gtflg(wind))
#define stmom(wind,x) ((wind)->funcs->stmom((wind),(x)))
#define gtmom(wind) ((wind)->funcs->gtmom(wind))
#define clmom(wind) ((wind)->funcs->clmom(wind))
#define stbknd(wind,gc) ((wind)->funcs->stbknd(wind,gc))
#define gtbknd(wind) ((wind)->funcs->gtbknd(wind))
#define gtfgnd(w) ((w)->funcs->gtfgnd(w))
#define stfgnd(w,gc) ((w)->funcs->stfgnd((w),(gc)))
#define add(w,x) ((w)->funcs->add((w),(x)))
#define ins(w,n,x) ((w)->funcs->ins((w),(n),(x)))
#define nth(w,n) ((w)->funcs->nth((w),(n)))
#define no(w) ((w)->funcs->no(w))
#define find(w,x) ((x)->funcs->find((w),(x)))
#define rmv(w,x) ((w)->funcs->rmv((w),(x)))
#define apply(w,func,arg) ((w)->funcs->apply((w),(func),(arg)))
#define stpmgr(x,y) ((x)->funcs->stpmgr((x),(y)))
#define st(x,y) ((x)->funcs->st((x),(y)))
#define gt(x) ((x)->funcs->gt(x))
#define stfn(x,y) ((x)->funcs->stfn((x),(y)))
#define strate(x,y) ((x)->funcs->strate((x),(y)))
#define stdelay(x,y) ((x)->funcs->stdelay((x),(y)))
#define stdispon(x,y) ((x)->funcs->stdispon((x),(y)))
#define stdispoff(x,y) ((x)->funcs->stdispoff((x),(y)))
#define gtborder(w) ((w)->funcs->gtborder(w))
#define stborder(w,x) ((w)->funcs->stborder((w),(x)))
#define gt2nd(w) ((w)->funcs->gt2nd(w))
#define gtlmargin(w) ((w)->funcs->gtlmargin(w))
#define gtrmargin(w) ((w)->funcs->gtrmargin(w))
#define gttmargin(w) ((w)->funcs->gttmargin(w))
#define gtbmargin(w) ((w)->funcs->gtbmargin(w))
#define stmargins(w,l,r,t,b) ((w)->funcs->stmargins((w),(l),(r),(t),(b)))
#define sttarget(w,t) ((w)->funcs->sttarget((w),(t)))
#define stvisual(w,n) ((w)->funcs->stvisual((w),(n)))
#define gtvisual(w) ((w)->funcs->gtvisual(w))
#define ststyle(w,raised,lowered,outlined) \
 ((w)->funcs->ststyle((w),(raised),(lowered),(outlined)))
#define stfixed(w,a,b,c,d,e) ((w)->funcs->stfixed((w),(a),(b),(c),(d),(e)))
#define calcx(a) ((a)->funcs->calcx(a))
#define calcy(a) ((a)->funcs->calcy(a))
#define calcw(a) ((a)->funcs->calcw(a))
#define calch(a) ((a)->funcs->calch(a))
#define sttitle(a,b) ((a)->funcs->sttitle((a),(b)))
#define gttitle(a) ((a)->funcs->gttitle(a))
#define groupadd(a,b) ((a)->funcs->groupadd((a),(b)))
#define groupins(a,b,c) ((a)->funcs->groupins((a),(b),(c)))
#define groupno(a) ((a)->funcs->groupno(a))
#define groupnth(a,b) ((a)->funcs->groupnth(a))
#define grouprmv(a,b) ((a)->funcs->grouprmv((a),(b)))
#define groupapply(a,func,arg) ((a)->funcs->groupapply((a),(func),(arg)))
#define stdsize(b,n) ((b)->funcs->stdsize((b),(n)))
#define stwsize(b,n) ((b)->funcs->stwsize((b),(n)))
#define stpos(b,n) ((b)->funcs->stpos((b),(n)))
#define ststep(b,n) ((b)->funcs->ststep((b),(n)))
#define stvstep(b,n) ((b)->funcs->stvstep((b),(n)))
#define sthstep(b,n) ((b)->funcs->sthstep((b),(n)))
#define gtstep(b) ((b)->funcs->gtstep(b))
#define gtvstep(b) ((b)->funcs->gtvstep(b))
#define gthstep(b) ((b)->funcs->gthstep(b))
#define ststate(w,n) ((w)->funcs->ststate((w),(n)))
#define gtlofst(w) ((w)->funcs->gtlofst(w))
#define gttofst(w) ((w)->funcs->gttofst(w))
#define stlofst(w,n) ((w)->funcs->stlofst((w),(n)))
#define sttofst(w,n) ((w)->funcs->sttofst((w),(n)))
#define sthelp(w,x) ((w)->help->funcs->sthelp((w)->help,(x)))
#define sthelpmgr(w,x) ((w)->funcs->sthelpmgr((w),(x)))
#define stpixmap(w,x) ((w)->funcs->stpixmap((w),(x)))
#define stcursor(w,x) ((w)->funcs->stcursor((w),(x)))
#define sttext(w,s) ((w)->funcs->sttext((w),(s)))
#define sticon(w,s) ((w)->funcs->sticon((w),(s)))
#define strtn(w,x) ((w)->funcs->strtn((w),(x)))
#define stsize(w,n) ((w)->funcs->stsize((w),(n)))
#define stvcfg(w,n) ((w)->funcs->stvcfg((w),(n)))
#define sthcfg(w,n) ((w)->funcs->sthcfg((w),(n)))
#define stwidth(w,n) ((w)->funcs->stwidth((w),(n)))
#define stheight(w,n) ((w)->funcs->stheight((w),(n)))
#define gtwidth(w) ((w)->funcs->gtwidth(w))
#define gtheight(w) ((w)->funcs->gtheight(w))
#define stscroll(w) ((w)->funcs->stscroll(w))
#define stsizenotifyfn(w,v) ((w)->funcs->stsizenotifyfn((w),(v)))
#define gtkmap(w) ((w)->funcs->gtkmap(w))
#define stkmap(w,k) ((w)->funcs->stkmap((w),(k)))
#define gtwidget(w,n) ((w)->funcs->gtwidget((w),(n)))
#define gtlabel(w,n) ((w)->funcs->gtlabel((w),(n)))
#define addpair(w,a,b) ((w)->funcs->addpair((w),(a),(b)))
#define stdest(w,a) ((w)->funcs->stdest((w),(a)))
#define stfield(w,f,v) ((w)->funcs->stfield((w),(f),(v)))

/* Creation function */

#define mk(type) (mk##type(malloc(sizeof(type))))

/* First instance macro */

#define firstinst(new,old) \
 ( \
 new##funcs.inhlvl ? 0 : \
  ( \
  mcpy(&new##funcs,&old##funcs,sizeof(struct old##funcs)), \
  ++new##funcs.inhlvl, \
  new##funcs.kmap=mkkmap(NULL,NULL,NULL), \
  kcpy(new##funcs.kmap,old##funcs.kmap), \
  1 \
  ) \
 )

/* Inheritance macro */

/* Core notif */
#include "event.h"
#include "lith.h"
#include "wind.h"
#include "border.h"
#include "place.h"
#include "widget.h"
#include "draw.h"
#include "help.h"

/* Widgets */
#include "icons.h"
#include "button.h"
#include "dragdrop.h"
#include "toggle.h"
#include "radio.h"
#include "text.h"
#include "icon.h"
#include "edit.h"
#include "slider.h"
#include "bar.h"
#include "menu.h"
#include "cascade.h"
#include "scroll.h"
#include "list.h"
#include "image.h"
#include "scope.h"
#include "option.h"
#include "index.h"
#include "gsep.h"
#include "tedit.h"

#endif
