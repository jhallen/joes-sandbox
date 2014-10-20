#ifndef _Iobject
#define _Iobject

#include "config.h"
#include "queue.h"

/* Display object */

typedef struct object O;
struct object
 {
 LINK(O) link;			/* Doubly linked list of objects */
 int what;			/* Type of object */
 int x,y;			/* Position of object */
 int up,down,left,right;	/* Extent of object */
 O *parent;			/* Object's parent.  0 for root */
 union
  {
  struct			/* If object is a line */
   {
   int x,y;
   } line;
  O *list;			/* If object is a list */
  } v;
 };

/* Object types */

#define OLIST 1
#define OLINE 2

O *fixext();
O *modfix();
O *mkline();
O *mklist();
O *rm();
void del();
O *ins();
O *dup();

#endif
