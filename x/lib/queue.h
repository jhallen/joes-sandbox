/* Doubly-linked list macros */

#ifndef _Iqueue
#define _Iqueue 1

#include "config.h"

/* Use this as first member of a structure to make it into a doubly linked
 * list node.
 */
#define QITEM(type) type *next, *prev

/* Use this to make the base node of a doubly-linked list.  Or if the item
 * making up the structure is small enough, just use the structure itself.
 */
#define QBASE(type) struct { QITEM(type); }

struct _qnode { QITEM(struct _qnode); };

#define IZQUE(item) \
 ( \
 (item)->next=(item)->prev=(void *)(item) \
 )

#define DEQUE(item) \
 ( \
 (item)->prev->next=(item)->next, \
 (item)->next->prev=(item)->prev, \
 (item) \
 )

#define QEMPTY(item) \
 ( \
 (void *)(item)==(void *)(item)->next \
 )

#define ENQUEF(queue,item) \
 ( \
 (item)->next=(queue)->next, \
 (item)->prev=(queue), \
 (queue)->next->prev=(item), \
 (queue)->next=(item) \
 )

#define ENQUEB(queue,item) \
 ( \
 (item)->next=(queue), \
 (item)->prev=(queue)->prev, \
 (queue)->prev->next=(item), \
 (queue)->prev=(item) \
 )

#define PROMOTE(queue,item) \
 ( \
 (item)->prev->next=(item)->next, \
 (item)->next->prev=(item)->prev, \
 (item)->next=(queue)->next, \
 (item)->prev=(queue), \
 (queue)->next->prev=(item), \
 (queue)->next=(item) \
 )

#define DEMOTE(queue,item) \
 ( \
 (item)->prev->next=(item)->next, \
 (item)->next->prev=(item)->prev, \
 (item)->next=(queue), \
 (item)->prev=(queue)->prev, \
 (queue)->prev->next=(item), \
 (queue)->prev=(item) \
 )

#define SPLICEF(queue,chain) \
 ( \
 (queue)->next->prev=(chain)->prev, \
 (chain)->prev->next=(queue)->next, \
 (chain)->prev=(queue), \
 (queue)->next=(chain) \
 )

#define SPLICEB(queue,chain) \
 ( \
 (chain)->prev->next=(queue), \
 (queue)->prev->next=(chain)->prev, \
 (chain)->prev=(queue)->prev, \
 (queue)->prev=(chain) \
 )

#define SNIP(first,last) \
 ( \
 (last)->next->prev=(first)->prev, \
 (first)->prev->next=(last)->next, \
 (first)->prev=(last), \
 (last)->next=(first) \
 )

/* Item allocation */

void *alitem();
void fritem();

/* Doubly linked list database */

typedef struct link LINK;

struct link { QITEM(LINK); void *data; };

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
