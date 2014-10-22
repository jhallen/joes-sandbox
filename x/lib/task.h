/* Continuations, call-backs, and threads. */

#ifndef _Itask
#define _Itask 1

#include "config.h"

#ifndef _Isetjmp
#define _Isetjmp 1
#include <setjmp.h>
#endif

#include "queue.h"

/* A task: either a callback function or a continuation. */

typedef struct task TASK;

struct task
 {
 QITEM(TASK);		/* Task may be on a doubly-linked list */
 int state;		/* State: 0=no task, 1=function/continuation installed
 			   into task, 2=task scheduled to run after a delay,
 			   3=task is waiting to run now. */
 void (*func)();	/* Function.  NULL for continuation. */
 jmp_buf jmpbuf;	/* Continuation point (PC + SP) */
 int *argptr;		/* Arg list pointer for continuations. */
 int n;			/* No. args */
 int usecs;		/* uSecs to wait before executing task */
 int args[10];		/* The args */
 };

/* TASK *fnN(TASK *,void (*func)(),int pass1,int pass2,...,int passN);
 *
 * Install a callback function into a TASK.  The TASK is not executed until
 * contN() is called with it as an argument.  N args are passed to the
 * function.  Additional args may be appended to the argument list with
 * contN().
 */
TASK *fn0();
TASK *fn1();
TASK *fn2();
TASK *fn3();
TASK *fn4();
TASK *fn5();

/* TASK *cfn(TASK *fn,int *argptr);
 * Prepare a task for continuation.
 */
TASK *cfn();

/* void contN(TASK *fn,int arg1,int arg2,...,int argN);
 *
 * Place TASK in the pending task queue.  Upon the next return to
 * the event loop, the task will be executed.  N args are appended
 * to the TASK function's argument list.
 */
void cont0();
void cont1();
void cont2();
void cont3();
void cont4();
void cont5();

/* void cancel(TASK *task);
 * Cancel execution of a task.
 */
void cancel();

/* int *go(void (*io)(), TASK *fn);
 * Execute tasks.  If there are none, call io()- a function which should
 * perform I/O and generate new tasks for us to do.
 */
void go();

/* int anyfns();
 * Return true if there are any TASKs waiting to be executed.
 */
int anyfns();

void iztask();

#endif
