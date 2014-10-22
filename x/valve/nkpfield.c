/* Editing field with pop-up numeric keypad
 */

#include <notif.h>
#include "nkpfield.h"

int nkpfieldclick();

int nkpfn(stat,b,n)
Nkpfield *b;
 {
 if(!stat)
  {
  if(!b->tedit->funcs->user(b->tedit,n,0,0,0,0,0) && n==13)
   {
   rm(b->keypad);
   stfn(b->button,fn1(b->buttonfn,nkpfieldclick,b));
   }
  else
   {
   stfn(b->keypad,fn1(b->keypadfn,nkpfn,b));
   }
  }
 return stat;
 }

int nkpfieldclick(stat,b)
Nkpfield *b;
 {
 if(!stat)
  {
  int xx=0, yy=0;
  b->keypad=mk(Keypad);
   stflg(b->keypad,gtflg(b->keypad)|flgoverride);
   placedialog(b,b->keypad);
   stfn(b->keypad,fn1(b->keypadfn,nkpfn,b));
  add(root,b->keypad);
  stfocus(b->tedit);
  }
 return stat;
 }

struct nkpfieldfuncs nkpfieldfuncs;

Nkpfield *mkNkpfield(b)
Nkpfield *b;
 {
 Button *t;
 mkWidget(b);
 if(!nkpfieldfuncs.on)
  {
  mcpy(&nkpfieldfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  }
 b->funcs= &nkpfieldfuncs;
 stborder(b,mkEkborder(malloc(sizeof(Eborder))));
 ledge(b,0);
 tedge(b,0);
 b->tedit=mk(Tedit);
  stborder(b->tedit,NULL);
  stfgnd(b->tedit,getgc("10x20","black"));
  stwidth(b->tedit,100);
  stheight(b->tedit,53);
  st(b->tedit,"{d 2 1 64}");
 add(b,b->tedit);
 tedge(b,0);
 ltor(b,b->tedit,4);
 b->button=mk(Button);
  stborder(b->button,mkIkborder(malloc(sizeof(Iborder))));
  sttext(b->button,"SET");
  stw(b->button,53);
  sth(b->button,53);
  stfn(b->button,fn1(b->buttonfn,nkpfieldclick,b));
 add(b,b->button);
 return b;
 }
