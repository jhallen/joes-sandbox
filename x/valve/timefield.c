/* Editing field with pop-up numeric keypad
 */

#include <notif.h>
#include "timefield.h"

int timefieldclick();

int timefn(stat,b,n)
Timefield *b;
 {
 if(!stat)
  {
  if(n==1)
   {
   b->ampmflg=0;
   st(b->ampm,"AM");
   stfn(b->keypad,fn1(b->keypadfn,timefn,b));
   }
  else if(n==2)
   {
   b->ampmflg=1;
   st(b->ampm,"PM");
   stfn(b->keypad,fn1(b->keypadfn,timefn,b));
   }
  else if(n==13)
   {
   rm(b->keypad);
   b->tedit->funcs->user(b->tedit,n,0,0,0,0,0);
   stfn(b->button,fn1(b->buttonfn,timefieldclick,b));
   }
  else
   {
   b->tedit->funcs->user(b->tedit,n,0,0,0,0,0);
   stfn(b->keypad,fn1(b->keypadfn,timefn,b));
   }
  }
 return stat;
 }

int timefieldclick(stat,b)
Timefield *b;
 {
 if(!stat)
  {
  b->keypad=mkTkeypad(malloc(sizeof(Keypad)));
   stflg(b->keypad,gtflg(b->keypad)|flgoverride);
   placedialog(b,b->keypad);
   stfn(b->keypad,fn1(b->keypadfn,timefn,b));
  add(root,b->keypad);
  stfocus(b->tedit);
  }
 return stat;
 }

struct timefieldfuncs timefieldfuncs;

Timefield *mkTimefield(b)
Timefield *b;
 {
 Button *t;
 mkWidget(b);
 if(!timefieldfuncs.on)
  {
  mcpy(&timefieldfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  }
 b->funcs= &timefieldfuncs;
 b->ampmflg=0;
 stborder(b,mkEkborder(malloc(sizeof(Eborder))));
 b->tedit=mk(Tedit);
  stborder(b->tedit,NULL);
  stfgnd(b->tedit,getgc("10x20","black"));
  stwidth(b->tedit,92);
  stheight(b->tedit,53);
  st(b->tedit,"{d 2 1 12}:{d 2 0 59}.{d 2 0 59}");
 add(b,b->tedit);
 b->ampm=mk(Text);
  stborder(b->ampm,NULL);
  stfgnd(b->ampm,getgc("10x20","black"));
  stheight(b->ampm,53);
  st(b->ampm,"AM");
 add(b,b->ampm);
 b->button=mk(Button);
  stborder(b->button,mkIkborder(malloc(sizeof(Iborder))));
  sttext(b->button,"SET");
  stw(b->button,53);
  sth(b->button,53);
  stfn(b->button,fn1(b->buttonfn,timefieldclick,b));
 add(b,b->button);
 return b;
 }
