#include "queue.h"

struct _qnode *_queue, *_item, *_last;

LINK *freelinks=0;

void *alitem(freelist,size)
void **freelist;
 {
 void *item;
 if(!*freelist)
  { /* Free-list is out of items so allocate more */
  int x;
  void *i=calloc(size,16);
  for(x=0;x!=16;++x)
   {
   *(void **)i= *freelist;
   *freelist=i;
   i=(char *)i+size;
   }
  }
 item= *freelist;
 *freelist= *(void **)item;
 return item;
 }

void fritem(freelist,item)
void **freelist, *item;
 {
 *(void **)item= *freelist;
 *freelist=item;
 }

void izque(q)
LINK *q;
 {
 IZQUE(q);
 }

LINK *newque(i)
void *i;
 {
 LINK *n=alitem(&freelinks,sizeof(LINK));
 izque(n);
 n->data=i;
 return n;
 }

LINK *enquef(q,i)
LINK *q;
void *i;
 {
 LINK *item=alitem(&freelinks,sizeof(LINK));
 item->data=i;
 return ENQUEF(q,item);
 }

LINK *enqueb(q,i)
LINK *q;
void *i;
 {
 LINK *item=alitem(&freelinks,sizeof(LINK));
 item->data=i;
 return ENQUEB(q,item);
 }

int qempty(i)
LINK *i;
 {
 return QEMPTY(i);
 }

void *deque(i)
LINK *i;
 {
 void *data=i->data;
 fritem(&freelinks,DEQUE(i));
 return data;
 }

LINK *qfind(q,data)
LINK *q;
void *data;
 {
 LINK *i;
 for(i=q->next;i!=q;i=i->next) if(i->data==data) return i;
 return 0;
 }

int qfindn(q,data)
LINK *q;
void *data;
 {
 LINK *i;
 int n=0;
 for(i=q->next;i!=q;i=i->next,++n) if(i->data==data) return n;
 return 0;
 }

LINK *qnth(q,n)
LINK *q;
 {
 while(n) q=q->next, --n;
 return q;
 }

int qno(q)
LINK *q;
 {
 int n=0;
 LINK *i;
 for(i=q->next;i!=q;i=i->next) ++n;
 return n;
 }

LINK *splicef(q,chain)
LINK *q, *chain;
 {
 SPLICEF(q,chain);
 return chain;
 }

LINK *spliceb(q,chain)
LINK *q, *chain;
 {
 SPLICEB(q,chain);
 return q;
 }

LINK *snip(first,last)
LINK *first, *last;
 {
 return SNIP(first,last);
 }
