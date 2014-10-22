#ifndef _Ilctrl
#define _Ilctrl 1

#include "vctrl.h"

typedef struct lctrlvars Lctrl;

struct limit
 {
 int valves[64];
 int cur;
 int max;
 int valid;
 };

extern struct limit limits[];

#define LCTRLFUNCS \
 WIDGETFUNCS \
 void (*stfn)(); \
 void (*sttext)(); \
 void (*st)(); \

#define LCTRLVARS \
 WIDGETVARS \
 int name; \
 TASK *fn; \

extern struct lctrlfuncs { LCTRLFUNCS } lctrlfuncs;
struct lctrlvars { struct lctrlfuncs *funcs; LCTRLVARS };

Lctrl *mkLctrl();

#endif
