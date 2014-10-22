#ifndef _Icaltouch
#define _Icaltouch 1

typedef struct caltouchvars Caltouch;

#define CALTOUCHFUNCS \
 WIDGETFUNCS \

#define CALTOUCHVARS \
 WIDGETVARS \
 int state; \

extern struct caltouchfuncs { CALTOUCHFUNCS } caltouchfuncs;
struct caltouchvars { struct caltouchfuncs *funcs; CALTOUCHVARS };

Caltouch *mkCaltouch();

#endif
