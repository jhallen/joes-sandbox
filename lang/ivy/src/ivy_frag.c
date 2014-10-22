/* Byte code fragment construction functions

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
#include "ivy_frag.h"

/* Initialize a fragment */

void init_frag(Frag *f)
{
	f->codesize = 128;
	f->begcode = malloc(f->codesize);
	f->code = 0;

	f->rtn = 0;
	f->scopelvl = 0;
	f->looplvls = 0;
}

/* Expand a fragment */

/* Expand code block by at least 'size' words */

void expand_frag(Frag *frag, int size)
{
	if ((frag->codesize >> 1) > size)
		// Grow by 50%
		frag->codesize = frag->codesize + (frag->codesize >> 1);
	else
		// Grow by 50% plus requested size
		frag->codesize += frag->codesize + (frag->codesize >> 1);

	frag->begcode = realloc(frag->begcode, frag->codesize);
}

/* Emit a byte */

int emitc(Frag *f, int c)
{
	int start;
	if (f->code + sizeof(unsigned char) > f->codesize)
		expand_frag(f, sizeof(unsigned char));
	start = f->code;
	f->begcode[f->code++] = c;
	return start;
}

/* Align to some power of 2 */

void align_frag(Frag *f,int alignment)
{
	int x;
	int add = align_o(f->code, alignment);
	for (x = 0; x != add; ++x)
		emitc(f, '-');
}

/* Emit a long long integer */

int emitl(Frag *f, long long c)
{
	int start;
	if (f->code & (sizeof(long long) - 1))
		align_frag(f, sizeof(long long));
	if (f->code + sizeof(long long) > f->codesize)
		expand_frag(f, sizeof(long long));
	start = f->code;
	*(long long *)(f->begcode + f->code) = c;
	f->code += sizeof(long long);
	return start;
}

/* Emit an integer */

int emitn(Frag *f, int c)
{
	int start;
	if (f->code & (sizeof(int) - 1))
		align_frag(f, sizeof(int));
	if (f->code + sizeof(int) > f->codesize)
		expand_frag(f, sizeof(int));
	start = f->code;
	*(int *)(f->begcode + f->code) = c;
	f->code += sizeof(int);
	return start;
}

/* Emit a double */

int emitd(Frag *f, double d)
{
	int start;
	if (f->code & (sizeof(double) - 1))
		align_frag(f, sizeof(double));
	if (f->code + sizeof(double) > f->codesize)
		expand_frag(f, sizeof(double));
	start = f->code;
	*(double *)(f->begcode + f->code) = d;
	f->code += sizeof(double);
	return start;
}

/* Emit a pointer */

int emitp(Frag *f, void *p)
{
	int start;
	if (f->code & (sizeof(void *) - 1))
		align_frag(f, sizeof(void *));
	if (f->code + sizeof(void *) > f->codesize)
		expand_frag(f, sizeof(void *));
	start = f->code;
	*(void **)(f->begcode + f->code) = p;
	f->code += sizeof(void *);
	return start;
}

/* Append a string to the code block */

int emits(Frag *frag, char *s, int len)
{
	int start;

	start = emitn(frag, len);

	if (frag->code + len + 1 > frag->codesize)
		expand_frag(frag, len + 1);

	if (len)
		memcpy(frag->begcode + frag->code, s, len);

	frag->begcode[frag->code + len] = 0;

	frag->code += len + 1;

	return start;
}
