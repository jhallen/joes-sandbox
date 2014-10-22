/* Program widget */

#include <notif.h>
#include "pdialog.h"


static int pctrldraw(b)
Pctrl *b;
 {
 char buf[1024];
 char buf1[1024];
 int wid;
 drawclr(b);
 sprintf(buf,"%d ",b->name+1);
 wid=txtw(gtfgnd(b),buf);
 if(programs[b->name].valid)
  {
  sprintf(buf+strlen(buf),"Duration=%d Repeat=%d Cycle=%d",programs[b->name].dwell,
          programs[b->name].repeat,programs[b->name].cycle);
  listem(buf1,programs[b->name].valves);
  drawtxt(b,gtfgnd(b),wid+10,40,buf1);
  }
 drawtxt(b,gtfgnd(b),10,15,buf);
 return 0;
 }

static int pctrluser(b,key,state,x,y)
Pctrl *b;
 {
 if(!(gtflg(b)&flgactivated)) return 1;
 if(key==XK_Press1)
  {
  Pdialog *p=mk(Pdialog);
   st(p,b);
   stx(p,0);
   sty(p,0);
  add(root,p);
  }
 return 1;
 }

static void pctrlstfn(b,c)
Pctrl *b;
TASK *c;
 {
 b->fn=c;
 }

static void pctrlsttext(b,n)
Pctrl *b;
 {
 b->name=n;
 }

static void pctrlst(b,n)
Pctrl *b;
 {
 redraw(b);
 }

static void pctrlrm(b)
Pctrl *b;
 {
 cancel(b->fn); b->fn=0;
 (widgetfuncs.rm)(b);
 }

struct pctrlfuncs pctrlfuncs;

Pctrl *mkPctrl(b)
Pctrl *b;
 {
 mkWidget(b);
 if(!pctrlfuncs.on)
  {
  mcpy(&pctrlfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  pctrlfuncs.user=pctrluser;
  pctrlfuncs.stfn=pctrlstfn;
  pctrlfuncs.sttext=pctrlsttext;
  pctrlfuncs.st=pctrlst;
  pctrlfuncs.expose=pctrldraw;
  pctrlfuncs.rm=pctrlrm;
  }
 b->funcs= &pctrlfuncs;
 b->fn=0;
 b->name=0;
 stborder(b,mkIkborder(malloc(sizeof(Iborder))));
 stw(b,312);
 sth(b,53);
 return b;
 }
