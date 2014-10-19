/* Compute subtotals: 2nd version */

/* Input:
 *   foo x(10) y=12 z=13
 *   foo x(20) y(12) z(13) p(9)
 *   bar a(1) b(2)
 *   bar a(1) b(2)
 *
 * Output:
 *   foo x(30) y(24) z(26) p(9)
 *   bar a(2) b(4)
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct field
  {
  struct field *next;
  char *s;
  int total;
  } *fields;

/* Lookup a named field */

struct field *find(char *s)
{
  struct field *f;
  for(f=fields;f;f=f->next)
    if(!strcmp(f->s,s)) break;
  if(f)
    return f;
  else
    {
    f=malloc(sizeof(struct field));
    f->next=fields;
    f->s=strdup(s);
    f->total=0;
    fields=f;
    return f;
    }
}

/* Free all fields */

void dumpem()
{
  struct field *f;
  struct field *n=fields;
  while((f = n) != 0)
    {
    n=f->next;
    free(f->s);
    free(f);
    }
  fields=0;
}

/* Reverse list */

struct field *rev(struct field *f)
{
  struct field *n=0;
  struct field *g;
  for(;f;f=g)
    {
    g=f->next;
    f->next=n;
    n=f;
    }
  return n;
}

/* Print totals */

void printem()
{
  struct field *f;
  for(f=fields;f;f=f->next)
    printf(" %s(%d)",f->s,f->total);
  printf("\n");
}

/* Zero terminate field, return pointer to start of next one */

char *field(char *s)
{
  while(*s && *s!=' ' && *s!='\t')
    ++s;
  if(*s)
    {
    *s++=0;
    while(*s==' ' || *s=='\t')
      ++s;
    }
  return s;
}

/* Zero terminate a word.  Return pointer to character after the NUL */

char *word(char *s)
{
  while(*s && *s!='(' && *s!='=' && *s!=' ' && *s!='\t')
    ++s;
  if(*s)
    *s++=0;
  return s;
}

/* Zero terminate a number.  Return pointer to character after the NUL */

char *num(char *s)
{
  while(*s && *s>='0' && *s<='9')
    ++s;
  if(*s)
    *s++=0;
  return s;
}

/* Skip whitespace */

char *skip(char *s)
{
  while(*s==' ' || *s=='\t')
    ++s;
  return s;
}

/* Parse name number list */

void parse_list(char *s)
{
  while(*s)
    {
    char *name;
    char *n;
    struct field *f;
    s=skip(s);
    name=s; s=word(s);
    n=s; s=num(s);
    f=find(name);
    f->total+=atoi(n);
    }
}

/* Main */

int main()
{
  char buf[1024];
  char cur[1024];
  cur[0]=0;
  while(gets(buf))
    {
    char *s;
    char *name=buf; s=word(buf);
    if(strcmp(name,cur))
      {
      if(cur[0])
        {
        printf("%s",cur);
        fields=rev(fields);
        printem();
        dumpem();
        }
      strcpy(cur,buf);
      }
    parse_list(s);
    }
  if(cur[0])
    {
    printf("%s",cur);
    fields=rev(fields);
    printem();
    }
  return 0;
}
