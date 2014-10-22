/* Job control widget */

#include <notif.h>
#include "sdialog.h"

static int sctrldraw(b)
Sctrl *b;
 {
 char buf[1024];
 int wid;
 drawclr(b);
 sprintf(buf,"%d ",b->name+1);
 wid=txtw(gtfgnd(b),buf);
 drawtxt(b,gtfgnd(b),4,30,buf);
 if(scheds[b->name].valid)
  {
  int hour=scheds[b->name].hour;
  if(hour>=0 && hour<12)
   {
   if(!hour) hour=12;
   sprintf(buf,"Time=%d:%2.2d:%2.2d AM",
           hour,scheds[b->name].min,scheds[b->name].sec);
   }
  else
   {
   hour-=12;
   if(!hour) hour=12;
   sprintf(buf,"Time=%d:%2.2d:%2.2d PM",
           hour,scheds[b->name].min,scheds[b->name].sec);
   }
  drawtxt(b,gtfgnd(b),4+wid,15,buf);
  sprintf(buf,"Program=%d",scheds[b->name].program);
  drawtxt(b,gtfgnd(b),4+wid,40,buf);
  }
 return 0;
 }

static int sctrluser(b,key,state,x,y)
Sctrl *b;
 {
 if(!(gtflg(b)&flgactivated)) return 1;
 if(key==XK_Press1)
  {
  Sdialog *p=mk(Sdialog);
   st(p,b);
   placedialog(b,p);
  add(root,p);
  }
 return 1;
 }

static void sctrlstfn(b,c)
Sctrl *b;
TASK *c;
 {
 b->fn=c;
 }

static void sctrlsttext(b,n)
Sctrl *b;
 {
 b->name=n;
 }

static void sctrlst(b,n)
Sctrl *b;
 {
 redraw(b);
 }

static void sctrlrm(b)
Sctrl *b;
 {
 cancel(b->fn); b->fn=0;
 (widgetfuncs.rm)(b);
 }

struct sctrlfuncs sctrlfuncs;

Sctrl *mkSctrl(b)
Sctrl *b;
 {
 mkWidget(b);
 if(!sctrlfuncs.on)
  {
  mcpy(&sctrlfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  sctrlfuncs.user=sctrluser;
  sctrlfuncs.stfn=sctrlstfn;
  sctrlfuncs.sttext=sctrlsttext;
  sctrlfuncs.st=sctrlst;
  sctrlfuncs.expose=sctrldraw;
  sctrlfuncs.rm=sctrlrm;
  }
 b->funcs= &sctrlfuncs;
 b->fn=0;
 b->name=0;
 stborder(b,mkIkborder(malloc(sizeof(Iborder))));
 stw(b,156);
 sth(b,53);
 return b;
 }
