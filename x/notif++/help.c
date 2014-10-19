/* Pop-up help widget
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

static int helpflg=0;		/* Set if help is on */
static Fn_0<void> *thelp=0;	/* Timer for turning help on */
static Widget *helpwin=0;	/* If help is on: current help window */

void helpzap()
  {
  if(helpflg)
    {
    if(helpwin) helpwin->clmom(), helpwin=0;
    helpflg=0;
    }
  else
    {
    cancel(thelp);
    thelp=0;
    }
  }

void Help::disphelp()
  {
  Widget *top=gttop(target);
  int xpos, ypos;
  Lith *d;
  thelp=0;
  helpflg=1;
  helpwin=help;
  xpos=5; ypos=target->gth();
  abspos(target->gtmain(),&xpos,&ypos);
  helpwin->stx(xpos);
  helpwin->sty(ypos);
  helpwin->stoverride();
  root->add(helpwin);
  }

void Help::enter(int x,int y,int mode,int detail)
  {
  if(help && !thelp && !helpwin)
    if(!helpflg) submit(750000,thelp = new Mfn0_0<void,Help>(&Help::disphelp,this));
    else disphelp();
  }

static void *helpchk(Lith *w)
  {
  if(((Widget *)w)->help && ((Widget *)w)->help->help) return w;
  else return 0;
  }

void Help::leave(int x,int y,int mode,int detail)
  {
  if(helpflg)
    {
    Fn0_1<void *,Lith *> fn(helpchk);
    if(helpwin) helpwin->clmom(), helpwin=0;
    if(!gttop(target)->xyapply(target->gtx()+x,target->gty()+y,&fn))
      helpflg=0;
    }
  else
    {
    cancel(thelp);
    thelp=0;
    }
  }

void Help::deactivate()
  {
  if(helpwin) helpwin->clmom(), helpwin=0;
  helpflg=0;
  cancel(thelp);
  thelp=0;
  }

Help::~Help()
  {
  deactivate();
  if(help) delete help;
  }

void Help::sthelp(Widget *s)
  {
  if(help) delete help;
  help=s;
  }

void Help::sttarget(Widget *t)
  {
  target=t;
  }

Help::Help()
  {
  help=0;
  target=0;
  }

/* Convenience functions */

void sthelptext(Widget *w,char *t)
  {
  Text *ht;
  w->sthelpmgr(new Help());
  ht=new Text();
    ht->stbknd(getgc("8x13","yellow"));
    ht->stborder(new Shadow());
    ht->st(t);
  w->help->sthelp(ht);
  }
