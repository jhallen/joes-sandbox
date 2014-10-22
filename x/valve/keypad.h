#ifndef _Ikeypad
#define _Ikeypad 1

typedef struct keypadvars Keypad;

#define KEYPADFUNCS \
 WIDGETFUNCS \
 void (*stfn)();

#define KEYPADVARS \
 WIDGETVARS \
 TASK tasks[14]; \
 TASK *fn;

extern struct keypadfuncs { KEYPADFUNCS } keypadfuncs;
struct keypadvars { struct keypadfuncs *funcs; KEYPADVARS };

Keypad *mkKeypad();
Keypad *mkTkeypad();

#endif
