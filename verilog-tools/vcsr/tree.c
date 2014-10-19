// Parse tree

#include <stdlib.h>
#include <string.h>
#include "scan.h"
#include "tree.h"

// Construct a constant (value taken from most recent token)

struct expr *cons_copy(int t)
  {
  struct expr *e=malloc(sizeof(struct expr));
  e->t=t;
  e->str=malloc(tok_len+1);
  e->str[tok_len]=0;
  if(tok_len)
    memcpy(e->str,tok_str,tok_len);
  e->len=tok_len;
  e->l=0;
  e->r=0;
  e->rr=0;
  e->rest_of_line=0;
  return e;
  }

// Construct an operator

struct expr *cons(int t)
  {
  struct expr *e=malloc(sizeof(struct expr));
  e->t=t;
  e->str=0;
  e->len=0;
  e->l=0;
  e->r=0;
  e->rr=0;
  e->rest_of_line=0;
  return e;
  }

// Show expression parse tree

void show_expr(int ind,struct expr *e)
  {
  int x;
  int c;

  for(x=0;x!=ind;++x) printf(" ");

  if(!e)
    {
    printf("<empty>\n");
    return;
    }

  c=e->t;

  if(c&0xFF000000) printf("%c",255&(c>>24));
  if(c&0x00FF0000) printf("%c",255&(c>>16));
  if(c&0x0000FF00) printf("%c",255&(c>>8));
  if(c&0x000000FF) printf("%c",255&c);

  if(c=='INT')
    printf(" %d\n",e->len);
  else if(c=='NUM')
    {
    int x;
    for(x=0;x!=e->len;++x)
      if(!(e->str[x]=='0' || e->str[x]=='1'))
        break;
    if(x!=e->len || e->len>32)
      {
      printf(" %d'b",e->len);
      for(x=0;x!=e->len;++x)
        printf("%c",e->str[e->len-1-x]);
      }
    else
      {
      int n=0;
      for(x=e->len-1;x>=0;--x)
        n=(n<<1)+(e->str[x]&1);
      printf(" %d'h%x",e->len,n);
      }
    printf("\n");
    }
  else if(c=='STR')
    printf(" \"%s\"\n",e->str);
  else if(c=='NAME')
    printf(" %s\n",e->str);
  else
    printf("\n");

  if(e->l)
    show_expr(ind+2,e->l);
  if(e->r)
    show_expr(ind+2,e->r);
  if(e->rr)
    show_expr(ind+2,e->rr);
  }

struct expr *dup_expr(struct expr *e)
  {
  if(e)
    {
    struct expr *f=malloc(sizeof(struct expr));
    f->t=e->t;
    if(e->str)
      {
      f->str=malloc(e->len+1);
      if(e->len)
        memcpy(f->str,e->str,e->len);
      f->str[e->len]=0;
      }
    else
      f->str=0;
    f->len=e->len;
    f->l=dup_expr(e->l);
    f->r=dup_expr(e->r);
    f->rr=dup_expr(e->rr);
    return f;
    }
  else
    return 0;
  }

// Check if name is in expression

int find_field(struct expr *e,char *name)
  {
  if(e)
    if(e->t==',')
      if(find_field(e->l,name)) return 1;
      else return find_field(e->r,name);
    else if(e->t=='{' || e->t=='!' || e->t=='~')
      return find_field(e->r,name);
    else if(e->t=='NAME' && !strcmp(e->str,name))
      return 1;
  return 0;
  }
 
// Find a named port connections ".name(nets)"

struct expr *find_port(struct expr *e,char *name)
  {
  struct expr *r;
  if(e)
    if(e->t==',')
      if(r=find_port(e->l,name)) return r;
      else return find_port(e->r,name);
    else if(e->t=='.' && e->l && e->l->t=='NAME' && !strcmp(e->l->str,name))
      return e;
  return 0;
  }
