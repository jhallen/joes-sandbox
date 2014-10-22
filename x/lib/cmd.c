/* Command execution */

#include "config.h"
#include "vs.h"
#include "va.h"
#include "macro.h"
#include "hash.h"
#include "zstr.h"
#include "main.h"
#include "cmd.h"

int smode;

/* Execute a command with k and arg */

int execmd(cmd,bw,k,arg)
void *bw;
CMD *cmd;
 {
 int ret= -1;

 if(cmd->m) return exmacro(cmd->m,bw,0);

 /* Execute command */
 ret=cmd->func(bw,k,arg);

 if(smode) --smode;

 /* Don't update anything if we're going to leave */
 if(leave) return 0;

 return ret;
 }

/* Return command table index for given command name */

static HASH *cmdhash=0;

void izcmds()
 {
 int x;
 cmdhash=htmk(256);
 for(x=0;cmds[x].name;++x)
  htadd(cmdhash,cmds[x].name,cmds+x);
 }

CMD *findcmd(s)
char *s;
 {
 if(!cmdhash) izcmds();
 if(!s) return 0;
 return (CMD *)htfind(cmdhash,s);
 }

void addcmd(s,m)
char *s;
MACRO *m;
 {
 CMD *cmd=(CMD *)malloc(sizeof(CMD));
 if(!cmdhash) izcmds();
 cmd->name=zdup(s);
 cmd->flag=0;
 cmd->func=0;
 cmd->m=m;
 cmd->arg=1;
 cmd->negarg=0;
 htadd(cmdhash,cmd->name,cmd);
 }

char **getcmds()
 {
 char **s=vaensure(NULL,cmdhash->len);
 int x;
 HENTRY *e;
 for(x=0;x!=cmdhash->len;++x)
  for(e=cmdhash->tab[x];e;e=e->next)
   s=vaadd(s,vsncpy(NULL,0,sz(e->name)));
 vasort(s,aLen(s));
 return s;
 }
