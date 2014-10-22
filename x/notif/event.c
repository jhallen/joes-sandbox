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

TASK wsfn[1];

/* These functions query the X server to determine their answers- which makes
 * them slow */

/* Calculate parent of a window */

Window calcmom(win)
Window win;
 {
 int ntree;
 Window *kids;
 Window mom;
 Window root;
 XQueryTree(dsp,win,&root,&mom,&kids,&ntree);
 XFree(kids);
 return mom;
 }

/* Get root x/y coords of a window */

void calcpos(win,x,y)
Window win;
int *x,*y;
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

/* Change pointer event to so that it goes to the window it's pointing at
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

Widget *gttop(x)
Widget *x;
 {
 while(x->in && x->in!=(Lith *)root) x=(Widget *)x->in;
 return x;
 }

/* Translate window coordinates into root coordinates */

void abspos(w,x,y)
Lith *w;
int *x, *y;
 {
 while(w!=gtmain(root))
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
Widget *delivroot;		/* Root to use for action event delivery */
XEvent ev;			/* Current event */

XContext wins;			/* Window -> wind database */

PORT *xport;

struct modalentry
 {
 struct modalentry *next;
 Lith *w;
 } *modalstack=0;

/* Event delivery flags & global variables */

/* Event handler for user action events: ButtonPress, ButtonRelease,
 * MotionNotify, and KeyPress
 */

int douser(x,ev,org)
Widget *x, *org;
XEvent *ev;
 {
 int xpos=0, ypos=0;
 char kbuf[2];
 KeySym kk;

 if(!x) return -1;
 if(gtmain(x)==delivroot) return -1;
 if(!douser(gtmom(x),ev,org)) return 0;
 if(!x->funcs->user) return -1;

 /* Kill any help window */
 helpzap();

 /* Translate coordinates to destination window's reference frame */
 abspos(gtmain(x),&xpos,&ypos);
 ev->xbutton.x=ev->xbutton.x_root-xpos;
 ev->xbutton.y=ev->xbutton.y_root-ypos;
 ev->xany.window=gtwin(x);

 /* Deliver the event */
 switch(ev->type)
  {
  case ButtonPress:
   return x->funcs->user(x,ev->xbutton.button-1+XK_Press1,ev->xbutton.state,
    ev->xbutton.x,ev->xbutton.y,ev->xbutton.time,org);

  case ButtonRelease:
   return x->funcs->user(x,ev->xbutton.button-1+XK_Release1,ev->xbutton.state,
    ev->xbutton.x,ev->xbutton.y,ev->xbutton.time,org);

  case KeyPress:
   if(XLookupString((XKeyEvent *)ev,kbuf,1,0,NULL)) kk=kbuf[0];
   else kk=XLookupKeysym((XKeyEvent *)ev,0);
/*   printf("code=%x state=%x\n",kk,ev->xbutton.state); */
   return x->funcs->user(x,kk,ev->xbutton.state,
    ev->xbutton.x,ev->xbutton.y,ev->xbutton.time,org);

  case MotionNotify:
   return x->funcs->user(x,XK_Motion,ev->xbutton.state,
    ev->xbutton.x,ev->xbutton.y,ev->xbutton.time,org);
  }
 return -1;
 }

int userevent(x,ev)
Widget *x;
XEvent *ev;
 {
 douser(x,ev,x);
 }

void sendevent(root,ev,ignoregrab)
Wind *root;
XEvent *ev;
 {
 Wind *x;

 delivroot=root;

 if(ev->type==Expose && ev->xexpose.count) return;

 /* Find window to deliver event to */

 if(grabber && !ignoregrab &&
   (ev->type==ButtonPress || ev->type==ButtonRelease ||
    ev->type==MotionNotify || ev->type==KeyPress))
  x=grabber;
 else if(XFindContext(dsp,ev->xany.window,wins,((XPointer *)&x)) ||
         x==(Wind *)root) return;
 else if(modalstack && !ignoregrab &&
         (ev->type==ButtonPress || ev->type==ButtonRelease ||
          ev->type==MotionNotify || ev->type==KeyPress))
  {
  Lith *q;
  for(q=(Lith *)x;q!=root && q!=modalstack->w;q=gtmom(q));
  if(q!=modalstack->w) return;
  }

 /* If window has a target, redirect event to target's event handler */

 if(!((int (**)())(&x->funcs->dummy0))[ev->type] && x->target &&
    ((int (**)())(&x->target->funcs->dummy0))[ev->type]) x=x->target;

 /* Deliver event */

 if(((int (**)())(&x->funcs->dummy0))[ev->type])
    ((int (**)())(&x->funcs->dummy0))[ev->type](x,ev);
 }

void doevent(root,ev)
Lith *root;
XEvent *ev;
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

/* Process pending X events */
int ws()
 {
 doX();
 stportread(xport,fn0(wsfn,ws),0);
 return 0;
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

void grab(w,n)
Widget *w;
 {
 struct grab *grab=malloc(sizeof(struct grab));
 greedy=n;
 grab->next=grabstack;
 grab->owner=grabber;
 grab->focus=whofocus;
 grabstack=grab;
 grabber=w;
 whofocus=w;
 XGrabPointer(dsp,gtwin(w),!greedy,
              ButtonPressMask|ButtonReleaseMask|EnterWindowMask|LeaveWindowMask,
              GrabModeAsync,GrabModeAsync,None,None,CurrentTime);
 XGrabKeyboard(dsp,gtwin(w),False,GrabModeAsync,GrabModeAsync,CurrentTime);
 }

/* Switch type of current grab */

void greedygrab(n)
 {
 if(greedy!=n && grabber)
  {
  greedy=n;
  XGrabPointer(dsp,gtwin(grabber),!greedy,
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
  if(grabber->funcs->ungrab) grabber->funcs->ungrab(grabber);
  grabber=grabstack->owner;
  whofocus=grabstack->focus;
  n=grabstack;
  grabstack=n->next;
  free(n);
  if(grabber)
   {
   int xpos, ypos;
   XGrabPointer(dsp,gtwin(grabber),!greedy,
                ButtonPressMask|ButtonReleaseMask|EnterWindowMask|LeaveWindowMask,
                GrabModeAsync,GrabModeAsync,None,None,CurrentTime);
   XGrabKeyboard(dsp,gtwin(grabber),False,GrabModeAsync,GrabModeAsync,
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

void zapto(z)
Widget *z;
 {
 if(grabber)
  {
  XUngrabPointer(dsp,CurrentTime);
  XUngrabKeyboard(dsp,CurrentTime);
  }
 while(grabber && grabber!=z)
  {
  struct grab *n;
  if(grabber->funcs->ungrab) grabber->funcs->ungrab(grabber);
  grabber=grabstack->owner;
  whofocus=grabstack->focus;
  n=grabstack;
  grabstack=n->next;
  free(n);
  }
 if(grabber)
  {
  int xpos, ypos;
  XGrabPointer(dsp,gtwin(grabber),!greedy,
               ButtonPressMask|ButtonReleaseMask|EnterWindowMask|LeaveWindowMask,
               GrabModeAsync,GrabModeAsync,None,None,CurrentTime);
  XGrabKeyboard(dsp,gtwin(grabber),False,GrabModeAsync,GrabModeAsync,
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

void modal(w)
Widget *w;
 {
 struct modalentry *m=malloc(sizeof(struct modalentry));
 m->w=gtouter(w);
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

 root=mk(Widget);

 root->win=root->outer->win=root->main->win=DefaultRootWindow(dsp);
 root->x=root->main->x=root->outer->x=0;
 root->y=root->main->y=root->outer->y=0;
 root->w=root->main->w=root->outer->w=DisplayWidth(dsp,DefaultScreen(dsp));
 root->h=root->main->h=root->outer->h=DisplayHeight(dsp,DefaultScreen(dsp));
 root->main->flags=root->outer->flags=root->flags=flgon+flgenabled;
 stpmgr(root,NULL);

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
 xport=portopen(ConnectionNumber(dsp));
 stnomode(xport);
 stportread(xport,fn0(wsfn,ws),0);
 }

void placedialog(cur,new)
Widget *cur, *new;
 {
 int curx=0, cury=0;
 int newx, newy;
 abspos(gtmain(cur),&curx,&cury);
 if(cury+gth(cur)+gth(new)<=gth(root))
  newy=cury+gth(cur);
 else if(cury>=gth(new))
  newy=cury-gth(new);
 else if(cury+gth(new)<=gth(root))
  newy=cury;
 else
  {
  newy=0;
  newx=0;
  goto skip;
  }
 if(newy==cury)
  if(curx+gtw(cur)+gtw(new)<=gtw(root))
   newx=curx+gtw(cur);
  else if(gtw(new)<=curx)
   newx=curx-gtw(new);
  else
   {
   newy=0;
   newx=0;
   goto skip;
   }
 else
  if(curx+gtw(new)<=gtw(root))
   newx=curx;
  else if(curx+gtw(cur)-gtw(new)>=0)
   newx=curx+gtw(cur)-gtw(new);
  else
   {
   newx=0;
   newy=0;
   }
 skip:
 stx(new,newx);
 sty(new,newy);
 }
