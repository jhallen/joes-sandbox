/* Callback manager */

#include "mutex.h"
#include "task.h"

#define STACKSIZE 1048576			/* Stack size */

static struct stack
 {
 struct stack *next;
 jmp_buf stack;
 } *freestacks;					/* Free stacks */

static TASK waiting[1]={{waiting,waiting}};	/* Pending task queue */

static TASK timer[1]={{timer,timer}};		/* Timer task queue */

static TASK *thefn;				/* temp vars for go */
static int (*theio)();

static int stackop;	/* Stack allocation operation code */

/* cond for waiting on task queue */
thread_cond_t go_cond[1]={THREAD_COND_INITIALIZER};

/* All above variables are protected by this mutex */
mutex_t go_mutex[1]={MUTEX_INITIALIZER};

/* Function just copies its args- for saving and restore local variables */
static void *copyarg(arg)
void *arg;
 {
 return arg;
 }

/* Execute tasks... */

void go(io,fn)
int (*io)();
TASK *fn;
 {
 mutex_lock(go_mutex);

 if(fn) /* Suspend current task? */
  {
  if(setjmp(fn->jmpbuf)) /* Save tasks's stack, or continue task */
   { /* Continue a suspended task. */
   /* Get fn back */
   fn=copyarg(thefn);

   /* Copy args */
   switch(fn->n)
    {
    case 5: fn->argptr[4]=fn->args[4];
    case 4: fn->argptr[3]=fn->args[3];
    case 3: fn->argptr[2]=fn->args[2];
    case 2: fn->argptr[1]=fn->args[1];
    case 1: fn->argptr[0]=fn->args[0];
    }

   fn->state=0;

   /* Continue existing task (return from go) */
   mutex_unlock(go_mutex);
   return;
   }
  else
   { /* Suspend task */
   if(freestacks)
    { /* Switch to top stack on free list and enter task loop. */
    stackop=2;
    thefn=copyarg(fn);
    theio=copyarg(io);
    longjmp(freestacks->stack,1);
    }
   else
    { /* No free stacks: allocate a stack and enter task loop. */
    freestacks=malloc(sizeof(struct stack));
    freestacks->next=0;
    stackop=3;
    goto dostack;
    }
   }
  }

 /* Task loop */
 for(;;)
  {
  /* Wait for a task to become available. */
  while(QEMPTY(waiting))
   {
   int flg;
   /* Queue is empty.  Try to get some io(). */
   mutex_unlock(go_mutex);
   flg=io();
   mutex_lock(go_mutex);
   /* io() returns true if it may have gotten some I/O, or returns false if
    * other threads are already select()ing on every possible I/O source. */
   if(!flg)
    /* Wait for other threads to give us I/O */
    thread_cond_wait(go_cond,go_mutex);
   else
    /* Tell waiting threads that there may be some tasks to do */
    thread_cond_broadcast(go_cond);
   }

  /* Extract task from queue */
  fn=waiting->next;
  DEQUE(fn);

  /* Execute the task */
  if(fn->func)
   { /* It's a call-back function */

   mutex_unlock(go_mutex);

   fn->state=0;

   /* Execute the task */
   switch(fn->n)
    {
    case 0:
     fn->func(0);
     break;
    case 1:
     fn->func(0,fn->args[0]);
     break;
    case 2:
     fn->func(0,fn->args[0],fn->args[1]);
     break;
    case 3:
     fn->func(0,fn->args[0],fn->args[1],fn->args[2]);
     break;
    case 4:
     fn->func(0,fn->args[0],fn->args[1],fn->args[2],fn->args[3]);
     break;
    case 5:
     fn->func(0,fn->args[0],fn->args[1],fn->args[2],fn->args[3],fn->args[4]);
     break;
    case 6:
     fn->func(0,fn->args[0],fn->args[1],fn->args[2],fn->args[3],fn->args[4],
                fn->args[5]);
     break;
    case 7:
     fn->func(0,fn->args[0],fn->args[1],fn->args[2],fn->args[3],fn->args[4],
                fn->args[5],fn->args[6]);
     break;
    case 8:
     fn->func(0,fn->args[0],fn->args[1],fn->args[2],fn->args[3],fn->args[4],
                fn->args[5],fn->args[6],fn->args[7]);
     break;
    case 9:
     fn->func(0,fn->args[0],fn->args[1],fn->args[2],fn->args[3],fn->args[4],
                fn->args[5],fn->args[6],fn->args[7],fn->args[8]);
     break;
    case 10:
     fn->func(0,fn->args[0],fn->args[1],fn->args[2],fn->args[3],fn->args[4],
                fn->args[5],fn->args[6],fn->args[7],fn->args[8],fn->args[9]);
     break;
    }

   /* Loop: do next task */
   mutex_lock(go_mutex);
   }
  else
   { /* Continue a suspended task */
   /* Save this stack for reuse */
   struct stack *stack=malloc(sizeof(struct stack));
   stack->next=freestacks;
   freestacks=stack;
   stackop=1;

   /* Stack allocation entry point. */
   dostack:
   /* Save stack (or return to this point from longjmp) */
   setjmp(freestacks->stack);
   switch(stackop)
    {
    case 1: /* Continue a suspended task. */
     {
     thefn=copyarg(fn);
     longjmp(fn->jmpbuf,1);
     }

    case 2: /* We just switched to stack on freelist.  Free top structure */
     {
     stack=freestacks;
     freestacks=stack->next;
     fn=copyarg(thefn);
     io=copyarg(theio);
     break;
     }

    case 3: /* Allocate and switch to new stack. */
     {
     char *newbp;
     long diff;
     char *stk=malloc(STACKSIZE);	/* Malloc space for stack */

     /* printf("New stack at %8.8x\n",stk); */

     /* This is the hugely un-portable system-dependent part.  You have
        to look at jmp_buf in setjmp.h to see how the system names the stack
        and base pointers.  */
     /* Leave enough space on the stack for this function's variables */

     newbp=stk+STACKSIZE-256;

     /* printf("New bp at %8.8x\n",newbp); */

     freestacks->stack->__jmpbuf[3] = newbp;
#if 0
     					/* Leave space for this func */
#ifndef JB_BP

#ifndef __sparc

     /* i86 Linux: libc5 */

     diff=newbp-(char *)freestacks->stack->__bp;
     					/* Diff. between new and old */
     ((char *)freestacks->stack->__bp)+=diff;	/* Adjust SP and BP, */
     ((char *)freestacks->stack->__sp)+=diff;	/* but keep their relationship */

#else

     /* sparc solaris */
     diff=newbp-(char *)freestacks->stack[3];
     					/* Diff. between new and old */
     ((char *)freestacks->stack[3])+=diff;	/* Adjust SP and BP, */
     ((char *)freestacks->stack[1])+=diff;	/* but keep their relationship */

#endif

#else
     /* i86 Linux: gnu-lib */

     /* printf("Old bp at %8.8x\n",freestacks->stack->__jmpbuf[JB_BP]); */

     diff=newbp-(char *)freestacks->stack->__jmpbuf[JB_BP];
     					/* Diff. between new and old */
     ((char *)freestacks->stack->__jmpbuf[JB_BP])+=diff;	/* Adjust SP and BP, */
     ((char *)freestacks->stack->__jmpbuf[JB_SP])+=diff;	/* but keep their relationship */

     /* printf("New bp at %8.8x\n",freestacks->stack->__jmpbuf[JB_BP]); */

#endif

#endif

     /* Switch to newly allocated stack */
     stackop=2;
     thefn=copyarg(fn);
     theio=copyarg(io);
     longjmp(freestacks->stack,1);
     }
     break;
    }
   }
  }
 }

TASK *cfn(fn,argptr)
TASK *fn;
int *argptr;
 {
 fn->func=0;
 fn->n=0;
 fn->argptr=argptr;
 fn->state=1;
 return fn;
 }

TASK *fn0(fn,func)
TASK *fn;
void (*func)();
 {
 fn->func=func;
 fn->argptr=0;
 fn->n=0;
 fn->state=1;
 return fn;
 }

TASK *fn1(fn,func,pass1)
TASK *fn;
void (*func)();
int pass1;
 {
 fn->func=func;
 fn->argptr=0;
 fn->n=1;
 fn->args[0]=pass1;
 fn->state=1;
 return fn;
 }

TASK *fn2(fn,func,pass1,pass2)
TASK *fn;
void (*func)();
int pass1,pass2;
 {
 fn->func=func;
 fn->argptr=0;
 fn->n=2;
 fn->args[0]=pass1; fn->args[1]=pass2;
 fn->state=1;
 return fn;
 }

TASK *fn3(fn,func,pass1,pass2,pass3)
TASK *fn;
void (*func)();
int pass1,pass2,pass3;
 {
 fn->func=func;
 fn->argptr=0;
 fn->n=3;
 fn->args[0]=pass1; fn->args[1]=pass2; fn->args[2]=pass3;
 fn->state=1;
 return fn;
 }

TASK *fn4(fn,func,pass1,pass2,pass3,pass4)
TASK *fn;
void (*func)();
int pass1,pass2,pass3,pass4;
 {
 fn->func=func;
 fn->argptr=0;
 fn->n=4;
 fn->args[0]=pass1; fn->args[1]=pass2; fn->args[2]=pass3;
 fn->args[3]=pass4;
 fn->state=1;
 return fn;
 }

TASK *fn5(fn,func,pass1,pass2,pass3,pass4,pass5)
TASK *fn;
void (*func)();
int pass1,pass2,pass3,pass4,pass5;
 {
 fn->func=func;
 fn->argptr=0;
 fn->n=5;
 fn->args[0]=pass1; fn->args[1]=pass2; fn->args[2]=pass3;
 fn->args[3]=pass4; fn->args[4]=pass5;
 fn->state=1;
 return fn;
 }

void cont0(fn)
TASK *fn;
 {
 if(!fn) return;
 if(!fn->state) return;
 mutex_lock(go_mutex);
 fn->state=3;
 ENQUEB(waiting,fn);
 mutex_unlock(go_mutex);
 }

void cont1(fn,arg1)
TASK *fn;
 {
 if(!fn) return;
 if(!fn->state) return;
 fn->args[fn->n++]=arg1;
 mutex_lock(go_mutex);
 fn->state=3;
 ENQUEB(waiting,fn);
 mutex_unlock(go_mutex);
 }

void cont2(fn,arg1,arg2)
TASK *fn;
 {
 if(!fn) return;
 if(!fn->state) return;
 fn->args[fn->n++]=arg1;
 fn->args[fn->n++]=arg2;
 mutex_lock(go_mutex);
 fn->state=3;
 ENQUEB(waiting,fn);
 mutex_unlock(go_mutex);
 }

void cont3(fn,arg1,arg2,arg3)
TASK *fn;
 {
 if(!fn) return;
 if(!fn->state) return;
 fn->args[fn->n++]=arg1;
 fn->args[fn->n++]=arg2;
 fn->args[fn->n++]=arg3;
 mutex_lock(go_mutex);
 fn->state=3;
 ENQUEB(waiting,fn);
 mutex_unlock(go_mutex);
 }

void cont4(fn,arg1,arg2,arg3,arg4)
TASK *fn;
 {
 if(!fn) return;
 if(!fn->state) return;
 fn->args[fn->n++]=arg1;
 fn->args[fn->n++]=arg2;
 fn->args[fn->n++]=arg3;
 fn->args[fn->n++]=arg4;
 mutex_lock(go_mutex);
 fn->state=3;
 ENQUEB(waiting,fn);
 mutex_unlock(go_mutex);
 }

void cont5(fn,arg1,arg2,arg3,arg4,arg5)
TASK *fn;
 {
 if(!fn) return;
 if(!fn->state) return;
 fn->args[fn->n++]=arg1;
 fn->args[fn->n++]=arg2;
 fn->args[fn->n++]=arg3;
 fn->args[fn->n++]=arg4;
 fn->args[fn->n++]=arg5;
 mutex_lock(go_mutex);
 fn->state=3;
 ENQUEB(waiting,fn);
 mutex_unlock(go_mutex);
 }

void submit(usecs,fn)
TASK *fn;
 {
 TASK *u;
 fn->usecs=usecs;
 mutex_lock(go_mutex);
 fn->state=2;
 for(u=timer;u->next!=timer && usecs>u->next->usecs;u=u->next);
 ENQUEF(u,fn);
 mutex_unlock(go_mutex);
 }

void note(usecs)
 {
 TASK *t;
 for(t=timer->next;t!=timer;t=t->next)
  if(t->usecs>0) t->usecs-=usecs;
 while(timer->next!=timer && timer->next->usecs<=0)
  {
  t=timer->next;
  t->state=3;
  DEMOTE(waiting,t);
  }
 }

/* How long to wait for next event */

int waittim()
 {
 int tim= -1;
 if(timer->next!=timer) tim=timer->next->usecs;
 return tim;
 }

void cancel(fn)
TASK *fn;
 {
 if(!fn) return;
 if(!fn->state) return;
 mutex_lock(go_mutex);
 if(fn->state>1) DEQUE(fn);
 fn->state=0;
 mutex_unlock(go_mutex);
 switch(fn->n)
  {
  case 0: fn->func(-1); break;
  case 1: fn->func(-1,fn->args[0]); break;
  case 2: fn->func(-1,fn->args[0],fn->args[1]); break;
  case 3: fn->func(-1,fn->args[0],fn->args[1],fn->args[2]); break;
  case 4: fn->func(-1,fn->args[0],fn->args[1],fn->args[2],fn->args[3]); break;
  case 5: fn->func(-1,fn->args[0],fn->args[1],fn->args[2],fn->args[3],fn->args[4]);
  }
 }

int anyfns()
 {
 if(!qempty(waiting)) return 1;
 else return 0;
 }

void iztask(t)
TASK *t;
 {
 IZQUE(t);
 t->state=0;
 }
