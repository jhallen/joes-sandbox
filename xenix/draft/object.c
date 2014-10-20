/* Object management */

#include "object.h"

O *downfix(o)
O *o;
{
if(o)
 {
 o->up=0; o->down=MAXINT; o->right=0; o->left=MAXINT;
 if(o->what==OLINE)
  {
  if(o->v.line.x+o->x>o->right) o->right=o->v.line.x+o->x;
  if(o->v.line.x+o->x<o->left) o->left=o->v.line.x+o->x;
  if(o->v.line.y+o->y>o->up) o->up=o->v.line.y+o->y;
  if(o->v.line.y+o->y<o->down) o->down=o->v.line.y+o->y;
  if(o->x>o->right) o->right=o->x;
  if(o->x<o->left) o->left=o->x;
  if(o->y>o->up) o->up=o->y;
  if(o->y<o->down) o->down=o->y;
  }
 else
  {
  O *p;
  if(p=o->v.list) do
   {
   downfix(p);
   if(p->up>o->up) o->up=p->up;
   if(p->down<o->down) o->down=p->down;
   if(p->right>o->right) o->right=p->right;
   if(p->left<o->left) o->left=p->left;
   }
   while((p=p->link.next)!=o->v.list);
  }
 }
return o;
}

O *upfix(o)
O *o;
{
if(o)
 while(o->parent)
  {
  if(o->up>o->parent->up) o->parent->up=o->up;
  if(o->down<o->parent->down) o->parent->down=o->down;
  if(o->right>o->parent->right) o->parent->right=o->right;
  if(o->left<o->parent->left) o->parent->left=o->left;
  o=o->parent;
  }
return o;
}

O *modfix(o)
O *o;
{
if(o)
 {
 o->up=0; o->down=MAXINT; o->right=0; o->left=MAXINT;
 if(o->what==OLINE)
  {
  if(o->v.line.x+o->x>o->right) o->right=o->v.line.x+o->x;
  if(o->v.line.x+o->x<o->left) o->left=o->v.line.x+o->x;
  if(o->v.line.y+o->y>o->up) o->up=o->v.line.y+o->y;
  if(o->v.line.y+o->y<o->down) o->down=o->v.line.y+o->y;
  if(o->x>o->right) o->right=o->x;
  if(o->x<o->left) o->left=o->x;
  if(o->y>o->up) o->up=o->y;
  if(o->y<o->down) o->down=o->y;
  }
 else if(o->what==OLIST)
  {
  O *p;
  if(p=o->v.list) do
   {
   if(p->up>o->up) o->up=p->up;
   if(p->down<o->down) o->down=p->down;
   if(p->right>o->right) o->right=p->right;
   if(p->left<o->left) o->left=p->left;
   }
   while((p=p->link.next)!=o->v.list);
  }
 upfix(o);
 }
return o;
}

O *fixext(o)
O *o;
{
if(o)
 {
 downfix(o);
 upfix(o);
 }
return o;
}

O *mkline(parent,x,y,tox,toy)
O *parent;
int x,y,tox,toy;
{
O *o=(O *)malloc(sizeof(O));
o->what=OLINE;
o->parent=parent;
o->x=x;
o->y=y;
o->v.line.x=tox;
o->v.line.y=toy;
izque(O,link,o);
if(parent)
 if(parent->v.list) enqueb(O,link,parent->v.list,o);
 else parent->v.list=o;
modfix(o);
return o;
}

O *mklist(parent,x,y,list)
O *parent;
int x,y;
O *list;
{
O *o=(O *)malloc(sizeof(O));
o->what=OLIST;
o->parent=parent;
o->x=x;
o->y=y;
o->v.list=list;
izque(O,link,o);
if(parent)
 if(parent->v.list) enqueb(O,link,parent->v.list,o);
 else parent->v.list=o;
modfix(o);
return o;
}

void del(o)
O *o;
{
if(!o) return;
if(o->what==OLIST)
 {
 O *p, *e, *n;
 if(e=p=o->v.list) do
  {
  n=p->link.next;
  del(p);
  } while(p=n, p!=e);
 }
free(o);
}

O *rm(o)
O *o;
{
if(o)
 {
 if(o->parent)
  {
  if(qempty(O,link,o)) o->parent->v.list=0;
  else
   {
   deque(O,link,o);
   if(o->parent->v.list==o) o->parent->v.list=o->link.next;
   izque(O,link,o);
   }
  modfix(o->parent);
  }
 }
return o;
}

O *dup(parent,o,x,y,mx,my)
O *parent, *o;
{
O *n, *p;
if(!o) return 0;
if(o->what==OLIST)
 {
 n=mklist(parent,x,y,NULL);
 if(p=o->v.list) do
  dup(n,p,(p->x-o->x)*mx+x,(p->y-o->y)*my+y,mx,my);
  while((p=p->link.next)!=o->v.list);
 }
else n=mkline(parent,x,y,o->v.line.x*mx,o->v.line.y*my);
return n;
}
