#include <dos.h>

/* Monitor dos call */

unsigned long here;

void outc(unsigned char a)
{
_AL=a;
_AH=0xe;
_BX=7;
geninterrupt(0x10);
}

void dsp(unsigned char a)
{
if(a>=10) outc(a+'A'-10);
else outc(a+'0');
}

void sh(unsigned char a)
{
char x;
x=a>>4;
dsp(x);
dsp(a&0xf);
}

void show(unsigned a)
{
sh(a>>8);
sh(a&0xff);
}

void interrupt monitor(unsigned bp,unsigned di,unsigned si,unsigned ds,
unsigned es,unsigned dx,unsigned cx,unsigned bx,unsigned ax,unsigned ip,
unsigned cs,unsigned flags)
{
outc('\r');
outc(':');
outc(' ');
show(ax);
outc(' ');
show(bx);
outc(' ');
show(es);
outc(' ');
show(cx);
outc(' ');
show(dx);
outc(' ');
outc(':');
disable();
*(unsigned long far *)0x0000004c=here;
enable();
_ES=es;
_BX=bx;
_DX=dx;
_CX=cx;
_AX=ax;
geninterrupt(0x13);
ax=_AX;
flags=_FLAGS;
show(ax);
if(flags&1) outc('*');
outc('\r');
outc('\n');
disable();
*(unsigned long far *)0x0000004c=(unsigned long)monitor;
enable();
}

/* Install the disk monitor */

main(int argc,char *argv[])
{
printf("Hello\n");
disable();
here=0xc80001cf;
*((unsigned long far *)0x0000004c)=(unsigned long)monitor;
enable();
printf("There\n");
show((unsigned long)(here>>16));
show(here&0xffff);
outc('\r');
outc('\n');
_AX=0;
geninterrupt(0x13);
printf("You\n");
system(argv[1]);
disable();
*((unsigned long far *)0x0000004c)=here;
enable();
printf("Bye\n");
}
