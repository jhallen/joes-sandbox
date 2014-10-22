/* Base class for all screen objects
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

static void lithrm(w)
Lith *w;
 {
 while(!qempty(w->kids)) rm((Lith *)w->kids->next->data);
 if(gtmom(w)) rmv(gtmom(w),w);
 if(w->pmgr) rm(w->pmgr);
 deque(w->kids);
 free(w);
 }

static void lithstwidth(w,width)
Lith *w;
 {
 if(gtmain(w)->w!=width)
  {
  if(gtmain(w)!=w) stwidth(gtmain(w),width);
  w->w=width;
  if(w->pmgr) calcx(w->pmgr);
  }
 }

static int lithgtwidth(w)
Lith *w;
 {
 if(!ckw(w) && w->pmgr) calcw(w->pmgr);
 return w->w;
 }

static void lithstheight(w,h)
Lith *w;
 {
 if(gtmain(w)->h!=h)
  {
  if(gtmain(w)!=w) stheight(gtmain(w),h);
  w->h=h;
  if(w->pmgr) calcy(w->pmgr);
  }
 }

static int lithgtheight(w)
Lith *w;
 {
 if(!ckh(w) && w->pmgr) calch(w->pmgr);
 return w->h;
 }

static void lithstx(w,x)
Lith *w;
 {
 w->x=x;
 if(gtouter(w)!=w) stx(gtouter(w),x);
 }

static int lithgtx(w)
Lith *w;
 {
 if(w->x!=MAXINT) return w->x;
 else return 0;
 }

static void lithsty(w,y)
Lith *w;
 {
 w->y=y;
 if(gtouter(w)!=w) sty(gtouter(w),y);
 }

static int lithgty(w)
Lith *w;
 {
 if(w->y!=MAXINT) return w->y;
 else return 0;
 }

static int lithckw(w)
Lith *w;
 {
 return w->w!=-1;
 }

static int lithckh(w)
Lith *w;
 {
 return w->h!=-1;
 }

static int lithckx(w)
Lith *w;
 {
 return w->x!=MAXINT;
 }

static int lithcky(w)
Lith *w;
 {
 return w->y!=MAXINT;
 }

static void lithon(w)
Lith *w;
 {
 if((gtflg(w)&flgenabled) && !(gtflg(w)&flgon))
  {
  LINK *l;
  gtwidth(w); gtheight(w);
  if(gtouter(w)!=w)
   {
   if(ckx(w)) stx(gtouter(w),gtx(w));
   if(cky(w)) sty(gtouter(w),gty(w));
   if(gtflg(w)&flgoverride) stflg(gtouter(w),gtflg(gtouter(w))|flgoverride);
   if(gtflg(w)&flgtransient) stflg(gtouter(w),gtflg(gtouter(w))|flgtransient);
   }
  w->funcs->localon(w);
  stflg(w,gtflg(w)|flgon);
  for(l=w->kids->next;l!=w->kids;l=l->next) on((Lith *)l->data);
  }
 }

static void lithoff(w)
Lith *w;
 {
 if(gtflg(w)&flgon)
  {
  LINK *l;
  for(l=w->kids->next;l!=w->kids;l=l->next) off((Lith *)l->data);
  w->funcs->localoff(w);
  stflg(w,gtflg(w)&~flgon);
  }
 }

static void lithenable(w)
Lith *w;
 {
 stflg(w,gtflg(w)|flgenabled);
 if(gtmom(w) && (gtflg(gtmom(w))&flgon)) on(w);
 }

static void lithdisable(w)
Lith *w;
 {
 stflg(w,gtflg(w)&~flgenabled);
 off(w);
 }

static void lithstflg(w,n)
Lith *w;
 {
 w->flags=n;
 }

static int lithgtflg(w)
Lith *w;
 {
 return w->flags;
 }

static Window lithgtwin(w)
Lith *w;
 {
 return w->win;
 }

static void lithstmom(w,x)
Lith *w, *x;
 {
 w->in=x;
 if(x->pmgr) add(x->pmgr,w);
 if(gtflg(x)&flgon) on(w);
 }

static Lith *lithgtmom(w)
Lith *w;
 {
 return w->in;
 }

static Lith *lithgtmain(w)
Lith *w;
 {
 return w;
 }

static void lithclmom(w)
Lith *w;
 {
 off(w);
 if(w->in && w->in->pmgr) rmv(w->in->pmgr,w);
 w->in=0;
 }

static void lithstbknd(w,gc)
Lith *w;
GC gc;
 {
 Lith *d=gtmain(w);
 if(d!=w) stbknd(d,gc);
 }

static GC lithgtbknd(w)
Lith *w;
 {
 Lith *d=gtmain(w);
 if(d!=w) return gtbknd(d);
 else return 0;
 }

static void lithstpixmap(w,pix)
Lith *w;
Pixmap pix;
 {
 Lith *d=gtmain(w);
 if(d!=w) stpixmap(d,pix);
 }

static void lithstcursor(w,cursor)
Lith *w;
Cursor cursor;
 {
 Lith *d=gtmain(w);
 if(d!=w) stcursor(d,cursor);
 }

static void lithsttitle(w,title)
Lith *w;
char *title;
 {
 Lith *d=gtouter(w);
 if(d!=w) sttitle(d,title);
 }

static void lithstfgnd(w,gc)
Lith *w;
GC gc;
 {
 Lith *d;
 w->fgnd=gc;
 d=gtmain(w);
 if(d!=w) stfgnd(d,gc);
 }

static GC lithgtfgnd(w)
Lith *w;
 {
 return w->fgnd;
 }

static void lithadd(w,x)
Lith *w, *x;
 {
 enqueb(w->kids,x);
 stmom(x,w);
 }

static void lithins(w,n,x)
Lith *w, *x;
 {
 enqueb(qnth(w->kids->next,n),x);
 stmom(x,w);
 }

static Lith *lithnth(w,n)
Lith *w;
 {
 return (Lith *)(qnth(w->kids->next,n)->data);
 }

static int lithno(w)
Lith *w;
 {
 return qno(w->kids);
 }

static int lithfind(w,x)
Lith *w, *x;
 {
 return qfindn(w->kids,x);
 }

static void lithrmv(w,x)
Lith *w, *x;
 {
 clmom(x);
 deque(qfind(w->kids,x));
 }

static void *lithapply(w,func,arg)
Lith *w;
void *(*func)();
void *arg;
 {
 LINK *l;
 for(l=w->kids->next;l!=w->kids;l=l->next)
  {
  Lith *q=(Lith *)l->data;
  void *rtn;
  if(rtn=func(q,arg)) return rtn;
  if(rtn=apply(q,func,arg)) return rtn;
  }
 return 0;
 }

static void lithstpmgr(w,x)
Lith *w;
Pmgr *x;
 {
 if(w->pmgr) rm(w->pmgr);
 w->pmgr=x;
 if(x) sttarget(x,w);
 }

static void lithvoid() {}

struct lithfuncs lithfuncs=
 {
 1, lithrm, lithgtwin, lithgtmain, lithgtmain, lithgtmain, lithstwidth,
 lithgtwidth, lithstheight, lithgtheight, lithstx, lithgtx, lithsty,
 lithgty, lithstwidth, lithgtwidth, lithstheight, lithgtheight, lithstwidth,
 lithgtwidth, lithstheight, lithgtheight, lithckx, lithcky, lithckw,
 lithckh, lithon, lithoff, lithvoid, lithvoid, lithenable, lithdisable,
 lithstflg, lithgtflg, lithstmom, lithgtmom, lithclmom, lithstbknd,
 lithgtbknd, lithstpixmap, lithstcursor, lithsttitle, lithstfgnd,
 lithgtfgnd, lithadd, lithins, lithnth, lithno, lithfind, lithrmv,
 lithapply, lithstpmgr
 };

Lith *mkLith(w)
Lith *w;
 {
 w->funcs= &lithfuncs;
 w->in=0;
 w->x=w->y= MAXINT;
 w->w=w->h= -1;
 w->kids=newque(NULL);
 w->flags=(flgenabled|flgactivated);
 w->fgnd=stdfgnd;
 w->pmgr=0;
 return w;
 }

void *xyapply(w,x,y,func,arg)
Lith *w;
void *(*func)();
void *arg;
 {
 LINK *l;
 for(l=w->kids->next;l!=w->kids;l=l->next)
  {
  Lith *q=(Lith *)l->data;
  void *rtn;
  if(x>=gtx(q) && x<gtx(q)+gtw(q) &&
     y>=gty(q) && y<gty(q)+gth(q))
   {
   if(rtn=func(q,arg)) return rtn;
   if(rtn=xyapply(q,x-gtx(q),y-gty(q),func,arg)) return rtn;
   }
  }
 return 0;
 }
