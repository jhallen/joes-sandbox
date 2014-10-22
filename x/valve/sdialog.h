#ifndef _Isdialog
#define _Isdialog 1

#include "sctrl.h"
#include "selset.h"
#include "timefield.h"
#include "nkpfield.h"

typedef struct sdialogvars Sdialog;

#define SDIALOGFUNCS \
 WIDGETFUNCS \
 void (*st)(); \

#define SDIALOGVARS \
 WIDGETVARS \
 Sctrl *sctrl; \
 Timefield *when; \
 Nkpfield *program; \
 Button *can; \
 TASK canfn[1]; \
 Button *clr; \
 TASK clrfn[1]; \
 Button *ok; \
 TASK okfn[1]; \

extern struct sdialogfuncs { SDIALOGFUNCS } sdialogfuncs;
struct sdialogvars { struct sdialogfuncs *funcs; SDIALOGVARS };

Sdialog *mkSdialog();

#endif
