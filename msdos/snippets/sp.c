#include <dos.h>
#include <conio.h>

/* These define which port */

#define a8250 0x3f8
#define airqveca ((unsigned long far *)0x00000030)
#define amask 16

/* This is the input buffer */

#define spbufsiz 512
char spbuf[spbufsiz];
unsigned spnew;
unsigned spold;

/* old serial port interrupt vector */

unsigned long spvect;

/* interrupt handler */

void interrupt spirqhdl(void)
{
spbuf[spnew++]=inportb(a8250);
if(spnew==spbufsiz) spnew=0;
outportb(0x20,0x20);
}

/* initialize the serial port */

void spopen(unsigned baud,char parity,unsigned bits,unsigned stopbits)
{
unsigned char x;
x=(stopbits-1)*4+(3&(bits-1));
switch(parity)
 {
 case 'n':
 break;
 case 'o':
 x+=8;
 break;
 case 'e':
 x+=24;
 }
spnew=0;
spold=0;

disable();
outportb(0x21,inportb(0x21)|amask);
spvect=*airqveca;
*airqveca=(unsigned long)spirqhdl;
outportb(a8250+3,128+x);
outport(a8250,115200/baud);
outportb(a8250+4,11);
outportb(a8250+3,x);
outportb(0x21,inportb(0x21)&(255-amask));
outportb(a8250+1,1);
enable();
}

/* return serial port to its original state */

void spclose(void)
{
disable();
*airqveca=spvect;
outportb(a8250+3,127&inportb(a8250+3));
outportb(a8250+1,0);
outportb(0x21,(inportb(0x21)&(255-amask)));
enable();
}

/* Return true if all bytes have been transmitted */

int spcanclose(void)
{
return(inportb(a8250+5)&64);
}

/* Return true when ready to accept next character to be transmitted */

int spcanputc(void)
{
return(inportb(a8250+5)&32);
}

/* send character */

void spputc(unsigned char ch)
{
while(!spcanputc());
outportb(a8250,ch);
}

/* return true when characters are in the buffer */

int spcangetc(void)
{
return(spold!=spnew);
}

/* return with next character received */

unsigned char spgetc(void)
{
unsigned char x;
while(!(spcangetc()));
x=spbuf[spold++];
if(spold==spbufsiz) spold=0;
return x;
}

main()
{
int x;
spopen(2400,'n',8,1);
do
 {
 if(spcangetc()) printf("%c",spgetc());
 if(kbhit())
  {
  x=getch();
  if(x==29) break;
  spputc(x);
  }
 }
 while(1);
spclose();
return 0;
}
