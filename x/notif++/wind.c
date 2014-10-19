/* Primitive window 
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

#include <stdio.h>

#include "notif.h"

#include <X11/cursorfont.h>

hints::hints()
  {
  flg=0;
  normal_hints.flags=0;
  wm_hints.flags=0;
  }

static void hintinit(Widget *w)
  {
  if(!w->hints)
    w->hints = new hints();
  }

static void sendhints(Window w,struct hints *h)
  {
  if(h->flg&1) XSetWMName(dsp,w,&h->window_name);
  if(h->flg&2) XSetWMIconName(dsp,w,&h->icon_name);
  if(h->flg&4) XSetCommand(dsp,w,h->argv,h->argc);
  if(h->flg&8) XSetClassHint(dsp,w,&h->class_hints);
  if(h->normal_hints.flags) XSetWMNormalHints(dsp,w,&h->normal_hints);  
  if(h->wm_hints.flags) XSetWMHints(dsp,w,&h->wm_hints);
  }

void hintwindowname(Widget *w,char *s)
  {
  hintinit(w);
  w->hints->flg|=1;
  XStringListToTextProperty(&s,1,&w->hints->window_name);
  }

void hinticonname(Widget *w,char *s)
  {
  hintinit(w);
  w->hints->flg|=2;
  XStringListToTextProperty(&s,1,&w->hints->icon_name);
  }

void hintposition(Widget *w,int a,int b)
  {
  hintinit(w);
  w->hints->normal_hints.flags|=USPosition;
  w->hints->normal_hints.x=a;
  w->hints->normal_hints.y=b;
  }

void hintminsize(Widget *w,int a,int b)
  {
  hintinit(w);
  w->hints->normal_hints.flags|=PMinSize;
  w->hints->normal_hints.min_width=a;
  w->hints->normal_hints.min_height=b;
  }

void hintmaxsize(Widget *w,int a,int b)
  {
  hintinit(w);
  w->hints->normal_hints.flags|=PMaxSize;
  w->hints->normal_hints.max_width=a;
  w->hints->normal_hints.max_height=b;
  }

void hintresizeinc(Widget *w,int a,int b)
  {
  hintinit(w);
  w->hints->normal_hints.flags|=PResizeInc;
  w->hints->normal_hints.width_inc=a;
  w->hints->normal_hints.height_inc=b;
  }

void hintaspect(Widget *w,int a,int b,int c,int d)
  {
  hintinit(w);
  w->hints->normal_hints.flags|=PAspect;
  w->hints->normal_hints.min_aspect.x=a;
  w->hints->normal_hints.min_aspect.y=b;
  w->hints->normal_hints.max_aspect.x=c;
  w->hints->normal_hints.max_aspect.y=d;
  }

void hintgravity(Widget *w,int a)
  {
  hintinit(w);
  w->hints->normal_hints.win_gravity=a;
  }

void hintargs(Widget *w,char **argv,int argc)
  {
  hintinit(w);
  w->hints->flg|=4;
  w->hints->argv=argv;
  w->hints->argc=argc;
  }

void hintinput(Widget *w,int n)
  {
  hintinit(w);
  w->hints->wm_hints.flags|=InputHint;
  w->hints->wm_hints.input=n;
  }

void hinticonified(Widget *w,int n)
  {
  hintinit(w);
  w->hints->wm_hints.flags|=StateHint;
  w->hints->wm_hints.initial_state=n;
  }

void hinticonpixmap(Widget *w,Pixmap n)
  {
  hintinit(w);
  w->hints->wm_hints.flags|=IconPixmapHint;
  w->hints->wm_hints.icon_pixmap=n;
  }

void hinticonwindow(Widget *w,Window n)
  {
  hintinit(w);
  w->hints->wm_hints.flags|=IconWindowHint;
  w->hints->wm_hints.icon_window=n;
  }

void hinticonposition(Widget *w,int a,int b)
  {
  hintinit(w);
  w->hints->wm_hints.flags|=IconPositionHint;
  w->hints->wm_hints.icon_x=a;
  w->hints->wm_hints.icon_y=b;
  }

void hinticonmask(Widget *w,Pixmap n)
  {
  hintinit(w);
  w->hints->wm_hints.flags|=IconMaskHint;
  w->hints->wm_hints.icon_mask=n;
  }

void hintgroup(Widget *w,XID n)
  {
  hintinit(w);
  w->hints->wm_hints.flags|=WindowGroupHint;
  w->hints->wm_hints.window_group=n;
  }

void hinturgent(Widget *w)
  {
  hintinit(w);
#ifdef XUrgencyHint
  w->hints->wm_hints.flags|=XUrgencyHint;
#endif
  }

void hintclass(Widget *w,char *a,char *b)
  {
  hintinit(w);
  w->hints->flg|=8;
  w->hints->class_hints.res_name=a;
  w->hints->class_hints.res_class=b;
  }

/* Primitive window */

void Wind::stwidth(int n)
  {
  if(w!=n && on_flag) XResizeWindow(dsp,gtwin(),n,gtheight());
  Lith::stwidth(n);
  }

void Wind::stheight(int n)
  {
  if(h!=n && on_flag) XResizeWindow(dsp,gtwin(),gtwidth(),n);
  Lith::stheight(n);
  }

void Wind::stx(int n)
  {
  if(x!=n && on_flag) XMoveWindow(dsp,gtwin(),n,gty());
  x=n;
  }

void Wind::sty(int n)
  {
  if(y!=n && on_flag) XMoveWindow(dsp,gtwin(),gtx(),n);
  y=n;
  }

void Wind::localon()
  {
  int evt=0;
  int mask=0;
  XSetWindowAttributes attributes;
  XGCValues gcv;

  XGetGCValues(dsp,gtbknd(),GCForeground,&gcv);

  attributes.background_pixel=gcv.foreground;
  mask|=CWBackPixel;

  if(pixmap)
    {
    attributes.background_pixmap=pixmap;
    mask|=CWBackPixmap;
    }

  if(cursor)
    {
    attributes.cursor=cursor;
    mask|=CWCursor;
    }

  // FIXME!
/*
  if(x->funcs->expose || x->target && x->target->funcs->expose) evt|=ExposureMask;
  if(x->funcs->enternotify || x->target && x->target->funcs->enternotify) evt|=EnterWindowMask;
  if(x->funcs->leavenotify || x->target && x->target->funcs->leavenotify) evt|=LeaveWindowMask;
  if(x->funcs->buttonpress || x->target && x->target->funcs->buttonpress) evt|=ButtonPressMask;
  if(x->funcs->buttonrelease || x->target && x->target->funcs->buttonrelease) evt|=ButtonReleaseMask;
  if(x->funcs->motionnotify || x->target && x->target->funcs->motionnotify) evt|=ButtonMotionMask;
  if(x->funcs->keypress || x->target && x->target->funcs->keypress) evt|=KeyPressMask|KeyReleaseMask;
  if(x->funcs->configurenotify && gtmom(x)==gtmain(root)) evt|=StructureNotifyMask;
  if(x->funcs->focusin || x->funcs->focusout) evt|=FocusChangeMask;
*/

  evt|=ExposureMask;
  evt|=EnterWindowMask;
  evt|=LeaveWindowMask;
  evt|=ButtonReleaseMask;
  evt|=ButtonMotionMask;
  evt|=KeyPressMask;
  evt|=KeyReleaseMask;
  evt|=StructureNotifyMask;
  evt|=FocusChangeMask;
  evt|=ButtonPressMask;

  attributes.event_mask=evt;
  mask|=CWEventMask;

  if(override_flag)
    {
    attributes.override_redirect=1;
    }
  else attributes.override_redirect=0;
  mask|=CWOverrideRedirect;

  /* Create window */
  win=XCreateWindow(dsp,gtmom()->gtwin(),gtx(),gty(),gtwidth(),gtheight(),0,
                    CopyFromParent,InputOutput,CopyFromParent,
                    mask,&attributes);

  /* Duh, this should be the default */
  if(in==root->gtmain())
    hintinput(target,True);

  if(in==root->gtmain() && ckx() && cky())
    {
    hintposition(target,gtx(),gty());
    }

  if(target && target->hints)
    sendhints(win,target->hints);

  if(transient_flag)
    XSetTransientForHint(dsp,win,gtmom()->gtwin());

  /* Map and raise window */
  XMapRaised(dsp,win);

  if(XSaveContext(dsp,win,wins,(XPointer)this))
    {
    printf("Error\n");
    exit(1);
    }
  }

void Wind::localoff()
  {
  XDeleteContext(dsp,gtwin(),wins);
  XDestroyWindow(dsp,gtwin());
  }

void Wind::stbknd(GC gc)
  {
  bknd=gc;
  if(on_flag)
    {
    XGCValues v;
    XGetGCValues(dsp,gc,GCForeground,&v);
    XSetWindowBackground(dsp,gtwin(),v.foreground);
    }
  }

GC Wind::gtbknd()
  {
  return bknd;
  }

void Wind::stpixmap(Pixmap pix)
  {
  pixmap=pix;
  if(pix && on_flag) XSetWindowBackgroundPixmap(dsp,gtwin(),pix);
  }

void Wind::sttitle(char *s)
  {
  title=s;
  hintwindowname(target,s);
  if(s && on_flag) XStoreName(dsp,gtwin(),s);
  }

void Wind::stcursor(Cursor c)
  {
  cursor=c;
  if(c && on_flag)
    {
    XSetWindowAttributes attr;
    attr.cursor=c;
    XChangeWindowAttributes(dsp,gtwin(),CWCursor,&attr);
    }
  }

void Wind::sttarget(Widget *n)
  {
  target=n;
  }

// Resize

int Wind::configurenotify(XEvent *ev)
  {
  int rtn;
  // Give it to the widget, otherwise give it to our kids
  if (target && target->gtouter() == this && (rtn = target->configurenotify(ev)) != -1)
    return rtn;
  doevent(this,ev);
  return 0;
  }

int Wind::focusin()
  {
  whofocus = target;
  focusflg = 1;
  return 0;
  }

int Wind::focusout()
  {
  focusflg = 0;
  return 0;
  }

/* Creation function */
Wind::Wind()
  {
  target = 0;
  bknd = stdbknd;
  title = 0;
  cursor = 0;
  pixmap = 0;
  }
