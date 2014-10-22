/* Time entry widget */

#ifndef _Itimefield
#define _Itimefield 1

#include "keypad.h"

typedef struct timefieldvars Timefield;

#define TIMEFIELDFUNCS \
 WIDGETFUNCS

#define TIMEFIELDVARS \
 WIDGETVARS \
 Button *button; \
 TASK buttonfn[1]; \
 Tedit *tedit; \
 Text *ampm; \
 int ampmflg; \
 Keypad *keypad; \
 TASK keypadfn[1]; \

extern struct timefieldfuncs { TIMEFIELDFUNCS } timefieldfuncs;
struct timefieldvars { struct timefieldfuncs *funcs; TIMEFIELDVARS };

Timefield *mkTimefield();

#endif
