/* UNIX Tty and Process interface
   Copyright (C) 1992 Joseph H. Allen

This file is part of JOE (Joe's Own Editor)

JOE is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation; either version 1, or (at your option) any later version.  

JOE is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.  

You should have received a copy of the GNU General Public License along with 
JOE; see the file COPYING.  If not, write to the Free Software Foundation, 
675 Mass Ave, Cambridge, MA 02139, USA.  */ 

/** System include files **/

/* These should exist on every UNIX system */
#include <sys/types.h>
#include <sys/stat.h>

/* SCO UNIX doesn't like <sys/ioctl.h> */
#ifndef M_COFF
#include <sys/ioctl.h>
#endif

#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include "main.h"
extern int errno;

int idleout=1;

#include <sys/param.h>

#include "config.h"

/* We use the defines in sys/ioctl to determine what type
 * tty interface the system uses and what type of system
 * we actually have.
 */
#ifdef TTYPOSIX

#ifdef SYSPOSIX
#include <sys/termios.h>
#else
#include <termios.h>
#endif

#else
#ifdef TTYSV

#ifdef SYSSV
#include <sys/termio.h>
#else
#include <termio.h>
#endif

#else
#include <sgtty.h>
#endif
#endif

/* If the signal SIGVTALRM exists, assume we have the setitimer system call
 * and the include file necessary for it.  I'm not so sure that this method
 * of detecting 'setitimer' is foolproof, so this is the only place where
 * SIGVTALRM will be checked... after here the itimer code will look for
 * ITIMER_REAL (which is defined in sys/time.h).
 */
#ifndef _SEQUENT_
#ifdef SIGVTALRM
#include <sys/time.h>
#endif
#endif

/* I'm not sure if SCO_UNIX and ISC have __svr4__ defined, but I think
   they might */
#ifdef M_COFF
#include <sys/stream.h>
#include <sys/ptem.h>
#ifndef __svr4__
#define __svr4__ 1
#endif
#endif

#ifdef ISC
#ifndef __svr4__
#define __svr4__ 1
#endif
#endif

#ifdef __svr4__
#include <stropts.h>
#endif

/* JOE include files */

#include "config.h"
#include "path.h"
#include "tty.h"

/** Aliased defines **/

/* O_NDELAY, O_NONBLOCK, and FNDELAY are all synonyms for placing a descriptor
 * in non-blocking mode; we make whichever one we have look like O_NDELAY
 */
#ifndef O_NDELAY
#ifdef O_NONBLOCK
#define O_NDELAY O_NONBLOCK
#endif
#ifdef FNDELAY
#define O_NDELAY FNDELAY
#endif
#endif

/* Some systems define this, some don't */
#ifndef sigmask
#define sigmask(x) (1<<((x)-1))
#endif

/* Some BSDs don't have TILDE */
#ifndef TILDE
#define TILDE 0
#endif

/* Global configuration variables */

int noxon=0;	/* Set if ^S/^Q processing should be disabled */
int Baud=0;	/* Baud rate from joerc, cmd line or environment */

/* The terminal */

FILE *termin=0;
FILE *termout=0;

/* Original state of tty */

#ifdef TTYPOSIX
struct termios oldterm;
#else
#ifdef TTYSV
static struct termio oldterm;
#else
static struct sgttyb oarg;
static struct tchars otarg;
static struct ltchars oltarg;
#endif
#endif

/* Output buffer, index and size */

char *obuf=0;
int obufp=0;
int obufsiz;

/* The baud rate */

unsigned baud;		/* Bits per second */
unsigned long upc;	/* Microseconds per character */

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

/* Input buffer */

int have=0;			/* Set if we have pending input */
static unsigned char havec;	/* Character read in during pending input check */

/* TTY mode flag.  1 for open, 0 for closed */

static int ttymode=0;

/* Signal state flag.  1 for joe, 0 for normal */

static int ttysig=0;

/* Stuff for shell windows */

static int kbdpid;		/* PID of kbd client */
static int ackkbd= -1;		/* Editor acks keyboard client to this */

static int mpxfd;		/* Editor reads packets from this fd */
static int mpxsfd;		/* Clients send packets to this fd */

static int nmpx=0;
static int acpt=MAXINT;	/* =MAXINT if we have last packet */

struct packet
 {
 MPX *who;
 int size;
 int ch;
 char data[1024];
 } pack;

MPX asyncs[NPROC];

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

static void winchd()
 {
 ++winched;
/*
#ifdef SIGWINCH
 signal(SIGWINCH,winchd);
#endif
*/
 }

/* Second ticker */

int ticked=0;
static void dotick() { ticked=1; /* dostaupd=1;*/ }
void tickoff() { alarm(0); }

#ifdef SA_INTERRUPT
struct sigaction vnew={dotick,0,SA_INTERRUPT};
#else
#ifdef SV_INTERRUPT
struct sigvec vnew={dotick,0,SV_INTERRUPT};
#endif
#endif

void tickon()
 {
 ticked=0;
#ifdef SA_INTERRUPT
 sigaction(SIGALRM,&vnew,(struct sigaction *)0);
#else
#ifdef SV_INTERRUPT
 sigvec(SIGALRM,&vnew,(struct sigvec *)0);
#else
 signal(SIGALRM,dotick);
#endif
#endif
 alarm(1);
 }

/* Open terminal */

void ttopnn()
 {
 int x, bbaud;
 
#ifdef TTYPOSIX
 struct termios newterm;
#else
#ifdef TTYSV
 struct termio newterm;
#else
 struct sgttyb arg;
 struct tchars targ;
 struct ltchars ltarg;
#endif
#endif
 
 if(!termin)
  {
  if(idleout) termin=stdin, termout=stdout;
  else if((!(termin=fopen("/dev/tty","r")) || !(termout=fopen("/dev/tty","w"))))
   {
   fprintf(stderr,"Couldn\'t open /dev/tty\n");
   exit(1);
   }
#ifdef SIGWINCH
  signal(SIGWINCH,winchd);
#endif
  tickon();
  }
 
 if(ttymode) return;
 ttymode=1;
 fflush(termout);
 
#ifdef TTYPOSIX
 tcgetattr(fileno(termin),&oldterm);
 newterm=oldterm;
 newterm.c_lflag=0;
 if(noxon)  newterm.c_iflag&=~(ICRNL|IGNCR|INLCR|IXON|IXOFF);
 else newterm.c_iflag&=~(ICRNL|IGNCR|INLCR);
 newterm.c_oflag=0;
 newterm.c_cc[VMIN]=1;
 newterm.c_cc[VTIME]=0;
 tcsetattr(fileno(termin),TCSADRAIN,&newterm);
 bbaud=cfgetospeed(&newterm);
#else
#ifdef TTYSV
 ioctl(fileno(termin),TCGETA,&oldterm);
 newterm=oldterm;
 newterm.c_lflag=0;
 if(noxon)  newterm.c_iflag&=~(ICRNL|IGNCR|INLCR|IXON|IXOFF);
 else newterm.c_iflag&=~(ICRNL|IGNCR|INLCR);
 newterm.c_oflag=0;
 newterm.c_cc[VMIN]=1;
 newterm.c_cc[VTIME]=0;
 ioctl(fileno(termin),TCSETAW,&newterm);
 bbaud=(newterm.c_cflag&CBAUD);
#else
 ioctl(fileno(termin),TIOCGETP,&arg);
 ioctl(fileno(termin),TIOCGETC,&targ);
 ioctl(fileno(termin),TIOCGLTC,&ltarg);
 oarg=arg; otarg=targ; oltarg=ltarg;
 arg.sg_flags=( (arg.sg_flags&~(ECHO|CRMOD|XTABS|ALLDELAY|TILDE) ) | CBREAK) ;
 if(noxon) targ.t_startc= -1, targ.t_stopc= -1;
 targ.t_intrc= -1;
 targ.t_quitc= -1;
 targ.t_eofc= -1;
 targ.t_brkc= -1;
 ltarg.t_suspc= -1;
 ltarg.t_dsuspc= -1;
 ltarg.t_rprntc= -1;
 ltarg.t_flushc= -1;
 ltarg.t_werasc= -1;
 ltarg.t_lnextc= -1;
 ioctl(fileno(termin),TIOCSETN,&arg);
 ioctl(fileno(termin),TIOCSETC,&targ);
 ioctl(fileno(termin),TIOCSLTC,&ltarg);
 bbaud=arg.sg_ospeed;
#endif
#endif
 
 baud=9600; upc=0;
 for(x=0;x!=30;x+=2)
  if(bbaud==speeds[x])
   {
   baud=speeds[x+1];
   break;
   }
 if(Baud) baud=Baud;
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

/* Close terminal */

void ttclsn()
 {
 int oleave;
 
 if(ttymode) ttymode=0;
 else return;
 
 oleave=leave; leave=1;
 
 ttflsh();
 
#ifdef TTYPOSIX
 tcsetattr(fileno(termin),TCSADRAIN,&oldterm);
#else
#ifdef TTYSV
 ioctl(fileno(termin),TCSETAW,&oldterm);
#else
 ioctl(fileno(termin),TIOCSETN,&oarg);
 ioctl(fileno(termin),TIOCSETC,&otarg);
 ioctl(fileno(termin),TIOCSLTC,&oltarg);
#endif
#endif
 
 leave=oleave;
 }

/* Timer interrupt handler */

static int yep;
static void dosig() { yep=1; } 

/* FLush output and check for typeahead */

#ifdef ITIMER_REAL
#ifdef SIG_SETMASK
maskit()
 {
 sigset_t set;
 sigemptyset(&set);
 sigaddset(&set,SIGALRM);
 sigprocmask(SIG_SETMASK,&set,NULL);
 }
unmaskit()
 {
 sigset_t set;
 sigemptyset(&set);
 sigprocmask(SIG_SETMASK,&set,NULL);
 }
pauseit()
 {
 sigset_t set;
 sigemptyset(&set);
 sigsuspend(&set);
 }
#else
maskit() { sigsetmask(sigmask(SIGALRM)); }
unmaskit() { sigsetmask(0); }
pauseit() { sigpause(0); }
#endif
#endif

#ifndef HZ
#define HZ 10
#endif

int ttflsh()
 {
 /* Flush output */
 if(obufp)
  {
  unsigned long usec=obufp*upc;		/* No. usecs this write should take */
#ifdef ITIMER_REAL
  if(usec>=500000/HZ && baud<9600)
   {
   struct itimerval a,b;
   a.it_value.tv_sec=usec/1000000;
   a.it_value.tv_usec=usec%1000000;
   a.it_interval.tv_usec=0;
   a.it_interval.tv_sec=0;
   alarm(0);
   signal(SIGALRM,dosig); yep=0;
   maskit();
   setitimer(ITIMER_REAL,&a,&b);
   jwrite(fileno(termout),obuf,obufp);
   while(!yep) pauseit(0);
   unmaskit();
   tickon();
   }
  else jwrite(fileno(termout),obuf,obufp);
 
#else
 
  jwrite(fileno(termout),obuf,obufp);

#ifdef FIORDCHK
  if(baud<9600 && usec/1000) nap(usec/1000);
#endif

#endif
 
  obufp=0;
  }
 
 /* Ack previous packet */
 if(ackkbd!= -1 && acpt!=MAXINT && !have)
  {
  char c=0;
  if(pack.who && pack.who->func) jwrite(pack.who->ackfd,&c,1);
  else jwrite(ackkbd,&c,1);
  acpt=MAXINT;
  }
 
 /* Check for typeahead or next packet */
 
 if(!have && !leave)
  if(ackkbd!= -1)
   {
   fcntl(mpxfd,F_SETFL,O_NDELAY);
   if(read(mpxfd,&pack,sizeof(struct packet)-1024)>0)
    {
    fcntl(mpxfd,F_SETFL,0);
    jread(mpxfd,pack.data,pack.size);
    have=1, acpt=pack.ch;
    }
   else fcntl(mpxfd,F_SETFL,0);
   }
  else
   {
   /* Set terminal input to non-blocking */
   fcntl(fileno(termin),F_SETFL,O_NDELAY);
  
   /* Try to read */
   if(read(fileno(termin),&havec,1)==1) have=1;
  
   /* Set terminal back to blocking */
   fcntl(fileno(termin),F_SETFL,0);
   }
 return 0;
 }

/* Read next character from input */

void mpxdied();

int ttgetc()
 {
 int stat;
 loop:
 ttflsh();
 while(winched) winched=0, edupd(1), ttflsh();
 if(ticked) edupd(0), ttflsh(), tickon();
 if(ackkbd!= -1)
  {
  if(!have)					/* Wait for input */
   {
   stat=read(mpxfd,&pack,sizeof(struct packet)-1024);
   if(pack.size && stat>0) jread(mpxfd,pack.data,pack.size);
   else if(stat<1)
    if(winched || ticked) goto loop;
    else ttsig(0);
   acpt=pack.ch;
   }
  have=0;
  if(pack.who)					/* Got bknd input */
   {
   if(acpt!=MAXINT)
    {
    if(pack.who->func)
     pack.who->func(pack.who->object,pack.data,pack.size),
     edupd(1);
    }
   else mpxdied(pack.who);
   goto loop;
   }
  else
   {
   if(acpt!=MAXINT) return acpt;
   else { ttsig(0); return 0; }
   }
  }
 if(have) have=0;
 else
  {
  if(read(fileno(termin),&havec,1)<1)
   if(winched || ticked) goto loop;
   else ttsig(0);
  }
 return havec;
 }

/* Write string to output */

void ttputs(s)
char *s;
 {
 while(*s)
  {
  obuf[obufp++]= *s++;
  if(obufp==obufsiz) ttflsh();
  }
 }

/* Get window size */

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
 if(ioctl(fileno(termout),TIOCGSIZE,&getit)!= -1)
  {
  *x=getit.ts_cols;
  *y=getit.ts_lines;
  }
#else
#ifdef TIOCGWINSZ
 if(ioctl(fileno(termout),TIOCGWINSZ,&getit)!= -1)
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
 tickoff();
#ifdef SIGTSTP
 omode=ttymode;
 ttclsn();
 fprintf(stderr,"You have suspended the program.  Type 'fg' to return\n");
 kill(0,SIGTSTP);
 if(ackkbd!= -1)
  kill(kbdpid,SIGCONT);
 if(omode) ttopnn();
#else
 ttshell(NULL);
#endif
 tickon();
 }

void mpxstart()
 {
 int fds[2];
 pipe(fds);
 mpxfd=fds[0];
 mpxsfd=fds[1];
 pipe(fds);
 acpt=MAXINT; have=0;
 if(!(kbdpid=fork()))
  {
  close(fds[1]);
  do
   {
   unsigned char c;
   int sta;
   pack.who=0;
   sta=jread(fileno(termin),&c,1);
   if(sta==0) pack.ch=MAXINT;
   else pack.ch=c;
   pack.size=0;
   jwrite(mpxsfd,&pack,sizeof(struct packet)-1024);
   }
   while(jread(fds[0],&pack,1)==1);
  _exit(0);
  }
 close(fds[0]);
 ackkbd=fds[1];
 }

void mpxend()
 {
 kill(kbdpid,9);
 while(wait(NULL)<0 && errno==EINTR);
 close(ackkbd); ackkbd= -1;
 close(mpxfd);
 close(mpxsfd);
 if(have) havec=pack.ch;
 }

/* Get a pty/tty pair.  Returns open pty in 'ptyfd' and returns tty name
 * string in static buffer or NULL if couldn't get a pair.
 */

#ifdef sgi

/* Newer sgi machines can do it the __svr4__ way, but old ones can't */

extern char *_getpty();

char *getpty(ptyfd)
int *ptyfd;
 {
 return _getpty(ptyfd,O_RDWR,0600,0);
 }

#else
#ifdef __svr4__

/* Strange streams way */

extern char *ptsname();

char *getpty(ptyfd)
int *ptyfd;
 {
 int fdm;
 char *name;
 *ptyfd=fdm=open("/dev/ptmx",O_RDWR);
 grantpt(fdm);
 unlockpt(fdm);
 return ptsname(fdm);
 }

#else

/* The normal way: for each possible pty/tty pair, try to open the pty and
 * then the corresponding tty.  If both could be opened, close them both and
 * then re-open the pty.  If that succeeded, return with the opened pty and the
 * name of the tty.
 *
 * Logically you should only have to succeed in opening the pty- but the
 * permissions may be set wrong on the tty, so we have to try that too.
 * We close them both and re-open the pty because we want the forked process
 * to open the tty- that way it gets to be the controlling tty for that
 * process and the process gets to be the session leader.
 */

char *getpty(ptyfd)
int *ptyfd;
 {
 int x, fd;
 char *orgpwd=pwd();
 static char **ptys=0;
 static char *ttydir;
 static char *ptydir;
 static char ttyname[32];

 if(!ptys)
  {
  ttydir="/dev/pty/"; ptydir="/dev/ptym/";	/* HPUX systems */
  if(chpwd(ptydir) || !(ptys=rexpnd("pty*")))
  if(!ptys)
   {
   ttydir=ptydir="/dev/";			/* Everyone else */
   if(!chpwd(ptydir)) ptys=rexpnd("pty*");
   }
  }
 chpwd(orgpwd);

 if(ptys) for(fd=0;ptys[fd];++fd)
  {
  zcpy(ttyname,ptydir); zcat(ttyname,ptys[fd]);
  if((*ptyfd=open(ttyname,O_RDWR))>=0)
   {
   ptys[fd][0]='t';
   zcpy(ttyname,ttydir); zcat(ttyname,ptys[fd]);
   ptys[fd][0]='p';
   x=open(ttyname,O_RDWR);
   if(x>=0)
    {
    close(x);
    close(*ptyfd);
    zcpy(ttyname,ptydir); zcat(ttyname,ptys[fd]);
    *ptyfd=open(ttyname,O_RDWR);
    ptys[fd][0]='t';
    zcpy(ttyname,ttydir); zcat(ttyname,ptys[fd]);
    ptys[fd][0]='p';
    return ttyname;
    }
   else close(*ptyfd);
   }
  }
 return 0;
 }

#endif
#endif

int dead=0;

void death()
 {
 wait(NULL);
 dead=1;
 }

#ifndef SIGCHLD
#define SIGCHLD SIGCLD
#endif

#ifdef SA_INTERRUPT
struct sigaction inew={death,0,SA_INTERRUPT};
#else
#ifdef SV_INTERRUPT
struct sigvec inew={death,0,SV_INTERRUPT};
#endif
#endif

/* Build a new environment */

extern char **mainenv;

char **newenv(old,s)
char **old, *s;
 {
 char **new;
 int x,y,z;
 for(x=0;old[x];++x);
 new=(char **)malloc((x+2)*sizeof(char *));
 for(x=0,y=0;old[x];++x)
  {
  for(z=0;s[z]!='=';++z) if(s[z]!=old[x][z]) break;
  if(s[z]=='=')
   { if(s[z+1]) new[y++]=s; }
  else new[y++]=old[x];
  }
 if(x==y) new[y++]=s;
 new[y]=0;
 return new;
 }

MPX *mpxmk(ptyfd,cmd,args,func,object,die,dieobj)
int *ptyfd;
char *cmd;
char *args[];
void (*func)();
void *object;
void (*die)();
void *dieobj;
 {
 int fds[2];
 int comm[2];
 int pid;
 int x;
 MPX *m;
 char *name;
 if(!(name=getpty(ptyfd))) return 0;
 for(x=0;x!=NPROC;++x) 
  if(!asyncs[x].func) { m=asyncs+x; goto ok; }
 return 0;
 ok:
 ttflsh();
 ++nmpx;
 if(ackkbd== -1) mpxstart();
 m->func=func;
 m->object=object;
 m->die=die;
 m->dieobj=dieobj;
 pipe(fds);
 pipe(comm);
 m->ackfd=fds[1];
 if(!(m->kpid=fork()))
  {
  close(fds[1]);
  close(comm[0]);
  dead=0;
#ifdef SA_INTERRUPT
  sigaction(SIGCHLD,&inew,(struct sigaction *)0);
#else
#ifdef SV_INTERRUPT
  sigvec(SIGCHLD,&inew,(struct sigvec *)0);
#else
  signal(SIGCHLD,death);
#endif
#endif

  if(!(pid=fork()))
   {
   signrm();
   close(*ptyfd);
  
#ifdef TIOCNOTTY
   x=open("/dev/tty",O_RDWR);
   ioctl(x,TIOCNOTTY,0);
#endif
  
#ifdef DGUX
   setpgrp();
#else
   setpgrp(0,0);
#endif
  
   for(x=0;x!=32;++x) close(x); /* Yes, this is quite a kludge... all in the
                                   name of portability */
  
   if((x=open(name,O_RDWR))!= -1)    /* Standard input */
    {
    char **env=newenv(mainenv,"TERM=");
#ifdef __svr4__
    ioctl(x,I_PUSH,"ptem");
    ioctl(x,I_PUSH,"ldterm");
#endif
    dup(x); dup(x);		/* Standard output, standard error */
    /* (yes, stdin, stdout, and stderr must all be open for reading and
     * writing.  On some systems the shell assumes this */
  
    /* We could probably have a special TTY set-up for JOE, but for now
     * we'll just use the TTY setup for the TTY was was run on */
#ifdef TTYPOSIX
    tcsetattr(0,TCSADRAIN,&oldterm);
#else
#ifdef TTYSV
    ioctl(0,TCSETAW,&oldterm);
#else
    ioctl(0,TIOCSETN,&oarg);
    ioctl(0,TIOCSETC,&otarg);
    ioctl(0,TIOCSLTC,&oltarg);
#endif
#endif
  
    /* Execute the shell */
    execve(cmd,args,env);
    }

   _exit(0);
   }
  jwrite(comm[1],&pid,sizeof(int));

  loop:
  pack.who=m;
  pack.ch=0;
  if(dead) pack.size=0;
  else pack.size=read(*ptyfd,pack.data,1024);
  if(pack.size>0)
   {
   jwrite(mpxsfd,&pack,sizeof(struct packet)-1024+pack.size);
   jread(fds[0],&pack,1);
   goto loop;
   }
  else
   {
   pack.ch=MAXINT;
   pack.size=0;
   jwrite(mpxsfd,&pack,sizeof(struct packet)-1024);
   _exit(0);
   }
  }
 jread(comm[0],&m->pid,sizeof(int));
 close(comm[0]); close(comm[1]);
 close(fds[0]);
 return m;
 }

void mpxdied(m)
MPX *m;
 {
 if(!--nmpx) mpxend();
 while(wait(NULL)<0 && errno==EINTR);
 if(m->die) m->die(m->dieobj);
 m->func=0;
 edupd(1);
 }
