#ifndef _Ipctrl
#define _Ipctrl 1

typedef struct pctrlvars Pctrl;

struct program
 {
 int valves[64];	/* Valves to open */
 int dwell;		/* Seconds for each valve */
 int repeat;		/* Total repetition time. 0==execute only once */
 int cycle;		/* Time for each watering cycle- only valid if
 			 * repeat is non-zero. */
 int valid;		/* Set if this program is valid */
 };

extern struct program programs[];

#define PCTRLFUNCS \
 WIDGETFUNCS \
 void (*stfn)(); \
 void (*sttext)(); \
 void (*st)(); \

#define PCTRLVARS \
 WIDGETVARS \
 int name; \
 TASK *fn; \

extern struct pctrlfuncs { PCTRLFUNCS } pctrlfuncs;
struct pctrlvars { struct pctrlfuncs *funcs; PCTRLVARS };

Pctrl *mkPctrl();

#endif
