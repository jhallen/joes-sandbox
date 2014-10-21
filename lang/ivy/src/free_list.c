/* Simple free-list based item allocator

   Copyright (C) 1993 Joseph H. Allen

This file is part of IVY

IVY is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation; either version 1, or (at your option) any later version.  

IVY is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.  


IVY; see the file COPYING.  If not, write to the Free Software Foundation, 
675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <stdlib.h>
#include <stdio.h>
#include "free_list.h"

#define AL_MULT 128 /* No. items to allocate when free-list is empty */

/* Item_size should be a valid non-empty structure size */

void mk_allocator(Free_list *free_list, int item_size)
{
	free_list->list = 0;
	free_list->size = item_size;
	free_list->end = item_size * AL_MULT;
	free_list->alloc = item_size;
	free_list->page = free_list->first_page = malloc(free_list->end);
	*(void **)free_list->page = 0;
}

/* Free a free list and all of the items allocated from item */

void rm_allocator(Free_list *free_list)
{
	/* Free all the pages */
	while (free_list->first_page) {
		void *ptr = free_list->first_page;
		free_list->first_page = *(void **)ptr;
		free(ptr);
	}
}

/* Allocate a single item */

void *al_item(Free_list *free_list)
{
	void *i;

	if (free_list->list) {
		void *item = free_list->list;
		free_list->list = *(void **)item;
		return item;
	}

	if (free_list->alloc == free_list->end) {
		free_list->alloc = free_list->size;
		if (!*(void **)free_list->page) {
			free_list->page = *(void **)free_list->page = malloc(free_list->end);
			*(void **)free_list->page = 0;
		} else {
			free_list->page = *(void **)free_list->page;
		}
	}

	i = free_list->page + free_list->alloc;
	free_list->alloc += free_list->size;

	return i;
}

/* Free a single item */

void fr_item(Free_list *free_list, void *item)
{
	*(void **)item = free_list->list;
	free_list->list = item;
}

/* Free all items */

void fr_all(Free_list *free_list)
{
	free_list->page = free_list->first_page;
	free_list->alloc = free_list->size;
	free_list->list = 0;
}
