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

void Edit::st(char *str)
 {
 len=strlen(str);
 if(org) free(org);
 org=strdup(str);
 if(len+1>bksize)
  {
  bksize=(len+1)*2;
  buf=(char *)realloc(buf,bksize);
  }
 strcpy(buf,str);
 cur=len;
 redraw(this);
 }

char *Edit::gt()
 {
 return buf;
 }

/* Calculate text offset and cursor position.  Returns true if text offset
   changed */

int Edit::pos()
 {
 int txtwid=txtw(gtfgnd(),buf);
 int winwid=gtwidth()-xpos*2;
 int tofst=ofst;
 char c;

 if(havefocus)
  {
  c=buf[cur]; buf[cur]=0;
  curpos=txtw(gtfgnd(),buf);
  buf[cur]=c;
  if(curpos-tofst<0)
   tofst=curpos;
  else if(curpos-tofst>winwid)
   tofst=curpos-winwid;
  }
 else tofst=0;
 if(tofst!=ofst)
  {
  ofst=tofst;
  return 1;
  }
 else return 0;
 }

int Edit::expose(XEvent *ev)
 {
 int txtwid=txtw(gtfgnd(),buf);
 GC gc=gtfgnd();
 int yy=ypos-txtb(gtfgnd());
 int yyh=txth(gtfgnd());

 pos();

 if(sel!=to)
  {
  int c;
  int of, of1;

  /* Draw text before selected area */
  c=buf[sel]; buf[sel]=0;
  of=txtw(gc,buf);
  drawtxt(this,gc,xpos-ofst,ypos,buf);

  /* Draw text in selected area */
  buf[sel]=c; c=buf[to]; buf[to]=0;
  of1=txtw(gc,buf+sel);
  drawfilled(this,gc,xpos-ofst+of,yy,of1,yyh);
  drawtxt(this,getfidpixel(gcfid(gc),getcolor("white")),
          xpos-ofst+of,ypos,buf+sel);

  /* Draw text after selected area */
  of+=of1; buf[to]=c;
  drawtxt(this,gc,xpos-ofst+of,ypos,buf+to);
  }
 else
  drawtxt(this,gc,xpos-ofst,ypos,buf);

 if(havefocus==1)
  {
  drawline(this,gc,xpos+curpos-ofst,yy,
           xpos+curpos-ofst,yy+yyh);
  }

 return 0;
 }

/* Erase cursor */

void Edit::curoff()
 {
 int yy=ypos-txtb(gtfgnd());
 drawline(this,gtbknd(),xpos+curpos-ofst,yy,
                        xpos+curpos-ofst,yy+txth(gtfgnd()));
 }

static void editdoblink(Edit *t)
  {
  if(t->havefocus==2) t->havefocus=1;
  else if(t->havefocus==1) t->havefocus=2;
  if (t->on_flag)
    {
    t->curoff();
    t->expose(NULL);
    // FIXME: redraw(this)?
    }
  submit(650000,t->blink=new Fn1_0<void,Edit *>(editdoblink,t));
  }

int Edit::focus()
  {
  Widget::focus();
  if(focusrtn) stfocusrtn(focusrtn);
  if(!havefocus)
    {
    havefocus=2;
    sel=start=0;
    to=cur=len;
    redraw(this);
    editdoblink(this);
    }
  return 0;
  }

int Edit::findofst(int x)
 {
 int n=0;
 int ofst=xpos;
 if(ofst<x)
  for(n=0;buf[n];++n)
   {
   int wid;
   int c=buf[n+1]; buf[n+1]=0;
   wid=txtw(gtfgnd(),buf+n);
   buf[n+1]=c;
   if(ofst+wid*3/4>x) break;
   else ofst+=wid;
   }
 return n;
 }

/* Update edit window.  Erase cursor, calculate new cursor and text offsets,
 * draw window and cursor, start cursor blink timer */

void Edit::update()
 {
 if(blink) cancel(blink);
 curoff();
 update_flag|=pos();
 havefocus=2;
 editdoblink(this);
 if(update_flag) redraw(this), (update_flag = 0);
 else expose(NULL);
 }

/* Insert */

void Edit::ins(int x,char *s,int s_len)
 {
 if(len+s_len>bksize)
  {
  bksize=len+s_len+80;
  buf=(char *)realloc(buf,bksize);
  }
 memmove(buf+x+s_len,buf+x,len-x+1);
 memcpy(buf+x,s,s_len);
 len+=s_len;

 /* Fixup pointers */
 if(cur>=x) cur+=s_len;
 if(start>=x) start+=s_len;
 if(sel>=x) sel+=s_len;
 if(to>=x) to+=s_len;

 update_flag=1;
 }

/* Delete */

void Edit::del(int x,int s_len)
 {
 memcpy(buf+x,buf+x+s_len,len-(x+s_len)+1);
 len-=s_len;

 /* Fix-up pointers */
 if(cur>=x+s_len) cur-=s_len;
 else if(cur>=x) cur=x;

 if(start>=x+s_len) start-=s_len;
 else if(start>=x) start=x;
 
 if(sel>=x+s_len) sel-=s_len;
 else if(sel>=x) sel=x;

 if(to>=x+s_len) to-=s_len;
 else if(to>=x) to=x;

 update_flag=1;
 }

int Edit::unfocus()
 {
 Widget::unfocus();
 havefocus=0;
 editing=0;
 if(blink) cancel(blink), (blink=0);
 redraw(this);
 return 0;
 }

void Edit::startediting()
 {
 if(!editing) update_flag=1;
 editing=1;
 }

/* Edit functions */

int Edit::ubacks(int key,int state,int x,int y,Time time,Widget *org)
 {
 if(sel!= -1 && editing) del(sel,to-sel);
 else if(cur) del(cur-1,1);
 startediting();
 sel=to=start= -1;
 update();
 return 0;
 }

void Edit::killselection()
 {
 if(sel!= -1)
  {
  start=sel=to= -1;
  update_flag=1;
  }
 }

int Edit::ubol(int key,int state,int x,int y,Time time,Widget *org)
 {
 killselection();
 startediting();
 cur=0;
 update();
 return 0;
 }

int Edit::ueol(int key,int state,int x,int y,Time time,Widget *org)
 {
 killselection();
 startediting();
 cur=len;
 update();
 return 0;
 }

int Edit::ultarw(int key,int state,int x,int y,Time time,Widget *org)
 {
 killselection();
 startediting();
 if(cur) --cur;
 update();
 return 0;
 }

int Edit::urtarw(int key,int state,int x,int y,Time time,Widget *org)
 {
 killselection();
 startediting();
 if(cur!=len) ++cur;
 update();
 return 0;
 }

int Edit::udelch(int key,int state,int x,int y,Time time,Widget *org)
 {
  killselection();
  startediting();
  if(cur!=len) del(cur,1);
  update();
 return 0;
 }

int Edit::udeleol(int key,int state,int x,int y,Time time,Widget *org)
 {
  killselection();
  editing=1;
  if(len-cur) del(cur,len-cur);
  update();
 return 0;
 }

int Edit::udellin(int key,int state,int x,int y,Time time,Widget *org)
 {
  killselection();
  editing=1;
  if(len) del(0,len);
  update();
  return 0;
 }

int Edit::urelease(int key,int state,int x,int y,Time time,Widget *org)
 {
  if(sel!=to)
   {
   /* User has selected some text: stick it in the cut buffer and
    * tell the server we own the selection */
   XStoreBytes(dsp,buf+sel,to-sel);
   XSetSelectionOwner(dsp,XA_PRIMARY,gtwin(),time);
   }
 return 0;
 }

int Edit::upress1(int key,int state,int x,int y,Time time,Widget *org)
 {
 killselection();
  if(!havefocus)
   { /* Get focus; start selecting */
   stfocus(this);
   }
  else
   { /* Position cursor and start selecting text */
   cur=start=sel=to=findofst(x+ofst);
   editing=1;
   update_flag=1;
   }
  update();
 return 0;
 }

Atom ouratom;
int atomflg;

int Edit::upress2(int key,int state,int x,int y,Time time,Widget *org)
 {
  char *s;
  int len;
  stfocus(this);
  editing=1;
  cur=findofst(x+ofst);
  if(!atomflg) ouratom=XInternAtom(dsp,"_NOTIF",False), atomflg=1;
  XConvertSelection(dsp,XA_PRIMARY,XA_STRING,ouratom,gtwin(),time);
 return 0;
 }

int Edit::umotion(int key,int state,int x,int y,Time time,Widget *org)
 {
  if(start!=-1 && (state&MouseLeft))
   {
   int n=findofst(x+ofst);
   if(!editing)
    {
    sel=start=to=n;
    editing=1;
    redraw(this);
    }
   if(n>start)
    { /* Select to the right */
    if(n!=to || sel!=start)
     {
     editing=1;
     sel=start;
     to=n;
     cur=n;
     redraw(this);
     }
    }
   else
    { /* Select to the left */
    if(n!=sel || to!=start)
     {
     editing=1;
     sel=n;
     to=start;
     cur=start;
     redraw(this);
     }
    }
   }
 return 0;
 }

int Edit::utype(int key,int state,int x,int y,Time time,Widget *org)
 {
  char k=key;
  if(sel!=to && cur==to)
   { /* Kill selected text */
   del(sel,to-sel);
   }
  killselection();
  ins(cur,&k,1);
  update();
 return 0;
 }

int Edit::urtn(int key,int state,int x,int y,Time time,Widget *orgw)
 {
 killselection();
 if(org) free(org);
 org=strdup(buf);
 clfocus(this);
 if (fn) fn->cont(fn);
 return 0;
 }

int Edit::urestore(int key,int state,int x,int y,Time time,Widget *orgw)
 {
  killselection();
  editing=0;
  if(org)
   {
   strcpy(buf,org);
   cur=len=strlen(buf);
   }
  else
   {
   buf[0]=0;
   cur=len=0;
   }
  update_flag=1;
  update();
 return 0;
 }

void Edit::stfn(Fn_0<void> *c)
 {
 fn=c;
 }

void Edit::strtn(Widget *w)
 {
 focusrtn=w;
 }

int Edit::gtheight()
 {
 if(ypos== -1)
  {
  if(!ckh())
   stheight(txth(gtfgnd()));
  xpos=txtw(gtfgnd(),"M")/2+1;
  ypos=Widget::gtheight()/2-txth(gtfgnd())/2+txtb(gtfgnd());
  }
 return Widget::gtheight();
 }

int Edit::selectionclear(XEvent *ev)
 {
 killselection();
 redraw(this);
 }

int Edit::selectionnotify(XEvent *evv)
 {
 XSelectionEvent *ev = (XSelectionEvent *)evv;
 unsigned long len;
 Atom rtn;
 int fmt;
 unsigned long rest;
 unsigned char *s;
 XGetWindowProperty(dsp,gtwin(),ouratom,0L,1024L,True,XA_STRING,&rtn,&fmt,&len,&rest,&s);
 if(rtn!=None)
  { /* Use selection */
  int orgcur=cur;
  ins(cur,(char *)s,(int)len);
  start=sel=orgcur;
  to=sel+len;
  update();
  XFree(s);
  XStoreBytes(dsp,buf+sel,to-sel);
  XSetSelectionOwner(dsp,XA_PRIMARY,gtwin(),ev->time);
  }
 else
  { /* Use cut buffer */
  int len;
  int orgcur=cur;
  s=(unsigned char *)XFetchBytes(dsp,&len);
  ins(cur,(char *)s,(int)len);
  start=sel=orgcur;
  to=sel+len;
  update();
  XFree(s);
  XStoreBytes(dsp,buf+sel,to-sel);
  XSetSelectionOwner(dsp,XA_PRIMARY,gtwin(),ev->time);
  }
 return 0;
 }

int Edit::selectionrequest(XEvent *evv)
 {
 XSelectionRequestEvent *ev=(XSelectionRequestEvent *)evv;
 XSelectionEvent se;
 /*
 printf("%s %s %s\n",XGetAtomName(dsp,ev->selection),
                     XGetAtomName(dsp,ev->target),
                     XGetAtomName(dsp,ev->property));
 */
 XChangeProperty(dsp,ev->requestor,ev->property,ev->target,
                 8,PropModeReplace,(unsigned char *)buf+sel,to-sel);
 
 se.type=SelectionNotify;
 se.requestor=ev->requestor;
 se.selection=ev->selection;
 se.target=ev->target;
 se.property=ev->property;
 se.time=ev->time;
 XSendEvent(dsp,se.requestor,True,0,(XEvent *)&se);
 }

Kmap<Key> editkmap;
int editkmap_flag;

Edit::Edit()
 {
 // Build edit widget keymap
 if(!editkmap_flag)
  {
  editkmap_flag = 1;
  editkmap.merge(stdkmap);
  editkmap.add("^H",new Mfn0_6<int,Edit,int,int,int,int,Time,Widget *>(&Edit::ubacks,NULL));
  editkmap.add("^?",new Mfn0_6<int,Edit,int,int,int,int,Time,Widget *>(&Edit::ubacks,NULL));
  editkmap.add("Release2",new Mfn0_6<int,Edit,int,int,int,int,Time,Widget *>(&Edit::urelease,NULL));
  editkmap.add("Release1",new Mfn0_6<int,Edit,int,int,int,int,Time,Widget *>(&Edit::urelease,NULL));
  editkmap.add("Press2",new Mfn0_6<int,Edit,int,int,int,int,Time,Widget *>(&Edit::upress2,NULL));
  editkmap.add("Press1",new Mfn0_6<int,Edit,int,int,int,int,Time,Widget *>(&Edit::upress1,NULL));
  editkmap.add("Motion",new Mfn0_6<int,Edit,int,int,int,int,Time,Widget *>(&Edit::umotion,NULL));
  editkmap.add("^B",new Mfn0_6<int,Edit,int,int,int,int,Time,Widget *>(&Edit::ultarw,NULL));
  editkmap.add("Left",new Mfn0_6<int,Edit,int,int,int,int,Time,Widget *>(&Edit::ultarw,NULL));
  editkmap.add("^D",new Mfn0_6<int,Edit,int,int,int,int,Time,Widget *>(&Edit::udelch,NULL));
  editkmap.add("^J",new Mfn0_6<int,Edit,int,int,int,int,Time,Widget *>(&Edit::udeleol,NULL));
  editkmap.add("^Y",new Mfn0_6<int,Edit,int,int,int,int,Time,Widget *>(&Edit::udellin,NULL));
  editkmap.add("^C",new Mfn0_6<int,Edit,int,int,int,int,Time,Widget *>(&Edit::urestore,NULL));
  editkmap.add("^A",new Mfn0_6<int,Edit,int,int,int,int,Time,Widget *>(&Edit::ubol,NULL));
  editkmap.add("Home",new Mfn0_6<int,Edit,int,int,int,int,Time,Widget *>(&Edit::ubol,NULL));
  editkmap.add("Begin",new Mfn0_6<int,Edit,int,int,int,int,Time,Widget *>(&Edit::ubol,NULL));
  editkmap.add("^E",new Mfn0_6<int,Edit,int,int,int,int,Time,Widget *>(&Edit::ueol,NULL));
  editkmap.add("End",new Mfn0_6<int,Edit,int,int,int,int,Time,Widget *>(&Edit::ueol,NULL));
  editkmap.add("^F",new Mfn0_6<int,Edit,int,int,int,int,Time,Widget *>(&Edit::urtarw,NULL));
  editkmap.add("Right",new Mfn0_6<int,Edit,int,int,int,int,Time,Widget *>(&Edit::urtarw,NULL));
  editkmap.add("SP TO ~",new Mfn0_6<int,Edit,int,int,int,int,Time,Widget *>(&Edit::utype,NULL));
  editkmap.add("^M",new Mfn0_6<int,Edit,int,int,int,int,Time,Widget *>(&Edit::urtn,NULL));
  }
 stkmap(&editkmap);
 stborder(new Eborder());
 stwidth(250);
 ypos= -1;
 havefocus=0;
 bksize=16;
 buf=(char *)malloc(bksize);
 len=0;
 cur=0;
 sel=to= -1;
 ofst=0;
 fn=0;
 buf[0]=0;
 editing=0;
 org=0;
 blink=0;
 focusrtn=0;
 stfocusable();
 }

Edit::~Edit()
  {
  free(buf);
  if (org) free(org);
  }
