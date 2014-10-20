/* Keyboard interrupt handler and primary ms-dos multi-tasking interface */

#include <dos.h>
#include "kbdhdl.h"

extern void doschain(void);
extern void (far *odos)(void);
extern int indos;		/* Set when in a dos function */
extern int inexec;		/* Set when in the EXEC function */

#define sstoax() __emit__(0x8c,0xd0)
#define sptoax() __emit__(0x89,0xe0)
#define axtoss() __emit__(0x8e,0xd0)
#define axtosp() __emit__(0x89,0xc4)

#define simint(a) __emit__(0x9c,0xfa); (a)()

#define kimask 2		/* This is the 8259 kbd interrupt mask bit */

int psp;			/* This program's PSP */
int ine=0;			/* Set if running an editor function */
int brkflg=0;			/* Set whenever Ctrl-break is hit */
int chk=0;			/* Set when a key was hit while in dos */

void (far *oldhdl)(void);	/* Original handlers which we replace */
void (far *kkkhdl)(void);
void (far *brkhdl)(void);

/* Replacement for 'int 0x16', the bios keyboard function */
/* This is replaced so that we can simulate terminal
 * input on the background DOS process
 */

#define stisize 64
unsigned char stibuf[stisize];
unsigned stinew;
unsigned stiold;

void interrupt kkk(unsigned bp,unsigned di,unsigned si,unsigned ds,
unsigned es,unsigned dx,unsigned cx,unsigned bx,unsigned ax,unsigned ip,
unsigned cs,unsigned flags)
{
enable();
if((ax&0xff00)==0x0000)
 {
 while(stinew==stiold);
 ax=stibuf[stiold]+(((unsigned)stibuf[stiold+1])<<8);
 stiold+=2;
 if(stiold==stisize) stiold=0;
 }
else if((ax&0xff00)==0x0100)
 {
 if(stinew!=stiold)
  {
  ax=stibuf[stiold]+(((unsigned)stibuf[stiold+1])<<8);
  flags&=(0xff-0x40);
  }
 else flags|=0x40;
 }
else if((ax&0xff00)==0x0200)
 ax=*(unsigned char far *)0x417+0x200;
}

/* Simulate terminal input */

void sti(unsigned char ascii,unsigned char scan)
{
disable();
stibuf[stinew++]=ascii;
stibuf[stinew++]=scan;
if(stinew==stisize) stinew=0;
enable();
}

/* Replacement break key handler */

void interrupt brkh()
{
brkflg=1;
}

/* Return true if there are any keys */

int kcheck(void)
{
_BX=0;
_AH=1;
simint(kkkhdl);
__emit__(0x74,1,0x43);
if(_BX) return 1;
else return 0;
}

/* Dump any keys in queue */

void kdump(void)
{
while(kcheck())
 {
 _AH=0;				/* Get the character */
 simint(kkkhdl);
 }
}

/* Replacement keyboard interrupt handler */

unsigned stackflg=0,oldss,oldsp,stackseg,stackp;

void interrupt dos(unsigned bp,unsigned di,unsigned si,unsigned ds,
unsigned es,unsigned dx,unsigned cx,unsigned bx,unsigned ax,unsigned ip,
unsigned cs,unsigned flags);

void khandle(void)
{
/* Saved context information */

int opsp;
unsigned dtaseg;
unsigned dtaofst;

/* Install normal dos vector for us */

disable();
*(void far * far *)0x84=odos;
enable();

/* Switch PSP */

_AH=0x51;
geninterrupt(0x21);
opsp=_BX;
_BX=psp;
_AH=0x50;
geninterrupt(0x21);

/* Save DTA */

_AH=0x2f;
geninterrupt(0x21);
dtaofst=_BX;
dtaseg=_ES;

/* Other information that should be preserved here:
 *
 * critical error handler (yes)
 * extended error information (yes- see green book for setting error byte)
 * current drive (yes)
 * current path (yes)
 *
 * Also:
 *  should intercept int20 for exiting
 *  should intercept absolute disk read/write
 *  should intercept TSR int (int 27H)
 */

up:				/* Test for keys */
disable();			/* Disable keyboard interrupts */
outportb(0x21,inportb(0x21)|kimask);
enable();
if(kcheck())
 {
 char scan, ascii;
 disable();			/* Reenable keyboard interrupts */
 outportb(0x21,inportb(0x21)&~kimask);
 enable();
 _AH=0;				/* Get the character */
 simint(kkkhdl);
 ascii=_AL;
 scan=_AH;
 if(key(ascii,scan)) goto bye;
 goto up;			/* See if any more keys to process */
 }
ine=0;
bye:;

/* Restore DTA */

_DX=dtaofst;
_DS=dtaseg;
_AH=0x1a;
geninterrupt(0x21);
_DS=_CS;

/* Restore PSP */

_BX=opsp;
_AH=0x50;
geninterrupt(0x21);

/* Install our DOS interceptor */

disable();
*(void far * far *)0x84=MK_FP(_CS,doschain);
enable();
}

void interrupt dos(unsigned bp,unsigned di,unsigned si,unsigned ds,
unsigned es,unsigned dx,unsigned cx,unsigned bx,unsigned ax,unsigned ip,
unsigned cs,unsigned flags)
{
if(chk && ine==0)
 {
 ine=1;
 if(_SS!=stackseg)
  {
  stackflg=1;
  sstoax();
  oldss=_AX;
  sptoax();
  oldsp=_AX;
  disable();
  _AX=stackseg;
  axtoss();
  _AX=stackp;
  axtosp();
  enable();
  }
 khandle();
 if(stackflg)
  {
  stackflg=0;
  _AX=oldss;
  axtoss();
  _AX=oldsp;
  axtosp();
  }
 chk=0;
 disable();
 outportb(0x21,inportb(0x21)&~kimask);
 enable();
 }
}

void interrupt kbdhdl()
{
simint(oldhdl);
disable();
if(stackflg==0 && ine==0 && indos==0 && inexec==0 && chk==0)
 {
 ine=1;
 chk=1;
 enable();
 if(_SS!=stackseg)
  {
  stackflg=1;
  sstoax();
  oldss=_AX;
  sptoax();
  oldsp=_AX;
  disable();
  _AX=stackseg;
  axtoss();
  _AX=stackp;
  axtosp();
  enable();
  }
 khandle();
 if(stackflg)
  {
  disable();
  _AX=oldss;
  axtoss();
  _AX=oldsp;
  axtosp();
  enable();
  stackflg=0;
  }
 chk=0;
 disable();
 outportb(0x21,inportb(0x21)&~kimask);
 enable();
 }
else
 {
 if(indos || inexec)
  {
  chk=1;
  return;
  }
 enable();
 }
}

void kopen(void)
{
/* Remeber where our stack is */

sstoax();
stackseg=_AX;
sptoax();
stackp=_AX-512;	/* 512 bytes of background stack, the rest foreground */

/* Remember which PSP we are */

_AH=0x51;
geninterrupt(0x21);
psp=_BX;

/* Install vectors */

disable();
oldhdl=*(void far * far *)0x24;
*(void far * far *)0x24=MK_FP(_CS,kbdhdl);

kkkhdl=*(void far * far *)0x58;
*(void far * far *)0x58=MK_FP(_CS,kkk);

brkhdl=*(void far * far *)0x6c;
*(void far * far *)0x6c=MK_FP(_CS,brkh);

odos=*(void far * far *)0x84;
*(void far * far *)0x84=MK_FP(_CS,doschain);
enable();
}

/* Restore vectors */

void kclose(void)
{
disable();
*(void far * far *)0x24=oldhdl;
*(void far * far *)0x58=kkkhdl;
*(void far * far *)0x6c=brkhdl;
*(void far * far *)0x84=odos;
enable();
}
