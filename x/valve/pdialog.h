#ifndef _Ipdialog
#define _Ipdialog 1

#include "pctrl.h"
#include "selset.h"
#include "nkpfield.h"

typedef struct pdialogvars Pdialog;

#define PDIALOGFUNCS \
 WIDGETFUNCS \
 void (*st)(); \

#define PDIALOGVARS \
 WIDGETVARS \
 Pctrl *pctrl; \
 Selset *selset; \
 Nkpfield *dwell; \
 Nkpfield *cycle; \
 Nkpfield *repeat; \
 Button *can; \
 TASK canfn[1]; \
 Button *clr; \
 TASK clrfn[1]; \
 Button *ok; \
 TASK okfn[1];

extern struct pdialogfuncs { PDIALOGFUNCS } pdialogfuncs;
struct pdialogvars { struct pdialogfuncs *funcs; PDIALOGVARS };

Pdialog *mkPdialog();

#endif
