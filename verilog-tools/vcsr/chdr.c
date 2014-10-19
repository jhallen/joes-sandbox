// Generate C header file

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chdr.h"

// A field of a register type

struct field
  {
  struct field *next;
  int start;
  int size;
  int iz;
  int iz_flag;
  char *type;
  char *name;
  };

// A register type

struct rtype
  {
  struct rtype *next;
  char *name;
  char *trial_name;
  struct field *fields;
  };

// List of register types in design

struct rtype *rtypes;

// A register

struct regs
  {
  struct regs *next;
  char *name;
  char *trial_name;
  int addr;
  struct rtype *rtype;
  char *type;
  int word_size;
  int no_words;
  };

// List of registers in design

struct regs *regs;

int rmatch(struct field *a,struct field *b)
  {
  for(;;)
    if(!a && !b)
      return 1;
    else if(!a || !b)
      return 0;
    else if(a->start==b->start && a->size==b->size && !strcmp(a->name,b->name) && !strcmp(a->type,b->type) && a->iz==b->iz &&
            a->iz_flag==b->iz_flag)
      {
      a=a->next;
      b=b->next;
      }
    else
      return 0;
  }

// Complete generation of register type for most recently found register.
// Find an existing type which matches, or keep this one as a new type.

void close_reg()
  {
  if(regs->rtype)
    { // Find matching type, otherwise add new type.
    struct rtype *r;
    for(r=rtypes;r;r=r->next)
      if(rmatch(r->fields,regs->rtype->fields))
        {
        regs->rtype=r;
        break;
        }
    if(!r)
      {
      regs->rtype->next=rtypes;
      rtypes=regs->rtype;
      }
    }
  }

// A register was found: add it to list

void found_reg(char *name,int addr,char *type,int word_size,int no_words)
  {
  struct regs *r;
  if(regs)
    close_reg();
  r=malloc(sizeof(struct regs));
  r->next=regs;
  regs=r;
  r->name=name;
  r->addr=addr;
  r->rtype=0;
  r->type=type;
  r->no_words=no_words;
  r->word_size=word_size;
  }

// A field was found: add it to register type of most recently found register

void found_field(char *name,int start,int size,char *type,int iz,int iz_flag)
  {
  struct field *f;
  if(!regs->rtype)
    {
    char buf[1024];
    regs->rtype=malloc(sizeof(struct rtype));
    sprintf(buf,"%s_t",regs->name);

    regs->rtype->name=strdup(buf);
    regs->rtype->fields=0;
    }
  f=malloc(sizeof(struct field));
  f->next=regs->rtype->fields;
  regs->rtype->fields=f;
  f->start=start;
  f->size=size;
  f->name=name;
  f->type=type;
  f->iz=iz;
  f->iz_flag=iz_flag;
  }

// Check if trial names are unique

int check_trial_names(struct regs *r)
  {
  while(r)
    {
    struct regs *s;
    for(s=r->next;s;s=s->next)
      if(!strcmp(r->trial_name,s->trial_name))
        return 0;
    r=r->next;
    }
  return 1;
  }

// Edit names into trial names

void trial(struct regs *r,char *keep,char *remove)
  {
  while(r)
    {
    char buf[1024];
    int x;
    buf[0]=0;
    r->trial_name=0;
    strcpy(buf,keep);
    /* Match keep prefix */
    for(x=0;keep[x] && r->name[x] && keep[x]==r->name[x];++x);
    if(!keep[x] && (!x || r->name[x]=='/'))
      {
      int y;
      if(x) ++x; // Bump past '/'
      /* Match remove part */
      for(y=0;remove[y] && r->name[x+y] && remove[y]==r->name[x+y];++y);
      if(!remove[y] && r->name[x+y]=='/')
        {
        if(buf[0])
          strcat(buf,r->name+x+y); // Keep the '/'
        else
          strcat(buf,r->name+x+y+1); // Bump past '/'
        r->trial_name=strdup(buf);
        }
      }
    if(!r->trial_name)
      r->trial_name=strdup(r->name);
    r=r->next;
    }
  }

// Transfer trial names to real names

void transfer(struct regs *r)
  {
  while(r)
    {
    r->name=r->trial_name;
    r=r->next;
    }
  }

// Return number of names which have common postfix

int postfix_count(struct regs *r,char *postfix)
  {
  int count=0;
  int len=strlen(postfix);
  while(r)
    {
    int l=strlen(r->name);
    if(l==len)
      {
      if(!strcmp(r->name,postfix)) ++count;
      }
    else if(l>len && r->name[l-len-1]=='/' && !strcmp(r->name+l-len,postfix))
      ++count;
    r=r->next;
    }
  return count;
  }

// Delete prefix of postfix which doesn't help uniqueness

void delete_useless_prefix(struct regs *r,char *postfix,int del)
  {
  int len=strlen(postfix);
  while(r)
    {
    int l=strlen(r->name);
    if(l==len)
      {
      if(!strcmp(r->name,postfix))
        {
        strcpy(r->name,postfix+del);
        }
      else if(l>len && r->name[l-len-1]=='/' && !strcmp(r->name+l-len,postfix))
        {
        strcpy(r->name+l-len,postfix+del);
        }
      }
    r=r->next;
    }
  }

// Remove redundant parts of names

void reduce_names(struct regs *regs)
  {
  struct regs *r;
  for(r=regs;r;r=r->next)
    {
    int x=strlen(r->name);
    int n=-1; // Current uniqueness
    int m;    // New uniqueness
    int y;

    printf("Working on '%s'\n",r->name);

    while(x>0) // If we fall out due to this, the names were not unique to begin with
      {
      // Position to beginning of name
      while(x && r->name[x-1]!='/') --x;
      // Get uniqueness
      m=postfix_count(regs,r->name+x);
      printf("Uniqueness for '%s' is %d\n",r->name+x,m);
      if(m==1)
        { // We're done: make postfix new name
        printf("Done.\n");
        r->name=strdup(r->name+x);
        break;
        }
      else if(m==n)
        { // This part doesn't help, so delete it.
        printf("Delete.\n");
        delete_useless_prefix(regs,r->name+x,y-x);
        y=x;
        --x;
        }
      else
        { // This part helps, but we're not done.
        printf("Keep.\n");
        n=m;
        y=x;
        --x;
        }
      }
    }
  }

// Copy string / remove column

void remove_col(char *d,char *s,int col)
  {
  int c=0;
  int flg=0;
  while(*s)
    if(c==col)
      {
      while(*s && *s!='/') ++s;
      if(*s=='/') ++s;
      ++c;
      }
    else
      {
      if(flg)
        *d++='/';
      else
        flg=1;
      while(*s && *s!='/') *d++= *s++;
      if(*s=='/') ++s;
      ++c;
      }
  *d=0;
  }

// Find column

char *find_col(char *s,int col)
  {
  while(col--)
    {
    while(*s && *s!='/') ++s;
    if(*s=='/') ++s;
    }
  return s;
  }

// Count no. names in path

int count_cols(char *s)
  {
  int x;
  int count=1;
  for(x=0;s[x];++x)
    if(s[x]=='/') ++count;
  return count;
  }

// Remove indicated column

int remove_it(struct regs *r,char *name,int ncols,int col)
  {
  char *bf=find_col(name,col+1);
  while(r)
    {
    if(count_cols(r->name)==ncols && !strcmp(bf,find_col(r->name,col+1)))
      {
      char bf[1024];
      remove_col(bf,r->name,col);
      printf("removing %s to %s\n",r->name,bf);
      r->name=strdup(bf);
      }
    r=r->next;
    }
  }

// Are names unique if suggested column is missing?

int unique(struct regs *regs,char *name,int ncols,int col)
  {
  char *bf=find_col(name,col+1);
  struct regs *r;

  printf("ncols=%d, del col=%d, postfix=%s\n",ncols,col,bf);

  for(r=regs;r;r=r->next)
    if(count_cols(r->name)==ncols && !strcmp(bf,find_col(r->name,col+1)))
      {
      char buf1[1024];
      struct regs *s;
      remove_col(buf1,r->name,col);
      printf("Checking %s (reduced to %s)\n",r->name,buf1);
      for(s=regs;s;s=s->next)
        if(!strcmp(buf1,s->name))
          return 0;
        else if(s!=r && count_cols(s->name)==ncols && !strcmp(bf,find_col(s->name,col+1)))
          {
          char buf2[1024];
          remove_col(buf2,s->name,col);
          printf("Comparing %s %s\n",buf1,buf2);
          if(!strcmp(buf1,buf2))
            return 0;
          }
      }
  return 1;
  }

// Simplfy names

char *get_name(struct regs *r)
  {
  return r->name;
  }

void simp_names(struct regs *regs)
  {
  struct regs *r;
  for(r=regs;r;r=r->next)
    {
    int ncols=count_cols(r->name);
    int x;
    for(x=ncols-2;x>=0;--x)
      if(unique(regs,get_name(r),ncols,x))
        {
        remove_it(regs,get_name(r),ncols,x);
        --ncols;
        }
    }
  }

// Convert slashes to underbars in names

void fix_names(struct regs *regs)
  {
  struct regs *r, *s;
  for(r=regs;r;r=r->next)
    {
    char new_name[1024];
    int x;
    strcpy(new_name,r->name);
    for(x=0;new_name[x];++x)
      if(new_name[x]=='/')
        new_name[x]='_';
    for(s=regs;s;s=s->next)
      if(!strcmp(s->name,new_name))
        break;
    if(!s)
      r->name=strdup(new_name);
    }
  }

// eliminate trailing _reg

void fix_names1(struct regs *regs)
  {
  struct regs *r, *s;
  for(r=regs;r;r=r->next)
    {
    char new_name[1024];
    int x;
    strcpy(new_name,r->name);
    x=strlen(new_name);
    if(x>4 && !strcmp(new_name+x-4,"_reg"))
      new_name[x-4]=0;
    if(x>6 && !strcmp(new_name+x-6,"_reg_t"))
      {
      new_name[x-6]='_';
      new_name[x-5]='t';
      new_name[x-4]=0;
      }
    for(s=regs;s;s=s->next)
      if(!strcmp(s->name,new_name))
        break;
    if(!s)
      r->name=strdup(new_name);
    }
  }

// Check for address conflicts

void check_conflict()
  {
  struct regs *r;
  for(r=regs;r;r=r->next)
    {
    struct regs *s;
    int addr=r->addr;
    int size=r->word_size/8*r->no_words;
    if(!size)
      size=4;
    for(s=r->next;s;s=s->next)
      {
      int ad=s->addr;
      int sz=s->word_size/8*s->no_words;
      if(!sz)
        sz=4;
      if(!(ad>=addr+size || ad+sz<=addr))
        printf("Register %s (addr='h%x size='h%x) conflicts with register %s (addr='h%x size='h%x)\n",s->name,ad,sz,r->name,addr,size);
      }
    }
  }

// Generate C header file file

void gen_chdr(FILE *fd,char *name)
  {
  struct rtype *r;
  struct regs *i;
  if(regs)
    close_reg();
  reduce_names(regs);
  simp_names(regs);
  fix_names(regs);
  fix_names1(regs);

  // Cheating here
  reduce_names((struct regs *)rtypes);
  simp_names((struct regs *)rtypes);
  fix_names((struct regs *)rtypes);
  fix_names1((struct regs *)rtypes);

  // Check for conflicting addresses
  check_conflict();

  fprintf(fd,"/* This file utomatically generated by vcsr */\n");
  fprintf(fd,"/* Do not edit: instead modify Verilog RTL */\n\n");
  fprintf(fd,"#include \"csrdef.h\"\n\n");

  for(r=rtypes;r;r=r->next)
    {
    struct field *f;
    fprintf(fd,"struct %s {\n",r->name);
    for(f=r->fields;f;f=f->next)
        if(f->iz_flag)
          fprintf(fd,"  %sInit(%s, %d, %d, 0x%x);\n",f->type,f->name,f->size,f->start,f->iz);
        else
          fprintf(fd,"  %s(%s, %d, %d);\n",f->type,f->name,f->size,f->start);
    fprintf(fd,"};\n\n");
    }
  fprintf(fd,"struct %s_t CHIP_COLLECTION {\n",name);
  for(i=regs;i;i=i->next)
    if(i->word_size)
      fprintf(fd,"  %s(%s[%d:%d], %s, 0x%x);\n",i->type,i->name,i->no_words,i->word_size,i->rtype ? i->rtype->name : "(undefined)",i->addr);
    else
      fprintf(fd,"  %s(%s, %s, 0x%x);\n",i->type,i->name,i->rtype ? i->rtype->name : "(undefined)",i->addr);
  fprintf(fd,"} %s;\n",name);
  }
