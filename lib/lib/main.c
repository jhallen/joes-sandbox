/* Test of joe's tty interface */

#include <stdio.h>
#include "scrn.h"
#include "macro.h"
#include "tty.h"
#include "termcap.h"
#include "vs.h"
#include "path.h"
#include "main.h"

extern char *joeterm;

/* Called when joe receives a signal or when input closes */

void ttsig()
 {
 _exit(0);
 }

char **mainenv;
char **mainargv;
int mainargc;
int leave;

/* Process keyboard map, termcap (required for keyboard map) and
 * environment variables (required for termcap). */

void outfunc(obj,c) { putchar(c); }

CAP *dorc()
 {
 char *run=namprt(mainargv[0]);
 char *s;
 int c;
 CAP *cap;

/* if(s=getenv("LINES")) sscanf(s,"%d",&lines);
 if(s=getenv("COLUMNS")) sscanf(s,"%d",&columns); */
 if(s=getenv("BAUD")) sscanf(s,"%u",&Baud);
 if(getenv("DOPADDING")) dopadding=1;
 if(getenv("NOXON")) noxon=1;
 if(s=getenv("JOETERM")) joeterm=s;

#ifndef __MSDOS__
 if(!(cap=getcap(NULL,9600,outfunc,NULL)))
  {
  fprintf(stderr,"Couldn't load termcap/terminfo entry\n");
  exit(1);
  }
#endif

 s=vsncpy(NULL,0,sc("."));
 s=vsncpy(sv(s),sv(run));
 s=vsncpy(sv(s),sc("rc"));
 c=procrc(cap,s);
 if(c==0) goto donerc;
 if(c==1)
  {
  char buf[8];
  fprintf(stderr,"There were errors in '%s'.  Use it anyway?",s);
  fflush(stderr);
  fgets(buf,8,stdin);
  if(buf[0]=='y' || buf[0]=='Y') goto donerc;
  }

 vsrm(s);
 s=getenv("HOME");
 if(s)
  {
  s=vsncpy(NULL,0,sz(s));
  s=vsncpy(sv(s),sc("/."));
  s=vsncpy(sv(s),sv(run));
  s=vsncpy(sv(s),sc("rc"));
  c=procrc(cap,s);
  if(c==0) goto donerc;
  if(c==1)
   {
   char buf[8];
   fprintf(stderr,"There were errors in '%s'.  Use it anyway?",s);
   fflush(stderr);
   fgets(buf,8,stdin);
   if(buf[0]=='y' || buf[0]=='Y') goto donerc;
   }
  }

 vsrm(s);
 s=vsncpy(NULL,0,sc(JOERC));
 s=vsncpy(sv(s),sv(run));
 s=vsncpy(sv(s),sc("rc"));
 c=procrc(cap,s);
 if(c==0) goto donerc;
 if(c==1)
  {
  char buf[8];
  fprintf(stderr,"There were errors in '%s'.  Use it anyway?",s);
  fflush(stderr);
  fgets(buf,8,stdin);
  if(buf[0]=='y' || buf[0]=='Y') goto donerc;
  }

 fprintf(stderr,"Couldn't open '%s'\n",s);
 exit(1);

 donerc:
 return cap;
 }
