/* Database
 *
 * Joe Allen,  Sep 2004
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simp.h"
#include "tree.h"
#include "database.h"

struct module *modules;		// Modules
struct module *top;		// Top-level module
char select_top[1024];		// User assigned top level module name

// Find module by name

struct module *find(char *name)
  {
  struct module *m;
  for(m=modules;m;m=m->next)
    if(!strcmp(m->name,name)) return m;
  return 0;
  }

// Add new module to database
// New module becomes current module.

struct module *add_module(char *name)
  {
  struct module *m=malloc(sizeof(struct module));
  m->name=name;
  m->next=modules;
  m->insts=0;
  m->insts_last=0;
  m->linked=0;
  m->refcount=0;
  m->instcount=0;
  m->params=0;
  m->params_last=0;
  m->defparams=0;
  m->defparams_last=0;
  m->decls=0;
  modules=m;
  return m;
  }

// Add an instance to current module

struct inst *add_inst(char *inst_name,char *mod_name)
  {
  struct inst *inst;
  // printf("%s %d: %s %s\n",modules->name,tok_line,mod,tok_str);
  inst=malloc(sizeof(struct inst));
  inst->next=0;
  if(modules->insts)
    {
    modules->insts_last->next=inst;
    modules->insts_last=inst;
    }
  else
    modules->insts=modules->insts_last=inst;
  inst->name=inst_name;
  inst->module=mod_name;
  inst->pos_defparams=0;
  inst->pos_last=0;
  inst->mod=0;
  inst->insts=0;
  inst->params=0;
  inst->connections=0;
  inst->decls=0;
  inst->mom=0;
  return inst;
  }

// Link

int link_one(struct module *m)
  {
  struct inst *i;
  if(m->linked)
    return m->instcount;
  m->instcount=1;
  for(i=m->insts;i;i=i->next)
    if(i->mod=find(i->module))
      {
      m->instcount+=link_one(i->mod);
      ++i->mod->refcount;
      }
  m->linked=1;
  return m->instcount;
  }

void link()
  {
  struct module *m;
  for(m=modules;m;m=m->next)
    link_one(m);
  }

// Find top level module

void find_top()
  {
  struct module *m;
  int sz;
  int flag=0;
  if(select_top[0])
    top=find(select_top);
  else
    {
    printf("Guessing top level module...\n");
    printf("Candidates:\n");
    for(m=modules;m;m=m->next)
      if(!m->refcount)
        {
        printf("  %s %d\n",m->name,m->instcount);
        if(!top) top=m;
        if(m->insts) flag=1;
        }
    if(flag)
      {
      top=0;
      sz=0;
      for(m=modules;m;m=m->next)
        if(!m->refcount && m->insts)
          if(!top || m->instcount>sz)
            {
            top=m;
            sz=m->instcount;
            }
      }
    }
  if(top)
    printf("Module %s selected as top\n",top->name);
  else
    {
    printf("No top level module\n");
    exit(-1);
    }
  }

// Copy parameters
// retains order.

struct param *copy_params(struct param *first,struct param *p)
  {
  if(p)
    {
    struct param *q=malloc(sizeof(struct param));
    q->name=p->name;
    q->inst=p->inst;
    q->expr=p->expr;
    if(!first)
      first=q;
    q->context=first;
    q->next=copy_params(first,p->next);
    return q;
    }
  else
    return 0;
  }

// Apply defparams to parameter list 'p' of a particular instance 'inst_name'.
// 'defparams' is list of defparams for all of the instances in the module, so we have to
// match agains a particular instance name 'inst_name'.  defparams refer to parameters in
// parameter list 'context' (should be already evaluated).

void apply_defparams(struct param *p,char *inst_name,struct param *context,struct param *defparams)
  {
  struct param *q;
  for(q=defparams;q;q=q->next)
    if(!strcmp(q->inst,inst_name))
      {
      struct param *i;
      for(i=p;i;i=i->next)
        if(!strcmp(i->name,q->name))
          {
          /*
          printf("replacing parameter %s\n",i->name);
          printf("old=\n");
          show_expr(0,i->expr);
          printf("new=\n");
          show_expr(0,q->expr);
          */
          i->context=context;
          i->expr=q->expr;
          break;
          }
      if(!i)
        {
        printf("Defparam error: couldn't find parameter to override %s\n",q->name);
        }
      }
  }

void apply_positional(struct param *p,struct param *context,struct param *defparams)
  {
  struct param *q;
  for(q=defparams;q;q=q->next)
    if(p)
      {
      p->expr=q->expr;
      p->context=context;
      p=p->next;
      }
    else
      {
      printf("Too many positional defparams\n");
      break;
      }
  }

// Evaluate parameters

void simp_params(struct param *p)
  {
  while(p)
    {
    p->expr=simp(p->context,p->expr);
    p=p->next;
    }
  }

// Copy and simplify declarations

struct decl *copy_decls(struct param *p,struct decl *d)
  {
  if(d)
    {
    struct decl *nd=malloc(sizeof(struct decl));
    nd->name=d->name;
    nd->top=simp(p,d->top);
    nd->bot=simp(p,d->bot);
    nd->next=copy_decls(p,d->next);
    return nd;
    }
  else
    return 0;
  }

// Elaborate

struct inst *elaborate(struct param *context,char *inst_name,struct param *defparams,struct param *pdp,struct module *m,struct expr *connections,struct inst *mom)
  {
  struct inst *i=malloc(sizeof(struct inst));
  struct inst *j;
  struct inst *last=0;
  i->next=0;
  i->name=strdup(inst_name);
  i->module=strdup(m->name);
  i->mod=m;
  i->insts=0;
  i->connections=connections;
  i->decls=0;
  i->mom=mom;

  // Copy parameters from module (list of params is its own context).
  i->params=copy_params(NULL,m->params);

  // Apply defparams
  apply_defparams(i->params,inst_name,context,defparams);

  // Apply positional defparams
  apply_positional(i->params,context,pdp);

  // Evaluate parameters
  simp_params(i->params);

  // Copy and simplify declarations
  i->decls=copy_decls(i->params,m->decls);

  // Recurse
  for(j=i->mod->insts;j;j=j->next)
    if(j->mod)
      {
      struct inst *q=elaborate(i->params,j->name,m->defparams,j->pos_defparams,j->mod,j->connections,i);
      if(i->insts)
        {
        last->next=q;
        last=q;
        }
      else
        last=i->insts=q;
      }

  return i;
  }

// Show tree

void show_tree(int ind,struct module *m)
  {
  int x;
  struct inst *i;
  struct param *p;
  struct decl *d;
  printf("%s module\n",m->name);
/*
  for(p=m->params;p;p=p->next)
    {
    for(x=0;x!=ind+2;++x) printf(" ");
    printf("parameter %s =\n",p->name);
    show_expr(ind+4,p->expr);
    }
  for(p=m->defparams;p;p=p->next)
    {
    for(x=0;x!=ind+2;++x) printf(" ");
    printf("set parameter %s in %s =\n",p->name,p->inst);
    show_expr(ind+4,p->expr);
    }
*/
  for(i=m->insts;i;i=i->next)
    {
    for(x=0;x!=ind+2;++x) printf(" ");
    printf("inst '%s' a ",i->name);
    if(i->mod)
      show_tree(ind+2,i->mod);
    else
      printf("%s(undefined)\n",i->module);
/*
    show_expr(ind+4,i->connections);
*/
    }
/*
  for(d=m->decls;d;d=d->next)
    {
    for(x=0;x!=ind+2;++x) printf(" ");
    printf("decl '%s'\n",d->name);
    show_expr(ind+4,d->top);
    show_expr(ind+4,d->bot);
    }
*/
  }

// Show design

void show_design(int ind,struct inst *j)
  {
  struct inst *i;
  struct param *p;
  struct decl *d;
  int x;
  printf("inst '%s' (a %s)\n",j->name,j->module);
  for(p=j->params;p;p=p->next)
    {
    for(x=0;x!=ind+2;++x) printf(" ");
    printf("parameter %s = ",p->name);
    show_expr(0,p->expr);
    }
  show_expr(ind+2,j->connections);
  for(i=j->insts;i;i=i->next)
    {
    for(x=0;x!=ind+2;++x) printf(" ");
    show_design(ind+2,i);
    }
  for(d=j->decls;d;d=d->next)
    {
    for(x=0;x!=ind+2;++x) printf(" ");
    printf("decl '%s'\n",d->name);
    show_expr(ind+4,d->top);
    show_expr(ind+4,d->bot);
    }
  }

// Find a declaration

struct decl *find_decl(struct decl *d,char *name)
  {
  while(d)
    if(!strcmp(d->name,name)) return d;
    else d=d->next;
  return 0;
  }

// Find a parameter

struct param *find_param(struct param *p,char *name)
  {
  while(p)
    if(!strcmp(p->name,name)) return p;
    else p=p->next;
  return 0;
  }
