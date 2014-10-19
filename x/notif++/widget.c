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

Widget::~Widget()
  {
  Lith *p, *q;
  off();
  // Delete kids before we delete components.  If components are deleted first, then we crash since
  // kids will try to delete their already deleted components.
  for (p = kids.next; p; p = q)
    {
    q = p->link.next;
    delete p;
    }
  kids.next = 0;
  if (help) delete help;
  if (main) delete main;
  if (mask) delete mask;
  if (border) delete border;
  if (hints) delete hints;
  if (sizenotifyfn) sizenotifyfn->cancel();
  }

Wind *Widget::gtmain()
  {
  return main;
  }

void Widget::stw(int n)
  {
  int amnt=0;
  if(border) amnt=border->gtlmargin()+border->gtrmargin();
  stiw(n-amnt);
  }

int Widget::gtw()
  {
  int amnt=0;
  if(border) amnt=border->gtlmargin()+border->gtrmargin();
  return gtiw()+amnt;
  }

void Widget::sth(int n)
  {
  int amnt=0;
  if (border) amnt=border->gttmargin()+border->gtbmargin();
  stih(n-amnt);
  }

int Widget::gth()
  {
  int amnt=0;
  if(border) amnt=border->gttmargin()+border->gtbmargin();
  return gtih()+amnt;
  }

void Widget::stiw(int n)
  {
  if(gt2nd()==gtmain()) stwidth(n);
  else
    {
    if(border)
      {
      int amnt=border->gtlmargin()+border->gtrmargin();
      outer->stwidth(n+amnt);
      }
    gt2nd()->stwidth(n);
    }
  }

int Widget::gtiw()
  {
  if(gt2nd()==gtmain()) return gtwidth();
  else return gt2nd()->gtwidth();
  }

void Widget::stwidth(int n)
  {
  if(gt2nd()==gtmain())
    {
    if(border)
      {
      int amnt=border->gtlmargin()+border->gtrmargin();
      outer->stwidth(n+amnt);
      }
    }
  Lith::stwidth(n);
  if (sizenotifyfn) sizenotifyfn->cont(sizenotifyfn);
  }

void Widget::stih(int n)
  {
  if(gt2nd()==gtmain())
    stheight(n);
  else
    {
    if(border)
      {
      int amnt=border->gttmargin()+border->gtbmargin();
      outer->stheight(n+amnt);
      }
    gt2nd()->stheight(n);
    }
  }

int Widget::gtih()
  {
  if(gt2nd()==gtmain())
    {
    return gtheight();
    }
  else return gt2nd()->gtheight();
  }

void Widget::stheight(int n)
  {
  if(gt2nd()==gtmain())
    {
    if(border)
      {
      int amnt=border->gttmargin()+border->gtbmargin();
      outer->stheight(n+amnt);
      }
    }
  Lith::stheight(n);
  if (sizenotifyfn) sizenotifyfn->cont(sizenotifyfn);
  }

int Widget::gtlofst()
  {
  return -gtmain()->gtx();
  }

int Widget::gttofst()
  {
  return -gtmain()->gty();
  }

void Widget::stlofst(int n)
  {
  gtmain()->stx(-n);
  }

void Widget::sttofst(int n)
  {
  gtmain()->sty(-n);
  }

void Widget::stscroll()
  {
  mask=new Wind();
  main->stx(0);
  main->sty(0);
  if (!border) outer=mask;
  }

void Widget::localon()
  {
  if(gtborder())
    {
    gt2nd()->stx(gtborder()->gtlmargin());
    gt2nd()->sty(gtborder()->gttmargin());
    }
  if(gtborder())
    {
    gtborder()->stmom(gtmom()->gtmain());
    gt2nd()->stmom(gtborder());
    }
  else
    {
    gt2nd()->stmom(gtmom()->gtmain());
    }
  if(gt2nd()!=gtmain())
    {
    gtmain()->stmom(gt2nd());
    }
  win=gtmain()->gtwin();
  }

void Widget::localoff()
  {
  if(gtfocus(this)==this) clfocus(this);
  gtmain()->clmom();
  gt2nd()->clmom();
  if(gtborder()) gtborder()->clmom();
  }

Border *Widget::gtborder()
  {
  return border;
  }

void Widget::stborder(Border *x)
  {
  if(border)
    {
    gt2nd()->clmom();
    delete border;
    }
  border=x;
  if(x)
    {
    outer=x;
    x->sttarget(this);
    }
  else outer=gt2nd();
  }

Wind *Widget::gt2nd()
  {
  if(mask) return mask;
  else return main;
  }

Wind *Widget::gtouter()
  {
  return outer;
  }

void Widget::sthelpmgr(Help *x)
 {
 if (help)
   delete help;
 help=x;
 if(x)
   x->sttarget(this);
 }

int Widget::user(unsigned c,unsigned state,int x,int y,Time time,Widget *org)
  {
  // Key bind;
  Mfn0_6<int,Widget,int,int,int,int,Time,Widget *> *bind;

  int stat;
  int normalkey=0;
  if(c!=XK_Press1 && c!=XK_Press2 && c!=XK_Press3 &&
     c!=XK_Release1 && c!=XK_Release2 && c!=XK_Release3 &&
     c!=XK_Motion)
   normalkey=1;
  if(normalkey && traprtn && c==13)
    {
    int rtn=traprtn->user(c,state,x,y,time,org);
    if(!rtn) return 0;
    }
  else if(normalkey && trap)
    {
    int rtn=trap->user(c,state,x,y,time,org);
    if(!rtn) return 0;
    }
  stat=kbd.dokey((Key *)&bind,c);	/* Do we have a command? */
  if(stat==2)
    {
    bind->argT = this;
    return bind->exec(c,state,x,y,time,org);
    }
  else if(stat==1)		/* Prefix key */
    return 0;
  else
    if(normalkey) return 0;	/* Eat normal keys */
    else return -1;		/* Key not accepted */
  }

int Widget::configurenotify(XEvent *ev)
  {
  int newx, newy;
  int neww=ev->xconfigure.width;
  int newh=ev->xconfigure.height;
  int xflg=0;
  int yflg=0;

  calcpos(ev->xany.window,&newx,&newy);

  if (gtmom() != root) return 0;

  /* Record window's position */
  gtouter()->x=newx;
  gtouter()->y=newy;
  x=newx;
  y=newy;

  if(neww!=gtouter()->w) xflg=1;
  if(newh!=gtouter()->h) yflg=1;
  if(xflg || yflg)
    {
    printf("Resize! (%d %d)->(%d %d)\n",gtouter()->w,gtouter()->h,neww,newh);
    /* We can't call stw/sth here: it would cause an infinite loop */
    gtouter()->w=neww;
    gtouter()->h=newh;
    if(gtborder())
      {
      w=neww-gtborder()->gtlmargin()-gtborder()->gtrmargin();
      h=newh-gtborder()->gttmargin()-gtborder()->gtbmargin();
      gt2nd()->stw(w);
      gt2nd()->sth(h);
      }
    else
      {
      w=neww;
      h=newh;
      }
    /* Reposition children */
    if(pmgr)
      {
      if(xflg) calcx();
      if(yflg) calcy();
      }
    /* Notify? */
    if (sizenotifyfn) sizenotifyfn->cont(sizenotifyfn);
    }
  return 0;
  }

void *dotrigger(Lith *t)
  {
  ((Widget *)t)->trigger();
  return NULL;
  }

int Widget::trigger()
  {
  Wind *z;
  Fn0_1<void *,Lith *> fn(dotrigger);
  /* Tell the border */
  if(gtborder()) gtborder()->trigger();
  /* Tell the kids */
  apply(&fn);
  return 0;
  }

static void *doreset(Lith *w)
  {
  ((Widget *)w)->reset();
  return NULL;
  }

int Widget::reset()
  {
  Wind *z;
  Fn0_1<void *,Lith *> fn(doreset);
  /* Tell the border */
  if(gtborder()) gtborder()->reset();
  /* Tell the kids */
  // FIXME: isn't this a combinatorial explosion: apply only to kids, not kid's kids */
  apply(&fn);
  return 0;
  }

static void *dofocus(Lith *w)
  {
  ((Widget *)w)->focus();
  return NULL;
  }

int Widget::focus()
  {
  Wind *z;
  Fn0_1<void *,Lith *> fn(dofocus);
  /* Tell the border */
  if(z=gtborder()) z->focus();
  /* Tell the kids */
  apply(&fn);
  return 0;
  }

static void *dounfocus(Lith *w)
  {
  ((Widget *)w)->unfocus();
  return NULL;
  }

int Widget::unfocus()
  {
  /* Tell the border */
  Wind *z;
  if(z=gtborder()) z->unfocus();
  /* Tell the kids */
  Fn0_1<void *,Lith *> fn(dounfocus);
  apply(&fn);
  return 0;
  }

static void *doselect(Lith *w)
  {
  ((Widget *)w)->select();
  return NULL;
  }

int Widget::select()
  {
  /* Tell the border */
  Wind *z;
  if(z=gtborder()) z->select();
  /* Tell the kids */
  Fn0_1<void *,Lith *> fn(doselect);
  apply(&fn);
  return 0;
  }

static void *dounselect(Lith *w)
  {
  ((Widget *)w)->unselect();
  return NULL;
  }

int Widget::unselect()
  {
  /* Tell the border */
  Wind *z;
  if(z=gtborder()) z->unselect();
  /* Tell the kids */
  Fn0_1<void *,Lith *> fn(dounselect);
  apply(&fn);
  return 0;
  }

static void *doactivate(Lith *w)
  {
  ((Widget *)w)->activate();
  return NULL;
  }

int Widget::activate()
  {
  if(!activated_flag)
    {
    activated_flag = 1;
    /* Tell the border */
    Wind *z;
    if(z=gtborder()) z->activate();
    /* Tell the kids */
    Fn0_1<void *,Lith *> fn(doactivate);
    apply(&fn);
    }
  return 0;
  }

static void *dodeactivate(Lith *w)
  {
  ((Widget *)w)->deactivate();
  return NULL;
  }

int Widget::deactivate()
  {
  if(activated_flag)
    {
    activated_flag = 0;
    /* Tell the border */
    Wind *z;
    if(z=gtborder()) z->deactivate();
    /* Tell the kids */
    Fn0_1<void *,Lith *> fn(dodeactivate);
    apply(&fn);
    }
  return 0;
  }

static void *dofocusrtn(Lith *w)
  {
  ((Widget *)w)->focusrtn();
  return NULL;
  }

int Widget::focusrtn()
  {
  /* Tell the border */
  Wind *z;
  if(z=gtborder()) z->focusrtn();
  /* Tell the kids */
  Fn0_1<void *,Lith *> fn(dofocusrtn);
  apply(&fn);
  return 0;
  }

static void *dounfocusrtn(Lith *w)
  {
  ((Widget *)w)->unfocusrtn();
  return NULL;
  }

int Widget::unfocusrtn()
  {
  /* Tell the border */
  Wind *z;
  if(z=gtborder()) z->unfocusrtn();
  /* Tell the kids */
  Fn0_1<void *,Lith *> fn(dounfocusrtn);
  apply(&fn);
  return 0;
  }

void Widget::stsizenotifyfn(Fn_0<void> *n)
  {
  sizenotifyfn=n;
  }

void Widget::stkmap(Kmap<Key> *n)
  {
  kmap=n;
  kbd.stkmap(n);
  }

Kmap<Key> *Widget::gtkmap()
  {
  return kmap;
  }

Widget *focusnext(Widget *t)
  {
  int wrap=0;
  Widget *last=0;

  if(t->gtmom()==root)
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
  if(t->focusn==t->no()-1)
    { /* Done with this level: go up one unless we're at top */
    if(t->gtmom()==root)
      {
      t->focusn= -1;
      if(++wrap==2) return 0;
      }
    else goto up;
    }

  /* Go to next kid */
  ++t->focusn;
  t=(Widget *)t->nth(t->focusn);

  t->focusn= -1;

  if(t->focusable_flag && t->on_flag)
    {
    last=t;
    if(t->no()) goto down;
    else return t;
    }
  else goto down;

  return t;
  }

static Widget *focusprev(Widget *t)
  {
  int wrap=0;
  Widget *last=0;

  if(t->gtmom()==root)
    {
    t->focusn=t->no();
    goto down;
    }

  up:
  /* Leave current node */
  t=(Widget *)t->in;
  if(t==last) return t;

  /* Find next child */
  down:
  if(t->focusn<=0)
    { /* Done with this level: go up one unless we're at top */
    if(t->gtmom()==root)
      {
      t->focusn=t->no();
      if(++wrap==2) return 0;
      }
    else goto up;
    }

  /* Go to next kid */
  --t->focusn;
  t=(Widget *)t->nth(t->focusn);

  t->focusn=t->no();

  if(t->focusable_flag && t->on_flag)
    {
    last=t;
    if(t->no()) goto down;
    else return t;
    }
  else goto down;

  return t;
  }

/* Record current tab position */

static void focuspos(Widget *t)
  {
  while(t->gtmom() && t->gtmom()!=root)
    {
    ((Widget *)t->in)->focusn=t->in->find(t);
    t=(Widget *)t->in;
    }
  }

int Widget::ufocusnext(int c,int state,int x,int y,Time time,Widget *org)
  {
  Widget *n;
  if(trap) n=focusnext(trap);
  else n=focusnext(this);
  if(n) stfocus(n);
  return 0;
  }

int Widget::ufocusprev(int c,int state,int x,int y,Time time,Widget *org)
  {
  Widget *n;
  if(trap) n=focusprev(trap);
  else n=focusprev(this);
  if(n) stfocus(n);
  return 0;
  }

void stfocus(Widget *w)
  {
  Widget *t=gttop(w);
  if(t->trap!=w)
    {
    clfocus(w);
    t->trap=w;
    t->traprtn=w;
    focuspos(w);
    if(t->traprtn)
      t->traprtn->focusrtn();
    if(t->trap)
      t->trap->focus();
    }
  }

void stfocusrtn(Widget *w)
  {
  Widget *t=gttop(w);
  if(t->traprtn!=w)
    {
    if(t->traprtn)
      t->traprtn->unfocusrtn();
    t->traprtn=w;
    if(t->traprtn)
      t->traprtn->focusrtn();
    }
  }

void clfocus(Widget *w)
  {
  w=gttop(w);
  if (w->trap) w->trap->unfocus();
  if (w->traprtn) w->traprtn->unfocusrtn();
  w->trap=0;
  w->traprtn=0;
  }

Widget *gtfocus(Widget *w)
  {
  w=gttop(w);
  return w->trap;
  }

void Widget::ungrab()
  {
  }

int Widget::keypress(XEvent *ev) { return userevent(this,ev); }
int Widget::keyrelease(XEvent *ev) { return userevent(this,ev); }
int Widget::buttonpress(XEvent *ev) { return userevent(this,ev); }
int Widget::buttonrelease(XEvent *ev) { return userevent(this,ev); }
int Widget::motionnotify(XEvent *ev) { return userevent(this,ev); }

Widget::Widget()
  {
  help=0;
  trap=0;
  traprtn=0;
  stkmap(&stdkmap);
  border=0;
  hints=0;
  focusn=0;
  mask=0;
  sizenotifyfn=0;

  outer=main=new Wind();
  main->sttarget(this);

  /* stborder(new Shadow()); */
  }
