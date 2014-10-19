#include "regex.h"

/* Simple (slow) regular expression parser.  Returns true if 'string'
 * matches 'pattern'.  Each substring which matches a spanning character ('+'
 * and '*') is placed in 'result' (which should be an array of string
 * buffers, each large enough to contain the longest expected substring).
 *
 * 'pattern' may be:
 *    .      matches any character.
 *    *      matches zero or more of the previous character.
 *    +      matches one or more of the previous character.
 *    [...]  matches one character in the list xx.
 *           ranges may be specified with the list, such as 0-9, a-z, etc.
 *    x      other characters match themselves only.
 */

static int rmatch(unsigned char *string,unsigned char *pattern,unsigned char **result)
  {
  int q;
  char sense;
  char test[256];
  char buf[1024];
  *result=0;
  for(;;)
    {
    char *laststring;
    /* Build character match array */
    for(q=0;q!=256;++q)	test[q]=0;	/* Clear array */
    sense=1;			/* Sense of match */
    if(!*pattern)		/* End of pattern */
      if(!*string) return 1;	/* Must match end of string */
      else return 0;
    else if(*pattern=='.')	/* Match any character */
      {
      ++pattern;
      for(q=0;q!=256;++q) test[q]=1;
      }
    else if(*pattern=='[')	/* Match characters within brackets */
      {
      ++pattern;
      if(*pattern=='^')		/* Inverse sense of match */
        ++pattern, sense=0;
      if(*pattern==']')		/* Match ] */
        ++pattern, test[']']=1;
      while(*pattern && *pattern!=']')
        {
        if(*pattern=='-' && pattern[-1]!='[' && pattern[1] &&
           pattern[1]!=']' && pattern[1]>=pattern[-1])
          { /* A Range of characters */
          for(q=pattern[-1];q<=pattern[1];++q)
            test[q]=1;
          ++pattern;
          }
        else test[*pattern]=1;
        ++pattern;
        }
      if(*pattern) ++pattern;
      }
    else			/* Match character itself */
      test[*pattern++]=1;

    /* Check for spanning characters... */
    if(*pattern=='*' || *pattern=='+')
      { /* Span... */
      char buf[1024];
      int x=0;
      int lastx= -1;
      if(*pattern++=='+')
        {
        if(test[*string]!=sense) return 0;
        buf[x++]=*string++;
        }
      laststring=0;
      for(;;)
        {
        if(rmatch(string,pattern,result+1))
          { /* A possible match... try to find the longest one */
          lastx=x;
          laststring=string;
          }
        if(!*string || test[*string]!=sense) break;
        buf[x++]= *string++;
        }
      if(lastx>=0)
        {
        buf[lastx]=0;
        *result=strdup(buf);
        return rmatch(laststring,pattern,result+1);
        }
      else return 0;
      }
    else
      { /* Just one character */
      if(test[*string++]!=sense) return 0;
      }
    }
  }

// Pattern substitute match

void subst(char *out,char *pat,char *sub,char *in)
  {
  char buf[10*256];
  char *res[10];
  int x;
  for(x=0;x!=10;++x)
    {
    res[x]=buf+256*x;
    buf[x*256]=0;
    }
  if(pat && sub && rmatch(in,pat,&res))
    {
    while(*sub)
      if(*sub=='&' && (sub[1]>='0' && sub[1]<='9' || sub[1]=='_'))
        {
        char *p;
        if(sub[1]=='_') p=in;
        else p=res[sub[1]-'0'];
        sub+=2;
        while(*p)
          *out++ = *p++;
        }
      else
        *out++ = *sub++;
    *out = 0;
    }
  else
    strcpy(out,in);
  }
