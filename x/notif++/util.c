// Legacy utilities

#include <string.h>
#include <stdio.h>
#include "util.h"

void fields(char *s,char **fld)
  {
  int y=1;
  fld[0]=s;
  while(*s && *s!='\t') ++s;
  while(*s)
    {
    *s++=0;
    fld[y++]=s;
    while(*s && *s!='\t') ++s;
    }
  }

int nfields(char *s)
  {
  int y=1;
  while(*s)
    {
    if(*s=='\t') ++y;
    ++s;
    }
  return y;
  }

void mset(char *dest,char val,int count)
  {
  for(int q=0;q!=count;++q) dest[q]=val;
  }

void msetI(int *dest,int val,int count)
  {
  for(int q=0;q!=count;++q) dest[q]=val;
  }

void mcpy(void *dest,void *src,int size)
  {
  memmove(dest,src,size);
  }

int Iabs(int x)
  {
  return x>=0 ? x : -x;
  }

long Labs(long x)
  {
  return x>=0 ? x : -x;
  }

int mcnt(const char *buf,char ch,int len)
  {
  int x, count=0;
  for(x=0;x!=len;++x) if(buf[x]==ch) ++count;
  return count;
  }

int toup(int c)
  {
  if(c>='a' && c<='z') return c+'A'-'a';
  else return c;
  }

bool crest(int c)
  {
  if(c>='A' && c<='Z' ||
     c>='a' && c<='z' ||
     c>='0' && c<='9' ||
     c=='_')
    return 1;
  else
    return 0;
  }

bool cwhite(int c)
  {
  if(c==' ' || c=='\t' || c=='\n') return 1;
  else return 0;
  }

/*
int val(string s)
  {
  int x,y,flg;
  if(s.size() && s[0]=='-')
    {
    flg=1;
    x=1;
    }
  else
    {
    flg=0;
    x=0;
    }
  for(y=0;x!=s.size() && s[x]>='0' && s[x]<='9';++x)
    y=y*10+s[x]-'0';
  return flg ? -y : y;
  }
*/

char *ntos(long n)
  {
  static char buf[12];
  sprintf(buf,"%d",n);
  return buf;
  }

long ston(char *s)
  {
  long r=0;
  while(*s>='0' && *s<='9') r=r*10+*s++-'0';
  return r;
  }
