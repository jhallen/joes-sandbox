/* Joe's Varient of lz77 compession
   Copyright (C) 1994 Joseph H. Allen

 This is lz77 compression with two modifications:
   1) It can use a non-greedy search (see the flag LOOKAHEAD below)
   2) Indexes into the compression window are always multiplied by the
      string length.  This way longer strings which are further away
      are still useable

This is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 1, or (at your option) any later version.

This software is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
more details.

You should have received a copy of the GNU General Public License along with
this software; see the file COPYING.  If not, write to the Free Software
Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <stdio.h>

/* Parameters */

#define REACH 512	/* Maximum number of offsets (must be power of 2) */
			/* Pointers will be able to offset to REACH*(SLEN+1)
			   characters before the pointer */
#define RBITS 9		/* Log base 2 of above */

#define SLEN 8		/* No. of string lengths (must be power of 2) */
			/* Strings of lengths 2 through SLEN+1 will be used */
#define SBITS 3		/* Log base 2 of above */

/* Amount of lookahead in non-greedy search */
/* If set to 0, search becomes simple greedy search */
#define LOOKAHEAD 1

#define HTSIZE (REACH*SLEN*2)	/* Hash table size */

/* 9 bits are used for literal characters */
/* SBITS+RBITS+1 are used for pointers */

/** Bit assembler/dis-assembler **/

int accu;
int nbits;

/* I/O buffer */

unsigned char *buf;

/* Output n bits of c */

void emit(c,n)
 {
 accu|=c<<nbits;
 nbits+=n;
 while(nbits>=8) *buf++=accu, nbits-=8, accu>>=8;
 }

/* Output any remaining bits */

void flsh()
 {
 if(nbits) *buf++=accu, nbits=0;
 }

/* Get n bits */

int get(n)
 {
 unsigned c;
 while(nbits<n) accu|=*buf++<<nbits, nbits+=8;
 c=accu;
 accu>>=n; nbits-=n;
 return c&((1<<n)-1);
 }

/** Database **/

struct entry
 {
 unsigned char str[SLEN+2];	/* String */
 int len;			/* Length of string */
 int pos;			/* Position string was found */
 struct entry *next;		/* Next entry w/ same hash value */
 struct entry *prev;		/* Prev. entry w/ same hash value */
 } *htab[HTSIZE];

/* Find longest (up to given length) matching string */

struct entry *find(str,len)
unsigned char *str;
 {
 unsigned hval;
 struct entry *e, *f;
 int x;
 f=0;
 hval=0;
 for(x=0;x!=len;++x)
  {
  hval=(hval<<4)+str[x];
  for(e=htab[hval%HTSIZE];e;e=e->next)
   if(x+1==e->len && !memcmp(e->str,str,x+1)) f=e;
  }
 return f;
 }

/* Add a string of a given length to the database */

void add(str,len,pos)
unsigned char *str;
 {
 unsigned hval;
 struct entry *e;
 int x;
 for(x=0,hval=0;x!=len;++x) hval=(hval<<4)+str[x];
 hval%=HTSIZE;
 e=(struct entry *)malloc(sizeof(struct entry));
 e->len=len;
 e->pos=pos;
 memcpy(e->str,str,len);
 e->str[len]=0;
 e->next=htab[hval];
 e->prev=0;
 if(htab[hval]) htab[hval]->prev=e;
 htab[hval]=e;
 }

/* Delete string from hash table */

void del(str,len,pos)
unsigned char *str;
 {
 unsigned hval;
 struct entry *e;
 int x;
 char bb[100];
 memcpy(bb,str,len);
 bb[len]=0;
 for(x=0,hval=0;x!=len;++x) hval=(hval<<4)+str[x];
 hval%=HTSIZE;
 for(e=htab[hval];e;e=e->next)
  if(len==e->len && pos==e->pos && !memcmp(e->str,str,len))
   {
   if(e->prev) e->prev->next=e->next;
   else htab[hval]=e->next;
   if(e->next) e->next->prev=e->prev;
   free(e);
   return;
   }
 fprintf(stderr,"Not found?\n");
 }

/* Clear hash table */

void clrtab()
 {
 int x;
 for(x=0;x!=HTSIZE;++x)
  {
  struct entry *e, *n;
  for(e=htab[x];e;e=n)
   {
   n=e->next;
   free(e);
   }
  htab[x]=0;
  }
 }

/** Compression/Decompression routines **/

/* Compress block at 'src' of length 'len' into buffer 'dst'.  Returns
 * compressed size.
 */

/* Calculate benefit of twp adjacent finds */

int lzj(dst,src,len)
unsigned char *dst, *src;
 {
 int x;

 /* Initialize output buffer */
 buf=dst;
 accu=0;
 nbits=0;

 /* Compress */
 for(x=0;x!=len;++x)
  {
  struct entry *e;
  int y;

  /* Add/Remove strings from database */
  for(y=2;y!=SLEN+2;++y)
   if(x>=y && !(x%y))
    {
    if(x/y>=REACH+1) del(src+x-(REACH+1)*y,y,x-(REACH+1)*y);
    add(src+x-y,y,x-y);
    }

  /* Lookup current string */
  /* Nongreedy search */
  if(len-x>=SLEN+1)
   {
   int benefit=0;
   int n;
   e=0;
   for(n=SLEN+1;n!=0;--n)
    {
    struct entry *f, *g;
    int l=x;
    int m;

    if(n>1) f=find(src+x,n);
    else f=0;
    if(f) l+=f->len;
    else ++l;

    for(m=0;m!=LOOKAHEAD;++m)
     {
     if(len-l>=SLEN+1) g=find(src+l,SLEN+1);
     else g=0;
     if(g) l+=g->len;
     else ++l;
     }

    if(l>benefit) benefit=l, e=f;
    }
   }
  else e=find(src+x,len-x);
  
  /* Output pointer or character */
  if(e)
   { /* Output pointer */
   int z;
   emit(1,1);
   emit(e->len-2,SBITS);
   emit((x-e->pos)/e->len-1,RBITS);

   /* Advance by size of found string: Have to add/remove strings here too */
   for(z=0;z!=e->len-1;++z)
    {
    ++x;
    for(y=2;y!=SLEN+2;++y)
     if(x>=y && !(x%y))
      {
      if(x/y>=REACH+1) del(src+x-(REACH+1)*y,y,x-(REACH+1)*y);
      add(src+x-y,y,x-y);
      }
    }

   }
  else
   /* Output character */
   emit(0,1), emit(src[x],8);
  }
  
 /* Flush any remaining bits */
 flsh();

 /* Clear hash table */
 clrtab();

 /* Return compressed size */
 return buf-dst;
 }

/* Uncompress a data in 'src' into buffser 'dst'.  Original size of data
 * must be passed in 'len'
 */

void ulzj(dst,src,len)
unsigned char *dst, *src;
 {
 int x;

 /* Initialize input buffer */
 buf=src;
 accu=0;
 nbits=0;
 
 /* Uncompress */
 for(x=0;x<len;)
  if(get(1))
   /* A pointer: copy a string */
   {
   int len=get(SBITS)+2;
   int offset=get(RBITS)+1;
   memcpy(dst+x,dst+x-len*offset-(x-len*offset)%len,len);
   x+=len;
   }
  else
   /* A literal character */
   dst[x++]=get(8);
 }

/** Example main **/

int main(argc,argv)
char *argv[];
 {
 unsigned char ibuf[65536];
 unsigned char obuf[65536];
 int bksize=16384;
 int olen;
 int ilen;
 int dcmp=0;
 int x;

 /* Uncompress if name starts with 'u' */
 for(x=strlen(argv[0])-1;x>0;--x) if(argv[0][x-1]=='/') break;
 if(argv[0][x]=='u') dcmp=1;

 /* Parse options */
 for(x=1;x!=argc;++x)
  if(!strcmp(argv[x],"-d")) dcmp=1;
  else if(!strcmp(argv[x],"-c")) dcmp=0;
  else if(!strcmp(argv[x],"-b"))
   {
   sscanf(argv[x+1],"%d",&bksize);
   ++x;
   }
  else
   {
   fprintf(stderr,"Compress/Decompress from stdin to stdout\n");
   fprintf(stderr,"-d          Decompress\n");
   fprintf(stderr,"-c          Compress\n");
   fprintf(stderr,"-b blksize  Set block size (max=32768, default=16384)\n");
   exit(1);
   }

 if(dcmp)
  { /* Uncompress */
  bksize=(unsigned)getchar();		/* Get block size */
  bksize+=(unsigned)getchar()<<8;
  while((ilen=getchar())!= -1)		/* Get compressed block size */
   {
   ilen+=getchar()<<8;
   if(!ilen)			/* If last block, get final block size */
    {
    bksize=(unsigned)getchar();
    bksize+=(unsigned)getchar()<<8;
    fread(ibuf,1,65536,stdin);
    }
   else fread(ibuf,1,ilen,stdin);
   ulzj(obuf,ibuf,bksize);		/* Uncompress a block */
   fwrite(obuf,1,bksize,stdout);	/* Output the block */
   if(!ilen) break;
   }
  }
 else
  { /* Compress */
  putchar(bksize);	/* Output block size */
  putchar(bksize>>8);
  do
   {
   ilen=fread(ibuf,1,bksize,stdin);	/* Input a block */
   if(ilen>0)
    {
    olen=lzj(obuf,ibuf,ilen);		/* Compress a block */
    if(ilen!=bksize)
     putchar(0),		/* Indicate end of file */
     putchar(0),
     putchar(ilen&255),		/* Output final block size */
     putchar(ilen>>8);
    else
     putchar(olen&255),		/* Output compressed block size */
     putchar(olen/256);
    fwrite(obuf,1,olen,stdout);
    }
   } while(ilen==bksize);
  }
 }
