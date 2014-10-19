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


void drawline(Lith *x,GC gc,int x1,int y1,int x2,int y2);	/* Draw line */
void drawdot(Lith *x,GC gc,int x1,int y1);		/* Draw pixel */
void drawrect(Lith *x,GC gc,int x1,int y1,int w,int h);	/* Draw rectangle */
void drawbox(Lith *,int x1,int y1,int w,int h,int size,GC gc);
void drawfilled(Lith *x,GC gc,int x1,int y1,int w,int h);	/* Draw filled rectangle */
void drawtxt(Lith *x,GC gc,int x1,int y1,char *str);		/* Draw text */
void drawclr(Lith *x);		/* Clear window */
void drawpic(Lith *x,int xpos,int ypos,int w,int h,GC gc,int *pic);
void draw3dbox(Lith *x,int xpos,int ypos,int w,int h,int thick,GC tedge,GC bedge);
void redraw(Lith *x);

/* Information functions */
int txtw(GC gc,char *s);		/* Text width */
int txth(GC gc);		/* Text height */
int txtb(GC gc);		/* Text baseline */

/* Read-only GC allocation */
GC getgc(char *font,char *color);			/* Get GC for given font and color name */
GC getgcrgb(char *font,int r,int g,int b);			/* Get GC for given font and rgb value */
GC getfidpixel(Font font,unsigned long pixel);		/* Get GC for given font id and pixel value */
Font gcfid(GC gc); /* Get font from a GC */
/* We usually us GCs for refering to fonts and colors, but these supporting
 * functions are available as well */
unsigned long getrgb(unsigned short r,unsigned short g,unsigned short b);		/* Get pixel value for specified rgb */
unsigned long getcolor(char *s);	/* Get pixel value for specified color name */
Font getfont(char *s);			/* Get Font id value for font name */
XFontStruct *getfs(Font fid);		/* Get font structure for specified font id */

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
