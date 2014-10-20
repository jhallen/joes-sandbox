/* Doubly linked list primitives
   Copyright (C) 1992 Joseph H. Allen

This file is part of JOE (Joe's Own Editor)

JOE is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation; either version 1, or (at your option) any later version.  

JOE is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.  

You should have received a copy of the GNU General Public License along with 
JOE; see the file COPYING.  If not, write to the Free Software Foundation, 
675 Mass Ave, Cambridge, MA 02139, USA.  */ 

#ifndef _Iqueue
#define _Iqueue 1

#include "config.h"

extern void *ITEM;
extern void *QUEUE;
extern void *LAST;

#define LINK(type) struct { type *next; type *prev; }

#define izque(type,member,item) \
	( \
	QUEUE=(void *)(item), \
	((type *)QUEUE)->member.prev=(type *)QUEUE, \
	((type *)QUEUE)->member.next=(type *)QUEUE, \
	(type *)QUEUE \
	)

#define deque(type,member,item) \
	( \
	ITEM=(void *)(item), \
	((type *)ITEM)->member.prev->member.next=((type *)ITEM)->member.next, \
	((type *)ITEM)->member.next->member.prev=((type *)ITEM)->member.prev, \
	(type *)ITEM \
	)

#define qempty(type,member,item) \
	( \
	QUEUE=(void *)(item), \
	(type *)QUEUE==((type *)QUEUE)->member.next \
	)

#define enquef(type,member,queue,item) \
	( \
	ITEM=(void *)(item), \
	QUEUE=(void *)(queue), \
	((type *)ITEM)->member.next=((type *)QUEUE)->member.next, \
	((type *)ITEM)->member.prev=(type *)QUEUE, \
	((type *)QUEUE)->member.next->member.prev=(type *)ITEM, \
	((type *)QUEUE)->member.next=(type *)ITEM, \
	(type *)ITEM \
	)

#define enqueb(type,member,queue,item) \
	( \
	ITEM=(void *)(item), \
	QUEUE=(void *)(queue), \
	((type *)ITEM)->member.next=(type *)QUEUE, \
	((type *)ITEM)->member.prev=((type *)QUEUE)->member.prev, \
	((type *)QUEUE)->member.prev->member.next=(type *)ITEM, \
	((type *)QUEUE)->member.prev=(type *)ITEM, \
	(type *)ITEM \
	)

#define promote(type,member,queue,item) \
	( \
	enquef(type,member,(queue),deque(type,member,(item))) \
	)

#define demote(type,member,queue,item) \
	( \
	enqueb(type,member,(queue),deque(type,member,(item))) \
	)

#define splicef(type,member,queue,chain) \
	( \
	ITEM=(void *)(chain), \
	LAST=(void *)((type *)ITEM)->member.prev, \
	QUEUE=(void *)(queue), \
	((type *)LAST)->member.next=((type *)QUEUE)->member.next, \
	((type *)ITEM)->member.prev=(type *)QUEUE, \
	((type *)QUEUE)->member.next->member.prev=(type *)LAST, \
	((type *)QUEUE)->member.next=(type *)ITEM, \
	(type *)ITEM \
	)

#define spliceb(type,member,queue,chain) \
	( \
	ITEM=(void *)(chain), \
	LAST=(void *)((type *)ITEM)->member.prev, \
	QUEUE=(void *)(queue), \
	((type *)LAST)->member.next=(type *)QUEUE, \
	((type *)ITEM)->member.prev=((type *)QUEUE)->member.prev, \
	((type *)QUEUE)->member.prev->member.next=(type *)ITEM, \
	((type *)QUEUE)->member.prev=(type *)LAST, \
	(type *)ITEM \
	)

#define snip(type,member,first,last) \
	( \
	ITEM=(void *)(first), \
	LAST=(void *)(last), \
	((type *)LAST)->member.next->member.prev=((type *)ITEM)->member.prev, \
	((type *)ITEM)->member.prev->member.next=((type *)LAST)->member.next, \
	((type *)ITEM)->member.prev=(type *)LAST, \
	((type *)LAST)->member.next=(type *)ITEM, \
	(type *)ITEM \
	)

void *alitem();
void frchn();

#endif
