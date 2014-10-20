#ifndef _Isc
#define _Isc 1

extern void (*set)();
extern void (*clr)();
extern void (*inv)();
extern void (*byte)();
extern int width;
extern int height;
extern char *hscreen;
void graph();
void text();
void lineto();
void dot();

#define CLR 0
#define SET 1
#define INV 2

#endif
