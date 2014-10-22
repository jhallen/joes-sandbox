/*
 * Simple fast obstack.
 *
 *	Copyright
 *		(C) 2006 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

#include <stdlib.h>
#include <string.h>
#include "obstack.h"

/* Create an obstack */

Obstack *mk_obstack()
{
	Obstack *ob = (Obstack *)malloc(sizeof(Obstack));
	ob->first = ob->last = (struct obpage *)malloc(sizeof(struct obpage));
	ob->last->next = 0;
	ob->last->start = ob->ptr = (unsigned char *)malloc(ob->last->size = ob->count = OB_PAGE_SIZE);
	return ob;
}

/* Free an obstack */

void rm_obstack(Obstack *ob)
{
	struct obpage *p, *n;
	for (p = ob->first; p; p = n) {
		n = p->next;
		free(p->start);
		free(p);
	}
	free(ob);
}

/* ob_malloc() helper function */

void *_ob_malloc(Obstack *ob)
{
	void *buf;
	struct obpage *p;
	int page_size;

	/* Allocate a new page */
	p = (struct obpage *)malloc(sizeof(struct obpage));
	if (ob->req > OB_PAGE_SIZE / 2)
		page_size = ob->req + OB_PAGE_SIZE;
	else
		page_size = OB_PAGE_SIZE;
	p->next = 0;
	p->start = ob->ptr = (unsigned char *)malloc(p->size = ob->count = page_size);
	ob->last->next = p;
	ob->last = p;

	/* Allocate block */
	ob->ptr += ob->req;
	ob->count -= ob->req;
	
	return (void *)(ob->ptr - ob->req);
}

/* Reallocate a block: if you're not reallocing the most recently allocated block,
   the new block will end up in a different allocation order- so you get unexpected
   results if you use ob_free on the block returned by ob_realloc */

void *ob_realloc(Obstack *ob, void *buf, int old_size, int new_size)
{
	unsigned char *blk = buf;
	unsigned char *new_blk;
	old_size = ob_align(old_size);
	new_size = ob_align(new_size);
	if (blk + old_size == ob->ptr) {
		/* We're reallocing the very last item */
		if (new_size - old_size <= ob->count) {
			/* It fits in current block */
			ob->ptr += new_size - old_size;
			ob->count -= new_size - old_size;
			return buf;
		}
		if (blk == ob->last->start) {
			/* The entire page is for just this block- we can realloc it */
			ob->last->start = (unsigned char *)realloc(ob->last->start, ob->last->size = new_size * 2);
			ob->ptr = ob->last->start + new_size;
			ob->count = ob->last->size - new_size;
			return (void *)ob->last->start;
		}
	}
	new_blk = ob_malloc(ob, new_size);
	if (old_size)
		memcpy(new_blk, blk, old_size);
	return (void *)new_blk;
}

/* Free a block and all more recent blocks */

void ob_free(Obstack *ob, void *buf)
{
	unsigned char *blk = (unsigned char *)buf;
	struct obpage *p;
	struct obpage *q, *r;

	/* Find page (segfaults if block is not in the obstack) */
	for (p = ob->first; blk >= p->start && blk < p->start + p->size; p = p->next);

	/* Free all remaining pages */
	for (q = p->next; q; q = r) {
		r = q->next;
		free(q->start);
		free(q);
	}

	/* p becomes new last page */
	ob->last = p;
	p->next = 0;
	ob->ptr = blk;
	ob->count = p->size - (blk - p->start);
}
