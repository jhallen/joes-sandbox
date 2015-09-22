/* Callback manager */

#include "mutex.h"
#include "task.h"

#define STACKSIZE 1048576			/* Stack size */

static struct stack
 {
 struct stack *next;
 jmp_buf stack;
 } *freestacks;					/* Free stacks */

static Task waiting[1]={ { waiting, waiting } };	/* Pending task queue */

static Task timer[1]= { { timer, timer } };		/* Timer task queue */

static Task *thefn;				/* temp vars for go */
static int (*theio)();

static int stackop;	/* Stack allocation operation code */

/* cond for waiting on task queue */
thread_cond_t go_cond[1] = { THREAD_COND_INITIALIZER };

/* All above variables are protected by this mutex */
mutex_t go_mutex[1] = { MUTEX_INITIALIZER };

/* Function just copies its args- for saving and restore local variables */
static void *copyarg(void *arg)
 {
 return arg;
 }

/* Execute tasks... */

void go(int (*io)(void),Task *fn)
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
    case 5: fn->argptr[4] = fn->args[4];
    case 4: fn->argptr[3] = fn->args[3];
    case 3: fn->argptr[2] = fn->args[2];
    case 2: fn->argptr[1] = fn->args[1];
    case 1: fn->argptr[0] = fn->args[0];
    }

   fn->state = 0;

   /* Continue existing task (return from go) */
   mutex_unlock(go_mutex);
   return;
   }
  else
   { /* Suspend task */
   if(freestacks)
    { /* Switch to top stack on free list and enter task loop. */
    stackop = 2;
    thefn = copyarg(fn);
    theio = copyarg(io);
    longjmp(freestacks->stack, 1);
    }
   else
    { /* No free stacks: allocate a stack and enter task loop. */
    freestacks = (struct stack *)malloc(sizeof(struct stack));
    freestacks->next = 0;
    stackop = 3;
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
    thread_cond_wait(go_cond, go_mutex);
   else
    /* Tell waiting threads that there may be some tasks to do */
    thread_cond_broadcast(go_cond);
   }

  /* Extract task from queue */
  fn = waiting->next;
  DEQUE(fn);

  /* Execute the task */
  if(fn->func.f0)
   { /* It's a call-back function */

   mutex_unlock(go_mutex);

   fn->state = 0;

   /* Execute the task */
   switch(fn->n)
    {
    case 0:
     fn->func.f0(0);
     break;
    case 1:
     fn->func.f1(0, fn->args[0]);
     break;
    case 2:
     fn->func.f2(0, fn->args[0], fn->args[1]);
     break;
    case 3:
     fn->func.f3(0, fn->args[0], fn->args[1], fn->args[2]);
     break;
    case 4:
     fn->func.f4(0, fn->args[0], fn->args[1], fn->args[2], fn->args[3]);
     break;
    case 5:
     fn->func.f5(0, fn->args[0], fn->args[1], fn->args[2], fn->args[3], fn->args[4]);
     break;
    case 6:
     fn->func.f6(0, fn->args[0], fn->args[1], fn->args[2], fn->args[3], fn->args[4], fn->args[5]);
     break;
    case 7:
     fn->func.f7(0, fn->args[0], fn->args[1], fn->args[2], fn->args[3], fn->args[4], fn->args[5], fn->args[6]);
     break;
    case 8:
     fn->func.f8(0, fn->args[0], fn->args[1], fn->args[2], fn->args[3], fn->args[4], fn->args[5], fn->args[6], fn->args[7]);
     break;
    case 9:
     fn->func.f9(0, fn->args[0], fn->args[1], fn->args[2], fn->args[3], fn->args[4], fn->args[5], fn->args[6], fn->args[7], fn->args[8]);
     break;
    case 10:
     fn->func.f10(0, fn->args[0], fn->args[1], fn->args[2], fn->args[3], fn->args[4], fn->args[5], fn->args[6], fn->args[7], fn->args[8], fn->args[9]);
     break;
    }

   /* Loop: do next task */
   mutex_lock(go_mutex);
   }
  else
   { /* Continue a suspended task */
   /* Save this stack for reuse */
   struct stack *stack = (struct stack *)malloc(sizeof(struct stack));
   stack->next = freestacks;
   freestacks = stack;
   stackop = 1;

   /* Stack allocation entry point. */
   dostack:
   /* Save stack (or return to this point from longjmp) */
   setjmp(freestacks->stack);
   switch(stackop)
    {
    case 1: /* Continue a suspended task. */
     {
     thefn = copyarg(fn);
     longjmp(fn->jmpbuf, 1);
     }

    case 2: /* We just switched to stack on freelist.  Free top structure */
     {
     stack = freestacks;
     freestacks = stack->next;
     fn = copyarg(thefn);
     io = copyarg(theio);
     break;
     }

    case 3: /* Allocate and switch to new stack. */
     {
     char *newbp;
     char *stk = (char *)malloc(STACKSIZE);	/* Malloc space for stack */

     /* printf("New stack at %8.8x\n",stk); */

     /* This is the hugely un-portable system-dependent part.  You have
        to look at jmp_buf in setjmp.h to see how the system names the stack
        and base pointers.  */
     /* Leave enough space on the stack for this function's variables */

     newbp = stk + STACKSIZE - 256;

     /* printf("New bp at %8.8x\n",newbp); */

     freestacks->stack->__jmpbuf[3] = newbp;
#if 0
     					/* Leave space for this func */
#ifndef JB_BP

#ifndef __sparc

     /* i86 Linux: libc5 */

     diff = newbp-(char *)freestacks->stack->__bp;
     					/* Diff. between new and old */
     ((char *)freestacks->stack->__bp) += diff;	/* Adjust SP and BP, */
     ((char *)freestacks->stack->__sp) += diff;	/* but keep their relationship */

#else

     /* sparc solaris */
     diff = newbp - (char *)freestacks->stack[3];
     					/* Diff. between new and old */
     ((char *)freestacks->stack[3]) += diff;	/* Adjust SP and BP, */
     ((char *)freestacks->stack[1]) += diff;	/* but keep their relationship */

#endif

#else
     /* i86 Linux: gnu-lib */

     /* printf("Old bp at %8.8x\n",freestacks->stack->__jmpbuf[JB_BP]); */

     diff = newbp - (char *)freestacks->stack->__jmpbuf[JB_BP];
     					/* Diff. between new and old */
     ((char *)freestacks->stack->__jmpbuf[JB_BP]) += diff;	/* Adjust SP and BP, */
     ((char *)freestacks->stack->__jmpbuf[JB_SP]) += diff;	/* but keep their relationship */

     /* printf("New bp at %8.8x\n",freestacks->stack->__jmpbuf[JB_BP]); */

#endif

#endif

     /* Switch to newly allocated stack */
     stackop = 2;
     thefn = copyarg(fn);
     theio = copyarg(io);
     longjmp(freestacks->stack, 1);
     }
     break;
    }
   }
  }
 }

Task *cfn(Task *fn,Arg *argptr,int nargs)
 {
 fn->func.f0=0;
 fn->n=0;
 fn->nargs=nargs;
 fn->argptr=argptr;
 fn->state=1;
 return fn;
 }

Task *fn0_0(Task *fn,void (*func)(int))
 {
 fn->func.f0 = func;
 fn->argptr = 0;
 fn->n = 0;
 fn->nargs = 0;
 fn->state = 1;
 return fn;
 }

Task *fn0_1(Task *fn,void (*func)(int, Arg argv))
 {
 fn->func.f1 = func;
 fn->argptr = 0;
 fn->n = 0;
 fn->args[0].p = NULL;
 fn->nargs = 1;
 fn->state = 1;
 return fn;
 }

Task *fn0_2(Task *fn,void (*func)(int, Arg argv, Arg argw))
 {
 fn->func.f2 = func;
 fn->argptr = 0;
 fn->n = 0;
 fn->args[0].p = NULL;
 fn->args[1].p = NULL;
 fn->nargs = 2;
 fn->state = 1;
 return fn;
 }

Task *fn0_3(Task *fn,void (*func)(int, Arg argv, Arg argw, Arg argx))
 {
 fn->func.f3 = func;
 fn->argptr = 0;
 fn->n = 0;
 fn->args[0].p = NULL;
 fn->args[1].p = NULL;
 fn->args[2].p = NULL;
 fn->nargs = 3;
 fn->state = 1;
 return fn;
 }

Task *fn0_4(Task *fn,void (*func)(int, Arg argv, Arg argw, Arg argx, Arg argy))
 {
 fn->func.f4 = func;
 fn->argptr = 0;
 fn->n = 0;
 fn->args[0].p = NULL;
 fn->args[1].p = NULL;
 fn->args[2].p = NULL;
 fn->args[3].p = NULL;
 fn->nargs = 4;
 fn->state = 1;
 return fn;
 }

Task *fn0_5(Task *fn,void (*func)(int, Arg argv, Arg argw, Arg argx, Arg argy, Arg argz))
 {
 fn->func.f5 = func;
 fn->argptr = 0;
 fn->n = 0;
 fn->args[0].p = NULL;
 fn->args[1].p = NULL;
 fn->args[2].p = NULL;
 fn->args[3].p = NULL;
 fn->args[4].p = NULL;
 fn->nargs = 5;
 fn->state = 1;
 return fn;
 }

Task *fn1_0(Task *fn,void (*func)(int, Arg arga), Arg arga)
 {
 fn->func.f1 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->n = 1;
 fn->nargs = 1;
 fn->state = 1;
 return fn;
 }

Task *fn1_1(Task *fn,void (*func)(int, Arg arga, Arg argv), Arg arga)
 {
 fn->func.f2 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->n = 1;
 fn->args[1].p = NULL;
 fn->nargs = 2;
 fn->state = 1;
 return fn;
 }

Task *fn1_2(Task *fn,void (*func)(int, Arg arga, Arg argv, Arg argw), Arg arga)
 {
 fn->func.f3 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->n = 1;
 fn->args[1].p = NULL;
 fn->args[2].p = NULL;
 fn->nargs = 3;
 fn->state = 1;
 return fn;
 }

Task *fn1_3(Task *fn,void (*func)(int, Arg arga, Arg argv, Arg argw, Arg argx), Arg arga)
 {
 fn->func.f4 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->n = 1;
 fn->args[1].p = NULL;
 fn->args[2].p = NULL;
 fn->args[3].p = NULL;
 fn->nargs = 4;
 fn->state = 1;
 return fn;
 }

Task *fn1_4(Task *fn,void (*func)(int, Arg arga, Arg argv, Arg argw, Arg argx, Arg argy), Arg arga)
 {
 fn->func.f5 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->n = 1;
 fn->args[1].p = NULL;
 fn->args[2].p = NULL;
 fn->args[3].p = NULL;
 fn->args[4].p = NULL;
 fn->nargs = 5;
 fn->state = 1;
 return fn;
 }

Task *fn1_5(Task *fn,void (*func)(int, Arg arga, Arg argv, Arg argw, Arg argx, Arg argy, Arg argz), Arg arga)
 {
 fn->func.f6 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->n = 1;
 fn->args[1].p = NULL;
 fn->args[2].p = NULL;
 fn->args[3].p = NULL;
 fn->args[4].p = NULL;
 fn->args[5].p = NULL;
 fn->nargs = 6;
 fn->state = 1;
 return fn;
 }

Task *fn2_0(Task *fn,void (*func)(int, Arg arga, Arg argb), Arg arga, Arg argb)
 {
 fn->func.f2 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->args[1] = argb;
 fn->n = 2;
 fn->nargs = 2;
 fn->state = 1;
 return fn;
 }

Task *fn2_1(Task *fn,void (*func)(int, Arg arga, Arg argb, Arg argv), Arg arga, Arg argb)
 {
 fn->func.f3 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->args[1] = argb;
 fn->n = 2;
 fn->args[2].p = NULL;
 fn->nargs = 3;
 fn->state = 1;
 return fn;
 }

Task *fn2_2(Task *fn,void (*func)(int, Arg arga, Arg argb, Arg argv, Arg argw), Arg arga, Arg argb)
 {
 fn->func.f4 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->args[1] = argb;
 fn->n = 2;
 fn->args[2].p = NULL;
 fn->args[3].p = NULL;
 fn->nargs = 4;
 fn->state = 1;
 return fn;
 }

Task *fn2_3(Task *fn,void (*func)(int, Arg arga, Arg argb, Arg argv, Arg argw, Arg argx), Arg arga, Arg argb)
 {
 fn->func.f5 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->args[1] = argb;
 fn->n = 2;
 fn->args[2].p = NULL;
 fn->args[3].p = NULL;
 fn->args[4].p = NULL;
 fn->nargs = 5;
 fn->state = 1;
 return fn;
 }

Task *fn2_4(Task *fn,void (*func)(int, Arg arga, Arg argb, Arg argv, Arg argw, Arg argx, Arg argy), Arg arga, Arg argb)
 {
 fn->func.f6 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->args[1] = argb;
 fn->n = 2;
 fn->args[2].p = NULL;
 fn->args[3].p = NULL;
 fn->args[4].p = NULL;
 fn->args[5].p = NULL;
 fn->nargs = 6;
 fn->state = 1;
 return fn;
 }

Task *fn2_5(Task *fn,void (*func)(int, Arg arga, Arg argb, Arg argv, Arg argw, Arg argx, Arg argy, Arg argz), Arg arga, Arg argb)
 {
 fn->func.f7 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->args[1] = argb;
 fn->n = 2;
 fn->args[2].p = NULL;
 fn->args[3].p = NULL;
 fn->args[4].p = NULL;
 fn->args[5].p = NULL;
 fn->args[6].p = NULL;
 fn->nargs = 7;
 fn->state = 1;
 return fn;
 }

Task *fn3_0(Task *fn,void (*func)(int, Arg arga, Arg argb, Arg argc), Arg arga, Arg argb, Arg argc)
 {
 fn->func.f3 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->args[1] = argb;
 fn->args[2] = argc;
 fn->n = 3;
 fn->nargs = 3;
 fn->state = 1;
 return fn;
 }

Task *fn3_1(Task *fn,void (*func)(int, Arg arga, Arg argb, Arg argc, Arg argv), Arg arga, Arg argb, Arg argc)
 {
 fn->func.f4 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->args[1] = argb;
 fn->args[2] = argc;
 fn->n = 3;
 fn->args[3].p = NULL;
 fn->nargs = 4;
 fn->state = 1;
 return fn;
 }

Task *fn3_2(Task *fn,void (*func)(int, Arg arga, Arg argb, Arg argc, Arg argv, Arg argw), Arg arga, Arg argb, Arg argc)
 {
 fn->func.f5 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->args[1] = argb;
 fn->args[2] = argc;
 fn->n = 3;
 fn->args[3].p = NULL;
 fn->args[4].p = NULL;
 fn->nargs = 5;
 fn->state = 1;
 return fn;
 }

Task *fn3_3(Task *fn,void (*func)(int, Arg arga, Arg argb, Arg argc, Arg argv, Arg argw, Arg argx), Arg arga, Arg argb, Arg argc)
 {
 fn->func.f6 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->args[1] = argb;
 fn->args[2] = argc;
 fn->n = 3;
 fn->args[3].p = NULL;
 fn->args[4].p = NULL;
 fn->args[5].p = NULL;
 fn->nargs = 6;
 fn->state = 1;
 return fn;
 }

Task *fn3_4(Task *fn,void (*func)(int, Arg arga, Arg argb, Arg argc, Arg argv, Arg argw, Arg argx, Arg argy), Arg arga, Arg argb, Arg argc)
 {
 fn->func.f7 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->args[1] = argb;
 fn->args[2] = argc;
 fn->n = 3;
 fn->args[3].p = NULL;
 fn->args[4].p = NULL;
 fn->args[5].p = NULL;
 fn->args[6].p = NULL;
 fn->nargs = 7;
 fn->state = 1;
 return fn;
 }

Task *fn3_5(Task *fn,void (*func)(int, Arg arga, Arg argb, Arg argc, Arg argv, Arg argw, Arg argx, Arg argy, Arg argz), Arg arga, Arg argb, Arg argc)
 {
 fn->func.f8 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->args[1] = argb;
 fn->args[2] = argc;
 fn->n = 3;
 fn->args[3].p = NULL;
 fn->args[4].p = NULL;
 fn->args[5].p = NULL;
 fn->args[6].p = NULL;
 fn->args[7].p = NULL;
 fn->nargs = 8;
 fn->state = 1;
 return fn;
 }

Task *fn4_0(Task *fn,void (*func)(int, Arg arga, Arg argb, Arg argc, Arg argd), Arg arga, Arg argb, Arg argc, Arg argd)
 {
 fn->func.f4 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->args[1] = argb;
 fn->args[2] = argc;
 fn->args[3] = argd;
 fn->n = 4;
 fn->nargs = 4;
 fn->state = 1;
 return fn;
 }

Task *fn4_1(Task *fn,void (*func)(int, Arg arga, Arg argb, Arg argc, Arg argd, Arg argv), Arg arga, Arg argb, Arg argc, Arg argd)
 {
 fn->func.f5 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->args[1] = argb;
 fn->args[2] = argc;
 fn->args[3] = argd;
 fn->n = 4;
 fn->args[4].p = NULL;
 fn->nargs = 5;
 fn->state = 1;
 return fn;
 }

Task *fn4_2(Task *fn,void (*func)(int, Arg arga, Arg argb, Arg argc, Arg argd, Arg argv, Arg argw), Arg arga, Arg argb, Arg argc, Arg argd)
 {
 fn->func.f6 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->args[1] = argb;
 fn->args[2] = argc;
 fn->args[3] = argd;
 fn->n = 4;
 fn->args[4].p = NULL;
 fn->args[5].p = NULL;
 fn->nargs = 6;
 fn->state = 1;
 return fn;
 }

Task *fn4_3(Task *fn,void (*func)(int, Arg arga, Arg argb, Arg argc, Arg argd, Arg argv, Arg argw, Arg argx), Arg arga, Arg argb, Arg argc, Arg argd)
 {
 fn->func.f7 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->args[1] = argb;
 fn->args[2] = argc;
 fn->args[3] = argd;
 fn->n = 4;
 fn->args[4].p = NULL;
 fn->args[5].p = NULL;
 fn->args[6].p = NULL;
 fn->nargs = 7;
 fn->state = 1;
 return fn;
 }

Task *fn4_4(Task *fn,void (*func)(int, Arg arga, Arg argb, Arg argc, Arg argd, Arg argv, Arg argw, Arg argx, Arg argy), Arg arga, Arg argb, Arg argc, Arg argd)
 {
 fn->func.f8 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->args[1] = argb;
 fn->args[2] = argc;
 fn->args[3] = argd;
 fn->n = 4;
 fn->args[4].p = NULL;
 fn->args[5].p = NULL;
 fn->args[6].p = NULL;
 fn->args[7].p = NULL;
 fn->nargs = 8;
 fn->state = 1;
 return fn;
 }

Task *fn4_5(Task *fn,void (*func)(int, Arg arga, Arg argb, Arg argc, Arg argd, Arg argv, Arg argw, Arg argx, Arg argy, Arg argz), Arg arga, Arg argb, Arg argc, Arg argd)
 {
 fn->func.f9 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->args[1] = argb;
 fn->args[2] = argc;
 fn->args[3] = argd;
 fn->n = 4;
 fn->args[4].p = NULL;
 fn->args[5].p = NULL;
 fn->args[6].p = NULL;
 fn->args[7].p = NULL;
 fn->args[8].p = NULL;
 fn->nargs = 9;
 fn->state = 1;
 return fn;
 }

Task *fn5_0(Task *fn,void (*func)(int, Arg arga, Arg argb, Arg argc, Arg argd, Arg arge), Arg arga, Arg argb, Arg argc, Arg argd, Arg arge)
 {
 fn->func.f5 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->args[1] = argb;
 fn->args[2] = argc;
 fn->args[3] = argd;
 fn->args[4] = arge;
 fn->n = 5;
 fn->nargs = 5;
 fn->state = 1;
 return fn;
 }

Task *fn5_1(Task *fn,void (*func)(int, Arg arga, Arg argb, Arg argc, Arg argd, Arg arge, Arg argv), Arg arga, Arg argb, Arg argc, Arg argd, Arg arge)
 {
 fn->func.f6 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->args[1] = argb;
 fn->args[2] = argc;
 fn->args[3] = argd;
 fn->args[4] = arge;
 fn->n = 5;
 fn->args[5].p = NULL;
 fn->nargs = 6;
 fn->state = 1;
 return fn;
 }

Task *fn5_2(Task *fn,void (*func)(int, Arg arga, Arg argb, Arg argc, Arg argd, Arg arge, Arg argv, Arg argw), Arg arga, Arg argb, Arg argc, Arg argd, Arg arge)
 {
 fn->func.f7 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->args[1] = argb;
 fn->args[2] = argc;
 fn->args[3] = argd;
 fn->args[4] = arge;
 fn->n = 5;
 fn->args[5].p = NULL;
 fn->args[6].p = NULL;
 fn->nargs = 7;
 fn->state = 1;
 return fn;
 }

Task *fn5_3(Task *fn,void (*func)(int, Arg arga, Arg argb, Arg argc, Arg argd, Arg arge, Arg argv, Arg argw, Arg argx), Arg arga, Arg argb, Arg argc, Arg argd, Arg arge)
 {
 fn->func.f8 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->args[1] = argb;
 fn->args[2] = argc;
 fn->args[3] = argd;
 fn->args[4] = arge;
 fn->n = 5;
 fn->args[5].p = NULL;
 fn->args[6].p = NULL;
 fn->args[7].p = NULL;
 fn->nargs = 8;
 fn->state = 1;
 return fn;
 }

Task *fn5_4(Task *fn,void (*func)(int, Arg arga, Arg argb, Arg argc, Arg argd, Arg arge, Arg argv, Arg argw, Arg argx, Arg argy), Arg arga, Arg argb, Arg argc, Arg argd, Arg arge)
 {
 fn->func.f9 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->args[1] = argb;
 fn->args[2] = argc;
 fn->args[3] = argd;
 fn->args[4] = arge;
 fn->n = 5;
 fn->args[5].p = NULL;
 fn->args[6].p = NULL;
 fn->args[7].p = NULL;
 fn->args[8].p = NULL;
 fn->nargs = 9;
 fn->state = 1;
 return fn;
 }

Task *fn5_5(Task *fn,void (*func)(int, Arg arga, Arg argb, Arg argc, Arg argd, Arg arge, Arg argv, Arg argw, Arg argx, Arg argy, Arg argz), Arg arga, Arg argb, Arg argc, Arg argd, Arg arge)
 {
 fn->func.f10 = func;
 fn->argptr = 0;
 fn->args[0] = arga;
 fn->args[1] = argb;
 fn->args[2] = argc;
 fn->args[3] = argd;
 fn->args[4] = arge;
 fn->n = 5;
 fn->args[5].p = NULL;
 fn->args[6].p = NULL;
 fn->args[7].p = NULL;
 fn->args[8].p = NULL;
 fn->args[9].p = NULL;
 fn->nargs = 10;
 fn->state = 1;
 return fn;
 }

void cont0(Task *fn)
 {
 if(!fn) return;
 if(!fn->state) return;
 mutex_lock(go_mutex);
 fn->state=3;
 ENQUEB(waiting,fn);
 mutex_unlock(go_mutex);
 }

void cont1(Task *fn,Arg arg1)
 {
 if(!fn) return;
 if(!fn->state) return;
 fn->args[fn->n++]=arg1;
 mutex_lock(go_mutex);
 fn->state=3;
 ENQUEB(waiting,fn);
 mutex_unlock(go_mutex);
 }

void cont2(Task *fn,Arg arg1,Arg arg2)
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

void cont3(Task *fn,Arg arg1,Arg arg2,Arg arg3)
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

void cont4(Task *fn,Arg arg1,Arg arg2,Arg arg3,Arg arg4)
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

void cont5(Task *fn,Arg arg1,Arg arg2,Arg arg3,Arg arg4,Arg arg5)
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

void submit(int usecs,Task *fn)
 {
 Task *u;
 fn->usecs=usecs;
 mutex_lock(go_mutex);
 fn->state=2;
 for(u=timer;u->next!=timer && usecs>u->next->usecs;u=u->next);
 ENQUEF(u,fn);
 mutex_unlock(go_mutex);
 }

void note(int usecs)
 {
 Task *t;
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

void cancel(Task *fn)
 {
 if(!fn) return;
 if(!fn->state) return;
 mutex_lock(go_mutex);
 if(fn->state>1) DEQUE(fn);
 fn->state=0;
 mutex_unlock(go_mutex);
 switch(fn->nargs)
  {
  case 0: fn->func.f0(-1); break;
  case 1: fn->func.f1(-1,fn->args[0]); break;
  case 2: fn->func.f2(-1,fn->args[0],fn->args[1]); break;
  case 3: fn->func.f3(-1,fn->args[0],fn->args[1],fn->args[2]); break;
  case 4: fn->func.f4(-1,fn->args[0],fn->args[1],fn->args[2],fn->args[3]); break;
  case 5: fn->func.f5(-1,fn->args[0],fn->args[1],fn->args[3],fn->args[4],fn->args[5]); break;
  case 6: fn->func.f6(-1,fn->args[0],fn->args[1],fn->args[2],fn->args[3],fn->args[4],fn->args[5]); break;
  case 7: fn->func.f7(-1,fn->args[0],fn->args[1],fn->args[2],fn->args[3],fn->args[4],fn->args[5],fn->args[6]); break;
  case 8: fn->func.f8(-1,fn->args[0],fn->args[1],fn->args[2],fn->args[3],fn->args[4],fn->args[5],fn->args[6],fn->args[7]); break;
  case 9: fn->func.f9(-1,fn->args[0],fn->args[1],fn->args[2],fn->args[3],fn->args[4],fn->args[5],fn->args[6],fn->args[7],fn->args[8]); break;
  case 10: fn->func.f10(-1,fn->args[0],fn->args[1],fn->args[2],fn->args[3],fn->args[4],fn->args[5],fn->args[6],fn->args[7],fn->args[8],fn->args[9]); break;
  }
 }

int anyfns()
 {
 if(!qempty(waiting)) return 1;
 else return 0;
 }

void iztask(Task *t)
 {
 IZQUE(t);
 t->state=0;
 }
