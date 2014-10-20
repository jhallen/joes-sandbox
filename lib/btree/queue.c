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

#include "config.h"
#include "queue.h"

void *QUEUE;
void *ITEM;
void *LAST;

typedef struct stditem STDITEM;
struct stditem
 {
 LINK(STDITEM) link;
 };

void *alitem(freelist,itemsize)
STDITEM *freelist;
 {
 if(qempty(STDITEM,link,freelist))
  {
  STDITEM *i=(STDITEM *)malloc(itemsize*16);
  STDITEM *z=(STDITEM *)((char *)i+itemsize*16);
  while(i!=z)
   {
   enquef(STDITEM,link,freelist,i);
   i=(STDITEM *)((char *)i+itemsize);
   }
  }
 return (void *)deque(STDITEM,link,freelist->link.prev);
 }

void frchn(freelist,chn)
STDITEM *freelist, *chn;
 {
 STDITEM *i;
 if((i=chn->link.prev)!=chn)
  {
  deque(STDITEM,link,chn);
  splicef(STDITEM,link,freelist,i);
  }
 }
