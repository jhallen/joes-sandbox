// Constant expression evaluator

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "database.h"
#include "tree.h"
#include "simp.h"

// Verify that expression is a number with only 0s and 1s

int check(struct expr *e)
  {
  if(e->t=='NUM')
    {
    int x;
    for(x=0;x!=e->len;++x)
      if(!(e->str[x]=='0' || e->str[x]=='1'))
        return 0;
    return 1;
    }
  return 0;
  }

// Zero extend a number to a particular size
// (this causes big problems with subtraction)

void extend(struct expr *e,int size)
  {
  int x;
  e->str=realloc(e->str,size+1);
  e->str[size]=0;
  for(x=e->len;x!=size;++x)
    e->str[x]='0';
  e->len=size;
  }

// Add r to l

int add(struct expr *l,struct expr *r)
  {
  int c=0;
  int x;
  for(x=0;x!=l->len;++x)
    if((r->str[x]=='0' || r->str[x]=='1') &&
       (l->str[x]=='0' || l->str[x]=='1'))
      {
      int a=(l->str[x]&1)+(r->str[x]&1)+c;
      l->str[x]='0'+(a&1);
      c=(a>>1);
      }
    else
      {
      printf("z or x in constant %c %c %d %d\n",l->str[x],r->str[x],l->len,r->len);
      return 0;
      }
  return 1;
  }

// Subtract r from l

int sub(struct expr *l,struct expr *r)
  {
  int c=1;
  int x;
  for(x=0;x!=l->len;++x)
    if((r->str[x]=='0' || r->str[x]=='1') &&
       (l->str[x]=='0' || l->str[x]=='1'))
      {
      int a=(l->str[x]&1)+((~r->str[x])&1)+c;
      l->str[x]='0'+(a&1);
      c=(a>>1);
      }
    else
      {
      printf("z or x in constant %c %c %d %d\n",l->str[x],r->str[x],l->len,r->len);
      return 0;
      }
  return 1;
  }

// shift l left r times

int shl(struct expr *l,struct expr *r)
  {
  int x;
  int n=val(r);
  if(n>=l->len)
    {
    for(x=0;x!=l->len;++x) l->str[x]='0';
    }
  else
    {
    memmove(l->str+n,l->str,l->len-n);
    for(x=0;x!=n;++x) l->str[x]='0';
    }
  return 1;
  }

// shift l right r times

int shr(struct expr *l,struct expr *r)
  {
  int x;
  int n=val(r);
  if(n>=l->len)
    {
    for(x=0;x!=l->len;++x) l->str[x]='0';
    }
  else
    {
    memmove(l->str,l->str+n,l->len-n);
    for(x=l->len-n;x!=l->len;++x) l->str[x]='0';
    }
  return 1;
  }

void show_params(struct param *p)
  {
  while(p)
    {
    printf("  %s\n",p->name);
    p=p->next;
    }
  }

struct expr *concat(struct param *params,struct expr *e)
  {
  if(e->t==',')
    {
    struct expr *l=concat(params,e->l);
    struct expr *r=concat(params,e->r);
    if(l->t=='NUM' && r->t=='NUM')
      {
      e=cons('NUM');
      e->len=l->len+r->len;
      e->str=malloc(e->len+1);
      e->str[e->len]=0;
      memcpy(e->str+r->len,l->str,l->len);
      memcpy(e->str,r->str,r->len);
      return e;
      }
    else
      {
      e=cons(',');
      e->l=l;
      e->r=r;
      return e;
      }
    }
  else
    return simp(params,e);
  }

// Simplify an expression

struct expr *simp(struct param *params,struct expr *e)
  {
  switch(e->t)
    {
    case '<<':
      {
      struct expr *l, *r;
      l=simp(params,e->l);
      r=simp(params,e->r);

      if(check(l) && check(r))
        {
        l=dup_expr(l);
        shl(l,r);
        return l;
        }
      else
        {
        e=cons('<<');
        e->l=l;
        e->r=r;
        return e;
        }
      }

    case '>>':
      {
      struct expr *l, *r;
      l=simp(params,e->l);
      r=simp(params,e->r);

      if(check(l) && check(r))
        {
        l=dup_expr(l);
        shr(l,r);
        return l;
        }
      else
        {
        e=cons('>>');
        e->l=l;
        e->r=r;
        return e;
        }
      }

    case '[': // Slice
      {
      struct expr *l=simp(params,e->l);
      if(check(l))
        if(e->r->t==':')
          {
          struct expr *top=simp(params,e->r->l);
          struct expr *bot=simp(params,e->r->r);
          if(check(top) && check(bot) && val(top)>=val(bot))
            return slice(l,val(bot),val(top)-val(bot)+1);
          else
            return e;
          }
        else
          {
          struct expr *idx=simp(params,e->r);
          if(check(idx))
            return slice(l,val(idx),1);
          else
            return e;
          }
      else
        return e;
      }

    case '{': // Concatenate
      {
      return concat(params,e->r);
      }

    case '+':
      {
      struct expr *l, *r;
      l=simp(params,e->l);
      r=simp(params,e->r);

      if(check(l) && check(r))
        {
        l=dup_expr(l);
        r=dup_expr(r);
        if(l->len>r->len)
          extend(r,l->len);
        else if(r->len>l->len)
          extend(l,r->len);
        add(l,r);
        return l;
        }
      else
        {
        e=cons('+');
        e->l=l;
        e->r=r;
        return e;
        }
      }

    case '-':
      {
      struct expr *l, *r;
      l=simp(params,e->l);
      r=simp(params,e->r);

      if(check(l) && check(r))
        {
        l=dup_expr(l);
        r=dup_expr(r);
        if(l->len>r->len)
          extend(r,l->len);
        else if(r->len>l->len)
          extend(l,r->len);
        add(l,r);
        return l;
        }
      else
        {
        e=cons('-');
        e->l=l;
        e->r=r;
        return e;
        }
      }

    case 'NUM':
      return e;

    case 'NAME':
      { // Find parameter
      struct param *p;
      for(p=params;p;p=p->next)
        if(!strcmp(p->name,e->str))
          {
          if(!p->expr)
            {
            printf("Parameter %s defined, but with no expression?\n",p->name);
            return 0;
            }
          else if(p->expr->t!='NUM')
            {
            printf("Parameter %s not a number\n",p->name);
            show_expr(0,p->expr);
            return 0;
            }
          // Should already be evaluated
          return p->expr;
          }
      printf("Parameter '%s' not found in\n",e->str);
      show_params(p);
      return 0;
      }

    default:
      {
      return e;
      }
    }
  }

// Convert verilog number to a C integer.  Returns 0
// if 'e' is not a 'NUM'.

int val(struct expr *e)
  {
  if(e && e->t=='NUM')
    {
    int n=0;
    int x;
    for(x=0;x!=e->len;++x)
      if(e->str[e->len-1-x]=='1')
        n=(n<<1)+1;
      else
        n=(n<<1);
    return n;
    }
  else
    return 0;
  }

// Chop some bits out of a constant

struct expr *slice(struct expr *e,int start,int size)
  {
  if(e && e->t=='NUM')
    {
    struct expr *r=cons('NUM');
    int x;

    r->str=malloc(size+1);
    r->str[size]=0;
    r->len=size;
    for(x=0;x!=size;++x)
      r->str[x]='0';

    for(x=0;start<e->len && x<size;++x,++start)
      r->str[x]=e->str[start];

    return r;
    }
  else
    return 0;
  }
