/* Terminal interface for XENIX
   Copyright (C) 1991 Joseph H. Allen

This file is part of JOE (Joe's Own Editor)

JOE is free software; you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software
Foundation; either version 1, or (at your option) any later version. 

JOE is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.  

You should have received a copy of the GNU General Public License
along with JOE; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <stdio.h>
#include <signal.h>
#include <termio.h>
#include "config.h"
#include "heap.h"
#include "tty.h"

FILE *term=0;

/* Original state of the tty */

static struct termio oldterm;

/* Output buffer, index and size */

static char *obuf=0;
static int obufp=0;
static int obufsiz;

/* Baud rate */

unsigned long upc;
unsigned baud;

/* Baud rate code to baud rate translation table */

static int speeds[]=
{
B50,50,B75,75,B110,110,B134,134,B150,150,B200,200,B300,300,B600,600,B1200,1200,
B1800,1800,B2400,2400,B4800,4800,B9600,9600,EXTA,19200,EXTB,38400
};

/* 'Typeahead' and 'editor is about to exit' flag */

int have=0;
int leave=0;
int ttymode=0;

void sigjoe()
{
signal(SIGHUP,ttsig);
signal(SIGTERM,ttsig);
signal(SIGINT,SIG_IGN);
signal(SIGPIPE,SIG_IGN);
}

void signrm()
{
signal(SIGHUP,SIG_DFL);
signal(SIGTERM,SIG_DFL);
signal(SIGINT,SIG_DFL);
signal(SIGPIPE,SIG_DFL);
}

void ttopen()
{
sigjoe();
ttopnn();
}

void ttopnn()
{
int x;
struct termio newterm;
if(!term && !(term=fopen("/dev/tty","r+")))
 {
 fprintf(stderr,"Couldn\'t open tty\n");
 exit(1);
 }
if(ttymode) return;
else ttymode=1;
fflush(term);
ioctl(fileno(term),TCGETA,&oldterm);
newterm=oldterm;
newterm.c_lflag=0;
newterm.c_iflag&=~(ICRNL|IGNCR|INLCR);
newterm.c_oflag=0;
newterm.c_cc[VMIN]=1;
newterm.c_cc[VTIME]=0;
ioctl(fileno(term),TCSETAW,&newterm);
upc=0;
baud=9600;
for(x=0;x!=30;x+=2)
 if((newterm.c_cflag&CBAUD)==speeds[x])
  {
  baud=speeds[x+1];
  break;
  }
{
char *bs=getenv("BAUD");
if(bs)
 {
 sscanf(bs,"%u",&baud);
 }
}
upc=DIVIDEND/baud;
if(obuf) free(obuf);
if(!(TIMES*upc)) obufsiz=4096;
else
 {
 obufsiz=1000000/(TIMES*upc);
 if(obufsiz>4096) obufsiz=4096;
 }
if(!obufsiz) obufsiz=1;
obuf=(char *)malloc(obufsiz);
}

void ttclose()
{
ttclsn();
signrm();
}

void ttclsn()
{
int oleave=leave;
if(ttymode) ttymode=0;
else return;
leave=1;
ttflsh();
ioctl(fileno(term),TCSETAW,&oldterm);
leave=oleave;
}

void ttflsh()
{
if(obufp)
 {
 write(fileno(term),obuf,obufp);
 if(baud<38400) if(obufp*upc/1000) nap(obufp*upc/1000);
 obufp=0;
 }
if(!have && !leave) if(rdchk(fileno(term))>0) have=1;
}

int ttgetc()
{
unsigned char c;
ttflsh();
if(read(fileno(term),&c,1)<1) ttsig(0);
have=0;
return c;
}

void ttputc(c)
char c;
{
obuf[obufp++]=c;
if(obufp==obufsiz) ttflsh();
}

void ttputs(s)
char *s;
{
while(*s)
 {
 obuf[obufp++]= *(s++);
 if(obufp==obufsiz) ttflsh();
 }
}

void ttgtsz(x,y)
int *x, *y;
{
#ifdef TIOCGSIZE
struct ttysize getit;
#else
#ifdef TIOCGWINSZ
struct winsize getit;
#endif
#endif
*x=0; *y=0;
#ifdef TIOCGSIZE
if(ioctl(fileno(term),TIOCGSIZE,&getit)!= -1)
 {
 *x=getit.ts_cols;
 *y=getit.ts_lines;
 }
#else
#ifdef TIOCGWINSZ
if(ioctl(fileno(term),TIOCGWINSZ,&getit)!= -1)
 {
 *x=getit.ws_col;
 *y=getit.ws_row;
 }
#endif
#endif
}

void ttshell(cmd)
char *cmd;
{
int x,omode=ttymode;
char *s=getenv("SHELL");
if(!s) return;
ttclsn();
if(x=fork())
 {
 if(x!= -1) wait(0);
 if(omode) ttopnn();
 }
else
 {
 signrm();
 if(cmd) execl(s,s,"-c",cmd,NULL);
 else
  {
  fprintf(stderr,"You are at the command shell.  Type 'exit' to return\n");
  execl(s,s,NULL);
  }
 _exit(0);
 }
}

static int gotsig;

static void dosi()
{
gotsig=1;
}

void ttsusp()
{
#ifdef SIGCONT
int omode=ttymode;
ttclsn();
gotsig=0;
fprintf(stderr,"You have suspended the program.  Type \'fg\' to return\n");
signal(SIGCONT,dosi);
sigsetmask(sigmask(SIGCONT));
kill(0,SIGTSTP);
while(!gotsig) sigpause(0);
signal(SIGCONT,SIG_DFL);
if(omode) ttopnn();
#else
ttshell(NULL);
#endif
}
