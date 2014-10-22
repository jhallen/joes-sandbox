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

#ifndef _JOE_HASH_H
#define _JOE_HASH_H 1

#include "free_list.h"

typedef struct hash_table Hash_table;
typedef struct hash_entry Hash_entry;

struct hash_entry {
	struct hash_entry *next;
	unsigned hash_val;
	char *name;
	void *val;
};

struct hash_table {
	Free_list free_entries[1];
	Hash_entry **table;
	int nentries;
	int size;
};

unsigned hash(char *s);

Hash_table *htmk(int len);
void htrm(Hash_table *ht);

void htaddhval(Hash_table *ht, char *name, unsigned hval, void *val);
void htadd(Hash_table *ht, char *name, void *val);

void *htfindhval(Hash_table *ht, char *name, unsigned hval);
void *htfind(Hash_table *ht, char *name);

void htexpand(Hash_table *ht);

#endif
