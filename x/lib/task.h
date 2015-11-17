/* Continuations, call-backs, and threads. */

#ifndef _Itask
#define _Itask 1

#include "config.h"

#ifndef _Isetjmp
#define _Isetjmp 1
#include <setjmp.h>
#endif

#include "queue.h"

/* A generic argument */

typedef union arg Arg;

union arg
 {
 char c;
 unsigned char uc;
 char *s;
 unsigned char *us;
 void *p;
 int (*fp)();
 int i;
 unsigned int ui;
 long l;
 unsigned long ul;
 float f;
 double d;
 };

typedef union taskfunc Taskfunc;

union taskfunc
 {
 void (*f0)(int status);
 void (*f1)(int status, Arg arg1);
 void (*f2)(int status, Arg arg1, Arg arg2);
 void (*f3)(int status, Arg arg1, Arg arg2, Arg arg3);
 void (*f4)(int status, Arg arg1, Arg arg2, Arg arg3, Arg arg4);
 void (*f5)(int status, Arg arg1, Arg arg2, Arg arg3, Arg arg4, Arg arg5);
 void (*f6)(int status, Arg arg1, Arg arg2, Arg arg3, Arg arg4, Arg arg5, Arg arg6);
 void (*f7)(int status, Arg arg1, Arg arg2, Arg arg3, Arg arg4, Arg arg5, Arg arg6, Arg arg7);
 void (*f8)(int status, Arg arg1, Arg arg2, Arg arg3, Arg arg4, Arg arg5, Arg arg6, Arg arg7, Arg arg8);
 void (*f9)(int status, Arg arg1, Arg arg2, Arg arg3, Arg arg4, Arg arg5, Arg arg6, Arg arg7, Arg arg8, Arg arg9);
 void (*f10)(int status, Arg arg1, Arg arg2, Arg arg3, Arg arg4, Arg arg5, Arg arg6, Arg arg7, Arg arg8, Arg arg9, Arg arg10);
 };

/* A task: either a callback function or a continuation. */

typedef struct task Task;

struct task
 {
 QITEM(Task);		/* Task may be on a doubly-linked list */
 int state;		/* State: 0=no task, 1=function/continuation installed
 			   into task, 2=task scheduled to run after a delay,
 			   3=task is waiting to run now. */
 Taskfunc func;		/* Function or NULL for continuation */
 jmp_buf jmpbuf;	/* Continuation point (PC + SP) */
 Arg *argptr;		/* Arg list pointer for continuations. */
 int n;			/* No. args so far */
 int usecs;		/* uSecs to wait before executing task */
 Arg args[10];		/* The args */
 int nargs;		/* No. of expected args */
 };

/* Task *fnN_M(Task *,void (*func)(Arg N..., Arg M...),Arg N...);
 *
 * Install a callback function into a Task.  The Task is not executed until
 * contM() is called with it as an argument.  N args are stored along with
 * the function.  These will appear first in the function's argument list
 * when it's called.  M additional arguments also will be appended to the
 * argument list with contM().
 */
Task *fn0_0(Task *t, void (*func)(int status));
Task *fn0_1(Task *t, void (*func)(int status, Arg argv));
Task *fn0_2(Task *t, void (*func)(int status, Arg argv, Arg argw));
Task *fn0_3(Task *t, void (*func)(int status, Arg argv, Arg argw, Arg argx));
Task *fn0_4(Task *t, void (*func)(int status, Arg argv, Arg argw, Arg argx, Arg argy));
Task *fn0_5(Task *t, void (*func)(int status, Arg argv, Arg argw, Arg argx, Arg argy, Arg argz));

Task *fn1_0(Task *t, void (*func)(int status, Arg arga), Arg arga);
Task *fn1_1(Task *t, void (*func)(int status, Arg arga, Arg argv), Arg arga);
Task *fn1_2(Task *t, void (*func)(int status, Arg arga, Arg argv, Arg argw), Arg arga);
Task *fn1_3(Task *t, void (*func)(int status, Arg arga, Arg argv, Arg argw, Arg argx), Arg arga);
Task *fn1_4(Task *t, void (*func)(int status, Arg arga, Arg argv, Arg argw, Arg argx, Arg argy), Arg arga);
Task *fn1_5(Task *t, void (*func)(int status, Arg arga, Arg argv, Arg argw, Arg argx, Arg argy, Arg argz), Arg arga);

Task *fn2_0(Task *t, void (*func)(int status, Arg arga, Arg argb), Arg arga, Arg argb);
Task *fn2_1(Task *t, void (*func)(int status, Arg arga, Arg argb, Arg argv), Arg arga, Arg argb);
Task *fn2_2(Task *t, void (*func)(int status, Arg arga, Arg argb, Arg argv, Arg argw), Arg arga, Arg argb);
Task *fn2_3(Task *t, void (*func)(int status, Arg arga, Arg argb, Arg argv, Arg argw, Arg argx), Arg arga, Arg argb);
Task *fn2_4(Task *t, void (*func)(int status, Arg arga, Arg argb, Arg argv, Arg argw, Arg argx, Arg argy), Arg arga, Arg argb);
Task *fn2_5(Task *t, void (*func)(int status, Arg arga, Arg argb, Arg argv, Arg argw, Arg argx, Arg argy, Arg argz), Arg arga, Arg argb);

Task *fn3_0(Task *t, void (*func)(int status, Arg arga, Arg argb, Arg argc), Arg arga, Arg argb, Arg argc);
Task *fn3_1(Task *t, void (*func)(int status, Arg arga, Arg argb, Arg argc, Arg argv), Arg arga, Arg argb, Arg argc);
Task *fn3_2(Task *t, void (*func)(int status, Arg arga, Arg argb, Arg argc, Arg argv, Arg argw), Arg arga, Arg argb, Arg argc);
Task *fn3_3(Task *t, void (*func)(int status, Arg arga, Arg argb, Arg argc, Arg argv, Arg argw, Arg argx), Arg arga, Arg argb, Arg argc);
Task *fn3_4(Task *t, void (*func)(int status, Arg arga, Arg argb, Arg argc, Arg argv, Arg argw, Arg argx, Arg argy), Arg arga, Arg argb, Arg argc);
Task *fn3_5(Task *t, void (*func)(int status, Arg arga, Arg argb, Arg argc, Arg argv, Arg argw, Arg argx, Arg argy, Arg argz), Arg arga, Arg argb, Arg argc);

Task *fn4_0(Task *t, void (*func)(int status, Arg arga, Arg argb, Arg argc, Arg argd), Arg arga, Arg argb, Arg argc, Arg argd);
Task *fn4_1(Task *t, void (*func)(int status, Arg arga, Arg argb, Arg argc, Arg argd, Arg argv), Arg arga, Arg argb, Arg argc, Arg argd);
Task *fn4_2(Task *t, void (*func)(int status, Arg arga, Arg argb, Arg argc, Arg argd, Arg argv, Arg argw), Arg arga, Arg argb, Arg argc, Arg argd);
Task *fn4_3(Task *t, void (*func)(int status, Arg arga, Arg argb, Arg argc, Arg argd, Arg argv, Arg argw, Arg argx), Arg arga, Arg argb, Arg argc, Arg argd);
Task *fn4_4(Task *t, void (*func)(int status, Arg arga, Arg argb, Arg argc, Arg argd, Arg argv, Arg argw, Arg argx, Arg argy), Arg arga, Arg argb, Arg argc, Arg argd);
Task *fn4_5(Task *t, void (*func)(int status, Arg arga, Arg argb, Arg argc, Arg argd, Arg argv, Arg argw, Arg argx, Arg argy, Arg argz), Arg arga, Arg argb, Arg argc, Arg argd);

Task *fn5_0(Task *t, void (*func)(int status, Arg arga, Arg argb, Arg argc, Arg argd, Arg arge), Arg arga, Arg argb, Arg argc, Arg argd, Arg arge);
Task *fn5_1(Task *t, void (*func)(int status, Arg arga, Arg argb, Arg argc, Arg argd, Arg arge, Arg argv), Arg arga, Arg argb, Arg argc, Arg argd, Arg arge);
Task *fn5_2(Task *t, void (*func)(int status, Arg arga, Arg argb, Arg argc, Arg argd, Arg arge, Arg argv, Arg argw), Arg arga, Arg argb, Arg argc, Arg argd, Arg arge);
Task *fn5_3(Task *t, void (*func)(int status, Arg arga, Arg argb, Arg argc, Arg argd, Arg arge, Arg argv, Arg argw, Arg argx), Arg arga, Arg argb, Arg argc, Arg argd, Arg arge);
Task *fn5_4(Task *t, void (*func)(int status, Arg arga, Arg argb, Arg argc, Arg argd, Arg arge, Arg argv, Arg argw, Arg argx, Arg argy), Arg arga, Arg argb, Arg argc, Arg argd, Arg arge);
Task *fn5_5(Task *t, void (*func)(int status, Arg arga, Arg argb, Arg argc, Arg argd, Arg arge, Arg argv, Arg argw, Arg argx, Arg argy, Arg argz), Arg arga, Arg argb, Arg argc, Arg argd, Arg arge);

/* Task *cfn(Task *fn,Arg *argptr);
 * Prepare to suspend the current task (go() actually suspends it).
 *
 * argptr points to an array of Args which will receive the M arguments
 * passed in by contM when the task is resumed.
 */
Task *cfn(Task *fn, Arg *argptr, int nargs);

/* void contM(Task *fn,int arg1,int arg2,...,int argN);
 *
 * Place Task in the pending task queue.  Upon the next return to
 * the event loop, the task will be executed.  N args are appended
 * to the Task function's argument list.
 */
void cont0(Task *t);
void cont1(Task *t, Arg arg1);
void cont2(Task *t, Arg arg1, Arg arg2);
void cont3(Task *t, Arg arg1, Arg arg2, Arg arg3);
void cont4(Task *t, Arg arg1, Arg arg2, Arg arg3, Arg arg4);
void cont5(Task *t, Arg arg1, Arg arg2, Arg arg3, Arg arg4, Arg arg5);

/* void cancel(Task *task);
 * Cancel execution of a task.
 */
void cancel(Task *t);

/* void go(void (*io)(), Task *fn);
 *
 * Execute tasks.  If there are none, call io()- a function which should
 * perform I/O and generate new tasks for us to do.
 *
 */
void go(int (*io)(void), Task *t);

/* int anyfns();
 * Return true if there are any Tasks waiting to be executed.
 */
int anyfns(void);

void iztask(Task *t);

#endif
