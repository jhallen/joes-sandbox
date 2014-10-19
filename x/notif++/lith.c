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

void Lith::localon()
  {
  }

void Lith::localoff()
  {
  }

int Lith::gtw()
  {
  return gtwidth();
  }

int Lith::gth()
  {
  return gtheight();
  }

int Lith::gtiw()
  {
  return gtwidth();
  }

int Lith::gtih()
  {
  return gtheight();
  }

void Lith::stw(int n)
  {
  stwidth(n);
  }

void Lith::stiw(int n)
  {
  stwidth(n);
  }

void Lith::sth(int n)
  {
  stheight(n);
  }

void Lith::stih(int n)
  {
  stheight(n);
  }

Lith::~Lith()
  {
  Lith *p, *q;
  // Delete kids
  for (p = kids.next; p; p = q)
    {
    q = p->kids.next;
    delete p;
    }
  // Remove from mom's list
  if (gtmom()) gtmom()->rmv(this);
  // Delete placement manager
  if (pmgr) delete pmgr;
  }

void Lith::stwidth(int n)
  {
  // If width changed...
  if(gtmain()->w != n)
    {
    // If we're not the main window...
    if(gtmain() != this)
      gtmain()->stwidth(n);
    // Record new width
    w = n;
    // Placement manager recalculate
    calcx();
    }
  }

int Lith::gtwidth()
  {
  if(!ckw()) calcw();
  return w;
  }

void Lith::stheight(int n)
  {
  if (gtmain()->h != n)
    {
    if(gtmain() != this) gtmain()->stheight(n);
    h = n;
    calcy();
    }
  }

int Lith::gtheight()
  {
  if(!ckh()) calch();
  return h;
  }

void Lith::stx(int n)
  {
  x = n;
  if(gtouter() != this) gtouter()->stx(n);
  }

int Lith::gtx()
  {
  if(x != MAXINT) return x;
  else return 0;
  }

void Lith::sty(int n)
  {
  y = n;
  if (gtouter() != this) gtouter()->sty(n);
  }

int Lith::gty()
  {
  if(y != MAXINT) return y;
  else return 0;
  }

int Lith::ckw()
  {
  return w != -1;
  }

int Lith::ckh()
  {
  return h != -1;
  }

int Lith::ckx()
  {
  return x != MAXINT;
  }

int Lith::cky()
  {
  return y != MAXINT;
  }

void Lith::on()
  {
  if(enabled_flag && !on_flag)
    {
    Lith *l;
    gtwidth(); gtheight();
    if(gtouter() != this)
      {
      if(ckx()) gtouter()->stx(gtx());
      if(cky()) gtouter()->sty(gty());
      if(override_flag)
        {
        gtouter()->stoverride();
        }
      if(transient_flag) gtouter()->sttransient();
      }
    localon();
    on_flag = 1;
    for(l=kids.next; l; l=l->link.next)
      {
      l->on();
      }
    }
  }

void Lith::off()
  {
  if(on_flag)
    {
    Lith *l, *n;
    for(l=kids.next; l; l=l->link.next) l->off();
    localoff();
    on_flag = 0;
    }
  }

void Lith::enable()
  {
  enabled_flag = 1;
  if(gtmom() && gtmom()->on_flag) on();
  }

void Lith::disable()
  {
  enabled_flag = 0;
  off();
  }

Window Lith::gtwin()
  {
  return win;
  }

void Lith::stmom(Lith *n)
  {
  in = n;
  if (n->pmgr) n->place(this);
  if (n->on_flag) on();
  }

Lith *Lith::gtmom()
  {
  return in;
  }

Lith *Lith::gtmain()
  {
  return this;
  }

Lith *Lith::gtouter()
  {
  return gtmain();
  }

Lith *Lith::gt2nd()
  {
  return gtmain();
  }

void Lith::clmom()
  {
  off();
  // if(in) in->rmv(this);
  // Needed in C version to delete associated structure
  in=0;
  }

void Lith::stbknd(GC n)
  {
  Lith *d=gtmain();
  if(d != this) d->stbknd(n);
  }

GC Lith::gtbknd()
  {
  Lith *d=gtmain();
  if(d != this) return d->gtbknd();
  else return 0;
  }

void Lith::stpixmap(Pixmap pix)
  {
  Lith *d=gtmain();
  if(d!=this) d->stpixmap(pix);
  }

void Lith::stcursor(Cursor cursor)
  {
  Lith *d=gtmain();
  if(d!=this) d->stcursor(cursor);
  }

void Lith::sttitle(char *n)
  {
  Lith *d=gtouter();
  if(d!=this) d->sttitle(n);
  }

void Lith::stfgnd(GC n)
  {
  Lith *d;
  fgnd=n;
  d=gtmain();
  if(d!=this) d->stfgnd(n);
  }

GC Lith::gtfgnd()
  {
  return fgnd;
  }

void Lith::add(Lith *n)
  {
  kids.push_back(n);
  n->stmom(this);
  }

void Lith::ins(int n,Lith *l)
  {
  kids.insert_before(kids.nth(n),l);
  l->stmom(this);
  }

Lith *Lith::nth(int n)
  {
  return kids.nth(n);
  }

int Lith::no()
  {
  return kids.no();
  }

int Lith::find(Lith *l)
  {
  return kids.findn(l);
  }

void Lith::rmv(Lith *l)
  {
  l->clmom();
  kids.deque(l);
  }

void show_tree(int idx,Widget *t)
  {
  int x;
  Lith *l;
  for (x = 0; x != idx; ++x)
    printf(" ");
  printf("%x main=%x mask=%x border=%x\n",t,t->main,t->mask,t->border);
  for (l=t->kids.next;l;l=l->link.next)
    {
    show_tree(idx+2,(Widget *)l);
    }
  }

void *Lith::apply(Fn_1<void *,Lith *> *func)
  {
  Lith *l;
  for(l=kids.next; l; l=l->link.next)
    {
    void *rtn;
    if (rtn = func->exec(l)) return rtn;
    if (rtn = l->apply(func)) return rtn;
    }
  return 0;
  }

void *Lith::xyapply(int x,int y,Fn_1<void *,Lith *> *func)
  {
  Lith *l;
  for(l=kids.next;l;l=l->link.next)
    {
    void *rtn;
    if (x>=l->gtx() && x<l->gtx()+l->gtw() && y>=l->gty() && y<l->gty()+l->gth())
      {
      if (rtn = func->exec(l)) return rtn;
      if (rtn = l->xyapply(x-l->gtx(),y-l->gty(),func)) return rtn;
      }
    }
  return 0;
  }

Lith::Lith()
  {
  in=0;
  x=y= MAXINT;
  w=h= -1;

  is_menu = 0;
  is_cascade = 0;
  on_flag = 0;
  enabled_flag = 1;
  activated_flag = 1;
  focusable_flag = 0;
  override_flag = 0;
  transient_flag = 0;

  fgnd=stdfgnd;
  pmgr=0;
  lofst=rofst=tofst=bofst= -1;
  }

/* Return 'did not take event' code (-1) for all event handlers */

/* Notif events */
int Lith::user(unsigned button,unsigned state,int x,int y,Time time,Widget *org) { return -1; }
                                                /* User action event */
int Lith::focus() { return -1; }		/* Attained focus event */
int Lith::unfocus() { return -1; }		/* Lost focus event */
int Lith::trigger() { return -1; }		/* Button trigger */
int Lith::reset() { return -1; }		/* Button reset */
int Lith::select() { return -1; }		/* Item selected */
int Lith::unselect() { return -1; }		/* Item unselected */
int Lith::activate() { return -1; }		/* Item activated */
int Lith::deactivate() { return -1; }	 	/* Item deactivated */
int Lith::focusrtn() { return -1; }		/* Attained return key focus */
int Lith::unfocusrtn() { return -1; }		/* Lost return-key focus */
int Lith::drop() { return -1; }			/* Something got dropped on us */

/* X events */
int Lith::keypress(XEvent *ev) { return -1; }
int Lith::keyrelease(XEvent *ev) { return -1; }
int Lith::buttonpress(XEvent *ev) { return -1; }
int Lith::buttonrelease(XEvent *ev) { return -1; }
int Lith::motionnotify(XEvent *ev) { return -1; }
int Lith::enternotify(XEvent *ev) { return -1; }
int Lith::leavenotify(XEvent *ev) { return -1; }
int Lith::focusin(XEvent *ev) { return -1; }
int Lith::focusout(XEvent *ev) { return -1; }
int Lith::keymapnotify(XEvent *ev) { return -1; }
int Lith::expose(XEvent *ev) { return -1; }
int Lith::graphicsexpose(XEvent *ev) { return -1; }
int Lith::noexpose(XEvent *ev) { return -1; }
int Lith::visibilitynotify(XEvent *ev) { return -1; }
int Lith::createnotify(XEvent *ev) { return -1; }
int Lith::destroynotify(XEvent *ev) { return -1; }
int Lith::unmapnotify(XEvent *ev) { return -1; }
int Lith::mapnotify(XEvent *ev) { return -1; }
int Lith::maprequest(XEvent *ev) { return -1; }
int Lith::reparentnotify(XEvent *ev) { return -1; }
int Lith::configurenotify(XEvent *ev) { return -1; }
int Lith::configurerequest(XEvent *ev) { return -1; }
int Lith::gravitynotify(XEvent *ev) { return -1; }
int Lith::resizerequest(XEvent *ev) { return -1; }
int Lith::circulatenotify(XEvent *ev) { return -1; }
int Lith::circulaterequest(XEvent *ev) { return -1; }
int Lith::propertynotify(XEvent *ev) { return -1; }
int Lith::selectionclear(XEvent *ev) { return -1; }
int Lith::selectionrequest(XEvent *ev) { return -1; }
int Lith::selectionnotify(XEvent *ev) { return -1; }
int Lith::colormapnotify(XEvent *ev) { return -1; }
int Lith::clientmessage(XEvent *ev) { return -1; }
int Lith::mappingnotify(XEvent *ev) { return -1; }

void Lith::stoverride()
  {
  override_flag = 1;
  }

void Lith::cloverride()
  {
  override_flag = 0;
  }

void Lith::sttransient()
  {
  transient_flag = 1;
  }

void Lith::cltransient()
  {
  transient_flag = 0;
  }

void Lith::stfocusable()
  {
  focusable_flag = 1;
  }

void Lith::clfocusable()
  {
  focusable_flag = 0;
  }

// Placement manager 

/* hmode values: 0=ltor, 1=ledge, 2=lsame, 3=rtol, 4=redge, 5=rsame, 6=center */
/* vmode values: 0=ttob, 1=tedge, 2=tsame, 3=btot, 4=bedge, 5=bsame, 6=center */

void Lith::calcw()
  {
  int max= -1;
  if (!pmgr)
    return;
  for(Lith *w=kids.next;w;w=w->link.next)
    {
    int x=w->gtw();
    if(w->lofst!=-1) x+=w->lofst;
    if(w->rofst!=-1) x+=w->rofst;
    if(x>max) max=x;
    }
  if(max==-1) max=16;
  stwidth(max);
  }

void Lith::calch()
  {
  int max= -1;
  if (!pmgr)
    return;
  for(Lith *w=kids.next;w;w=w->link.next)
    {
    int y=w->gth();
    if(w->tofst!=-1) y+=w->tofst;
    if(w->bofst!=-1) y+=w->bofst;
    if(y>max) max=y;
    }
  if(max==-1) max=16;
  stheight(max);
  }

void Lith::dox(Lith *w)
  {
  if(w->lofst!=-1 && w->rofst!=-1)
    w->stw(w->in->gtwidth()-w->lofst-w->rofst);
  if(w->lofst!=-1) w->stx(w->lofst);
  else if(w->rofst!=-1) w->stx(w->in->gtwidth()-w->rofst-w->gtw());
  else w->stx(w->in->gtwidth()/2-w->gtw()/2);
  }

void Lith::calcx()
  {
  if (pmgr)
    for(Lith *w=kids.next;w;w=w->link.next)
      dox(w);
  }

void Lith::doy(Lith *w)
  {
  if(w->tofst!=-1 && w->bofst!=-1)
    w->sth(w->in->gtheight()-w->tofst-w->bofst);
  if(w->tofst!=-1) w->sty(w->tofst);
  else if(w->bofst!=-1) w->sty(w->in->gtheight()-w->bofst-w->gth());
  else w->sty(w->in->gtheight()/2-w->gth()/2);
  }

void Lith::calcy()
  {
  if (pmgr)
    for(Lith *w=kids.next;w;w=w->link.next)
      doy(w);
  }

void Lith::place(Lith *w)
  {
  if (!pmgr) pmgr = new Pmgr();

  w->lofst=w->rofst=w->tofst=w->bofst= -1;

  if(!w->ckx())
    {
    if(pmgr->lrel!=-1)
      switch(pmgr->hmode)
        {
        case 0:
          w->lofst=pmgr->lrel+pmgr->wid+pmgr->hofst; w->rofst= -1; break;
        case 1:
          w->lofst=pmgr->hofst; w->rofst= -1; break;
        case 2:
          w->lofst=pmgr->lrel+pmgr->hofst; w->rofst= -1; pmgr->hofst=0; break;
        case 3:
          w->lofst=pmgr->lrel-w->gtw()-pmgr->hofst; w->rofst= -1; break;
        case 4:
          w->rofst=pmgr->hofst; w->lofst= -1; break;
        case 5:
          w->lofst=pmgr->lrel+pmgr->wid-w->gtw()-pmgr->hofst; w->rofst= -1; pmgr->hofst=0; break;
        case 6:
          w->lofst=w->rofst= -1; break;
        }
    else
      switch(pmgr->hmode)
        {
        case 0:
          if(pmgr->rrel!=-1)
            { w->rofst=pmgr->rrel-w->gtw()-pmgr->hofst; w->lofst= -1; break; }
          else
            { w->lofst=0; w->rofst= -1; break; }
        case 1:
          w->lofst=pmgr->hofst; w->rofst= -1; break;
        case 2:
          w->rofst=pmgr->rrel+pmgr->wid-w->gtw()-pmgr->hofst; w->lofst= -1; pmgr->hofst=0; break;
        case 3:
          if(pmgr->rrel!=-1)
            { w->rofst=pmgr->rrel+pmgr->wid+pmgr->hofst; w->lofst= -1; break; }
          else
            { w->rofst=0; w->lofst= -1; break; }
        case 4:
          w->rofst=pmgr->hofst; w->lofst= -1; break;
        case 5:
          w->rofst=pmgr->rrel+pmgr->hofst; w->lofst= -1; pmgr->hofst=0; break;
        case 6:
          w->lofst=w->rofst= -1; break;
        }
    if(pmgr->alrel!=-1)
      switch(pmgr->ahmode)
        {
        case 0:
          w->lofst=pmgr->alrel+pmgr->awid+pmgr->ahofst; break;
        case 1:
          w->lofst=pmgr->ahofst; break;
        case 2:
          w->lofst=pmgr->alrel+pmgr->ahofst; break;
        case 3:
          w->lofst=pmgr->alrel-w->gtw()-pmgr->ahofst; break;
        case 4:
          w->rofst=pmgr->ahofst; break;
        case 5:
          w->lofst=pmgr->alrel+pmgr->awid-w->gtw()-pmgr->ahofst; break;
        }
    else
      switch(pmgr->ahmode)
        {
        case 0:
          if(pmgr->arrel!=-1)
            { w->rofst=pmgr->arrel-w->gtw()-pmgr->ahofst; break; }
          else
            { w->lofst=0; break; }
        case 1:
          w->lofst=pmgr->ahofst; break;
        case 2:
          w->rofst=pmgr->arrel+pmgr->awid-w->gtw()-pmgr->ahofst; break;
        case 3:
          if(pmgr->arrel!=-1)
            { w->rofst=pmgr->arrel+pmgr->awid+pmgr->ahofst; break; }
          else
            { w->rofst=0; break; }
        case 4:
          w->rofst=pmgr->ahofst; break;
        case 5:
          w->rofst=pmgr->arrel+pmgr->ahofst; break;
        }
    }
  else
    {
    w->lofst=w->gtx();
    w->rofst= -1;
    }
  pmgr->lrel=w->lofst;
  pmgr->rrel=w->rofst;
  pmgr->wid=w->gtw();
  if(ckw()) dox(w);

  if(!w->cky())
    {
    if(pmgr->trel!=-1)
      switch(pmgr->vmode)
        {
        case 0:
          w->tofst=pmgr->trel+pmgr->hgt+pmgr->vofst; w->bofst= -1; break;
        case 1:
          w->tofst=pmgr->vofst; w->bofst= -1; break;
        case 2:
          w->tofst=pmgr->trel+pmgr->vofst; w->bofst= -1; pmgr->vofst=0; break;
        case 3:
          w->tofst=pmgr->trel-w->gth()-pmgr->vofst; w->bofst= -1; break;
        case 4:
          w->bofst=pmgr->vofst; w->tofst= -1; break;
        case 5:
          w->tofst=pmgr->trel+pmgr->hgt-w->gth()-pmgr->vofst; w->bofst= -1; pmgr->vofst=0; break;
        case 6:
          w->tofst=w->bofst= -1; break;
        }
    else
      switch(pmgr->vmode)
        {
        case 0:
          if(pmgr->brel!=-1)
            { w->bofst=pmgr->brel-w->gth()-pmgr->vofst; w->tofst= -1; break; }
          else
            { w->tofst=0; w->bofst= -1; break; }
        case 1:
          w->tofst=pmgr->vofst; w->bofst= -1; break;
        case 2:
          w->bofst=pmgr->brel+pmgr->hgt-w->gth()-pmgr->vofst; w->tofst= -1; pmgr->vofst=0; break;
        case 3:
          if(pmgr->brel!=-1)
            { w->bofst=pmgr->brel+pmgr->hgt+pmgr->vofst; w->tofst= -1; break; }
          else
            { w->bofst=0; w->tofst= -1; break; }
        case 4:
          w->bofst=pmgr->vofst; w->tofst= -1; break;
        case 5:
          w->bofst=pmgr->brel+pmgr->vofst; w->tofst= -1; pmgr->vofst=0; break;
        case 6:
          w->tofst=w->bofst= -1; break;
        }
    if(pmgr->atrel!=-1)
      switch(pmgr->avmode)
        {
        case 0:
          w->tofst=pmgr->atrel+pmgr->ahgt+pmgr->avofst; break;
        case 1:
          w->tofst=pmgr->avofst; break;
        case 2:
          w->tofst=pmgr->atrel+pmgr->avofst; break;
        case 3:
          w->tofst=pmgr->atrel-w->gth()-pmgr->avofst; break;
        case 4:
          w->bofst=pmgr->avofst; break;
        case 5:
          w->tofst=pmgr->atrel+pmgr->ahgt-w->gth()-pmgr->avofst; break;
        }
    else
      switch(pmgr->avmode)
        {
        case 0:
          if(pmgr->abrel!=-1)
            { w->bofst=pmgr->abrel-w->gth()-pmgr->avofst; break; }
          else
            { w->tofst=0; break; }
        case 1:
          w->tofst=pmgr->avofst; break;
        case 2:
          w->bofst=pmgr->abrel+pmgr->ahgt-w->gth()-pmgr->avofst; break;
        case 3:
          if(pmgr->abrel!=-1)
            { w->bofst=pmgr->abrel+pmgr->ahgt+pmgr->avofst; break; }
          else
            { w->bofst=0; break; }
        case 4:
          w->bofst=pmgr->avofst; break;
        case 5:
          w->bofst=pmgr->abrel+pmgr->avofst; break;
        }
    }
  else
    {
    w->tofst=w->gty();
    w->bofst= -1;
    }
  pmgr->trel=w->tofst;
  pmgr->brel=w->bofst;
  pmgr->hgt=w->gth();
  if(ckh()) doy(w);
  }

Pmgr::Pmgr()
  {
  wid=0; hgt=0;
  hmode=0; vmode=1;
  hofst=0; vofst=0;
  lrel=rrel=trel=brel=-1;

  awid=0; ahgt=0;
  ahmode=0; avmode=1;
  ahofst=0; avofst=0;
  alrel=arrel=atrel=abrel=-1;
  }

void Lith::ltor(Lith *rel,int ofst)
  {
  if (!pmgr) pmgr = new Pmgr();
  pmgr->hmode=0; pmgr->ahmode=6;
  pmgr->hofst=ofst;
  if(rel) pmgr->lrel=rel->lofst, pmgr->rrel=rel->rofst, pmgr->wid=rel->gtw();
  else pmgr->lrel=pmgr->rrel= -1;
  }

void Lith::ledge(int ofst)
  {
  if (!pmgr) pmgr = new Pmgr();
  pmgr->hmode=1; pmgr->ahmode=6;
  pmgr->hofst=ofst;
  pmgr->lrel=pmgr->rrel= -1;
  }

void Lith::lsame(Lith *rel,int ofst)
  {
  if (!pmgr) pmgr = new Pmgr();
  pmgr->hmode=2; pmgr->ahmode=6;
  pmgr->hofst=ofst;
  if(rel) pmgr->lrel=rel->lofst, pmgr->rrel=rel->rofst, pmgr->wid=rel->gtw();
  else pmgr->lrel=pmgr->rrel= -1;
  }

void Lith::rtol(Lith *rel,int ofst)
  {
  if (!pmgr) pmgr = new Pmgr();
  pmgr->hmode=3; pmgr->ahmode=6;
  pmgr->hofst=ofst;
  if(rel) pmgr->lrel=rel->lofst, pmgr->rrel=rel->rofst, pmgr->wid=rel->gtw();
  else pmgr->lrel=pmgr->rrel= -1;
  }

void Lith::redge(int ofst)
  {
  if (!pmgr) pmgr = new Pmgr();
  pmgr->hmode=4; pmgr->ahmode=6;
  pmgr->hofst=ofst;
  pmgr->lrel=pmgr->rrel= -1;
  }

void Lith::rsame(Lith *rel,int ofst)
  {
  if (!pmgr) pmgr = new Pmgr();
  pmgr->hmode=5; pmgr->ahmode=6;
  pmgr->hofst=ofst;
  if(rel) pmgr->lrel=rel->lofst, pmgr->rrel=rel->rofst, pmgr->wid=rel->gtw();
  else pmgr->lrel=pmgr->rrel= -1;
  }

void Lith::hcenter()
  {
  if (!pmgr) pmgr = new Pmgr();
  pmgr->hmode=6; pmgr->ahmode=6;
  pmgr->hofst=0;
  pmgr->lrel=pmgr->rrel= -1;
  }

void Lith::ttob(Lith *rel,int ofst)
  {
  if (!pmgr) pmgr = new Pmgr();
  pmgr->vmode=0; pmgr->avmode=6;
  pmgr->vofst=ofst;
  if(rel) pmgr->trel=rel->tofst, pmgr->brel=rel->bofst, pmgr->hgt=rel->gth();
  else pmgr->trel=pmgr->brel= -1;
  }

void Lith::tedge(int ofst)
  {
  if (!pmgr) pmgr = new Pmgr();
  pmgr->vmode=1; pmgr->avmode=6;
  pmgr->vofst=ofst;
  pmgr->trel=pmgr->brel= -1;
  }

void Lith::tsame(Lith *rel,int ofst)
  {
  if (!pmgr) pmgr = new Pmgr();
  pmgr->vmode=2; pmgr->avmode=6;
  pmgr->vofst=ofst;
  if(rel) pmgr->trel=rel->tofst, pmgr->brel=rel->bofst, pmgr->hgt=rel->gth();
  else pmgr->trel=pmgr->brel= -1;
  }

void Lith::btot(Lith *rel,int ofst)
  {
  if (!pmgr) pmgr = new Pmgr();
  pmgr->vmode=3; pmgr->avmode=6;
  pmgr->vofst=ofst;
  if(rel) pmgr->trel=rel->tofst, pmgr->brel=rel->bofst, pmgr->hgt=rel->gth();
  else pmgr->trel=pmgr->brel= -1;
  }

void Lith::bedge(int ofst)
  {
  if (!pmgr) pmgr = new Pmgr();
  pmgr->vmode=4; pmgr->avmode=6;
  pmgr->vofst=ofst;
  pmgr->trel=pmgr->brel= -1;
  }

void Lith::bsame(Lith *rel,int ofst)
  {
  if (!pmgr) pmgr = new Pmgr();
  pmgr->vmode=5; pmgr->avmode=6;
  pmgr->vofst=ofst;
  if(rel) pmgr->trel=rel->tofst, pmgr->brel=rel->bofst, pmgr->hgt=rel->gth();
  else pmgr->trel=pmgr->brel= -1;
  }

void Lith::vcenter()
  {
  if (!pmgr) pmgr = new Pmgr();
  pmgr->vmode=6; pmgr->avmode=6;
  pmgr->vofst=0;
  pmgr->trel=pmgr->brel= -1;
  }

void Lith::auxltor(Lith *rel,int ofst)
  {
  if (!pmgr) pmgr = new Pmgr();
  pmgr->ahmode=0;
  pmgr->ahofst=ofst;
  if(rel) pmgr->alrel=rel->lofst, pmgr->arrel=rel->rofst, pmgr->awid=rel->gtw();
  else pmgr->alrel=pmgr->arrel= -1;
  }

void Lith::auxledge(int ofst)
  {
  if (!pmgr) pmgr = new Pmgr();
  pmgr->ahmode=1;
  pmgr->ahofst=ofst;
  pmgr->alrel=pmgr->arrel= -1;
  }

void Lith::auxlsame(Lith *rel,int ofst)
  {
  if (!pmgr) pmgr = new Pmgr();
  pmgr->ahmode=2;
  pmgr->ahofst=ofst;
  if(rel) pmgr->alrel=rel->lofst, pmgr->arrel=rel->rofst, pmgr->awid=rel->gtw();
  else pmgr->alrel=pmgr->arrel= -1;
  }

void Lith::auxrtol(Lith *rel,int ofst)
  {
  if (!pmgr) pmgr = new Pmgr();
  pmgr->ahmode=3;
  pmgr->ahofst=ofst;
  if(rel) pmgr->alrel=rel->lofst, pmgr->arrel=rel->rofst, pmgr->awid=rel->gtw();
  else pmgr->alrel=pmgr->arrel= -1;
  }

void Lith::auxredge(int ofst)
  {
  if (!pmgr) pmgr = new Pmgr();
  pmgr->ahmode=4;
  pmgr->ahofst=ofst;
  pmgr->alrel=pmgr->arrel= -1;
  }

void Lith::auxrsame(Lith *rel,int ofst)
  {
  if (!pmgr) pmgr = new Pmgr();
  pmgr->ahmode=5;
  pmgr->ahofst=ofst;
  if(rel) pmgr->alrel=rel->lofst, pmgr->arrel=rel->rofst, pmgr->awid=rel->gtw();
  else pmgr->alrel=pmgr->arrel= -1;
  }

void Lith::auxttob(Lith *rel,int ofst)
  {
  if (!pmgr) pmgr = new Pmgr();
  pmgr->avmode=0;
  pmgr->avofst=ofst;
  if(rel) pmgr->atrel=rel->tofst, pmgr->abrel=rel->bofst, pmgr->ahgt=rel->gth();
  else pmgr->atrel=pmgr->abrel= -1;
  }

void Lith::auxtedge(int ofst)
  {
  if (!pmgr) pmgr = new Pmgr();
  pmgr->avmode=1;
  pmgr->avofst=ofst;
  pmgr->atrel=pmgr->abrel= -1;
  }

void Lith::auxtsame(Lith *rel,int ofst)
  {
  if (!pmgr) pmgr = new Pmgr();
  pmgr->avmode=2;
  pmgr->avofst=ofst;
  if(rel) pmgr->atrel=rel->tofst, pmgr->abrel=rel->bofst, pmgr->ahgt=rel->gth();
  else pmgr->atrel=pmgr->abrel= -1;
  }

void Lith::auxbtot(Lith *rel,int ofst)
  {
  if (!pmgr) pmgr = new Pmgr();
  pmgr->avmode=3;
  pmgr->avofst=ofst;
  if(rel) pmgr->atrel=rel->tofst, pmgr->abrel=rel->bofst, pmgr->ahgt=rel->gth();
  else pmgr->atrel=pmgr->abrel= -1;
  }

void Lith::auxbedge(int ofst)
  {
  if (!pmgr) pmgr = new Pmgr();
  pmgr->avmode=4;
  pmgr->avofst=ofst;
  pmgr->atrel=pmgr->abrel= -1;
  }

void Lith::auxbsame(Lith *rel,int ofst)
  {
  if (!pmgr) pmgr = new Pmgr();
  pmgr->avmode=5;
  pmgr->avofst=ofst;
  if(rel) pmgr->atrel=rel->tofst, pmgr->abrel=rel->bofst, pmgr->ahgt=rel->gth();
  else pmgr->atrel=pmgr->abrel= -1;
  }
