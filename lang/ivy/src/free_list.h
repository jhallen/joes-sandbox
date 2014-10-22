/* Free-list

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

#ifndef _Ifree_list
#define _Ifree_list 1

typedef struct free_list Free_list;

/* A simple item allocator */

struct free_list {
	unsigned char *page;	/* Current allocation page */
	int alloc;		/* Current allocation index */
	int size;		/* Item size */
	int end;		/* size * AL_MULT */
	void *list;		/* Free items */
	void *first_page;	/* All pages */
};

/* Create an allocator */
void mk_allocator(Free_list *free_list, int item_size);

/* Free an allocator */
void rm_allocator(Free_list *free_list);

/* Allocate an item */
void *al_item(Free_list *free_list);

/* Free an item */
void fr_item(Free_list *free_list, void *item);

/* Free all items */
void fr_all(Free_list *free_list);

#endif
