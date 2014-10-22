/* Job dialog widget */

#include <notif.h>
#include "jdialog.h"

static void jdialogst(b,n)
Jdialog *b;
Jctrl *n;
 {
 b->jctrl=n;
 }

struct jdialogfuncs jdialogfuncs;

static int doclr(stat,b)
Jdialog *b;
 {
 if(!stat)
  {
  canit(b->jctrl->name);
  redraw(b->jctrl);
  rm(b);
  }
 }

static int jdocan(stat,b)
Jdialog *b;
 {
 if(!stat)
  {
  rm(b);
  }
 }

static int udialogpress1(stat,m,junk,key,state,x,y,org)
Jdialog *m;
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

void jdialogrm(b)
Jdialog *b;
 {
 modalpop();
 (widgetfuncs.rm)(b);
 }

Jdialog *mkJdialog(b)
Jdialog *b;
 {
 Text *t;
 mkWidget(b);
 if(!jdialogfuncs.on)
  {
  mcpy(&jdialogfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  jdialogfuncs.st=jdialogst;
  jdialogfuncs.rm=jdialogrm;
  kcpy(jdialogfuncs.kmap,widgetfuncs.kmap);
  kadd(jdialogfuncs.kmap,NULL,"Press1",udialogpress1,NULL);
  }
 b->funcs= &jdialogfuncs;
 modal(b);
 stflg(b,gtflg(b)|flgoverride);
 stborder(b,mk(Titlebar));
  st((Titlebar *)gtborder(b),"Job control");

 t=mk(Text);
  st(t,"Cancel job?");
 add(b,t);

 sth(b,80);

 ltor(b,NULL,0);
 bedge(b,0);
 b->can=mk(Button);
  stborder(b->can,mkIkborder(malloc(sizeof(Iborder))));
  stw(b->can,70);
  sth(b->can,42);
  sttext(b->can,"  No  ");
  stfn(b->can,fn1(b->canfn,jdocan,b));
 add(b,b->can);
 b->clr=mk(Button);
  stborder(b->clr,mkIkborder(malloc(sizeof(Iborder))));
  stw(b->clr,70);
  sth(b->clr,42);
  sttext(b->clr,"  OK  ");
  stfn(b->clr,fn1(b->clrfn,doclr,b));
 add(b,b->clr);
 return b;
 }
