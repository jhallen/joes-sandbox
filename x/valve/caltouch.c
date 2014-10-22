/* Touch screen calibration */

#include <notif.h>
#include "caltouch.h"

struct caltouchfuncs caltouchfuncs;

extern int calytop, calybot, calxleft, calxright;

static int caltouchdraw(t)
Caltouch *t;
 {
 drawclr(t);
 drawtxt(t,gtfgnd(t),100,100,"Calibrate touch-screen.");
 switch(t->state)
  {
  case 0:
   drawdot(t,gtfgnd(t),10,480/2);
   break;
  case 1:
   drawdot(t,gtfgnd(t),640-10,480/2);
   break;
  case 2:
   drawdot(t,gtfgnd(t),640/2,10);
   break;
  case 3:
   drawdot(t,gtfgnd(t),640/2,480-10);
   break;
  }
 }

static int caltouchuser(t,key,state,x,y)
Caltouch *t;
 {
 if(key==XK_Press1)
  {
  switch(t->state)
   {
   case 0:
    calxleft=x;
    break;
   case 1:
    calxright=x;
    break;
   case 2:
    calytop=y;
    break;
   case 3:
    calybot=y;
    break;
   }
  if(++t->state==4)
   {
   rm(t);
   }
  else
   redraw(t);
  }
 }

Caltouch *mkCaltouch(w)
Caltouch *w;
 {
 mkWidget(w);
 if(!caltouchfuncs.on)
  {
  mcpy(&caltouchfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  caltouchfuncs.expose=caltouchdraw;
  caltouchfuncs.user=caltouchuser;
  }
 w->funcs= &caltouchfuncs;
 stw(w,640);
 sth(w,480);
 stx(w,0);
 sty(w,0);
 w->state=0;
 return w;
 }
