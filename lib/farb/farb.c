/* Arbitrary precision math routines
   These routines take ASCII numbers in C strings and return
   ASCII string results in malloc blocks.
   Copyright (C) 1994 Joseph H. Allen

This is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 1, or (at your option) any later version.

This software is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
more details.

You should have received a copy of the GNU General Public License along with 
JOE; see the file COPYING.  If not, write to the Free Software Foundation, 
675 Mass Ave, Cambridge, MA 02139, USA.  */ 


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "farb.h"

/* A valid number string is zero terminated C string containing 1 or more ASCII
 * digits possibly preceded by a '-' sign.  Whitespace and negative zero is
 * not allowed.  Leading zeros _must_ be suppressed.
 */

static int max(int a,int b) { return a>=b?a:b; }
/* static int min(int a,int b) { return a<=b?a:b; } */

/* Unsigned Add.  If 'n' is true, precede the result string with '-' */

char *Uadd(char *a,char *b,int n)
 {
 int lena=strlen(a), lenb=strlen(b), lens;
 int c, x;
 char *s, *r;

 if(a[0]=='-') --lena, ++a;
 if(b[0]=='-') --lenb, ++b;

 s=(char *)malloc((lens=max(lena,lenb)+1)+1);
 s[lens]=0;

 if(lena>lenb)
  {
  for(x=0,c=0;x!=lenb;++x)
   if((s[lens-x-1]=a[lena-x-1]-'0'+b[lenb-x-1]-'0'+c)>=10)
    c=1, s[lens-x-1]+='0'-10;
   else
    c=0, s[lens-x-1]+='0';
  for(;x!=lena;++x)
   if((s[lens-x-1]=a[lena-x-1]-'0'+c)>=10) c=1, s[lens-x-1]+='0'-10;
   else c=0, s[lens-x-1]+='0';
  }
 else
  {
  for(x=0,c=0;x!=lena;++x)
   if((s[lens-x-1]=a[lena-x-1]-'0'+b[lenb-x-1]-'0'+c)>=10)
    c=1, s[lens-x-1]+='0'-10;
   else c=0, s[lens-x-1]+='0';
  for(;x!=lenb;++x)
   if((s[lens-x-1]=b[lenb-x-1]-'0'+c)>=10) c=1, s[lens-x-1]+='0'-10;
   else c=0, s[lens-x-1]+='0';
  }
 if(c) s[lens-++x]='1';

 if(n) r=(char *)malloc(x+2), r[0]='-', strcpy(r+1,s+lens-x);
 else r=(char *)malloc(x+1), strcpy(r,s+lens-x);

 free(s);
 return r;
 }

/* Unsigned Subtract.  If 'n' is true, precede the result string with '-'. The
 * magnitude of 'a' _must_ be larger than the magnitude of 'b'. */

char *Usub(char *a,char *b,int n)
 {
 int lena=strlen(a), lenb=strlen(b), lens;
 int c, x;
 char *s, *r;

 if(a[0]=='-') --lena, ++a;
 if(b[0]=='-') --lenb, ++b;

 s=(char *)malloc((lens=max(lena,lenb))+1);
 s[lens]=0;

 for(x=0,c=0;x!=lenb;++x)
  if((s[lens-x-1]=a[lena-x-1]-b[lenb-x-1]-c)<0) c=1, s[lens-x-1]+='0'+10;
  else c=0, s[lens-x-1]+='0';
 for(;x!=lena;++x)
  if((s[lens-x-1]=a[lena-x-1]-'0'-c)<0) c=1, s[lens-x-1]+='0'+10;
  else c=0, s[lens-x-1]+='0';
 while(x!=1 && s[lens-x]=='0') --x;

 if(n) r=(char *)malloc(x+2), r[0]='-', strcpy(r+1,s+lens-x);
 else r=(char *)malloc(x+1), strcpy(r,s+lens-x);

 free(s);
 return r;
 }

/* Magnitude comparison.
 * Returns 1 if |a|>|b|, 0 if |a|=|b|, or -1 if |a|<|b| */

static int ncmp(char *a,int lena,char *b,int lenb)
 {
 while(*a=='0' && lena!=1) ++a, --lena;
 while(*b=='0' && lenb!=1) ++b, --lenb;

 if(lena>lenb) return 1;
 else if(lena<lenb) return -1;

 do
  if(*a>*b) return 1;
  else if(*a<*b) return -1;
  while(++a, ++b, --lena);

 return 0;
 }

int Ucmp(char *a,char *b)
 {
 int lena=strlen(a), lenb=strlen(b);
 if(a[0]=='-') --lena, ++a;
 if(b[0]=='-') --lenb, ++b;
 return ncmp(a,lena,b,lenb);
 }

/* Compare.  Returns 1 if a>b, 0 if a=b, or -1 if a<b */

int Cmp(char *a,char *b)
 {
 if(a[0]!='-' && b[0]=='-') return 1;
 if(a[0]=='-' && b[0]!='-') return -1;
 if(a[0]=='-') return -Ucmp(a,b);
 else return Ucmp(a,b);
 }

/* Add */

char *Add(char *a,char *b)
 {
 if(a[0]!='-' && b[0]=='-')
  if(Ucmp(a,b)>=0) return Usub(a,b,0);
  else return Usub(b,a,1);
 else if(a[0]=='-' && b[0]!='-')
  if(Ucmp(a,b)<=0) return Usub(b,a,0);
  else return Usub(a,b,1);
 else if(a[0]!='-') return Uadd(a,b,0);
 else return Uadd(a,b,1);
 }

/* Subtract */

char *Sub(char *a,char *b)
 {
 if(a[0]!='-' && b[0]!='-')
  if(Ucmp(a,b)>=0) return Usub(a,b,0);
  else return Usub(b,a,1);
 else if(a[0]=='-' && b[0]=='-')
  if(Ucmp(a,b)<=0) return Usub(b,a,0);
  else return Usub(a,b,1);
 else if(a[0]!='-') return Uadd(a,b,0);
 else return Uadd(a,b,1);
 }

/* Multiply */

char *Mul(char *a,char *b)
 {
 int lena=strlen(a), lenb=strlen(b), lens;
 int n=0;
 int x, y, m, c;
 char *s, *r;

 if(a[0]=='-') n^=1, ++a, --lena;
 if(b[0]=='-') n^=1, ++b, --lenb;

 s=(char *)malloc((lens=lena+lenb)+1);
 s[lens]=0;
 memset(s,'0',lens);

 for(x=0;x!=lenb;++x)
  if((m = b[lenb-x-1]-'0') != 0)
   {
   for(y=0, c=0;y!=lena;++y)
    if((s[lens-x-y-1]+=c+(a[lena-y-1]-'0')*m-'0')>=10)
     c=s[lens-x-y-1]/10, s[lens-x-y-1]+='0'-c*10;
    else c=0, s[lens-x-y-1]+='0';
   for(;c;++y)
    if((s[lens-x-y-1]+=c-'0')>=10) c=1, s[lens-x-y-1]+='0'-10;
    else c=0, s[lens-x-y-1]+='0';
   }

 for(x=lens;x!=1 && s[lens-x]=='0';--x);

 if(x==1 && s[lens-x]=='0') n=0;

 if(n) r=(char *)malloc(x+2), r[0]='-', strcpy(r+1,s+lens-x);
 else r=(char *)malloc(x+1), strcpy(r,s+lens-x);

 free(s);
 return r;
 }

/* Negate */

char *Neg(char *a)
 {
 if(a[0]=='-') return strdup(a+1);
 else if(a[0]!='0' || a[1])
  {
  char *r=(char *)malloc(strlen(a)+2);
  r[0]='-';
  strcpy(r+1,a);
  return r;
  }
 else return strdup(a);
 }

/* Absolute value */

char *Abs(char *a)
 {
 if(a[0]=='-') return strdup(a+1);
 else return strdup(a);
 }

/* Subtract b/blen from a/alen as many times as possible.  Return the number of
 * times (0-9) which we subtracted
 */

static int into(char *a,int lena,char *b,int lenb)
 {
 int m;
 for(m=0;ncmp(a,lena,b,lenb)>=0;++m)
  {
  int x, c;
  for(x=0,c=0;x!=lenb;++x)
   if((a[lena-x-1]-=b[lenb-x-1]+c)<0)
    c=1, a[lena-x-1]+='0'+10;
   else c=0, a[lena-x-1]+='0';
  for(;c;++x)
   if((a[lena-x-1]-='0'+c)<0)
    c=1, a[lena-x-1]+='0'+10;
   else c=0, a[lena-x-1]+='0';
  }
 return m;
 }

/* Divide */

char *Div(char *a,char *b)
 {
 int lena=strlen(a), lenb=strlen(b), lenq;
 int n=0;
 int x;
 char *q, *r, *z;

 if(a[0]=='-') n^=1, ++a, --lena;
 if(b[0]=='-') n^=1, ++b, --lenb;

 r=strcpy((char *)malloc(lena+1),a);

 q=(char *)malloc((lenq=lena)+1);
 q[lenq]=0;

 for(x=0;x!=lena;++x) q[x]=into(r,x+1,b,lenb)+'0';

 for(x=lenq;x!=1 && q[lenq-x]=='0';--x);
 if(x==1 && q[lenq-x]=='0') n=0;
 if(n) z=(char *)malloc(x+2), z[0]='-', strcpy(z+1,q+lenq-x);
 else z=(char *)malloc(x+1), strcpy(z,q+lenq-x);

 free(r); free(q);
 return z;
 }

/* Modulous */

char *Mod(char *a,char *b)
 {
 int lena=strlen(a), lenb=strlen(b), lenq;
 int n=0;
 int x;
 char *q, *r, *z;

 if(a[0]=='-') n^=1, ++a, --lena;
 if(b[0]=='-') n^=1, ++b, --lenb;

 r=strcpy((char *)malloc(lena+1),a);

 q=(char *)malloc((lenq=lena)+1);
 q[lenq]=0;

 for(x=0;x!=lena;++x) q[x]=into(r,x+1,b,lenb)+'0';

 for(x=lena;x!=1 && r[lena-x]=='0';--x);
 if(x==1 && r[lena-x]=='0') n=0;
 if(n) z=(char *)malloc(x+2), z[0]='-', strcpy(z+1,r+lena-x);
 else z=(char *)malloc(x+1), strcpy(z,r+lena-x);

 free(r); free(q);
 return z;
 }

/* Convert arb to integer */

int Int(char *a)
 {
 int x=0;
 sscanf(a,"%d",&x);
 return x;
 }

/* Convert integer to arb */

char *Arb(int a)
 {
 char buf[64];
 sprintf(buf,"%d",a);
 return strdup(buf);
 }

/** Decimal and floating point routines **/

/* Convert floating point number to integer and exponant */

char *Extract(char *a,int *expa)
 {
 int ex, 	/* Offset to 'e' or 'E' */
     sgn,	/* Set to 1 if number is negative, otherwise set to zero */
     dot,	/* Offset to '.' */
     first,	/* Offset to first significant digit */
     last,	/* Offset to one past last significant digit */
     x;
 int exp=0;	/* Value of exponant */
 char *r;

 /* Calculate offsets */
 for(ex=0;a[ex] && a[ex]!='e' && a[ex]!='E';++ex);
 for(dot=0;dot!=ex && a[dot]!='.';++dot);
 if(a[0]=='-') sgn=1;
 else sgn=0;
 for(first=sgn;(a[first]=='.' || a[first]=='0') && first!=ex;++first);
 if(first!=ex)
  {
  for(last=ex-1;last!=first && (a[last]=='0' || a[last]=='.');--last);
  ++last;
  }
 else last=ex;

 /* Get exponant if there is one */
 if(a[ex])
  {
  for(x=((a[ex+1]=='-'||a[ex+1]=='+')?ex+2:ex+1);a[x];++x)
   exp=exp*10+a[x]-'0';
  if(a[ex+1]=='-') exp= -exp;
  }
 /* Adjust exponant for position of decimal point */
 if(dot<last) exp-=last-dot-1;
 else exp-=last-dot;

 /* Get mantissa */
 if(dot<last && dot>first)
  {
  r=(char *)malloc(1+sgn+last-first-1);
  if(dot-first) memcpy(r+sgn,a+first,dot-first);
  if(last-(dot+1)) memcpy(r+sgn+dot-first,a+dot+1,last-(dot+1));
  r[sgn+last-first-1]=0;
  }
 else
  {
  r=(char *)malloc(2+sgn+last-first);
  if(last-first) memcpy(r+sgn,a+first,last-first), r[sgn+last-first]=0;
  else r[sgn]='0', r[sgn+1]=0, exp=0;
  }

 if(sgn) r[0]='-';

 *expa=exp;
 return r;
 }

/* Normalize floating point number into engineering format (the exponant is
 * a multiple of three and there is one to three digits to the left of the
 * decimal point).
 */

char *Eng(char *s,int exp)
 {
 int sgn;
 int dot;
 int len;
 int x;
 int chng;
 char *r;
 char buf[32];

 if(s[0]=='-') sgn=1;
 else sgn=0;
 len=strlen(s);

 if(s[sgn]=='0') exp=0;

 /* Zap trailing zeros */
 for(x=len-1;x>sgn && s[x]=='0';--x) ++exp;
 len=x+1;

 /* Set dot position and set exponant value */
 chng=exp%3;
 exp-=chng;
 dot=len-sgn+chng;

 if(dot<=0) dot+=3, exp-=3;

 chng=(dot-1)/3;
 exp+=chng*3;
 dot-=chng*3;

 /* Generate number */
 sprintf(buf,"e%d",exp);
 r=(char *)malloc(strlen(buf)+strlen(s)+12);
 if(sgn && s[sgn]!='0') r[0]='-', x=1;
 else x=0;
 if(dot<0)
  {
  r[x++]='.';
  while(dot++<0) r[x++]='0';
  memcpy(r+x,s+sgn,len-sgn); x+=len-sgn;
  if(exp) strcpy(r+x,buf);
  else r[x]=0;
  }
 else if(dot<len-sgn)
  {
  if(dot) memcpy(r+x,s+sgn,dot); x+=dot;
  r[x]='.'; ++x;
  memcpy(r+x,s+sgn+dot,len-sgn-dot); x+=len-sgn-dot;
  if(exp) strcpy(r+x,buf);
  else r[x]=0;
  }
 else
  {
  int q;
  memcpy(r+x,s+sgn,len-sgn); x+=len-sgn;
  for(q=0;q!=dot-(len-sgn);++q) r[x++]='0';
  if(exp) strcpy(r+x,buf);
  else r[x]=0;
  }

 return r;
 }

/* Add n zeros to end of s */

char *Shift(char *s,int n)
 {
 int len=strlen(s), x;
 char *new=(char *)malloc(len+n+1);
 memcpy(new,s,len);
 for(x=0;x!=n;++x) new[len+x]='0';
 new[len+x]=0;
 return new;
 }

/* Floating point multiply */

char *Fmul(char *a,char *b)
 {
 char *am, *bm, *r;
 int ae, be, re;
 am=Extract(a,&ae);
 bm=Extract(b,&be);
 re=ae+be;
 r=Mul(am,bm);
 free(am); free(bm);
 return Eng(r,re);
 }

/* Floating point addition */

char *Fadd(char *a,char *b)
 {
 char *am, *bm, *r;
 int ae, be, re;
 am=Extract(a,&ae);
 bm=Extract(b,&be);
 if(ae>be)
  {
  char *n=Shift(am,ae-be);
  ae=be;
  free(am); am=n;
  }
 else if(be>ae)
  {
  char *n=Shift(bm,be-ae);
  be=ae;
  free(bm); bm=n;
  }
 re=ae;
 r=Add(am,bm);
 free(am); free(bm);
 return Eng(r,re);
 }

/* Floating point subtraction */

char *Fsub(char *a,char *b)
 {
 char *am, *bm, *r;
 int ae, be, re;
 am=Extract(a,&ae);
 bm=Extract(b,&be);
 if(ae>be)
  {
  char *n=Shift(am,ae-be);
  ae=be;
  free(am); am=n;
  }
 else if(be>ae)
  {
  char *n=Shift(bm,be-ae);
  be=ae;
  free(bm); bm=n;
  }
 re=ae;
 r=Sub(am,bm);
 free(am); free(bm);
 return Eng(r,re);
 }

/* Floating point division: to n places */

char *Fdiv(char *a,char *b,int n)
 {
 char *am, *bm, *r;
 int ae, be, re;
 int sa, sb;
 am=Extract(a,&ae);
 bm=Extract(b,&be);
 sa=strlen(am); if(am[0]=='-') --sa;
 sb=strlen(bm); if(bm[0]=='-') --sb;
 if(sa-sb<n)
  {
  am=Shift(am,n-(sa-sb));
  ae-=n-(sa-sb);
  }
 re=ae-be;
 r=Div(am,bm);
 free(am); free(bm);
 return Eng(r,re);
 }

/* Negate */

char *Fneg(char *a)
 {
 int exp;
 char *s=Extract(a,&exp), *t, *r;
 t=Neg(s);
 r=Eng(t,exp);
 free(s);
 free(t);
 return r;
 }

/* Absolute value */

char *Fabs(char *a)
 {
 if(a[0]=='-') return strdup(a+1);
 else return strdup(a);
 }

/* Magnitude comparison */

int Fucmp(char *a,char *b)
 {
 char *am, *bm;
 int ae, be;
 int la, lb;
 int r;
 am=Extract(a,&ae);
 bm=Extract(b,&be);
 la=strlen(am); if(am[0]=='-') --la;
 lb=strlen(bm); if(bm[0]=='-') --lb;

 if(la+ae>lb+be) r=1;
 else if(la+ae<lb+be) r= -1;
 else
  {
  if(ae>be)
   {
   char *n=Shift(am,ae-be);
   free(am); am=n;
   }
  else if(ae<be)
   {
   char *n=Shift(bm,be-ae);
   free(bm); bm=n;
   }
  r=Ucmp(am,bm);
  }

 free(am); free(bm);
 return r;
 }

/* Comparison */

int Fcmp(char *a,char *b)
 {
 if(a[0]=='-' && b[0]!='-') return -1;
 else if(a[0]!='-' && b[0]=='-') return 1;
 else if(a[0]!='-') return Fucmp(a,b);
 else return -Fucmp(a,b);
 }

/* Truncate everything after the decimal point */

char *Ftrunc(char *a)
 {
 int ae;
 int len;
 int sgn;
 char *am=Extract(a,&ae);
 char *r;
 if(am[0]=='-') sgn=1; else sgn=0;
 len=strlen(am);
 if(ae<0)
  {
  if(-ae>=len-sgn) am[sgn]='0', am[sgn+1]=0;
  else am[len+ae]=0;
  ae=0;
  }
 r=Eng(am,ae);
 free(am);
 return r;
 }

/* Convert integer part of floating point number to an integer string with
 * no exponant */

char *FtoArb(char *a)
 {
 int ae;
 int len;
 int sgn;
 char *am=Extract(a,&ae);
 if(am[0]=='-') sgn=1; else sgn=0;
 len=strlen(am);
 if(ae<0)
  {
  if(-ae>=len-sgn) free(am), am=strdup("0"), sgn=0;
  else am[len+ae]=0;
  ae=0;
  }
 else if(ae>0)
  {
  char *n=Shift(am,ae);
  free(am);
  am=n;
  }
 return am;
 }

/* Convert to double */

double Fdouble(char *a)
 {
 double n=0.0;
 sscanf(a,"%lg",&n);
 return n;
 }

/* Convert from double */

char *Farb(double n)
 {
 char buf[64];
 sprintf(buf,"%lg",n);
 return strdup(buf);
 }

/* Generate a money number from an arbitrary precision integer and an exponant
 */

char *money(char *s,int exp)
 {
 char *r;
 int sgn;
 int len;
 int dot;
 int x;

 if(s[0]=='-') sgn=1;
 else sgn=0;

 len=strlen(s);

 if(s[sgn]=='0') exp=0;

 /* Zap trailing zeros */
 for(x=len-1;x>sgn && s[x]=='0';--x) ++exp;
 len=x+1;

 /* Set dot position */
 dot=len-sgn+exp;

 /* Generate number */
 r=(char *)malloc(5+max(len-sgn,dot));
 if(sgn && s[sgn]!='0') r[0]='-', x=1;
 else x=0;

 if(dot<0)
  {
  r[x++]='0';
  r[x++]='.';
  while(dot++<0) r[x++]='0';
  memcpy(r+x,s+sgn,len-sgn); x+=len-sgn;
  if(len-sgn==1) r[x++]='0';
  r[x]=0;
  }
 else if(dot<len-sgn)
  {
  if(dot) memcpy(r+x,s+sgn,dot), x+=dot;
  else r[x++]='0';
  r[x++]='.';
  memcpy(r+x,s+sgn+dot,len-sgn-dot); x+=len-sgn-dot;
  if(len-sgn-dot==1) r[x++]='0';
  r[x]=0;
  }
 else
  {
  int q;
  memcpy(r+x,s+sgn,len-sgn); x+=len-sgn;
  for(q=0;q!=dot-(len-sgn);++q) r[x++]='0';
  r[x++]='.';
  r[x++]='0';
  r[x++]='0';
  r[x]=0;
  }
 return r;
 }

/* Money multiply */

char *Mmul(char *a,char *b)
 {
 char *am, *bm, *r;
 int ae, be, re;
 am=Extract(a,&ae);
 bm=Extract(b,&be);
 re=ae+be;
 r=Mul(am,bm);
 free(am); free(bm);
 return money(r,re);
 }

/* Money addition */

char *Madd(char *a,char *b)
 {
 char *am, *bm, *r;
 int ae, be, re;
 am=Extract(a,&ae);
 bm=Extract(b,&be);
 if(ae>be)
  {
  char *n=Shift(am,ae-be);
  ae=be;
  free(am); am=n;
  }
 else if(be>ae)
  {
  char *n=Shift(bm,be-ae);
  be=ae;
  free(bm); bm=n;
  }
 re=ae;
 r=Add(am,bm);
 free(am); free(bm);
 return money(r,re);
 }

/* Floating point subtraction */

char *Msub(char *a,char *b)
 {
 char *am, *bm, *r;
 int ae, be, re;
 am=Extract(a,&ae);
 bm=Extract(b,&be);
 if(ae>be)
  {
  char *n=Shift(am,ae-be);
  ae=be;
  free(am); am=n;
  }
 else if(be>ae)
  {
  char *n=Shift(bm,be-ae);
  be=ae;
  free(bm); bm=n;
  }
 re=ae;
 r=Sub(am,bm);
 free(am); free(bm);
 return money(r,re);
 }

/* Money division */

char *Mdiv(char *a,char *b,int n)
 {
 char *am, *bm, *r;
 int ae, be, re;
 int sa, sb;
 am=Extract(a,&ae);
 bm=Extract(b,&be);
 sa=strlen(am); if(am[0]=='-') --sa;
 sb=strlen(bm); if(bm[0]=='-') --sb;
 if(sa-sb<n)
  {
  am=Shift(am,n-(sa-sb));
  ae-=n-(sa-sb);
  }
 re=ae-be;
 r=Div(am,bm);
 free(am); free(bm);
 return money(r,re);
 }

/* Negate */

char *Mneg(char *a)
 {
 int exp;
 char *s=Extract(a,&exp), *t, *r;
 t=Neg(s);
 r=money(t,exp);
 free(s);
 free(t);
 return r;
 }

/* Format money strings nicely */

char *Mfmt(char *s,int flg)
 {
 char *r;
 int sgn;
 int dot;
 int len;
 int x, q;
 len=strlen(s);
 flg= !!flg;
 if(s[0]=='-') sgn=1;
 else sgn=0;
 for(dot=sgn;s[dot]!='.';++dot);
 r=(char *)malloc(flg+len+(dot-sgn-1)/3+flg);
 if(flg) r[0]='$', x=1;
 else x=0;
 if(sgn) r[x++]='-';
 x+=dot-sgn+(dot-sgn-1)/3;
 strcpy(r+x,s+dot);
 for(q=0;q!=dot-sgn;++q)
  {
  if(q%3==0 && q) r[--x]=',';
  r[--x]=s[dot-q-1];
  }
 return r;
 }

#if 0
int main(int arbc,char *argv[])
 {
 int n;
 sscanf(argv[3],"%d",&n);
 printf("%s\n",Fdiv(argv[1],argv[2],n));
 /* printf("%s\n",Mfmt(Madd(argv[1],argv[2]),1)); */
 return 0;
 }
#endif
