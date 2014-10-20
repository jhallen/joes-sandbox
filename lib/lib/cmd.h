#ifndef _Icmd
#define _Icmd 1

#include "config.h"

typedef struct cmd CMD;
typedef struct macro MACRO;

/* Command entry */

struct cmd
 {
 char *name;		/* Command name */
 int flag;		/* Execution flags */
 int (*func)();		/* Function bound to name */
 MACRO *m;		/* Macro bound to name */
 int arg;		/* 0= arg is meaningless, 1= ok */
 char *negarg;		/* Command to use if arg was negative */
 };

extern CMD cmds[];	/* Global command table */

/* CMD *findcmd(char *s);
 * Return command address for given name.  Returns NULL is 's' is NULL or
 * if command is not found.
 */
EXTERN CMD *findcmd();

/* void addcmd(char *s,MACRO *m);
 * Add a command to macro binding to the command table. */
EXTERN void addcmd();

/* Execute a command.  Returns return value of command */
EXTERN int execmd();

#endif
