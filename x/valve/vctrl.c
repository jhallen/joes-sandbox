/* Valve switch widget */

#include <notif.h>
#include "vctrl.h"

static int vctrldraw(b)
Vctrl *b;
 {
 char buf[10];
 int wid;
 drawclr(b);
 wid=txtw(gtfgnd(b),"On Off Auto");
 drawtxt(b,gtfgnd(b),gtwidth(b)/2-wid/2,15,"On Off Auto");
 if(valves[b->name].manual)
  { /* on */
  drawbox(b,2,3,15,15,1,gtfgnd(b));
  }
 else if(valves[b->name].enable)
  { /* auto */
  drawbox(b,43,3,29,15,1,gtfgnd(b));
  }
 else
  { /* Off */
  drawbox(b,20,3,21,15,1,gtfgnd(b));
  }
 sprintf(buf,"%d",b->name+1);
 wid=txtw(gtfgnd(b),buf);
 if(valves[b->name].on)
  {
  draw3dbox(b,gtwidth(b)/2-wid/2-4,30-8,wid+6,17,2,stdshadowtop,stdshadowbot);
  drawfilled(b,getgc("10x20","cyan"),gtwidth(b)/2-wid/2-2,30-6,wid+2,13);
  }
 drawtxt(b,gtfgnd(b),gtwidth(b)/2-wid/2,35,buf);
 return 0;
 }

static int vctrluser(b,key,state,x,y)
Vctrl *b;
 {
 if(!(gtflg(b)&flgactivated)) return 1;
 if(key==XK_Press1)
  {
  if(valves[b->name].manual)
   {
   valves[b->name].manual=0;
   turnoff(b->name);
   }
  else if(valves[b->name].enable)
   {
   valves[b->name].enable=0;
   turnoff(b->name);
   }
  else if(x>=gtwidth(b)/2)
   {
   valves[b->name].enable=1;
   }
  else if(hardon(b->name,0))
   {
   valves[b->name].on=1;
   valves[b->name].manual=1;
   }
  redraw(b);
  cont1(b->fn,b->state); b->fn=0;
  return 0;
  }
 return 1;
 }

static void vctrlstfn(b,c)
Vctrl *b;
TASK *c;
 {
 b->fn=c;
 }

static void vctrlsttext(b,n)
Vctrl *b;
 {
 b->name=n;
 }

static void vctrlst(b,n)
Vctrl *b;
 {
 b->autom=n;
 redraw(b);
 }

static void vctrlrm(b)
Vctrl *b;
 {
 cancel(b->fn); b->fn=0;
 (widgetfuncs.rm)(b);
 }

struct vctrlfuncs vctrlfuncs;

Vctrl *mkVctrl(b)
Vctrl *b;
 {
 mkWidget(b);
 if(!vctrlfuncs.on)
  {
  mcpy(&vctrlfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  vctrlfuncs.user=vctrluser;
  vctrlfuncs.stfn=vctrlstfn;
  vctrlfuncs.sttext=vctrlsttext;
  vctrlfuncs.st=vctrlst;
  vctrlfuncs.expose=vctrldraw;
  vctrlfuncs.rm=vctrlrm;
  }
 b->funcs= &vctrlfuncs;
 b->fn=0;
 b->name=0;
 b->state=0;
 b->autom=0;
 stfgnd(b,getgc("6x13","black"));
 stborder(b,mkIkborder(malloc(sizeof(Iborder))));
 stw(b,78);
 sth(b,45);
 return b;
 }
