/* Keypad thingy
 *  set maximum limits
 *  allow for return key and callback function
 *  allow for keypresses
 */

#include <notif.h>
#include "keypad.h"

struct keypadfuncs keypadfuncs;

int keypadfn(stat,b,t,n,task)
Keypad *b;
Button *t;
TASK *task;
 {
 if(!stat)
  {
  stfn(t,fn4(task,keypadfn,b,t,n,task));
  cont1(b->fn,n); b->fn=0;
  }
 return stat;
 }

void keypadstfn(b,fn)
Keypad *b;
TASK *fn;
 {
 b->fn=fn;
 }

void keypadrm(b)
Keypad *b;
 {
 modalpop();
 cancel(b->fn); b->fn=0;
 (widgetfuncs.rm)(b);
 }

Keypad *mkKeypad(b)
Keypad *b;
 {
 Button *t;
 mkWidget(b);
 if(!keypadfuncs.on)
  {
  mcpy(&keypadfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  keypadfuncs.stfn=keypadstfn;
  keypadfuncs.rm=keypadrm;
  }
 b->funcs= &keypadfuncs;
 modal(b);
 stborder(b,mk(Titlebar));
  st((Titlebar *)gtborder(b),"Keypad");
 b->fn=0;
 t=mk(Button);
  stborder(t,mkIkborder(malloc(sizeof(Iborder))));
  sttext(t,"7");
  sth(t,60);
  stw(t,60);
  stfn(t,fn4(&b->tasks[7],keypadfn,b,t,0x37,&b->tasks[7]));
 add(b,t);
 tsame(b,t,0);
 t=mk(Button);
  stborder(t,mkIkborder(malloc(sizeof(Iborder))));
  sttext(t,"8");
  sth(t,60);
  stw(t,60);
  stfn(t,fn4(&b->tasks[8],keypadfn,b,t,0x38,&b->tasks[8]));
 add(b,t);
 t=mk(Button);
  stborder(t,mkIkborder(malloc(sizeof(Iborder))));
  sttext(t,"9");
  sth(t,60);
  stw(t,60);
  stfn(t,fn4(&b->tasks[9],keypadfn,b,t,0x39,&b->tasks[9]));
 add(b,t);
 ltor(b,NULL,0);
 ttob(b,t,0);
 t=mk(Button);
  stborder(t,mkIkborder(malloc(sizeof(Iborder))));
  sttext(t,"4");
  sth(t,60);
  stw(t,60);
  stfn(t,fn4(&b->tasks[4],keypadfn,b,t,0x34,&b->tasks[4]));
 add(b,t);
 tsame(b,t,0);
 t=mk(Button);
  stborder(t,mkIkborder(malloc(sizeof(Iborder))));
  sttext(t,"5");
  sth(t,60);
  stw(t,60);
  stfn(t,fn4(&b->tasks[5],keypadfn,b,t,0x35,&b->tasks[5]));
 add(b,t);
 t=mk(Button);
  stborder(t,mkIkborder(malloc(sizeof(Iborder))));
  sttext(t,"6");
  sth(t,60);
  stw(t,60);
  stfn(t,fn4(&b->tasks[6],keypadfn,b,t,0x36,&b->tasks[6]));
 add(b,t);
 ltor(b,NULL,0);
 ttob(b,t,0);
 t=mk(Button);
  stborder(t,mkIkborder(malloc(sizeof(Iborder))));
  sttext(t,"1");
  sth(t,60);
  stw(t,60);
  stfn(t,fn4(&b->tasks[1],keypadfn,b,t,0x31,&b->tasks[1]));
 add(b,t);
 tsame(b,t,0);
 t=mk(Button);
  stborder(t,mkIkborder(malloc(sizeof(Iborder))));
  sttext(t,"2");
  sth(t,60);
  stw(t,60);
  stfn(t,fn4(&b->tasks[2],keypadfn,b,t,0x32,&b->tasks[2]));
 add(b,t);
 t=mk(Button);
  stborder(t,mkIkborder(malloc(sizeof(Iborder))));
  sttext(t,"3");
  sth(t,60);
  stw(t,60);
  stfn(t,fn4(&b->tasks[3],keypadfn,b,t,0x33,&b->tasks[3]));
 add(b,t);
 ltor(b,NULL,0);
 ttob(b,t,0);
 t=mk(Button);
  stborder(t,mkIkborder(malloc(sizeof(Iborder))));
  sticon(t,picltarw);
  sth(t,60);
  stw(t,60);
  stfn(t,fn4(&b->tasks[10],keypadfn,b,t,8,&b->tasks[10]));
 add(b,t);
 tsame(b,t,0);
 t=mk(Button);
  stborder(t,mkIkborder(malloc(sizeof(Iborder))));
  sttext(t,"0");
  sth(t,60);
  stw(t,60);
  stfn(t,fn4(&b->tasks[0],keypadfn,b,t,0x30,&b->tasks[0]));
 add(b,t);
 t=mk(Button);
  stborder(t,mkIkborder(malloc(sizeof(Iborder))));
  sttext(t,"OK");
  sth(t,60);
  stw(t,60);
  stfn(t,fn4(&b->tasks[11],keypadfn,b,t,13,&b->tasks[11]));
 add(b,t);
 return b;
 }

Keypad *mkTkeypad(b)
Keypad *b;
 {
 Button *t;
 mkKeypad(b);
 t=qnth(b->kids->next,2)->data;
 ltor(b,t,0);
 tsame(b,t,0);
 t=mk(Button);
  stborder(t,mkIkborder(malloc(sizeof(Iborder))));
  sttext(t,"AM");
  sth(t,60);
  stw(t,60);
  stfn(t,fn4(&b->tasks[12],keypadfn,b,t,0x1,&b->tasks[12]));
 add(b,t);
 t=qnth(b->kids->next,5)->data;
 ltor(b,t,0);
 tsame(b,t,0);
 t=mk(Button);
  stborder(t,mkIkborder(malloc(sizeof(Iborder))));
  sttext(t,"PM");
  sth(t,60);
  stw(t,60);
  stfn(t,fn4(&b->tasks[13],keypadfn,b,t,0x2,&b->tasks[13]));
 add(b,t);
 return b;
 }
