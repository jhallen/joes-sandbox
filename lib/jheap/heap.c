/* Fast space-efficient heap manager
   Copyright (C) 1992 Joseph H. Allen

   Berkeley random number generator (C) Regent's of UCB

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
675 Mass Ave, Cambridge, MA 02139, USA.

11/04/93 - JHA - We now align on long bounderies - because on the @#@%#! Alpha,
                 'int' is 32 bits, not 64 like it should be
*/

// #include "config.h"

typedef unsigned long size_t;
#define MAX_SIZE_T (~(size_t)0)

#define normalize(x) (x)

void *malloc(size_t len);
void *realloc(void *ptr, size_t len);
void *calloc(size_t a, size_t b);
void free(void *ptr);

#define MAXSIZE (MAX_SIZE_T-127)	/* Largest allowable block size */

/* If an unfreed block is larger than BAL(size), the balance of the block is
 * returned to the heap.  'size' is the requested block size. */
#define BAL(size) ((size)+((size)>>2))

/** Berekely random number generator */

#define	DEG 31
#define	SEP 3

struct state {
	long state[DEG];
	long *fptr;
	long *rptr;
};

static struct state state = {
	{
	 0x9a319039, 0x32d9c024, 0x9b663182, 0x5da1f342,
	 0xde3b81e0, 0xdf0a6fb5, 0xf103bc02, 0x48f340fb,
	 0x7449e56b, 0xbeb1dbb0, 0xab5c5918, 0x946554fd,
	 0x8c2e680f, 0xeb3d799f, 0xb11ee0b7, 0x2d436b86,
	 0xda672e2a, 0x1588ca88, 0xe369735d, 0x904f35f7,
	 0xd7158fd6, 0x6fa6f051, 0x616e6b96, 0xac94efdc,
	 0x36413f93, 0xc622c298, 0xf5a42ab8, 0x8a88d77b,
	 0xf5ad9d0e, 0x8999220b, 0x27fb47b9},
	&state.state[SEP],
	&state.state[0]
};

static long hrandom()
{
	long i = ((*state.fptr += *state.rptr) >> 1) & 0x7FFFFFFF;
	if (++state.fptr == &state.state[DEG])
		state.fptr = state.state, ++state.rptr;
	else if (++state.rptr == &state.state[DEG])
		state.rptr = state.state;
	return i;
}

/* A free block of memory */

/** Phong Voo's Suggested alignment unit **/

union _align_ {
	char c, *cp;
	int i, *ip;
	long l, *lp;
	double d, *dp, ***dppp[8];
	void (*fn) ();
	union _align_ *align;
};
struct _a_ {
	char c[2];		/* ALIGN must be at least 2 */
	union _align_ a;
};
#define ALIGN	(sizeof(struct _a_) - sizeof(union _align_))

typedef union size SIZE;
typedef struct freeblck FREE;

// There is always an allocated block at the end of the heap

union size {
	size_t size; // LSB set if block is allocated
	char align[ALIGN];
};

struct freeblck {
	union size u;		/* Includes self, +1 for allocated blocks */
	/* equals '1' for end of heap or segment */

	FREE *next;		/* Data part of allocated blocks begins here */
	FREE *prev;		/* Otherwise it's a doubly linked list */
};

static int reent = 0;		/* Set if freeit is reentering itself */
char *heapend = 0;		/* Top of heap */

/* A skiplist entry */

struct skipnode {
	size_t key;
	int nptrs;
	FREE *value;
	struct skipnode *ptrs[1];
};

/* Max number of pointers in a skiplist node */
#define SKIPDPTH 16

static struct skipnode *freeones[SKIPDPTH + 1];	/* Free lists of skip nodes */
static struct skipnode *top = 0;	/* Skip list root node */
static struct skipnode nil;	/* Ending skiplist node */
static struct skipnode *update[SKIPDPTH];	/* Record of search path */

/* Memory allocator for skip-list nodes */

static char *big = 0;
static int bigx = 0;

/* Allocation amount for skiplist data */
#define SKIPCHNK 256

/* Key is for recomputing skip-list search after a reentrant malloc */

static struct skipnode *skipa(size_t x, size_t key)
{
	struct skipnode *t;
	int amnt;
	if (freeones[x]) {
		t = freeones[x];
		freeones[x] = (struct skipnode *) t->value;
		return t;
	}
	amnt = sizeof(struct skipnode) + sizeof(struct skipnode *) * (x - 1);
	if (bigx + amnt > SKIPCHNK || !big) {
		reent = 1;
		big = (char *) malloc(SKIPCHNK);
		if (key) {
			struct skipnode *t = top;
			int y;
			for (y = top->nptrs; --y >= 0;) {
				while (t->ptrs[y]->key < key)
					t = t->ptrs[y];
				update[y] = t;
			}
		}
		reent = 0;
		bigx = 0;
	}
	t = (struct skipnode *) (big + bigx);
	bigx += amnt;
	return t;
}

/* Free a skip node */

static void freea(struct skipnode *t)
{
	t->value = (FREE *) freeones[t->nptrs];
	freeones[t->nptrs] = t;
}

/* Insert a free block into the free-list */

static void freeit(FREE *b)
{
	struct skipnode *t = top;
	int x;
	b = normalize(b);
	for (x = top->nptrs; --x >= 0;) {
		while (t->ptrs[x]->key < b->u.size)
			t = t->ptrs[x];
		update[x] = t;
	}
	t = t->ptrs[0];
	if (t->key == b->u.size) {
		b->next = t->value;
		b->prev = 0;
		t->value->prev = b;
		t->value = b;
	} else {
		int level;
		for (level = 1; level != 15 && hrandom() < 0x29000000L; ++level);
		b->u.size += 1;
		t = (struct skipnode *) skipa(level, b->u.size - 1);
		b->u.size -= 1;
		while (level > top->nptrs)
			update[top->nptrs++] = top;
		t->nptrs = level;
		for (x = 0; x != level; ++x)
			t->ptrs[x] = update[x]->ptrs[x], update[x]->ptrs[x] = t;
		t->key = b->u.size;
		t->value = b;
		b->next = 0;
		b->prev = 0;
	}
}

/* Remove an element from the free list */

static void unfreeit(FREE *b)
{
	struct skipnode *t;
	int y;
	if (b->prev) {
		if ((b->prev->next = b->next) != 0)
			b->next->prev = b->prev;
		return;
	}
	t = top;
	for (y = top->nptrs; --y >= 0;) {
		while (t->ptrs[y]->key < b->u.size)
			t = t->ptrs[y];
		update[y] = t;
	}
	t = t->ptrs[0];
	if (b->next)
		b->next->prev = 0, t->value = b->next;
	else {
		int x;
		for (x = 0; x != top->nptrs && update[x]->ptrs[x] == t; ++x)
			update[x]->ptrs[x] = t->ptrs[x];
		while (top->nptrs && top->ptrs[top->nptrs - 1] == &nil)
			--top->nptrs;
		freea(t);
	}
}

/* Allocate a block of memory */

void *malloc(size_t size)
{
	int x;
	int y;
	FREE *b;
	struct skipnode *t;
	/* Initialize database if this is first call to heap manager */
	if (!top) {
		nil.key = MAX_SIZE_T;
		nil.nptrs = 0;
		nil.ptrs[0] = &nil;
		top = &nil;
		top = (struct skipnode *) skipa(16, 0);
		top->key = 0;
		top->nptrs = 0;
		for (x = 0; x != 16; ++x)
			top->ptrs[x] = &nil;
	}

	if (size > MAXSIZE) {
		write(2, "Invalid parameter to malloc\n", 28);
		_exit(1);
	}
	/* Add allocated block overhead to size, make sure size is at least large
	 * enough to hold a free block header, and round size up to multiple of ALIGN
	 */
	size += sizeof(SIZE);
	if (size < sizeof(FREE))
		size = sizeof(FREE);
	size = (size + ALIGN - 1) - (size + ALIGN - 1) % ALIGN;

      loop:
	/* Try to find block in database */
	b = 0;
	t = top;
	for (y = top->nptrs; --y >= 0;) {
		while (t->ptrs[y]->key < size)
			t = t->ptrs[y];
		update[y] = t;
	}
	t = t->ptrs[0];
	if (t->key != MAX_SIZE_T) {	/* Remove block from database */
		b = t->value;
		if (b->next)
			b->next->prev = 0, t->value = b->next;
		else {
			for (x = 0; x != top->nptrs && update[x]->ptrs[x] == t; ++x)
				update[x]->ptrs[x] = t->ptrs[x];
			while (top->nptrs && top->ptrs[top->nptrs - 1] == &nil)
				--top->nptrs;
			freea(t);
		}
	}

	if (b) {		/* We found a block */
		/* First, coalesce it with any free blocks which follow it */
		while (!(1 & ((FREE *) ((char *) b + b->u.size))->u.size)) {
			FREE *c = (FREE *) ((char *) b + b->u.size);
			/* Not possible for this to happen */
			/* if (b->u.size + c->u.size > MAXSIZE || b->u.size + c->u.size <= 0)
				break; */
			b->u.size += c->u.size;
			unfreeit(c);
		}
		/* If the block is at the end of the heap, give it back with sbrk() */
		/* And then try allocating again.. */
		if (normalize((char *) b + b->u.size) + sizeof(SIZE) == heapend) {
			sbrk(-b->u.size);
			heapend = (char *) b;
			b->u.size = 1;
			goto loop;
		}
		/* Return balance of the block back to the heap */
		if (b->u.size >= BAL(size) && b->u.size - size >= sizeof(FREE) && !reent) {
			FREE *c = (FREE *) ((char *) b + size);
			c->u.size = b->u.size - size;
			b->u.size = size | 1;
			freeit(c);
		} else
			b->u.size |= 1;	/* Mark block as allocated */
		return (void *) ((char *) b + sizeof(SIZE));
	}

	/* No suitable block in database: use sbrk */
	b = (FREE *) sbrk(size + sizeof(SIZE));
	if (!b || b == ((FREE *) (long) -1)) {
#ifdef MEMOUT
		write(2, "Out of memory\n", 14);
		_exit(1);
#else
		return 0;
#endif
	}
	b = normalize(b);
	if (normalize(heapend) == (char *) b) {
		b = (FREE *) (heapend - sizeof(SIZE));
		size += sizeof(SIZE);
	}
	heapend = normalize((char *) b + size) + sizeof(SIZE);
	((FREE *) (heapend - sizeof(SIZE)))->u.size = 1;
	b->u.size = size | 1;
	return (void *) ((char *) b + sizeof(SIZE));
}

void *realloc(void *blk, size_t size)
{
	FREE *b;
	size_t osize;
	if (!blk)
		return malloc(size);	/* Simply malloc if blk is NULL */

	b = (FREE *) ((char *) blk - sizeof(SIZE));	/* Get address of block */
	osize = b->u.size - 1 - sizeof(SIZE);	/* Get original block size */

	if (size > MAXSIZE) {
		write(2, "Invalid parameter to realloc\n", 29);
		_exit(1);
	}
	size += sizeof(SIZE);	/* Include overhead in req. size */
	if (size < sizeof(FREE))
		size = sizeof(FREE);	/* Make sure blk is large enough
					   to freed lator */
	size = (size + ALIGN - 1) - (size + ALIGN - 1) % ALIGN;	/* Round up to alignment */

	/* Append any free blocks which immediately follow this block */
	while (!(1 & ((FREE *) ((char *) b + b->u.size - 1))->u.size)) {
		FREE *c = (FREE *) ((char *) b + b->u.size - 1);
		/* Not possible for this to happen */
		/* if (b->u.size + c->u.size > MAXSIZE || b->u.size + c->u.size < 0)
			break; */
		b->u.size += c->u.size;
		unfreeit(c);
	}
	if (size <= b->u.size - 1) {	/* If block is larger than requested size... */
		/* Free the unused space in the block if it's large enough to be a free
		 * block and if it's larger than 100% of the requested new size (this is
		 * an arbitrary value which seems to work well) */
		if (b->u.size - 1 - size > size && b->u.size - 1 - size >= sizeof(FREE)) {
			FREE *c = (FREE *) ((char *) b + size);
			c->u.size = b->u.size - size - 1;
			b->u.size = size + 1;
			freeit(c);
		}
		/* Block has not moved:  return original address */
		return blk;
	} else {		/* If block is smaller than requested new size... */

		char *n = (char *) malloc(size);	/* Allocate a new block */
		FREE *c;
		if (!n) {
			free(blk);
			return (void *) n;
		}
		c = (FREE *) (n - sizeof(SIZE));	/* Get address of block */
		/* If the new block is at end of heap and adjacent to org block, simply
		 * extend the heap */
		if (normalize((char *) c + c->u.size - 1) + sizeof(SIZE) == heapend && 
		    (FREE *) normalize(((char *) b + b->u.size - 1)) == c) {
			sbrk(-(c->u.size - 1 + b->u.size - 1 - size));	/* Adjust heap */
			b->u.size = size + 1;	/* Change size of org. block */
			heapend = normalize((char *) b + b->u.size - 1) + sizeof(SIZE);	/* New heapend */
			((FREE *) (heapend - sizeof(SIZE)))->u.size = 1;	/* End of heap flag */
			return blk;
		} else {	/* We didn't allocate a new block at end of heap adjacent to org
				   block, so simply copy data to new block */
			memcpy(n, blk, osize);	/* Copy data */
			free(blk);	/* Free org block */
			return (void *) n;	/* Return new block */
		}
	}
}

void free(void *blk)
{
	FREE *b;
	if (!blk)
		return;		/* Ignore NULLs */
	b = (FREE *) ((char *) blk - sizeof(SIZE));	/* Get address of block */
	b->u.size -= 1;		/* Clear allocated flag */
	/* Append any free blocks which immediately follows this block */
	while (!(1 & ((FREE *) ((char *) b + b->u.size))->u.size)) {
		FREE *c = (FREE *) ((char *) b + b->u.size);
                /* This can't happen.. */
		/* if (b->u.size + c->u.size > MAXSIZE || b->u.size + c->u.size <= 0)
			break; */
		b->u.size += c->u.size;
		unfreeit(c);
	}
	if (normalize((char *) b + b->u.size) + sizeof(SIZE) == heapend)
		/* If block is at end of heap... */
	{
		heapend = (char *) b + sizeof(SIZE);	/* Shrink the heap */
		sbrk(-b->u.size);
		b->u.size = 1;
	} else
		freeit(b);	/* Otherwise put block into free list */
}

void *calloc(size_t a, size_t b)
{
	void *z = malloc(a * b);
	return z ? memset(z, 0, a * b) : 0;
}
