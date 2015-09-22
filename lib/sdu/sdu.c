/* Meta data table and utilities
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "sdu.h"
#include "hash.h"

/* Search for metadata structure definition table entry */

struct meta *metafind(struct schema *schema, char *name)
  {
  if(!schema->meta_hash)
    {
    int x;
    schema->meta_hash=htmk(64);
    /* First pass: add all structures to hash table */
    for(x=0;schema->metadata[x].code;++x)
      {
      /* Add this structure */
      htadd(schema->meta_hash,schema->metadata[x].str,schema->metadata+x);
      /* Skip to end of it */
      do
        ++x;
        while(schema->metadata[x].code);
      }
    /* Second pass: link */
    for (x = 0; schema->metadata[x].code; ++x)
      {
      int y = x;
      // printf("struct %s\n", schema->metadata[x].str);
      /* Skip structure name */
      for (++x; schema->metadata[x].code; ++x)
        {
        switch (schema->metadata[x].code)
          {
          case tLIST: case tSTRUCT:
            {
            ++x;
            schema->metadata[x].link = htfind(schema->meta_hash, schema->metadata[x].str);
            if (!schema->metadata[x].link)
              {
              fprintf(stderr, "Error in schema: couldn't find structure named '%s' (found in structure '%s')\n", schema->metadata[x].str,
                      schema->metadata[y].str);
              exit(-1);
              }
            break;
            }
          }
        }
      }
    }
  return htfind(schema->meta_hash,name);
  }

/* Return allocation size of structure */

int structsize(struct meta *m)
  {
  int x;
  /* Count no. members of structure */
  for(x=0,++m;m->code;++x)
    switch(m->code)
      {
      case tSTRUCT: m+=2; break;
      case tLIST: m+=2; break;
      case tSTRING: m+=1; break;
      case tINTEGER: m+=1; break;
      }

  /* Add in size for name and next pointers.  Multiply by member size. */
  return x*sizeof(void *)+sizeof(struct base);
  }

/* Create record */

void *mkraw(struct meta *m)
  {
  struct base *b=malloc(structsize(m));
  void **t=(void **)((char *)b+sizeof(struct base));
  b->_name=0;
  b->mom=0;
  b->_meta=m;
  b->next=0;
  for(++m;m->code;)
    {
    *t++=0;
    switch(m->code)
      {
      case tSTRUCT: m+=2; break;
      case tLIST: m+=2; break;
      case tSTRING: m+=1; break;
      case tINTEGER: m+=1; break;
      }
    }
  return b;
  }

/* Create record by name */

void *mk(struct schema *schema, char *name)
  {
  return mkraw(metafind(schema, name));
  }

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

#define GETC(f) do { \
  c = getc(f); \
  if (c == '\n') \
    ++line; \
  } while (0)

#define UNGETC(f, c) do { \
  if (c == '\n') \
    --line; \
  ungetc((c),(f)); \
  } while (0)

// Some XML information:

//     Whitespace: space, \r, \n, \t
//       _ means optional whitespace
//      ' ' means required whitespace
//     Element: <name[ attribute_=_value_]*>content</name_>
//     Empty element: <name[ attribute_=_value_]* /> (whitespace allowed around =)
//       Value: 'text' (no single quotes in text) | "text" (no double quotes in text)
//     content has:
//        Any characters except < or &
//        References: &#123; &#xFe; &name;
//          Predefined entities: &lt; &gt; &amp; &apos; &quot;
//        Comments: <!-- text -->  (-- is not allowed within text)
//        Processing instructions: <?name text ?>
//        Quoted: <![CDATA[....]]>   .... can have anything but ]]>

static int xml_tok(FILE *f)
  {
  int c;
  if(tok_unget!=-1)
    {
    c=tok_unget;
    tok_unget= -1;
    return c;
    }
  GETC(f);
  if (c == '<')
    {
    int t;
    int x;
    GETC(f);
    if(c=='/')
      t=END_TAG;
    else
      {
      t=TAG;
      UNGETC(c,f);
      }
    for(x=0;x!=sizeof(tok_str)-1;++x)
      {
      GETC(f);
      if(c!='>' && c != ' ' && c != '\n' && c != '\r' && c != '\t' && c!=EOF)
        tok_str[x]=c;
      else
        break;
      }
    while(c!='>' && c!=EOF)
      {
      if (c == '"')
        {
        do
          GETC(f);
          while(c != '"' && c != EOF);
        }
      else if (c == '\'')
        {
        do
          GETC(f);
          while(c != '\'' && c != EOF);
        }
      GETC(f);
      }
    tok_str[x]=0;
    return t;
    }
  else if (c == '&')
    {
    int hash;

    /* Read to next ; or EOF.  Save first MAX_ENTITY_LEN chars. */
    hash=0;

    GETC(f);
    if(c==EOF || c==';') goto done;
    hash = c;

    GETC(f);
    if(c==EOF || c==';') goto done;
    hash = (hash<<8) + c;

    GETC(f);
    if(c==EOF || c==';') goto done;
    hash = (hash<<8) + c;

    GETC(f);
    if(c==EOF || c==';') goto done;
    hash = (hash<<8) + c;

    GETC(f);
    if(c==EOF || c==';') goto done;
    hash = 0;

    while(c!=';' && c!=EOF)
      GETC(f);

    done:

    switch(hash)
      {
      case ('l'<<8)+'t': return '<';
      case ('g'<<8)+'t': return '>';
      case ('q'<<24)+('u'<<16)+('o'<<8)+'t': return '"';
      case ('a'<<24)+('p'<<16)+('o'<<8)+'s': return '\'';
      case ('a'<<16)+('m'<<8)+'p': return '&';
      }

    fprintf(stderr,"%d: bad entity\n",line);
    return ' ';
    }
  else
    {
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

struct base *xml_parse(FILE *f,char *name,struct schema *schema,struct meta *expect,int require)
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
            struct meta *n=(m+1)->link;
            first=last=xml_parse(f,n->str,schema,n,0);
            while(last)
              last=last->next=xml_parse(f,n->str,schema,n,0);
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
          struct meta *n=(m+1)->link;
          if ((*t++ = xml_parse(f,m->str,schema,n,1)) == 0)
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

/* Parse JSON structure */

// { items }
//
// item:
//   "name" : value
//
// value:
//    number
//    "string"
//    { items }

#define STR -4
#define NUM -5

static int json_tok(FILE *f)
  {
  int c;
  if (tok_unget!=-1)
    {
    c = tok_unget;
    tok_unget = -1;
    return c;
    }
  GETC(f);
  if (c == '"')
    {
    for (x = 0; x != sizeof(tok_str) - 1; ++x)
      {
      GETC(f);
      if (c == '"' || c == EOF)
        break;
      else if (c == '\\')
        {
        GETC(f);
        switch(c)
          {
          case '\\': tok_str[x] = '\\'; break;
          case '"': tok_str[x] = '"'; break;
          case 'n': tok_str[x] = '\n'; break;
          case 'r': tok_str[x] = '\r'; break;
          case 't': tok_str[x] = '\t'; break;
          case 'b': tok_str[x] = '\b'; break;
          case 'f': tok_str[x] = '\f'; break;
          case 'u':
            {
            }
          default:
            {
            fprintf(stderr, "%d: Error: Unknown escape character\n", line);
            }
          }
        }
      else
        tok_str[x] = c;
      }
    for (;;)
      {
      GETC(f);
      if (c == '"' || c == EOF)
        break;
      else if (c == '\\')
        {
        GETC(f);
        switch(c)
          {
          case '\\': break;
          case '"': break;
          case 'n': break;
          case 'r': break;
          case 't': break;
          case 'b': break;
          case 'f': break;
          }
        }
      }
    tok_str[x] = 0;
    return STR;
    }
  else if (c == '-')
    {
    tok_str[x = 0] = c;
    goto num;
    }
  else if (c == '.')
    {
    tok_str[x = 0] = c;
    goto flt_after_dp;
    }
  else if (c >= '0' && c <= '9')
    {
    tok_str[x = 0] = c;
    num:
    for (;;)
      {
      GETC(f);
      if (c >= '0' && c <= '9')
        {
        tok_str[x++] = c;
        }
      else break;
      }
    if (c == '.')
      {
      tok_str[x++] = c;
      goto flt_after_dp;
      }
    else if (c == 'e' || c =='E')
      {
      tok_str[x++] = 'e';
      goto flt_after_e;
      }
    else
      {
      UNGETC(f, c);
      tok_str[x] = 0;
      return NUM;
      }
    }
  else
    return c;
  }

static int json_skip(FILE *f)
  {
  int c;
  do
    c = json_tok(f);
    while (c == ' ' || c == 10 || c == 13 || c == 9);
  return c;
  }

struct base *json_parse(FILE *f,char *name,struct schema *schema,struct meta *expect,int require)
  {
  int c;
  c = json_skip(f);
  if (name) /* Named object */
    {
    if (c != STR)
      {
      fprintf(stderr,"%d: Missing name (expected named \"%s\")\n", line, name);
      return 0;
      }
    if (strcmp(tok_str, name))
      {
      fprintf(stderr,"%d: Unexpected name (found \"%s\", but expected \"%s\"\n", line, tok_str, name);
      return 0;
      }
    c = json_skip(f);
    if (c != ':')
      {
      fprintf(stdrr,"%d: Missing : between name and value\n", line);
      return 0;
      }
    c = json_skip(f);
    }
  if (c != '{')
    {
    fprintf(stderr,"%d: Unexpected start of object, but got '%c'\n", line, c);
    return NULL;
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
    fputs("{\n", f);
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
          fputs("],\n", f);
        else
          fputs("]\n", f);
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
            case '\\': fputs("\\\\", f); break;
            case '\r': fputs("\\r", f); break;
            case '\n': fputs("\\n", f); break;
            case '\f': fputs("\\f", f); break;
            case '\b': fputs("\\b", f); break;
            case '\t': fputs("\\t", f); break;
            case '"': fputs("\\\"", f); break;
            default:
              if (*(unsigned char *)s < 32) fprintf(f,"\\u%4.4x",*(unsigned char *)s);
              else fputc(*s,f);
            }
          ++s;
          }
        if((q+1)->code)
          fputs("\",\n", f);
        else
          fputs("\"\n", f);
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
    fputs("},\n", f);
  else
    fputs("}\n", f);
  }
