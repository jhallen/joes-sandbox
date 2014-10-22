/* Job control widget */

#include <notif.h>
#include "ldialog.h"

void listem(s,a)
char *s;
int a[64];
 {
 int x;
 int flg=0;
 for(x=0;x<64;++x)
  if(a[x])
   {
   int y;
   if(flg) *s++=',';
   flg=1;
   for(y=x;y!=64 && a[y];++y);
   if(y-1!=x) sprintf(s,"%d-%d",x+1,y);
   else sprintf(s,"%d",x+1);
   s+=strlen(s);
   x=y-1;
   }
 *s=0;
 }

static int lctrldraw(b)
Lctrl *b;
 {
 char buf[1024];
 drawclr(b);
 sprintf(buf,"%d",b->name+1);
 if(limits[b->name].valid)
  {
  sprintf(buf+strlen(buf),"  %d Max of %d in ",limits[b->name].cur,limits[b->name].max);
  listem(buf+strlen(buf),limits[b->name].valves);
  }
 drawtxt(b,gtfgnd(b),10,30,buf);
 return 0;
 }

static int lctrluser(b,key,state,x,y)
Lctrl *b;
 {
 if(!(gtflg(b)&flgactivated)) return 1;
 if(key==XK_Press1)
  {
  Ldialog *l=mk(Ldialog);
   st(l,b);
   stx(l,0);
   sty(l,0);
  add(root,l);
  }
 return 1;
 }

static void lctrlstfn(b,c)
Lctrl *b;
TASK *c;
 {
 b->fn=c;
 }

static void lctrlsttext(b,n)
Lctrl *b;
 {
 b->name=n;
 }

static void lctrlst(b,n)
Lctrl *b;
 {
 redraw(b);
 }

static void lctrlrm(b)
Lctrl *b;
 {
 cancel(b->fn); b->fn=0;
 (widgetfuncs.rm)(b);
 }

struct lctrlfuncs lctrlfuncs;

Lctrl *mkLctrl(b)
Lctrl *b;
 {
 mkWidget(b);
 if(!lctrlfuncs.on)
  {
  mcpy(&lctrlfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  lctrlfuncs.user=lctrluser;
  lctrlfuncs.stfn=lctrlstfn;
  lctrlfuncs.sttext=lctrlsttext;
  lctrlfuncs.st=lctrlst;
  lctrlfuncs.expose=lctrldraw;
  lctrlfuncs.rm=lctrlrm;
  }
 b->funcs= &lctrlfuncs;
 b->fn=0;
 b->name=0;
 stborder(b,mkIkborder(malloc(sizeof(Iborder))));
 stw(b,300);
 sth(b,53);
 return b;
 }
