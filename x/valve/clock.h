#ifndef _Iclock
#define _Iclock 1

typedef struct clockvars Clock;

#define CLOCKFUNCS \
 WIDGETFUNCS \

#define CLOCKVARS \
 WIDGETVARS \
 Text *timew; \
 Button *setclock; \
 TASK setclockfn[1]; \
 char timestr[40]; \
 TASK tevent[1]; \
 struct tm *curtm; \
 time_t oldtime;

extern struct clockfuncs { CLOCKFUNCS } clockfuncs;
struct clockvars { struct clockfuncs *funcs; CLOCKVARS };

Clock *mkClock();

typedef struct clocksetvars Clockset;

#define CLOCKSETFUNCS \
 WIDGETFUNCS \
 void (*st)(); \

#define CLOCKSETVARS \
 WIDGETVARS \
 Clock *clock; \
 Button *inch; \
 TASK inchfn[1]; \
 Button *dech; \
 TASK dechfn[1]; \
 Button *incm; \
 TASK incmfn[1]; \
 Button *decm; \
 TASK decmfn[1]; \
 Button *setit; \
 TASK setitfn[1]; \
 Button *canit; \
 TASK canitfn[1]; \

extern struct clocksetfuncs { CLOCKSETFUNCS } clocksetfuncs;
struct clocksetvars { struct clocksetfuncs *funcs; CLOCKSETVARS };

Clockset *mkClockset();

#endif
