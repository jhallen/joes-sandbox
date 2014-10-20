/* Simple hash table */

#ifndef _Ihash
#define _Ihash 1

#include "config.h"

#define hnext(accu,c) (((accu)<<4)+((accu)>>28)+(c))

typedef struct entry HENTRY;
struct entry
 {
 char *name;
 HENTRY *next;
 void *val;
 };

typedef struct hash HASH;
struct hash
 {
 int len;
 HENTRY **tab;
 };

unsigned long hash();
HASH *htmk();
void htrm();
void *htfind();
void *htadd();

#endif
