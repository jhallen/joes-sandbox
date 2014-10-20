#ifndef _Idisp
#define _Idisp 1

#include "config.h"

typedef struct window W;
struct window
 {
 /* Where window is on screen */
 int x,y,w,h;
 
 /* What area of image window is looking at */
 int ix,iy,iw,ih;

 /* Grid dot spacing */
 int dx,dy;
 };

W *mkw();
void genw();
void clrw();
void gridw();

#endif
