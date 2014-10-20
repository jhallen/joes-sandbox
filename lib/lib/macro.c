/* Keyboard macros
   Copyright (C) 1992 Joseph H. Allen

This file is part of JOE (Joe's Own Editor)

JOE is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation; either version 1, or (at your option) any later version.  

JOE is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.  

You should have received a copy of the GNU General Public License along with 
JOE; see the file COPYING.  If not, write to the Free Software Foundation, 
675 Mass Ave, Cambridge, MA 02139, USA.  */ 

#include "main.h"
#include "vs.h"
#include "cmd.h"
#include "zstr.h"
#include "macro.h"

/* Macro Building functions */

MACRO *freemacros=0;	/* List of free macros */

/* Create a macro */

MACRO *mkmacro(k,n,cmd)
CMD *cmd;
 {
 MACRO *macro;
 if(!freemacros)
  {
  int x;
  macro=(MACRO *)malloc(sizeof(MACRO)*64);
  for(x=0;x!=64;++x)
   macro[x].steps=(MACRO **)freemacros,
   freemacros=macro+x;
  }
 macro=freemacros;
 freemacros=(MACRO *)macro->steps;
 macro->steps=0;
 macro->size=0;
 macro->n=n;
 macro->cmd=cmd;
 macro->k=k;
 return macro;
 }

/* Eliminate a macro */

void rmmacro(macro)
MACRO *macro;
 {
 if(macro)
  {
  if(macro->steps)
   {
   int x;
   for(x=0;x!=macro->n;++x) rmmacro(macro->steps[x]);
   free(macro->steps);
   }
  macro->steps=(MACRO **)freemacros;
  freemacros=macro;
  }
 }

/* Add a step to block macro */

void addmacro(macro,m)
MACRO *macro, *m;
 {
 if(macro->n==macro->size)
  if(macro->steps)
   macro->steps=(MACRO **)realloc(macro->steps,(macro->size+=8)*sizeof(MACRO *));
  else
   macro->steps=(MACRO **)malloc((macro->size=8)*sizeof(MACRO *));
 macro->steps[macro->n++]=m;
 }

/* Duplicate a macro */

MACRO *dupmacro(mac)
MACRO *mac;
 {
 MACRO *m=mkmacro(mac->k,mac->n,mac->cmd);
 if(mac->steps)
  {
  int x;
  m->steps=(MACRO **)malloc((m->size=mac->n)*sizeof(MACRO *));
  for(x=0;x!=m->n;++x) m->steps[x]=dupmacro(mac->steps[x]);
  }
 return m;
 }

/* Set key part of macro */

MACRO *macstk(m,k)
MACRO *m;
 {
 m->k=k;
 return m;
 }

/* Parse text into a macro
 * sta is set to:  ending position in buffer for no error.
 *                 -1 for syntax error
 *                 -2 for need more input
 */

MACRO *mparse(m,buf,sta)
MACRO *m;
char *buf;
int *sta;
 {
 int y, c, x=0;

 macroloop:

 /* Skip whitespace */
 while(cwhite(buf[x])) ++x;

 /* Do we have a string? */
 if(buf[x]=='\"')
  {
  ++x;
  while(buf[x] && buf[x]!='\"')
   {
   if(buf[x]=='\\' && buf[x+1])
    {
    ++x;
    switch(buf[x])
     {
    case 'n': buf[x]=10; break;
    case 'r': buf[x]=13; break;
    case 'b': buf[x]=8; break;
    case 'f': buf[x]=12; break;
    case 'a': buf[x]=7; break;
    case 't': buf[x]=9; break;
    case 'x':
     c=0;
     if(buf[x+1]>='0' && buf[x+1]<='9') c=c*16+buf[++x]-'0';
     else if(buf[x+1]>='a' && buf[x+1]<='f' ||
             buf[x+1]>='A' && buf[x+1]<='F') c=c*16+(buf[++x]&0xF)+9;
     if(buf[x+1]>='0' && buf[x+1]<='9') c=c*16+buf[++x]-'0';
     else if(buf[x+1]>='a' && buf[x+1]<='f' ||
             buf[x+1]>='A' && buf[x+1]<='F') c=c*16+(buf[++x]&0xF)+9;
     buf[x]=c;
     break;
    case '0': case '1': case '2': case '3':
    case '4': case '5': case '6': case '7':
    case '8': case '9':
     c=buf[x]-'0';
     if(buf[x+1]>='0' && buf[x+1]<='7') c=c*8+buf[++x]-'0';
     if(buf[x+1]>='0' && buf[x+1]<='7') c=c*8+buf[++x]-'0';
     buf[x]=c;
     break;
     }
    }
   if(m)
    {
    if(!m->steps)
     {
     MACRO *macro=m;
     m=mkmacro(MAXINT,0,NULL);
     addmacro(m,macro);
     }
    }
   else m=mkmacro(MAXINT,0,NULL);
   addmacro(m,mkmacro(buf[x],0,findcmd("type")));
   ++x;
   }
  if(buf[x]=='\"') ++x;
  }

 /* Do we have a command? */
 else
  {
  for(y=x;
      buf[y] && buf[y]!=',' && buf[y]!=' ' && buf[y]!='\t' && buf[y]!='\n' && buf[x]!='\r';
      ++y);
  if(y!=x)
   {
   CMD *cmd;
   c=buf[y]; buf[y]=0;
   cmd=findcmd(buf+x);
   if(!cmd)
    {
    *sta = -1;
    return 0;
    }
   else if(m)
    {
    if(!m->steps)
     {
     MACRO *macro=m;
     m=mkmacro(MAXINT,0,NULL);
     addmacro(m,macro);
     }
    addmacro(m,mkmacro(MAXINT,0,cmd));
    }
   else m=mkmacro(MAXINT,0,cmd);
   buf[x=y]=c;
   }
  }

 /* Skip whitespace */
 while(cwhite(buf[x])) ++x;

 /* Do we have a comma? */
 if(buf[x]==',')
  {
  ++x;
  while(cwhite(buf[x])) ++x;
  if(buf[x] && buf[x]!='\r' && buf[x]!='\n') goto macroloop;
  *sta= -2;
  return m;
  }

 /* Done */
 *sta=x;
 return m;
 }

/* Convert macro to text */

static char *ptr;
static int first;
static int instr;

char *unescape(ptr,c)
char *ptr;
 {
 if(c=='"') *ptr++='\\', *ptr++='"';
 else if(c=='\'') *ptr++='\\', *ptr++='\'';
 else if(c<32 || c>126)
  {
  *ptr++='\\';
  *ptr++='x';
  *ptr++="0123456789ABCDEF"[c>>4];
  *ptr++="0123456789ABCDEF"[c&15];
  }
 else *ptr++=c;
 return ptr;
 }

void domtext(m)
MACRO *m;
 {
 int x;
 if(!m) return;
 if(m->steps)
  for(x=0;x!=m->n;++x) domtext(m->steps[x]);
 else
  {
  if(instr && zcmp(m->cmd->name,"type")) *ptr++='\"', instr=0;
  if(first) first=0;
  else if(!instr) *ptr++=',';
  if(!zcmp(m->cmd->name,"type"))
   {
   if(!instr) *ptr++='\"', instr=1;
   ptr=unescape(ptr,m->k);
   }
  else
   {
   for(x=0;m->cmd->name[x];++x) *ptr++=m->cmd->name[x];
   if(!zcmp(m->cmd->name,"play") ||
      !zcmp(m->cmd->name,"gomark") ||
      !zcmp(m->cmd->name,"setmark") ||
      !zcmp(m->cmd->name,"record") ||
      !zcmp(m->cmd->name,"uarg"))
    {
    *ptr++=',';
    *ptr++='"';
    ptr=unescape(ptr,m->k);
    *ptr++='"';
    }
   }
  }
 }

char *mtext(s,m)
char *s;
MACRO *m;
 {
 ptr=s;
 first=1;
 instr=0;
 domtext(m);
 if(instr) *ptr++='\"';
 *ptr=0;
 return s;
 }

/* Macro execution */

MACRO *curmacro=0;		/* Set if we're in a macro */
static int macroptr;		/* Current step within macro */
static int arg=0;		/* Repeat argument */
static int argset=0;		/* Set if 'arg' is set */

int exmacro(m,object,u)
MACRO *m;
void *object;
 {
 int repeat;			/* Repeat value */
 int larg;			/* Argument to pass to command */
 CMD *cmd;			/* Command we're going to execute */
 int n;
 int ret=0;			/* Command's return value */

 /* Check if user set an argument */
 cmd=m->cmd;
 repeat=1;
 larg=1;

 if(argset)
  {
  larg=arg;		/* User specified argument value */
  arg=0;
  argset=0;
  if(m->steps)
   { /* If this is a macro, then change argument into a repeat value */
   repeat=larg; larg=1;
   if(repeat<0) repeat=1;
   }
  else if(cmd->arg)
   { /* Convert into a simple repeat */
   repeat=larg; larg=1;
   if(repeat<0)
    {
    repeat= -repeat;
    if(cmd->negarg) cmd=findcmd(cmd->negarg);
    if(!cmd) repeat=0;
    }
   }
  }

 while(repeat-- && !leave && !ret)
  if(m->steps)
   {
   MACRO *tmpmac=curmacro;
   int tmpptr=macroptr;
   int x=0;
   while(m && x!=m->n && !leave && !ret)
    {
    MACRO *d;
    d=m->steps[x++];
    curmacro=m;
    macroptr=x;
    ret=exmacro(d,object,0);
    m=curmacro;
    x=macroptr;
    }
   curmacro=tmpmac;
   macroptr=tmpptr;
   }
  else ret=execmd(cmd,object,m->k,larg);

 if(leave) return ret;

 return ret;
 }

/* Execute a macro */

int exemac(object,m)
void *object;
MACRO *m;
 {
 return exmacro(m,object,1);
 }
