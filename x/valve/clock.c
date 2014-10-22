#include <time.h>
#include <notif.h>
#include "clock.h"

struct clockfuncs clockfuncs;

/* Convert 'struct tm' to time string and set it in 'timew' */

updtimestr(w)
Clock *w;
 {
 if(w->curtm->tm_hour==0)
  sprintf(w->timestr,"%2d:%2.2d.%2.2d AM",12,w->curtm->tm_min,w->curtm->tm_sec);
 else if(w->curtm->tm_hour<12)
  sprintf(w->timestr,"%2d:%2.2d.%2.2d AM",w->curtm->tm_hour,w->curtm->tm_min,w->curtm->tm_sec);
 else if(w->curtm->tm_hour==12)
  sprintf(w->timestr,"%2d:%2.2d.%2.2d PM",12,w->curtm->tm_min,w->curtm->tm_sec);
 else
  sprintf(w->timestr,"%2d:%2.2d.%2.2d PM",w->curtm->tm_hour-12,w->curtm->tm_min,w->curtm->tm_sec);
 st(w->timew,w->timestr);
 }

int clocktick(stat,w)
Clock *w;
 {
 if(!stat)
  {
  time_t curtime;
  struct tm *curtm;
  if((curtime=time(NULL))!=w->oldtime)
   {
   w->oldtime=curtime;
   w->curtm=localtime(&curtime);
   updtimestr(w);
   sched(w->curtm->tm_hour*3600+w->curtm->tm_min*60+w->curtm->tm_sec);
   }
  submit(100000,fn1(w->tevent,clocktick,w));
  }
 return stat;
 }

int dosetclock(stat,w)
Clock *w;
 {
 if(!stat)
  {
  int x=0,y=0;
  Clockset *xx=mk(Clockset);
  abspos(gtmain(w->timew),&x,&y);
   xx->clock=w;
   stx(xx,x);
   sty(xx,y+gth(w->timew));
  add(root,xx);
  w->curtm=localtime(&w->oldtime);
  w->curtm->tm_sec=0;
  updtimestr(w);
  cancel(w->tevent);
  }
 }

void clockrm(w)
Clock *w;
 {
 cancel(w->tevent);
 (widgetfuncs.rm)(w);
 }

Clock *mkClock(w)
Clock *w;
 {
 mkWidget(w);
 if(!clockfuncs.on)
  {
  mcpy(&clockfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  clockfuncs.rm=clockrm;
  }
 w->funcs= &clockfuncs;
 stborder(w,mkEkborder(malloc(sizeof(Eborder))));
 iztask(w->tevent);
 tedge(w,0);
 w->timew=mk(Text);
  stborder(w->timew,NULL);
  sth(w->timew,53);
  stfgnd(w->timew,getgc("10x20","black"));
  clocktick(0,w);
 add(w,w->timew);
 tedge(w,0);
 ltor(w,w->timew,4);
 w->setclock=mk(Button);
  stborder(w->setclock,mkIkborder(malloc(sizeof(Iborder))));
  stw(w->setclock,53);
  sth(w->setclock,53);
  sttext(w->setclock,"Set");
  stfn(w->setclock,fn1(w->setclockfn,dosetclock,w));
 add(w,w->setclock);
 return w;
 }

struct clocksetfuncs clocksetfuncs;

int doinch(stat,w)
Clockset *w;
 {
 if(!stat)
  {
  if(++w->clock->curtm->tm_hour==24) w->clock->curtm->tm_hour=0;
  updtimestr(w->clock);
  stfn(w->inch,fn1(w->inchfn,doinch,w));
  }
 }

int dodech(stat,w)
Clockset *w;
 {
 if(!stat)
  {
  if(w->clock->curtm->tm_hour) --w->clock->curtm->tm_hour;
  else w->clock->curtm->tm_hour=23;
  updtimestr(w->clock);
  stfn(w->dech,fn1(w->dechfn,dodech,w));
  }
 }

int doincm(stat,w)
Clockset *w;
 {
 if(!stat)
  {
  if(++w->clock->curtm->tm_min==60) w->clock->curtm->tm_min=0;
  updtimestr(w->clock);
  stfn(w->incm,fn1(w->incmfn,doincm,w));
  }
 }

int dodecm(stat,w)
Clockset *w;
 {
 if(!stat)
  {
  if(w->clock->curtm->tm_min) --w->clock->curtm->tm_min;
  else w->clock->curtm->tm_min=59;
  updtimestr(w->clock);
  stfn(w->decm,fn1(w->decmfn,dodecm,w));
  }
 }

int dosetit(stat,w)
Clockset *w;
 {
 if(!stat)
  {
  Clock *clk=w->clock;
  char buf[80];
  sprintf(buf,"date %2.2d%2.2d%2.2d%2.2d",w->clock->curtm->tm_mon+1,
          w->clock->curtm->tm_mday,w->clock->curtm->tm_hour,
          w->clock->curtm->tm_min);
  system(buf);
  system("/sbin/clock -w");
  rm(w);
  clocktick(0,clk);
  stfn(clk->setclock,fn1(clk->setclockfn,dosetclock,clk));
  }
 }

int docanit(stat,w)
Clockset *w;
 {
 if(!stat)
  {
  Clock *clk=w->clock;
  rm(w);
  clocktick(0,clk);
  stfn(clk->setclock,fn1(clk->setclockfn,dosetclock,clk));
  }
 }

int uclocksetpress1(stat,m,junk,key,state,x,y,org)
Clockset *m;
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

void clocksetrm(w)
Clockset *w; 
 {
 modalpop();
 (widgetfuncs.rm)(w);
 }

Clockset *mkClockset(w)
Clockset *w;
 {
 Text *t;
 mkWidget(w);
 if(!clocksetfuncs.on)
  {
  mcpy(&clocksetfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  clocksetfuncs.rm=clocksetrm;
  kcpy(clocksetfuncs.kmap,widgetfuncs.kmap);
  kadd(clocksetfuncs.kmap,NULL,"Press1",uclocksetpress1,NULL);
  }
 w->funcs= &clocksetfuncs;

/* stborder(w,mk(Shadow));*/ 
 modal(w);
 stborder(w,mk(Titlebar));
  st((Titlebar *)gtborder(w),"Set clock");
 stflg(w,gtflg(w)|flgoverride);

 ttob(w,NULL,0);
 ledge(w,0);

 t=mk(Text);
  st(t,"Hour");
 add(w,t);

 w->inch=mk(Button);
  stborder(w->inch,mkIkborder(malloc(sizeof(Iborder))));
  stw(w->inch,60);
  sth(w->inch,60);
  sticon(w->inch,picuparw);
  stfn(w->inch,fn1(w->inchfn,doinch,w));
 add(w,w->inch);

 w->dech=mk(Button);
  stborder(w->dech,mkIkborder(malloc(sizeof(Iborder))));
  stw(w->dech,60);
  sth(w->dech,60);
  sticon(w->dech,picdnarw);
  stfn(w->dech,fn1(w->dechfn,dodech,w));
 add(w,w->dech);

 ttob(w,NULL,0);
 ltor(w,w->inch,0);

 t=mk(Text);
  st(t,"Minute");
 add(w,t);
 
 lsame(w,t,0);

 w->incm=mk(Button);
  stborder(w->incm,mkIkborder(malloc(sizeof(Iborder))));
  stw(w->incm,60);
  sth(w->incm,60);
  sticon(w->incm,picuparw);
  stfn(w->incm,fn1(w->incmfn,doincm,w));
 add(w,w->incm);

 w->decm=mk(Button);
  stborder(w->decm,mkIkborder(malloc(sizeof(Iborder))));
  stw(w->decm,60);
  sth(w->decm,60);
  sticon(w->decm,picdnarw);
  stfn(w->decm,fn1(w->decmfn,dodecm,w));
 add(w,w->decm);

 tsame(w,w->incm,0);
 ltor(w,w->incm,0);

 w->setit=mk(Button);
  stborder(w->setit,mkIkborder(malloc(sizeof(Iborder))));
  stw(w->setit,60);
  sth(w->setit,60);
  sttext(w->setit,"OK");
  stfn(w->setit,fn1(w->setitfn,dosetit,w));
 add(w,w->setit);

 ttob(w,w->setit,0);
 lsame(w,w->setit,0);

 w->canit=mk(Button);
  stborder(w->canit,mkIkborder(malloc(sizeof(Iborder))));
  stw(w->canit,60);
  sth(w->canit,60);
  sttext(w->canit,"Cancel");
  stfn(w->canit,fn1(w->canitfn,docanit,w));
 add(w,w->canit);

 return w;
 }
