// Extract information from database

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "simp.h"
#include "tree.h"
#include "database.h"
#include "chdr.h"
#include "extract.h"

// Show input connections

int connect_list(struct expr *e,int start,struct decl *context)
  {
  if(e)
    if(e->t==',')
      {
      start=connect_list(e->r,start,context);
      return connect_list(e->l,start,context);
      }
    else if(e->t=='{')
      {
      return connect_list(e->r,start,context);
      }
    else if(e->t=='NAME')
      {
      int size=1;
      struct decl *d;
      d=find_decl(context,e->str);
      if(d && d->top && d->top->t=='NUM' && d->bot && d->bot->t=='NUM')
        size=1+val(d->top)-val(d->bot);
      printf("  bit=%d width=%d name=%s\n",start,size,e->str);
      start += size;
      return start;
      }
    else
      return start;
  }

void show_connect(struct expr *e,struct decl *context)
  {
  if(e)
    if(e->t==',')
      {
      show_connect(e->l,context);
      show_connect(e->r,context);
      }
    else if(e->t=='.' && e->l && e->l->t=='NAME' && (!strcmp(e->l->str,"in") || !strcmp(e->l->str,"func_in") || !strcmp(e->l->str,"func_out")))
      {
      connect_list(e->r,0,context);
      }
  }

// Return true if any children have ADDR

void find_em(struct inst *i)
  {
  if(i->insts)
    {
    for(i=i->insts;i;i=i->next) find_em(i);
    }
  else
    {
    struct param *p;
    for(p=i->params;p;p=p->next)
      if(!strcmp(p->name,"ADDR"))
        {
        printf("Found '%s' (a %s) at ",i->name,i->module);
        show_expr(0,p->expr);
        show_connect(i->connections,i->mom->decls);
        break;
        }
    }
  }

// Find a net in a port connection

void cat_name(char *buf,struct inst *i)
  {
  if(i)
    {
    cat_name(buf,i->mom);
    if(buf[0])
      strcat(buf,"/");
    strcat(buf,i->name);
    }
  }

char *build_name(struct inst *i)
  {
  char buf[1024];
  buf[0]=0;
  cat_name(buf,i);
  return strdup(buf);
  }

/* Add a trailing _l to a string if it doesn't have one.
   Remove a trailing _l to a string if it does have one. */

void invcpy(char *dst,char *src)
  {
  while(*src)
    if(src[0]=='_' && src[1]=='l' && !src[2])
      {
      *dst=0;
      return;
      }
    else
      *dst++ = *src++;
  *dst++ = '_';
  *dst++ = 'l';
  *dst=0;
  }

int chdr_list_gen(struct expr *e,int start,struct decl *context,struct expr *o,char *pat,char *sub,int inv,int inv_map,struct expr *iz)
  {
  int size=1;
  struct decl *d;

  if(e->t=='[')
    {
    if(e->r->t==':')
      {
      struct expr *top=simp(NULL,e->r->l);
      struct expr *bot=simp(NULL,e->r->r);
      size=1+val(top)-val(bot);
      }
    e=e->l;
    }
  else
    {
    d=find_decl(context,e->str);
    if(d && d->top && d->top->t=='NUM' && d->bot && d->bot->t=='NUM')
      size=1+val(d->top)-val(d->bot);
    }
  // printf("  bit=%d width=%d name=%s\n",start,size,e->str);
  // if(find_field(o,e->str)) // If field is in 'out', field must be _RdWr.
  if(o) // If there is an 'out', field is _RdWr
    {
    char bf[256];
    char buf[256];
    if(inv ^ !!((1<<start)&inv_map))
      invcpy(bf,e->str);
    else
      strcpy(bf,e->str);
    subst(buf,pat,sub,bf);
    found_field(strdup(buf),start,size,"_RdWr",val(slice(iz,start,size)),!!iz);
    }
  else
    {
    char bf[256];
    char buf[256];
    if(inv ^ !!((1<<start)&inv_map))
      invcpy(bf,e->str);
    else
      strcpy(bf,e->str);
    subst(buf,pat,sub,bf);
    found_field(strdup(buf),start,size,"_RdOnly",val(slice(iz,start,size)),!!iz);
    }
  start += size;
  return start;
  }

int chdr_list(struct expr *e,int start,struct decl *context,struct expr *o,char *pat,char *sub,int inv,int inv_map,struct expr *iz)
  {
  if(e)
    if(e->t==',')
      {
      start=chdr_list(e->r,start,context,o,pat,sub,inv,inv_map,iz);
      return chdr_list(e->l,start,context,o,pat,sub,inv,inv_map,iz);
      }
    else if(e->t=='{')
      {
      return chdr_list(e->r,start,context,o,pat,sub,inv,inv_map,iz);
      }
    else if(e->t=='!' || e->t=='~')
      {
      return chdr_list(e->r,start,context,o,pat,sub,!inv,inv_map,iz);
      }
    else if(e->t=='[')
      {
      return chdr_list_gen(e,start,context,o,pat,sub,inv,inv_map,iz);
      }
    else if(e->t=='NAME')
      {
      return chdr_list_gen(e,start,context,o,pat,sub,inv,inv_map,iz);
      }
    else
      return start;
  }

// Find regs: generate c header file

void find_em_chdr(struct inst *i)
  {
  struct param *p;
  char buf_i[80];
  char buf_o[80];
  char *pat=0;
  char *sub=0;
  struct param *inv_p;
  struct param *iz_p;
  strcpy(buf_i,"in");
  strcpy(buf_o,"out");
  if(find_param(i->params,"REG") && (p=find_param(i->params,"ADDR")))
    {
    struct expr *e;
    char *ptrs[5];
    int x;
    for(x=0;x!=5;++x) ptrs[x]=0;
    found_reg(build_name(i),val(p->expr),"REGISTER",0,0);
    loop:
    if(e=find_port(i->connections,buf_i))
      {
      struct expr *f;
      if(e->rest_of_line && scan_comment(e->rest_of_line,ptrs) && ptrs[0] && ptrs[1] && ptrs[2] && !strcmp(ptrs[0],"VCSR"))
        {
        pat=ptrs[3];
        sub=ptrs[4];
        i=i->mom;
        strcpy(buf_i,ptrs[1]);
        strcpy(buf_o,ptrs[2]);
        goto loop;
        }
      f=find_port(i->connections,buf_o);
      if(f)
        {
        if(f->rest_of_line && scan_comment(f->rest_of_line,ptrs) && ptrs[0] && ptrs[1] && ptrs[2] && !strcmp(ptrs[0],"VCSR"))
          {
          pat=ptrs[3];
          sub=ptrs[4];
          i=i->mom;
          strcpy(buf_i,ptrs[1]);
          strcpy(buf_o,ptrs[2]);
          goto loop;
          }
        f=f->r;
        }
      inv_p=find_param(i->params,"INV");
      iz_p=find_param(i->params,"IZ");
      chdr_list(e->r,0,i->mom->decls,f,pat,sub,0,val(inv_p ? inv_p->expr : 0),(iz_p?iz_p->expr:0));
      }
    else if(e=find_port(i->connections,buf_o))
      {
      if(e->rest_of_line && scan_comment(e->rest_of_line,ptrs) && ptrs[0] && ptrs[1] && ptrs[2] && !strcmp(ptrs[0],"VCSR"))
        {
        pat=ptrs[3];
        sub=ptrs[4];
        i=i->mom;
        strcpy(buf_i,ptrs[1]);
        strcpy(buf_o,ptrs[2]);
        goto loop;
        }
      inv_p=find_param(i->params,"INV");
      iz_p=find_param(i->params,"IZ");
      chdr_list(e->r,0,i->mom->decls,e->r,pat,sub,0,val(inv_p ? inv_p->expr : 0),(iz_p?iz_p->expr:0));
      }
    }
  else if(find_param(i->params,"TABLE") && (p=find_param(i->params,"ADDR")))
    {
    struct param *q;
    struct expr *e;
    int word_size=0;
    int no_words=0;
    q=find_param(i->params,"DATAWIDTH");
    if(q) word_size=val(q->expr);
    if(!word_size)
      {
      printf("Problem with TABLE %s: can't determine word size (need DATAWIDTH parameter)\n",i->module);
      }
    q=find_param(i->params,"SIZE");
    if(q)
      no_words=val(q->expr)*8;
    else if(q=find_param(i->params,"ADDRWIDTH"))
      no_words=(1<<val(q->expr))*8;
    if(!no_words)
      {
      printf("Problem with TABLE %s: can't determine size (need ADDRWIDTH or SIZE parameter)\n",i->module);
      }
    if(no_words%word_size)
      {
      printf("Problem with TABLE %s: table size is not exact multiple of word size\n",i->module);
      }   
    no_words/=word_size;

    found_reg(build_name(i),val(p->expr),"TABLE",word_size,no_words);
    if(e=find_port(i->connections,"in"))
      {
      struct expr *f=find_port(i->connections,"out");
      if(f) f=f->r;
      inv_p=find_param(i->params,"INV");
      iz_p=find_param(i->params,"IZ");
      chdr_list(e->r,0,i->mom->decls,f,0,0,0,val(inv_p ? inv_p->expr : 0),(iz_p?iz_p->expr:0));
      }
    else if(e=find_port(i->connections,"out"))
      {
      inv_p=find_param(i->params,"INV");
      iz_p=find_param(i->params,"IZ");
      chdr_list(e->r,0,i->mom->decls,e->r,0,0,0,val(inv_p ? inv_p->expr : 0),(iz_p?iz_p->expr:0));
      }
    }
  else if(i->insts)
    {
    for(i=i->insts;i;i=i->next) find_em_chdr(i);
    }
  }
