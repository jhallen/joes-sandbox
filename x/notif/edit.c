/* Edit widget 
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

static void editrm(t)
Edit *t;
 {
 cancel(t->blink);
 cancel(t->fn); t->fn=0;
 free(t->buf);
 if(t->org) free(t->org);
 (widgetfuncs.rm)(t);
 }

static void editst(t,str)
Edit *t;
char *str;
 {
 t->len=strlen(str);
 if(t->org) free(t->org);
 t->org=strcpy(malloc(strlen(str)+1),str);
 if(t->len+1>t->bksize)
  {
  t->bksize=(t->len+1)*2;
  t->buf=realloc(t->buf,t->bksize);
  }
 strcpy(t->buf,str);
 t->cur=t->len;
 redraw(t);
 }

static char *editgt(t)
Edit *t;
 {
 return t->buf;
 }

/* Calculate text offset and cursor position.  Returns true if text offset
   changed */

static int editpos(t)
Edit *t;
 {
 int txtwid=txtw(gtfgnd(t),t->buf);
 int winwid=gtwidth(t)-t->xpos*2;
 int ofst=t->ofst;
 char c;

 if(t->havefocus)
  {
  c=t->buf[t->cur]; t->buf[t->cur]=0;
  t->curpos=txtw(gtfgnd(t),t->buf);
  t->buf[t->cur]=c;
  if(t->curpos-ofst<0)
   ofst=t->curpos;
  else if(t->curpos-ofst>winwid)
   ofst=t->curpos-winwid;
  }
 else ofst=0;
 if(ofst!=t->ofst)
  {
  t->ofst=ofst;
  return 1;
  }
 else return 0;
 }

static int editdraw(t)
Edit *t;
 {
 int txtwid=txtw(gtfgnd(t),t->buf);
 GC gc=gtfgnd(t);
 int yy=t->ypos-txtb(gtfgnd(t),"M");
 int yyh=txth(gtfgnd(t),"M");

 editpos(t);

 if(t->sel!=t->to)
  {
  int c;
  int of, of1;

  /* Draw text before selected area */
  c=t->buf[t->sel]; t->buf[t->sel]=0;
  of=txtw(gc,t->buf);
  drawtxt(t,gc,t->xpos-t->ofst,t->ypos,t->buf);

  /* Draw text in selected area */
  t->buf[t->sel]=c; c=t->buf[t->to]; t->buf[t->to]=0;
  of1=txtw(gc,t->buf+t->sel);
  drawfilled(t,gc,t->xpos-t->ofst+of,yy,of1,yyh);
  drawtxt(t,getfidpixel(gcfid(gc),getcolor("white")),
          t->xpos-t->ofst+of,t->ypos,t->buf+t->sel);

  /* Draw text after selected area */
  of+=of1; t->buf[t->to]=c;
  drawtxt(t,gc,t->xpos-t->ofst+of,t->ypos,t->buf+t->to);
  }
 else
  drawtxt(t,gc,t->xpos-t->ofst,t->ypos,t->buf);

 if(t->havefocus==1)
  {
  drawline(t,gc,t->xpos+t->curpos-t->ofst,yy,
           t->xpos+t->curpos-t->ofst,yy+yyh);
  }

 return 0;
 }

/* Erase cursor */

static void editcuroff(t)
Edit *t;
 {
 int yy=t->ypos-txtb(gtfgnd(t),"M");
 drawline(t,gtbknd(t),t->xpos+t->curpos-t->ofst,yy,
                      t->xpos+t->curpos-t->ofst,yy+txth(gtfgnd(t),"M"));
 }

static void editdoblink(stat,t)
Edit *t;
 {
 if(!stat)
  {
  if(t->havefocus==2) t->havefocus=1;
  else if(t->havefocus==1) t->havefocus=2;
  if(gtflg(t)&flgon)
   {
   editcuroff(t); editdraw(t);
   }
  submit(650000,fn1(t->blink,editdoblink,t));
  }
 }

static int editfocus(t)
Edit *t;
 {
 (widgetfuncs.focus)(t);
 if(t->focusrtn) stfocusrtn(t->focusrtn);
 if(!t->havefocus)
  {
  t->havefocus=2;
  t->sel=t->start=0;
  t->to=t->cur=t->len;
  redraw(t);
  editdoblink(0,t);
  }
 return 0;
 }

static int findofst(t,x)
Edit *t;
 {
 int n=0;
 int ofst=t->xpos;
 if(ofst<x)
  for(n=0;t->buf[n];++n)
   {
   int wid;
   int c=t->buf[n+1]; t->buf[n+1]=0;
   wid=txtw(gtfgnd(t),t->buf+n);
   t->buf[n+1]=c;
   if(ofst+wid*3/4>x) break;
   else ofst+=wid;
   }
 return n;
 }

/* Update edit window.  Erase cursor, calculate new cursor and text offsets,
 * draw window and cursor, start cursor blink timer */

static void editupdate(t)
Edit *t;
 {
 cancel(t->blink);
 editcuroff(t);
 t->update|=editpos(t);
 t->havefocus=2;
 editdoblink(0,t);
 if(t->update) redraw(t), t->update=0;
 else editdraw(t);
 }

/* Insert */

static void editins(t,x,s,len)
Edit *t;
char *s;
 {
 if(t->len+len>t->bksize)
  {
  t->bksize=t->len+len+80;
  t->buf=realloc(t->buf,t->bksize);
  }
 mcpy(t->buf+x+len,t->buf+x,t->len-x+1);
 mcpy(t->buf+x,s,len);
 t->len+=len;

 /* Fixup pointers */
 if(t->cur>=x) t->cur+=len;
 if(t->start>=x) t->start+=len;
 if(t->sel>=x) t->sel+=len;
 if(t->to>=x) t->to+=len;

 t->update=1;
 }

/* Delete */

static void editdel(t,x,len)
Edit *t;
 {
 mcpy(t->buf+x,t->buf+x+len,t->len-(x+len)+1);
 t->len-=len;

 /* Fix-up pointers */
 if(t->cur>=x+len) t->cur-=len;
 else if(t->cur>=x) t->cur=x;

 if(t->start>=x+len) t->start-=len;
 else if(t->start>=x) t->start=x;
 
 if(t->sel>=x+len) t->sel-=len;
 else if(t->sel>=x) t->sel=x;

 if(t->to>=x+len) t->to-=len;
 else if(t->to>=x) t->to=x;

 t->update=1;
 }

static int editunfocus(t)
Edit *t;
 {
 (widgetfuncs.unfocus)(t);
 t->havefocus=0;
 t->editing=0;
 cancel(t->blink);
 redraw(t);
 return 0;
 }

static startediting(t)
Edit *t;
 {
 if(!t->editing) t->update=1;
 t->editing=1;
 }

/* Edit functions */

static int ubacks(stat,t,junk,key,state,x,y,org)
Edit *t;
 {
 if(!stat)
  {
  if(t->sel!= -1 && t->editing) editdel(t,t->sel,t->to-t->sel);
  else if(t->cur) editdel(t,t->cur-1,1);
  startediting(t);
  t->sel=t->to=t->start= -1;
  editupdate(t);
  }
 return stat;
 }

static killselection(t)
Edit *t;
 {
 if(t->sel!= -1)
  {
  t->start=t->sel=t->to= -1;
  t->update=1;
  }
 }

static int ubol(stat,t,junk,key,state,x,y,org)
Edit *t;
 {
 if(!stat)
  {
  killselection(t);
  startediting(t);
  t->cur=0;
  editupdate(t);
  }
 return stat;
 }

static int ueol(stat,t,junk,key,state,x,y,org)
Edit *t;
 {
 if(!stat)
  {
  killselection(t);
  startediting(t);
  t->cur=t->len;
  editupdate(t);
  }
 return stat;
 }

static int ultarw(stat,t,junk,key,state,x,y,org)
Edit *t;
 {
 if(!stat)
  {
  killselection(t);
  startediting(t);
  if(t->cur) --t->cur;
  editupdate(t);
  }
 return stat;
 }

static int urtarw(stat,t,junk,key,state,x,y,org)
Edit *t;
 {
 if(!stat)
  {
  killselection(t);
  startediting(t);
  if(t->cur!=t->len) ++t->cur;
  editupdate(t);
  }
 return stat;
 }

static int udelch(stat,t,junk,key,state,x,y,org)
Edit *t;
 {
 if(!stat)
  {
  killselection(t);
  startediting(t);
  if(t->cur!=t->len) editdel(t,t->cur,1);
  editupdate(t);
  }
 return stat;
 }

static int udeleol(stat,t,junk,key,state,x,y,org)
Edit *t;
 {
 if(!stat)
  {
  killselection(t);
  t->editing=1;
  if(t->len-t->cur) editdel(t,t->cur,t->len-t->cur);
  editupdate(t);
  }
 return stat;
 }

static int udellin(stat,t,junk,key,state,x,y,org)
Edit *t;
 {
 if(!stat)
  {
  killselection(t);
  t->editing=1;
  if(t->len) editdel(t,0,t->len);
  editupdate(t);
  }
 return stat;
 }

static int urelease(stat,t,junk,key,state,x,y,org)
Edit *t;
 {
 if(!stat)
  {
  if(t->sel!=t->to)
   {
   /* User has selected some text: stick it in the cut buffer and
    * tell the server we own the selection */
   XStoreBytes(dsp,t->buf+t->sel,t->to-t->sel);
   XSetSelectionOwner(dsp,XA_PRIMARY,gtwin(t),ev.xbutton.time);
   }
  }
 return stat;
 }

static int upress1(stat,t,junk,key,state,x,y,org)
Edit *t;
 {
 if(!stat)
  {
  killselection(t);
  if(!t->havefocus)
   { /* Get focus; start selecting */
   stfocus(t);
   }
  else
   { /* Position cursor and start selecting text */
   t->cur=t->start=t->sel=t->to=findofst(t,x+t->ofst);
   t->editing=1;
   t->update=1;
   }
  editupdate(t);
  }
 return stat;
 }

Atom ouratom;
int atomflg;

static int upress2(stat,t,junk,key,state,x,y,org)
Edit *t;
 {
 if(!stat)
  {
  char *s;
  int len;
  stfocus(t);
  t->editing=1;
  t->cur=findofst(t,x+t->ofst);
  if(!atomflg) ouratom=XInternAtom(dsp,"_NOTIF",False), atomflg=1;
  XConvertSelection(dsp,XA_PRIMARY,XA_STRING,ouratom,gtwin(t),ev.xbutton.time);
  }
 return stat;
 }

static int umotion(stat,t,junk,key,state,x,y,org)
Edit *t;
 {
 if(!stat)
  {
  if(t->start!=-1 && (state&MouseLeft))
   {
   int n=findofst(t,x+t->ofst);
   if(!t->editing)
    {
    t->sel=t->start=t->to=n;
    t->editing=1;
    redraw(t);
    }
   if(n>t->start)
    { /* Select to the right */
    if(n!=t->to || t->sel!=t->start)
     {
     t->editing=1;
     t->sel=t->start;
     t->to=n;
     t->cur=n;
     redraw(t);
     }
    }
   else
    { /* Select to the left */
    if(n!=t->sel || t->to!=t->start)
     {
     t->editing=1;
     t->sel=n;
     t->to=t->start;
     t->cur=t->start;
     redraw(t);
     }
    }
   }
  }
 return stat;
 }

static int utype(stat,t,junk,key,state,x,y,org)
Edit *t;
 {
 if(!stat)
  {
  char k=key;
  if(t->sel!=t->to && t->cur==t->to)
   { /* Kill selected text */
   editdel(t,t->sel,t->to-t->sel);
   }
  killselection(t);
  editins(t,t->cur,&k,1);
  editupdate(t);
  }
 return stat;
 }

static void urtn(stat,t,junk,key,state,x,y,org)
Edit *t;
 {
 if(!stat)
  {
  killselection(t);
  if(t->org) free(t->org);
  t->org=strcpy(malloc(t->len+1),t->buf);
  clfocus(t);
  cont0(t->fn); t->fn=0;
  }
 }

static int urestore(stat,t,junk,key,state,x,y,org)
Edit *t;
 {
 if(!stat)
  {
  killselection(t);
  t->editing=0;
  if(t->org)
   {
   strcpy(t->buf,t->org);
   t->cur=t->len=strlen(t->buf);
   }
  else
   {
   t->buf[0]=0;
   t->cur=t->len=0;
   }
  t->update=1;
  editupdate(t);
  }
 return stat;
 }

static void editstfn(t,c)
Edit *t;
TASK *c;
 {
 t->fn=c;
 }

static void editstrtn(t,w)
Edit *t;
Widget *w;
 {
 t->focusrtn=w;
 }

static int editgtheight(t)
Edit *t;
 {
 if(t->ypos== -1)
  {
  if(!ckh(t))
   stheight(t,txth(gtfgnd(t),"M"));
  t->xpos=txtw(gtfgnd(t),"M")/2+1;
  t->ypos=(widgetfuncs.gtheight)(t)/2-
          txth(gtfgnd(t),"M")/2+txtb(gtfgnd(t),"M");
  }
 return (widgetfuncs.gtheight)(t);
 }

static int editselectionclear(t,ev)
Edit *t;
XEvent *ev;
 {
 killselection(t);
 redraw(t);
 }

static int editselectionnotify(t,ev)
Edit *t;
XSelectionEvent *ev;
 {
 unsigned long len;
 Atom rtn;
 int fmt;
 unsigned long rest;
 unsigned char *s;
 XGetWindowProperty(dsp,gtwin(t),ouratom,0L,1024L,True,XA_STRING,
                    &rtn,&fmt,&len,&rest,&s);
 if(rtn!=None)
  { /* Use selection */
  int org=t->cur;
  editins(t,t->cur,s,(int)len);
  t->start=t->sel=org;
  t->to=t->sel+len;
  editupdate(t);
  XFree(s);
  XStoreBytes(dsp,t->buf+t->sel,t->to-t->sel);
  XSetSelectionOwner(dsp,XA_PRIMARY,gtwin(t),ev->time);
  }
 else
  { /* Use cut buffer */
  int len;
  int org=t->cur;
  s=XFetchBytes(dsp,&len);
  editins(t,t->cur,s,len);
  t->start=t->sel=org;
  t->to=t->sel+len;
  editupdate(t);
  XFree(s);
  XStoreBytes(dsp,t->buf+t->sel,t->to-t->sel);
  XSetSelectionOwner(dsp,XA_PRIMARY,gtwin(t),ev->time);
  }
 }

static int editselectionrequest(t,ev)
Edit *t;
XSelectionRequestEvent *ev;
 {
 XSelectionEvent se;
 /*
 printf("%s %s %s\n",XGetAtomName(dsp,ev->selection),
                     XGetAtomName(dsp,ev->target),
                     XGetAtomName(dsp,ev->property));
 */
 XChangeProperty(dsp,ev->requestor,ev->property,ev->target,
                 8,PropModeReplace,t->buf+t->sel,t->to-t->sel);
 
 se.type=SelectionNotify;
 se.requestor=ev->requestor;
 se.selection=ev->selection;
 se.target=ev->target;
 se.property=ev->property;
 se.time=ev->time;
 XSendEvent(dsp,se.requestor,True,0,(XEvent *)&se);
 }

struct editfuncs editfuncs;

Edit *mkEdit(t)
Edit *t;
 {
 mkWidget(t);
 if(!editfuncs.on)
  {
  mcpy(&editfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  editfuncs.st=editst;
  editfuncs.gt=editgt;
  editfuncs.expose=editdraw;
  editfuncs.unfocus=editunfocus;
  editfuncs.focus=editfocus;
  editfuncs.stfn=editstfn;
  editfuncs.strtn=editstrtn;
  editfuncs.gtheight=editgtheight;
  editfuncs.kmap=mkkmap(NULL,NULL,NULL);
  editfuncs.selectionclear=editselectionclear;
  editfuncs.selectionnotify=editselectionnotify;
  editfuncs.selectionrequest=editselectionrequest;
  editfuncs.rm=editrm;
  kcpy(editfuncs.kmap,widgetfuncs.kmap);
  kadd(editfuncs.kmap,NULL,"^H",ubacks,NULL);
  kadd(editfuncs.kmap,NULL,"^?",ubacks,NULL);
  kadd(editfuncs.kmap,NULL,"Release2",urelease,NULL);
  kadd(editfuncs.kmap,NULL,"Release1",urelease,NULL);
  kadd(editfuncs.kmap,NULL,"Press2",upress2,NULL);
  kadd(editfuncs.kmap,NULL,"Press1",upress1,NULL);
  kadd(editfuncs.kmap,NULL,"Motion",umotion,NULL);
  kadd(editfuncs.kmap,NULL,"^B",ultarw,NULL);
  kadd(editfuncs.kmap,NULL,"Left",ultarw,NULL);
  kadd(editfuncs.kmap,NULL,"^D",udelch,NULL);
  kadd(editfuncs.kmap,NULL,"^J",udeleol,NULL);
  kadd(editfuncs.kmap,NULL,"^Y",udellin,NULL);
  kadd(editfuncs.kmap,NULL,"^C",urestore,NULL);
  kadd(editfuncs.kmap,NULL,"^A",ubol,NULL);
  kadd(editfuncs.kmap,NULL,"Home",ubol,NULL);
  kadd(editfuncs.kmap,NULL,"Begin",ubol,NULL);
  kadd(editfuncs.kmap,NULL,"^E",ueol,NULL);
  kadd(editfuncs.kmap,NULL,"End",ueol,NULL);
  kadd(editfuncs.kmap,NULL,"^F",urtarw,NULL);
  kadd(editfuncs.kmap,NULL,"Right",urtarw,NULL);
  kadd(editfuncs.kmap,NULL,"SP TO ~",utype,NULL);
  kadd(editfuncs.kmap,NULL,"^M",urtn,NULL);
  }
 t->funcs= &editfuncs;
 stkmap(t,editfuncs.kmap);
 stborder(t,mk(Eborder));
 stwidth(t,250);
 iztask(t->blink);
 t->ypos= -1;
 t->havefocus=0;
 t->bksize=16;
 t->buf=malloc(t->bksize);
 t->len=0;
 t->cur=0;
 t->sel=t->to= -1;
 t->ofst=0;
 t->fn=0;
 t->buf[0]=0;
 t->editing=0;
 t->org=0;
 t->focusrtn=0;
 stflg(t,gtflg(t)|flgfocus);
 return t;
 }
