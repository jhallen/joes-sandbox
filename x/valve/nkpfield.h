/* Numeric keypad editing field */

#ifndef _Inkpfield
#define _Inkpfield 1

#include "keypad.h"

typedef struct nkpfieldvars Nkpfield;

#define NKPFIELDFUNCS \
 WIDGETFUNCS

#define NKPFIELDVARS \
 WIDGETVARS \
 Button *button; \
 TASK buttonfn[1]; \
 Keypad *keypad; \
 TASK keypadfn[1]; \
 Tedit *tedit; \

extern struct nkpfieldfuncs { NKPFIELDFUNCS } nkpfieldfuncs;
struct nkpfieldvars { struct nkpfieldfuncs *funcs; NKPFIELDVARS };

Nkpfield *mkNkpfield();

#endif
