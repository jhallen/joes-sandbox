/* Simple hash table */

#ifndef _Ihash
#define _Ihash 1

#include "config.h"

#define hnext(accu,c) (((accu)<<4)+((accu)>>28)+(c))

typedef struct entry HENTRY;
struct entry
 {
 char *name;		/* Symbol */
 int len;		/* Symbol length */
 HENTRY *next;		/* Next entry with same hash value */
 void *val;		/* Value bound to symbol */
 };

typedef struct hash HASH;
struct hash
 {
 int len;		/* No. entries in hash table - 1 */
 int llen;		/* No. entires in length table */
 HENTRY **tab;		/* Hash table itself */
 char *ltab;		/* Length table to use for find */
 char *btab;		/* Length table created specifically for this level */
 HASH *next;		/* Hash table with next broader scope */
 };

EXTERN unsigned long hash();
EXTERN unsigned long hashn();
EXTERN HASH *htmk();
EXTERN void htrm();
EXTERN void *htfind();
EXTERN void *htfindn();
EXTERN void *htlfind();
EXTERN void *htadd();
EXTERN HASH *htpsh();
EXTERN void htpop();
EXTERN void htall();

#endif
