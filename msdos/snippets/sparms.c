/* Automagically determine screen parameters */

#include <stdio.h>
#include <dos.h>

char far *screen;
unsigned height;
unsigned width;

void sparms(void)
{
union REGS regs;
regs.h.ah=0x0f;
int86(0x10,&regs,&regs);
if(regs.h.al==7) screen=(char far *)0xb0000000;
else screen=(char far *)0xb8000000;
width=regs.h.ah;
height=*(unsigned char far *)0x00400084;
if(!height) height=25;
}

main()
{
sparms();
printf("Screen address: $%lx\n",screen);
printf("Width %d\n",width);
printf("Height %d\n",height);
}

