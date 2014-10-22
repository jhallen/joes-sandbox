/* Job control widget */

#include <notif.h>
#include "pdialog.h"

static void pdialogst(b,n)
Pdialog *b;
Pctrl *n;
 {
 b->pctrl=n;
 if(programs[n->name].valid)
  {
  char buf[10];
  st(b->selset,programs[n->name].valves);

  sprintf(buf,"%d",programs[n->name].dwell/3600);
  stfield(b->dwell->tedit,0,buf);
  sprintf(buf,"%d",programs[n->name].dwell%3600/60);
  stfield(b->dwell->tedit,2,buf);
  sprintf(buf,"%d",programs[n->name].dwell%60);
  stfield(b->dwell->tedit,4,buf);

  sprintf(buf,"%d",programs[n->name].cycle/3600);
  stfield(b->cycle->tedit,0,buf);
  sprintf(buf,"%d",programs[n->name].cycle%3600/60);
  stfield(b->cycle->tedit,2,buf);
  sprintf(buf,"%d",programs[n->name].cycle%60);
  stfield(b->cycle->tedit,4,buf);

  sprintf(buf,"%d",programs[n->name].repeat/3600);
  stfield(b->repeat->tedit,0,buf);
  sprintf(buf,"%d",programs[n->name].repeat%3600/60);
  stfield(b->repeat->tedit,2,buf);
  sprintf(buf,"%d",programs[n->name].repeat%60);
  stfield(b->repeat->tedit,4,buf);
  }
 }

struct pdialogfuncs pdialogfuncs;

static int dook(stat,b)
Pdialog *b;
 {
 if(!stat)
  {
  int x;
  for(x=0;x!=64;++x)
   programs[b->pctrl->name].valves[x]=b->selset->set[x];
  programs[b->pctrl->name].dwell=atoi(gt(b->dwell->tedit)[0])*3600+
                                 atoi(gt(b->dwell->tedit)[2])*60+
                                 atoi(gt(b->dwell->tedit)[4]);
  programs[b->pctrl->name].cycle=atoi(gt(b->cycle->tedit)[0])*3600+
                                 atoi(gt(b->cycle->tedit)[2])*60+
                                 atoi(gt(b->cycle->tedit)[4]);
  programs[b->pctrl->name].repeat=atoi(gt(b->repeat->tedit)[0])*3600+
                                  atoi(gt(b->repeat->tedit)[2])*60+
                                  atoi(gt(b->repeat->tedit)[4]);
  programs[b->pctrl->name].valid=1;
  redraw(b->pctrl);
  rm(b);
  }
 }

static int doclr(stat,b)
Pdialog *b;
 {
 if(!stat)
  {
  int n=b->pctrl->name;
  dook(stat,b);
  doit(n);
  }
 }

static int docan(stat,b)
Pdialog *b;
 {
 if(!stat)
  {
  rm(b);
  }
 }

static int udialogpress1(stat,m,junk,key,state,x,y,org)
Pdialog *m;
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

void pdialogrm(b)
Pdialog *b;
 {
 modalpop();
 (widgetfuncs.rm)(b);
 }

Pdialog *mkPdialog(b)
Pdialog *b;
 {
 Text *t;
 mkWidget(b);
 if(!pdialogfuncs.on)
  {
  mcpy(&pdialogfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  pdialogfuncs.st=pdialogst;
  pdialogfuncs.rm=pdialogrm;
  kcpy(pdialogfuncs.kmap,widgetfuncs.kmap);
  kadd(pdialogfuncs.kmap,NULL,"Press1",udialogpress1,NULL);
  }
 b->funcs= &pdialogfuncs;
 modal(b);
 stflg(b,gtflg(b)|flgoverride);
 stborder(b,mk(Titlebar));
  st((Titlebar *)gtborder(b),"Define program");
 
 ttob(b,NULL,0);
 ledge(b,NULL,0);

 t=mk(Text);
  st(t,"Watering duration for");
 add(b,t);

 t=mk(Text);
  st(t,"each selected valve in");
 add(b,t);

 t=mk(Text);
  st(t,"hours:mins.secs:");
 add(b,t);

 b->pctrl=0;
 ledge(b,20);
 b->dwell=mk(Nkpfield);
  b->dwell->tedit->pushmode=1;
  st(b->dwell->tedit,"{d 2 0 23}:{d 2 0 59}.{d 2 0 59}");
 add(b,b->dwell);

 ledge(b,0);
 ttob(b,b->dwell,20);

 t=mk(Text);
  st(t,"Total time to continually");
 add(b,t);

 ttob(b,t,0);

 t=mk(Text);
  st(t,"repeat watering cycle in");
 add(b,t);

 t=mk(Text);
  st(t,"hours:mins.secs.  Leave");
 add(b,t);

 t=mk(Text);
  st(t,"blank to water each");
 add(b,t);

 t=mk(Text);
  st(t,"valve just once:");
 add(b,t);


 ledge(b,20);
 b->repeat=mk(Nkpfield);
  b->repeat->tedit->pushmode=1;
  st(b->repeat->tedit,"{d 2 0 23}:{d 2 0 59}.{d 2 0 59}");
 add(b,b->repeat);

 ttob(b,b->repeat,20);
 ledge(b,0);

 t=mk(Text);
  st(t,"Time between each cycle");
 add(b,t);

 ttob(b,t,0);

 t=mk(Text);
  st(t,"in hours:mins.secs:");
 add(b,t);

 ledge(b,20);
 b->cycle=mk(Nkpfield);
  b->cycle->tedit->pushmode=1;
  st(b->cycle->tedit,"{d 2 0 23}:{d 2 0 59}.{d 2 0 59}");
 add(b,b->cycle);

 ltor(b,NULL,0);
 bedge(b,0);
 b->can=mk(Button);
  stborder(b->can,mkIkborder(malloc(sizeof(Iborder))));
  stw(b->can,70);
  sth(b->can,42);
  sttext(b->can,"Cancel");
  stfn(b->can,fn1(b->canfn,docan,b));
 add(b,b->can);
 b->clr=mk(Button);
  stborder(b->clr,mkIkborder(malloc(sizeof(Iborder))));
  stw(b->clr,70);
  sth(b->clr,42);
  sttext(b->clr,"Do it");
  stfn(b->clr,fn1(b->clrfn,doclr,b));
 add(b,b->clr);
 b->ok=mk(Button);
  stborder(b->ok,mkIkborder(malloc(sizeof(Iborder))));
  stw(b->ok,70);
  sth(b->ok,42);
  sttext(b->ok,"  OK  ");
  stfn(b->ok,fn1(b->okfn,dook,b));
 add(b,b->ok);
 b->selset=mk(Selset);
 add(b,b->selset);
 btot(b,b->selset,0);
 lsame(b,b->selset,0);
 t=mk(Text);
  st(t,"Select valves to be watered:");
 add(b,t);
 return b;
 }
