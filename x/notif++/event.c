/* Event handler 
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
#include <sys/time.h>
#include <sys/types.h>
#include "notif.h"

Kmap<Key> stdkmap;


/* These functions query the X server to determine their answers- which makes
 * them slow */

/* Calculate parent of a window */

Window calcmom(Window win)
  {
  unsigned ntree;
  Window *kids;
  Window mom;
  Window root;
  XQueryTree(dsp,win,&root,&mom,&kids,&ntree);
  XFree(kids);
  return mom;
  }

/* Get root x/y coords of a window */

void calcpos(Window win,int *x,int *y)
  {
  XWindowAttributes attr;
  int xx=0,yy=0;
  do
    {
    XGetWindowAttributes(dsp,win,&attr);
    xx+=attr.x; yy+=attr.y;
    win=calcmom(win);
    }
    while(win!=RootWindow(dsp,0));
  *x=xx; *y=yy;
  }

/* Change pointer event so that it goes to the window it's pointing at
 * (bypassing grab) */

void calcpointer()
  {
  int crap1,crap2;
  unsigned int crap3;
  Window crap4;
  ev.xbutton.window=ev.xbutton.root;
  loop:
  XQueryPointer(dsp,ev.xbutton.window,
                &crap4,
                &ev.xbutton.subwindow,
                &crap1,&crap2,
                &ev.xbutton.x,&ev.xbutton.y,
                &crap3);
  if(ev.xbutton.subwindow!=None)
    {
    ev.xbutton.window=ev.xbutton.subwindow;
    goto loop;
    }
  }

/* These functions are fast */

/* Find top-most window of a hierarchy */

Widget *gttop(Widget *x)
  {
  while(x->in && x->in != (Lith *)root)
    x = (Widget *)x->in;
  return x;
  }

/* Translate window coordinates into root coordinates */

void abspos(Lith *w,int *x,int *y)
  {
  while(w!=root->gtmain())
    {
    *x+=w->x;
    *y+=w->y;
    w=w->in;
    }
  }

GC stdbknd;
GC stdtopbknd;
GC stdfgnd;
GC stdshadowbknd;
GC stdshadowtop;
GC stdshadowbot;
GC stdborderbknd;
GC stdoutline;

Display *dsp;			/* Primary display */
Widget *root;			/* Root window */
Lith *delivroot;		/* Root to use for action event delivery */
XEvent ev;			/* Current event */

XContext wins;			/* Window -> wind database */

Port *xport;

struct modalentry
  {
  struct modalentry *next;
  Lith *w;
  } *modalstack=0;

/* Event delivery flags & global variables */

/* Event handler for user action events: ButtonPress, ButtonRelease,
 * MotionNotify, and KeyPress
 *
 * Deliver event to each Widget on parent stack, starting with 'delivroot'.
 * Delivery stops on first Widget which returns something other than -1.
 */

int douser(Widget *x,XEvent *ev,Widget *org)
  {
  int rtn;
  int xpos=0, ypos=0;
  char kbuf[2];
  KeySym kk;

  if (!x) return -1;
  if (x->gtmain() == delivroot) return -1;
  if ((rtn = douser((Widget *)x->gtmom(),ev,org)) != -1) return rtn;

  /* Kill any help window */
  helpzap();

  /* Translate coordinates to destination window's reference frame */
  abspos(x->gtmain(),&xpos,&ypos);
  ev->xbutton.x=ev->xbutton.x_root-xpos;
  ev->xbutton.y=ev->xbutton.y_root-ypos;
  ev->xany.window=x->gtwin();

  /* Deliver the event */
  switch(ev->type)
    {
    case ButtonPress:
      return x->user(ev->xbutton.button-1+XK_Press1,ev->xbutton.state,ev->xbutton.x,ev->xbutton.y,ev->xbutton.time,org);

    case ButtonRelease:
      return x->user(ev->xbutton.button-1+XK_Release1,ev->xbutton.state,ev->xbutton.x,ev->xbutton.y,ev->xbutton.time,org);

    case KeyPress:
      if (XLookupString((XKeyEvent *)ev,kbuf,1,0,NULL)) kk=kbuf[0];
      else kk=XLookupKeysym((XKeyEvent *)ev,0);
      // printf("code=%x state=%x\n",kk,ev->xbutton.state);
      return x->user(kk,ev->xbutton.state,ev->xbutton.x,ev->xbutton.y,ev->xbutton.time,org);

    case MotionNotify:
      return x->user(XK_Motion,ev->xbutton.state,ev->xbutton.x,ev->xbutton.y,ev->xbutton.time,org);
   }
 return -1;
 }

int userevent(Widget *x,XEvent *ev)
  {
  return douser(x,ev,x);
  }

int deliver_x_event(Lith *x,XEvent *ev)
  {
  switch(ev->type)
    {
    case KeyPress: return x->keypress(ev);
    case KeyRelease: return x->keyrelease(ev);
    case ButtonPress: return x->buttonpress(ev);
    case ButtonRelease: return x->buttonrelease(ev);
    case MotionNotify: return x->motionnotify(ev);
    case EnterNotify: return x->enternotify(ev);
    case LeaveNotify: return x->leavenotify(ev);
    case FocusIn: return x->focusin(ev);
    case FocusOut: return x->focusout(ev);
    case KeymapNotify: return x->keymapnotify(ev);
    case Expose: return x->expose(ev);
    case GraphicsExpose: return x->graphicsexpose(ev);
    case NoExpose: return x->noexpose(ev);
    case VisibilityNotify: return x->visibilitynotify(ev);
    case CreateNotify: return x->createnotify(ev);
    case DestroyNotify: return x->destroynotify(ev);
    case UnmapNotify: return x->unmapnotify(ev);
    case MapNotify: return x->mapnotify(ev);
    case MapRequest: return x->maprequest(ev);
    case ReparentNotify: return x->reparentnotify(ev);
    case ConfigureNotify: return x->configurenotify(ev);
    case ConfigureRequest: return x->configurerequest(ev);
    case GravityNotify: return x->gravitynotify(ev);
    case ResizeRequest: return x->resizerequest(ev);
    case CirculateNotify: return x->circulatenotify(ev);
    case CirculateRequest: return x->circulaterequest(ev);
    case PropertyNotify: return x->propertynotify(ev);
    case SelectionClear: return x->selectionclear(ev);
    case SelectionRequest: return x->selectionrequest(ev);
    case SelectionNotify: return x->selectionnotify(ev);
    case ColormapNotify: return x->colormapnotify(ev);
    case ClientMessage: return x->clientmessage(ev);
    case MappingNotify: return x->mappingnotify(ev);
    }
  return -1;
  }

void sendevent(Wind *root,XEvent *ev,int ignoregrab)
  {
  Wind *x=0;

  delivroot=root;

  if(ev->type==Expose && ev->xexpose.count) return;

  /* Find window to deliver event to */

  if(grabber && !ignoregrab &&
    (ev->type==ButtonPress || ev->type==ButtonRelease ||
     ev->type==MotionNotify || ev->type==KeyPress))
   x=grabber->gtmain(); // used to be just grabber...
  else if(XFindContext(dsp,ev->xany.window,wins,((XPointer *)&x)) ||
          x==(Wind *)root) return;
  else if(modalstack && !ignoregrab &&
          (ev->type==ButtonPress || ev->type==ButtonRelease ||
           ev->type==MotionNotify || ev->type==KeyPress))
    {
    Lith *q;
    for(q=(Lith *)x;q!=root && q!=modalstack->w;q=q->gtmom());
    if(q!=modalstack->w) return;
    }

  /* If window has a target, redirect event to target's event handler */

  if(deliver_x_event(x,ev)==-1) // We don't want the event... try target
    if(x->target)
      deliver_x_event(x->target,ev);
  }

void doevent(Wind *root,XEvent *ev)
  {
  sendevent(root,ev,1);
  }

void wsflsh()
  {
  XSync(dsp,0);
  }

void doX()
  {
  wsflsh();
  while(XPending(dsp))
    {
    XNextEvent(dsp,&ev);
    sendevent(root->outer,&ev,0);
    }
  }

void ws(char *,int);

Fn0_2<void,char *,int> *wsfn;

/* Process pending X events */
void ws(char *b,int l)
  {
  doX();
  xport->stportread(wsfn,0);
  }

/* Stack of previous grabbers */

struct grab
  {
  struct grab *next;
  Widget *owner;
  Widget *focus;
  } *grabstack;

/* Current grabber */

Widget *grabber;	/* Current grabber */
Widget *whofocus;	/* Who has focus */
int focusflg;		/* Set if our client has focus */
int greedy;		/* Set if grabber is greedy */

/* Grab input for a widget.  All future ButtonPress, ButtonRelease and
 * KeyPress events will be delivered to w instead of their natural
 * destination.  If 'n' is true, the grab is 'greedy', which means that all
 * other events besides ButtonPress, ButtonRelease and KeyPress will be
 * redirected to w.  Greedy grabs are especially useful for preventing enter
 * and leave notify events from being naturally delivered.  Enter and leave
 * notify are used to trigger cascade buttons in menus, but you don't want
 * this behavior once the user has released mouse button, and changed the 
 * menu to a trigger-on-click menu, instead of a trigger-release-menu.
 */

void grab(Widget *w,int n)
  {
  struct grab *grab=(struct grab *)malloc(sizeof(struct grab));
  greedy=n;
  grab->next=grabstack;
  grab->owner=grabber;
  grab->focus=whofocus;
  grabstack=grab;
  grabber=w;
  whofocus=w;
  XGrabPointer(dsp,w->gtwin(),!greedy,
               ButtonPressMask|ButtonReleaseMask|EnterWindowMask|LeaveWindowMask,
               GrabModeAsync,GrabModeAsync,None,None,CurrentTime);
  XGrabKeyboard(dsp,w->gtwin(),False,GrabModeAsync,GrabModeAsync,CurrentTime);
  }

/* Switch type of current grab */

void greedygrab(int n)
  {
  if(greedy!=n && grabber)
    {
    greedy=n;
    XGrabPointer(dsp,grabber->gtwin(),!greedy,
                 ButtonPressMask|ButtonReleaseMask|EnterWindowMask|LeaveWindowMask,
                 GrabModeAsync,GrabModeAsync,None,None,CurrentTime);
    }
  }

/* Revert to prevous grab and send the event to it.  If the grabber decides
 * that the event wasn't for him and should be delivered normally as usual.
 */

void popgrab()
  {
  if(grabber)
    {
    struct grab *n;
    XUngrabPointer(dsp,CurrentTime);
    XUngrabKeyboard(dsp,CurrentTime);
    grabber->ungrab();
    grabber=grabstack->owner;
    whofocus=grabstack->focus;
    n=grabstack;
    grabstack=n->next;
    free(n);
    if(grabber)
      {
      int xpos, ypos;
      XGrabPointer(dsp,grabber->gtwin(),!greedy,
                   ButtonPressMask|ButtonReleaseMask|EnterWindowMask|LeaveWindowMask,
                   GrabModeAsync,GrabModeAsync,None,None,CurrentTime);
      XGrabKeyboard(dsp,grabber->gtwin(),False,GrabModeAsync,GrabModeAsync,
                    CurrentTime);
      sendevent(root->outer,&ev,0);
      }
    else
      {
      greedy=0;
      calcpointer();
      XSendEvent(dsp,PointerWindow,True,0,&ev);
      }
    }
  }

/* Pop grab off stack.  Send no events: send grab killed event instead. */

void zapto(Widget *z)
  {
  if(grabber)
    {
    XUngrabPointer(dsp,CurrentTime);
    XUngrabKeyboard(dsp,CurrentTime);
    }
  while(grabber && grabber!=z)
    {
    struct grab *n;
    grabber->ungrab();
    grabber=grabstack->owner;
    whofocus=grabstack->focus;
    n=grabstack;
    grabstack=n->next;
    free(n);
    }
  if(grabber)
    {
    int xpos, ypos;
    XGrabPointer(dsp,grabber->gtwin(),!greedy,
                 ButtonPressMask|ButtonReleaseMask|EnterWindowMask|LeaveWindowMask,
                 GrabModeAsync,GrabModeAsync,None,None,CurrentTime);
    XGrabKeyboard(dsp,grabber->gtwin(),False,GrabModeAsync,GrabModeAsync,
                  CurrentTime);
    }
  else greedy=0;
  }

void zapgrab()
  {
  if(grabstack) zapto(grabstack->owner);
  }

/* Zap all grabs */

void grabunwind()
  {
  zapto(0);
  }

/* Modal push */

void modal(Widget *w)
  {
  struct modalentry *m=(struct modalentry *)malloc(sizeof(struct modalentry));
  m->w=w->gtouter();
  m->next=modalstack;
  modalstack=m;
  }

/* Modal pop */

void modalpop()
  {
  struct modalentry *m=modalstack;
  if(m)
    {
    modalstack=m->next;
    free(m);
    }
  }

extern "C" XContext XrmUniqueQuark();

/* Initialize windowing system */
void izws()
  {
  int q;
  if(!(dsp=XOpenDisplay("")))
    {
    fprintf(stderr,"Couldn't open display\n");
    exit(1);
    }
  wins=XUniqueContext();

  stdkmap.add("^I",new Mfn0_6<int,Widget,int,int,int,int,Time,Widget *>(&Widget::ufocusnext,NULL));
  stdkmap.add("Right",new Mfn0_6<int,Widget,int,int,int,int,Time,Widget *>(&Widget::ufocusnext,NULL));
  stdkmap.add("Down",new Mfn0_6<int,Widget,int,int,int,int,Time,Widget *>(&Widget::ufocusnext,NULL));
  stdkmap.add("Left",new Mfn0_6<int,Widget,int,int,int,int,Time,Widget *>(&Widget::ufocusprev,NULL));
  stdkmap.add("Up",new Mfn0_6<int,Widget,int,int,int,int,Time,Widget *>(&Widget::ufocusprev,NULL));

  root=new Widget();

  root->win=root->outer->win=root->main->win=DefaultRootWindow(dsp);
  root->x=root->main->x=root->outer->x=0;
  root->y=root->main->y=root->outer->y=0;
  root->w=root->main->w=root->outer->w=DisplayWidth(dsp,DefaultScreen(dsp));
  root->h=root->main->h=root->outer->h=DisplayHeight(dsp,DefaultScreen(dsp));
  root->main->on_flag=1;
  root->main->enabled_flag=1;
  root->outer->on_flag=1;
  root->outer->enabled_flag=1;
  root->on_flag=1;
  root->enabled_flag=1;

  stdtopbknd=getgc("8x13","grey69");
  stdbknd=getgc("8x13","grey69");
  stdfgnd=getgc("8x13","black");
  stdshadowbknd=getgc("8x13","grey69");
  stdshadowtop=getgc("8x13","grey91");
  stdshadowbot=getgc("8x13","grey25");
  stdborderbknd=getgc("8x13","black");
  stdoutline=getgc("8x13","black");

  root->fgnd=root->main->fgnd=root->outer->fgnd=stdfgnd;
  root->main->bknd=root->outer->bknd=stdbknd;

  /* printf("%d\n",ConnectionNumber(dsp)); */
  xport=new Port(ConnectionNumber(dsp));
  xport->stnomode();
  wsfn=new Fn0_2<void,char *,int>(ws);
  xport->stportread(wsfn,0);
  }

void placedialog(Widget *cur,Widget *new_w)
  {
  int curx=0, cury=0;
  int newx, newy;
  abspos(cur->gtmain(),&curx,&cury);
  if(cury+cur->gth()+new_w->gth()<=root->gth())
   newy=cury+cur->gth();
  else if(cury>=new_w->gth())
   newy=cury-new_w->gth();
  else if(cury+new_w->gth()<=root->gth())
   newy=cury;
  else
   {
   newy=0;
   newx=0;
   goto skip;
   }
  if(newy==cury)
   if(curx+cur->gtw()+new_w->gtw()<=root->gtw())
    newx=curx+cur->gtw();
   else if(new_w->gtw()<=curx)
    newx=curx-new_w->gtw();
   else
    {
    newy=0;
    newx=0;
    goto skip;
    }
  else
   if(curx+new_w->gtw()<=root->gtw())
    newx=curx;
   else if(curx+cur->gtw()-new_w->gtw()>=0)
    newx=curx+cur->gtw()-new_w->gtw();
   else
    {
    newx=0;
    newy=0;
    }
  skip:
  new_w->stx(newx);
  new_w->sty(newy);
  }
