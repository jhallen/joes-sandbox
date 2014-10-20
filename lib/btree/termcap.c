/* TERMCAP/TERMINFO database interface
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

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "config.h"
#include "blocks.h"
#include "vs.h"
#include "va.h"
#include "zstr.h"
#include "queue.h"
#include "termcap.h"

int dopadding=0;
char *joeterm=0;

#ifdef TERMINFO
extern char *tgoto();
extern char *tgetstr();
#endif

/* Default termcap entry */

char defentry[]=
"\
:co#80:li#25:am:\
:ho=\\E[H:cm=\\E[%i%d;%dH:cV=\\E[%i%dH:\
:up=\\E[A:UP=\\E[%dA:DO=\\E[%dB:nd=\\E[C:RI=\\E[%dC:LE=\\E[%dD:\
:cd=\\E[J:ce=\\E[K:cl=\\E[H\\E[J:\
:so=\\E[7m:se=\\E[m:us=\\E[4m:ue=\\E[m:\
:mb=\\E[5m:md=\\E[1m:mh=\\E[2m:me=\\E[m:\
:ku=\\E[A:kd=\\E[B:kl=\\E[D:kr=\\E[C:\
:al=\\E[L:AL=\\E[%dL:dl=\\E[M:DL=\\E[%dM:\
:ic=\\E[@:IC=\\E[%d@:dc=\\E[P:DC=\\E[%dP:\
";

/* Return true if termcap line matches name */

static int match(s,name)
char *s, *name;
{
if(s[0]==0 || s[0]=='#') return 0;
do
 {
 int x;
 for(x=0;s[x]==name[x] && name[x] && s[x];++x);
 if(name[x]==0 && (s[x]==':' || s[x]=='|')) return 1;
 while(s[x]!=':' && s[x]!='|' && s[x]) ++x;
 s+=x+1;
 }
 while(s[-1]=='|');
return 0;
}

/* Find termcap entry in a file */

static char *lfind(s,pos,fd,name)
char *s, *name;
FILE *fd;
int pos;
{
int c,x;
if(!s) s=vsmk(1024);
loop:
while(c=getc(fd), c==' ' || c=='\t' || c=='#')
 do c=getc(fd); while(!(c== -1 || c=='\n'));
if(c== -1) return s=vstrunc(s,pos);
ungetc(c,fd);
s=vstrunc(s,x=pos);
while(1)
 {
 c=getc(fd);
 if(c== -1 || c=='\n')
  if(x!=pos && s[x-1]=='\\')
   {
   --x;
   if(!match(s+pos,name)) goto loop;
   else break;
   }
  else
   if(!match(s+pos,name)) goto loop;
   else return vstrunc(s,x);
 else if(c=='\r') ;
 else s=vsset(s,x,c), ++x;
 }
while(c=getc(fd), c!= -1)
 if(c=='\n')
  if(s[x-1]=='\\') --x;
  else break;
 else if(c=='\r') ;
 else s=vsset(s,x,c), ++x;
s=vstrunc(s,x);
return s;
}

/* Lookup termcap entry in index */

static long findidx(file,name)
FILE *file;
char *name;
{
char buf[80];
long addr=0;
while(fgets(buf,80,file))
 {
 int x=0, flg=0, c, y, z;
 do
  {
  for(y=x;buf[y] && buf[y]!=' ' && buf[y]!='\n';++y);
  c=buf[y]; buf[y]=0;
  if(c=='\n' || !c)
   {
   z=0; sscanf(buf+x,"%x",&z);
   addr+=z;
   }
  else if(!zcmp(buf+x,name)) flg=1;
  x=y+1;
  }
  while(c && c!='\n');
 if(flg) return addr;
 }
return 0;
}

/* Load termcap entry */

CAP *getcap(name,baud,out,outptr)
char *name;
unsigned baud;
void (*out)();
void *outptr;
{
CAP *cap;
FILE *f, *f1;
long idx;
int x,y,c,z,ti;
char *tp, *pp, *qq, *namebuf, **npbuf, *idxname;
int sortsiz;

if(!name && !(name=joeterm) && !(name=getenv("TERM"))) return 0;
cap=(CAP *)malloc(sizeof(CAP));
cap->tbuf=vsmk(4096);
cap->abuf=0;
cap->sort=0;

#ifdef TERMINFO
cap->abuf=(char *)malloc(4096);
cap->abufp=cap->abuf;
if(tgetent(cap->tbuf,name)==1)
 return setcap(cap,baud,out,outptr);
else
 {
 free(cap->abuf);
 cap->abuf=0;
 }
#endif

name=vsncpy(NULL,0,sz(name));
cap->sort=(struct sortentry *)malloc(sizeof(struct sortentry)*(sortsiz=64));
cap->sortlen=0;

tp=getenv("TERMCAP");

if(tp && tp[0]=='/') namebuf=vsncpy(NULL,0,sz(tp));
else
 {
 if(tp) cap->tbuf=vsncpy(sv(cap->tbuf),sz(tp));
 if((tp=getenv("TERMPATH"))) namebuf=vsncpy(NULL,0,sz(tp));
 else
  {
  if((tp=getenv("HOME")))
   namebuf=vsncpy(NULL,0,sz(tp)),
   namebuf=vsadd(namebuf,'/');
  else
   namebuf=0;
  namebuf=vsncpy(sv(namebuf),sc(".termcap "));
  namebuf=vsncpy(sv(namebuf),sc(JOERC));
  namebuf=vsncpy(sv(namebuf),sc("termcap /etc/termcap"));
  }
 }

npbuf=vawords(NULL,sv(namebuf),sc("\t :"));
vsrm(namebuf);

y=0; ti=0;

if(match(cap->tbuf,name)) goto checktc;

cap->tbuf=vstrunc(cap->tbuf,0);

nextfile:
if(!npbuf[y])
 {
/*
 varm(npbuf);
 vsrm(name);
 vsrm(cap->tbuf);
 free(cap->sort);
 free(cap);
 return 0;
*/
 fprintf(stderr,"Couldn't load termcap entry.  Using ansi default\n");
 ti=0;
 cap->tbuf=vsncpy(cap->tbuf,0,sc(defentry));
 goto checktc;
 }
idx=0;
idxname=vsncpy(NULL,0,sz(npbuf[y]));
idxname=vsncpy(idxname,sLEN(idxname),sc(".idx"));
f1=fopen(npbuf[y],"r");
++y;
if(!f1) goto nextfile;
f=fopen(idxname,"r");
if(f)
 {
 struct stat buf, buf1;
 fstat(fileno(f),&buf);
 fstat(fileno(f1),&buf1);
 if(buf.st_mtime>buf1.st_mtime) idx=findidx(f,name);
 else fprintf(stderr,"%s is out of date\n",idxname);
 fclose(f);
 }
vsrm(idxname);
fseek(f1,idx,0);
cap->tbuf=lfind(cap->tbuf,ti,f1,name);
fclose(f1);
if(sLEN(cap->tbuf)==ti) goto nextfile;

checktc:
x=sLEN(cap->tbuf);
do
 {
 cap->tbuf[x]=0;
 while(x && cap->tbuf[--x]!=':');
 }
 while(x && (!cap->tbuf[x+1] || cap->tbuf[x+1]==':'));

if(cap->tbuf[x+1]=='t' && cap->tbuf[x+2]=='c' && cap->tbuf[x+3]=='=')
 {
 name=vsncpy(NULL,0,sz(cap->tbuf+x+4));
 cap->tbuf[x]=0;
 cap->tbuf[x+1]=0;
 ti=x+1;
 sLen(cap->tbuf)=x+1;
 if(y) --y;
 goto nextfile;
 }

doline:
pp=cap->tbuf+ti;

/* Process line at pp */

loop:
while(*pp && *pp!=':') ++pp;
if(*pp)
 {
 int q;
 *pp++=0;
 loop1:
 if(pp[0]==' ' || pp[0]=='\t') goto loop;
 for(q=0;pp[q] && pp[q]!='#' && pp[q]!='=' && pp[q]!='@' && pp[q]!=':';++q);
 qq=pp;
 c=pp[q]; pp[q]=0;
 if(c) pp+=q+1;
 else pp+=q;

 x=0; y=cap->sortlen; z= -1;
 if(!y) { z=0; goto in; }
 while(z!=(x+y)/2)
  {
  z=(x+y)/2;
  switch(zcmp(qq,cap->sort[z].name))
   {
  case  1: x=z; break;
  case -1: y=z; break;
  case  0:
   if(c=='@')
    mfwrd(cap->sort+z,cap->sort+z+1,
          (cap->sortlen---(z+1))*sizeof(struct sortentry));
   else
    if(c && c!=':') cap->sort[z].value=qq+q+1;
    else cap->sort[z].value=0;
   if(c==':') goto loop1;
   else goto loop;
   }
  }
 in:
 if(cap->sortlen==sortsiz)
  cap->sort=(struct sortentry *)realloc(cap->sort,
                                        (sortsiz+=32)*sizeof(struct sortentry));
 mbkwd(cap->sort+y+1,cap->sort+y,
       (cap->sortlen++-y)*sizeof(struct sortentry));
 cap->sort[y].name=qq;
 if(c && c!=':') cap->sort[y].value=qq+q+1;
 else cap->sort[y].value=0;
 if(c==':') goto loop1;
 else goto loop;
 }

if(ti)
 {
 for(--ti;ti;--ti) if(!cap->tbuf[ti-1]) break;
 goto doline;
 }

varm(npbuf);
vsrm(name);

cap->pad=jgetstr(cap,"pc");
if(dopadding) cap->dopadding=1;
else cap->dopadding=0;
return setcap(cap,baud,out,outptr);
}

struct sortentry *findcap(cap,name)
CAP *cap;
char *name;
{
int x,y,z;
x=0; y=cap->sortlen; z= -1;
while(z!=(x+y)/2)
 {
 z=(x+y)/2;
 switch(zcmp(name,cap->sort[z].name))
  {
 case  1: x=z; break;
 case -1: y=z; break;
 case  0: return cap->sort+z;
  }
 }
return 0;
}

CAP *setcap(cap,baud,out,outptr)
CAP *cap;
unsigned baud;
void (*out)();
void *outptr;
{
cap->baud=baud;
cap->div=100000/baud;
cap->out=out;
cap->outptr=outptr;
return cap;
}

int getflag(cap,name)
CAP *cap;
char *name;
{
#ifdef TERMINFO
if(cap->abuf) return tgetflag(name);
#endif
return findcap(cap,name)!=0;
}

char *jgetstr(cap,name)
CAP *cap;
char *name;
{
struct sortentry *s;
#ifdef TERMINFO
if(cap->abuf) return tgetstr(name,&cap->abufp);
#endif
s=findcap(cap,name);
if(s) return s->value;
else return 0;
}

int getnum(cap,name)
CAP *cap;
char *name;
{
struct sortentry *s;
#ifdef TERMINFO
if(cap->abuf) return tgetnum(name);
#endif
s=findcap(cap,name);
if(s && s->value) return atoi(s->value);
return -1;
}

void rmcap(cap)
CAP *cap;
{
vsrm(cap->tbuf);
if(cap->abuf) free(cap->abuf);
if(cap->sort) free(cap->sort);
free(cap);
}

static char escape(s)
char **s;
{
char c= *(*s)++;
if(c=='^' && **s)
 if(**s!='?') return 037&*(*s)++;
 else return (*s)++, 127;
else if(c=='\\' && **s)
 switch(c= *((*s)++))
  {
 case '0': case '1': case '2': case'3': case '4': case '5': case '6': case '7':
           c-='0';
           if(**s>='0' && **s<='7') c=(c<<3)+*((*s)++)-'0';
           if(**s>='0' && **s<='7') c=(c<<3)+*((*s)++)-'0';
           return c;
 case 'e':
 case 'E': return 27;
 case 'n':
 case 'l': return 10;
 case 'r': return 13;
 case 't': return 9;
 case 'b': return 8;
 case 'f': return 12;
 case 's': return 32;
 default: return c;
  }
else return c;
}

static CAP *outcap;
static int outout(c)
 {
 outcap->out(outcap->outptr,c);
 }

void texec(cap,s,l,a0,a1,a2,a3)
CAP *cap;
char *s;
int l,a0,a1,a2,a3;
{
int c, tenth=0, x;
int args[4];
int vars[128];
int *a=args;

/* Do nothing if there is no string */
if(!s) return;

#ifdef TERMINFO
if(cap->abuf)
 {
 char *a;
 outcap=cap;
 a=tgoto(s,a1,a0);
 tputs(a,l,outout);
 return;
 }
#endif

/* Copy args into array (yuk) */
args[0]=a0; args[1]=a1; args[2]=a2; args[3]=a3;

/* Get tenths of MS of padding needed */
while(*s>='0' && *s<='9') tenth=tenth*10+*s++-'0';
tenth*=10;
if(*s=='.') ++s, tenth+= *s++-'0';

/* Check if we have to multiply by number of lines */
if(*s=='*') ++s, tenth*=l;

/* Output string */
while(c= *s++)
 if(c=='%' && *s)
  switch(x=a[0], c= escape(&s))
   {
  case 'C': if(x>=96) cap->out(cap->outptr,x/96), x%=96;
  case '+': if(*s) x+= escape(&s);
  case '.': cap->out(cap->outptr,x); ++a; break;
  case 'd': if(x<10) goto one;
  case '2': if(x<100) goto two;
  case '3': c='0'; while(x>=100) ++c, x-=100; cap->out(cap->outptr,c);
       two: c='0'; while(x>=10) ++c, x-=10; cap->out(cap->outptr,c);
       one: cap->out(cap->outptr,'0'+x); ++a; break;
  case 'r': a[0]=a[1]; a[1]=x; break;
  case 'i': ++a[0]; ++a[1]; break;
  case 'n': a[0]^=0140; a[1]^=0140; break;
  case 'm': a[0]^=0177; a[1]^=0177; break;
  case 'f': ++a; break;
  case 'b': --a; break;
  case 'a': x=s[2];
            if(s[1]=='p') x=a[x-0100];
            switch(*s)
             {
             case '+': a[0]+=x; break;
             case '-': a[0]-=x; break;
             case '*': a[0]*=x; break;
             case '/': a[0]/=x; break;
             case '%': a[0]%=x; break;
             case 'l': a[0]=vars[x]; break;
             case 's': vars[x]=a[0]; break;
             default:  a[0]=x;
             }
            s+=3;
            break;
  case 'D': a[0]=a[0]-2*(a[0]&15); break;
  case 'B': a[0]=16*(a[0]/10)+a[0]%10; break;
  case '>': if(a[0]>escape(&s)) a[0]+=escape(&s); else escape(&s);
   default: cap->out(cap->outptr,'%'); cap->out(cap->outptr,c);
   }
 else --s, cap->out(cap->outptr,escape(&s));

/* Output padding characters */
if(cap->dopadding)
 if(cap->pad)
  while(tenth>=cap->div)
   for(s=cap->pad;*s;++s) cap->out(cap->outptr,*s), tenth-=cap->div;
 else
  while(tenth>=cap->div) cap->out(cap->outptr,0), tenth-=cap->div;
}

static int total;

static void cst()
{
++total;
}

int tcost(cap,s,l,a0,a1,a2,a3)
CAP *cap;
char *s;
int l,a0,a1,a2,a3;
{
void (*out)()=cap->out;
if(!s) return 10000;
total=0;
cap->out=cst;
texec(cap,s,l,a0,a1,a2,a3);
cap->out=out;
return total;
}

static char *ssp;
static void cpl(ptr,c)
char *ptr;
char c;
{
ssp=vsadd(ssp,c);
}

char *tcompile(cap,s,a0,a1,a2,a3)
CAP *cap;
char *s;
int a0,a1,a2,a3;
{
void (*out)()=cap->out;
int div=cap->div;
if(!s) return 0;
cap->out=cpl; cap->div=10000;
ssp=vsmk(10);
texec(cap,s,0,a0,a1,a2,a3);
cap->out=out; cap->div=div;
return ssp;
}

/* Old termcap compatibility (not to be used when TERMINFO is set) */
#ifdef junk
short ospeed;		/* Output speed */
char PC, *UP, *BC;		/* Unused */
static CAP *latest;	/* CAP entry to use */

static void stupid(ptr,c)
void (*ptr)();
char c;
{
ptr(c);
}

int tgetent(buf,name)
char *buf, *name;
{
latest=getcap(name,9600,stupid,NULL);
if(latest) return 1;
else return -1;
}

int tgetflag(name)
char *name;
{
return getflag(latest,name);
}

int tgetnum(name)
char *name;
{
return getnum(latest,name);
}

char *tgetstr(name)
char *name;
{
return jgetstr(latest,name);
}

static int latestx, latesty;

char *tgoto(str,x,y)
char *str;
int x,y;
{
latestx=x; latesty=y;
return str;
}

void tputs(str,l,out)
char *str;
int l;
void (*out)();
{
latest->outptr=(void *)out;
if(latest->baud!=ospeed) latest->baud=ospeed, latest->div=100000/ospeed;
texec(latest,str,l,latesty,latestx);
}
#endif
