/* B-Tree library
   Copyright (C) 1992 Joseph H. Allen

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 1, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
more details.

You should have received a copy of the GNU General Public License along with
this program; see the file COPYING.  If not, write to the Free Software
Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "blocks.h"	/* Memcpy library */
#include "vfile.h"	/* Buffer system */
#include "vs.h"		/* Variable length strings */
#include "va.h"		/* Variable length arrays of strings */
#include "btree.h"

/* Macros for transferring longs/shorts between variables and disk blocks */

#define GETL(x,y) \
 ( \
 (x)[0]=((unsigned char *)(y))[0], \
 ((unsigned char *)(x))[1]=((unsigned char *)(y))[1], \
 ((unsigned char *)(x))[2]=((unsigned char *)(y))[2], \
 ((unsigned char *)(x))[3]=((unsigned char *)(y))[3] \
 )
/* (x)[0]=((unsigned long *)(y))[0] \ */
/*
 (x)[0]=((unsigned char *)(y))[0], \
 ((unsigned char *)(x))[2]=((unsigned char *)(y))[1], \
 ((unsigned char *)(x))[1]=((unsigned char *)(y))[2], \
 ((unsigned char *)(x))[0]=((unsigned char *)(y))[3] \
*/

#define PUTL(x,y) \
 ( \
 ((unsigned char *)(y))[0]=((unsigned char *)(x))[0], \
 ((unsigned char *)(y))[1]=((unsigned char *)(x))[1], \
 ((unsigned char *)(y))[2]=((unsigned char *)(x))[2], \
 ((unsigned char *)(y))[3]=((unsigned char *)(x))[3] \
 )
/* ((unsigned long *)(y))[0]=(x)[0] \ */
/*
 ((unsigned char *)(y))[0]=((unsigned char *)(x))[3], \
 ((unsigned char *)(y))[1]=((unsigned char *)(x))[2], \
 ((unsigned char *)(y))[2]=((unsigned char *)(x))[1], \
 ((unsigned char *)(y))[3]=((unsigned char *)(x))[0] \
*/

#define GETW(x,y) \
 ( \
 (x)[0]=((unsigned char *)(y))[0], \
 ((unsigned char *)(x))[1]=((unsigned char *)(y))[1] \
 )
/* (x)[0]=((unsigned short *)(y))[0] \ */
/* (x)[0]=((unsigned char *)(y))[0], \
 ((unsigned char *)(x))[0]=((unsigned char *)(y))[1] \
*/

#define PUTW(x,y) \
 ( \
 ((unsigned char *)(y))[0]=((unsigned char *)(x))[0], \
 ((unsigned char *)(y))[1]=((unsigned char *)(x))[1] \
 )
/* ((unsigned short *)(y))[0]=(x)[0] \ */
/*
 ((unsigned char *)(y))[0]=((unsigned char *)(x))[1], \
 ((unsigned char *)(y))[1]=((unsigned char *)(x))[0] \
*/

BTREE *btopen(name)
char *name;
 {
 BTREE *bt=(BTREE *)malloc(sizeof(BTREE));
 char *s;
 char *ptr, *p1;
 if(!(bt->fd=vopen(name))) { free(bt);  return 0;  } 
 ptr=vlock(bt->fd,0L);
 GETL(&bt->top,ptr);
 GETL(&bt->free,ptr+4);
 GETL(&bt->nrecs,ptr+8);
 s=vsncpy(NULL,0,ptr+12,slen(ptr+12));
 bt->colnames=vawords(NULL,s,sLEN(s),":",1);
 p1=ptr+12+sLEN(s)+1;
 s=vstrunc(s,0);
 s=vsncpy(s,0,p1,slen(p1));
 bt->colattribs=vawords(NULL,s,sLEN(s),":",1);
 vsrm(s);
 vunlock(ptr);
 btfirst(bt);
 return bt;
 }

void btclose(tree)
BTREE *tree;
 {
 vclose(tree->fd);
 varm(tree->colnames);
 varm(tree->colattribs);
 free(tree);
 }

void btcreat(name,colnames,colattribs)
char *name, *colnames, *colattribs;
 {
 char buf[BTPGSIZE];
 int fd=creat(name,0666);
 buf[0]=0; buf[1]=0; buf[2]=0; buf[3]=0;
 buf[4]=0; buf[5]=0; buf[6]=0; buf[7]=0;
 buf[8]=0; buf[9]=0; buf[10]=0; buf[11]=0;
 strcpy(buf+12,colnames);
 strcpy(buf+12+strlen(colnames)+1,colattribs);
 write(fd,buf,BTPGSIZE);
 close(fd);
 }

static void btleftmost(tree,down)
BTREE *tree;
long down;
 {
 do
  {
  char *ptr=vlock(tree->fd,down);
  tree->stk[tree->stkptr++]=down+8;
  GETL(&down,ptr);
  vunlock(ptr);
  } while(down);
 }

int btfirst(tree)
BTREE *tree;
 {
 tree->stkptr=0;
 if(!tree->top) return 0;
 btleftmost(tree,tree->top);
 return 1;
 }

static void btrightmost(tree,down)
BTREE *tree;
long down;
 {
 do
  {
  int ofst;
  char *ptr=vlock(tree->fd,down);
  GETW(&ofst,ptr+4);
  tree->stk[tree->stkptr++]=down+ofst;
  GETL(&down,ptr+ofst-4);
  vunlock(ptr);
  } while(down);
 tree->stk[tree->stkptr-1]-=6;
 }

int btlast(tree)
BTREE *tree;
 {
 tree->stkptr=0;
 if(!tree->top) return 0;
 btrightmost(tree,tree->top);
 tree->stk[tree->stkptr-1]+=6;
 return 1;
 }

int btfwrd(tree)
BTREE *tree;
 {
 int flg=0;
 char *ptr;
 int bod;
 int ofst;
 long down;

 loop:
 if(!tree->stkptr)
  {
  btlast(tree);
  return 0;
  }
 down=(tree->stk[tree->stkptr-1]&~(BTPGSIZE-1));
 ofst=(tree->stk[tree->stkptr-1]&(BTPGSIZE-1));
 ptr=vlock(tree->fd,down);
 GETW(&bod,ptr+4);
 if(ofst==bod)
  {
  vunlock(ptr);
  --tree->stkptr;
  flg=1;
  goto loop;
  }
 GETL(&down,ptr+ofst+2);
 vunlock(ptr);
 if(flg) return 1;
 ofst+=6;
 tree->stk[tree->stkptr-1]+=6;
 if(down)
  {
  btleftmost(tree,down);
  return 1;
  }
 if(ofst==bod)
  {
  --tree->stkptr;
  flg=1;
  goto loop;
  }
 return 1;
 }

int btbkwd(tree)
BTREE *tree;
 {
 int flg=0;
 char *ptr;
 int ofst;
 long down;
 long left;
 
 loop:
 if(!tree->stkptr)
  {
  btfirst(tree);
  return 0;
  }
 down=(tree->stk[tree->stkptr-1]&~(BTPGSIZE-1));
 ofst=(tree->stk[tree->stkptr-1]&(BTPGSIZE-1));
 ptr=vlock(tree->fd,down);
 if(ofst==8)
  {
  GETL(&left,ptr);
  vunlock(ptr);
  if(flg || !left)
   {
   if(tree->stkptr==1)
    {
    if(left) btfirst(tree);
    return 0;
    }
   --tree->stkptr;
   flg=1;
   vunlock(ptr);
   goto loop;
   }
  }
 else
  {
  GETL(&left,ptr+ofst-4);
  vunlock(ptr);
  if(flg || !left)
   {
   tree->stk[tree->stkptr-1]=down+ofst-6;
   return 1;
   }
  }
 btrightmost(tree,left);
 return 1;
 }

int btsearch(tree,cmp,cmpptr,data,datasz)
BTREE *tree;
int (*cmp)();
void *cmpptr;
char *data;
int datasz;
 {
 long down=tree->top;
 tree->stkptr=0;
 while(down)
  {
  char *ptr;
  int bod;
  int eod;
  int bor;
  int eor;
  int ofst;
  int x,y,z;
  ptr=vlock(tree->fd,down);
  GETW(&bod,ptr+4);
  GETW(&eod,ptr+6);
  x=0; y=(bod-8)/6; z=~0;
  while(z!=(x+y)/2)
   {
   z=(x+y)/2;
   ofst=z*6+8;
   GETW(&bor,ptr+ofst);
   if(ofst+6==bod) eor=eod;
   else GETW(&eor,ptr+ofst+6);
   switch(cmp(cmpptr,data,datasz,ptr+bor,eor-bor))
    {
   case  1:
    x=z;
    break;
   case -1:
    y=z;
    break;
   case  0:
    tree->stk[tree->stkptr++]=down+ofst;
    vunlock(ptr);
    return 1;
    }
   }
  ofst=y*6+8;
  tree->stk[tree->stkptr++]=down+ofst;
  if(ofst==8) GETL(&down,ptr);
  else GETL(&down,ptr+ofst-4);
  vunlock(ptr);
  }
 return 0;
 }

char *btread(tree,s)
BTREE *tree;
char *s;
 {
 long blkadr;	/* File address of block */
 int recofst;	/* Block offset to record header */
 char *ptr;		/* Address of block */
 int bod;		/* Block offset to end of header section */
 int bor;		/* Block offset to start of our record */
 int eor;		/* Block offset to end of our record */
 if(!tree->stkptr) { vsrm(s); return 0; }
 blkadr=tree->stk[tree->stkptr-1]&~(BTPGSIZE-1);
 recofst=tree->stk[tree->stkptr-1]&(BTPGSIZE-1);
 ptr=vlock(tree->fd,blkadr);
 GETW(&bod,ptr+4);
 if(recofst==bod) { vsrm(s); return 0; }
 GETW(&bor,ptr+recofst);
 if(recofst+6!=bod) GETW(&eor,ptr+recofst+6);
 else GETW(&eor,ptr+6);
 s=vsensure(s,eor-bor); sLen(s)=eor-bor; s[eor-bor]=0;
 mcpy(s,ptr+bor,eor-bor);
 vunlock(ptr);
 return s;
 }

static long btalloc(tree)
BTREE *tree;
 {
 if(tree->free)
  {
  long tmp=tree->free;
  char *ptr;
  ptr=vlock(tree->fd,tmp);
  GETL(&tree->free,ptr);
  vunlock(ptr);
  ptr=vlock(tree->fd,0L);
  PUTL(&tree->free,ptr+4);
  vchanged(ptr);
  vunlock(ptr);
  return tmp;
  }
 else return valloc(tree->fd,BTPGSIZE);
 }

static void btfree(tree,addr)
BTREE *tree;
long addr;
 {
 char *ptr;
 ptr=vlock(tree->fd,addr);
 PUTL(&tree->free,ptr);
 vchanged(ptr);
 vunlock(ptr);
 tree->free=addr;
 ptr=vlock(tree->fd,0L);
 PUTL(&tree->free,ptr+4);
 vchanged(ptr);
 vunlock(ptr);
 }

static long newroot(tree,left,data,datasz,right)
BTREE *tree;
long left;
char *data;
int datasz;
long right;
 {
 long node=btalloc(tree);
 char *ptr=vlock(tree->fd,node);
 int w;
 PUTL(&left,ptr);
 w=14; PUTW(&w,ptr+4);
 w=14+datasz; PUTW(&w,ptr+6);
 w=14; PUTW(&w,ptr+8);
 PUTL(&right,ptr+10);
 mcpy(ptr+14,data,datasz);
 vchanged(ptr);
 vunlock(ptr);
 return node;
 }

static void simpleins(tree,ptr,ofst,data,datasz,right)
BTREE *tree;
char *ptr;
int ofst;
char *data;
int datasz;
long right;
 {
 int bod, eod, w, bor;
 GETW(&bod,ptr+4);
 GETW(&eod,ptr+6);
 for(w=8;w!=bod;w+=6)
  {
  int q;
  GETW(&q,ptr+w);
  q+=6;
  PUTW(&q,ptr+w);
  }
 mbkwd(ptr+ofst+6,ptr+ofst,eod-ofst);
 bod+=6; eod+=6;
 if(ofst+6==bod) bor=eod;
 else GETW(&bor,ptr+ofst+6);
 for(w=ofst+6;w!=bod;w+=6)
  {
  int q;
  GETW(&q,ptr+w);
  q+=datasz;
  PUTW(&q,ptr+w);
  }
 mbkwd(ptr+bor+datasz,ptr+bor,eod-bor);
 mcpy(ptr+bor,data,datasz);
 eod+=datasz;
 PUTW(&bor,ptr+ofst);
 PUTL(&right,ptr+ofst+2);
 PUTW(&bod,ptr+4); PUTW(&eod,ptr+6);
 }

static long splitnode(tree,ptr,splited)
BTREE *tree;
char *ptr;
int *splited;
 {
 /* Split node */
 int split;
 char *nptr;
 int bod, eod, nbod, neod;
 long nnode;
 int dtao;
 int w;
 GETW(&bod,ptr+4);
 GETW(&eod,ptr+6);
 for(split=8;GETW(&dtao,ptr+split), dtao-bod+split-8<(BTPGSIZE-8)/2;split+=6);
 nnode=btalloc(tree);
 nptr=vlock(tree->fd,nnode);
 nbod=bod-split+8;
 neod=nbod+eod-dtao;
 for(w=split;w!=bod;w+=6)
  {
  int o;
  long r;
  GETW(&o,ptr+w);
  o-=dtao;
  o+=nbod;
  PUTW(&o,nptr+w-split+8);
  GETL(&r,ptr+w+2);
  PUTL(&r,nptr+w-split+10);
  }
 mcpy(nptr+nbod,ptr+dtao,eod-dtao);
 mfwrd(ptr+split,ptr+bod,dtao-bod);
 for(w=8;w!=split;w+=6)
  {
  int o;
  GETW(&o,ptr+w);
  o-=bod-split;
  PUTW(&o,ptr+w);
  }
 eod=dtao-(bod-split);
 bod=split;
 PUTW(&bod,ptr+4); PUTW(&eod,ptr+6);
 PUTW(&nbod,nptr+4); PUTW(&neod,nptr+6);
 *splited=split;
 vchanged(nptr); vunlock(nptr);
 return nnode;
 }

void doins(tree,idx,data,datasz,right)
BTREE *tree;
int idx;
char *data;
int datasz;
long right;
 {
 char insbuf[BTPGSIZE];
 int adjflg=0;
 loop:
 if(!idx)
  {
  /* Create new root node */
  char *ptr;
  tree->top=newroot(tree,tree->top,data,datasz,right);
  ptr=vlock(tree->fd,0L);
  PUTL(&tree->top,ptr);
  vchanged(ptr);
  vunlock(ptr);
  mbkwd(tree->stk+1,tree->stk,sizeof(long)*tree->stkptr);
  tree->stk[0]=tree->top+8;
  if(adjflg) tree->stk[0]+=6;
  ++tree->stkptr;
  }
 else
  {
  long node=tree->stk[idx-1]&~(BTPGSIZE-1);
  int ofst=tree->stk[idx-1]&(BTPGSIZE-1);
  int bod, eod;
  char *ptr=vlock(tree->fd,node);
  GETW(&eod,ptr+6);
  if(eod+datasz+6<=BTPGSIZE)
   {
   /* Simple insert */
   simpleins(tree,ptr,ofst,data,datasz,right);
   vchanged(ptr);
   vunlock(ptr);
   if(adjflg) tree->stk[idx-1]+=6;
   }
  else
   {
   /* Have to split the node */
   int split;
   int eor;
   int w;
   node=splitnode(tree,ptr,&split);
   if(ofst<split)
    {
    simpleins(tree,ptr,ofst,data,datasz,right);
    if(adjflg) tree->stk[idx-1]+=6;
    adjflg=0;
    }
   vchanged(ptr);
   vunlock(ptr);
   ptr=vlock(tree->fd,node);
   if(ofst>=split)
    {
    simpleins(tree,ptr,ofst-split+8,data,datasz,right);
    tree->stk[idx-1]+=node+ofst-split+8;
    if(adjflg) tree->stk[idx-1]+=6;
    adjflg=1;
    }
   GETW(&bod,ptr+4); GETW(&eod,ptr+6);
   data=insbuf;
   GETW(&eor,ptr+14);
   datasz=eor-bod;
   mcpy(insbuf,ptr+bod,datasz);
   right=node;
   GETL(&node,ptr+10);
   PUTL(&node,ptr);
   mfwrd(ptr+bod,ptr+bod+datasz,eod-(bod+datasz));
   eod-=datasz;
   for(w=14;w!=bod;w+=6)
    {
    int o;
    GETW(&o,ptr+w);
    o-=datasz+6;
    PUTW(&o,ptr+w);
    }
   mfwrd(ptr+8,ptr+14,eod-14);
   bod-=6; eod-=6;
   PUTW(&bod,ptr+4); PUTW(&eod,ptr+6);
   vchanged(ptr);
   vunlock(ptr);
   --idx;
   goto loop;
   }
  }
 }

void btins(tree,data,datasz)
BTREE *tree;
char *data;
int datasz;
 {
 char *ptr;
 doins(tree,tree->stkptr,data,datasz,0L);
 ++tree->nrecs;
 ptr=vlock(tree->fd,0L);
 PUTL(&tree->nrecs,ptr+8);
 vchanged(ptr); vunlock(ptr);
 }

int btdel(tree)
BTREE *tree;
 {
 char *ptr;
 long left;
 long right;
 long node;
 int ofst;
 int bod, eod, bor, eor;
 int w;
 if(!tree->stkptr) return 0;
 node=tree->stk[tree->stkptr-1]&~(BTPGSIZE-1);
 ofst=tree->stk[tree->stkptr-1]&(BTPGSIZE-1);
 ptr=vlock(tree->fd,node);
 GETW(&bod,ptr+4);
 GETW(&eod,ptr+6);
 if(ofst==bod) { vunlock(ptr); return 0; }
 GETL(&left,ptr);
 GETL(&right,ptr+ofst+2);
 GETW(&bor,ptr+ofst);
 if(ofst+6==bod) eor=eod;
 else GETW(&eor,ptr+ofst+6);

 mfwrd(ptr+ofst,ptr+ofst+6,eod-(ofst+6));
 bod-=6; eod-=6; bor-=6; eor-=6;
 for(w=8;w!=bod;w+=6)
  {
  int o;
  GETW(&o,ptr+w);
  o-=6;
  PUTW(&o,ptr+w);
  }

 mfwrd(ptr+bor,ptr+eor,eod-eor);
 for(w=ofst;w!=bod;w+=6)
  {
  int o;
  GETW(&o,ptr+w);
  o-=eor-bor;
  PUTW(&o,ptr+w);
  }
 eod-=eor-bor;

 if(bod==8)
  {
  vunlock(ptr);
  btfree(tree,node);
  if(!--tree->stkptr)
   {
   tree->top=left;
   ptr=vlock(tree->fd,0L);
   PUTL(&tree->top,ptr);
   vchanged(ptr);
   vunlock(ptr);
   }
  else
   {
   node=tree->stk[tree->stkptr-1]&~(BTPGSIZE-1);
   ofst=tree->stk[tree->stkptr-1]&(BTPGSIZE-1);
   ptr=vlock(tree->fd,node);
   if(ofst==8) PUTL(&left,ptr);
   else PUTL(&left,ptr+ofst-4);
   vchanged(ptr);
   vunlock(ptr);
   }
  }
 else
  {
  PUTW(&bod,ptr+4);
  PUTW(&eod,ptr+6);
  vchanged(ptr);
  vunlock(ptr);
  }
 if(right)
  {
  int tmp=tree->stkptr;
  btleftmost(tree,right);
  /* Get left-most element of sub-tree */
  node=tree->stk[tree->stkptr-1]&~(BTPGSIZE-1);
  ofst=tree->stk[tree->stkptr-1]&(BTPGSIZE-1);
  ptr=vlock(tree->fd,node);
  GETW(&bor,ptr+ofst);
  GETW(&bod,ptr+4);
  if(ofst+6==bod) eor=bod;
  else GETW(&eor,ptr+ofst+6);
  tree->stkptr=tmp;
  doins(tree,tmp,ptr+bor,eor-bor,right);
  vunlock(ptr);
  tmp=tree->stkptr;
  btleftmost(tree,right);
  tree->stk[tmp-1]+=6;
  ++tree->nrecs;
  btdel(tree);
  tree->stk[tmp-1]-=6;
  tree->stkptr=tmp;
  }
 else
  {
  loop:
  if(!tree->stkptr)
   {
   btlast(tree);
   goto done;
   }
  node=(tree->stk[tree->stkptr-1]&~(BTPGSIZE-1));
  ofst=(tree->stk[tree->stkptr-1]&(BTPGSIZE-1));
  ptr=vlock(tree->fd,node);
  GETW(&bod,ptr+4);
  vunlock(ptr);
  if(ofst==bod)
   {
   --tree->stkptr;
   goto loop;
   }
  }
 done:
 ptr=vlock(tree->fd,0L);
 --tree->nrecs;
 PUTL(&tree->nrecs,ptr+8);
 vchanged(ptr); vunlock(ptr);
 return 1;
 }
