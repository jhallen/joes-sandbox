/* Free list

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

#ifndef _Jivy_hash_h
#define _Jivy_hash_h 1

#include "ivy_free_list.h"

typedef struct ivy_hash_table Ivy_hash_table;
typedef struct ivy_hash_entry Ivy_hash_entry;

struct ivy_hash_entry {
	struct ivy_hash_entry *next;
	unsigned hash_val;
	const char *name;
	void *val;
};

struct ivy_hash_table {
	Ivy_free_list free_entries[1];
	Ivy_hash_entry **table;
	int nentries;
	int size;
};

unsigned ivy_hash(const char *s);

Ivy_hash_table *ivy_create_ht(int len);
void ivy_free_ht(Ivy_hash_table *ht);

void ivy_ht_hval_add(Ivy_hash_table *ht, const char *name, unsigned hval, void *val);
void ivy_ht_add(Ivy_hash_table *ht, const char *name, void *val);

void *ivy_ht_hval_find(Ivy_hash_table *ht, const char *name, unsigned hval);
void *ivy_ht_find(Ivy_hash_table *ht, const char *name);

void ivy_ht_expand(Ivy_hash_table *ht);

#endif
