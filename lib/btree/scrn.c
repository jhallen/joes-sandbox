/* Device independant TTY interface for JOE
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
#include <signal.h>
#include "blocks.h"
#include "vs.h"
#include "termcap.h"
#include "tty.h"
#include "zstr.h"
#include "scrn.h"

int skiptop=0;
int lines=0;
int columns=0;

extern int mid;

/* How to display characters */

unsigned xlata[256]=
 {
 UNDERLINE, UNDERLINE, UNDERLINE, UNDERLINE,
 UNDERLINE, UNDERLINE, UNDERLINE, UNDERLINE,
 UNDERLINE, UNDERLINE, UNDERLINE, UNDERLINE,
 UNDERLINE, UNDERLINE, UNDERLINE, UNDERLINE,
 UNDERLINE, UNDERLINE, UNDERLINE, UNDERLINE,
 UNDERLINE, UNDERLINE, UNDERLINE, UNDERLINE,
 UNDERLINE, UNDERLINE, UNDERLINE, UNDERLINE,
 UNDERLINE, UNDERLINE, UNDERLINE, UNDERLINE,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 UNDERLINE,

 INVERSE+UNDERLINE, INVERSE+UNDERLINE, INVERSE+UNDERLINE, INVERSE+UNDERLINE, 
 INVERSE+UNDERLINE, INVERSE+UNDERLINE, INVERSE+UNDERLINE, INVERSE+UNDERLINE, 
 INVERSE+UNDERLINE, INVERSE+UNDERLINE, INVERSE+UNDERLINE, INVERSE+UNDERLINE, 
 INVERSE+UNDERLINE, INVERSE+UNDERLINE, INVERSE+UNDERLINE, INVERSE+UNDERLINE, 
 INVERSE+UNDERLINE, INVERSE+UNDERLINE, INVERSE+UNDERLINE, INVERSE+UNDERLINE, 
 INVERSE+UNDERLINE, INVERSE+UNDERLINE, INVERSE+UNDERLINE, INVERSE+UNDERLINE, 
 INVERSE+UNDERLINE, INVERSE+UNDERLINE, INVERSE+UNDERLINE, INVERSE+UNDERLINE, 
 INVERSE+UNDERLINE, INVERSE+UNDERLINE, INVERSE+UNDERLINE, INVERSE+UNDERLINE, 

 INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, 
 INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, 
 INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, 
 INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, 
 INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, 
 INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, 
 INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, 
 INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, 
 INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, 
 INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, 
 INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, 
 INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE, INVERSE,
 
 INVERSE+UNDERLINE, 
 };

unsigned char xlatc[256]=
 {
 64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
 80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,
 32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
 48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
 64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
 80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,
 96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,
 112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,63,
 64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
 80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,
 32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
 48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
 64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
 80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,
 96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,
 112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,63
 }; 

/* Set attributes */

int attr(t,c)
SCRN *t;
int c;
{
int e;
c&=~255;
e=(t->attrib&~c);
if(e) /* If any attribute go off, switch them all off: fixes bug on PCs */
 {
 if(t->me) texec(t->cap,t->me,1);
 else
  {
  if(t->ue) texec(t->cap,t->ue,1);
  if(t->se) texec(t->cap,t->se,1);
  }
 t->attrib=0;
 }
e=(c&~t->attrib);
if(e&INVERSE)
 if(t->mr) texec(t->cap,t->mr,1);
 else if(t->so) texec(t->cap,t->so,1);
if(e&UNDERLINE)
 if(t->us) texec(t->cap,t->us,1);
if(e&BLINK)
 if(t->mb) texec(t->cap,t->mb,1);
if(e&BOLD)
 if(t->md) texec(t->cap,t->md,1);
if(e&DIM)
 if(t->mh) texec(t->cap,t->mh,1);
t->attrib=c;
return 0;
}

/* Set scrolling region */

void setregn(t,top,bot)
SCRN *t;
int top,bot;
{
if(!t->cs)
 {
 t->top=top;
 t->bot=bot;
 return;
 }
if(t->top!=top || t->bot!=bot)
 {
 t->top=top;
 t->bot=bot;
 texec(t->cap,t->cs,1,top,bot-1);
 t->x= -1; t->y= -1;
 }
}

/* Enter insert mode */

void setins(t,x)
SCRN *t;
{
if(t->ins!=1 && t->im)
 {
 t->ins=1;
 texec(t->cap,t->im,1,x);
 }
}

/* Exit insert mode */

int clrins(t)
SCRN *t;
{
if(t->ins!=0)
 {
 texec(t->cap,t->ei,1);
 t->ins=0;
 }
return 0;
}

/* Erase from given screen coordinate to end of line */

int eraeol(t,x,y)
SCRN *t;
{
int *s, *ss;
int w=t->co-x-1;			/* Don't worry about last column */
if(w<=0) return 0;
s=t->scrn+y*t->co+x;
ss=s+w;
do if(*--ss!=' ') { ++ss; break; } while(ss!=s);
if((ss-s>3 || s[w]!=' ') && t->ce)
 {
 cpos(t,x,y);
 attr(t,0);
 texec(t->cap,t->ce,1);
 msetI(s,' ',w);
 }
else if(s!=ss)
 {
 if(t->ins) clrins(t);
 if(t->x!=x || t->y!=y) cpos(t,x,y);
 if(t->attrib) attr(t,0);
 while(s!=ss)
  {
  *s=' '; ttputc(' '); ++t->x; ++s;
  }
 }
return 0;
}

/* As above but useable in insert mode */
/* The cursor position must already be correct */

void outatri(t,x,y,c)
SCRN *t;
{
unsigned char ch;
if(c== -1) c=' ';
ch=c; c-=ch;
if(c!=t->attrib) attr(t,c);
if(t->haz && ch=='~') ch='\\';
ttputc(ch);
++t->x;
}

void out(t,c)
char *t;
char c;
 {
 ttputc(c);
 }

SCRN *nopen(cap)
CAP *cap;
{
SCRN *t=(SCRN *)malloc(sizeof(SCRN));
int x,y;
ttopen();

t->cap=cap;
setcap(cap,baud,out,NULL);

t->li=getnum(t->cap,"li"); if(t->li<1) t->li=24;
t->co=getnum(t->cap,"co"); if(t->co<2) t->co=80;
x=y=0;
ttgtsz(&x,&y);
if(x>7 && y>3) t->li=y, t->co=x;

t->haz=getflag(t->cap,"hz");
t->os=getflag(t->cap,"os");
t->eo=getflag(t->cap,"eo");
if(getflag(t->cap,"hc")) t->os=1;
if(t->os || getflag(t->cap,"ul")) t->ul=1;
else t->ul=0;

t->xn=getflag(t->cap,"xn");
t->am=getflag(t->cap,"am");

t->ti=jgetstr(t->cap,"ti");
t->cl=jgetstr(t->cap,"cl");
t->cd=jgetstr(t->cap,"cd");

t->te=jgetstr(t->cap,"te");

t->mb=0; t->md=0; t->mh=0; t->mr=0; t->avattr=0;
if(!(t->me=jgetstr(t->cap,"me"))) goto oops;
if((t->mb=jgetstr(t->cap,"mb"))) t->avattr|=BLINK;
if((t->md=jgetstr(t->cap,"md"))) t->avattr|=BOLD;
if((t->mh=jgetstr(t->cap,"mh"))) t->avattr|=DIM;
if((t->mr=jgetstr(t->cap,"mr"))) t->avattr|=INVERSE;
oops:

t->so=0; t->se=0;
if(getnum(t->cap,"sg")<=0 && !t->mr && jgetstr(t->cap,"se"))
 {
 if(t->so=jgetstr(t->cap,"so")) t->avattr|=INVERSE;
 t->se=jgetstr(t->cap,"se");
 }
if(getflag(t->cap,"xs") || getflag(t->cap,"xt")) t->so=0;

t->us=0; t->ue=0;
if(getnum(t->cap,"ug")<=0 && jgetstr(t->cap,"ue"))
 {
 if(t->us=jgetstr(t->cap,"us")) t->avattr|=UNDERLINE;
 t->ue=jgetstr(t->cap,"ue");
 }

if(!(t->uc=jgetstr(t->cap,"uc"))) if(t->ul) t->uc="_";
if(t->uc) t->avattr|=UNDERLINE;

t->ms=getflag(t->cap,"ms");

t->da=getflag(t->cap,"da");
t->db=getflag(t->cap,"db");
t->cs=jgetstr(t->cap,"cs");
t->rr=getflag(t->cap,"rr");
t->sf=jgetstr(t->cap,"sf");
t->sr=jgetstr(t->cap,"sr");
t->SF=jgetstr(t->cap,"SF");
t->SR=jgetstr(t->cap,"SR");
t->al=jgetstr(t->cap,"al");
t->dl=jgetstr(t->cap,"dl");
t->AL=jgetstr(t->cap,"AL");
t->DL=jgetstr(t->cap,"DL");
if(!getflag(t->cap,"ns") && !t->sf) t->sf="\12";

if(!getflag(t->cap,"in") && baud<38400 )
 {
 t->dc=jgetstr(t->cap,"dc");
 t->DC=jgetstr(t->cap,"DC");
 t->dm=jgetstr(t->cap,"dm");
 t->ed=jgetstr(t->cap,"ed");

 t->im=jgetstr(t->cap,"im");
 t->ei=jgetstr(t->cap,"ei");
 t->ic=jgetstr(t->cap,"ic");
 t->IC=jgetstr(t->cap,"IC");
 t->ip=jgetstr(t->cap,"ip");
 t->mi=getflag(t->cap,"mi");
 }
else
 {
 t->dm=0; t->dc=0; t->DC=0; t->ed=0;
 t->im=0; t->ic=0; t->IC=0; t->ip=0; t->ei=0;
 t->mi=1;
 }

t->bs=0;
if(jgetstr(t->cap,"bc")) t->bs=jgetstr(t->cap,"bc");
else if(jgetstr(t->cap,"le")) t->bs=jgetstr(t->cap,"le");
if(getflag(t->cap,"bs")) t->bs="\10";

t->cbs=tcost(t->cap,t->bs,1,2,2);

t->lf="\12";
if(jgetstr(t->cap,"do")) t->lf=jgetstr(t->cap,"do");
t->clf=tcost(t->cap,t->lf,1,2,2);

t->up=jgetstr(t->cap,"up");
t->cup=tcost(t->cap,t->up,1,2,2);

t->nd=jgetstr(t->cap,"nd");

t->tw=8;
if(getnum(t->cap,"it")>0) t->tw=getnum(t->cap,"it");
else if(getnum(t->cap,"tw")>0) t->tw=getnum(t->cap,"tw");

if(!(t->ta=jgetstr(t->cap,"ta"))) if(getflag(t->cap,"pt")) t->ta="\11";
t->bt=jgetstr(t->cap,"bt");
if(getflag(t->cap,"xt")) t->ta=0, t->bt=0;

t->cta=tcost(t->cap,t->ta,1,2,2);
t->cbt=tcost(t->cap,t->bt,1,2,2);

t->ho=jgetstr(t->cap,"ho");
t->cho=tcost(t->cap,t->ho,1,2,2);
t->ll=jgetstr(t->cap,"ll");
t->cll=tcost(t->cap,t->ll,1,2,2);

t->cr="\15";
if(jgetstr(t->cap,"cr")) t->cr=jgetstr(t->cap,"cr");
if(getflag(t->cap,"nc") || getflag(t->cap,"xr")) t->cr=0;
t->ccr=tcost(t->cap,t->cr,1,2,2);

t->cRI=tcost(t->cap,t->RI=jgetstr(t->cap,"RI"),1,2,2);
t->cLE=tcost(t->cap,t->LE=jgetstr(t->cap,"LE"),1,2,2);
t->cUP=tcost(t->cap,t->UP=jgetstr(t->cap,"UP"),1,2,2);
t->cDO=tcost(t->cap,t->DO=jgetstr(t->cap,"DO"),1,2,2);
t->cch=tcost(t->cap,t->ch=jgetstr(t->cap,"ch"),1,2,2);
t->ccv=tcost(t->cap,t->cv=jgetstr(t->cap,"cv"),1,2,2);
t->ccV=tcost(t->cap,t->cV=jgetstr(t->cap,"cV"),1,2,2);
t->ccm=tcost(t->cap,t->cm=jgetstr(t->cap,"cm"),1,2,2);

t->cce=tcost(t->cap,t->ce=jgetstr(t->cap,"ce"),1,2,2);

/* Make sure terminal can do absolute positioning */
if(t->cm) goto ok;
if(t->ch && t->cv) goto ok;
if(t->ho && (t->lf || t->DO || t->cv)) goto ok;
if(t->ll && (t->up || t->UP || t->cv)) goto ok;
if(t->cr && t->cv) goto ok;
leave=1;
ttclose();
signrm();
fprintf(stderr,"Sorry, your terminal can't do absolute cursor positioning.\nIt's broken\n");
return 0;
ok:

/* Determine if we can scroll */
if((t->sr || t->SR) && (t->sf || t->SF) && t->cs ||
   (t->al || t->AL) && (t->dl || t->DL)) t->scroll=1;
else
 {
 t->scroll=0;
 if(baud<38400) mid=1;
 }

/* Determine if we can ins/del within lines */
if((t->im || t->ic || t->IC) && (t->dc || t->DC)) t->insdel=1;
else t->insdel=0;

/* Adjust for high baud rates */
if(baud>=38400) t->scroll=0, t->insdel=0;

/* Initialize variable screen size dependant vars */
t->scrn=0; t->sary=0; t->updtab=0; t->compose=0;
t->ofst=0; t->ary=0;
t->htab=(struct hentry *)malloc(256*sizeof(struct hentry));
nresize(t,t->co,t->li);

/* Send out terminal initialization string */
if(t->ti) texec(t->cap,t->ti,1);

return t;
} 

/* Change size of screen */

void nresize(t,w,h)
SCRN *t;
{
if(h<4) h=4;
if(w<8) w=8;
t->li=h;
t->co=w;
if(t->sary) free(t->sary);
if(t->updtab) free(t->updtab);
if(t->scrn) free(t->scrn);
if(t->compose) free(t->compose);
if(t->ofst) free(t->ofst);
if(t->ary) free(t->ary);
t->scrn=(int *)malloc(t->li*t->co*sizeof(int));
t->sary=(int *)calloc(t->li,sizeof(int));
t->updtab=(int *)malloc(t->li*sizeof(int));
t->compose=(int *)malloc(t->co*sizeof(int));
t->ofst=(int *)malloc(t->co*sizeof(int));
t->ary=(struct hentry *)malloc(t->co*sizeof(struct hentry));
nredraw(t);
}

/* Calculate cost of positioning the cursor using only relative cursor
 * positioning functions: t->(lf, DO, up, UP, bs, LE, RI, ta, bt) and rewriting
 * characters (to move right)
 *
 * This doesn't use the am and bw capabilities although it probably could.
 */

static int relcost(t,x,y,ox,oy)
register SCRN *t;
register int x,y,ox,oy;
{
int cost=0;

/* If we don't know the cursor position, force use of absolute positioning */
if(oy== -1 || ox== -1) return 10000;

/* First adjust row */
if(y>oy)
 {
 int dist=y-oy;
 /* Have to go down */
 if(t->lf)
  {
  int mult=dist*t->clf;
  if(dist<10 && t->cDO<mult) cost+=t->cDO;
  else if(dist>=10 && t->cDO+1<mult) cost+=t->cDO+1;
  else cost+=mult;
  }
 else if(t->DO)
  if(dist<10) cost+=t->cDO;
  else cost+=t->cDO+1;
 else return 10000;
 }
else if(y<oy)
 {
 int dist=oy-y;
 /* Have to go up */
 if(t->up)
  {
  int mult=dist*t->cup;
  if(dist<10 && t->cUP<mult) cost+=t->cUP;
  else if(dist>=10 && t->cUP<mult) cost+=t->cUP+1;
  else cost+=mult;
  }
 else if(t->UP)
  if(dist<10) cost+=t->cUP;
  else cost+=t->cUP+1;
 else return 10000;
 }

/* Now adjust column */

/* Use tabs */
if(x>ox && t->ta)
 {
 int dist=x-ox;
 int ntabs=(dist+ox%t->tw)/t->tw;
 int cstunder=x%t->tw+t->cta*ntabs, cstover;
 if(x+t->tw<t->co && t->bs) cstover=t->cbs*(t->tw-x%t->tw)+t->cta*(ntabs+1);
 else cstover=10000;
 if(dist<10 && cstunder<t->cRI && cstunder<x-ox && cstover>cstunder)
  return cost+cstunder;
 else if(cstunder<t->cRI+1 && cstunder<x-ox && cstover>cstunder)
  return cost+cstunder;
 else if(dist<10 && cstover<t->cRI && cstover<x-ox) return cost+cstover;
 else if(cstover<t->cRI+1 && cstover<x-ox) return cost+cstover;
 }
else if(x<ox && t->bt)
 {
 int dist=ox-x; 
 int ntabs=(dist+t->tw-ox%t->tw)/t->tw;
 int cstunder,cstover;
 if(t->bs) cstunder=t->cbt*ntabs+t->cbs*(t->tw-x%t->tw); else cstunder=10000;
 if(x-t->tw>=0) cstover=t->cbt*(ntabs+1)+x%t->tw; else cstover=10000;
 if(dist<10 && cstunder<t->cLE && (t->bs?cstunder<(ox-x)*t->cbs:1) && cstover>cstunder)
  return cost+cstunder;
 if(cstunder<t->cLE+1 && (t->bs?cstunder<(ox-x)*t->cbs:1) && cstover>cstunder)
  return cost+cstunder;
 else if(dist<10 && cstover<t->cRI && (t->bs?cstover<(ox-x)*t->cbs:1)) return cost+cstover;
 else if(cstover<t->cRI+1 && (t->bs?cstover<(ox-x)*t->cbs:1)) return cost+cstover;
 }

/* Use simple motions */
if(x<ox)
 {
 int dist=ox-x;
 /* Have to go left */
 if(t->bs)
  {
  int mult=dist*t->cbs;
  if(t->cLE<mult && dist<10) cost+=t->cLE;
  else if(t->cLE+1<mult) cost+=t->cLE+1;
  else cost+=mult;
  }
 else if(t->LE) cost+=t->cLE;
 else return 10000;
 }
else if(x>ox)
 {
 int dist=x-ox;
 /* Have to go right */
 /* Hmm.. this should take into account possible attribute changes */
 if(t->cRI<dist && dist<10) cost+=t->cRI;
 else if(t->cRI+1<dist) cost+=t->cRI+1;
 else cost+=dist;
 }

return cost;
}

/* Find optimal set of cursor positioning commands to move from the current
 * cursor row and column (either or both of which might be unknown) to the
 * given new row and column and execute them.
 */

static void cposs(t,x,y)
register SCRN *t;
register int x,y;
{
register int bestcost,cost;
int bestway;
int hy;
int hl;

/* Home y position is usually 0, but it is 'top' if we have scrolling region
 * relative addressing
 */
if(t->rr) hy=t->top, hl=t->bot-1;
else hy=0, hl=t->li-1;

/* Assume best way is with only using relative cursor positioning */

bestcost=relcost(t,x,y,t->x,t->y); bestway=0;

/* Now check if combinations of absolute cursor positioning functions are
 * better (or necessary in case one or both cursor positions are unknown)
 */

if(t->ccm<bestcost)
 {
 cost=tcost(t->cap,t->cm,1,y,x);
 if(cost<bestcost) bestcost=cost, bestway=6;
 }
if(t->ccr<bestcost)
 {
 cost=relcost(t,x,y,0,t->y)+t->ccr;
 if(cost<bestcost) bestcost=cost, bestway=1;
 }
if(t->cho<bestcost)
 {
 cost=relcost(t,x,y,0,hy)+t->cho;
 if(cost<bestcost) bestcost=cost, bestway=2;
 }
if(t->cll<bestcost)
 {
 cost=relcost(t,x,y,0,hl)+t->cll;
 if(cost<bestcost) bestcost=cost, bestway=3;
 }
if(t->cch<bestcost && x!=t->x)
 {
 cost=relcost(t,x,y,x,t->y)+tcost(t->cap,t->ch,1,x);
 if(cost<bestcost) bestcost=cost, bestway=4;
 }
if(t->ccv<bestcost && y!=t->y)
 {
 cost=relcost(t,x,y,t->x,y)+tcost(t->cap,t->cv,1,y);
 if(cost<bestcost) bestcost=cost, bestway=5;
 }
if(t->ccV<bestcost)
 {
 cost=relcost(t,x,y,0,y)+tcost(t->cap,t->cV,1,y);
 if(cost<bestcost) bestcost=cost, bestway=13;
 }
if(t->cch+t->ccv<bestcost && x!=t->x && y!=t->y)
 {
 cost=tcost(t->cap,t->cv,1,y-hy)+tcost(t->cap,t->ch,1,x);
 if(cost<bestcost) bestcost=cost, bestway=7;
 }
if(t->ccv+t->ccr<bestcost && y!=t->y)
 {
 cost=tcost(t->cap,t->cv,1,y)+tcost(t->cap,t->cr,1)+
      relcost(t,x,y,0,y);
 if(cost<bestcost) bestcost=cost, bestway=8;
 }
if(t->cll+t->cch<bestcost)
 {
 cost=tcost(t->cap,t->ll,1)+tcost(t->cap,t->ch,1,x)+
      relcost(t,x,y,x,hl);
 if(cost<bestcost) bestcost=cost, bestway=9;
 }
if(t->cll+t->ccv<bestcost)
 {
 cost=tcost(t->cap,t->ll,1)+tcost(t->cap,t->cv,1,y)+
      relcost(t,x,y,0,y);
 if(cost<bestcost) bestcost=cost, bestway=10;
 }
if(t->cho+t->cch<bestcost)
 {
 cost=tcost(t->cap,t->ho,1)+tcost(t->cap,t->ch,1,x)+
      relcost(t,x,y,x,hy);
 if(cost<bestcost) bestcost=cost, bestway=11;
 }
if(t->cho+t->ccv<bestcost)
 {
 cost=tcost(t->cap,t->ho,1)+tcost(t->cap,t->cv,1,y)+
      relcost(t,x,y,0,y);
 if(cost<bestcost) bestcost=cost, bestway=12;
 }

/* Do absolute cursor positioning if we don't know the cursor position or
 * if it is faster than doing only relative cursor positioning
 */

switch(bestway)
 {
case 1: texec(t->cap,t->cr,1); t->x=0; break;
case 2: texec(t->cap,t->ho,1); t->x=0; t->y=hy; break;
case 3: texec(t->cap,t->ll,1); t->x=0; t->y=hl; break;
case 9: texec(t->cap,t->ll,1); t->x=0; t->y=hl; goto doch;
case 11: texec(t->cap,t->ho,1); t->x=0; t->y=hy;
  doch:
case 4: texec(t->cap,t->ch,1,x); t->x=x; break;
case 10: texec(t->cap,t->ll,1); t->x=0; t->y=hl; goto docv;
case 12: texec(t->cap,t->ho,1); t->x=0; t->y=hy; goto docv;
case 8: texec(t->cap,t->cr,1); t->x=0;
  docv:
case 5: texec(t->cap,t->cv,1,y); t->y=y; break;
case 6: texec(t->cap,t->cm,1,y,x); t->y=y, t->x=x; break;
case 7: texec(t->cap,t->cv,1,y); t->y=y;
        texec(t->cap,t->ch,1,x); t->x=x;
        break;
case 13: texec(t->cap,t->cV,1,y); t->y=y; t->x=0; break;
 }

/* Use relative cursor position functions if we're not there yet */

/* First adjust row */
if(y>t->y)
 /* Have to go down */
 if(!t->lf || t->cDO<(y-t->y)*t->clf)
  texec(t->cap,t->DO,1,y-t->y), t->y=y;
 else while(y>t->y) texec(t->cap,t->lf,1), ++t->y;
else if(y<t->y)
 /* Have to go up */
 if(!t->up || t->cUP<(t->y-y)*t->cup)
  texec(t->cap,t->UP,1,t->y-y), t->y=y;
 else while(y<t->y) texec(t->cap,t->up,1), --t->y;

/* Use tabs */
if(x>t->x && t->ta)
 {
 int ntabs=(x-t->x+t->x%t->tw)/t->tw;
 int cstunder=x%t->tw+t->cta*ntabs, cstover;
 if(x+t->tw<t->co && t->bs) cstover=t->cbs*(t->tw-x%t->tw)+t->cta*(ntabs+1);
 else cstover=10000;
 if(cstunder<t->cRI && cstunder<x-t->x && cstover>cstunder)
  {
  if(ntabs)
   {
   t->x=x-x%t->tw;
   do texec(t->cap,t->ta,1); while(--ntabs);
   }
  }
 else if(cstover<t->cRI && cstover<x-t->x)
  {
  t->x=t->tw+x-x%t->tw;
  ++ntabs;
  do texec(t->cap,t->ta,1); while(--ntabs);
  }
 }
else if(x<t->x && t->bt)
 {
 int ntabs=((t->x+t->tw-1)-(t->x+t->tw-1)%t->tw-
            ((x+t->tw-1)-(x+t->tw-1)%t->tw))/t->tw;
 int cstunder,cstover;
 if(t->bs) cstunder=t->cbt*ntabs+t->cbs*(t->tw-x%t->tw); else cstunder=10000;
 if(x-t->tw>=0) cstover=t->cbt*(ntabs+1)+x%t->tw; else cstover=10000;
 if(cstunder<t->cLE && (t->bs?cstunder<(t->x-x)*t->cbs:1) && cstover>cstunder)
  {
  if(ntabs)
   {
   do texec(t->cap,t->bt,1); while(--ntabs);
   t->x=x+t->tw-x%t->tw;
   }
  }
 else if(cstover<t->cRI && (t->bs?cstover<(t->x-x)*t->cbs:1))
  {
  t->x=x-x%t->tw; ++ntabs;
  do texec(t->cap,t->bt,1); while(--ntabs);
  }
 }

/* Now adjust column */
if(x<t->x)
 /* Have to go left */
 if(!t->bs || t->cLE<(t->x-x)*t->cbs)
  texec(t->cap,t->LE,1,t->x-x), t->x=x;
 else while(x<t->x) texec(t->cap,t->bs,1), --t->x;
else if(x>t->x)
 /* Have to go right */
 /* Hmm.. this should take into account possible attribute changes */
 if(t->cRI<x-t->x) texec(t->cap,t->RI,1,x-t->x), t->x=x;
 else
  {
  int *s=t->scrn+t->x+t->y*t->co;
  if(t->ins) clrins(t);
  while(x>t->x)
   {
   int c= (0xFF&*s);
   int a= (0xFF00&*s);
   if(a!=t->attrib) attr(t,a);
   ttputc(c);
   ++s; ++t->x;
   }
  }
}

int cpos(t,x,y)
register SCRN *t;
register int x,y;
{
if(y==t->y)
 {
 if(x==t->x) return 0;
 if(x>t->x && x-t->x<4 && !t->ins)
  {
  int *cs=t->scrn+t->x+t->co*t->y;
  if(t->ins) clrins(t);
  do
   {
   int c= (0xFF& *cs);
   int a= (0xFF00& *cs);
   if(a!=t->attrib) attr(t,a);
   ttputc(c);
   ++cs; ++t->x;
   } while(x!=t->x);
  return 0;
  }
 }
if(!t->ms && t->attrib&(INVERSE|UNDERLINE))
 attr(t,t->attrib&~(INVERSE|UNDERLINE));
if(y<t->top || y>=t->bot) setregn(t,0,t->li);
cposs(t,x,y);
return 0;
}

static void doinschr(t,x,y,s,n)
SCRN *t;
int x,y,*s,n;
{
int a;
if(x<0) s-=x, x=0;
if(x>=t->co-1 || n<=0) return;
if(t->im || t->ic || t->IC)
 {
 cpos(t,x,y);
 if(n==1 && t->ic || !t->IC)
  {
  if(!t->ic) setins(t,x);
  for(a=0;a!=n;++a)
   {
   texec(t->cap,t->ic,1,x);
   outatri(t,x+a,y,s[a]);
   texec(t->cap,t->ip,1,x);
   }
  if(!t->mi) clrins(t);
  }
 else
  {
  texec(t->cap,t->IC,1,n);
  for(a=0;a!=n;++a) outatri(t,x+a,y,s[a]);
  }
 }
mmove(t->scrn+x+t->co*y+n,t->scrn+x+t->co*y,(t->co-(x+n))*sizeof(int));
mcpy(t->scrn+x+t->co*y,s,n*sizeof(int));
}

static void dodelchr(t,x,y,n)
SCRN *t;
int x,y,n;
{
int a;
if(x<0) x=0;
if(!n || x>=t->co-1) return;
if(t->dc || t->DC)
 {
 cpos(t,x,y);
 texec(t->cap,t->dm,1,x);		/* Enter delete mode */
 if(n==1 && t->dc || !t->DC)
  for(a=n;a;--a) texec(t->cap,t->dc,1,x);
 else texec(t->cap,t->DC,1,n);
 texec(t->cap,t->ed,1,x);		/* Exit delete mode */
 }
mmove(t->scrn+t->co*y+x,t->scrn+t->co*y+x+n,(t->co-(x+n))*sizeof(int));
msetI(t->scrn+t->co*y+t->co-n,' ',n);
}

/* Insert/Delete within line */

void magic(t,y,cs,s,placex)
SCRN *t;
int y,*cs,*s;
{
struct hentry *htab=t->htab;
int *ofst=t->ofst;
int aryx=1;
int x;
if(!(t->im || t->ic || t->IC) ||
   !(t->dc || t->DC)) return;
mset(htab,0,256*sizeof(struct hentry));
msetI(ofst,0,t->co);

/* Build hash table */
for(x=0;x!=t->co-1;++x)
 t->ary[aryx].next=htab[cs[x]&255].next,
 t->ary[aryx].loc=x,
 ++htab[cs[x]&255].loc,
 htab[cs[x]&255].next=aryx++;

/* Build offset table */
for(x=0;x<t->co-1;)
 if(htab[s[x]&255].loc>=15) ofst[x++]= t->co-1;
 else
  {
  int aryy;
  int maxaryy;
  int maxlen=0;
  int best=0;
  int bestback=0;
  int z;
  for(aryy=htab[s[x]&255].next;aryy;aryy=t->ary[aryy].next)
   {
   int amnt,back;
   int tsfo=t->ary[aryy].loc-x;
   int cst= -Iabs(tsfo);
   int pre=32;
   for(amnt=0;x+amnt<t->co-1 && x+tsfo+amnt<t->co-1;++amnt)
    {
    if(cs[x+tsfo+amnt]!=s[x+amnt]) break;
    else if(s[x+amnt]&255!=32 || pre!=32) ++cst;
    pre=s[x+amnt]&255;
    }
   pre=32;
   for(back=0;back+x>0 && back+tsfo+x>0;--back)
    {
    if(cs[x+tsfo+back-1]!=s[x+back-1]) break;
    else if(s[x+back-1]&255!=32 || pre!=32) ++cst;
    pre=s[x+back-1]&255;
    }
   if(cst>best) maxaryy=aryy, maxlen=amnt, best=cst, bestback=back;
   }
  if(!maxlen) ofst[x]=t->co-1, maxlen=1;
  else if(best<2) for(z=0;z!=maxlen;++z) ofst[x+z]=t->co-1;
  else for(z=0;z!=maxlen-bestback;++z)
   ofst[x+z+bestback]=t->ary[maxaryy].loc-x;
  x+=maxlen;
  }

/* Apply scrolling commands */

for(x=0;x!=t->co-1;++x)
 {
 int q=ofst[x];
 if(q && q!=t->co-1)
  if(q>0)
   {
   int z,fu;
   for(z=x;z!=t->co-1 && ofst[z]==q;++z);
   while(s[x]==cs[x] && x<placex) ++x;
   dodelchr(t,x,y,q);
   for(fu=x;fu!=t->co-1;++fu) if(ofst[fu]!=t->co-1) ofst[fu]-=q;
   x=z-1;
   }
  else
   {
   int z,fu;
   for(z=x;z!=t->co-1 && ofst[z]==q;++z);
   while(s[x+q]==cs[x+q] && x-q<placex) ++x;
   doinschr(t,x+q,y,s+x+q,-q);
   for(fu=x;fu!=t->co-1;++fu) if(ofst[fu]!=t->co-1) ofst[fu]-=q;
   x=z-1;
   }
 }
}

static void doupscrl(t,top,bot,amnt)
SCRN *t;
int top,bot,amnt;
{
int a=amnt;
if(!amnt) return;
attr(t,0);
if(top==0 && bot==t->li && (t->sf || t->SF))
 {
 setregn(t,0,t->li);
 cpos(t,0,t->li-1);
 if(amnt==1 && t->sf || !t->SF) while(a--) texec(t->cap,t->sf,1,t->li-1);
 else texec(t->cap,t->SF,a,a);
 goto done;
 }
if(bot==t->li && (t->dl || t->DL))
 {
 setregn(t,0,t->li);
 cpos(t,0,top);
 if(amnt==1 && t->dl || !t->DL) while(a--) texec(t->cap,t->dl,1,top);
 else texec(t->cap,t->DL,a,a);
 goto done;
 }
if(t->cs && ( t->sf || t->SF ))
 {
 setregn(t,top,bot);
 cpos(t,0,bot-1);
 if(amnt==1 && t->sf || !t->SF) while(a--) texec(t->cap,t->sf,1,bot-1);
 else texec(t->cap,t->SF,a,a);
 goto done;
 }
if((t->dl || t->DL) && (t->al || t->AL))
 {
 cpos(t,0,top);
 if(amnt==1 && t->dl || !t->DL) while(a--) texec(t->cap,t->dl,1,top);
 else texec(t->cap,t->DL,a,a);
 a=amnt;
 cpos(t,0,bot-amnt);
 if(amnt==1 && t->al || !t->AL) while(a--) texec(t->cap,t->al,1,bot-amnt);
 else texec(t->cap,t->AL,a,a);
 goto done;
 }
msetI(t->updtab+top,1,bot-top);
return;

done:
mfwrd(t->scrn+top*t->co,t->scrn+(top+amnt)*t->co,
      (bot-top-amnt)*t->co*sizeof(int));
if(bot==t->li && t->db)
 {
 msetI(t->scrn+(t->li-amnt)*t->co,-1,amnt*t->co);
 msetI(t->updtab+t->li-amnt,1,amnt);
 }
else msetI(t->scrn+(bot-amnt)*t->co,' ',amnt*t->co);
}

static void dodnscrl(t,top,bot,amnt)
SCRN *t;
int top,bot,amnt;
{
int a=amnt;
if(!amnt) return;
attr(t,0);
if(top==0 && bot==t->li && (t->sr || t->SR))
 {
 setregn(t,0,t->li);
 cpos(t,0,0);
 if(amnt==1 && t->sr || !t->SR)
  while(a--) texec(t->cap,t->sr,1,0);
 else texec(t->cap,t->SR,a,a);
 goto done;
 }
if(bot==t->li && (t->al || t->AL))
 {
 setregn(t,0,t->li);
 cpos(t,0,top);
 if(amnt==1 && t->al || !t->AL)
  while(a--) texec(t->cap,t->al,1,top);
 else texec(t->cap,t->AL,a,a);
 goto done;
 }
if(t->cs && (t->sr || t->SR))
 {
 setregn(t,top,bot);
 cpos(t,0,top);
 if(amnt==1 && t->sr || !t->SR)
  while(a--) texec(t->cap,t->sr,1,top);
 else texec(t->cap,t->SR,a,a);
 goto done;
 }
if((t->dl || t->DL) && (t->al || t->AL))
 {
 cpos(t,0,bot-amnt);
 if(amnt==1 && t->dl || !t->DL)
  while(a--) texec(t->cap,t->dl,1,bot-amnt);
 else texec(t->cap,t->DL,a,a);
 a=amnt;
 cpos(t,0,top);
 if(amnt==1 && t->al || !t->AL)
  while(a--) texec(t->cap,t->al,1,top);
 else texec(t->cap,t->AL,a,a);
 goto done;
 }
msetI(t->updtab+top,1,bot-top);
return;
done:
mbkwd(t->scrn+(top+amnt)*t->co,t->scrn+top*t->co,
      (bot-top-amnt)*t->co*sizeof(int));
if(!top && t->da)
 {
 msetI(t->scrn,-1,amnt*t->co);
 msetI(t->updtab,1,amnt);
 }
else msetI(t->scrn+t->co*top,' ',amnt*t->co);
}

void nscroll(t)
SCRN *t;
{
int y,z,q,r,p;
for(y=0;y!=t->li;++y)
 {
 q=t->sary[y];
 if(have) return;
 if(q && q!=t->li)
  if(q>0)
   {
   for(z=y;z!=t->li && t->sary[z]==q;++z) t->sary[z]=0;
   doupscrl(t,y,z+q,q), y=z-1;
   }
  else
   {
   for(r=y;r!=t->li && (t->sary[r]<0 || t->sary[r]==t->li);++r);
   p=r-1; do
    {
    q=t->sary[p];
    if(q && q!=t->li)
     {
     for(z=p;t->sary[z]=0, (z && t->sary[z-1]==q);--z);
     dodnscrl(t,z+q,p+1,-q);
     p=z+1;
     }
    }
    while(p--!=y);
   y=r-1;
   }
 }
msetI(t->sary,0,t->li);
}

void npartial(t)
SCRN *t;
 {
 attr(t,0);
 clrins(t);
 setregn(t,0,t->li);
 }

void nescape(t)
SCRN *t;
 {
 npartial(t);
 cpos(t,0,t->li-1);
 eraeol(t,0,t->li-1);
 if(t->te) texec(t->cap,t->te,1);
 }

void nreturn(t)
SCRN *t;
 {
 if(t->ti) texec(t->cap,t->ti,1);
 nredraw(t);
 }

void nclose(t)
SCRN *t;
{
int x;
leave=1;
attr(t,0);
clrins(t);
setregn(t,0,t->li);
cpos(t,0,t->li-1);
if(t->te) texec(t->cap,t->te,1);
ttclose();
rmcap(t->cap);
free(t->scrn);
free(t->sary);
free(t->ofst);
free(t->htab);
free(t->ary);
free(t);
}

void nscrldn(t,top,bot,amnt)
SCRN *t;
int top,bot,amnt;
{
int x;
if(!amnt || top>=bot || bot>t->li) return;
if(amnt<bot-top && bot-top-amnt<amnt/2 || !t->scroll) amnt=bot-top;
if(amnt<bot-top)
 {
 for(x=bot;x!=top+amnt;--x)
  t->sary[x-1]=(t->sary[x-amnt-1]==t->li?t->li:t->sary[x-amnt-1]-amnt),
  t->updtab[x-1]=t->updtab[x-amnt-1];
 for(x=top;x!=top+amnt;++x) t->updtab[x]=1;
 }
if(amnt>bot-top) amnt=bot-top;
msetI(t->sary+top,t->li,amnt);
if(amnt==bot-top) msetI(t->updtab+top,1,amnt);
}

void nscrlup(t,top,bot,amnt)
SCRN *t;
int top,bot,amnt;
{
int x;
if(!amnt || top>=bot || bot>t->li) return;
if(amnt<bot-top && bot-top-amnt<amnt/2 || !t->scroll) amnt=bot-top;
if(amnt<bot-top)
 {
 for(x=top+amnt;x!=bot;++x)
  t->sary[x-amnt]=(t->sary[x]==t->li?t->li:t->sary[x]+amnt),
  t->updtab[x-amnt]=t->updtab[x];
 for(x=bot-amnt;x!=bot;++x) t->updtab[x]=1;
 }
if(amnt>bot-top) amnt=bot-top;
msetI(t->sary+bot-amnt,t->li,amnt);
if(amnt==bot-top) msetI(t->updtab+bot-amnt,1,amnt);
}

extern int dostaupd;

void nredraw(t)
SCRN *t;
{
dostaupd=1;
msetI(t->scrn,' ',t->co*skiptop);
msetI(t->scrn+skiptop*t->co,-1,(t->li-skiptop)*t->co);
msetI(t->sary,0,t->li);
msetI(t->updtab+skiptop,-1,t->li-skiptop);
t->x= -1;
t->y= -1;
t->top=t->li;
t->bot=0;
t->attrib= -1;
t->ins= -1;
attr(t,0);
clrins(t);
setregn(t,0,t->li);

if(!skiptop)
 if(t->cl)
  {
  texec(t->cap,t->cl,1,0);
  t->x=0; t->y=0;
  msetI(t->scrn,' ',t->li*t->co);
  }
 else if(t->cd)
  {
  cpos(t,0,0);
  texec(t->cap,t->cd,1,0);
  msetI(t->scrn,' ',t->li*t->co);
  }
}
