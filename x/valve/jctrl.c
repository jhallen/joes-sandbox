/* Job control widget */

#include <notif.h>
#include "jdialog.h"

static int jctrldraw(b)
Jctrl *b;
 {
 char buf[1024];
 int wid;
 drawclr(b);
 sprintf(buf,"%d ",b->name+1);
 wid=txtw(gtfgnd(b),buf);
 drawtxt(b,gtfgnd(b),4,30,buf);
 if(jobs[b->name].valid)
  {
  int hour=jobs[b->name].hour;
  if(hour>=0 && hour<12)
   {
   if(!hour) hour=12;
   sprintf(buf,"Time=%d:%2.2d:%2.2d AM",
           hour,jobs[b->name].min,jobs[b->name].sec);
   }
  else
   {
   hour-=12;
   if(!hour) hour=12;
   sprintf(buf,"Time=%d:%2.2d:%2.2d PM",
           hour,jobs[b->name].min,jobs[b->name].sec);
   }
  drawtxt(b,gtfgnd(b),4+wid,15,buf);
  sprintf(buf,"Program=%d",jobs[b->name].program);
  drawtxt(b,gtfgnd(b),4+wid,40,buf);
  }
 }

static int jctrluser(b,key,state,x,y)
Jctrl *b;
 {
 if(!(gtflg(b)&flgactivated)) return 1;
 if(key==XK_Press1)
  {
  Jdialog *p=mk(Jdialog);
   st(p,b);
   placedialog(b,p);
  add(root,p);
  }
 return 1;
 }

static void jctrlstfn(b,c)
Jctrl *b;
TASK *c;
 {
 b->fn=c;
 }

static void jctrlsttext(b,n)
Jctrl *b;
 {
 b->name=n;
 }

static void jctrlst(b,n)
Jctrl *b;
 {
 redraw(b);
 }

static void jctrlrm(b)
Jctrl *b;
 {
 cancel(b->fn); b->fn=0;
 (widgetfuncs.rm)(b);
 }

struct jctrlfuncs jctrlfuncs;

Jctrl *mkJctrl(b)
Jctrl *b;
 {
 mkWidget(b);
 if(!jctrlfuncs.on)
  {
  mcpy(&jctrlfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  jctrlfuncs.user=jctrluser;
  jctrlfuncs.stfn=jctrlstfn;
  jctrlfuncs.sttext=jctrlsttext;
  jctrlfuncs.st=jctrlst;
  jctrlfuncs.expose=jctrldraw;
  jctrlfuncs.rm=jctrlrm;
  }
 b->funcs= &jctrlfuncs;
 b->fn=0;
 b->name=0;
 stborder(b,mkIkborder(malloc(sizeof(Iborder))));
 stw(b,156);
 sth(b,53);
 return b;
 }
