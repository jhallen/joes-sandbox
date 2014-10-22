#ifndef _Ivctrl
#define _Ivctrl 1

typedef struct vctrlvars Vctrl;

struct valve
 {
 /* Actual state of valve */
 int on;	/* Set if valve is currently on */
 /* Which program owns valve's resource */
 int job;	/* Job no. which owns this valve */
 int enable;	/* Set if automatic control is enabled */
 int manual;	/* Set if valve is on from manual control */
 /* If either manual or dwell is set, the valve's resources are allocated */
 };

extern struct valve valves[64];

#define VCTRLFUNCS \
 WIDGETFUNCS \
 void (*stfn)(); \
 void (*sttext)(); \
 void (*st)(); \

#define VCTRLVARS \
 WIDGETVARS \
 int state; \
 int autom; \
 int name; \
 TASK *fn;

extern struct vctrlfuncs { VCTRLFUNCS } vctrlfuncs;
struct vctrlvars { struct vctrlfuncs *funcs; VCTRLVARS };

Vctrl *mkVctrl();

#endif
