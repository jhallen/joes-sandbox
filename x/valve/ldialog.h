#ifndef _Ildialog
#define _Ildialog 1

#include "lctrl.h"
#include "selset.h"
#include "nkpfield.h"

typedef struct ldialogvars Ldialog;

#define LDIALOGFUNCS \
 WIDGETFUNCS \
 void (*st)(); \

#define LDIALOGVARS \
 WIDGETVARS \
 Lctrl *lctrl; \
 Selset *selset; \
 Nkpfield *nkp; \
 Button *can; \
 TASK canfn[1]; \
 Button *clr; \
 TASK clrfn[1]; \
 Button *ok; \
 TASK okfn[1]; \

extern struct ldialogfuncs { LDIALOGFUNCS } ldialogfuncs;
struct ldialogvars { struct ldialogfuncs *funcs; LDIALOGVARS };

Ldialog *mkLdialog();

#endif
