/* Doubly-linked list macros */

#ifndef _Iqueue
#define _Iqueue 1

#include "config.h"

/* Use this as first member of a structure to make it into a doubly linked
 * list node.
 */
#define QITEM(type) type *next, *prev

/* Use this to make the base node of a doubly-linked list.  Or if the item
 * making up the structure is small enough, just use the structure itself */
#define QBASE(type) struct { QITEM(type); }

struct _qnode { QITEM(struct _qnode); };

extern struct _qnode *_queue, *_item, *_last;

#define IZQUE(item) \
 ( \
 _queue=(void *)(item), \
 (void *)(_queue->next=_queue->prev=_queue) \
 )

#define DEQUE(item) \
 ( \
 _queue=(void *)(item), \
 _queue->prev->next=_queue->next, \
 _queue->next->prev=_queue->prev, \
 (void *)_queue \
 )

#define QEMPTY(item) \
 ( \
 _queue=(void *)(item), \
 _queue==_queue->next \
 )

#define ENQUEF(queue,item) \
 ( \
 _item=(void *)(item), \
 _queue=(void *)(queue), \
 _item->next=_queue->next, \
 _item->prev=_queue, \
 _queue->next->prev=_item, \
 _queue->next=_item, \
 (void *)_item \
 )

#define ENQUEB(queue,item) \
 ( \
 _item=(void *)(item), \
 _queue=(void *)(queue), \
 _item->next=_queue, \
 _item->prev=_queue->prev, \
 _queue->prev->next=_item, \
 _queue->prev=_item, \
 (void *)_item \
 )

#define PROMOTE(queue,item) ENQUEF(queue,DEQUE(item))

#define DEMOTE(queue,item) ENQUEB(queue,DEQUE(item))

#define SPLICEF(queue,chain) \
 ( \
 _item=(void *)(chain), \
 _last=_item->prev, \
 _queue=(void *)(queue), \
 _last->next=_queue->next, \
 _item->prev=_queue, \
 _queue->next->prev=_last, \
 _queue->next=_item, \
 (void *)_item \
 )

#define SPLICEB(queue,chain) \
 ( \
 _item=(void *)(chain), \
 _last=_item->prev, \
 _queue=(void *)(queue), \
 _last->next=_queue, \
 _item->prev=_queue->prev, \
 _queue->prev->next=_item, \
 _queue->prev=_last, \
 (void *)_item \
 )

#define SNIP(first,last) \
 ( \
 _item=(void *)(first), \
 _last=(void *)(last), \
 _last->next->prev=_item->prev, \
 _item->prev->next=_last->next, \
 _item->prev=_last, \
 _last->next=_item, \
 (void *)_item \
 )

/* Doubly linked list database */

typedef struct link LINK;

struct link { QITEM(LINK); void *data; };

void *alitem();
void fritem();

void izque();
LINK *newque();
LINK *enquef();
LINK *enqueb();
int qempty();
void *deque();
LINK *qfind();
int qfindn();
LINK *qnth();
int qno();
LINK *splicef();
LINK *spliceb();
LINK *snip();

#endif
