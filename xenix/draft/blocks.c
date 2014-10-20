/* Super-fast block set and copy functions
 *
 * by: Joseph H. Allen
 * on: January 10, 1992
 */

/* Take a look at the configuration information in types.h */
/* This module requires that AUTOINC, ALIGNED, ISIZ, SHFT and BITS be defined
 * correctly
 */
#include "blocks.h"

/* Set 'sz' 'int's beginning at 'd' to the value 'c' */
/* Returns address of block.  Does nothing if 'sz' equals zero */

int *msetI(d,c,sz)
register int *d, c;
register int sz;
{
int *orgd=d;
while(sz>=16)
 {
#ifdef AUTOINC
 *d++=c; *d++=c; *d++=c; *d++=c; *d++=c; *d++=c; *d++=c; *d++=c;
 *d++=c; *d++=c; *d++=c; *d++=c; *d++=c; *d++=c; *d++=c; *d++=c;
#else
 d[0]=c; d[1]=c; d[2]=c; d[3]=c; d[4]=c; d[5]=c; d[6]=c; d[7]=c;
 d[8]=c; d[9]=c; d[10]=c; d[11]=c; d[12]=c; d[13]=c; d[14]=c; d[15]=c;
 d+=16;
#endif
 sz-=16;
 }
switch(sz)
 {
#ifdef AUTOINC
case 15: *d++=c;
case 14: *d++=c;
case 13: *d++=c;
case 12: *d++=c;
case 11: *d++=c;
case 10: *d++=c;
case 9: *d++=c;
case 8: *d++=c;
case 7: *d++=c;
case 6: *d++=c;
case 5: *d++=c;
case 4: *d++=c;
case 3: *d++=c;
case 2: *d++=c;
case 1: *d++=c;
#else
case 15: d[14]=c;
case 14: d[13]=c;
case 13: d[12]=c;
case 12: d[11]=c;
case 11: d[10]=c;
case 10: d[9]=c;
case 9: d[8]=c;
case 8: d[7]=c;
case 7: d[6]=c;
case 6: d[5]=c;
case 5: d[4]=c;
case 4: d[3]=c;
case 3: d[2]=c;
case 2: d[1]=c;
case 1: d[0]=c;
#endif
case 0:;
 }
return orgd;
}

/* Set 'sz' 'char's beginning at 'd' to the value 'c' */
/* Returns address of block.  Does nothing if 'sz' equals zero */

char *mset(d,c,sz)
register char *d, c;
register int sz;
{
char *orgd=d;
if(sz<16)
 switch(sz)
  {
#ifdef AUTOINC
  case 15: *d++=c;
  case 14: *d++=c;
  case 13: *d++=c;
  case 12: *d++=c;
  case 11: *d++=c;
  case 10: *d++=c;
  case 9: *d++=c;
  case 8: *d++=c;
  case 7: *d++=c;
  case 6: *d++=c;
  case 5: *d++=c;
  case 4: *d++=c;
  case 3: *d++=c;
  case 2: *d++=c;
  case 1: *d++=c;
#else
  case 15: d[14]=c;
  case 14: d[13]=c;
  case 13: d[12]=c;
  case 12: d[11]=c;
  case 11: d[10]=c;
  case 10: d[9]=c;
  case 9: d[8]=c;
  case 8: d[7]=c;
  case 7: d[6]=c;
  case 6: d[5]=c;
  case 5: d[4]=c;
  case 4: d[3]=c;
  case 3: d[2]=c;
  case 2: d[1]=c;
  case 1: d[0]=c;
#endif
  case 0:;
  }
else
 {
 int z=ISIZ-((int)d&(ISIZ-1));
 if(z!=ISIZ)
  switch(z)
   {
#ifdef AUTOINC
  case 7: *d++=c;
  case 6: *d++=c;
  case 5: *d++=c;
  case 4: *d++=c;
  case 3: *d++=c;
  case 2: *d++=c;
  case 1: *d++=c;
  case 0:;
#else
  case 7: d[6]=c;
  case 6: d[5]=c;
  case 5: d[4]=c;
  case 4: d[3]=c;
  case 3: d[2]=c;
  case 2: d[1]=c;
  case 1: d[0]=c;
  case 0:;
          d+=z;
#endif
   sz-=z;
   }
 msetI(d,
#if ISIZ>=8
 (c<<(BITS*7))+(c<<(BITS*6))+(c<<(BITS*5))+(c<<(BITS*4))+
#endif
#if ISIZ>=4
 (c<<(BITS*3))+(c<<(BITS*2))+
#endif
#if ISIZ>=2
 (c<<BITS)+
#endif
 c,sz>>SHFT);
 d+=sz&~(ISIZ-1);
 switch(sz&(ISIZ-1))
  {
#ifdef AUTOINC
 case 7: *d++=c;
 case 6: *d++=c;
 case 5: *d++=c;
 case 4: *d++=c;
 case 3: *d++=c;
 case 2: *d++=c;
 case 1: *d++=c;
#else
 case 7: d[6]=c;
 case 6: d[5]=c;
 case 5: d[4]=c;
 case 4: d[3]=c;
 case 3: d[2]=c;
 case 2: d[1]=c;
 case 1: d[0]=c;
#endif
 case 0:;
  }
 }
return orgd;
}

/* Copy a block of integers */
/* Copy from highest address to lowest */

static int *mbkwdI(d,s,sz)
register int *d, *s;
register int sz;
{
if(d==s) return d;
d+=sz; s+=sz;
while(sz>=16)
 {
#ifdef AUTOINC
 *--d= *--s; *--d= *--s; *--d= *--s; *--d= *--s; *--d= *--s; *--d= *--s; 
 *--d= *--s; *--d= *--s; *--d= *--s; *--d= *--s; *--d= *--s; *--d= *--s; 
 *--d= *--s; *--d= *--s; *--d= *--s; *--d= *--s;
#else
 d-=16; s-=16;
 d[15]=s[15]; d[14]=s[14]; d[13]=s[13]; d[12]=s[12]; d[11]=s[11]; d[10]=s[10];
 d[9]=s[9]; d[8]=s[8]; d[7]=s[7]; d[6]=s[6]; d[5]=s[5]; d[4]=s[4]; d[3]=s[3];
 d[2]=s[2]; d[1]=s[1]; d[0]=s[0];
#endif
 sz-=16;
 }
#ifndef AUTOINC
 d-=sz; s-=sz;
#endif
switch(sz)
 {
#ifdef AUTOINC
 case 15: *--d= *--s;
 case 14: *--d= *--s;
 case 13: *--d= *--s;
 case 12: *--d= *--s;
 case 11: *--d= *--s;
 case 10: *--d= *--s;
 case 9: *--d= *--s;
 case 8: *--d= *--s;
 case 7: *--d= *--s;
 case 6: *--d= *--s;
 case 5: *--d= *--s;
 case 4: *--d= *--s;
 case 3: *--d= *--s;
 case 2: *--d= *--s;
 case 1: *--d= *--s;
#else
 case 15: d[14]=s[14];
 case 14: d[13]=s[13];
 case 13: d[12]=s[12];
 case 12: d[11]=s[11];
 case 11: d[10]=s[10];
 case 10: d[9]=s[9];
 case 9: d[8]=s[8];
 case 8: d[7]=s[7];
 case 7: d[6]=s[6];
 case 6: d[5]=s[5];
 case 5: d[4]=s[4];
 case 4: d[3]=s[3];
 case 3: d[2]=s[2];
 case 2: d[1]=s[1];
 case 1: d[0]=s[0];
#endif
 case 0:;
 }
return d;
}

/* Copy a block of 'int's.  Copy from lowest address to highest */

static int *mfwrdI(d,s,sz)
register int *d, *s;
register int sz;
{
int *od=d;
if(s==d) return d;
while(sz>=16)
 {
#ifdef AUTOINC
 *d++= *s++; *d++= *s++; *d++= *s++; *d++= *s++;
 *d++= *s++; *d++= *s++; *d++= *s++; *d++= *s++;
 *d++= *s++; *d++= *s++; *d++= *s++; *d++= *s++;
 *d++= *s++; *d++= *s++; *d++= *s++; *d++= *s++;
#else
 d[0]=s[0]; d[1]=s[1]; d[2]=s[2]; d[3]=s[3]; d[4]=s[4]; d[5]=s[5]; d[6]=s[6];
 d[7]=s[7]; d[8]=s[8]; d[9]=s[9]; d[10]=s[10]; d[11]=s[11]; d[12]=s[12];
 d[13]=s[13]; d[14]=s[14]; d[15]=s[15];
 s+=16; d+=16;
#endif
 sz-=16;
 }
#ifndef AUTOINC
s-=15-sz; d-=15-sz;
#endif
switch(sz)
 {
#ifdef AUTOINC
 case 15: *d++= *s++;
 case 14: *d++= *s++;
 case 13: *d++= *s++;
 case 12: *d++= *s++;
 case 11: *d++= *s++;
 case 10: *d++= *s++;
 case 9: *d++= *s++;
 case 8: *d++= *s++;
 case 7: *d++= *s++;
 case 6: *d++= *s++;
 case 5: *d++= *s++;
 case 4: *d++= *s++;
 case 3: *d++= *s++;
 case 2: *d++= *s++;
 case 1: *d++= *s++;
#else
 case 15: d[0]=s[0];
 case 14: d[1]=s[1];
 case 13: d[2]=s[2];
 case 12: d[3]=s[3];
 case 11: d[4]=s[4];
 case 10: d[5]=s[5];
 case 9: d[6]=s[6];
 case 8: d[7]=s[7];
 case 7: d[8]=s[8];
 case 6: d[9]=s[9];
 case 5: d[10]=s[10];
 case 4: d[11]=s[11];
 case 3: d[12]=s[12];
 case 2: d[13]=s[13];
 case 1: d[14]=s[14];
#endif
 case 0:;
 }
return od;
}

/* Copy the block of 'sz' bytes beginning at 's' to 'd'.  If 'sz' is zero or
 * if 's'=='d', nothing happens.  The bytes at the highest address ('s'+'sz'-1)
 * are copied before the ones at the lowest ('s') are.
 */ 

char *mbkwd(d,s,sz)
register char *d, *s;
register int sz;
{
if(s==d) return d;
s+=sz; d+=sz;
#ifdef ALIGNED
if( sz>=16 )
#else
if( ((int)s&(ISIZ-1))==((int)d&(ISIZ-1)) && sz>=16)
#endif
 {
 int z=((int)s&(ISIZ-1));
#ifndef AUTOINC
 s-=z; d-=z;
#endif
 switch(z)
  {
#ifdef AUTOINC
  case 7: *--d= *--s;
  case 6: *--d= *--s;
  case 5: *--d= *--s;
  case 4: *--d= *--s;
  case 3: *--d= *--s;
  case 2: *--d= *--s;
  case 1: *--d= *--s;
#else
  case 7: d[6]=s[6];
  case 6: d[5]=s[5];
  case 5: d[4]=s[4];
  case 4: d[3]=s[3];
  case 3: d[2]=s[2];
  case 2: d[1]=s[1];
  case 1: d[0]=s[0];
#endif
  case 0:;
  }
 sz-=z;
 mbkwdI(d-(sz&~(ISIZ-1)),s-(sz&~(ISIZ-1)),sz>>SHFT);
#ifndef AUTOINC
 d-=sz; s-=sz;
#else
 d-=(sz&~(ISIZ-1)); s-=(sz&~(ISIZ-1));
#endif
 switch(sz&(ISIZ-1))
  {
#ifdef AUTOINC
  case 7: *--d= *--s;
  case 6: *--d= *--s;
  case 5: *--d= *--s;
  case 4: *--d= *--s;
  case 3: *--d= *--s;
  case 2: *--d= *--s;
  case 1: *--d= *--s;
#else
  case 7: d[6]=s[6];
  case 6: d[5]=s[5];
  case 5: d[4]=s[4];
  case 4: d[3]=s[3];
  case 3: d[2]=s[2];
  case 2: d[1]=s[1];
  case 1: d[0]=s[0];
#endif
  case 0:;
  }
 }
else
 {
 while(sz>=16)
  {
#ifdef AUTOINC
 *--d= *--s; *--d= *--s; *--d= *--s; *--d= *--s;
 *--d= *--s; *--d= *--s; *--d= *--s; *--d= *--s;
 *--d= *--s; *--d= *--s; *--d= *--s; *--d= *--s;
 *--d= *--s; *--d= *--s; *--d= *--s; *--d= *--s;
#else
  d-=16; s-=16;
  d[15]=s[15]; d[14]=s[14]; d[13]=s[13]; d[12]=s[12]; d[11]=s[11]; d[10]=s[10];
  d[9]=s[9]; d[8]=s[8]; d[7]=s[7]; d[6]=s[6]; d[5]=s[5]; d[4]=s[4]; d[3]=s[3];
  d[2]=s[2]; d[1]=s[1]; d[0]=s[0];
#endif
  sz-=16;
  }
#ifndef AUTOINC
 d-=sz; s-=sz;
#endif
 switch(sz)
  {
#ifdef AUTOINC
  case 15: *--d= *--s;
  case 14: *--d= *--s;
  case 13: *--d= *--s;
  case 12: *--d= *--s;
  case 11: *--d= *--s;
  case 10: *--d= *--s;
  case 9: *--d= *--s;
  case 8: *--d= *--s;
  case 7: *--d= *--s;
  case 6: *--d= *--s;
  case 5: *--d= *--s;
  case 4: *--d= *--s;
  case 3: *--d= *--s;
  case 2: *--d= *--s;
  case 1: *--d= *--s;
#else
  case 15: d[14]=s[14];
  case 14: d[13]=s[13];
  case 13: d[12]=s[12];
  case 12: d[11]=s[11];
  case 11: d[10]=s[10];
  case 10: d[9]=s[9];
  case 9: d[8]=s[8];
  case 8: d[7]=s[7];
  case 7: d[6]=s[6];
  case 6: d[5]=s[5];
  case 5: d[4]=s[4];
  case 4: d[3]=s[3];
  case 3: d[2]=s[2];
  case 2: d[1]=s[1];
  case 1: d[0]=s[0];
#endif
  case 0:;
  }
 }
return d;
}

char *mmove(d,s,sz)
char *d, *s;
int sz;
{
if(d>s) mbkwd(d,s,sz);
else mfwrd(d,s,sz);
}

/* Copy the block of 'sz' bytes beginning at 's' to 'd'.  If 'sz' is zero or
 * if 's'=='d', nothing happens.  The bytes at the lowest address ('s')
 * are copied before the ones at the highest ('s'+'sz'-1) are.
 */ 

char *mfwrd(d,s,sz)
register char *d, *s;
register int sz;
{
char *od=d;
if(d==s) return d;
#ifdef ALIGNED
if(sz>=16)
#else
if( ((int)d&(ISIZ-1))==((int)s&(ISIZ-1)) && sz>=16 )
#endif
 {
 int z=((int)s&(ISIZ-1));
 if(z)
  {
#ifndef AUTOINC
  s-=z; d-=z;
  switch(ISIZ-z)
   {
#if ISIZ==8
   case 7: d[1]=s[1];
   case 6: d[2]=s[2];
   case 5: d[3]=s[3];
   case 4: d[4]=s[4];
   case 3: d[5]=s[5];
   case 2: d[6]=s[6];
   case 1: d[7]=s[7];
   case 0:;
#else
#if ISIZ==4
   case 3: d[1]=s[1];
   case 2: d[2]=s[2];
   case 1: d[3]=s[3];
   case 0:;
#else
#if ISIZ==2
   case 1: d[1]=s[1];
   case 0:;
#endif
#endif
#endif
   }
  s+=ISIZ; d+=ISIZ;
#else
  switch(ISIZ-z)
   {
   case 7: *d++= *s++;
   case 6: *d++= *s++;
   case 5: *d++= *s++;
   case 4: *d++= *s++;
   case 3: *d++= *s++;
   case 2: *d++= *s++;
   case 1: *d++= *s++;
   case 0:;
   }
#endif
  sz-=ISIZ-z;
  }
 mfwrdI(d,s,sz>>SHFT);
#ifdef AUTOINC
 s+=(sz&~(ISIZ-1)); d+=(sz&~(ISIZ-1));
 switch(sz&(ISIZ-1))
  {
  case 7: *d++= *s++;
  case 6: *d++= *s++;
  case 5: *d++= *s++;
  case 4: *d++= *s++;
  case 3: *d++= *s++;
  case 2: *d++= *s++;
  case 1: *d++= *s++;
  case 0:;
  }
#else
 s+=sz-(ISIZ-1); d+=sz-(ISIZ-1);
 switch(sz&(ISIZ-1))
  {
#if ISIZ==8
 case 7: d[0]=s[0];
 case 6: d[1]=s[1];
 case 5: d[2]=s[2];
 case 4: d[3]=s[3];
 case 3: d[4]=s[4];
 case 2: d[5]=s[5];
 case 1: d[6]=s[6];
 case 0:;
#else
#if ISIZ==4
  case 3: d[0]=s[0];
  case 2: d[1]=s[1];
  case 1: d[2]=s[2];
  case 0:;
#else
#if ISIZ==2
  case 1: d[0]=s[0];
  case 0:;
#endif
#endif
#endif
  }
#endif
 }
else
 {
 while(sz>=16)
  {
#ifdef AUTOINC
 *d++= *s++; *d++= *s++; *d++= *s++; *d++= *s++;
 *d++= *s++; *d++= *s++; *d++= *s++; *d++= *s++;
 *d++= *s++; *d++= *s++; *d++= *s++; *d++= *s++;
 *d++= *s++; *d++= *s++; *d++= *s++; *d++= *s++;
#else
  d[0]=s[0]; d[1]=s[1]; d[2]=s[2]; d[3]=s[3]; d[4]=s[4]; d[5]=s[5]; d[6]=s[6];
  d[7]=s[7]; d[8]=s[8]; d[9]=s[9]; d[10]=s[10]; d[11]=s[11]; d[12]=s[12];
  d[13]=s[13]; d[14]=s[14]; d[15]=s[15];
  s+=16; d+=16;
#endif
  sz-=16;
  }
#ifndef AUTOINC
 s-=15-sz; d-=15-sz;
#endif
 switch(sz)
  {
#ifdef AUTOINC
 case 15: *d++= *s++;
 case 14: *d++= *s++;
 case 13: *d++= *s++;
 case 12: *d++= *s++;
 case 11: *d++= *s++;
 case 10: *d++= *s++;
 case 9: *d++= *s++;
 case 8: *d++= *s++;
 case 7: *d++= *s++;
 case 6: *d++= *s++;
 case 5: *d++= *s++;
 case 4: *d++= *s++;
 case 3: *d++= *s++;
 case 2: *d++= *s++;
 case 1: *d++= *s++;
 case 0:;
#else
  case 15: d[0]=s[0];
  case 14: d[1]=s[1];
  case 13: d[2]=s[2];
  case 12: d[3]=s[3];
  case 11: d[4]=s[4];
  case 10: d[5]=s[5];
  case 9: d[6]=s[6];
  case 8: d[7]=s[7];
  case 7: d[8]=s[8];
  case 6: d[9]=s[9];
  case 5: d[10]=s[10];
  case 4: d[11]=s[11];
  case 3: d[12]=s[12];
  case 2: d[13]=s[13];
  case 1: d[14]=s[14];
  case 0:;
#endif
  }
 }
return od;
}

/* Utility to count number of lines within a segment */

int mcnt(blk,c,size)
char *blk,c;
int size;
{
int nlines=0;
while(size>=16)
 {
 if(blk[0]==c) ++nlines;
 if(blk[1]==c) ++nlines;
 if(blk[2]==c) ++nlines;
 if(blk[3]==c) ++nlines;
 if(blk[4]==c) ++nlines;
 if(blk[5]==c) ++nlines;
 if(blk[6]==c) ++nlines;
 if(blk[7]==c) ++nlines;
 if(blk[8]==c) ++nlines;
 if(blk[9]==c) ++nlines;
 if(blk[10]==c) ++nlines;
 if(blk[11]==c) ++nlines;
 if(blk[12]==c) ++nlines;
 if(blk[13]==c) ++nlines;
 if(blk[14]==c) ++nlines;
 if(blk[15]==c) ++nlines;
 blk+=16; size-=16;
 }
while(size--) if(*blk++=='\n') ++nlines;
return nlines;
}
