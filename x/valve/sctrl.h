#ifndef _Isctrl
#define _Isctrl 1

typedef struct sctrlvars Sctrl;

struct sched
 {
 int hour;	/* Time to execute program */
 int min;
 int sec;
 int program;	/* Program to execute */
 int valid;	/* Set if program is valid */
 };

extern struct sched scheds[];

#define SCTRLFUNCS \
 WIDGETFUNCS \
 void (*stfn)(); \
 void (*sttext)(); \
 void (*st)(); \

#define SCTRLVARS \
 WIDGETVARS \
 int name; \
 TASK *fn; \

extern struct sctrlfuncs { SCTRLFUNCS } sctrlfuncs;
struct sctrlvars { struct sctrlfuncs *funcs; SCTRLVARS };

Sctrl *mkSctrl();

#endif
