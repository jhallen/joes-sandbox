/* Parse verilog token stream
 *
 * Joe Allen,  Sep 2004
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scan.h"
#include "parse.h"
#include "tree.h"
#include "database.h"

// Parse an expression

struct expr *parse_expr(int prec)
  {
  struct expr *f;
  struct expr *e;
  int t=token();
  int t_prec;
  if(t=='NAME' || t=='NUM' || t=='STR' || t=='FLT' || t=='INT')
    e=cons_copy(t);
  else if(t=='(')
    {
    e=parse_expr(0);
    t=token();
    if(t!=')')
      {
      error(tok_file,tok_line,"Missing )");
      unget_tok(t);
      }
    }
  else if(t=='{')
    {
    e=cons(t);
    e->r=parse_expr(0);
    t=token();
    if(t!='}')
      {
      error(tok_file,tok_line,"Missing }");
      unget_tok(t);
      }
    }
  else if(t=='!' || t=='~' || t=='-' || t=='&' || t=='|' || t=='^')
    {
    e=cons(t);
    e->r=parse_expr(100);
    }
  else if(t=='.')
    { // .name(expr)
    char *a, *b, *c;
    e=cons(t);
    t=token();
    if(t=='NAME')
      e->l=cons_copy(t);
    else
      {
      error(tok_line,tok_line,"Expected identifier after .");
      unget_tok(t);
      }
    t=token();
    if(t!='(')
      {
      error(tok_file,tok_line,"Expected ( after .");
      unget_tok(t);
      }
    e->r=parse_expr(0);
    t=token();
    if(t!=')')
      {
      error(tok_file,tok_line,"Missing )");
      unget_tok(t);
      }
    e->rest_of_line=rest_of_line();
    }
  else
    {
    unget_tok(t);
    return 0;
    }

  loop:

  f=e;

  t=token();

  if((t==',' || t==':') && prec<10)
    {
    e=cons(t);
    e->l=f;
    e->r=parse_expr(10);
    goto loop;
    }
  else if(t=='=' && prec<15)
    {
    e=cons(t);
    e->l=f;
    e->r=parse_expr(15);
    goto loop;
    }
  else if(t=='?' && prec<=20)
    {
    e=cons(t);
    e->l=f;
    e->r=parse_expr(20);
    t=token();
    if(t!=':')
      error(tok_file,tok_line,"Missing : after ?");
    e->rr=parse_expr(20);
    goto loop;
    }
  else if((t=='||' || t=='&&' || t=='|' || t=='^' || t=='&') && prec<30)
    {
    e=cons(t);
    e->l=f;
    e->r=parse_expr(30);
    goto loop;
    }
  else if((t=='==' || t=='!=' || t=='===' || t=='!==' || t=='<' || t=='<=' || t=='>' || t=='>=') && prec<40)
    {
    e=cons(t);
    e->l=f;
    e->r=parse_expr(40);
    goto loop;
    }
  else if((t=='<<' || t=='>>' || t=='+' || t=='-') && prec<50)
    {
    e=cons(t);
    e->l=f;
    e->r=parse_expr(50);
    goto loop;
    }
  else if((t=='*' || t=='/' || t=='%') && prec<60)
    {
    e=cons(t);
    e->l=f;
    e->r=parse_expr(60);
    goto loop;
    }
  else if(t=='[' && prec<70)
    {
    e=cons(t);
    e->l=f;
    e->r=parse_expr(0);
    t=token();
    if(t!=']')
      error(tok_file,tok_line,"Missing ] after [");
    goto loop;
    }
  else if(t=='{' && prec<70)
    {
    e=cons(t);
    e->l=f;
    e->r=parse_expr(0);
    t=token();
    if(t!='}')
      error(tok_file,tok_line,"Missing } after {");
    goto loop;
    }
  else if(t=='(' && prec<70)
    {
    e=cons(t);
    e->l=f;
    e->r=parse_expr(0);
    t=token();
    if(t!=')')
      error(tok_file,tok_line,"Missing ) after (");
    goto loop;
    }
  else if(t=='.')
    {
    e=cons(t);
    e->l=f;
    e->r=parse_expr(80);
    goto loop;
    }

  unget_tok(t);

  return e;
  }

// Skip to next )

void skip_parens()
  {
  int t;
  for(t=token();t!='EOF';t=token())
    if(t==')')
      return;
    else if(t=='(')
      skip_parens();
  error(tok_file,tok_line,"Missing )");
  }

// Skip to next ]

void skip_square()
  {
  int t;
  for(t=token();t!='EOF';t=token())
    if(t==']')
      return;
    else if(t=='[')
      skip_square();
  error(tok_file,tok_line,"Missing ]");
  }

// Parse a begin block

void parse_block(int n)
  {
  int t;
  for(t=token();t!='EOF';t=token())
    if(t=='NAME' && !strcmp(tok_str,"end"))
      return;
    else if(t=='NAME' && !strcmp(tok_str,"begin"))
      parse_block(n+1);
  error(tok_file,tok_line,"Missing end");
  }

// Parse a case block

void parse_case()
  {
  int t;
  for(t=token();t!='EOF';t=token())
    if(t=='NAME' && !strcmp(tok_str,"endcase"))
      return;
    else if(t=='NAME' && (!strcmp(tok_str,"case") || !strcmp(tok_str,"casex") || !strcmp(tok_str,"casez")))
      parse_case();
  error(tok_file,tok_line,"Missing endcase");
  }

// Parse a single statement

void parse_stmnt()
  {
  int t;

  again:

  t=token();
  if(t=='NAME' && !strcmp(tok_str,"begin"))
    {
    parse_block(1);
    return;
    }
  if(t=='NAME' && !strcmp(tok_str,"for"))
    { // Have to parse this because for has ;s in it
    t=token();
    if(t=='(')
      {
      skip_parens();
      parse_stmnt();
      }
    else
      error(tok_file,tok_line,"Missing ( after for");
    return;
    }
  else if(t=='NAME' && !strcmp(tok_str,"if"))
    { // Have to check for else
    t=token();
    if(t=='(')
      {
      skip_parens();
      parse_stmnt();
      /* Check for else here */
      t=token();
      if(t=='NAME' && !strcmp(tok_str,"else"))
        parse_stmnt();
      else
        unget_tok(t);
      }
    else
      error(tok_file,tok_line,"Missing (");
    }
  else if(t=='NAME' && (!strcmp(tok_str,"case") || !strcmp(tok_str,"casex") || !strcmp(tok_str,"casez")))
    { // Check to endcase
    parse_case();
    return;
    }
  else if(t==';')
    return;
  else
    { /* Probably an assignment */
    do
      {
      again1:
      t=token();
      if(t=='NAME' && !strcmp(tok_str,"begin"))
        {
        parse_block(1);
        goto again1;
        }
      } while(t!=';' && t!='EOF');
    return;
    }
  }

// Convert parse tree into parameters

void add_params(struct expr *e)
  {
  if(!e) return;
  else if(e->t==',')
    {
    add_params(e->l);
    add_params(e->r);
    }
  else if(e->t=='=' && e->l->t=='NAME')
    {
    struct param *p=malloc(sizeof(struct param));
    p->next=0;
    if(modules->params)
      {
      modules->params_last->next=p;
      modules->params_last=p;
      }
    else
      {
      modules->params_last=modules->params=p;
      }
    p->inst=0;
    p->name=e->l->str;
    p->expr=e->r;
    p->context=0;
    }
  else
    error(tok_file,tok_line,"Bad parameters");
  }

// Convert parse tree into defparams

void add_defparams(struct expr *e)
  {
  if(!e) return;
  else if(e->t==',')
    {
    add_defparams(e->l);
    add_defparams(e->r);
    }
  else if(e->t=='=' && e->l->t=='.' && e->l->l->t=='NAME' && e->l->r->t=='NAME')
    {
    struct param *p=malloc(sizeof(struct param));
    p->next=0;
    if(modules->defparams)
      {
      modules->defparams_last->next=p;
      modules->defparams_last=p;
      }
    else
      {
      modules->defparams_last=modules->defparams=p;
      }
    p->inst=e->l->l->str;
    p->name=e->l->r->str;
    p->expr=e->r;
    p->context=0;
    }
  else
    error(tok_file,tok_line,"Bad defparam");
  }

// Convert parse tree ".name(expr), .name(expr),..." into defparams

void add_defparams1(struct inst *inst,struct expr *e)
  {
  if(!e) return;
  else if(e->t==',')
    {
    add_defparams1(inst,e->l);
    add_defparams1(inst,e->r);
    }
  else if(e->t=='.' && e->l->t=='NAME')
    {
    struct param *p=malloc(sizeof(struct param));
    p->next=0;
    if(modules->defparams)
      {
      modules->defparams_last->next=p;
      modules->defparams_last=p;
      }
    else
      {
      modules->defparams_last=modules->defparams=p;
      }
    p->inst=inst->name;
    p->name=e->l->str;
    p->expr=e->r;
    p->context=0;
    }
  else
    { // Positional defparam
    struct param *p=malloc(sizeof(struct param));
    p->next=0;
    if(inst->pos_defparams)
      {
      inst->pos_last->next=p;
      inst->pos_last=p;
      }
    else
      {
      inst->pos_last=inst->pos_defparams=p;
      }
    p->inst=0;
    p->name=0;
    p->expr=e;
    p->context=0;
    }
  }

// Parse defparams in this format: ".name(expr), .name(expr), ... )"

struct expr *parse_params()
  {
  struct expr *p=parse_expr(0);
  int t=token();
  if(t!=')')
    {
    error(tok_file,tok_line,"Missing )");
    unget_tok(t);
    }
  return p;
  }

// Add declaration list to module

void add_decls(struct expr *e,struct expr *top,struct expr *bot)
  {
  if(e)
    if(e->t==',')
      {
      add_decls(e->l,top,bot);
      add_decls(e->r,top,bot);
      }
    else if(e->t=='[' || e->t=='=')
      { // for a=expr and a[]
      add_decls(e->l,top,bot);
      }
    else if(e->t=='NAME')
      {
      struct decl *d=malloc(sizeof(struct decl));
      d->next=modules->decls;
      modules->decls=d;
      d->name=strdup(e->str);
      d->top=top;
      d->bot=bot;
      }
    else
      {
      error(tok_file,tok_line,"Unknown thing in declaration list");
      show_expr(0,e);
      }
  }

// Parse module body (after the module ... ;)

void parse_module()
  {
  int t;

  again:

  t=token();

  if(t=='EOF')
    {
    error(tok_file,tok_line,"File ended before 'endmodule'");
    return;
    }
  else if(t=='NAME')
    {
    if(!strcmp(tok_str,"assign") || !strcmp(tok_str,"integer") || !strcmp(tok_str,"real") || !strcmp(tok_str,"function") ||

       /* primitives: gates */
       !strcmp(tok_str,"and") || !strcmp(tok_str,"nand") || !strcmp(tok_str,"or") || !strcmp(tok_str,"nor") ||
       !strcmp(tok_str,"xor") || !strcmp(tok_str,"xnor") || !strcmp(tok_str,"buf") || !strcmp(tok_str,"not") ||
       /* tristate drivers */
       !strcmp(tok_str,"bufif0") || !strcmp(tok_str,"bufif1") || !strcmp(tok_str,"notif0") || !strcmp(tok_str,"notif1") ||
       /* MOS switches */
       !strcmp(tok_str,"nmos") || !strcmp(tok_str,"pmos") || !strcmp(tok_str,"cmos") || !strcmp(tok_str,"rnmos") ||
       !strcmp(tok_str,"rpmos") || !strcmp(tok_str,"rcmos") ||
       /* bidirectional switches */
       !strcmp(tok_str,"tran") || !strcmp(tok_str,"tranif0") || !strcmp(tok_str,"tranif1") ||
       /* resistive bidirectional switches */
       !strcmp(tok_str,"rtran") || !strcmp(tok_str,"rtranif0") || !strcmp(tok_str,"rtranif1") ||
       /* pullups */
       !strcmp(tok_str,"pullup") || !strcmp(tok_str,"pulldown"))
      { /* Skip to next ; */
      do
        t=token();
        while(t!=';' && t!='EOF');
      goto again;
      }
    else if(!strcmp(tok_str,"inout") || !strcmp(tok_str,"output") || !strcmp(tok_str,"input") || !strcmp(tok_str,"reg") ||
            !strcmp(tok_str,"wire") || !strcmp(tok_str,"wand") || !strcmp(tok_str,"wor") || !strcmp(tok_str,"tri") ||
            !strcmp(tok_str,"triand") || !strcmp(tok_str,"trior") || !strcmp(tok_str,"tri0") || !strcmp(tok_str,"tri1") ||
            !strcmp(tok_str,"supply0") || !strcmp(tok_str,"supply1") || !strcmp(tok_str,"trireg"))
      { // a declaration
      struct expr *top=0;
      struct expr *bot=0;
      struct expr *e;

      t=token();
      if(t=='[')
        {
        e=parse_expr(0);
        t=token();
        if(t!=']')
          {
          error(tok_file,tok_line,"Missing ]");
          unget_tok(t);
          }
        if(e->t==':')
          {
          top=e->l;
          bot=e->r;
          }
        else
          error(tok_file,tok_line,"Bad bus declaration");
        }

      // Get declaration list
      e=parse_expr(0);

      // Add it to database
      add_decls(e,top,bot);

      do
        t=token();
        while(t!=';' && t!='EOF');
      goto again;
      }
    else if(!strcmp(tok_str,"parameter"))
      {
      struct expr *e;
      t=token();
      if(t=='[')
        skip_square();
      else
        unget_tok(t);
      e=parse_expr(0);
      t=token();
      if(t!=';')
        {
        error(tok_file,tok_line,"Missing ;");
        show_tok(t);
        unget_tok(t);
        }
      add_params(e);
      goto again;
      }
    else if(!strcmp(tok_str,"defparam"))
      {
      struct expr *e=parse_expr(0);
      t=token();
      if(t!=';')
        {
        error(tok_file,tok_line,"Missing ;");
        unget_tok(t);
        }
      add_defparams(e);
      goto again;
      }
    else if(!strcmp(tok_str,"always"))
      {
      t=token();
      if(t=='@')
        {
        t=token();
        if(t=='(')
          {
          skip_parens();
          }
        else
          {
          error(tok_file,tok_line,"Expected (");
          }
        parse_stmnt();
        }
      else
        {
        error(tok_file,tok_line,"Expected @ here");
        show_tok(t);
        }
      goto again;
      }
    else if(!strcmp(tok_str,"initial"))
      {
      parse_stmnt();
      goto again;
      }
    else if(!strcmp(tok_str,"begin"))
      { /* For function bodies */
      parse_block(1);
      goto again;
      }
    else if(!strcmp(tok_str,"endfunction"))
      {
      goto again;
      }
    else if(!strcmp(tok_str,"endmodule"))
      {
      return;
      }
    else
      { // This should be a module instantiation
      char *mod=strdup(tok_str);
      struct expr *p=0;
      struct inst *i=0;
      t=token();
      if(t=='#')
        {
        t=token();
        if(t=='(')
          {
          p=parse_params();
          t=token();
          }
        else
          {
          error(tok_file,tok_line,"Missing (");
          }
        }
      if(t=='NAME')
        { // Found an instance
        i=add_inst(strdup(tok_str),mod);
        if(p)
          add_defparams1(i,p);
        t=token();
        if(t=='(')
          { // Port connections
          struct expr *e=parse_expr(0);
          t=token();
          if(t!=')')
            {
            error(tok_file,tok_line,"Missing )");
            show_tok(t);
            }
          else
            t=token();
          i->connections=e;
          }
        }
      else
        {
        error(tok_file,tok_line,"Missing instance name");
        }
      // Skip to ;
      while(t!='EOF' && t!=';') t=token();
      goto again;
      }
    }
  else
    {
    error(tok_file,tok_line,"Unknown token?");
    show_tok(t);
    }
  }

// A file is made up of modules

void parse()
  {
  int t;
  again:
  t=token();
  if(t=='EOF')
    {
    return;
    }
  else if(t=='NAME' && !strcmp(tok_str,"module"))
    {
    struct module *m;
    t=token();
    if(t=='#')
      {
      t=token();
      if(t=='(')
        {
        skip_parens();
        t=token();
        }
      else
        {
        error(tok_file,tok_line,"Expected (");
        }
      }
    if(t!='NAME')
      {
      error(tok_file,tok_line,"Expected module name here");
      show_tok(t);
      }
    m=add_module(strdup(tok_str));
    do
      t=token();
      while(t!=';' && t!='EOF');
    parse_module();
    goto again;
    }
  else if(t=='NAME' && !strcmp(tok_str,"parameter"))
    { // This is not legal, but happens frequently
    do
      t=token();
      while(t!=';' && t!='EOF');
    goto again;
    }
  else
    {
    error(tok_file,tok_line,"Unexpected token, looking for 'module' here");
    show_tok(t);
    goto again;
    }
  }
