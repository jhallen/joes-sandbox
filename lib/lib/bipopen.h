/* Bidirection version of popen() */

#ifndef _Ibipopen
#define _Ibipopen 1

#include "config.h"

typedef struct bip BIP;

struct bip
 {
 int infd;	/* Input file descriptor */
 int outfd;	/* Output file descriptor */
 int pid;	/* Sub-process id */
 };

BIP *bipopen();
void bipclose();
void bipputs();
char *bipgets();

#endif
