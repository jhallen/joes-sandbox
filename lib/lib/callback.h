/* Call-back function manager */

#ifndef _Icb
#define _Icb 1

#include "config.h"

/* A call-back function */

typedef struct fn FN;

struct fn
 {
 int (*func)();		/* Function to call */
 FN *fn;		/* Continuation */
 int n;			/* No. args given by creator */
 long args[5];		/* The args */
 };

/* Create a callback function */

/* FN *fnN(int (*func)(),FN *fn,long pass1,long pass2,...,long passN);
 * Where N is the no. of args to pass to the callback function
 * when it is executed.
 */
FN *fn0();
FN *fn1();
FN *fn2();
FN *fn3();
FN *fn4();
FN *fn5();

/* Execute a call-back */

/* int execN(FN *fn,int stat,long arg1,long arg2,...,long argN);
 */
#define exec0(fn,stat) \
 ((_cont=(fn)), ((fn)=0), doexec0(_cont,(stat)))
#define exec1(fn,stat,a) \
 ((_cont=(fn)), ((fn)=0), doexec1(_cont,(stat),(a)))
#define exec2(fn,stat,a,b) \
 ((_cont=(fn)), ((fn)=0), doexec2(_cont,(stat),(a),(b)))
#define exec3(fn,stat,a,b,c) \
 ((_cont=(fn)), ((fn)=0), doexec3(_cont,(stat),(a),(b),(c)))
#define exec4(fn,stat,a,b,c,d) \
 ((_cont=(fn)), ((fn)=0), doexec4(_cont,(stat),(a),(b),(c),(d)))
#define exec5(fn,stat,a,b,c,d,e) \
 ((_cont=(fn)), ((fn)=0), doexec5(_cont,(stat),(a),(b),(c),(d),(e)))

extern FN *_cont;

/* Hook an extra proceedure to the end of a call-back chain
 *
 * FN *hook(FN *fn,FN *proc);
 */
FN *hook();

#endif
