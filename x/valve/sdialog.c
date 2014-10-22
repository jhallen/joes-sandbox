/* Schedule widget */

#include <notif.h>
#include "timefield.h"
#include "sdialog.h"

static void sdialogst(b,n)
Sdialog *b;
Sctrl *n;
 {
 b->sctrl=n;
 if(scheds[n->name].valid)
  {
  char buf[10];
  if(scheds[n->name].hour>=12)
   {
   st(b->when->ampm,"PM");
   b->when->ampmflg=1;
   sprintf(buf,"%2.2d",(scheds[n->name].hour-12)==0?12:(scheds[n->name].hour-12));
   stfield(b->when->tedit,0,buf); 
   }
  else
   {
   sprintf(buf,"%2.2d",scheds[n->name].hour==0?12:scheds[n->name].hour);
   stfield(b->when->tedit,0,buf); 
   }
  sprintf(buf,"%2.2d",scheds[n->name].min);
  stfield(b->when->tedit,2,buf); 
  sprintf(buf,"%2.2d",scheds[n->name].sec);
  stfield(b->when->tedit,4,buf); 
  sprintf(buf,"%d",scheds[n->name].program);
  stfield(b->program->tedit,0,buf);
  }
 }

struct sdialogfuncs sdialogfuncs;

static int dook(stat,b)
Sdialog *b;
 {
 if(!stat)
  {
  int hr=atoi(gt(b->when->tedit)[0]);
  if(hr==12) hr-=12;
  if(b->when->ampmflg) hr+=12;
  scheds[b->sctrl->name].hour=hr;
  scheds[b->sctrl->name].min=atoi(gt(b->when->tedit)[2]);
  scheds[b->sctrl->name].sec=atoi(gt(b->when->tedit)[4]);
  scheds[b->sctrl->name].program=atoi(gt(b->program->tedit)[0]);
  if(scheds[b->sctrl->name].program) scheds[b->sctrl->name].valid=1;
  redraw(b->sctrl);
  rm(b);
  }
 }

static int doclr(stat,b)
Sdialog *b;
 {
 if(!stat)
  {
  scheds[b->sctrl->name].valid=0;
  redraw(b->sctrl);
  rm(b);
  }
 }

static int docan(stat,b)
Sdialog *b;
 {
 if(!stat)
  {
  rm(b);
  }
 }

static int udialogpress1(stat,m,junk,key,state,x,y,org)
Sdialog *m;
 {
 if(!stat)
  {
  if(x<0 || x>=gtwidth(m) || y<0 || y>=gtheight(m))
   { /* Mouse press is out of our window */
   }
  else
   { /* Mouse press is in our window */
   calcpointer();
   /* Event is for one of our kids maybe */
   doevent(gtmain(m),&ev);
   }
  }
 return stat;
 }
 
void sdialogrm(b)
Sdialog *b;
 {
 modalpop();
 (widgetfuncs.rm)(b);
 }

Sdialog *mkSdialog(b)
Sdialog *b;
 {
 Text *t;
 mkWidget(b);
 if(!sdialogfuncs.on)
  {
  mcpy(&sdialogfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  sdialogfuncs.st=sdialogst;
  sdialogfuncs.rm=sdialogrm;
  kcpy(sdialogfuncs.kmap,widgetfuncs.kmap);
  kadd(sdialogfuncs.kmap,NULL,"Press1",udialogpress1,NULL);
  }
 b->funcs= &sdialogfuncs;
 modal(b);
 stflg(b,gtflg(b)|flgoverride);
 stborder(b,mk(Titlebar));
  st((Titlebar *)gtborder(b),"Schedule a program");
 
 t=mk(Text);
  st(t,"Time to execute program:");
 add(b,t);

 ttob(b,t,0);
 ltor(b,NULL,0);

 ledge(b,20);
 b->sctrl=0;
 b->when=mk(Timefield);
 add(b,b->when);

 ledge(b,0);
 ttob(b,b->when,10);

 t=mk(Text);
  st(t,"Program No.:");
 add(b,t);

 ttob(b,t,0);

 ledge(b,20);
 b->program=mk(Nkpfield);
  st(b->program->tedit,"{d 2 1 16}");
 add(b,b->program);

 ltor(b,NULL,0);
 ttob(b,b->program,10);
 b->can=mk(Button);
  stborder(b->can,mkIkborder(malloc(sizeof(Iborder))));
  stw(b->can,70);
  sth(b->can,42);
  sttext(b->can,"Cancel");
  stfn(b->can,fn1(b->canfn,docan,b));
 add(b,b->can);
 tsame(b,b->can,0);
 b->clr=mk(Button);
  stborder(b->clr,mkIkborder(malloc(sizeof(Iborder))));
  stw(b->clr,70);
  sth(b->clr,42);
  sttext(b->clr,"Clear");
  stfn(b->clr,fn1(b->clrfn,doclr,b));
 add(b,b->clr);
 b->ok=mk(Button);
  stborder(b->ok,mkIkborder(malloc(sizeof(Iborder))));
  stw(b->ok,70);
  sth(b->ok,42);
  sttext(b->ok,"  OK  ");
  stfn(b->ok,fn1(b->okfn,dook,b));
 add(b,b->ok);
 return b;
 }
