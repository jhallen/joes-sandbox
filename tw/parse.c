// String parsing utilities

#include "parse.h"
#include "ctype.h"

// Test for keyword (case insensitive).  Skip over it if it's found and return true
// otherwise return false.

String parse_keyword(String s,const char *keyword)
  {
  int x;
  for(x=0;x!=s.size() && keyword[x] && toupper(keyword[x])==toupper(s[x]);++x);
  if(!keyword[x] && (x==s.size() || s[x]==' ' || s[x]=='\t'))
    return parse_ws(s+x);
  else
    return String();
  }

// Skip a prefix string

String parse_prefix(String s,const char *prefix)
  {
  int x;
  for(x=0;x!=s.size() && prefix[x] && toupper(prefix[x])==toupper(s[x]);++x);
  if(!prefix[x])
    return s+x;
  else
    return String();
  }

// Skip over whitespace

String parse_ws(String s)
  {
  if(s)
    {
    int x;
    for(x=0;x!=s.size() && (s[x]==' ' || s[x]=='\t');++x);
    return s+x;
    }
  return String();
  }

// Parse a single word

String parse_word(String s,String& rtn)
  {
  if(s)
    {
    int x;
    for(x=0;x!=s.size() && s[x]!=' ' && s[x]!='\t';++x);
    if(x)
      {
      rtn=s-x;
      return s+x;
      }
    }
  return String();
  }

// Parse list of words

list<String> parse_words(String s)
  {
  list<String> lst;
  String word;

  while(s=parse_word(s,word))
    {
    s=parse_ws(s);
    lst.push_back(word);
    }

  return lst;
  }
