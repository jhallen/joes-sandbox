

#include <stdio.h>
#include <stdlib.h>
#include "scan.h"

int step = 2; // Indent step

int ind=0;	// Current indentation level
int dirty=0;	// Set if we haven't finished line
int want_one=0;	// Always want a space
int text=0;	// Last thing was text
int blk_lvl;	// Set if we're in a block

void statement();
void paren();
void brack_skip();
void brack_skipn();
void paren_noflush();

void indent()
  {
  int x;
  if (ind < 0)
    ind = 0;
  for(x=0;x!=ind;++x)
    putchar(' ');
  }

void flush()
  {
  if(dirty)
    {
    printf("\n");
    }
  dirty=0;
  text=0;
  want_one=0;
  }

void emit_line(char *s)
  {
  flush();
  indent();
  printf("%s\n",s);
  }

void emit_line_n(char *s)
  {
  flush();
  indent();
  printf("%s",s);
  }

void emit_start(char *s)
  {
  flush();
  if (tok->prev && tok->line > tok->prev->line + 1)
    printf("\n");
  indent();
  printf("%s",s);
  dirty=1;
  want_one=1;
  }

int brack;
int brack_flag; // Set after wire, reg, input, output, inout, wor.  Clear after ] or ;

int not_opr()
  {
  /* Skip backwards to first non-whitespace token */
  struct tok_hist *t = tok->prev;
  if (t)
    if (t->tok == 'NAME')
      return 1;
    else if (t->tok == 'NUM')
      return 1;
    else if (t->tok == 'INT')
      return 1;
    else if (t->tok == 'STR')
      return 1;
    else if (t->tok == ')')
      return 1;
    else if (t->tok == ']')
      return 1;
    else if (t->tok == '}')
      return 1;
  return 0;
  }

int isinfix(int c)
  {
  if(c=='<=') return 1;
  else if(c==':' && !brack) return 1;
  else if(c=='%') return 1;
  else if(c=='^' && not_opr()) return 1;
  else if(c=='-' && not_opr()) return 1;
  else if(c=='&' && not_opr()) return 1;
  else if(c=='&&') return 1;
  else if(c=='*' && not_opr()) return 1; /* For always @(*) */
  else if(c=='+') return 1;
  else if(c=='=') return 1;
  else if(c=='>') return 1;
  else if(c=='>>') return 1;
  else if(c=='>=') return 1;
  else if(c=='<') return 1;
  else if(c=='<<') return 1;
  else if(c=='/') return 1;
  else if(c=='?') return 1;
  else if(c=='||') return 1;
  else if(c=='!=') return 1;
  else if(c=='==') return 1;
  else if (c == '|' && not_opr()) return 1;
  else return 0;
  }

void emit_token(int c)
  {
  int infix;
  if (c == ';')
    brack_flag = 0;
  if (c == '[')
    ++brack;
  if (c == ']')
    --brack;
  infix = isinfix(c);
  if (c != '//' || tok->col)
    if(!dirty)
      {
      indent();
      dirty=1;
      }
  if(want_one)
    {
    if(c!='(')
      printf(" ");
    }
  else if(text && c=='NAME') // Need space after keyword
    printf(" ");
  text=0;
  want_one=0;

  if(infix || c=='}') printf(" ");
  if (c == '{' && not_opr()) printf(" "); // For { fred {
  if(c!='NAME' && c!='INT' && c!='NUM' && c!='STR')
    {
    if(c&0xFF000000) printf("%c",255&(c>>24));
    if(c&0x00FF0000) printf("%c",255&(c>>16));
    if(c&0x0000FF00) printf("%c",255&(c>>8));
    if(c&0x000000FF) printf("%c",255&c);
    }
  if(infix || c=='{' || c==',' || (c==']' && brack_flag)) printf(" ");
  if (c==']' && brack_flag)
    brack_flag = 0;

  if(c=='INT')
    printf("%d",tok->len);
  else if(c=='NUM')
    {
    int x;
    printf("%s",tok->str);
    //printf("%d'b",tok->len);
    //for(x=0;x!=tok->len;++x)
    //  printf("%c",tok->str[tok->len-1-x]);
    }
  else if(c=='STR')
    {
    printf("\"%s\"",tok->str);
    }
  else if(c=='NAME')
    {
    printf("%s",tok->str);
    text=1;
    }
  else if(c=='//')
    {
    printf("%s\n",tok->str);
    dirty=0;
    text=0;
    }
  else if(c=='/*')
    {
    printf("%s*/",tok->str);
    }
  }

void emit_rest()
  {
  printf("%s",ptr);
  *ptr=0;
  dirty=0;
  text=0;
  }

// Things which always begin a line:
//-   module (to semi) (eol)
//   `ifdef (one token) (eol)
//   `ifndef (one token) (eol)
//   `else (eol)
//   `endif (eol)
//   `define (to end of line) (eol)
//   `include (one token) (eol)
//   `timescale (to end of line) (eol)
//-   input (to semi) (eol)
//-   output (to semi) (eol)
//-   reg (to semi) (eol)
//-   parameter (to semi) (eol)
//-   assign (to semi) (eol)
//-   if (paren) (eol)
//-   else (eol)
//-   begin (eol)
//-   end (eol)
//-   case (paren) (eol)
//-   endcase (eol)
//-   endmodule (eol)
//-   always (maybe @) (eol)
//
// Things which always end a line:
//   ;
//   ( (only after module)
//   , (only after module and parameter)
//   : (after case)

void eat_comment()
  {
  int c = token();
  if (c == '//' && tok->prev && tok->prev->line == tok->line)
    return;
  else
    unget_tok(tok);
  }

void comments()
  {
  struct tok_hist *h;
  int c;
  while((c=token())!='EOF')
    if(c=='//')
      {
      if (tok->prev && tok->prev->line != tok->line)
        {
        flush();
        if (tok->prev && tok->line > tok->prev->line + 1)
          printf("\n");
        // unget_tok(tok);
        // return;
        }
      if(dirty) printf(" ");
      emit_token(c);
      }
    else if(c=='/*')
      {
      if (tok->prev && tok->prev->line != tok->line)
        {
        flush();
        if (tok->prev && tok->line > tok->prev->line + 1)
          printf("\n");
        // unget_tok(tok);
        // return;
        }
      if(dirty)
        {
        printf(" ");
        emit_token(c);
        printf(" ");
        }
      else
        {
        emit_token(c);
        flush();
        }
      }
    else
      {
      unget_tok(tok);
      break;
      }
  }

void block()
  {
  int c;
  while(comments(), ((c=token())!='EOF'))
    {
    if (c=='NAME' && !strcmp(tok->str,"end"))
      {
      ind-=step;
      emit_line(tok->str);
      eat_comment();
      break;
      }
    else
      {
      unget_tok(tok);
      statement();
      }
    }
  }

void case_block()
  {
  int c;
  while(comments(), ((c=token())!='EOF'))
    if(c=='NAME' && !strcmp(tok->str,"endcase"))
      {
      ind-=step;
      emit_line(tok->str);
      eat_comment();
      //c=token();
      //if(c!='NAME' || strcmp(tok->str,"end") && strcmp(tok->str,"endcase"))
      //  printf("\n");
      //unget_tok(tok);
      return;
      }
    else if(c==':')
      {
      struct tok_hist *sav;
      printf(":");
      // emit_token(c);
      c=token();
      sav = tok;
      while (c == '//' || c == '/*')
        c = token();
      if(c=='NAME' && !strcmp(tok->str,"begin"))
        {
        unget_tok(sav);
        comments();
        flush();
        ind+=step;
        statement();
        ind-=step;
        }
      else
        {
        unget_tok(sav);
        printf(" ");
        statement();
        }
      }
    else
      {
      emit_token(c);
      }
  }

void tosemi()
  {
  int c;
  while((c=token())!='EOF')
    if(c==';')
      {
      emit_token(c);
      comments();
      flush();
      break;
      }
    else
      {
      emit_token(c);
      }
  }

int is_large()
  {
  struct tok_hist *h = 0;
  int count = 0;
  int c;
  while((c=token())!='EOF')
    {
    if (!h) h = tok;
    if(c==';')
      {
      break;
      }
    else if(c=='{')
      {
      unget_tok(tok);
      brack_skipn();
      }
    else if(c==',')
      {
      ++count;
      }
    }
  if (h) unget_tok(h);
  return count;
  }

void tosemi_module()
  {
  int c;
  while((c=token())!='EOF')
    if (c=='#')
      {
      printf(" ");
      emit_token(c);
      paren();
      }
    else if(c=='(' || c==';')
      break;
    else
      emit_token(c);

  flush();
  ind=step;

  if(c=='(')
    {
    emit_token(c);
    comments();
    flush();
    while((c=token())!='EOF')
      if(c==',')
        {
        printf(",");
        comments();
        flush();
        }
      else if(c==')')
        {
        flush();
        emit_token(c);
        }
      else if(c==';')
        {
        emit_token(c);
        ind = 0;
        comments();
        flush();
        break;
        }
      else
        {
        emit_token(c);
        }
    }
  else if(c==';')
    {
    emit_token(c);
    ind = 0;
    comments();
    flush();
    }
  ind=0;
  }

void tocomma_inst()
  {
  int c;
  while((c=token())!='EOF')
    if(c==',')
      {
      unget_tok(tok);
      break;
      }
    else if (c == '{')
      {
      unget_tok(tok);
      brack_skip();
      }
    else if (c == '(')
      {
      unget_tok(tok);
      printf(" ");
      paren_noflush();
      }
    else if(c==')')
      {
      unget_tok(tok);
      break;
      }
    else if(c==';')
      {
      unget_tok(tok);
      break;
      }
    else
      {
      emit_token(c);
      }
  }

/* Find positional parameter */

extern FILE *infile;

char *find_it(char *name, int nth)
  {
  char n[1024];
  struct scanner_state *ss = save_scanner_state();
  sprintf(n,"%s.v",name);
  if (!source_push(n, 1))
    {
    int c;
    // Look for module
    do
      c = token();
      while (!(c == 'EOF' || c == 'NAME' && !strcmp(tok->str, "module")));
    if (c == 'NAME')
      {
      // Skip to (
      while((c=token())!='EOF')
        if(c=='(' || c==';')
          break;
      // Find nth name
      while ((c=token()) != 'EOF')
        if (c == ';')
          break;
        else if (c == 'NAME')
          {
          if (!nth--)
            {
            char *s = strdup(tok->str);
            fclose(infile);
            restore_scanner_state(ss);
            return s;
            }
          }
      }
    fclose(infile);
    }
  restore_scanner_state(ss);
  return 0;
  }

void tosemi_inst(char *name)
  {
  int nth = 0;
  int c;
  while((c=token())!='EOF')
    if (c=='#')
      {
      emit_token(c);
      paren_noflush();
      printf(" ");
      }
    else if(c=='(' || c==';')
      break;
    else
      emit_token(c);

  flush();
  ind=step;

  if(c=='(')
    {
    emit_token(c);
    comments();
    flush();
    while((c=token())!='EOF')
      if(c==',')
        {
        printf(",");
        comments();
        flush();
        ++nth;
        }
      else if (c == '//' || c == '/*')
        {
        emit_token(c);
        }
      else if (c == '.')
        { /* Oh good... */
        emit_token(c);
        tocomma_inst();
        }
      else if (c != ')' && c != ';')
        { /* Who wrote this code? */
        struct tok_hist *h = tok;
        char *s;
        if (s = find_it(name, nth))
          {
          emit_token('.');
          printf("%s ",s);
          emit_token('(');
          unget_tok(tok);
          // emit_token(c);
          tocomma_inst();
          emit_token(')');
          }
        else
          { /* Oh well */
          emit_token(c);
          tocomma_inst();
          }
        }
      else if(c==')')
        {
        flush();
        emit_token(c);
        }
      else if(c==';')
        {
        emit_token(c);
        ind = 0;
        comments();
        flush();
        break;
        }
    }
  else if(c==';')
    {
    emit_token(c);
    ind=0;
    comments();
    flush();
    }
  ind=0;
  }

void tosemi_param()
  {
  int c;

  flush();
  ind=step;

  while((c=token())!='EOF')
    if(c==',')
      {
      printf(",");
      comments();
      flush();
      }
    else if(c==';')
      {
      emit_token(c);
      ind = 0;
      comments();
      flush();
      break;
      }
    else
      {
      emit_token(c);
      }

  ind=0;
  }

void paren()
  {
  int c;
  int l=0;
  while((c=token())!='EOF')
    if(c=='(')
      {
      emit_token(c);
      ++l;
      }
    else if(c==')')
      {
      emit_token(c);
      if(!--l)
        {
        comments();
        flush();
        break;
        }
      }
    else
      emit_token(c);
  }

void brack_skip()
  {
  int c;
  int l=0;
  while((c=token())!='EOF')
    if(c=='{')
      {
      emit_token(c);
      ++l;
      }
    else if(c=='(')
      {
      unget_tok(tok);
      paren_noflush();
      }
    else if(c=='}')
      {
      emit_token(c);
      if(!--l)
        {
        break;
        }
      }
    else
      emit_token(c);
  }

void brack_skipn()
  {
  int c;
  int l=0;
  while((c=token())!='EOF')
    if(c=='{')
      {
      ++l;
      }
    else if(c=='}')
      {
      if(!--l)
        {
        break;
        }
      }
  }

void paren_noflush()
  {
  int c;
  int l=0;
  while((c=token())!='EOF')
    if(c=='(')
      {
      emit_token(c);
      ++l;
      }
    else if (c=='{')
      {
      unget_tok(tok);
      brack_skip();
      }
    else if(c==')')
      {
      emit_token(c);
      if(!--l)
        {
        break;
        }
      }
    else
      emit_token(c);
  }

void maybe_sens()
  {
  int c;
  comments();
  c=token();
  if(c=='@')
    {
    emit_token(c);
    paren();
    }
  else
    {
    unget_tok(tok);
    }
  }

void statement()
  {
  int c;
  comments();
  c=token();
  if(c=='EOF') return;
  if(c=='NAME' && (!strcmp(tok->str,"if")))
    {
    emit_start(tok->str);
    again:
    printf(" ");
    paren();
    ind+=step;
    statement();
    ind-=step;
    comments();
    c=token();
    if(c=='NAME' && (!strcmp(tok->str,"else")))
      {
      c=token();
      if (c=='NAME' && (!strcmp(tok->str,"if")))
        {
        emit_start("else if");
        goto again;
        }
      else
        {
        unget_tok(tok);
        }
      emit_line("else");
      ind+=step;
      statement();
      ind-=step;
      }
    else
      unget_tok(tok);
    }
  else if(c=='NAME' && (!strcmp(tok->str,"begin")))
    {
    struct tok_hist *sav;
    emit_line_n(tok->str);
    c=token();
    sav = tok;
    if (c==':')
      {
      c=token();
      if (c=='NAME')
        printf(": %s",tok->str);
      else
        unget_tok(sav);
      }
    else
      {
      unget_tok(sav);
      }
    printf("\n");
    ind+=step;
    block();
    }
  else if(c=='NAME' && (!strcmp(tok->str,"case") || !strcmp(tok->str,"casex") || !strcmp(tok->str,"casez")))
    {
    flush();
    // printf("\n");
    emit_start(tok->str);
    printf(" ");
    paren();
    ind+=step;
    case_block();
    }
  else
    {
    unget_tok(tok);
    tosemi();
    }
  }

/* Keywords */

/* Return:
     Bit 0: It's like always, initial
     Bit 1: It's like wire, reg
*/

int kw(char *s)
  {
  if (!strcmp(s, "wire")) return 2;
  else if (!strcmp(s, "reg")) return 2;
  else if (!strcmp(s, "wor")) return 2;
  else if (!strcmp(s, "wand")) return 2;
  else if (!strcmp(s, "input")) return 2;
  else if (!strcmp(s, "output")) return 2;
  else if (!strcmp(s, "inout")) return 2;

  else if (!strcmp(s, "always")) return 1;
  else if (!strcmp(s, "initial")) return 1;

  else if (!strcmp(s, "assign")) return 4;
  else if (!strcmp(s, "defparam")) return 4;
  else if (!strcmp(s, "parameter")) return 4;
  }

void pretty()
  {
  int c;
  while((c=token())!='EOF')
    {
    if (c=='NAME' && (!strcmp(tok->str, "wire") || !strcmp(tok->str, "reg") || !strcmp(tok->str, "wor") || !strcmp(tok->str, "wand") ||
                      !strcmp(tok->str, "input") || !strcmp(tok->str, "output") || !strcmp(tok->str, "inout")))
      brack_flag = 1;
    if(c=='NAME' && (!strcmp(tok->str,"always") || !strcmp(tok->str,"initial")))
      {
      flush();
      emit_start(tok->str);
      maybe_sens();
      ind+=step;
      statement();
      ind-=step;
      }
    else if(c=='NAME' && (!strcmp(tok->str,"endmodule")))
      {
      flush();
      ind=0;
      emit_line(tok->str);
      eat_comment();
      }
    else if(c=='`')
      {
      int sav = ind;
      flush();
      ind=0;
      emit_token(c);
      emit_rest();
      ind = sav;
      }
    else if(c=='NAME' && !strcmp(tok->str,"module"))
      {
      flush();
      ind=0;
      emit_start(tok->str);
      tosemi_module();
      }
    else if(c=='NAME' && !strcmp(tok->str,"parameter"))
      {
      flush();
      ind=0;
      emit_start(tok->str);
      if (is_large())
        tosemi_param();
      else
        tosemi();
      }
    else if(c=='NAME' && (!strcmp(tok->str,"wire") || !strcmp(tok->str,"assign") || !strcmp(tok->str,"wor") || !strcmp(tok->str,"wand") ||
                          !strcmp(tok->str,"input") || !strcmp(tok->str,"output") || !strcmp(tok->str,"inout") || !strcmp(tok->str,"reg")))
      {
      flush();
      ind=0;
      emit_start(tok->str);
      tosemi();
      }
    else if(c=='NAME')
      { /* Must be an instantiation */
      char *name;
      name = tok->str;
      flush();
      ind=0;
      emit_start(tok->str);
      if (is_large() > 3)
        tosemi_inst(name);
      else
        tosemi();
      }
    else
      {
      emit_token(c);
      flush();
      }
    }
  }
