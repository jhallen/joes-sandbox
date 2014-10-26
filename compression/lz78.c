/* LZ78 compression */

#include <string.h>
#include "lz78.h"

extern void *malloc(int);

/* Dictionary size */
#define HTSIZE 16384

/* Dictionary */
int *larray;
int *carray;
int *codearray;

/* Initialize compressor/uncompressor */

int izcmp(void)
 {
 if(!(larray=malloc(sizeof(int)*HTSIZE))) return -1;
 if(!(carray=malloc(sizeof(int)*HTSIZE))) return -1;
 if(!(codearray=malloc(sizeof(int)*HTSIZE))) return -1;
 return 0;
 }

/* Compress block 'src' or size 'len' to output buffer 'dst'.  Returns
 * compressed size of block. */

int cmp(unsigned char *dst,unsigned char *src,int len)
 {
 int x;
 int l, c;
 int count=0;
 int ln=len;
 unsigned char *s=src;
 unsigned char *d=dst;
 unsigned assm=0;		/* Bit assembler */
 int nbits=0;			/* No. bits in bit assembler */
 int code=256;			/* Current code */
 int end=512;			/* Add another bit when code reaches here */
 int bits=9;			/* No. bits needed for current code */
 int tmp;

 /* Current and previous character */

 if(!len)       /* Empty block */
  return 0;

 /* Quick check for run-length compression */
 c=s[0];
 for(x=0;x!=len;++x) if(s[x]!=c) break;
 if(x==len)
  { /* Run length coding for constant blocks */
  *d++=0;
  *d++=c;
  *d++=len;
  *d++=(len>>8);
  return d-dst;
  }

 /* Initalize dictionary */
 memset(carray,255,sizeof(int)*HTSIZE);
 memset(larray,255,sizeof(int)*HTSIZE);
 memset(codearray,255,sizeof(int)*HTSIZE);

 /* Compress */

 /* Get first character */
 l= *s++; --ln;

 /* Output compression type */
 *d++=2;

 while(ln)
  {
  int hv;
  c= *s++; --ln;
  
  /* Find string l+c in hash table */
  for(hv=((l<<8)^(c)^(l>>6))&(HTSIZE-1);larray[hv]!= -1;hv=(hv+1)&(HTSIZE-1))
   if(larray[hv]==l && carray[hv]==c) break;

  if(larray[hv]!= -1)
   { /* We found the entry */
   l=codearray[hv];
   }
  else
   {
   /* String l+c is not in hash table: add an entry */
   larray[hv]=l;
   carray[hv]=c;
   codearray[hv]=code++;
   if(code==end) count=3;
   if(count>0) if(!--count) (end<<=1), ++bits;

   /* Output 'l' */
   tmp=bits;
   while(tmp)
    {
    assm|=(l<<nbits); 
    if(tmp+nbits<=16) nbits+=tmp, tmp=0;
    else tmp-=(16-nbits), l>>=(16-nbits), nbits=16;
    while(nbits>=8)
     {
     *d++=assm; assm>>=8; nbits-=8;
     if(d-dst==len) goto none;
     }
    }
   /* Begin new string */
   l=c;
   }
  }

 /* Output last code */
 tmp=bits;
 while(tmp)
  {
  assm|=(l<<nbits); 
  if(tmp+nbits<=16) nbits+=tmp, tmp=0;
  else tmp-=(16-nbits), l>>=(16-nbits), nbits=16;
  while(nbits>=8)
   {
   *d++=assm; assm>>=8; nbits-=8;
   if(d-dst==len) goto none;
   }
  }
 while(nbits>0)
  {
  *d++=assm; assm>>=8; nbits-=8;
  if(d-dst==len) goto none;
  }

 return d-dst;

 /* No compression */
 none:
 dst[0]=1;			/* Code for no compression */
 memcpy(dst+1,src,len);		/* Just copy the block */
 return len+1;
 }

/* Uncompress.  'src' contains compressed data, 'len' is compressed
 * size of data.  'dst' gets the uncompressed data, and ucmp returns
 * the original size of the data.
 */

int ucmp(unsigned char *dst, unsigned char *src, int len)
 {
 if(!len)       /* No data */
  return 0;
 
 if(*src==0)    /* Zeroed block */
  {
  int x;
  int c=src[1];
  len=src[2]+(src[3]<<8);
  for(x=0;x!=len;++x) dst[x]=c;
  return len;
  }

 if(*src==1)    /* No compression */
  {
  if(len-1) memcpy(dst,src+1,len-1);
  return len-1;
  }

 if(*src==2)    /* lz78 */
  {
  int x, q, cnt;
  
  /* Word assembler */
  unsigned long assm=0;
  int nbits=0;

  /* Code manager */
  int bits=9;
  int end=512;

  /* Current string */
  int l, c;
  int code;
  unsigned char *d=dst;
  code=256;

  /* Uncompress */

  ++src; --len;

  /* Get first character */
  while(nbits<bits)
   {
   assm|=((unsigned long)*src++<<nbits);
   --len;
   nbits+=8;
   }
  l=(assm&(end-1));
  assm>>=bits; nbits-=bits;
  *d++=l;

  while(len)
   {
   /* Get */
   while(nbits<bits)
    {
    assm|=((unsigned long)*src++<<nbits);
    --len;
    nbits+=8;
    }
   c=(assm&(end-1));
   assm>>=bits; nbits-=bits;

   if(c<256)
    {
    /* Got a character */
    /* Must have generated a new code at this point */
    larray[code]=l;
    carray[code]=c;
    ++code;

    /* Output */
    *d++=c;

    /* Start a new string */
    l=c;
    }
   else
    {
    /* Got a code */

    /* Must have generated a new code at this point */
    larray[code]=l;
    for(q=larray[c];q>=256;q=larray[q]);
    carray[code]=q;
    ++code;

    /* Start a new string */
    l=c;

    /* Output */
    cnt=0;
    while(larray[c]>=256)
     {
     ++cnt;
     *d++=carray[c];
     c=larray[c];
     }
    *d++=carray[c]; ++cnt;
    *d++=larray[c]; ++cnt;
    for(x=0;x!=cnt/2;++x)
     {
     q=d[-1-x];
     d[-1-x]=d[-cnt+x];
     d[-cnt+x]=q;
     }
    }
   if(code==end) end*=2, bits+=1;
   }
  return d-dst;
  }
 
 return -1;     /* Don't know how to uncompress this */
 }
