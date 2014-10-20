/* Terminal emulator main loop
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

#include <stdio.h>
#include <bios.h>
#include <dos.h>
#include "sp.h"
#include "lp.h"
#include "tty.h"

/* Parallel port buffer */
#define pbufsiz 16384
unsigned char pbuf[pbufsiz];
int psize=0;
int pnew=0;
int pold=0;

/* Give character to parallel port buffer */
void pp(unsigned char c)
{
if(psize!=pbufsiz)
 {
 pbuf[pnew++]=c;
 if(pnew==pbufsiz) pnew=0;
 psize++;
 }
}

/* Port addresses and stuff */
int ports[4]={0x3f8,0x2f8,0x3E8,0x2E8};
int ints[4]={12,11,13,15};
int masks[4]={16,8,32,128};
int lports[3]={LP0,LP1,LP2};

/* Break interrupt */
int intflg=0;
interrupt far brkint()
{
intflg=1;
}

main(int argc,char *argv[])
{
long old;

int sport= -1;  /* Serial port number */
long baud= -1;  /* Baud rate */
int vecno= -1;  /* Vector number */

int port= -1;   /* Printer port number */
unsigned short c;

if(argc!=3)
 {
 oops:
 fprintf(stderr,"jterm COMx:[BAUD][,IRQ] LPy:\n");
 exit(1);
 }
sscanf(argv[1],"com%d:%ld,%d",&sport,&baud,&vecno);
if(sport== -1) sscanf(argv[1],"COM%d:%ld,%d",&sport,&baud,&vecno);

sscanf(argv[2],"lp%d:",&port);
if(port== -1) sscanf(argv[2],"LP%d:",&port);
--sport;
if(port<0 || port>2 || sport<0 || sport>3) goto oops;

/* Install break interrupt handler */
disable();
old= *(long far *)0x0000006C;
*(long far *)0x0000006C=(long)brkint;
enable();

if(vecno== -1) vecno=ints[sport];
else vecno+=8;

if(baud!= -1) spbaud(ports[sport],baud);

/* Open serial port */
spopen(ports[sport],vecno,1<<(vecno-8));

/* Open parallel port */
port=lports[port];

/* Initialize tty */
ttyinit();

loop:

/* Print chracters */
if(psize) if(pcheck(port))
 {
 pout(port,pbuf[pold++]);
 if(pold==pbufsiz) pold=0;
 psize--;
 }

if(intflg)
 {
 intflg=0;
 spbreak();
 }

/* Check keyboard */
/* The trick here is to use the DOS interrupt 6 instead of the bios interrupt.
 * This eliminates untold numbers of problems with ^C and Ctrl-Break for
 * some microsoft's-programmers-only-know reason.
 */
_DL=0xFF;
_AH=0x6;
geninterrupt(0x21);
__emit__(0x75,0x2,0x31,0xC0);   /* Clear AX if no chars ready */
c=_AX;
if(c)
 {
 c&=0xFF;
 if(!c)
  {
  _DL=0xFF;
  _AH=0x6;
  geninterrupt(0x21);
  c=_AX;
  c<<=8;
  }

 /* Check for exit key */
 if((c&0xff)==']'-64)
  {
  spclose();
  disable();
  *(long far *)0x0000006C=old;
  enable();
  bioscpos(0,height-1);
  exit(1);
  }

 if(c&0xff) spputc(c&0xff);	/* ASCII code */
 else switch(c)			/* Xenix special keys */
  {
  case 0x0300: spputc(0); break;
  case 0x4800: spputc('\033'); if(mapplication) spputc('O'), spputc('A'); else spputc('['), spputc('A'); break;
  case 0x5000: spputc('\033'); if(mapplication) spputc('O'), spputc('B'); else spputc('['), spputc('B'); break;
  case 0x4d00: spputc('\033'); if(mapplication) spputc('O'), spputc('C'); else spputc('['), spputc('C'); break;
  case 0x4b00: spputc('\033'); if(mapplication) spputc('O'), spputc('D'); else spputc('['), spputc('D'); break;
  case 0x4700: spputc('\033'), spputc('['), spputc('H'); break;
  case 0x4f00: spputc('\033'), spputc('['), spputc('F'); break;
  case 0x4900: spputc('\033'), spputc('['), spputc('I'); break;
  case 0x5100: spputc('\033'), spputc('['), spputc('G'); break;
  case 0x5200: spputc('\033'), spputc('['), spputc('L'); break;
  case 0x5300: spputc(127); break;
  case 0x0f00: spputc('\033'), spputc('['), spputc('Z'); break;
  case 0x3b00: spputc('\033'), spputc('['), spputc('M'); break;
  case 0x3c00: spputc('\033'), spputc('['), spputc('N'); break;
  case 0x3d00: spputc('\033'), spputc('['), spputc('O'); break;
  case 0x3e00: spputc('\033'), spputc('['), spputc('P'); break;
  case 0x3f00: spputc('\033'), spputc('['), spputc('Q'); break;
  case 0x4000: spputc('\033'), spputc('['), spputc('R'); break;
  case 0x4100: spputc('\033'), spputc('['), spputc('S'); break;
  case 0x4200: spputc('\033'), spputc('['), spputc('T'); break;
  case 0x4300: spputc('\033'), spputc('['), spputc('U'); break;
  case 0x4400: spputc('\033'), spputc('['), spputc('V'); break;
  case 0x5400: spputc('\033'), spputc('['), spputc('Y'); break;
  case 0x5500: spputc('\033'), spputc('['), spputc('Z'); break;
  case 0x5600: spputc('\033'), spputc('['), spputc('a'); break;
  case 0x5700: spputc('\033'), spputc('['), spputc('b'); break;
  case 0x5800: spputc('\033'), spputc('['), spputc('c'); break;
  case 0x5900: spputc('\033'), spputc('['), spputc('d'); break;
  case 0x5a00: spputc('\033'), spputc('['), spputc('e'); break;
  case 0x5b00: spputc('\033'), spputc('['), spputc('f'); break;
  case 0x5c00: spputc('\033'), spputc('['), spputc('g'); break;
  case 0x5d00: spputc('\033'), spputc('['), spputc('h'); break;
  case 0x5e00: spputc('\033'), spputc('['), spputc('k'); break;
  case 0x5f00: spputc('\033'), spputc('['), spputc('l'); break;
  case 0x6000: spputc('\033'), spputc('['), spputc('m'); break;
  case 0x6100: spputc('\033'), spputc('['), spputc('n'); break;
  case 0x6200: spputc('\033'), spputc('['), spputc('o'); break;
  case 0x6300: spputc('\033'), spputc('['), spputc('p'); break;
  case 0x6400: spputc('\033'), spputc('['), spputc('q'); break;
  case 0x6500: spputc('\033'), spputc('['), spputc('r'); break;
  case 0x6600: spputc('\033'), spputc('['), spputc('s'); break;
  case 0x6700: spputc('\033'), spputc('['), spputc('t'); break;
  case 0x6800: spputc('\033'), spputc('['), spputc('w'); break;
  case 0x6900: spputc('\033'), spputc('['), spputc('x'); break;
  case 0x6a00: spputc('\033'), spputc('['), spputc('y'); break;
  case 0x6b00: spputc('\033'), spputc('['), spputc('z'); break;
  case 0x6c00: spputc('\033'), spputc('['), spputc('@'); break;
  case 0x6d00: spputc('\033'), spputc('['), spputc('['); break;
  case 0x6e00: spputc('\033'), spputc('['), spputc('\\'); break;
  case 0x6f00: spputc('\033'), spputc('['), spputc(']'); break;
  case 0x7000: spputc('\033'), spputc('['), spputc('^'); break;
  case 0x7100: spputc('\033'), spputc('['), spputc('_'); break;
  }
 }

/* Check serial port */
if(spcangetc()) ttyout(spgetc());
goto loop;
}
