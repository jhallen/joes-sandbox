/* Callback function manager */

#include "callback.h"

FN *_cont;

static FN *freefns=0;

static FN *alfn()
 {
 FN *r;
 if(!freefns)
  {
  int n;
  r=(FN *)malloc(sizeof(FN)*16);
  for(n=0;n!=16;++n) r[n].fn=freefns, freefns=r+n;
  }
 r=freefns;
 freefns=r->fn;
 return r;
 }

static void frfn(fn)
FN *fn;
 {
 fn->fn=freefns;
 freefns=fn;
 }

FN *hook(fn,proc)
FN *fn, *proc;
 {
 if(fn)
  {
  FN *n=fn;
  for(n=fn;n->fn;n=n->fn);
  n->fn=proc;
  return fn;
  }
 else return proc;
 }

FN *fn0(func,fn)
int (*func)();
FN *fn;
 {
 FN *new=alfn();
 new->func=func;
 new->n=0;
 new->fn=fn;
 return new;
 }

FN *fn1(func,fn,pass1)
int (*func)();
FN *fn;
long pass1;
 {
 FN *new=alfn();
 new->func=func;
 new->n=1;
 new->fn=fn;
 new->args[0]=pass1;
 return new;
 }

FN *fn2(func,fn,pass1,pass2)
int (*func)();
FN *fn;
long pass1,pass2;
 {
 FN *new=alfn();
 new->func=func;
 new->n=2;
 new->fn=fn;
 new->args[0]=pass1; new->args[1]=pass2;
 return new;
 }

FN *fn3(func,fn,pass1,pass2,pass3)
int (*func)();
FN *fn;
long pass1,pass2,pass3;
 {
 FN *new=alfn();
 new->func=func;
 new->n=3;
 new->fn=fn;
 new->args[0]=pass1; new->args[1]=pass2; new->args[2]=pass3;
 return new;
 }

FN *fn4(func,fn,pass1,pass2,pass3,pass4)
int (*func)();
FN *fn;
long pass1,pass2,pass3,pass4;
 {
 FN *new=alfn();
 new->func=func;
 new->n=3;
 new->fn=fn;
 new->args[0]=pass1; new->args[1]=pass2; new->args[2]=pass3;
 new->args[3]=pass4;
 return new;
 }

FN *fn5(func,fn,pass1,pass2,pass3,pass4,pass5)
int (*func)();
FN *fn;
long pass1,pass2,pass3,pass4,pass5;
 {
 FN *new=alfn();
 new->func=func;
 new->n=3;
 new->fn=fn;
 new->args[0]=pass1; new->args[1]=pass2; new->args[2]=pass3;
 new->args[3]=pass4; new->args[4]=pass5;
 return new;
 }

int doexec0(fn,stat)
FN *fn;
 {
 int (*func)();
 FN *next;
 long pass1,pass2,pass3,pass4,pass5;
 if(!fn) return stat;
 func=fn->func;
 next=fn->fn;
 switch(fn->n)
  {
  case 5: pass5=fn->args[4];
  case 4: pass4=fn->args[3];
  case 3: pass3=fn->args[2];
  case 2: pass2=fn->args[1];
  case 1: pass1=fn->args[0];
  }
 frfn(fn);
 switch(fn->n)
  {
  case 0: return func(next,stat);
  case 1: return func(next,stat,pass1);
  case 2: return func(next,stat,pass1,pass2);
  case 3: return func(next,stat,pass1,pass2,pass3);
  case 4: return func(next,stat,pass1,pass2,pass3,pass4);
  case 5: return func(next,stat,pass1,pass2,pass3,pass4,pass5);
  }
 }

int doexec1(fn,stat,arg1)
FN *fn;
long arg1;
 {
 int (*func)();
 FN *next;
 long pass1,pass2,pass3,pass4,pass5;
 if(!fn) return stat;
 func=fn->func;
 next=fn->fn;
 switch(fn->n)
  {
  case 5: pass5=fn->args[4];
  case 4: pass4=fn->args[3];
  case 3: pass3=fn->args[2];
  case 2: pass2=fn->args[1];
  case 1: pass1=fn->args[0];
  }
 frfn(fn);
 switch(fn->n)
  {
  case 0: return func(next,stat,arg1);
  case 1: return func(next,stat,pass1,arg1);
  case 2: return func(next,stat,pass1,pass2,arg1);
  case 3: return func(next,stat,pass1,pass2,pass3,arg1);
  case 4: return func(next,stat,pass1,pass2,pass3,pass4,arg1);
  case 5: return func(next,stat,pass1,pass2,pass3,pass4,pass5,arg1);
  }
 }

int doexec2(fn,stat,arg1,arg2)
FN *fn;
long arg1,arg2;
 {
 int (*func)();
 FN *next;
 long pass1,pass2,pass3,pass4,pass5;
 if(!fn) return stat;
 func=fn->func;
 next=fn->fn;
 switch(fn->n)
  {
  case 5: pass5=fn->args[4];
  case 4: pass4=fn->args[3];
  case 3: pass3=fn->args[2];
  case 2: pass2=fn->args[1];
  case 1: pass1=fn->args[0];
  }
 frfn(fn);
 switch(fn->n)
  {
  case 0: return func(next,stat,arg1,arg2);
  case 1: return func(next,stat,pass1,arg1,arg2);
  case 2: return func(next,stat,pass1,pass2,arg1,arg2);
  case 3: return func(next,stat,pass1,pass2,pass3,arg1,arg2);
  case 4: return func(next,stat,pass1,pass2,pass3,pass4,arg1,arg2);
  case 5: return func(next,stat,pass1,pass2,pass3,pass4,pass5,arg1,arg2);
  }
 }

int doexec3(fn,stat,arg1,arg2,arg3)
FN *fn;
long arg1,arg2,arg3;
 {
 int (*func)();
 FN *next;
 long pass1,pass2,pass3,pass4,pass5;
 if(!fn) return stat;
 func=fn->func;
 next=fn->fn;
 switch(fn->n)
  {
  case 5: pass5=fn->args[4];
  case 4: pass4=fn->args[3];
  case 3: pass3=fn->args[2];
  case 2: pass2=fn->args[1];
  case 1: pass1=fn->args[0];
  }
 frfn(fn);
 switch(fn->n)
  {
  case 0: return func(next,stat,arg1,arg2,arg3);
  case 1: return func(next,stat,pass1,arg1,arg2,arg3);
  case 2: return func(next,stat,pass1,pass2,arg1,arg2,arg3);
  case 3: return func(next,stat,pass1,pass2,pass3,arg1,arg2,arg3);
  case 4: return func(next,stat,pass1,pass2,pass3,pass4,arg1,arg2,arg3);
  case 5: return func(next,stat,pass1,pass2,pass3,pass4,pass5,arg1,arg2,arg3);
  }
 }

int doexec4(fn,stat,arg1,arg2,arg3,arg4)
FN *fn;
long arg1,arg2,arg3,arg4;
 {
 int (*func)();
 FN *next;
 long pass1,pass2,pass3,pass4,pass5;
 if(!fn) return stat;
 func=fn->func;
 next=fn->fn;
 switch(fn->n)
  {
  case 5: pass5=fn->args[4];
  case 4: pass4=fn->args[3];
  case 3: pass3=fn->args[2];
  case 2: pass2=fn->args[1];
  case 1: pass1=fn->args[0];
  }
 frfn(fn);
 switch(fn->n)
  {
  case 0: return func(next,stat,arg1,arg2,arg3,arg4);
  case 1: return func(next,stat,pass1,arg1,arg2,arg3,arg4);
  case 2: return func(next,stat,pass1,pass2,arg1,arg2,arg3,arg4);
  case 3: return func(next,stat,pass1,pass2,pass3,arg1,arg2,arg3,arg4);
  case 4: return func(next,stat,pass1,pass2,pass3,pass4,arg1,arg2,arg3,arg4);
  case 5: return func(next,stat,pass1,pass2,pass3,pass4,pass5,arg1,arg2,arg3,
                      arg4);
  }
 }

int doexec5(fn,stat,arg1,arg2,arg3,arg4,arg5)
FN *fn;
long arg1,arg2,arg3,arg4,arg5;
 {
 int (*func)();
 FN *next;
 long pass1,pass2,pass3,pass4,pass5;
 if(!fn) return stat;
 func=fn->func;
 next=fn->fn;
 switch(fn->n)
  {
  case 5: pass5=fn->args[4];
  case 4: pass4=fn->args[3];
  case 3: pass3=fn->args[2];
  case 2: pass2=fn->args[1];
  case 1: pass1=fn->args[0];
  }
 frfn(fn);
 switch(fn->n)
  {
  case 0: return func(next,stat,arg1,arg2,arg3,arg4,arg5);
  case 1: return func(next,stat,pass1,arg1,arg2,arg3,arg4,arg5);
  case 2: return func(next,stat,pass1,pass2,arg1,arg2,arg3,arg4,arg5);
  case 3: return func(next,stat,pass1,pass2,pass3,arg1,arg2,arg3,arg4,arg5);
  case 4: return func(next,stat,pass1,pass2,pass3,pass4,arg1,arg2,arg3,arg4,
                      arg5);
  case 5: return func(next,stat,pass1,pass2,pass3,pass4,pass5,arg1,arg2,arg3,
                      arg4,arg5);
  }
 }
