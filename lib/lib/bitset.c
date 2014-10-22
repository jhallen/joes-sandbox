#include <stdlib.h>
#include <string.h>
#include "bitset.h"

/* Create a bitset- all bits start out clear */

Bitset *mk_bitset(int prealloc)
{
	Bitset *set = (Bitset *)malloc(sizeof(Bitset));
	if (prealloc < 4)
		prealloc = 4;
	set->size = prealloc;
	set->map = (unsigned char *)calloc(set->size, 1);
	return set;
}

void rm_bitset(Bitset *set)
{
	free(set->map);
	free(set);
}

void bitset_expand(Bitset *set, int size)
{
	if (size > set->size) {
		set->map = (unsigned char *)realloc(set->map, size);
		while (set->size < size)
			set->map[set->size++] = 0;
	}
}

/* Set a bit */

void bitset_set(Bitset *set, int bit)
{
	int byte = (bit >> 3);
	bit = (1 << (bit & 7));

	if (byte >= set->size)
		bitset_expand(set, byte + 1);

	set->map[byte] |= bit;
}

/* Clear a bit */

void bitset_clr(Bitset *set, int bit)
{
	int byte = (bit >> 3);
	bit = (1 << (bit & 7));

	if (byte >= set->size)
		bitset_expand(set, byte + 1);

	set->map[byte] &= ~bit;
}

/* Test a bit */

int bitset_tst(Bitset *set, int bit)
{
	int byte = (bit >> 3);
	bit = (1 << (bit & 7));

	if (byte >= set->size)
		return 0;
	else if (set->map[byte] & bit)
		return 1;
	else
		return 0;
}

/* True if two bitsets are the same */

int bitset_cmp(Bitset *a, Bitset *b)
{
	if (a->size > b->size) {
		int e;
		if (memcmp(a->map, b->map, b->size))
			return 0;
		for (e = b->size; e != a->size; ++e)
			if (a->map[e])
				return 0;
		return 1;
	} else {
		int e;
		if (memcmp(a->map, b->map, a->size))
			return 0;
		for (e = a->size; e != b->size; ++e)
			if (b->map[e])
				return 0;
		return 1;
	}
}

/* Copy a bitmap */

Bitset *bitset_dup(Bitset *a)
{
	Bitset *n = mk_bitset(a->size);
	memcpy(n->map, a->map, a->size);
	return n;
}
