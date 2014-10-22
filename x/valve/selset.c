/* Selset thingy
 *  set maximum limits
 *  allow for return key and callback function
 *  allow for keypresses
 */

#include <notif.h>
#include "selset.h"

struct selsetfuncs selsetfuncs;

void selsetst(b,n)
Selset *b;
int *n;
 {
 int x;
 for(x=0;x!=64;++x)
  {
  b->set[x]=n[x];
  st(b->tog[x],n[x]);
  }
 }

int selsetfn(stat,b,t,n,fn,state)
Selset *b;
Button *t;
TASK *fn;
 {
 if(!stat)
  {
  b->set[n]=state;
  stfn(t,fn4(fn,selsetfn,b,t,n,fn));
  }
 return stat;
 }

Selset *mkSelset(b)
Selset *b;
 {
 int x, y;
 Toggle *t;
 mkWidget(b);
 if(!selsetfuncs.on)
  {
  mcpy(&selsetfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  selsetfuncs.st=selsetst;
  }
 b->funcs= &selsetfuncs;
 for(x=0;x!=64;++x) b->set[x]=0;
 for(x=0;x!=8;++x)
  {
  ttob(b,NULL,0);
  if(x) ltor(b,t,0);
  for(y=0;y!=8;++y)
   {
   Text *a;
   Icon *i;
   Widget *w;
   char *buf1=b->buf1s[y+x*8];
   char *buf2=b->buf2s[y+x*8];
   sprintf(buf1,"%d",y+x*8+1);
   sprintf(buf2,"%d",y+x*8+1);
   t=mk(Toggle);
    stborder(t,mkIkborder(malloc(sizeof(Iborder))));

    a=mk(Text);
     st(a,buf1);
    stdispoff(t,a);

    w=mk(Widget);
     stw(w,49);
     a=mk(Text);
      st(a,buf2);
     add(w,a);
     hcenter(w);
     ttob(w,a,0);
     i=mk(Icon);
      st(i,piccheck);
     add(w,i);
    stdispon(t,w);

    sth(t,53);
    stw(t,53);
    stfn(t,fn4(&b->tasks[y+x*8],selsetfn,b,t,y+x*8,&b->tasks[y+x*8]));
   add(b,t);
   b->tog[y+x*8]=t;
   lsame(b,t,0);
   }
  }
 return b;
 }
