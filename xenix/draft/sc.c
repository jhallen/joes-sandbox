/* Xenix graphics interface */

#include <stdio.h>
#include <fcntl.h>
#include <sys/machdep.h>
#include "zstr.h"
#include "sc.h"

char *hscreen;
int sc_fd= -1;
int width;
int height;
int stype;
int omode;

void (*set)();
void (*clr)();
void (*inv)();
void (*byte)();

void hbyte(int x,int y,int byte)
{
hscreen[0x2000*(y&3)+90*(y>>2)+x]=byte;
}

void hset(int x,int y)
{
y=height-1-y;
hscreen[0x2000*(y&3)+90*(y>>2)+(x>>3)]|=(128>>(x&7));
}

void hclr(int x,int y)
{
y=height-1-y;
hscreen[0x2000*(y&3)+90*(y>>2)+(x>>3)]&=~(128>>(x&7));
}

void hinv(int x,int y)
{
y=height-1-y;
hscreen[0x2000*(y&3)+90*(y>>2)+(x>>3)]^=(128>>(x&7));
}

void vgbyte(int x,int y,int byte)
{
hscreen[16384+y*640+x]=byte;
}

void vgset(int x,int y)
{
y=height-1-y;
hscreen[16384+y*80+(x>>3)]|=(128>>(x&7));
}

void vgclr(int x,int y)
{
y=height-1-y;
hscreen[16384+y*80+(x>>3)]&=~(128>>(x&7));
}

void vginv(int x,int y)
{
y=height-1-y;
hscreen[16384+y*80+(x>>3)]^=(128>>(x&7));
}

struct
 {
 int width;
 int height;
 int gmode;
 int tmode;
 void (*set)(int x,int y);
 void (*clr)(int x,int y);
 void (*inv)(int x,int y);
 void (*byte)(int x,int y,int byte);
 } modes[]=
{
{0,0,0},
{720,348,M_HGC_P0,M_B80x25,hset,hclr,hinv,hbyte},	/* Herculese */
{640,200,M_BG640,M_C80x25,vgset,vgclr,vginv,vgbyte},	/* CGA */
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{640,480,M_VGA13,M_VGA_80x25,vgset,vgclr,vginv,vgbyte}	/* VGA */
};

void graph(void)
{
if(sc_fd== -1)
 {
 sc_fd=open("/dev/tty",O_RDWR);
 if(sc_fd== -1) return;
 hscreen=ioctl(sc_fd,MAPCONS,0);
 stype=ioctl(sc_fd,CONS_CURRENT,0);
 omode=ioctl(sc_fd,CONS_GET,0);
 }
ioctl(sc_fd,omode|MODESWITCH,0);
ioctl(sc_fd,modes[stype].gmode|MODESWITCH,0);
width=modes[stype].width;
height=modes[stype].height;
set=modes[stype].set;
clr=modes[stype].clr;
inv=modes[stype].inv;
byte=modes[stype].byte;
}

void text(void)
{
if(sc_fd== -1)
 {
 sc_fd=open("/dev/tty",O_RDWR);
 if(sc_fd== -1) return;
 hscreen=ioctl(sc_fd,MAPCONS,0);
 stype=ioctl(sc_fd,CONS_CURRENT,0);
 omode=ioctl(sc_fd,CONS_GET,0);
 }
ioctl(sc_fd,MODESWITCH|M_B80x25,0);
}

void dot(x,y,m)
{
if(x>=width || y>=height ||
   x<0 || y<0) return;
switch(m)
 {
case CLR: clr(x,y); break;
case SET: set(x,y); break;
case INV: inv(x,y); break;
 }
}

void lineto(x,y,i,j,m)
{
int dist;	/* Distance */
int incx;	/* X increment value */
int incy;	/* Y increment value */
int accu;	/* Accumulator */
if(x<0) return;
if(y<0) return;
if(x>=width) return;
if(y>=height) return;
if(x+i<0) return;
if(y+j<0) return;
if(x+i>=width) return;
if(y+j>=height) return;
dot(x,y,m);
if(i==0 && j==0) return;
dot(x+i,y+j,m);
if(Iabs(i)<Iabs(j))
 {
 if(i>0) incx=1;
 else incx= -1, i= -i;
 if(j>0) dist=j, incy=1;
 else dist= -j, incy= -1, j= -j;
 accu=j/2;
 do
  {
  dot(x,y,m);
  y+=incy;
  accu+=i;
  if(accu>=j)
   {
   accu-=j;
   x+=incx;
   }
  } while(--dist);
 }
else
 {
 if(i>0) dist=i, incx=1;
 else dist= -i, incx= -1, i= -i;
 if(j>0) incy=1;
 else incy= -1, j= -j;
 accu=i/2;
 do
  {
  dot(x,y,m);
  x+=incx;
  accu+=j;
  if(accu>=i)
   {
   accu-=i;
   y+=incy;
   }
  } while(--dist);
 }
}
