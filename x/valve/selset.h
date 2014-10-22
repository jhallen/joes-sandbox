#ifndef _Iselset
#define _Iselset 1

typedef struct selsetvars Selset;

#define SELSETFUNCS \
 WIDGETFUNCS \
 void (*st)(); \

#define SELSETVARS \
 WIDGETVARS \
 int set[64]; \
 Toggle *tog[64]; \
 TASK tasks[64]; \
 char buf1s[64][10]; \
 char buf2s[64][10];

extern struct selsetfuncs { SELSETFUNCS } selsetfuncs;
struct selsetvars { struct selsetfuncs *funcs; SELSETVARS };

Selset *mkSelset();

#endif
