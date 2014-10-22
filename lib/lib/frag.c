/* Byte code fragment construction functions */

#include <stdlib.h>
#include "obstack.h"
#include "frag.h"

/* Initialize a fragment */

void iz_frag(Obstack *ob, Frag *f, int alignment)
{
	f->ob = ob;
	f->start = ob_malloc(ob, f->size = 1024);
	f->len = 0;
	f->align = alignment;
}

void fin_code(Frag *f)
{
	f->start = ob_realloc(f->ob, f->start, f->size, f->len);
	f->size = f->len;
}

/* Expand a fragment */

/* Expand code block by at least 'size' words */

void expand_frag(Frag *frag, int size)
{
	int org = frag->size;
	if ((frag->size >> 1) > size)
		// Grow by 50%
		frag->size = frag->size + (frag->size >> 1);
	else
		// Grow by 50% plus requested size
		frag->size += frag->size + (frag->size >> 1);

	frag->start = (unsigned char *)ob_realloc(frag->ob, frag->start, org, frag->size);
}

/* Emit a byte: this does no alignment */

int emitb_noalign(Frag *f, int c)
{
	int start;
	if (f->len + sizeof(unsigned char) > f->size)
		expand_frag(f, sizeof(unsigned char));
	start = f->len;
	f->start[f->len++] = c;
	return start;
}

/* Align to some power of 2 */

void align_frag(Frag *f,int alignment)
{
	int x;
	int add = align_o(f->len, alignment);
	for (x = 0; x != add; ++x)
		emitb_noalign(f, '-');
}

/* Emit a byte and align */

int emitb(Frag *f, int c)
{
	int ofst = emitb_noalign(f, c);
	if (f->len & (f->align - 1))
		align_frag(f, f->align);
	return ofst;
}

/* Emit a short */

int emith(Frag *f, int c)
{
	int start;
	if (f->len & (sizeof(short) - 1))
		align_frag(f, sizeof(short));
	if (f->len + sizeof(short) > f->size)
		expand_frag(f, sizeof(short));
	start = f->len;
	*(short *)(f->start + f->len) = c;
	f->len += sizeof(short);
	if (f->len & (f->align - 1))
		align_frag(f, f->align);
	return start;
}

/* Emit an integer */

int emiti(Frag *f, int c)
{
	int start;
	if (f->len & (sizeof(int) - 1))
		align_frag(f, sizeof(int));
	if (f->len + sizeof(int) > f->size)
		expand_frag(f, sizeof(int));
	start = f->len;
	*(int *)(f->start + f->len) = c;
	f->len += sizeof(int);
	if (f->len & (f->align - 1))
		align_frag(f, f->align);
	return start;
}

/* Emit a double */

int emitd(Frag *f, double d)
{
	int start;
	if (f->len & (sizeof(double) - 1))
		align_frag(f, sizeof(double));
	if (f->len + sizeof(double) > f->size)
		expand_frag(f, sizeof(double));
	start = f->len;
	*(double *)(f->start + f->len) = d;
	f->len += sizeof(double);
	if (f->len & (f->align - 1))
		align_frag(f, f->align);
	return start;
}

/* Emit a pointer */

int emitp(Frag *f, void *p)
{
	int start;
	if (f->len & (sizeof(void *) - 1))
		align_frag(f, sizeof(void *));
	if (f->len + sizeof(void *) > f->size)
		expand_frag(f, sizeof(void *));
	start = f->len;
	*(void **)(f->start + f->len) = p;
	f->len += sizeof(void *);
	if (f->len & (f->align - 1))
		align_frag(f, f->align);
	return start;
}

/* Append a string to the code block */

int emits(Frag *f, unsigned char *s, int len)
{
	int start;

	start = emiti(f, len);

	if (f->len + len + 1 > f->size)
		expand_frag(f, len + 1);

	if (len)
		memcpy(f->start + f->len, s, len);

	f->start[f->len + len] = 0;

	f->len += len + 1;

	if (f->len & (f->align - 1))
		align_frag(f, f->align);

	return start;
}

int emit_branch(Frag *f, int target)
{
	int ofst = emiti(f, 0);
	fragi(f, ofst) = target - ofst;
	return ofst;
}

void fixup_branch(Frag *f, int pos)
{
	align_frag(f, sizeof(int));
	fragi(f, pos) = f->len - pos;
}

/* Modify each item in a linked-list to point to here */

void link(Frag *f, int chain)
{
	int ket;
	align_frag(f, sizeof(int));
	ket = f->len;
	while (chain) {
		int next = fragi(f, chain);
		fragi(f, chain) = ket - chain;
		chain = next;
	}
}

int fetchh(Frag *f, int *pcp)
{
	int pc = *pcp;
	int i;
	pc += align_o(pc, sizeof(short));
	i = fragh(f, pc);
	pc += sizeof(short);
	if (pc & (f->align - 1))
		pc += align_o(pc, f->align);
	*pcp = pc;
	return i;
}

int fetchi(Frag *f, int *pcp)
{
	int pc = *pcp;
	int i;
	pc += align_o(pc, sizeof(int));
	i = fragi(f, pc);
	pc += sizeof(int);
	if (pc & (f->align - 1))
		pc += align_o(pc, f->align);
	*pcp = pc;
	return i;
}

void *fetchp(Frag *f, int *pcp)
{
	int pc = *pcp;
	int *p;
	pc += align_o(pc, sizeof(void *));
	p = fragp(f, pc);
	pc += sizeof(void *);
	if (pc & (f->align - 1))
		pc += align_o(pc, f->align);
	*pcp = pc;
	return p;
}
