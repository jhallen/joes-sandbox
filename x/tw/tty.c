// Control the tty

#define _STREAM_COMPAT 1

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <termios.h>
#include <fstream>
#include <iostream>

using namespace std;

#include "tty.h"

/* Set if we're leaving */

int leave=0;

/* Global configuration variables */

int noxon=0;	/* Set if ^S/^Q processing should be disabled */
int Baud=0;	/* Baud rate from joerc, cmd line or environment */

/* Original state of tty */

struct termios oldterm;

/* The baud rate */

unsigned baud;		/* Bits per second */

/* TTY Speed code to baud-rate conversion table (this is dumb- is it really
 * too much to ask for them to just use an integer for the baud-rate?)
 */

static int speeds[]=
  {
  B50,50,B75,75,B110,110,B134,134,B150,150,B200,200,B300,300,B600,600,
  B1200,1200,B1800,1800,B2400,2400,B4800,4800,B9600,9600
#ifdef EXTA
  ,EXTA,19200
#endif
#ifdef EXTB
  ,EXTB,38400
#endif
#ifdef B19200
  ,B19200,19200
#endif
#ifdef B38400
  ,B38400,38400
#endif
  };

/* TTY mode flag.  1 for open, 0 for closed */

static int ttymode=0;

/* Signal state flag.  1 for joe, 0 for normal */

static int ttysig=0;

/* Set signals for JOE */

void sigjoe()
  {
  if(ttysig) return;
  ttysig=1;
  signal(SIGHUP,ttsig);
  signal(SIGTERM,ttsig);
  signal(SIGINT,SIG_IGN);
  signal(SIGPIPE,SIG_IGN);
  }

/* Restore signals for exiting */

void signrm()
  {
  if(!ttysig) return;
  ttysig=0;
  signal(SIGHUP,SIG_DFL);
  signal(SIGTERM,SIG_DFL);
  signal(SIGINT,SIG_DFL);
  signal(SIGPIPE,SIG_DFL);
  }

/* Open terminal and set signals */

void ttopen()
  {
  sigjoe();
  ttopnn();
  }

/* Close terminal and restore signals */

void ttclose()
  {
  ttclsn();
  signrm();
  }

/* Window size interrupt handler */

static int winched=0;

static void winchd(int n)
  {
  ++winched;
#ifdef SIGWINCH
  signal(SIGWINCH,winchd);
#endif
  }

/* Open terminal */

void ttopnn()
  {
  int x, bbaud;
 
  struct termios newterm;
 
#ifdef SIGWINCH
  signal(SIGWINCH,winchd);
#endif
 
  if(ttymode) return;
  ttymode=1;
  cout.flush();
 
  tcgetattr(0 /* fileno(cin) */,&oldterm);
  newterm=oldterm;
  newterm.c_lflag=0;
  if(noxon)  newterm.c_iflag&=~(ICRNL|IGNCR|INLCR|IXON|IXOFF);
  else newterm.c_iflag&=~(ICRNL|IGNCR|INLCR);
  newterm.c_oflag=0;
  newterm.c_cc[VMIN]=1;
  newterm.c_cc[VTIME]=0;
  tcsetattr(0 /* fileno(cin) */,TCSADRAIN,&newterm);
  bbaud=cfgetospeed(&newterm);

  baud=9600;
  for(x=0;x!=30;x+=2)
    if(bbaud==speeds[x])
      {
      baud=speeds[x+1];
      break;
      }
  if(Baud) baud=Baud;
  }

/* Close terminal */

void ttclsn()
  {
  int oleave;
 
  if(ttymode) ttymode=0;
  else return;
 
  oleave=leave; leave=1;
 
  cout.flush();
 
  tcsetattr(0 /* fileno(cin)*/,TCSADRAIN,&oldterm);
 
  leave=oleave;
  }

/* Get window size */

void ttgtsz(int *x,int *y)
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
  if(ioctl(0 /* fileno(cin)*/,TIOCGSIZE,&getit)!= -1)
    {
    *x=getit.ts_cols;
    *y=getit.ts_lines;
    }
#else
#ifdef TIOCGWINSZ
  if(ioctl(0 /* fileno(cin) */,TIOCGWINSZ,&getit)!= -1)
    {
    *x=getit.ws_col;
    *y=getit.ws_row;
    }
#endif
#endif
  }

void ttshell(char *cmd)
  {
  int x,omode=ttymode;
  char *s=getenv("SHELL");
  if(!s) return;
  ttclsn();
  if(x=fork())
    {
    if(x!= -1) wait(NULL);
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

void ttsusp()
  {
  int omode;
#ifdef SIGTSTP
  omode=ttymode;
  ttclsn();
  fprintf(stderr,"You have suspended the program.  Type 'fg' to return\n");
  kill(0,SIGTSTP);
  if(omode) ttopnn();
#else
  ttshell(NULL);
#endif
  }
