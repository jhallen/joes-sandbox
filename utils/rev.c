// reverse lines

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct linebuf
  {
  struct linebuf *next;
  char *line;
  };

struct linebuf *linebuf;

int main()
  {
  char buf[1024];
  struct linebuf *x;
  while(gets(buf))
    {
    x=malloc(sizeof(linebuf));
    x->next=linebuf;
    x->line=strdup(buf);
    linebuf=x;
    }
  for(x=linebuf;x;x=x->next)
    puts(x->line);
  return 0;
  }
