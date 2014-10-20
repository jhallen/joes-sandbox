/* VTxxx/ANSI terminal emulator
   Copyright (C) 1991 Joseph H. Allen

JTERM - Joe's pass-through printing terminal emulator

JTERM is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 1, or (at your option) any later version.

JTERM is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.  

You should have received a copy of the GNU General Public License
along with JTERM; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <dos.h>
#include <stdio.h>
#include "tty.h"

/* Address and size of screen */

unsigned short far *screen;
unsigned height;
unsigned width;

/* Determine screen address and size */

int a6845;      /* Port No. of 6845 chip */
int color=0;    /* Set if we have a color screen */

void sparms(void)
{
int flag;
_AH=0x0f;
geninterrupt(0x10);
flag=_AX;
width=(flag>>8);
if((flag&0xFF)==7) screen=(unsigned short far *)0xb0000000, a6845=0x3b4;
else screen=(unsigned short far *)0xb8000000, a6845=0x3d4, color=1;
height=*(unsigned char far *)0x00400084;
if(height<25) height=25;
}

/* Set cursor position using bios */
/* See macro cpos() in tty.h for the routine we use */
void bioscpos(int x,int y)
{
_DH=y;
_DL=x;
_BH=0;
_AH=2;
geninterrupt(0x10);
}

/* Terminal simulator variables */

unsigned attrib;                /* Current attribute video OR value */
int vtattrib;                   /* Vt220 attirbutes which are set */
unsigned x,y;                   /* Cursor position */
int mode;			/* Current input mode */
unsigned stop,sbot;             /* Top/Bottom margins */
char tabs[512];			/* Tab stops */

/* Mode flags */
/* See ttyinit for initial values */

int mapplication;               /* Set for application keypad */
int mautowrap;                  /* Set for autowrap */
int minmargins;                 /* Set if cursor stays in margins */
int minsert;                    /* Set for insert mode */
int mnl;                        /* Set if LF is NL */
int minvisable;                 /* Set for invisable cursor */

/* Buffer for reading in numeric parameters */

unsigned parry[MAXPARMS];
int nptr;			/* Paramenter pointer */

/* Scroll the screen up */

void scrollup(unsigned first,unsigned last,unsigned count)
{
if(!count) count=1;
if(first+count>=last+1) count=last+1-first;
else if(last!=first)
 fbfwrdw(first*width+screen,(first+count)*width+screen,
         width*(1+last-(first+count)));
fbsetw((last+1-count)*width+screen,width*count,BLANK);
}

/* Scroll the screen down */

void scrolldown(unsigned first,unsigned last,unsigned count)
{
if(!count) count=1;
if(first+count>=last+1) count=last+1-first;
else if(last!=first)
 fbbkwdw((first+count)*width+screen,first*width+screen,
         width*(1+last-(first+count)));
fbsetw(first*width+screen,width*count,BLANK);
}

/* Begin entering parameters */
void begin(void)
{
int n;
for(n=0;n!=MAXPARMS;n++) parry[n]=0;
nptr=0;
}

/************/
/* Commands */
/************/

/* Position cursor */
void pos(void)
{
y=parry[0];
x=parry[1];
if(y) --y;
if(x) --x;
if(minmargins) y+=stop;
if(minmargins) { if(y>sbot) y=sbot; }
else if(y>=height) y=height-1;
if(x>=width) x=width-1;
if(!minvisable) cpos(x,y);
}

/* Carriage return */
void cr(void)
{
x=0;
if(!minvisable) cpos(x,y);
}

/* Backspace */
void bs(void)
{
if(x)
 {
 --x;
 if(!minvisable) cpos(x,y);
 }
}

/* Cursor down */
void down(void)
{
if(y>sbot)
 {
 if(!parry[0]) ++parry[0];
 y+=parry[0];
 if(y>=height) y=height-1;
 if(!minvisable) cpos(x,y);
 }
else
 {
 if(!parry[0]) ++parry[0];
 y+=parry[0];
 if(y>sbot) y=sbot;
 if(!minvisable) cpos(x,y);
 }
}

/* Cursor up */
void up(void)
{
if(y<stop)
 {
 if(!parry[0]) ++parry[0];
 if(parry[0]>y) y=0;
 else y-=parry[0];
 if(!minvisable) cpos(x,y);
 }
else
 {
 if(!parry[0]) ++parry[0];
 if(parry[0]>y-stop) y=stop;
 else y-=parry[0];
 if(!minvisable) cpos(x,y);
 }
}

/* Cursor right */
void right(void)
{
if(!parry[0]) ++parry[0];
x+=parry[0];
if(x>=width) x=width-1;
if(!minvisable) cpos(x,y);
}

/* Cursor left */
void left(void)
{
if(!parry[0]) ++parry[0];
if(parry[0]>x) x=0;
else x-=parry[0];
if(!minvisable) cpos(x,y);
}

/* Set scrolling region */
void setregn(void)
{
int n;
stop=parry[0];
sbot=parry[1];
if(stop) --stop;
if(sbot) --sbot;
if(stop>=height) stop=height-1;
if(sbot>=height) sbot=height-1;
if(stop>sbot) n=stop, stop=sbot, sbot=n;
begin();
pos();
}

/* Linefeed */
void lf(void)
{
if(y==sbot) scrollup(stop,sbot,1);
else parry[0]=0, down();
}

/* Overtype a character */

/* Flag set when we hit right margin in magicwrap mode */
int dowrap=0;

/* Called by some functions to when dowrap is set to wrap cursor */
void wrap(void)
{
cr(); lf();
dowrap=0;
}

void type(unsigned char c)
{
if(dowrap) wrap();
screen[x+y*width]=c+attrib;
if(x!=width-1) { ++x; if(!minvisable) cpos(x,y); }
else if(mautowrap) dowrap=1;
}

/* Cursor up, maybe scroll */
void ups(void)
{
if(y==stop) scrolldown(stop,sbot,1);
else parry[0]=0, up();
}

/* Cursor to next tab stop */
void tab(void)
{
if(dowrap) wrap();
while(x!=width-1) if(tabs[++x]) goto done;
dowrap=1;
done:
if(!minvisable) cpos(x,y);
}

/* Set tab stop */
void settab(void)
{
tabs[x]=1;
}

/* Clear tab stop(s) */
void clrt(void)
{
int n;
if(parry[0]==0) tabs[x]=0;
else if(parry[0]==3) for(n=0;n!=width;++n) tabs[n]=0;
}

/* Clear screen */
void clrs(void)
{
if(parry[0]==0) fbsetw(screen+x+y*width,width*height-(y*width+x),BLANK);
else if(parry[0]==1) fbsetw(screen,y*width+x,BLANK);
else if(parry[0]==2) fbsetw(screen,width*height,BLANK);
}

/* Clear line */
void clrl(void)
{
if(parry[0]==0) fbsetw(screen+y*width+x,width-x,BLANK);
else if(parry[0]==1) fbsetw(screen+y*width,x,BLANK);
else if(parry[0]==2) fbsetw(screen+y*width,width,BLANK);
}

/* Clear characters */
void clrc(void)
{
if(!parry[0]) parry[0]=1;
if(parry[0]+x+y*width>width*height) parry[0]=width*height-(x+y*width);
fbsetw(screen+width*y+x,parry[0],BLANK);
}

/* Delete lines */
void dell(void)
{
if(y>=stop && y<=sbot) scrollup(y,sbot,parry[0]);
}

/* Delete characters */
void delc(void)
{
if(dowrap) wrap();
if(!parry[0]) parry[0]=1;
if(parry[0]>=width-x) parry[0]=width-x;
else
 fbfwrdw(screen+x+y*width,screen+x+y*width+parry[0],width-(x+parry[0]));
fbsetw(screen+(y+1)*width-parry[0],parry[0],BLANK);
}

/* Insert lines */
void insl(void)
{
if(y>=stop && y<=sbot) scrolldown(y,sbot,parry[0]);
}

/* Insert spaces */
void inss(void)
{
if(dowrap) wrap();
if(!parry[0]) parry[0]=1;
if(parry[0]+x>=width) parry[0]=width-x;
else fbbkwdw(screen+x+y*width+parry[0],screen+x+y*width,width-(x+parry[0]));
fbsetw(screen+x+y*width,parry[0],BLANK);
}

/* Attribute table - attribute bits to attribute or value translation */
unsigned atable[16]=
 {
 0x0700,                /* Normal */
 0x0F00,                /* Bold */
 0x7000,                /* Inverse */
 0x7800,                /* Bold+Inverse */
 0x8700,                /* Blink */
 0x8F00,                /* Bold+Blink */
 0xF000,                /* Blink+Inverse */
 0xF800,                /* Blink+Inverse+Bold */
 0x0100,                /* Underline */
 0x0900,                /* Underline+Bold */
 0x7000,                /* Underline+Inverse */
 0x7800,                /* Underline+Bold+Inverse */
 0x8100,                /* Underline+Blink */
 0x8900,                /* Underline+Bold+Blink */
 0xF000,                /* Underline+Blink+Inverse */
 0xF800                 /* Underline+Blink+Inverse+Bold */
 };

/* Set attributes */
void setattrib(void)
{
int n;
for(n=0;n<=nptr;++n)
 switch(parry[n])
  {
 case 0: vtattrib=0; break;
 case 1: if(!color) vtattrib|=VTABOLD; break;
 case 22: vtattrib&=~VTABOLD; break;
 case 4: if(!color) vtattrib|=VTAUNDERLINE; break;
 case 24: vtattrib&=~VTAUNDERLINE; break;
 case 5: if(!color) vtattrib|=VTABLINK; break;
 case 25: vtattrib&=~VTABLINK; break;
 case 7: vtattrib|=VTAINVERSE; break;
 case 27: vtattrib&=~VTAINVERSE; break;
  }
attrib=atable[vtattrib];
}

/* Set modes */
void hmode(void)
{
switch(parry[0])
 {
case 1: mapplication=1; break;
case 3: ttyinit(); break;
case 4: minsert=1; break;
case 6: dowrap=0; minmargins=1; parry[0]=0; parry[1]=0; pos(); break;
case 7: mautowrap=1; break;
case 20: mnl=1; break;
case 25: minvisable=0; cpos(x,y); break;
 }
}

/* Reset modes */
void lmode(void)
{
switch(parry[0])
 {
case 1: mapplication=0; break;
case 3: ttyinit(); break;
case 4: minsert=0; break;
case 6: dowrap=0; minmargins=0; parry[0]=0; parry[1]=0; pos(); break;
case 7: dowrap=0; mautowrap=0; break;
case 20: mnl=0; break;
case 25: minvisable=1; cpos(width,height); break;
 }
}

/* Insert character (for insert mode) */
void insc(unsigned char c)
{
if(dowrap) wrap();
parry[0]=0; inss();
type(c);
}

/* Ring bell */
void bell(void)
{
/* Beep! */
}

/* Initialize the terminal */

void ttyinit(void)
{
mapplication=0;
mautowrap=1;
minmargins=0;
minsert=0;
mnl=0;
minvisable=0;
sparms();
attrib=0x0700;
fbsetw(screen,width*height,BLANK);
for(x=0;x<width;x++)
 if(x&7) tabs[x]=0;
 else tabs[x]=1;
x=0;
y=0;
mode=0;
stop=0;
dowrap=0;
sbot=height-1;
cpos(x,y);
}

/* Process a tty character */

void ttyout(unsigned char c)
{
switch(mode)
 {
/* Print mode */
case 5:
 if(c==6) break;
 pp(c);
 return;

/* No prefix characters */
case 0:
 switch(c)
  {
 case 6:        mode=5; return;         /* Pass-thru printing */
 case 7:        bell(); break;
 case 8:        dowrap=0; bs(); break;
 case 9:        tab(); break;
 case 10:
 case 11:
 case 12:       dowrap=0; if(mnl) cr(); lf(); break;
 case 13:       dowrap=0; cr(); break;
 case 27:       mode=1; return;
 case 0x84:     dowrap=0; lf(); break;
 case 0x85:     dowrap=0; cr(); lf(); break;
 case 0x88:     settab(); break;
 case 0x8d:     dowrap=0; ups(); break;
 case 0x9b:     begin(); mode=2; return;
 default:       if(minsert) insc(c);
                else type(c);
  }
 break;

/* ESC has been received */
case 1:
 switch(c)
  {
 case 27:       return;
 case '[':      begin(); mode=2; return;
 case 'D':      dowrap=0; lf(); break;
 case 'E':      dowrap=0; cr(); lf(); break;
 case 'H':      settab(); break;
 case 'M':      dowrap=0; ups(); break;
 case '#':      mode=3; return;
  }
 break;

/* ESC [ or 0x9B has been received */
case 2:
 /* Enter numbers */
 switch(c)
  {
 case '0': case '1': case '2': case '3': case '4':
 case '5': case '6': case '7': case '8': case '9':
  parry[nptr]=parry[nptr]*10+c-'0';
  return;
 case ';':      if(MAXPARMS== ++nptr) break;
                else return;
 case 27:       mode=1; return;
 case '[':      begin(); return;
 case 'c':      ttyinit(); break;
 case 'p':      dowrap=0; break;  /* Reset what? */
 case 'r':      dowrap=0; setregn(); break;
 case 'm':      setattrib(); break;
 case 'J':      clrs(); break;
 case 'K':      clrl(); break;
 case 'X':      clrc(); break;
 case 'H':      dowrap=0; pos(); break;
 case 'C':      dowrap=0; right(); break;
 case 'D':      dowrap=0; left(); break;
 case 'A':      dowrap=0; up(); break;
 case 'B':      dowrap=0; down(); break;
 case 'g':      clrt(); break;
 case 'M':      dell(); break;
 case 'L':      insl(); break;
 case 'P':      delc(); break;
 case '@':      inss(); break;
 case 'h':      hmode(); break;
 case 'l':      lmode(); break;
 case '!':
 case '?':      return;
  }
 break;

/* ESC # has been received */
case 3:
 switch(c)
  {
 case 27:       mode=1; return;
 case '[':      begin(); mode=2; return;
  }
 break;
 }
mode=0;
}

