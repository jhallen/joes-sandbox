/* Hash table

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
#include <string.h>
#include "ivy_hash.h"

Ivy_hash_table *ivy_create_ht(int starting_size)
{
	Ivy_hash_table *ht = (Ivy_hash_table *)malloc(sizeof(Ivy_hash_table));
	ht->size = starting_size - 1;
	ht->nentries = 0;
	ht->table = (Ivy_hash_entry **)calloc(starting_size, sizeof(Ivy_hash_entry *));
	ivy_create_allocator(ht->free_entries, sizeof(Ivy_hash_entry));
	return ht;
}

void ivy_free_ht(Ivy_hash_table *h)
{
	ivy_free_allocator(h->free_entries);
	free(h->table);
	free(h);
}

/* Find an entry */

void *ivy_ht_hval_find(Ivy_hash_table *h, const char *name, unsigned hval)
{
	Ivy_hash_entry *e;
	for (e = h->table[hval & h->size]; e; e=e->next)
		if (!strcmp(e->name, name))
			return e->val;
	return NULL;
}

void *ivy_ht_find(Ivy_hash_table *h, const char *name)
{
	return ivy_ht_hval_find(h, name, ivy_hash(name));
}

/* Expand hash table */

void ivy_ht_expand(Ivy_hash_table *h)
{
	/* Allocate new table */
	unsigned new_size = (h->size + 1) * 2 - 1;
	Ivy_hash_entry **new_table = (Ivy_hash_entry **)calloc(h->size + 1, sizeof(Ivy_hash_entry *));
	/* Copy entries from old table to new */
	int x;
	for (x = 0; x != h->size + 1; ++x) {
		Ivy_hash_entry *e;
		while ((e = h->table[x])) {
			h->table[x] = e->next;
			e->next = new_table[e->hash_val & new_size];
			new_table[e->hash_val & new_size] = e;
		}
	}
	/* Replace old table with new */
	free(h->table);
	h->table = new_table;
	h->size = new_size;
}

/* Add an entry */

void ivy_ht_hval_add(Ivy_hash_table *h, const char *name, unsigned hval, void *val)
{
	Ivy_hash_entry *e;
	e = (Ivy_hash_entry *)ivy_alloc_item(h->free_entries);
	e->name = name;
	e->val = val;
	e->hash_val = hval;
	e->next = h->table[hval & h->size];
	h->table[hval & h->size] = e;
	if (++h->nentries == h->size/2 + h->size/4)
		ivy_ht_expand(h);
}

void ivy_ht_add(Ivy_hash_table *h, const char *name, void *val)
{
	ivy_ht_hval_add(h, name, ivy_hash(name), val);
}

/* Compute hash value */

#define hnext(accu, c) (((accu) << 4) + ((accu) >> 28) + (c))

unsigned ivy_hash(const char *s)
{
	unsigned accu = 0;
	while (*s)
		accu = hnext(accu, *(unsigned char *)s++);
	return accu;
}
