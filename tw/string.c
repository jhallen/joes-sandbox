// Joe's String library.  This includes two basic enhancements over the standard
// string library:
//
//    The substr operation is copy-on-write just as a full string assignment is.
//
//    A String can be tested for assignment.  If a string!=0, then it has been
//    assigned to or has been initialized.  If a string==0, then it has never
//    been assigned to.  An unassigned string otherwise looks just like an
//    empty string.
//
//    s+n is the same as s.substr(n,s.size()-n)
//    s-n is the same as s.substr(0,n)

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "string.h"

// Insert data

String& String::insert(int pos,const char *new_s,int new_len)
  {
  if(raw)
    {
    if(new_len)
      {
      if(raw->ref==1 && s==(char *)(raw+1) && s+len==(char *)(raw+1)+raw->len)
        { // Insert in place
        raw=(Stringraw *)realloc(raw,sizeof(Stringraw)+len+new_len+1);
        s=(char *)(raw+1);
        if(len-pos) memmove(s+pos+new_len,s+pos,len-pos);
        memcpy(s+pos,new_s,new_len);
        len+=new_len;
        raw->len+=new_len;
        s[len]=0;
        }
      else
        { // Copy on write insert
        Stringraw *org=raw;
        char *org_s=s;
        s=allocate_raw(raw,len+new_len);
        if(pos) memcpy(s,org_s,pos);
        memcpy(s+pos,new_s,new_len);
        if(len-pos) memcpy(s+pos+new_len,org_s+pos,len-pos);
        len+=new_len;
        dereference(org);
        }
      }
    }
  else
    { // String was empty
    s=allocate_raw(raw,new_len);
    len=new_len;
    if(new_len) memcpy(s,new_s,new_len);
    }
  return *this;
  }

// Delete data

String& String::erase(int pos,int n)
  {
  if(raw && n)
    {
    if(raw->ref==1 && s==(char *)(raw+1) && s+len==(char *)(raw+1)+raw->len)
      { // Delete in place
      if(len-(pos+n)) memmove(s+pos,s+pos+n,len-(pos+n));
      raw=(Stringraw *)realloc(raw,sizeof(Stringraw)+len-n+1);
      s=(char *)(raw+1);
      len-=n;
      s[len]=0;
      }
    else
      { // Copy on write delete
      Stringraw *org=raw;
      char *org_s=s;
      s=allocate_raw(raw,len-n);
      if(pos) memcpy(s,org_s,pos);
      if(len-(pos+n)) memcpy(s+pos,org_s+pos+n,len-(pos+n));
      len-=n;
      dereference(org);
      }
    }
  return *this;
  }

// Compare

int String::cmp(const char *a,int a_len,const char *b,int b_len) const
  {
  if(a_len)
    if(b_len)
      if(a_len>b_len)
        {
        int rtn=memcmp(a,b,b_len);
        if(!rtn) return 1;
        else return rtn;
        }
      else if(a_len<b_len)
        {
        int rtn=memcmp(a,b,a_len);
        if(!rtn) return -1;
        else return rtn;
        }
      else
        return memcmp(a,b,a_len);
    else
      return 1;
  else
    if(b_len)
      return -1;
    else
      return 0;
  }

// Find

int String::find(char c,int start) const
  {
  if(start!=npos)
    {
    while(start!=len)
      if(s[start]==c) return start;
      else ++start;
    }
  return npos;
  }

int String::find_first_of(String sep,int start) const
  {
  char ary[256];
  int x;
  if(len && start!=npos)
    {
    for(x=0;x!=256;++x)
      ary[x]=0;
    for(x=0;x!=sep.size();++x)
      ary[(unsigned char)sep[x]]=1;
    while(start!=len)
      if(ary[(unsigned char)s[start]]) return start;
      else ++start;
    }
  return npos;
  }

int String::find_first_not_of(String sep,int start) const
  {
  char ary[256];
  int x;
  if(len && start!=npos)
    {
    for(x=0;x!=256;++x)
      ary[x]=1;
    for(x=0;x!=sep.size();++x)
      ary[(unsigned char)sep[x]]=0;
    while(start!=len)
      if(ary[(unsigned char)s[start]]) return start;
      else ++start;
    }
  return npos;
  }

// Convert to integer

int val(String s)
  {
  int accu=0;
  if(s[0]=='-')
    {
    for(int x=1;x!=s.size();++x)
      if(s[x]>='0' && s[x]<='9')
        accu=accu*10+s[x]-'0';
      else
        break;
    return -accu;
    }
  else
    {
    for(int x=0;x!=s.size();++x)
      if(s[x]>='0' && s[x]<='9')
        accu=accu*10+s[x]-'0';
      else
        break;
    return accu;
    }
  }

// Convert number to string

String numstr(int n)
  {
  char ary[20];
  sprintf(ary,"%d",n);
  return String(ary);
  }

// I/O

ostream& operator<< (ostream& out, const String& s)
  {
  for(int x=0;x!=s.size();++x) out << s[x];
  return out;
  }

// Get line of input into a string
// Return true if we got a line, or false if we're at eof.

bool getline(istream& in,String& s)
  {
  char buf[1024];
  int x;
  char c;

  s=String(); // Should truncate not replace here.
  x=0;

  if(!in.get(c))
    return 0;

  do
    if(c=='\n') break;
    else if(c=='\r') ;
    else
      {
      buf[x++]=c;
      if(x==1024)
        {
        s.append(buf,1024);
        x=0;
        }
      }
    while(in.get(c));

  if(x)
    s.append(buf,x);

  return 1;
  }
