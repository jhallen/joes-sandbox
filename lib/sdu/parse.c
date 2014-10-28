/* Parsers/Printers
   Copyright (C) 2005 Joseph H. Allen

This file is part of SDU (Structured Data Utilities)

SDU is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation; either version 1, or (at your option) any later version.  

SDU is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.  

You should have received a copy of the GNU General Public License along with 
SDU; see the file COPYING.  If not, write to the Free Software Foundation, 
675 Mass Ave, Cambridge, MA 02139, USA.  */ 

/* Todo:
      List of primitive types.

      More primitive types (double, enum?)

      Better addressing for correct member length handling.
      (difficult to fix this)

      Fixed length arrays.

      Sub-structure (real sub-structure or pointer?)

      Pointer to structure (how? on print only one copy should exist, everyone else uses pointer)

      (bkwd pointer in struct base?)

      Strings should use length or zero terminate?
      Binary (base64) data?
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "meta.h"

/* Read next character or tag */

/* EOF is -1 */
#define TAG -2
#define END_TAG -3

static int line=1;		/* Current line number */
static char tok_str[1024];	/* String associated with token */
static int tok_unget= -1;	/* Ungotten token */

static void xml_unget(int c)
  {
  tok_unget=c;
  }

static int xml_tok(FILE *f)
  {
  int c;
  if(tok_unget!=-1)
    {
    c=tok_unget;
    tok_unget= -1;
    return c;
    }
  again: c=getc(f);
  switch(c)
    {
    case '<':
      {
      int t;
      int x;
      c=getc(f);
      if(c=='/')
        t=END_TAG;
      else
        {
        t=TAG;
        ungetc(c,f);
        }
      for(x=0;x!=1023;++x)
        {
        c=getc(f);
        if(c=='\n')
          ++line;
        if(c!='>' && c!=EOF)
          tok_str[x]=c;
        else
          break;
        }
      while(c!='>' && c!=EOF)
        {
        c=getc(f);
        if(c=='\n')
          ++line;
        }
      tok_str[x]=0;
      return t;
      }

    case '&':
      {
      int hash;

      /* Read to next ; or EOF.  Save first MAX_ENTITY_LEN chars. */
      hash=0;

      c=getc(f);
      if(c==EOF || c==';') goto done;
      hash = c;

      c=getc(f);
      if(c==EOF || c==';') goto done;
      hash = (hash<<8) + c;

      c=getc(f);
      if(c==EOF || c==';') goto done;
      hash = (hash<<8) + c;

      c=getc(f);
      if(c==EOF || c==';') goto done;
      hash = (hash<<8) + c;

      c=getc(f);
      if(c==EOF || c==';') goto done;
      hash = 0;

      while(c!=';' && c!=EOF)
        c=getc(f);

      done:

      switch(hash)
        {
        case ('l'<<8)+'t': return '<';
        case ('g'<<8)+'t': return '>';
        case ('q'<<24)+('u'<<16)+('o'<<8)+'t': return '"';
        case ('a'<<24)+('p'<<16)+('o'<<8)+'s': return '\'';
        case ('a'<<16)+('m'<<8)+'p': return '&';
        }

      fprintf(stderr,"%d: skipping bad entity\n",line);
      goto again;
      }

    case '\n':
      {
      ++line;
      return c;
      }

    default:
      return c;
    }
  }

/* Get next non-whitepace/non-comment character or tag */

static int xml_skip(FILE *f)
  {
  int c;
  do
    c=xml_tok(f);
    while(c==' ' || c==10 || c==13 || c==9 || (c==TAG && (tok_str[0]=='!' || tok_str[0]=='?')));
  return c;
  }

/* Parse a structure */

struct base *xml_parse(FILE *f,char *name,struct meta *expect,int require)
  {
  /* Looking for whitespace <name> ..... </name> */
  int c=xml_skip(f);
  if(c==TAG && !strcmp(tok_str,name))
    {
    struct base *b=mkraw(expect);
    struct meta *m;
    void **t=(void **)((char *)b+sizeof(struct base));
    for(m=expect+1;m->code;++m)
      switch(m->code)
        {
        case tLIST:
          {
          c=xml_skip(f);
          if(c==TAG && !strcmp(tok_str,m->str))
            {
            struct base *first, *last;
            struct meta *n=metafind((m+1)->str);
            if(!n)
              {
              fprintf(stderr,"Error in grammar '%s' not found\n",(m+1)->str);
              exit(-1);
              }
            first=last=xml_parse(f,n->str,n,0);
            while(last)
              last=last->next=xml_parse(f,n->str,n,0);
            *t++ = first;
            c=xml_skip(f);
            if(c!=END_TAG || strcmp(tok_str,m->str))
              {
              fprintf(stderr,"%d: Missing end tag '%s'\n",line,m->str);
              xml_unget(c);
              }
            ++m;
            }
          else
            {
            fprintf(stderr,"%d: missing list '%s'\n",line,m->str);
            if (c == TAG) fprintf(stderr,"%d:  ... we found '%s'\n",line,tok_str);
            return 0;
            }
          break;
          }

        case tSTRUCT:
          { /* Structure reference (rethink this)... */
          struct meta *n=metafind((m+1)->str);
          if (!n)
            {
            fprintf(stderr,"Error in grammar '%s' not found\n",m->str);
            exit(-1);
            }
          if ((*t++ = xml_parse(f,m->str,n,1)) == 0)
            return 0;
          ++m;
          break;
          }

        case tSTRING:
          {
          int siz;
          int len;
          char *s;
          c=xml_skip(f);
          if(c==TAG && !strcmp(tok_str,m->str))
            {
            s=malloc(siz=1024);
            do
              for(len=0;(c=xml_tok(f)),c!=EOF && c!=TAG && c!=END_TAG;)
                {
                if(len==siz) s=realloc(s,siz*=2);
                s[len++]=c;
                }
              while(c==TAG && (tok_str[0]=='!'));
            if(len==siz) s=realloc(s,siz+=1);
            s[len]=0;
            *t++ = s;
            if(c!=END_TAG || strcmp(tok_str,m->str))
              {
              fprintf(stderr,"%d: missing end tag '%s'\n",line,m->str);
              if(c!=EOF) xml_unget(c);
              }
            }
          else
            {
            fprintf(stderr,"%d: missing string '%s'\n",line,m->str);
            if (c == TAG) fprintf(stderr,"%d:  ... we found '%s'\n",line,tok_str);
            return 0;
            }
          break;
          }

        case tINTEGER:
          {
          int neg=0;
          int val=0;
          c=xml_skip(f);
          if(c==TAG && !strcmp(tok_str,m->str))
            {
            c=xml_skip(f);
            if(c=='-') neg=1;
            else xml_unget(c);
            while((c=xml_skip(f)), c>='0' && c<='9')
              val=val*10+c-'0';
            if(neg) *t++ = (void *)-val;
            else *t++ = (void *)val;
            if(c!=END_TAG || strcmp(tok_str,m->str))
              {
              fprintf(stderr,"%d: missing end tag '%s'\n",line,m->str);
              if(c!=EOF) xml_unget(c);
              }
            }
          else
            {
            fprintf(stderr,"%d: missing integer '%s'\n",line,m->str);
            if (c == TAG) fprintf(stderr,"%d:  ... we found '%s'\n",line,tok_str);
            return 0;
            }
          break;
          }
        }
    c=xml_skip(f);
    if(c!=END_TAG || strcmp(tok_str,name))
      {
      fprintf(stderr,"%d: Missing end tag '%s'\n",line,m->str);
      xml_unget(c);
      }
    return b;
    }
  else
    {
    if (require) {
      fprintf(stderr,"%d: Missing struct '%s'\n",line,name);
      if (c == TAG) fprintf(stderr,"%d:  ... we found '%s'\n",line,tok_str);
    }
    xml_unget(c);
    return 0;
    }
  }

/* Print XML structure */

void xml_print(FILE *f,char *name,int i,struct base *b)
  {
  struct meta *m=b->_meta;
  struct meta *q;
  int x;
  void **t=(void **)((char *)b+sizeof(struct base));
  for(x=0;x!=i;++x) fputc(' ',f);
  fprintf(f,"<%s>\n",name);
  for(q=m+1;q->code;++q)
    switch(q->code)
      {
      case tLIST:
        {
        struct base *bb;
        for(x=0;x!=i+2;++x) fputc(' ',f);
        fprintf(f,"<%s>\n",q->str);
        for(bb=(struct base *)*t++;bb;bb=bb->next) xml_print(f,bb->_meta->str,i+4,bb);
        for(x=0;x!=i+2;++x) fputc(' ',f);
        fprintf(f,"</%s>\n",q->str);
        ++q;
        break;
        }
      case tSTRUCT:
        {
        xml_print(f,q->str,i+2,(struct base *)*t++);
        ++q;
        break;
        }
      case tSTRING:
        {
        char *s;
        for(x=0;x!=i+2;++x) fputc(' ',f);
        fprintf(f,"<%s>",q->str);
        s= *t++;
        while(*s)
          {
          switch(*s)
            {
            case '<': fprintf(f,"&lt;"); break;
            case '>': fprintf(f,"&gt;"); break;
            case '&': fprintf(f,"&apos;"); break;
            default: fputc(*s,f);
            }
          ++s;
          }
        fprintf(f,"</%s>\n",q->str);
        break;
        }
      case tINTEGER:
        {
        for(x=0;x!=i+2;++x) fputc(' ',f);
        fprintf(f,"<%s>%d</%s>\n",q->str,(int)*t++,q->str);
        break;
        }
      }
  for(x=0;x!=i;++x) fputc(' ',f);
  fprintf(f,"</%s>\n",name);
  }

/* Print LISP structure */

void lisp_print(FILE *f,char *name,int i,struct base *b)
  {
  struct meta *m=b->_meta;
  struct meta *q;
  int x;
  void **t=(void **)((char *)b+sizeof(struct base));
  for(x=0;x!=i;++x) fputc(' ',f);
  fprintf(f,"(%s\n",name); // Instance name
  // fprintf(f,"(%s\n",m->str); // Type name
  for(q=m+1;q->code;++q)
    switch(q->code)
      {
      case tLIST:
        {
        struct base *bb;
        for(x=0;x!=i+2;++x) fputc(' ',f);
        fprintf(f,"(%s\n",q->str);
        for(bb=(struct base *)*t++;bb;bb=bb->next) lisp_print(f,bb->_meta->str,i+4,bb);
        for(x=0;x!=i+2;++x) fputc(' ',f);
        fprintf(f,")\n");
        ++q;
        break;
        }
      case tSTRUCT:
        {
        lisp_print(f,q->str,i+2,(struct base *)*t++);
        ++q;
        break;
        }
      case tSTRING:
        {
        char *s;
        for(x=0;x!=i+2;++x) fputc(' ',f);
        fprintf(f,"(%s \"",q->str);
        s= *t++;
        while(*s)
          {
          switch(*s)
            {
            case '"': fprintf(f,"\""); break;
            default: fputc(*s,f);
            }
          ++s;
          }
        fprintf(f,"\")\n");
        break;
        }
      case tINTEGER:
        {
        for(x=0;x!=i+2;++x) fputc(' ',f);
        fprintf(f,"(%s %d)\n",q->str,(int)*t++);
        break;
        }
      }
  for(x=0;x!=i;++x) fputc(' ',f);
  fprintf(f,")\n");
  }

void lisp_print_untagged(FILE *f,char *name,int i,struct base *b)
  {
  struct meta *m=b->_meta;
  struct meta *q;
  int x;
  void **t=(void **)((char *)b+sizeof(struct base));
  for(x=0;x!=i;++x) fputc(' ',f);
  // fprintf(f,"(%s\n",name); // Instance name
  fprintf(f,"(%s\n",m->str); // Type name
  for(q=m+1;q->code;++q)
    switch(q->code)
      {
      case tLIST:
        {
        struct base *bb;
        for(x=0;x!=i+2;++x) fputc(' ',f);
        // fprintf(f,"(%s\n",q->str);
        fprintf(f,"(LIST\n");
        for(bb=(struct base *)*t++;bb;bb=bb->next) lisp_print_untagged(f,bb->_meta->str,i+4,bb);
        for(x=0;x!=i+2;++x) fputc(' ',f);
        fprintf(f,")\n");
        ++q;
        break;
        }
      case tSTRUCT:
        {
        lisp_print_untagged(f,q->str,i+2,(struct base *)*t++);
        ++q;
        break;
        }
      case tSTRING:
        {
        char *s;
        for(x=0;x!=i+2;++x) fputc(' ',f);
        fprintf(f,"\"");
        s= *t++;
        while(*s)
          {
          switch(*s)
            {
            case '"': fprintf(f,"\""); break;
            default: fputc(*s,f);
            }
          ++s;
          }
        fprintf(f,"\"\n");
        break;
        }
      case tINTEGER:
        {
        for(x=0;x!=i+2;++x) fputc(' ',f);
        fprintf(f,"%d\n",(int)*t++);
        break;
        }
      }
  for(x=0;x!=i;++x) fputc(' ',f);
  fprintf(f,")\n");
  }

/* Print indent structure */

void indent_print(FILE *f,char *name,int i,struct base *b)
  {
  struct meta *m=b->_meta;
  struct meta *q;
  int x;
  void **t=(void **)((char *)b+sizeof(struct base));
  for(x=0;x!=i;++x) fputc(' ',f);
  fprintf(f,"%s\n",name);
  for(q=m+1;q->code;++q)
    switch(q->code)
      {
      case tLIST:
        {
        struct base *bb;
        for(x=0;x!=i+2;++x) fputc(' ',f);
        fprintf(f,"%s\n",q->str);
        for(bb=(struct base *)*t++;bb;bb=bb->next) indent_print(f,bb->_meta->str,i+4,bb);
        ++q;
        break;
        }
      case tSTRUCT:
        {
        indent_print(f,q->str,i+2,(struct base *)*t++);
        ++q;
        break;
        }
      case tSTRING:
        {
        char *s;
        for(x=0;x!=i+2;++x) fputc(' ',f);
        fprintf(f,"%s \"",q->str);
        s= *t++;
        while(*s)
          {
          switch(*s)
            {
            case '"': fprintf(f,"\""); break;
            default: fputc(*s,f);
            }
          ++s;
          }
        fprintf(f,"\"\n");
        break;
        }
      case tINTEGER:
        {
        for(x=0;x!=i+2;++x) fputc(' ',f);
        fprintf(f,"%s %d\n",q->str,(int)*t++);
        break;
        }
      }
  }

void indent_print_untagged(FILE *f,char *name,int i,struct base *b)
  {
  struct meta *m=b->_meta;
  struct meta *q;
  int x;
  void **t=(void **)((char *)b+sizeof(struct base));
  for(x=0;x!=i;++x) fputc(' ',f);
  // fprintf(f,"%s\n",name);
  fprintf(f,"%s\n",m->str);
  for(q=m+1;q->code;++q)
    switch(q->code)
      {
      case tLIST:
        {
        struct base *bb;
        for(x=0;x!=i+2;++x) fputc(' ',f);
        fprintf(f,"LIST\n");
        for(bb=(struct base *)*t++;bb;bb=bb->next) indent_print_untagged(f,bb->_meta->str,i+4,bb);
        ++q;
        break;
        }
      case tSTRUCT:
        {
        indent_print_untagged(f,(q+1)->str,i+2,(struct base *)*t++);
        ++q;
        break;
        }
      case tSTRING:
        {
        char *s;
        for(x=0;x!=i+2;++x) fputc(' ',f);
        fprintf(f,"\"");
        s= *t++;
        while(*s)
          {
          switch(*s)
            {
            case '"': fprintf(f,"\""); break;
            default: fputc(*s,f);
            }
          ++s;
          }
        fprintf(f,"\"\n");
        break;
        }
      case tINTEGER:
        {
        for(x=0;x!=i+2;++x) fputc(' ',f);
        fprintf(f,"%d\n",(int)*t++);
        break;
        }
      }
  }

/* Print JSON structure */

void json_print(FILE *f,char *name,int i,struct base *b,int comma)
  {
  struct meta *m=b->_meta;
  struct meta *q;
  int x;
  void **t=(void **)((char *)b+sizeof(struct base));
  for(x=0;x!=i;++x) fputc(' ',f);
  if (name)
    fprintf(f,"\"%s\" : {\n", name);
  else
    fprintf(f,"{\n");
  for(q=m+1;q->code;++q)
    switch(q->code)
      {
      case tLIST:
        {
        struct base *bb;
        for(x=0;x!=i+2;++x) fputc(' ',f);
        fprintf(f,"\"%s\" : [\n",q->str);
        for(bb=(struct base *)*t++;bb;bb=bb->next) json_print(f,NULL,i+4,bb,(int)bb->next);
        for(x=0;x!=i+2;++x) fputc(' ',f);
        ++q;
        if((q+1)->code)
          fprintf(f,"],\n");
        else
          fprintf(f,"]\n");
        break;
        }
      case tSTRUCT:
        {
        json_print(f,q->str,i+2,(struct base *)*t++,(q+2)->code);
        ++q;
        break;
        }
      case tSTRING:
        {
        char *s;
        for(x=0;x!=i+2;++x) fputc(' ',f);
        fprintf(f,"\"%s\" : \"",q->str);
        s= *t++;
        while(*s)
          {
          switch(*s)
            {
            case '"': fprintf(f,"\""); break;
            default: fputc(*s,f);
            }
          ++s;
          }
        if((q+1)->code)
          fprintf(f,"\",\n");
        else
          fprintf(f,"\"\n");
        break;
        }
      case tINTEGER:
        {
        for(x=0;x!=i+2;++x) fputc(' ',f);
        if((q+1)->code)
          fprintf(f,"\"%s\" : %d,\n",q->str,(int)*t++);
        else
          fprintf(f,"\"%s\" : %d\n",q->str,(int)*t++);
        break;
        }
      }
  for(x=0;x!=i;++x) fputc(' ',f);
  if(comma)
    fprintf(f,"},\n");
  else
    fprintf(f,"}\n");
  }
