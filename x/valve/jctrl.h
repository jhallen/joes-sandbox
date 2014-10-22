#ifndef _Ijctrl
#define _Ijctrl 1

#include "pctrl.h"

typedef struct jctrlvars Jctrl;

/* When job is triggered: copy program to pgm.
 * For each watering cycle: copy dwell from pgm to each requested valve in
 * 'valves'.  These become the actual timers for active valves.  'next'
 * which o 'valves' will be next sceduled (scheduling is done in a round-robin
 * fashion).  When all 'valves' are zero, the next cycle is initiated.
 *
 * Any time a valve is found to be disabled when it is requesting scheduling,
 * it is zeroed.
 */

struct job
 {
 int hour;		/* Starting time */
 int min;
 int sec;
 int program;		/* Program number */
 struct program pgm;	/* Program */
 int valves[64];	/* Dwell counter for each valve currently on */
 int waiting[64];	/* Valves waiting to be turned on */
 int cycle;		/* Delay counter for next watering cycle */
 int next;		/* Next valve no. waiting to go */
 int valid;		/* Set if we have data */
 };

extern struct job jobs[];

#define JCTRLFUNCS \
 WIDGETFUNCS \
 void (*stfn)(); \
 void (*sttext)(); \
 void (*st)(); \

#define JCTRLVARS \
 WIDGETVARS \
 int name; \
 TASK *fn;

extern struct jctrlfuncs { JCTRLFUNCS } jctrlfuncs;
struct jctrlvars { struct jctrlfuncs *funcs; JCTRLVARS };

Jctrl *mkJctrl();

#endif
