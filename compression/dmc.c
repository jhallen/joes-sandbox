/* Dynamic Markov Compression
   Copyright (C) 1994 Joseph H. Allen

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
#include <string.h>
#include <stdlib.h>

/* The order of the statistical model */

int order=3;

/* Bit assembler/dis-assembler */

int accu;
int nbits;

/* I/O buffer */

unsigned char *buf;
int buflen;

/* Output n bits of c */

void emit(int c,int n)
 {
 accu|=c<<nbits;
 nbits+=n;
 while(nbits>=8) buf[buflen++]=accu, nbits-=8, accu>>=8;
 }

/* Output any remaining bits */

void flsh()
 {
 if(nbits) buf[buflen++]=accu, nbits=0;
 }

/* Get n bits */

int get(int n)
 {
 unsigned char c;
 while(nbits<n) accu|=*buf++<<nbits, nbits+=8;
 c=accu;
 accu>>=n; nbits-=n;
 return c&((1<<n)-1);
 }

/* Huffman node pool */

struct huff_node
 {
 struct huff_node *zero;	/* These point towards the leaves */
 struct huff_node *one;		/* (they equal 0 if this is a leaf) */

 struct huff_node *parent;	/* This points towards the root (0 for root) */
 
 struct huff_node *sort;	/* This points to node with next higher freq */

 int count;			/* Frequency of occurance of this leaf.  Or
 				   sum of freqs of children */

 struct entry *entry;		/* Points to hash table entry for this char */
 };

struct huff_node *free_huff_node=0;
struct huff_node *alloced_huff_node=0;
int nfree_huff_nodes=0;

struct huff_node *allocate_huff_node()
 {
 if(!nfree_huff_nodes)
  {
  free_huff_node=(struct huff_node *)malloc(sizeof(struct huff_node)*
                                            (nfree_huff_nodes=1024));
  free_huff_node->zero=alloced_huff_node;
  alloced_huff_node=free_huff_node++;
  --nfree_huff_nodes;
  }
 ++free_huff_node;
 --nfree_huff_nodes;
 return free_huff_node-1;
 }

void free_huff_nodes()
 {
 struct huff_node *h;
 while((h=alloced_huff_node) != 0)
  {
  alloced_huff_node=h->zero;
  free(h);
  }
 free_huff_node=0;
 nfree_huff_nodes=0;
 }

/* Hash table entry pool */

struct entry
 {
 unsigned char str[8];		/* String */
 int len;			/* Length */
 struct entry *next;		/* Next node with same hash value */
 
 /* If this entry refers to a table */
 struct huff_node *root;	/* Root node for the table */
 struct huff_node *last;	/* Last node added to table */
 struct huff_node *esc;		/* Node for ESC symbol */

 /* If this entry refers to a character in a table */
 struct huff_node *node;
 };

struct entry *free_entry=0;
int nfree_entries=0;
struct entry *alloced_entry=0;

struct entry *allocate_entry()
 {
 if(!nfree_entries)
  {
  free_entry=(struct entry *)malloc(sizeof(struct entry)*(nfree_entries=1024));
  free_entry->next=alloced_entry;
  alloced_entry=free_entry++;
  --nfree_entries;
  }
 ++free_entry;
 --nfree_entries;
 return free_entry-1;
 }

void free_entries()
 {
 struct entry *e;
 while((e=alloced_entry) != 0)
  {
  alloced_entry=e->next;
  free(e);
  }
 free_entry=0;
 nfree_entries=0;
 }

/* Hash table */

#define HTSIZE 8192
struct entry *htab[HTSIZE];

/* Add a new node to a huffman table */

void add(struct entry *base,struct entry *ent)
 {
 struct huff_node *new=allocate_huff_node();
 struct huff_node *coppied=allocate_huff_node();
 struct huff_node *last=base->last;

 new->entry=ent;
 new->count=0;
 new->sort=coppied;
 new->parent=last;
 new->zero=0;
 new->one=0;
 ent->node=new;

 coppied->entry=last->entry;
 coppied->count=last->count;
 coppied->sort=last;
 coppied->zero=last->zero;
 coppied->one=last->one;
 coppied->parent=last;
 if(coppied->entry==base) base->esc=coppied;
 else if(coppied->entry) coppied->entry->node=coppied;

 last->entry=0;
 last->zero=coppied;
 last->one=new;

 base->last=new;
 }

/* Increment count on a huffman node and update huffman tree */

void update(struct entry *base,struct huff_node *i)
 {
 while(i)
  {
  ++i->count;
  if(i->sort && i->count>i->sort->count)
   {
   struct huff_node *e;
   struct huff_node tmp;
   for(e=i->sort;e->sort && e->count==e->sort->count;e=e->sort);
   tmp= *e;

   e->count=i->count;
   e->entry=i->entry;
   e->zero=i->zero;
   e->one=i->one;
   
   i->count=tmp.count;
   i->entry=tmp.entry;
   i->zero=tmp.zero;
   i->one=tmp.one;
   
   if(i->zero)
    {
    i->zero->parent=i;
    i->one->parent=i;
    }
   else
    {
    if(i->entry==base) i->entry->esc=i;
    else i->entry->node=i;
    }

   if(e->zero)
    {
    e->zero->parent=e;
    e->one->parent=e;
    }
   else
    {
    if(e->entry==base) e->entry->esc=e;
    else e->entry->node=e;
    }

   i=e;
   }
  i=i->parent;
  }
 }

/* Create a new huffman table */

void mklevel(struct entry *base,struct entry *ent)
 {
 struct huff_node *root=allocate_huff_node();
 struct huff_node *esc=allocate_huff_node();
 struct huff_node *first=allocate_huff_node();
 
 root->parent=0;
 root->count=2;
 root->entry=0;
 root->zero=esc;
 root->one=first;
 root->sort=0;
 
 esc->parent=root;
 esc->count=1;
 esc->entry=base;
 esc->zero=0;
 esc->one=0;
 esc->sort=root;
 
 first->parent=root;
 first->count=1;
 first->entry=ent;
 first->zero=0;
 first->one=0;
 first->sort=esc;

 base->last=first;
 base->esc=esc;
 base->root=root;
 ent->node=first;
 }

/* Output a huffman code */

void up(struct huff_node *i)
 {
 if(i->parent) up(i->parent), emit(i->parent->one==i,1);
 }

/* Decode a huffman code */

struct huff_node *down(struct huff_node *i)
 {
 while(i->zero)
  if(get(1)) i=i->one;
  else i=i->zero;
 return i;
 }

/* Lookup a string in the hash table */

struct entry *find(unsigned char *str,int len)
 {
 struct entry *p;
 unsigned long hval=0;
 int x;
 for(x=0;x!=len;++x) hval=(hval<<4)+str[x];
 hval&=(HTSIZE-1);
 for(p=htab[hval];p;p=p->next)
  if(len==p->len)
   {
   for(x=0;x!=len;++x) if(str[x]!=p->str[x]) break;
   if(x==len) return p;
   }
 p=allocate_entry();
 p->len=len;
 if(len) memcpy(p->str,str,len);
 p->next=htab[hval];
 htab[hval]=p;
 p->node=0;
 p->esc=0;
 p->root=0;
 p->last=0;
 return p;
 }

/* Encode last byte of string.  All but the last byte of the string is used
 * to select the huffman table to use */

void encode(unsigned char *str,int len)
 {
 struct entry *base, *ent;
 base=find(str,len-1);
 ent=find(str,len);
 if(ent->node)
  { /* Table and character exist */
  up(ent->node);
  update(base,ent->node);
  }
 else if(base->last)
  { /* Only table exists */
  up(base->esc);
  update(base,base->esc);
  if(len-1) encode(str+1,len-1);
  else emit(str[0],8);
  add(base,ent);
  update(base,ent->node);
  }
 else
  { /* Neither table nor character exist */
  if(len-1) encode(str+1,len-1);
  else emit(str[0],8);
  mklevel(base,ent);
  }
 }

/* Decode one character.  Adds the character to right after end of string.
 * Uses characters already in the string to select huffman table */

void decode(unsigned char *str, int len)
 {
 struct entry *base=find(str,len);
 struct entry *ent;
 struct huff_node *t;
 if(base->root)
  {
  t=down(base->root);
  if(t->entry==base)
   { /* Received escape character */
   update(base,t);
   if(len)
    {
    decode(str+1,len-1);
    }
   else
    {
    str[len]=get(8);
    }
   add(base,ent=find(str,len+1));
   update(base,ent->node);
   }
  else
   { /* Received a real character */
   unsigned char c=t->entry->str[t->entry->len-1];
   update(base,t);
   str[len]=c;
   }
  }
 else
  { /* A new table */
  if(len)
   {
   decode(str+1,len-1);
   }
  else
   {
   str[len]=get(8);
   }
  mklevel(base,find(str,len+1));
  }
 }

/* Encode a block (src/len) into an output buffer (dst).  Returns encoded size
 */

int h(unsigned char *dst,unsigned char *src,int len)
 {
 int x;

 buf=dst;
 buflen=0;
 nbits=0;
 accu=0;
 
 for(x=0;x!=order;++x) encode(src,x+1);
 for(x=order;x!=len;++x) encode(src+x-order,order+1);
 
 flsh();
 
 free_huff_nodes();
 free_entries();
 for(x=0;x!=HTSIZE;++x) htab[x]=0;

 return buflen;
 }

/* Decode a block (dst/len) from a buffer (src).  Returns len.  Note: len is
 * the decoded size, not the encoded size
 */

int uh(unsigned char *dst,unsigned char *src,int len)
 {
 int x;

 buf=src;
 nbits=0;
 accu=0;
 
 for(x=0;x!=order;++x) decode(dst,x);
 for(x=order;x!=len;++x) decode(dst+x-order,order);

 free_huff_nodes();
 free_entries();
 for(x=0;x!=HTSIZE;++x) htab[x]=0;

 return len;
 }

/* Decode options and encode or decode stdin to stdout */

int main(int argc,char *argv[])
 {
 unsigned char ibuf[65536];
 unsigned char obuf[65536];
 int bksize=4096;
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
  else if(!strcmp(argv[x],"-o"))
   {
   sscanf(argv[x+1],"%d",&order);
   ++x;
   }
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
   fprintf(stderr,"-o order    Set ORDER of statistical model (0-7, default=3)\n");
   fprintf(stderr,"-b blksize  Set block size (max=32768, default=4096)\n");
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
   uh(obuf,ibuf,bksize);		/* Uncompress a block */
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
    olen=h(obuf,ibuf,ilen);		/* Compress a block */
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
 return 0;
 }
