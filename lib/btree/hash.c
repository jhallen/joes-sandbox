/* Simple hash table */

#include "zstr.h"
#include "hash.h"

static HENTRY *freentry=0;

unsigned long hash(s)
char *s;
 {
 unsigned long accu=0;
 while(*s) accu=hnext(accu,*s++);
 return accu;
 }

HASH *htmk(len)
 {
 HASH *t=(HASH *)malloc(sizeof(HASH));
 t->len=len-1;
 t->tab=(HENTRY **)calloc(sizeof(HENTRY *),len);
 return t;
 }

void htrm(ht)
HASH *ht;
 {
 free(ht->tab);
 free(ht);
 }

void *htadd(ht,name,val)
HASH *ht;
char *name;
void *val;
 {
 int idx=hash(name)&ht->len;
 HENTRY *entry;
 if(!freentry)
  {
  int x;
  entry=(HENTRY *)malloc(sizeof(HENTRY)*64);
  for(x=0;x!=64;++x) entry[x].next=freentry, freentry=entry+x;
  }
 entry=freentry;
 freentry=entry->next;
 entry->next=ht->tab[idx];
 ht->tab[idx]=entry;
 entry->name=name;
 return entry->val=val;
 }

void *htfind(ht,name)
HASH *ht;
char *name;
 {
 HENTRY *e;
 for(e=ht->tab[hash(name)&ht->len];e;e=e->next)
  if(!zcmp(e->name,name)) return e->val;
 return 0;
 }
