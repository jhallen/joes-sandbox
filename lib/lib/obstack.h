/*
 * Simple fast obstack.
 *
 *	Copyright
 *		(C) 2006 Joseph H. Allen
 */

typedef struct obstack Obstack;

#define OB_PAGE_SIZE 4096

#define ob_align(n) (((n) + 7) & ~7)

/* An obstack page */

struct obpage {
	struct obpage *next;		/* Next page */
	unsigned char *start;		/* Start of malloc block */
	int size;			/* Size of malloc block */
};

/* An obstack */

struct obstack {
	struct obpage *first, *last;	/* Points to first and last page */
	unsigned char *ptr;		/* Current allocation pointer (points in last page) */
	int count;			/* Space remaining in this page */
	int req;			/* ob_malloc request size */
};

/* Create an obstack */

Obstack *mk_obstack();

/* Delete an obstack, and all space allocated in it */

void rm_obstack(Obstack *ob);

/* Allocate a block */

#define ob_malloc(ob, size) \
( \
	((ob)->req = ob_align(size)) <= (ob)->count ? ( \
		(ob)->ptr += (ob)->req, \
		(ob)->count -= (ob)->req, \
		(void *)((ob)->ptr - (ob)->req) \
	) : \
		_ob_malloc(ob) \
)

void *_ob_malloc(Obstack *ob);

/* Free block and all more recently allocated blocks */

void ob_free(Obstack *ob, void *buf);

/* Reallocate a block, especially good for most recently allocated block */

void *ob_realloc(Obstack *ob, void *buf, int old_size, int new_size);
