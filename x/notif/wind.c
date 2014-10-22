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

#include "notif.h"
#include <X11/cursorfont.h>

/* Window manager hints */

struct hints
 {
 int flg;
 XSizeHints normal_hints;
 XTextProperty window_name;
 XTextProperty icon_name;
 XWMHints wm_hints;
 char **argv;
 int argc;
 XClassHint class_hints;
 };

static void hintinit(w)
Widget *w;
 {
 if(!w->hints)
  {
  w->hints=malloc(sizeof(struct hints));
  w->hints->flg=0;
  w->hints->normal_hints.flags=0;
  w->hints->wm_hints.flags=0;
  }
 }

static void sendhints(w,h)
struct hints *h;
 {
 if(h->flg&1) XSetWMName(dsp,w,&h->window_name);
 if(h->flg&2) XSetWMIconName(dsp,w,&h->icon_name);
 if(h->flg&4) XSetCommand(dsp,w,h->argv,h->argc);
 if(h->flg&8) XSetClassHint(dsp,w,&h->class_hints);
 if(h->normal_hints.flags) XSetWMNormalHints(dsp,w,&h->normal_hints);  
 if(h->wm_hints.flags) XSetWMHints(dsp,w,&h->wm_hints);
 }

void hintwindowname(w,s)
Widget *w;
char *s;
 {
 hintinit(w);
 w->hints->flg|=1;
 XStringListToTextProperty(&s,1,&w->hints->window_name);
 }

void hinticonname(w,s)
Widget *w;
char *s;
 {
 hintinit(w);
 w->hints->flg|=2;
 XStringListToTextProperty(&s,1,&w->hints->icon_name);
 }

void hintposition(w,a,b)
Widget *w;
 {
 hintinit(w);
 w->hints->normal_hints.flags|=USPosition;
 w->hints->normal_hints.x=a;
 w->hints->normal_hints.y=b;
 }

void hintminsize(w,a,b)
Widget *w;
 {
 hintinit(w);
 w->hints->normal_hints.flags|=PMinSize;
 w->hints->normal_hints.min_width=a;
 w->hints->normal_hints.min_height=b;
 }

void hintmaxsize(w,a,b)
Widget *w;
 {
 hintinit(w);
 w->hints->normal_hints.flags|=PMaxSize;
 w->hints->normal_hints.max_width=a;
 w->hints->normal_hints.max_height=b;
 }

void hintresizeinc(w,a,b)
Widget *w;
 {
 hintinit(w);
 w->hints->normal_hints.flags|=PResizeInc;
 w->hints->normal_hints.width_inc=a;
 w->hints->normal_hints.height_inc=b;
 }

void hintaspect(w,a,b,c,d)
Widget *w;
 {
 hintinit(w);
 w->hints->normal_hints.flags|=PAspect;
 w->hints->normal_hints.min_aspect.x=a;
 w->hints->normal_hints.min_aspect.y=b;
 w->hints->normal_hints.max_aspect.x=c;
 w->hints->normal_hints.max_aspect.y=d;
 }

void hintgravity(w,a)
Widget *w;
 {
 hintinit(w);
 w->hints->normal_hints.win_gravity=a;
 }

void hintargs(w,argv,argc)
Widget *w;
char **argv;
 {
 hintinit(w);
 w->hints->flg|=4;
 w->hints->argv=argv;
 w->hints->argc=argc;
 }

void hintinput(w,n)
Widget *w;
 {
 hintinit(w);
 w->hints->wm_hints.flags|=InputHint;
 w->hints->wm_hints.input=n;
 }

void hinticonified(w,n)
Widget *w;
 {
 hintinit(w);
 w->hints->wm_hints.flags|=StateHint;
 w->hints->wm_hints.initial_state=n;
 }

void hinticonpixmap(w,n)
Widget *w;
Pixmap n;
 {
 hintinit(w);
 w->hints->wm_hints.flags|=IconPixmapHint;
 w->hints->wm_hints.icon_pixmap=n;
 }

void hinticonwindow(w,n)
Widget *w;
Window n;
 {
 hintinit(w);
 w->hints->wm_hints.flags|=IconWindowHint;
 w->hints->wm_hints.icon_window=n;
 }

void hinticonposition(w,a,b)
Widget *w;
 {
 hintinit(w);
 w->hints->wm_hints.flags|=IconPositionHint;
 w->hints->wm_hints.icon_x=a;
 w->hints->wm_hints.icon_y=b;
 }

void hinticonmask(w,n)
Widget *w;
Pixmap n;
 {
 hintinit(w);
 w->hints->wm_hints.flags|=IconMaskHint;
 w->hints->wm_hints.icon_mask=n;
 }

void hintgroup(w,n)
Widget *w;
XID n;
 {
 hintinit(w);
 w->hints->wm_hints.flags|=WindowGroupHint;
 w->hints->wm_hints.window_group=n;
 }

void hinturgent(w)
Widget *w;
 {
 hintinit(w);
#ifdef XUrgencyHint
 w->hints->wm_hints.flags|=XUrgencyHint;
#endif
 }

void hintclass(w,a,b)
Widget *w;
char *a, *b;
 {
 hintinit(w);
 w->hints->flg|=8;
 w->hints->class_hints.res_name=a;
 w->hints->class_hints.res_class=b;
 }

/* Primitive window */

static void windstwidth(x,w)
Wind *x;
 {
 if(x->w!=w && (gtflg(x)&flgon)) XResizeWindow(dsp,x->win,w,gtheight(x));
 (lithfuncs.stwidth)(x,w);
 }

static void windstheight(x,h)
Wind *x;
 {
 if(x->h!=h && (gtflg(x)&flgon)) XResizeWindow(dsp,gtwin(x),gtwidth(x),h);
 (lithfuncs.stheight)(x,h);
 }

static void windstx(x,n)
Wind *x;
 {
 if(x->x!=n && (gtflg(x)&flgon)) XMoveWindow(dsp,x->win,n,gty(x));
 x->x=n;
 }

static void windsty(x,n)
Wind *x;
 {
 if(x->y!=n && (gtflg(x)&flgon)) XMoveWindow(dsp,x->win,gtx(x),n);
 x->y=n;
 }

static void windon(x)
Wind *x;
 {
 int evt=0;
 int mask=0;
 XSetWindowAttributes attributes;
 XGCValues gcv;

 XGetGCValues(dsp,gtbknd(x),GCForeground,&gcv);

 attributes.background_pixel=gcv.foreground;
 mask|=CWBackPixel;

 if(x->pixmap)
  {
  attributes.background_pixmap=x->pixmap;
  mask|=CWBackPixmap;
  }

 if(x->cursor)
  {
  attributes.cursor=x->cursor;
  mask|=CWCursor;
  }

 if(x->funcs->expose || x->target && x->target->funcs->expose) evt|=ExposureMask;
 if(x->funcs->enternotify || x->target && x->target->funcs->enternotify) evt|=EnterWindowMask;
 if(x->funcs->leavenotify || x->target && x->target->funcs->leavenotify) evt|=LeaveWindowMask;
 if(x->funcs->buttonpress || x->target && x->target->funcs->buttonpress) evt|=ButtonPressMask;
 if(x->funcs->buttonrelease || x->target && x->target->funcs->buttonrelease) evt|=ButtonReleaseMask;
 if(x->funcs->motionnotify || x->target && x->target->funcs->motionnotify) evt|=ButtonMotionMask;
 if(x->funcs->keypress || x->target && x->target->funcs->keypress) evt|=KeyPressMask|KeyReleaseMask;
 if(x->funcs->configurenotify && gtmom(x)==gtmain(root)) evt|=StructureNotifyMask;
 if(x->funcs->focusin || x->funcs->focusout) evt|=FocusChangeMask;

 attributes.event_mask=evt;
 mask|=CWEventMask;

 if(gtflg(x)&flgoverride) attributes.override_redirect=1;
 else attributes.override_redirect=0;
 mask|=CWOverrideRedirect;

 /* Create window */
 x->win=XCreateWindow(dsp,gtwin(gtmom(x)),gtx(x),gty(x),gtwidth(x),gtheight(x),0,
                          CopyFromParent,InputOutput,CopyFromParent,
                          mask,&attributes);

 /* Duh, this should be the default */
 if(x->in==gtmain(root))
  hintinput(x->target,True);

 if(x->in==gtmain(root) && ckx(x) && cky(x))
  hintposition(x->target,gtx(x),gty(x));

 if(x->target && x->target->hints)
  sendhints(x->win,x->target->hints);

 if(gtflg(x)&flgtransient)
  XSetTransientForHint(dsp,x->win,gtwin(gtmom(x)));

 /* Map and raise window */
 XMapRaised(dsp,x->win);

 if(XSaveContext(dsp,x->win,wins,(XPointer)x))
  {
  printf("Error\n");
  exit(1);
  }
 }

static void windoff(x)
Wind *x;
 {
 XDeleteContext(dsp,gtwin(x),wins);
 XDestroyWindow(dsp,gtwin(x));
 }

static void windstbknd(in,gc)
Wind *in;
GC gc;
 {
 in->bknd=gc;
 if(gtflg(in)&flgon)
  {
  XGCValues v;
  XGetGCValues(dsp,gc,GCForeground,&v);
  XSetWindowBackground(dsp,gtwin(in),v.foreground);
  }
 }

static GC windgtbknd(in)
Wind *in;
 {
 return in->bknd;
 }

static void windstpixmap(w,pix)
Wind *w;
Pixmap pix;
 {
 w->pixmap=pix;
 if(pix && (gtflg(w)&flgon)) XSetWindowBackgroundPixmap(dsp,gtwin(w),pix);
 }

static void windsttitle(w,s)
Wind *w;
char *s;
 {
 w->title=s;
 hintwindowname(w->target,s);
 if(s && gtflg(w)&flgon) XStoreName(dsp,gtwin(w),s);
 }

static void windstcursor(w,c)
Wind *w;
Cursor c;
 {
 w->cursor=c;
 if(c && (gtflg(w)&flgon))
  {
  XSetWindowAttributes attr;
  attr.cursor=c;
  XChangeWindowAttributes(dsp,gtwin(w),CWCursor,&attr);
  }
 }

static void windsttarget(t,target)
Wind *t;
Widget *target;
 {
 t->target=target;
 }

static int windresize(w,ev)
Wind *w;
XEvent *ev;
 {
 if(w->target && w->target->funcs->configurenotify && (Wind *)gtouter(w->target)==w)
  return w->target->funcs->configurenotify(w->target,ev);
 else
  {
  doevent(w,ev);
  return 0;
  }
 }

static int windfocusin(w)
Wind *w;
 {
 whofocus=w->target;
 focusflg=1;
 }

static int windfocusout(w)
Wind *w;
 {
 focusflg=0;
 }

struct windfuncs windfuncs;

/* Creation function */
Wind *mkWind(x)
Wind *x;
 {
 mkLith(x);
 if(!windfuncs.on)
  {
  mcpy(&windfuncs,&lithfuncs,sizeof(struct lithfuncs));
  windfuncs.stwidth=windstwidth;
  windfuncs.stheight=windstheight;
  windfuncs.stw=windstwidth;
  windfuncs.sth=windstheight;
  windfuncs.stiw=windstwidth;
  windfuncs.stih=windstheight;
  windfuncs.stwidth=windstwidth;
  windfuncs.stheight=windstheight;
  windfuncs.stx=windstx;
  windfuncs.sty=windsty;
  windfuncs.localon=windon;
  windfuncs.localoff=windoff;
  windfuncs.stbknd=windstbknd;
  windfuncs.gtbknd=windgtbknd;
  windfuncs.stpixmap=windstpixmap;
  windfuncs.stcursor=windstcursor;
  windfuncs.sttitle=windsttitle;
  windfuncs.sttarget=windsttarget;
  windfuncs.configurenotify=windresize;
  windfuncs.focusin=windfocusin;
  windfuncs.focusout=windfocusout;
  }
 x->funcs= &windfuncs;
 x->target=0;
 x->bknd=stdbknd;
 x->title=0;
 x->cursor=0;
 x->pixmap=0;
 return x;
 }
