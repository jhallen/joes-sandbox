/* Job control widget */

#include <notif.h>
#include "ldialog.h"

static void ldialogst(b,n)
Ldialog *b;
Lctrl *n;
 {
 b->lctrl=n;
 if(limits[n->name].valid)
  {
  char buf[10];
  st(b->selset,limits[n->name].valves);
  sprintf(buf,"%d",limits[n->name].max);
  stfield(b->nkp->tedit,0,buf);
  }
 }

struct ldialogfuncs ldialogfuncs;

static int dook(stat,b)
Ldialog *b;
 {
 if(!stat)
  {
  int x;
  for(x=0;x!=64;++x)
   limits[b->lctrl->name].valves[x]=b->selset->set[x];
  limits[b->lctrl->name].cur=0;
  for(x=0;x!=64;++x)
   if(limits[b->lctrl->name].valves[x] && (valves[x].on))
    ++limits[b->lctrl->name].cur;
  limits[b->lctrl->name].max=atoi(gt(b->nkp->tedit)[0]);
  limits[b->lctrl->name].valid=1;
  redraw(b->lctrl);
  rm(b);
  }
 }

static int doclr(stat,b)
Ldialog *b;
 {
 if(!stat)
  {
  limits[b->lctrl->name].valid=0;
  redraw(b->lctrl);
  rm(b);
  }
 }

static int docan(stat,b)
Ldialog *b;
 {
 if(!stat)
  {
  rm(b);
  }
 }

static int udialogpress1(stat,m,junk,key,state,x,y,org)
Ldialog *m;
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

void ldialogrm(b)
Ldialog *b;
 {
 modalpop();
 (widgetfuncs.rm)(b);
 }

Ldialog *mkLdialog(b)
Ldialog *b;
 {
 Text *t;
 mkWidget(b);
 if(!ldialogfuncs.on)
  {
  mcpy(&ldialogfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  ldialogfuncs.st=ldialogst;
  ldialogfuncs.rm=ldialogrm;
  kcpy(ldialogfuncs.kmap,widgetfuncs.kmap);
  kadd(ldialogfuncs.kmap,NULL,"Press1",udialogpress1,NULL);
  }
 b->funcs= &ldialogfuncs;
 modal(b);
 stflg(b,gtflg(b)|flgoverride);
 stborder(b,mk(Titlebar));
  st((Titlebar *)gtborder(b),"Define limit group");
 
 ttob(b,NULL,0);
 ledge(b,0);

 t=mk(Text);
  st(t,"Max no. of valves which");
 add(b,t);

 t=mk(Text);
  st(t,"can simultaneously be");
 add(b,t);

 t=mk(Text);
  st(t,"watered for this group:");
 add(b,t);

 ledge(b,20);
 b->lctrl=0;
 b->nkp=mk(Nkpfield);
 add(b,b->nkp);

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
 b->selset=mk(Selset);
 add(b,b->selset);
 btot(b,b->selset,0);
 lsame(b,b->selset,0);
 t=mk(Text);
  st(t,"Valves which make up this group:");
 add(b,t);
 return b;
 }
