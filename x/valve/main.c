#include <stdio.h>
#include <time.h>
#include <notif.h>
#include "keypad.h"
#include "nkpfield.h"
#include "selset.h"
#include "clock.h"
#include "vctrl.h"
#include "jctrl.h"
#include "lctrl.h"
#include "sctrl.h"
#include "pctrl.h"
#include "caltouch.h"

int calxleft, calxright, calytop, calybot;

Index *mainw;

PORT *tport;

int tstate=0;
int tone, ttwo;

void xysend(w,x,y)
Widget *w;
 {
 LINK *l;
 for(l=w->kids->prev;l!=w->kids;l=l->prev)
  {
  Widget *q=(Widget *)l->data;
  void *rtn;
  if(x>=gtx(q) && x<gtx(q)+gtw(q) &&
     y>=gty(q) && y<gty(q)+gth(q) && (gtflg(q)&flgon))
   {
   xysend(q,x-gtx(q),y-gty(q));
   return;
   }
  }
 ev.xbutton.x=x;
 ev.xbutton.y=y;
 ev.xbutton.window=gtwin(w);
 ev.xbutton.root=gtwin(root);
 ev.xbutton.display=dsp;
 sendevent(gtmain(root),&ev,0);
 }

void click(x,y)
 {
 XWarpPointer(dsp,None,gtwin(root),0,0,0,0,x,y);
 ev.type=ButtonPress;
 ev.xbutton.x_root=x;
 ev.xbutton.y_root=y;
 ev.xbutton.state=0;
 ev.xbutton.button=1;
 xysend(root,x,y);
 wsflsh();
 ev.type=ButtonRelease;
 ev.xbutton.x_root=x;
 ev.xbutton.y_root=y;
 ev.xbutton.state=0;
 ev.xbutton.button=1;
 xysend(root,x,y);
 wsflsh();
 }

TASK tfuncfn[1];

TASK calfn[1];

void tfunc(arg,buf,len)
void *arg;
unsigned char *buf;
 {
 unsigned char c=buf[0];
 if(c&0x80) tstate=0;
 switch(tstate)
  {
  case 0:
   if((c&0xc0)==0xc0)
    {
    tone=c;
    tstate=1;
    }
   break;
  case 1:
   ttwo=c;
   tstate=2;
   break;
  case 2:
   {
   int x=ttwo+((tone<<4)&0x380);
   int y=c+((tone<<7)&0x380);

/*   printf("x=%d y=%d\n",x,y); */

   x=(x-35)*640/(983-35);
   y=(y-80)*480/(971-80);

   if(x<0) x=0;
   if(x>=640) x=639;
   if(y<0) y=0;
   if(y>=480) y=479;
   tstate=0;
   click(x,y);
   break;
   }
  }
 stportread(tport,fn0(tfuncfn,tfunc),1);
 }

Button *calibrate;	/* Calibration button */
Caltouch *caltouch;

int docal(stat)
 {
 if(!stat)
  {
  stfn(calibrate,fn0(calfn,docal));
  caltouch=mk(Caltouch);
  add(root,caltouch);
  }
 return stat;
 }

Widget *clockw;		/* Clock window */
Clock *theclock;		/* Clock in the window */

Widget *jobw;		/* Jobs window */
Jctrl *jctrls[32];
struct job jobs[32];

Widget *schedw;		/* Schedule window */
struct sched scheds[32];	/* Schedule */

Widget *progw;		/* Programs window */
struct program programs[16];	/* Programs */

Widget *limw;		/* Limits window */
Lctrl *lctrls[8];
struct limit limits[8];	/* Current limits */

/* Current valve state */

Widget *valvew;		/* Valves window */
Vctrl *vctrls[64];	/* Valve control widgets */

struct valve valves[64];

/* Write long to I/O port */

inline void outportb(unsigned short port,unsigned char v)
 {
 __asm__ __volatile__ ("outb %%al,%%dx"::"a" (v),"d" (port));
 }

int port=0x378;

int map[]=
 {
 0, 2, 1, 4, 3, 6, 5, 8, 7, 10, 9, 12, 11, 14, 13, 16,
 15, 18, 17, 20, 19, 22, 21, 24, 23, 26, 25, 28, 27, 30, 29, 31
 };

int valveizd=0;

void valveiz()
 {
 int x;
 outportb(port+2,0);
 for(x=0;x!=32;++x)
  {
  outportb(port,0x00);
  outportb(port,0x00);
  outportb(port,0x00);
  outportb(port,0x44);
  outportb(port,0x44);
  outportb(port,0x44);
  }
 outportb(port,0x66);
 outportb(port+2,1);
 valveizd=1;
 }

void valveout()
 {
 int x;
 if(!valveizd) return;
 for(x=0;x!=32;++x)
  {
  outportb(port,((valves[map[x]].on)<<3)+((valves[map[x]+32].on)<<7)+0x22);
  outportb(port,((valves[map[x]].on)<<3)+((valves[map[x]+32].on)<<7)+0x22);
  outportb(port,((valves[map[x]].on)<<3)+((valves[map[x]+32].on)<<7)+0x22);
  outportb(port,((valves[map[x]].on)<<3)+((valves[map[x]+32].on)<<7)+0x66);
  outportb(port,((valves[map[x]].on)<<3)+((valves[map[x]+32].on)<<7)+0x66);
  outportb(port,((valves[map[x]].on)<<3)+((valves[map[x]+32].on)<<7)+0x66);
  }
 outportb(port,0x44);
 outportb(port,0x44);
 outportb(port,0x44);
 outportb(port,0x66);
 outportb(port,0x66);
 outportb(port,0x66);
 outportb(port+2,1);
 }

/* Deallocate valve's resources and turn off valve */

turnoff(x)
 {
 int y;
 if(valves[x].on)
  {
  valves[x].on=0;
  redraw(vctrls[x]);
  for(y=0;y!=8;++y)
   if(limits[y].valid && limits[y].valves[x])
    {
    --limits[y].cur;
    redraw(lctrls[y]);
    }
  }
 }

/* Allocate resources for a valve and turn valve on.  Returns 1 if we
   turned the valve on.  Return 0 if we could not allocate the resources
   or 2 if valve was already on. */

turnon(valve)
 {
 int y, z;
/* printf("turnon: valve=%d\n",valve); */
 /* Valve already on? */
 if(valves[valve].on) return 2;
 for(y=0;y!=8;++y)
  if(limits[y].valid && limits[y].valves[valve])
   if(limits[y].cur+1>limits[y].max) break;
   else
    {
    ++limits[y].cur;
    redraw(lctrls[y]);
    }
 if(y==8)
  {
  valves[valve].on=1;
  redraw(vctrls[valve]);
  return 1;
  }
 else
  {
  for(z=0;z!=y;++z)
   if(limits[z].valid && limits[z].valves[valve])
    {
    --limits[z].cur;
    redraw(lctrls[z]);
    }
  return 0;
  }
 }

/* Try to turn a valve on- turns off valves with lower prec. value if it 
   has to.  Returns 0 for failure. */

int hardon(valve,prec)
 {
 int flg;
 int x;
/* printf("hardon: valve=%d prec=%d\n",valve,prec); */
 do 
  {
  if((x=turnon(valve))==1)
   { /* We did it! */
   return 1;
   }
  /* Try to suspend lower prec. jobs */
  flg=0;
  for(x=0;x!=64;++x)
   if(valves[x].on && !valves[x].manual && jobs[valves[x].job].pgm.dwell>prec /* &&
      jobs[valves[x].job].valves[x]>prec */ )
    { /* Turn off this one */
    flg=1;
    turnoff(x);
    jobs[valves[x].job].waiting[x]=1;
    }
  }
  while(flg);
 return 0;
 }

/* Schedular */

int lasttime=60*60*24;	/* Last time we visited the schedular */

/* Check if 'val' is between last and cur */

int inrange(last,cur,val)
 {
 if(cur>last)
  {
  if(val>last && val<=cur) return 1;
  else return 0;
  }
 else
  {
  if(val<=cur) val+=60*60*24;
  cur+=60*60*24;
  if(val>last && val<=cur) return 1;
  }
 return 0;
 }

canit(n)
 {
 if(jobs[n].valid)
  {
  int x;
  jobs[n].valid=0;
  for(x=0;x!=64;++x)
   if(valves[x].on && !valves[x].manual && valves[x].job==n)
    turnoff(x);
  }
 }

/* Copy program dwell time into requested valve timers */

cycleit(x)
 {
 int n;
 for(n=0;n!=64;++n)
  if(jobs[x].pgm.valves[n])
   {
   jobs[x].waiting[n]=1;
   jobs[x].valves[n]=jobs[x].pgm.dwell;
   }
  else
   {
   jobs[x].waiting[n]=0;
   jobs[x].valves[n]=0;
   }
 }

/* Find free job, copy program into it */

doit(x)
 {
 int y;
 time_t curtime=time(0L);
 struct tm *curtm=localtime(&curtime);
 for(y=0;y!=32;++y) if(!jobs[y].valid) break;
 if(y==32) return;
 jobs[y].hour=curtm->tm_hour;
 jobs[y].min=curtm->tm_min;
 jobs[y].sec=curtm->tm_sec;
 jobs[y].program=x+1;
 jobs[y].pgm=programs[x];
 jobs[y].valid=1;
 jobs[y].cycle=0;
 cycleit(y);
 redraw(jctrls[y]);
 }

/* Start a valve */

startit(job,valve)
 {
/* printf("startit job=%d valve=%d\n",job,valve); */
 if(hardon(valve,jobs[job].pgm.dwell))
  { /* We did it! */
  valves[valve].job=job;
  jobs[job].waiting[valve]=0;
  return 1;
  }
 return 0;
 }

sched(curtime)
 {
 int flg;
 int x;
 int dly;
 int prec;
 int lowest;

/* printf("sched:\n"); */

 if(curtime<lasttime) lasttime=curtime-1;
 dly=curtime-lasttime;

 /* Decrement dwell times.  Turn off any valves which are done. */
 for(x=0;x!=64;++x)
  if(valves[x].on && !valves[x].manual)
   { /* Valve must be on because of job */
   jobs[valves[x].job].valves[x]-=dly;
   if(jobs[valves[x].job].valves[x]<=0)
    {
    jobs[valves[x].job].valves[x]=0;
    turnoff(x);
    }
   }

 /* Submit any scheduled jobs */
 for(x=0;x!=32;++x)
  if(scheds[x].valid && programs[scheds[x].program-1].valid)
   {
   int stime=scheds[x].sec+scheds[x].min*60+scheds[x].hour*3600;
   if(inrange(lasttime,curtime,stime))
    doit(scheds[x].program-1);
   }

 /* Decrement repeat counters */
 for(x=0;x!=32;++x)
  if(jobs[x].valid)
   if((jobs[x].pgm.repeat-=dly)<0)
    jobs[x].pgm.repeat=0;

 /* Decrement cycle counters and start job */
 for(x=0;x!=32;++x)
  if(jobs[x].valid)
   if(jobs[x].cycle)
    {
    if((jobs[x].cycle-=dly)<=0)
     {
     jobs[x].cycle=0;
     if(jobs[x].pgm.repeat)
      cycleit(x);
     }
    }

 /* Check for jobs which are done */
 for(x=0;x!=32;++x)
  if(jobs[x].valid)
   {
   int n;
   for(n=0;n!=64;++n)
    if(jobs[x].valves[n] || jobs[x].waiting[n]) break;
   if(n==64)
    { /* We're done with this cycle */
    if(jobs[x].pgm.repeat)
     { /* We should repeat */
     if(!jobs[x].cycle)
      /* If we're not in a cycle delay already */
      if(!(jobs[x].cycle=jobs[x].pgm.cycle))
       /* No cycle delay: start next cycle now */
       cycleit(x);
     }
    else
     { /* No repeating: clear job */
     jobs[x].valid= 0;
     redraw(jctrls[x]);
     }
    }
   }

 /* Schedule new valves */
 prec=0;
 loop:
 lowest=0x7FFFFFFF;
 /* Find lowest dwell which is greater than prec */
 for(x=0;x!=32;++x)
  if(jobs[x].valid)
   {
   int start;
   for(start=0;start!=64;++start)
    if(jobs[x].waiting[start])
     if(!valves[start].enable)
      /* Skip valves which are manually disabled */
      jobs[x].waiting[start]=jobs[x].valves[start]=0;
     else if(jobs[x].pgm.dwell>prec && jobs[x].pgm.dwell<lowest)
      lowest=jobs[x].pgm.dwell;
   }
 if(lowest!=0x7FFFFFFF)
  {
  /* Try to turn on valves with dwells which equal prec */
  prec=lowest;
  for(x=0;x!=32;++x)
   if(jobs[x].valid)
    {
    int start;
    for(start=0;start!=64;++start)
     if(jobs[x].waiting[start])
      if(!valves[start].enable)
       /* Skip valves which are manually disabled */
       jobs[x].waiting[start]=jobs[x].valves[start]=0;
      else if(jobs[x].pgm.dwell==prec) startit(x,start);
    }
  goto loop;
  }

 lasttime=curtime;
 valveout();
 }

/* Database load & store */

vlist(f,v)
FILE *f;
int *v;
 {
 int x;
 for(x=0;x!=64;++x)
  fprintf(f,"%c",'0'+v[x]);
 }

listv(v,s)
int *v;
char *s;
 {
 int x;
 for(x=0;x!=64;++x) v[x]=s[x]-'0';
 }

void store()
 {
 int x;
 FILE *f=fopen("valve.db","r+");
 if(!f) f=fopen("valve.db","w");
 fprintf(f,"# Limits (must be given first)\n");
 for(x=0;x!=8;++x)
  {
  fprintf(f,"L	%d	",x);
  vlist(f,limits[x].valves);
  fprintf(f,"	%2.2d	%d\n",limits[x].max,limits[x].valid);
  }
 fprintf(f,"# Valves\n");
 for(x=0;x!=64;++x)
  fprintf(f,"V	%2.2d	%d	%d\n",x,valves[x].enable,valves[x].manual);
 fprintf(f,"# Schedule\n");
 for(x=0;x!=32;++x)
  {
  fprintf(f,"S	%2.2d	%2.2d	%2.2d	%2.2d	%2.2d	%d\n",x,scheds[x].hour,
         scheds[x].min,scheds[x].sec,scheds[x].program,scheds[x].valid);
  }
 fprintf(f,"# Programs\n");
 for(x=0;x!=16;++x)
  {
  fprintf(f,"P	%2.2d	",x);
  vlist(f,programs[x].valves);
  fprintf(f,"	%5.5d	%5.5d	%5.5d	%d\n",programs[x].dwell,
  	programs[x].repeat,programs[x].cycle,programs[x].valid);
  }
 fclose(f);
 }

void load()
 {
 int x;
 char buf[1024];
 char *ptrs[20];
 FILE *f=fopen("valve.db","r");
 if(!f) return;
 while(fgets(buf,1023,f))
  {
  fields(buf,ptrs,'\t');
  switch(buf[0])
   {
   case 'V':
    {
    x=atoi(ptrs[1]);
    valves[x].enable=atoi(ptrs[2]);
    if(atoi(ptrs[3]) && turnon(x)==1)
     valves[x].manual=1,
     valves[x].on=1;
    break;
    }
   case 'S':
    {
    x=atoi(ptrs[1]);
    scheds[x].hour=atoi(ptrs[2]);
    scheds[x].min=atoi(ptrs[3]);
    scheds[x].sec=atoi(ptrs[4]);
    scheds[x].program=atoi(ptrs[5]);
    scheds[x].valid=atoi(ptrs[6]);
    break;
    }
   case 'P':
    {
    x=atoi(ptrs[1]);
    listv(programs[x].valves,ptrs[2]);
    programs[x].dwell=atoi(ptrs[3]);
    programs[x].repeat=atoi(ptrs[4]);
    programs[x].cycle=atoi(ptrs[5]);
    programs[x].valid=atoi(ptrs[6]);
    break;
    }
   case 'L':
    {
    x=atoi(ptrs[1]);
    listv(limits[x].valves,ptrs[2]);
    limits[x].max=atoi(ptrs[3]);
    limits[x].valid=atoi(ptrs[4]);
    break;
    }
   }
  }
 fclose(f);
 }

TASK autosv[1];

int autosave(stat)
 {
 if(!stat)
  {
  store();
  submit(30*1000000,fn0(autosv,autosave));
  }
 }

int flshio()
 {
 int flg;
 wsflsh();
 flg=io();
 wsflsh();
 return flg;
 }

main()
 {
 int fd;
 int x, y;
 Button *b, *bb;
 Vctrl *t;
 Jctrl *j;
 Sctrl *s;
 Pctrl *p;
 Lctrl *l;
 Text *txt;
 izws();
 if(!iopl(3)) valveiz();
 mainw=mk(Index);
  stwidth(mainw,640);
  stheight(mainw,480);
  stx(mainw,0);
  sty(mainw,0);

  b=mk(Button);
   stborder(b,mkIkborder(malloc(sizeof(Iborder))));
   stw(b,80);
   sth(b,34);
   txt=mk(Text);
    st(txt,"Clock");
   add(b,txt);
  clockw=mk(Widget);
   stw(clockw,629);
   sth(clockw,431);
   txt=mk(Text);
    st(txt,"The current time:");
    stx(txt,150);
    sty(txt,100);
   add(clockw,txt);
   theclock=mk(Clock);
    stx(theclock,200);
    sty(theclock,150);
   add(clockw,theclock);
  addpair(mainw,b,clockw);

  b=mk(Button);
   stborder(b,mkIkborder(malloc(sizeof(Iborder))));
   stw(b,80);
   sth(b,34);
   txt=mk(Text);
    st(txt,"Valves");
   add(b,txt);
  valvew=mk(Widget);
   stw(valvew,629);
   sth(valvew,431);
   ledge(valvew,3);
   for(x=0;x!=8;++x)
    {
    tedge(valvew,68);
    if(x) ltor(valvew,t,0);
    for(y=0;y!=8;++y)
     {
     t=mk(Vctrl);
      sttext(t,y+x*8);
     add(valvew,t);
     vctrls[y+x*8]=t;
     if(!y) ttob(valvew,t,0), lsame(valvew,t,0);
     }
    }
   txt=mk(Text);
    st(txt,"Manual valve controls");
    sty(txt,0);
    stx(txt,0);
   add(valvew,txt);
  addpair(mainw,b,valvew);

  b=mk(Button);
   stborder(b,mkIkborder(malloc(sizeof(Iborder))));
   stw(b,80);
   sth(b,34);
   txt=mk(Text);
    st(txt,"Status");
   add(b,txt);
  jobw=mk(Widget);
   stw(jobw,629);
   sth(jobw,431);
   ledge(jobw,3);
   for(x=0;x!=4;++x)
    {
    tedge(jobw,3);
    if(x) ltor(jobw,j,0);
    for(y=0;y!=8;++y)
     {
     j=mk(Jctrl);
      sttext(j,y+x*8);
     add(jobw,j);
     jctrls[y+x*8]=j;
     if(!y) ttob(jobw,j,0), lsame(jobw,j,0);
     }
    }
  addpair(mainw,b,jobw);

  b=mk(Button);
   stborder(b,mkIkborder(malloc(sizeof(Iborder))));
   stw(b,80);
   sth(b,34);
   txt=mk(Text);
    st(txt,"Schedule");
   add(b,txt);
  schedw=mk(Widget);
   stw(schedw,629);
   sth(schedw,431);
   ledge(schedw,3);
   for(x=0;x!=4;++x)
    {
    tedge(schedw,3);
    if(x) ltor(schedw,s,0);
    for(y=0;y!=8;++y)
     {
     s=mk(Sctrl);
      sttext(s,y+x*8);
     add(schedw,s);
     if(!y) ttob(schedw,s,0), lsame(schedw,s,0);
     }
    }
  addpair(mainw,b,schedw);

  b=mk(Button);
   stborder(b,mkIkborder(malloc(sizeof(Iborder))));
   stw(b,80);
   sth(b,34);
   txt=mk(Text);
    st(txt,"Programs");
   add(b,txt);
  progw=mk(Widget);
   stw(progw,629);
   sth(progw,431);
   ledge(progw,3);
   for(x=0;x!=2;++x)
    {
    tedge(progw,3);
    if(x) ltor(progw,p,0);
    for(y=0;y!=8;++y)
     {
     p=mk(Pctrl);
      sttext(p,y+x*8);
     add(progw,p);
     if(!y) ttob(progw,p,0), lsame(progw,p,0);
     }
    }
  addpair(mainw,b,progw);

  b=mk(Button);
   stborder(b,mkIkborder(malloc(sizeof(Iborder))));
   stw(b,80);
   sth(b,34);
   txt=mk(Text);
    st(txt,"Limits");
   add(b,txt);
  limw=mk(Widget);
   stw(limw,629);
   sth(limw,431);
   ledge(limw,3);
   for(x=0;x!=1;++x)
    {
    tedge(limw,3);
    if(x) ltor(limw,l,0);
    for(y=0;y!=8;++y)
     {
     l=mk(Lctrl);
      sttext(l,y+x*8);
     add(limw,l);
     lctrls[y+x*8]=l;
     if(!y) ttob(limw,l,0), lsame(limw,l,0);
     }
    }
   bedge(limw,0);
   redge(limw,0);
   calibrate=mk(Button);
    stborder(calibrate,mkIkborder(malloc(sizeof(Iborder))));
    stw(calibrate,80);
    sth(calibrate,34);
    txt=mk(Text);
     st(txt,"Calibrate");
    add(calibrate,txt);
    stfn(calibrate,fn0(calfn,docal));
   add(limw,calibrate);
  addpair(mainw,b,limw);

 add(root,mainw);
 fd=open("/dev/cua1",0);
 system("stty </dev/cua1 2400 -echo raw");
 tport=portopen(fd);
 stportread(tport,fn0(tfuncfn,tfunc),0);
 stblockmode(tport,1);

 system("/usr/bin/X11/xset s off");
 load();
 autosave(0);

 go(flshio,NULL);
 }
