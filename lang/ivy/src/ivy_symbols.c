/* Symbol (interned string) management

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

#include <string.h>
#include "ivy_hash.h"
#include "ivy_symbols.h"

static Ivy_hash_table *symbol_table;

int ivy_symbol_count;

char *ivy_symbol_add(const char *name)
{
	char *s;
	unsigned hval;
	if (!symbol_table)
		symbol_table = ivy_create_ht(1024);
	hval = ivy_hash(name);
	s = (char *)ivy_ht_hval_find(symbol_table, name, hval);
	if (!s) {
		s = strdup(name);
		ivy_ht_hval_add(symbol_table, s, hval, s);
		++ivy_symbol_count;
	}
	return s;
}

char *ivy_symbol_noadd(const char *name)
{
	unsigned hval;
	if (!symbol_table)
		symbol_table = ivy_create_ht(1024);
	hval = ivy_hash(name);
  	return (char *)ivy_ht_hval_find(symbol_table, name, hval);
}
