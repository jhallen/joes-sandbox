/* Standard widget
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

static void widgetrm(w)
Widget *w;
 {
 while(!qempty(w->kids)) rm((Widget *)w->kids->next->data);
 if(gtmom(w)) rmv(gtmom(w),w);
 if(w->kbd) rmkbd(w->kbd);
 if(w->help) rm(w->help);
 if(w->pmgr) rm(w->pmgr);
 rm(w->main);
 if(w->mask) rm(w->mask);
 if(w->border) rm(w->outer);
 if(w->hints) free(w->hints);
 cancel(w->sizenotifyfn); w->sizenotifyfn=0;
 deque(w->kids);
 free(w);
 }

static Wind *widgetgtmain(w)
Widget *w;
 {
 return w->main;
 }

static void widgetstw(w,n)
Widget *w;
 {
 int amnt=0;
 if(w->border) amnt=gtlmargin(w->border)+gtrmargin(w->border);
 stiw(w,n-amnt);
 }

static int widgetgtw(w)
Widget *w;
 {
 int amnt=0;
 if(w->border) amnt=gtlmargin(w->border)+gtrmargin(w->border);
 return gtiw(w)+amnt;
 }

static void widgetsth(w,n)
Widget *w;
 {
 int amnt=0;
 if(w->border) amnt=gttmargin(w->border)+gtbmargin(w->border);
 stih(w,n-amnt);
 }

static int widgetgth(w)
Widget *w;
 {
 int amnt=0;
 if(w->border) amnt=gttmargin(w->border)+gtbmargin(w->border);
 return gtih(w)+amnt;
 }

static void widgetstiw(w,n)
Widget *w;
 {
 if(gt2nd(w)==gtmain(w)) stwidth(w,n);
 else
  {
  if(w->border)
   {
   int amnt=gtlmargin(w->border)+gtrmargin(w->border);
   stwidth(w->outer,n+amnt);
   }
  stwidth(gt2nd(w),n);
  }
 }

static int widgetgtiw(w)
Widget *w;
 {
 if(gt2nd(w)==gtmain(w)) return gtwidth(w);
 else return gtwidth(gt2nd(w));
 }

static void widgetstwidth(w,n)
Widget *w;
 {
 if(gt2nd(w)==gtmain(w))
  {
  if(w->border)
   {
   int amnt=gtlmargin(w->border)+gtrmargin(w->border);
   stwidth(w->outer,n+amnt);
   }
  }
 (lithfuncs.stwidth)(w,n);
 cont0(w->sizenotifyfn); w->sizenotifyfn=0;
 }

static void widgetstih(w,n)
Widget *w;
 {
 if(gt2nd(w)==gtmain(w))
  stheight(w,n);
 else
  {
  if(w->border)
   {
   int amnt=gttmargin(w->border)+gtbmargin(w->border);
   stheight(w->outer,n+amnt);
   }
  stheight(gt2nd(w),n);
  }
 }

static int widgetgtih(w)
Widget *w;
 {
 if(gt2nd(w)==gtmain(w)) return gtheight(w);
 else return gtheight(gt2nd(w));
 }

static void widgetstheight(w,n)
Widget *w;
 {
 if(gt2nd(w)==gtmain(w))
  {
  if(w->border)
   {
   int amnt=gttmargin(w->border)+gtbmargin(w->border);
   stheight(w->outer,n+amnt);
   }
  }
 (lithfuncs.stheight)(w,n);
 cont0(w->sizenotifyfn); w->sizenotifyfn=0;
 }

static int widgetgtlofst(s)
Widget *s;
 {
 return -gtx(gtmain(s));
 }

static int widgetgttofst(s)
Widget *s;
 {
 return -gty(gtmain(s));
 }

static void widgetstlofst(s,n)
Widget *s;
 {
 stx(gtmain(s),-n);
 }

static void widgetsttofst(s,n)
Widget *s;
 {
 sty(gtmain(s),-n);
 }

static void widgetstscroll(w)
Widget *w;
 {
 w->mask=mk(Wind);
 stx(w->main,0);
 sty(w->main,0);
 if(!w->border) w->outer=w->mask;
 }

static void widgeton(w)
Widget *w;
 {
 if(gtborder(w))
  {
  stx(gt2nd(w),gtlmargin(gtborder(w)));
  sty(gt2nd(w),gttmargin(gtborder(w)));
  }
 if(gtborder(w))
  {
  stmom(gtborder(w),gtmain(gtmom(w)));
  stmom(gt2nd(w),gtborder(w));
  }
 else
  {
  stmom(gt2nd(w),gtmain(gtmom(w)));
  }
 if(gt2nd(w)!=gtmain(w))
  {
  stmom(gtmain(w),gt2nd(w));
  }
 w->win=gtwin(gtmain(w));
 }

static void widgetoff(w)
Widget *w;
 {
 if(gtfocus(w)==w) clfocus(w);
 clmom(gtmain(w));
 clmom(gt2nd(w));
 if(gtborder(w)) clmom(gtborder(w));
 }

static Border *widgetgtborder(w)
Widget *w;
 {
 return w->border;
 }

static void widgetstborder(w,x)
Widget *w;
Border *x;
 {
 if(w->border)
  {
  clmom(gt2nd(w));
  rm(w->border);
  }
 w->border=x;
 if(x)
  {
  w->outer=(Wind *)x;
  sttarget(x,w);
  }
 else w->outer=gt2nd(w);
 }

static Wind *widgetgt2nd(w)
Widget *w;
 {
 if(w->mask) return w->mask;
 else return w->main;
 }

static Wind *widgetgtouter(w)
Widget *w;
 {
 return w->outer;
 }

static void widgetsthelpmgr(w,x)
Widget *w;
Help *x;
 {
 if(w->help)
  rm(w->help);
 w->help=x;
 if(x)
  sttarget(x,w);
 }

static int widgetuser(w,c,state,x,y,time,org)
Widget *w;
Time time;
Wind *org;
 {
 KEY *bind;
 int normalkey=0;
 if(c!=XK_Press1 && c!=XK_Press2 && c!=XK_Press3 &&
    c!=XK_Release1 && c!=XK_Release2 && c!=XK_Release3 &&
    c!=XK_Motion)
  normalkey=1;
 if(normalkey && w->traprtn && w->traprtn->funcs->user && c==13)
  {
  int rtn=w->traprtn->funcs->user(w->traprtn,c,state,x,y,time,org);
  if(!rtn) return 0;
  }
 else if(normalkey && w->trap && w->trap->funcs->user)
  {
  int rtn=w->trap->funcs->user(w->trap,c,state,x,y,time,org);
  if(!rtn) return 0;
  }
 bind=dokey(w->kbd,c);			/* Do we have a command? */
 if(!bind)
  if(normalkey) return 0;		/* Eat normal keys */
  else return -1;			/* Key not accepted */
 else if(bind==&keytaken) return 0;	/* Prefix key */
 else return bind->func(0,w,bind->arg,c,state,x,y,time,org);
 }

static int widgetresize(x,ev)
Widget *x;
XEvent *ev;
 {
 int newx, newy;
 int w=ev->xconfigure.width;
 int h=ev->xconfigure.height;
 int xflg=0;
 int yflg=0;

 calcpos(ev->xany.window,&newx,&newy);

 if((Widget *)gtmom(x)!=root) return 0;

 /* Record window's position */
 gtouter(x)->x=newx;
 gtouter(x)->y=newy;
 x->x=newx;
 x->y=newy;

 if(w!=gtouter(x)->w) xflg=1;
 if(h!=gtouter(x)->h) yflg=1;
 if(xflg || yflg)
  {
/*  printf("Resize! (%d %d)->(%d %d)\n",gtouter(x)->w,gtouter(x)->h,w,h); */
  /* We can't call stw/sth here: it would cause an infinite loop */
  gtouter(x)->w=w;
  gtouter(x)->h=h;
  if(gtborder(x))
   {
   x->w=w-gtlmargin(gtborder(x))-gtrmargin(gtborder(x));
   x->h=h-gttmargin(gtborder(x))-gtbmargin(gtborder(x));
   stw(gt2nd(x),x->w);
   sth(gt2nd(x),x->h);
   }
  else
   {
   x->w=w;
   x->h=h;
   }
  /* Reposition children */
  if(x->pmgr)
   {
   if(xflg) calcx(x->pmgr);
   if(yflg) calcy(x->pmgr);
   }
  }
 return 0;
 }

static void *dotrigger(w)
Widget *w;
 {
 if(w->funcs->trigger) w->funcs->trigger(w);
 return NULL;
 }

static void widgettrigger(b)
Widget *b;
 {
 Wind *z;
 /* Tell the border */
 if((z=gtborder(b)) && z->funcs->trigger) z->funcs->trigger(z);
 /* Tell the kids */
 apply(b,dotrigger,NULL);
 }

static void *doreset(w)
Widget *w;
 {
 if(w->funcs->reset) w->funcs->reset(w);
 return NULL;
 }

static void widgetreset(b)
Widget *b;
 {
 Wind *z;
 /* Tell the border */
 if((z=gtborder(b)) && z->funcs->reset) z->funcs->reset(z);
 /* Tell the kids */
 apply(b,doreset,NULL);
 }

static void *dofocus(w)
Widget *w;
 {
 if(w->funcs->focus) w->funcs->focus(w);
 return NULL;
 }

static void widgetfocus(b)
Widget *b;
 {
 Wind *z;
 /* Tell the border */
 if((z=gtborder(b)) && z->funcs->focus) z->funcs->focus(z);
 /* Tell the kids */
 apply(b,dofocus,NULL);
 }

static void *dounfocus(w)
Widget *w;
 {
 if(w->funcs->unfocus) w->funcs->unfocus(w);
 return NULL;
 }

static void widgetunfocus(b)
Widget *b;
 {
 Wind *z;
 /* Tell the border */
 if((z=gtborder(b)) && z->funcs->unfocus) z->funcs->unfocus(z);
 /* Tell the kids */
 apply(b,dounfocus,NULL);
 }

static void *doselect(w)
Widget *w;
 {
 if(w->funcs->select) w->funcs->select(w);
 return NULL;
 }

static void widgetselect(b)
Widget *b;
 {
 Wind *z;
 /* Tell the border */
 if((z=gtborder(b)) && z->funcs->select) z->funcs->select(z);
 /* Tell the kids */
 apply(b,doselect,NULL);
 }

static void *dounselect(w)
Widget *w;
 {
 if(w->funcs->unselect) w->funcs->unselect(w);
 return NULL;
 }

static void widgetunselect(b)
Widget *b;
 {
 Wind *z;
 /* Tell the border */
 if((z=gtborder(b)) && z->funcs->unselect) z->funcs->unselect(z);
 /* Tell the kids */
 apply(b,dounselect,NULL);
 }

static void *doactivate(w)
Widget *w;
 {
 if(w->funcs->activate) w->funcs->activate(w);
 return NULL;
 }

static void widgetactivate(b)
Widget *b;
 {
 Wind *z;
 if(!(gtflg(b)&flgactivated))
  {
  stflg(b,gtflg(b)|flgactivated);
  /* Tell the border */
  if((z=gtborder(b)) && z->funcs->activate) z->funcs->activate(z);
  /* Tell the kids */
  apply(b,doactivate,NULL);
  }
 }

static void *dodeactivate(w)
Widget *w;
 {
 if(w->funcs->deactivate) w->funcs->deactivate(w);
 return NULL;
 }

static void widgetdeactivate(b)
Widget *b;
 {
 Wind *z;
 if(gtflg(b)&flgactivated)
  {
  stflg(b,gtflg(b)&~flgactivated);
  /* Tell the border */
  if((z=gtborder(b)) && z->funcs->deactivate) z->funcs->deactivate(z);
  /* Tell the kids */
  apply(b,dodeactivate,NULL);
  }
 }

static void *dofocusrtn(w)
Widget *w;
 {
 if(w->funcs->focusrtn) w->funcs->focusrtn(w);
 return NULL;
 }

static void widgetfocusrtn(b)
Widget *b;
 {
 Wind *z;
 /* Tell the border */
 if((z=gtborder(b)) && z->funcs->focusrtn) z->funcs->focusrtn(z);
 /* Tell the kids */
 apply(b,dofocusrtn,NULL);
 }

static void *dounfocusrtn(w)
Widget *w;
 {
 if(w->funcs->unfocusrtn) w->funcs->unfocusrtn(w);
 return NULL;
 }

static void widgetunfocusrtn(b)
Widget *b;
 {
 Wind *z;
 /* Tell the border */
 if((z=gtborder(b)) && z->funcs->unfocusrtn) z->funcs->unfocusrtn(z);
 /* Tell the kids */
 apply(b,dounfocusrtn,NULL);
 }

static void widgetstsizenotifyfn(b,v)
Widget *b;
TASK *v;
 {
 b->sizenotifyfn=v;
 }

static void widgetstkmap(w,kmap)
Widget *w;
KMAP *kmap;
 {
 w->kmap=kmap;
 if(w->kbd) rmkbd(w->kbd);
 w->kbd=mkkbd(w->kmap);
 }

static KMAP *widgetgtkmap(w)
Widget *w;
 {
 return w->kmap;
 }

static Widget *focusnext(t)
Widget *t;
 {
 int wrap=0;
 Widget *last=0;

 if((Widget *)gtmom(t)==root)
  {
  t->focusn= -1;
  goto down;
  }

 up:
 /* Leave current node */
 t=(Widget *)t->in;
 if(t==last) return t;

 /* Find next child */
 down:
 if(t->focusn==no(t)-1)
  { /* Done with this level: go up one unless we're at top */
  if((Widget *)gtmom(t)==root)
   {
   t->focusn= -1;
   if(++wrap==2) return 0;
   }
  else goto up;
  }

 /* Go to next kid */
 ++t->focusn;
 t=(Widget *)nth(t,t->focusn);

 t->focusn= -1;

 if((gtflg(t)&flgfocus) && (gtflg(t)&flgon))
  {
  last=t;
  if(no(t)) goto down;
  else return t;
  }
 else goto down;

 return t;
 }

static Widget *focusprev(t)
Widget *t;
 {
 int wrap=0;
 Widget *last=0;

 if((Widget *)gtmom(t)==root)
  {
  t->focusn=no(t);
  goto down;
  }

 up:
 /* Leave current node */
 t=(Widget *)t->in;
 if(t==last) return (Widget *)t;

 /* Find next child */
 down:
 if(t->focusn<=0)
  { /* Done with this level: go up one unless we're at top */
  if((Widget *)gtmom(t)==root)
   {
   t->focusn=no(t);
   if(++wrap==2) return 0;
   }
  else goto up;
  }

 /* Go to next kid */
 --t->focusn;
 t=(Widget *)nth(t,t->focusn);

 t->focusn=no(t);

 if((gtflg(t)&flgfocus) && (gtflg(t)&flgon))
  {
  last=t;
  if(no(t)) goto down;
  else return (Widget *)t;
  }
 else goto down;

 return (Widget *)t;
 }

/* Record current tab position */

static void focuspos(t)
Widget *t;
 {
 while(gtmom(t) && (Widget *)gtmom(t)!=root)
  {
  ((Widget *)t->in)->focusn=find(t->in,t);
  t=(Widget *)t->in;
  }
 }

static int ufocusnext(stat,w)
Widget *w;
 {
 Widget *n;
 if(!stat)
  {
  if(w->trap) n=focusnext(w->trap);
  else n=focusnext(w);
  if(n) stfocus(n);
  }
 return stat;
 }

static int ufocusprev(stat,w)
Widget *w;
 {
 Widget *n;
 if(!stat)
  {
  if(w->trap) n=focusprev(w->trap);
  else n=focusprev(w);
  if(n) stfocus(n);
  }
 return stat;
 }

void stfocus(w)
Widget *w;
 {
 Widget *t=gttop(w);
 if(t->trap!=w)
  {
  clfocus(w);
  t->trap=w;
  t->traprtn=w;
  focuspos(w);
  if(t->traprtn->funcs->focusrtn)
   t->traprtn->funcs->focusrtn(t->traprtn);
  if(t->trap->funcs->focus) t->trap->funcs->focus(t->trap);
  }
 }

void stfocusrtn(w)
Widget *w;
 {
 Widget *t=gttop(w);
 if(t->traprtn!=w)
  {
  if(t->traprtn && t->traprtn->funcs->unfocusrtn)
   t->traprtn->funcs->unfocusrtn(t->traprtn);
  t->traprtn=w;
  if(t->traprtn->funcs->focusrtn)
   t->traprtn->funcs->focusrtn(t->traprtn);
  }
 }

void clfocus(w)
Widget *w;
 {
 w=gttop(w);
 if(w->trap && w->trap->funcs->unfocus) w->trap->funcs->unfocus(w->trap);
 if(w->traprtn && w->traprtn->funcs->unfocusrtn)
  w->traprtn->funcs->unfocusrtn(w->traprtn);
 w->trap=0;
 w->traprtn=0;
 }

Widget *gtfocus(w)
Widget *w;
 {
 w=gttop(w);
 return w->trap;
 }

struct widgetfuncs widgetfuncs;

Widget *mkWidget(w)
Widget *w;
 {
 mkLith(w);
 if(!widgetfuncs.on)
  {
  mcpy(&widgetfuncs,&lithfuncs,sizeof(struct lithfuncs));
  widgetfuncs.rm=widgetrm;
  widgetfuncs.gtmain=widgetgtmain;
  widgetfuncs.gtw=widgetgtw;
  widgetfuncs.stw=widgetstw;
  widgetfuncs.gth=widgetgth;
  widgetfuncs.sth=widgetsth;
  widgetfuncs.stiw=widgetstiw;
  widgetfuncs.stih=widgetstih;
  widgetfuncs.gtiw=widgetgtiw;
  widgetfuncs.gtih=widgetgtih;
  widgetfuncs.stwidth=widgetstwidth;
  widgetfuncs.stheight=widgetstheight;
  widgetfuncs.gtlofst=widgetgtlofst;
  widgetfuncs.gttofst=widgetgttofst;
  widgetfuncs.stlofst=widgetstlofst;
  widgetfuncs.sttofst=widgetsttofst;
  widgetfuncs.stscroll=widgetstscroll;
  widgetfuncs.localon=widgeton;
  widgetfuncs.localoff=widgetoff;
  widgetfuncs.stborder=widgetstborder;
  widgetfuncs.gtborder=widgetgtborder;
  widgetfuncs.gt2nd=widgetgt2nd;
  widgetfuncs.gtouter=widgetgtouter;
  widgetfuncs.sthelpmgr=widgetsthelpmgr;
  widgetfuncs.configurenotify=widgetresize;
  widgetfuncs.trigger=widgettrigger;
  widgetfuncs.reset=widgetreset;
  widgetfuncs.select=widgetselect;
  widgetfuncs.unselect=widgetunselect;
  widgetfuncs.focus=widgetfocus;
  widgetfuncs.unfocus=widgetunfocus;
  widgetfuncs.focusrtn=widgetfocusrtn;
  widgetfuncs.unfocusrtn=widgetunfocusrtn;
  widgetfuncs.activate=widgetactivate;
  widgetfuncs.deactivate=widgetdeactivate;
  widgetfuncs.stsizenotifyfn=widgetstsizenotifyfn;
  widgetfuncs.user=widgetuser;
  widgetfuncs.buttonpress=userevent;
  widgetfuncs.buttonrelease=userevent;
  widgetfuncs.keypress=userevent;
  widgetfuncs.motionnotify=userevent;
  widgetfuncs.stkmap=widgetstkmap;
  widgetfuncs.gtkmap=widgetgtkmap;
  widgetfuncs.kmap=mkkmap(NULL,NULL,NULL);
  kadd(widgetfuncs.kmap,NULL,"^I",ufocusnext,NULL);
  kadd(widgetfuncs.kmap,NULL,"Right",ufocusnext,NULL);
  kadd(widgetfuncs.kmap,NULL,"Down",ufocusnext,NULL);
  kadd(widgetfuncs.kmap,NULL,"Left",ufocusprev,NULL);
  kadd(widgetfuncs.kmap,NULL,"Up",ufocusprev,NULL);
  }
 w->funcs= &widgetfuncs;

 w->help=0;
 w->kbd=0;
 w->trap=0;
 w->traprtn=0;
 stkmap(w,w->funcs->kmap);

 w->outer=w->main=mk(Wind);
  sttarget(w->main,w);

 w->border=0;
 w->hints=0;
 w->focusn=0;
 w->mask=0;
 w->sizenotifyfn=0;

/* stborder(w,mk(Shadow)); */

 stpmgr(w,mk(Pmgr));

 return w;
 }
