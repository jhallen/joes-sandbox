/* Tokenize verilog
 *
 * Joe Allen,  Sep 2004
 */


/* Todo:
 *
 * \ continuation lines
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scan.h"

char tok_file[];		// File it was found in
int tok_line;			// File line number
char tok_str[];		// Strings, identifiers, and numbers
int tok_len;			// String length, or number size

struct path
  {
  struct path *next;
  char buf[1024];
  } *inc_paths;

char *(*getmore)()=0;	/* Function to get another line of input */

char *ptr="";		/* Input pointer */
static int col=0;		/* Column no. 'ptr' is at.  0 is first */
static int lvl= -1;		/* Indentation level of current line */
static int line=0;		/* Line number 'ptr' is on.  0 is first */
static char *name="stdin";	/* File name of current input file */
static int eof=0;		/* Set when EOF encountered */

FILE *infile;		/* Input file/buffer for 'include' command */
char inbuf[4096];

/* Print an error message */

int error_flag;

void error(name,line,str,a,b,c,d)
char *name, *str;
  {
  error_flag = 1;
  fprintf(stdout,"\"%s\" %d: error: ",name,line);
  fprintf(stdout,str,a,b,c,d);
  fprintf(stdout,"\n");
  }

static char *morefile() { return fgets(inbuf,4095,infile); }

/* Stack of source files or macros */

struct source
  {
  struct source *next;
  FILE *infile;
  char *ptr;
  int col;
  int lvl;
  int line;
  char *name;
  char inbuf[4096];
  } *source_stack;

int source_push(char *new_name, int no_error)
  {
  char buf[4096];
  FILE *f;
  struct path *p;
  
  /* Find file (look in include path */
  f=fopen(new_name,"r");
  if(!f)
    for(p=inc_paths;p;p=p->next)
      {
      sprintf(buf,"%s/%s",p->buf,new_name);
      f=fopen(buf,"r");
      if(f)
        break;
      }

  if(f)
    {
    struct source *src=malloc(sizeof(struct source));
    src->infile=infile;
    src->ptr=ptr;
    src->col=col;
    src->lvl=lvl;
    src->line=line;
    src->name=name;
    memcpy(src->inbuf,inbuf,4096);
    src->next=source_stack;
    source_stack=src;

    eof=0;
    infile=f;
    ptr=inbuf;
    col=0;
    lvl= -1;
    line=1;
    name=new_name;
    inbuf[0]=0;
    fgets(inbuf,4095,infile);
    getmore=morefile;
    return 0;
    }
  else
    {
    if (!no_error)
      error(name,line,"Couldn't open file '%s'",new_name);
    return -1;
    }
  }

void macro_push(char *new_name,char *s)
  {
  struct source *src=malloc(sizeof(struct source));
  src->infile=infile;
  src->ptr=ptr;
  src->col=col;
  src->lvl=lvl;
  src->line=line;
  src->name=name;
  memcpy(src->inbuf,inbuf,4096);
  src->next=source_stack;
  source_stack=src;

  infile=0;
  ptr=inbuf;
  col=0;
  lvl= -1;
  line=1;
  name=new_name;
  strcpy(inbuf,s);
  getmore=0;
  }

void source_pop()
  {
  struct source *src=source_stack;
  source_stack=src->next;
  ptr=src->ptr;
  col=src->col;
  lvl=src->lvl;
  line=src->line;
  name=src->name;
  infile=src->infile;
  getmore=morefile;
  memcpy(inbuf,src->inbuf,4096);
  }

/* Skip over whitespace.  Don't attempt to get more input */

void skipws(void)
  {
  for(;;) switch(*ptr)
    {
    case  ' ':
      ++ptr; ++col;
      break;

    case '\t':
      ++ptr; col+=8-col%8;
      break;

    case '\n':
    case '\r':
      *ptr=0;
    case 0:
      return;

    case '/':
      if(ptr[1]=='/')
        { /* C++ comment */
        *ptr=0;
        return;
        }
      else if(ptr[1]=='*')
        { /* C comment */
        int first=line;
        ptr+=2; col+=2;
        while(!(ptr[0]=='*' && ptr[1]=='/'))
          {
          if(*ptr=='\t')
            {
            ++ptr; col+=8-col%8;
            }
          else if(*ptr)
            {
            ++ptr; ++col;
            }
          else if(infile && getmore && !eof)
            if(ptr=getmore())
              {
              col=0;
              lvl= -1;
              ++line;
              }
            else
              {
              ptr="";
              eof=1;
              }
          else
            {
            error(name,line,"Unterminated comment (began on line %d)",first);
            return;
            }
          }
        ptr+=2;
        col+=2;
        break;
        }

    default:
      if(lvl== -1)
        lvl=col;
      return;
    }
  }

/* Skip whitespace; get new lines if needed */

void skipwsm(void)
  {
  loop:
  skipws();
  if(!*ptr)
    if(getmore && infile && !eof && (ptr=getmore()))
      {
      col=0;
      lvl= -1;
      ++line;
      goto loop;
      }
    else if(source_stack)
      {
      source_pop();
      goto loop;
      }
    else
      {
      eof=1, ptr="";
      }
  }

/* Fix a number */

void fix_num(int num)
  {
  int c=tok_str[0];
  int x;
  if(c=='1') c='0';
  // Truncate check
  if(num<tok_len)
    {
    for(x=0;x!=tok_len-num;++x)
      if(tok_str[x]=='1')
        {
        error(tok_file,tok_line,"Number of digits exceeds number size");
        break;
        }
    }
  // Reverse
  for(x=0;x!=tok_len/2;++x)
    {
    int d=tok_str[x];
    tok_str[x]=tok_str[tok_len-1-x];
    tok_str[tok_len-1-x]=d;
    }
  if(num>tok_len)
    { // Extend
    for(x=tok_len;x!=num;++x)
      tok_str[x]=c;
    }
  tok_len=num;
  tok_str[tok_len]=0;
  }

/* Get next token */

char tok_file[1024];
int tok_line;		/* Line token was found on */
char tok_str[4096];	/* String or identifier */
int tok_len;		/* Length */
int tok_col;

int token_raw()
  {
  for(;;)
    {
    tok_line = line;
    tok_col = col;
    strcpy(tok_file,name);

    switch(*ptr)
      {
      case 0:
        {
        if(getmore && infile && !eof && (ptr=getmore()))
          {
          col=0;
          lvl= -1;
          ++line;
          break;
          }
        else if(source_stack)
          {
          source_pop();
          break;
          }
        else
          {
          eof=1, ptr="";
          return 'EOF';
          }
        }

      case ' ':
        {
        ++col; ++ptr;
        break;
        }

      case '\t':
        {
        ++ptr; col+=8-col%8;
        break;
        }

      case '\r': case '\n':
        {
        *ptr=0;
        break;
        }

      case '"':
        { /* String */
        int x;
        ++col; ++ptr;
        tok_len=0;
        while(*ptr && *ptr!='"')
          if(*ptr=='\\')
            {
            ++ptr; ++col;
            if(*ptr=='\\')
              {
              ++ptr; ++col;
              tok_str[tok_len++]='\\';
              }
            else if(*ptr=='n')
              {
              ++ptr; ++col;
              tok_str[tok_len++]='\n';
              }
            else if(*ptr=='t')
              {
              ++ptr; ++col;
              tok_str[tok_len++]='\t';
              }
            else if(*ptr=='"')
              {
              ++ptr; ++col;
              tok_str[tok_len++]='"';
              }
            else if(*ptr>='0' && *ptr<='7')
              {
              int n=*ptr++-'0';
              ++col;
              if(*ptr>='0' && *ptr<='7')
                {
                n=n*8+*ptr++-'0';
                ++col;
                }
              if(*ptr>='0' && *ptr<='7')
                {
                n=n*8+*ptr++-'0';
                ++col;
                }
              tok_str[tok_len++]=n;
              }
            else
              {
              error(name,line,"Unknown string escape sequence");
              }
            }
          else
            {
            tok_str[tok_len++]= *ptr++;
            ++col;
            }
        if(*ptr=='"')
          {
          ++ptr;
          ++col;
          }
        else
          error(name,line,"Unterminated string constant");
        tok_str[tok_len]=0;
        return 'STR';
        }

      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
      case '\'':
        { /* Number */
        int first=0;
        tok_len=0;
        if(*ptr!='\'')
          {
          tok_str[tok_len++] = *ptr;
          first= *ptr++-'0'; ++col;
          while(*ptr>='0' && *ptr<='9')
            {
            tok_str[tok_len++] = *ptr;
            first=first*10+*ptr++-'0';
            ++col;
            }
          }
        else
          {
          first=32;
          }
        if(*ptr=='\'')
          { /* Sized number */
          tok_str[tok_len++] = *ptr;
          ++ptr; ++col;
          if(*ptr=='h' || *ptr=='H')
            {
            tok_str[tok_len++] = *ptr;
            ++ptr; ++col;
            while((*ptr>='0' && *ptr<='9') || (*ptr>='a' && *ptr<='f') || (*ptr>='A' && *ptr<='F') ||
                  *ptr=='_' || *ptr=='z' || *ptr=='Z' || *ptr=='x' || *ptr=='X' || *ptr=='?')
              {
              tok_str[tok_len++] = *ptr;
              ++ptr; ++col;
              }
            tok_str[tok_len]=0;
            return 'NUM';
            }
          else if(*ptr=='d' || *ptr=='D')
            { /* Yuk... (single z, x, ? allowed.  Otherwise only a decimal number) */
            tok_str[tok_len++] = *ptr;
            ++ptr; ++col;
            while((*ptr>='0' && *ptr<='9') || *ptr=='_' || *ptr=='z' || *ptr=='Z' || *ptr=='x' || *ptr=='X' || *ptr=='?')
              {
              tok_str[tok_len++] = *ptr;
              ++ptr; ++col;
              }
            tok_str[tok_len]=0;
            return 'NUM';
            }
          else if(*ptr=='o' || *ptr=='O')
            {
            tok_str[tok_len++] = *ptr;
            ++ptr; ++col;
            while((*ptr>='0' && *ptr<='7') || *ptr=='_' || *ptr=='z' || *ptr=='Z' || *ptr=='x' || *ptr=='X' || *ptr=='?')
              {
              tok_str[tok_len++] = *ptr;
              ++ptr; ++col;
              }
            tok_str[tok_len]=0;
            return 'NUM';
            }
          else if(*ptr=='b' || *ptr=='B')
            {
            tok_str[tok_len++] = *ptr;
            ++ptr; ++col;
            while(*ptr=='0' || *ptr=='1' || *ptr=='_' || *ptr=='z' || *ptr=='Z' || *ptr=='x' || *ptr=='X' || *ptr=='?')
              {
              tok_str[tok_len++] = *ptr;
              ++ptr; ++col;
              }
            tok_str[tok_len]=0;
            return 'NUM';
            }
          else
            {
            error(name,line,"Bad number");
            tok_str[tok_len]=0;
            fix_num(first);
            return 'NUM';
            }
          }
        else if(*ptr=='.' || *ptr=='e' || *ptr=='E')
          { /* Floating point */
          do_float:
          if(*ptr=='.')
            {
            tok_str[tok_len++]= *ptr;
            ++ptr; ++col;
            while(*ptr>='0' && *ptr<='9')
              {
              tok_str[tok_len++]= *ptr++; col++;
              }
            }
          if(*ptr=='e' ||  *ptr=='E')
            {
            tok_str[tok_len++]= *ptr++; col++;
            if(*ptr=='-' || *ptr=='+')
              {
              tok_str[tok_len++]= *ptr++; col++;
              }
            while(*ptr>='0' && *ptr<='9')
              {
              tok_str[tok_len++]= *ptr++; col++;
              }
            }
          tok_str[tok_len]=0;
          return 'FLT';
          }
        else
          { /* Simple integer */
          tok_str[tok_len]=0;
          return 'NUM';
          }
        }

      case '.':
        { /* Maybe float... maybe token */
        if(ptr[1]>='0' && ptr[1]<='9')
          {
          tok_len=0;
          goto do_float;
          }
        else
          {
          ++ptr; ++col;
          return '.';
          }
        }

      case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h':
      case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p':
      case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x':
      case 'y': case 'z':
      case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H':
      case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P':
      case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
      case 'Y': case 'Z':
      case '_': case '$':
        { /* Identifier or keyword */
        tok_str[0]= *ptr++;
        ++col;
        tok_len=1;
        while(*ptr>='a' && *ptr<='z' ||
              *ptr>='A' && *ptr<='Z' ||
              *ptr>='0' && *ptr<='9' ||
              *ptr=='_' || *ptr=='$')
          {
          tok_str[tok_len++]= *ptr++;
          ++col;
          }
        tok_str[tok_len]=0;
        return 'NAME';
        }

      case '/':
        {
        if(ptr[1]=='/')
          {
          int x;
          strcpy(tok_str,ptr+2);
          *ptr=0;
          for(x=0;tok_str[x];++x)
            if(tok_str[x]=='\r' || tok_str[x]=='\n')
              {
              tok_str[x]=0;
              break;
              }
          tok_len=x;
          return '//';
          }
        else if(ptr[1]=='*')
          {
          int first=line;
          tok_len=0;
          ptr+=2; col+=2;
          while(!(ptr[0]=='*' && ptr[1]=='/'))
            {
            if(*ptr) tok_str[tok_len++]=ptr[0];
            if(*ptr=='\t')
              {
              ++ptr; col+=8-col%8;
              }
            else if(*ptr)
              {
              ++ptr; ++col;
              }
            else if(infile && getmore && !eof)
              if(ptr=getmore())
                {
                col=0;
                lvl= -1;
                ++line;
                }
              else
                {
                ptr="";
                eof=1;
                }
            else
              {
              error(name,line,"Unterminated comment (began on line %d)",first);
              return '/*';
              }
            }
          tok_str[tok_len]=0;
          ptr+=2;
          col+=2;
          return '/*';
          }
        else
          {
          ++col;
          return *ptr++;
          }
        }

      case '#': case ',': case '?': case ':': case ';': case '+': case '-':
      case '{': case '}': case '[': case ']': case '(': case ')': case '*': case '^':
      case '~': case '@': case '`': case '%':
        { /* Single character token */
        ++col;
        return *ptr++;
        }

      case '=':
        { /* = group */
        if(ptr[1]=='=' && ptr[2]=='=')
          {
          ptr+=3; col+=3;
          return '===';
          }
        else if(ptr[1]=='=')
          {
          ptr+=2; col+=2;
          return '==';
          }
        else
          {
          ptr+=1; col+=1;
          return '=';
          }
        }

      case '!':
        { /* ! group */
        if(ptr[1]=='=' && ptr[2]=='=')
          {
          ptr+=3; col+=3;
          return '!==';
          }
        else if(ptr[1]=='=')
          {
          ptr+=2; col+=2;
          return '!=';
          }
        else
          {
          ptr+=1; col+=1;
          return '!';
          }
        }

      case '&':
        { /* & group */
        if(ptr[1]=='&')
          {
          ptr+=2; col+=2;
          return '&&';
          }
        else
          {
          ptr+=1; col+=1;
          return '&';
          }
        }

      case '|':
        { /* | group */
        if(ptr[1]=='|')
          {
          ptr+=2; col+=2;
          return '||';
          }
        else
          {
          ptr+=1; col+=1;
          return '|';
          }
        }

      case '>':
        { /* > group */
        if(ptr[1]=='=')
          {
          ptr+=2; col+=2;
          return '>=';
          }
        else if(ptr[1]=='>')
          {
          ptr+=2; col+=2;
          return '>>';
          }
        else
          {
          ptr+=1; col+=1;
          return '>';
          }
        }

      case '<':
        { /* < group */
        if(ptr[1]=='=')
          {
          ptr+=2; col+=2;
          return '<=';
          }
        else if(ptr[1]=='<')
          {
          ptr+=2; col+=2;
          return '<<';
          }
        else
          {
          ptr+=1; col+=1;
          return '<';
          }
        }

      default:
        { /* Unknown character */
        error(name,line,"Unknown character '%c'",*ptr);
        ++ptr; ++col;
        break;
        }
      }
    }
  }

/* Preprocessor */

struct macro
  {
  struct macro *next;
  char *name;
  char *macro;
  } *macros;

void mk_macro(char *name,char *body)
  {
  struct macro *m=malloc(sizeof(struct macro));
  m->next=macros;
  macros=m;
  m->name=strdup(name);
  m->macro=strdup(body);
  }

struct cond
  {
  struct cond *next;
  int t; // set to keep code, clear to remove it
  int e; // set if we've seen else
  } *conds;

struct tok_hist *tok, *last_tok, *ungot_tok;

void unget_tok(struct tok_hist *t)
  {
  ungot_tok = t;
  }

int token()
  {
  struct tok_hist *h;
  int t;
  if (ungot_tok)
    {
    tok = ungot_tok;
    ungot_tok = ungot_tok->next;
    return tok->tok;
    }
  again:
  t=token_raw();
  // For pretty printer, we don't need preprocessor

  // Record token in history
  h = malloc(sizeof(struct tok_hist));
  h->next = 0;
  h->prev = last_tok;
  h->tok = t;
  h->file = tok_file;
  h->line = tok_line;
  h->col = tok_col;
  h->str = malloc(tok_len+1);
  h->len = tok_len;
  h->str[h->len] = 0;
  if (h->len)
    memcpy(h->str, tok_str, h->len);
  if (last_tok)
    {
    last_tok->next = h;
    }
  last_tok = h;
  tok = h;
  return t;

  if(conds && !conds->t)
    { // Delete until endif or else
    if(t=='`')
      {
      t=token_raw();
      if(t=='NAME' && !conds->e && !strcmp(tok_str,"else"))
        {
        conds->e=1;
        conds->t=1;
        goto again;
        }
      else if(t=='NAME' && !strcmp(tok_str,"endif"))
        {
        struct cond *c=conds;
        conds=c->next;
        free(c);
        goto again;
        }
      else if(t=='EOF')
        {
        error(name,line,"EOF before endif");
        conds=0;
        return t;
        }
      else
        goto again;
      }
    else if(t=='EOF')
      {
      error(name,line,"EOF before endif");
      conds=0;
      return t;
      }
    else goto again;
    }
  if(t=='`')
    {
    t=token_raw();
    if(t=='NAME')
      { /* A macro or preprocessor command */
      if(!strcmp(tok_str,"include"))
        {
        t=token_raw();
        if(t=='STR')
          {
          source_push(strdup(tok_str), 0);
          goto again;
          }
        else
          {
          error(name,line,"expected string after `include");
          return t;
          }
        }
      else if(!strcmp(tok_str,"ifdef"))
        {
        t=token_raw();
        if(t=='NAME')
          {
          struct macro *m;
          struct cond *cond;
          for(m=macros;m;m=m->next)
            if(!strcmp(m->name,tok_str)) break;
          cond=malloc(sizeof(struct cond));
          cond->next=conds;
          conds=cond;
          cond->e=0;
          if(m)
            cond->t=1;
          else
            cond->t=0;
          goto again;
          }
        else
          {
          error(name,line,"expected identifier after `ifdef");
          return t;
          }
        }
      else if(!strcmp(tok_str,"define"))
        { /* Create a macro */
        t=token_raw();
        if(t=='NAME')
          {
          mk_macro(strdup(tok_str),strdup(ptr));
          *ptr = 0;
          goto again;
          }
        else
          {
          error(name,line,"expected identifier after `define");
          return t;
          }
        }
      else if(!strcmp(tok_str,"timescale"))
        { /* Ignore to end of line */
        *ptr = 0;
        goto again;
        }
      else if(!strcmp(tok_str,"else"))
        {
        if(conds && !conds->e)
          { // Delete until endif
          conds->e=1;
          conds->t=0;
          goto again;
          }
        else
          {
          error(name,line,"else without matching ifdef");
          goto again;
          }
        }
      else if(!strcmp(tok_str,"endif"))
        {
        if(conds)
          {
          struct cond *c=conds;
          conds=c->next;
          free(c);
          goto again;
          }
        else
          {
          error(name,line,"endif without matching ifdef");
          goto again;
          }
        }
      else
        { /* Look for macro */
        struct macro *m;
        for(m=macros;m;m=m->next)
          if(!strcmp(m->name,tok_str)) break;
        if(m)
          {
          macro_push(m->name,m->macro);
          goto again;
          }
        else
          {
          error(name,line,"unknown preprocessor directive/macro '%s'",tok_str);
          goto again;
          }
        }
      }
    else
      {
      error(name,line,"` in bad place");
      return t;
      }
    }
  else
    return t;
  }

void show_tok(int c)
  {
  printf("\"%s\" %d ",tok_file,tok_line);

  if(c&0xFF000000) printf("%c",255&(c>>24));
  if(c&0x00FF0000) printf("%c",255&(c>>16));
  if(c&0x0000FF00) printf("%c",255&(c>>8));
  if(c&0x000000FF) printf("%c",255&c);

  if(c=='INT')
    printf(" %d\n",tok_len);
  else if(c=='NUM')
    {
    int x;
    printf(" %s\n",tok_str);
    //printf(" %d'b",tok_len);
    //for(x=0;x!=tok_len;++x)
    //  printf("%c",tok_str[tok_len-1-x]);
    //printf("\n");
    }
  else if(c=='STR')
    printf(" \"%s\"\n",tok_str);
  else if(c=='NAME')
    printf(" %s\n",tok_str);
  else if(c=='//')
    printf(" %s\n",tok_str);
  else if(c=='/*')
    printf(" %s\n",tok_str);
  else
    printf("\n");
  }

void compfile()
  {
  int c;
  while((c=token_raw())!='EOF') show_tok(c);
  }

void add_path(char *path)
  {
  struct path *p=malloc(sizeof(struct path));
  p->next=inc_paths;
  inc_paths=p;
  strcpy(p->buf,path);
  }

/* parse first three words in first /* comment on this line */

void scan_comment(char *t,char **a)
  {
  char buf[1024];
  a[0]=0;
  a[1]=0;
  a[2]=0;
  a[3]=0;
  a[4]=0;
  for(;*t;++t)
    if(t[0]=='/' && t[1]=='*')
      {
      int x;
      t+=2;
      while(*t==' ' || *t=='\t') ++t;
      for(x=0;*t && !(t[0]=='*' && t[1]=='/') && *t!=' ' && *t!='\t';++x, ++t)
        buf[x]=*t;
      buf[x]=0;
      if(x)
        {
        a[0]=strdup(buf);
        if(*t==' ' || *t=='\t')
          {
          while(*t==' ' || *t=='\t') ++t;
          for(x=0;*t && !(t[0]=='*' && t[1]=='/') && *t!=' ' && *t!='\t';++x, ++t)
            buf[x]=*t;
          buf[x]=0;
          if(x)
            {
            a[1]=strdup(buf);
            if(*t==' ' || *t=='\t')
              {
              while(*t==' ' || *t=='\t') ++t;
              for(x=0;*t && !(t[0]=='*' && t[1]=='/') && *t!=' ' && *t!='\t';++x, ++t)
                buf[x]=*t;
              buf[x]=0;
              if(x)
                {
                a[2]=strdup(buf);
                if(*t==' ' || *t=='\t')
                  {
                  while(*t==' ' || *t=='\t') ++t;
                  for(x=0;*t && !(t[0]=='*' && t[1]=='/') && *t!=' ' && *t!='\t';++x, ++t)
                    buf[x]=*t;
                  buf[x]=0;
                  if(x)
                    {
                    a[3]=strdup(buf);
                    if(*t==' ' || *t=='\t')
                      {
                      while(*t==' ' || *t=='\t') ++t;
                      for(x=0;*t && !(t[0]=='*' && t[1]=='/') && *t!=' ' && *t!='\t';++x, ++t)
                        buf[x]=*t;
                      buf[x]=0;
                      if(x)
                        {
                        a[4]=strdup(buf);
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      break;
      }
  }

char *rest_of_line()
  {
  return strdup(ptr);
  }

struct scanner_state *save_scanner_state()
  {
  struct scanner_state *ss = (struct scanner_state *)malloc(sizeof(struct scanner_state));
  ss->inc_paths = inc_paths;
    inc_paths = 0;
  ss->getmore = getmore;
    getmore = 0;
  ss->ptr = ptr;
    ptr = "";
  ss->col = col;
    col = 0;
  ss->lvl = lvl;
    lvl = -1;
  ss->line = line;
    line = 0;
  ss->name = name;
    name = "stdin";
  ss->eof = eof;
    eof = 0;
  ss->infile = infile;
    infile = 0;
  memcpy(ss->inbuf,inbuf,4096);
    memset(inbuf, 0, 4096);
  ss->source_stack = source_stack;
    source_stack = 0;
  memcpy(ss->tok_file,tok_file,1024);
    memset(tok_file, 0, 1024);
  ss->tok_line = tok_line;
    tok_line = 0;
  memcpy(ss->tok_str,tok_str,4096);
    memset(tok_str, 0, 4096);
  ss->tok_len = tok_len;
    tok_len = 0;
  ss->tok_col = tok_col;
    tok_col = 0;
  ss->macros = macros;
    macros = 0;
  ss->tok = tok;
    tok = 0;
  ss->last_tok = last_tok;
    last_tok = 0;
  ss->ungot_tok = ungot_tok;
    ungot_tok = 0;
  return ss;
  }

void restore_scanner_state(struct scanner_state *ss)
  {
  inc_paths = ss->inc_paths;
  getmore = ss->getmore;
  ptr = ss->ptr;
  col = ss->col;
  lvl = ss->lvl;
  line = ss->line;
  name = ss->name;
  eof = ss->eof;
  infile = ss->infile;
  memcpy(inbuf,ss->inbuf,4096);
  source_stack = ss->source_stack;
  memcpy(tok_file,ss->tok_file,1024);
  tok_line = ss->tok_line;
  memcpy(tok_str,ss->tok_str,4096);
  tok_len = ss->tok_len;
  tok_col = ss->tok_col;
  macros = ss->macros;
  tok = ss->tok;
  last_tok = ss->last_tok;
  ungot_tok = ss->ungot_tok;
  free(ss);
  }
