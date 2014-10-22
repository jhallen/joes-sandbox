#ifndef _Ijdialog
#define _Ijdialog 1

#include "jctrl.h"

typedef struct jdialogvars Jdialog;

#define JDIALOGFUNCS \
 WIDGETFUNCS \
 void (*st)(); \

#define JDIALOGVARS \
 WIDGETVARS \
 Jctrl *jctrl; \
 TASK jctrlfn[1]; \
 Button *can; \
 TASK canfn[1]; \
 Button *clr; \
 TASK clrfn[1]; \

extern struct jdialogfuncs { JDIALOGFUNCS } jdialogfuncs;
struct jdialogvars { struct jdialogfuncs *funcs; JDIALOGVARS };

Jdialog *mkJdialog();

#endif
