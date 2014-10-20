/* Simple serial port interface
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
#include <conio.h>
#include "sp.h"

#define INB(x) (inportb(0x80), inportb(0x80), inportb(x))
#define OUTB(x,c) (inportb(0x80), inportb(0x80), outportb((x),(c)))


int a8250= -1;			/* Port number for current 8250 */
int airqvec;			/* IRQ vector number for current 8250 */

char *spbuf=0;			/* pointer to buffer */
int spbufsiz;			/* size of buffer */
int spnew;			/* pointer to next free position for new char */
int spold;			/* pointer to oldest character in buffer */
int sptotal;

unsigned char sp8259;		/* original 8259 set-up */
long spvect;                    /* Original interrupt vector */

/* Interrupt handler */
interrupt far spirqhdl()
{
while(INB(a8250+5)&1)           /* Check if data available */
 if(sptotal!=spbufsiz)
  {
  spbuf[spnew++]=INB(a8250);    /* Store data */
  if(spnew==spbufsiz) spnew=0;
  ++sptotal;
  }
OUTB(0x20,0x20);                /* Acknowledge interrupt */
}

void spbaud(int port,long baud)
 {
 char c;
 baud=115200/baud;
 disable();
 OUTB(port+3,3+128);
 OUTB(port,baud);
 OUTB(port+1,baud>>8);
 OUTB(port+3,3);
 enable();
 }

void spopen(int port,int vect,int mask)
{
delay(0);                               /* Initialize msec timer */
spclose();				/* Close previous serial port */
if(!spbuf)				/* Initialize input buffer */
 {
 spbuf=(char *)malloc(spbufsiz=8192);
 sp8259=INB(0x21);
 }
a8250=port;
airqvec=vect;
spold=spnew=sptotal=0;
spvect=*(long far *)(vect*4);           /* Install interrupt handler */
*(long far *)(vect*4)=(long)spirqhdl;
OUTB(0x21,INB(0x21)|mask);      /* Disable interrupts in 8259 */
OUTB(a8250+3,INB(a8250+3)&127); /* Select data registers */
OUTB(a8250+4,11);               /* Enable interrupts in 8250 */
OUTB(a8250+1,1);                /* Enable interrupts in 8250 */
OUTB(0x21,INB(0x21)&(255-mask));/* Enable interrupts in 8259 */
enable();				/* Enable interrupts in 8088 */
}

void spclose(void)
{
if(a8250 != -1)
 {
 disable();
 OUTB(a8250+1,0);               /* Disable 8250 interrupts */
 OUTB(a8250+4,3);
 OUTB(0x21,sp8259);             /* Restore original 8259 setting */
                                /* Restore original interrupt vector */
 *(long far *)(airqvec*4)=spvect;
 enable();
 a8250= -1;
 }
}

int spcanclose(void)
{
return INB(a8250+5)&64;
}

int spcanputc(void)
{
return INB(a8250+5)&32;
}

void spputc(unsigned char ch)
{
while(!spcanputc());
OUTB(a8250,ch);
}

int spcangetc(void)
{
return sptotal;
}

unsigned char spgetc(void)
{
unsigned char x;
while(!spcangetc());
x=spbuf[spold++];
if(spold==spbufsiz) spold=0;
disable();
--sptotal;
enable();
return x;
}

/* Send break */

void spbreak(void)
{
while(!spcanputc());
if(a8250!= -1)
 {
 OUTB(a8250+3,INB(a8250+3)|0x40);
 delay(25);
 OUTB(a8250+3,INB(a8250+3)&~0x40);
 }
}

