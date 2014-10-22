/* Drawing functions 
   Copyright (C) 1999 Joseph H. Allen

This file is part of Notif

Notif is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation; either version 1, or (at your option) any later version.  

Notif is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.  

You should have received a copy of the GNU General Public License along with 
Notif; see the file COPYING.  If not, write to the Free Software Foundation, 
675 Mass Ave, Cambridge, MA 02139, USA.  */ 


void drawline();	/* Draw line */
void drawdot();		/* Draw pixel */
void drawrect();	/* Draw rectangle */
void drawfilled();	/* Draw filled rectangle */
void drawtxt();		/* Draw text */
void drawclr();		/* Clear window */
void drawicon();
void draw3dbox();
void redraw();

/* Information functions */
int txtw();		/* Text width */
int txth();		/* Text height */
int txtb();		/* Text baseline */

/* Read-only GC allocation */
GC getgc();			/* Get GC for given font and color name */
GC getgcrgb();			/* Get GC for given font and rgb value */
GC getfidpixel();		/* Get GC for given font id and pixel value */

/* We usually us GCs for refering to fonts and colors, but these supporting
 * functions are available as well */
unsigned long getrgb();		/* Get pixel value for specified rgb */
unsigned long getcolor();	/* Get pixel value for specified color name */
Font getfont();			/* Get Font id value for font name */
XFontStruct *getfs();		/* Get font structure for specified font id */

/* Macros for constructing scalable icons */

/* A command with color */
#define picdo(cmd,r,g,b) (((0xFF&(b))<<24)|((0xFF&(g))<<16)|((0xFF&(r))<<8)|(0xFF&(cmd)))

/* A coordinate */
#define picxy(x,y) (((0xFFFF&(y))<<16)|(0xFFFF&(x)))

/* Command codes */
#define picend 0	/* End of commands */
#define picfil 1	/* Area fill */
#define picmrg 2	/* Merge drawing buffer into output buffer with color;
                           then clear drawing buffer */
#define picusr 3	/* Same as above, but user color */
#define picbez 4	/* Bezier curve */
#define piclin 5	/* Line */

/* Combination macros */
#define picline(x1,y1,x2,y2) picdo(piclin,0,0,0), picxy(x1,y1), picxy(x2,y2)
#define picbezier(x1,y1,x2,y2,x3,y3,x4,y4) \
 picdo(picbez,0,0,0), picxy(x1,y1), picxy(x2,y2), picxy(x3,y3), picxy(x4,y4)
#define picmerge(r,g,b) picdo(picmrg,r,g,b)
#define picuser picdo(picusr,0,0,0)
#define picfill picdo(picfil,0,0,0)
#define picdone picdo(picend,0,0,0)

/* Extract elements from picdo and picxy */
#define gtpiccmd(x) ((x)&0xFF)
#define gtpicr(x) (((x)>>8)&0xFF)
#define gtpicg(x) (((x)>>16)&0xFF)
#define gtpicb(x) (((x)>>24)&0xFF)
#define gtpicy(x) (((x)>>16)&0xFFFF)
#define gtpicx(x) ((x)&0xFFFF)
