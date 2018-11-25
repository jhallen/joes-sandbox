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

typedef struct ivy_frag Ivy_frag;

struct ivy_frag {
	unsigned char *begcode;
	size_t code;
	size_t codesize;

	int rtn;
	int scopelvl;
	struct ivy_looplvl *looplvls;
};

/* Initialize a fragment */

void ivy_setup_frag(Ivy_frag *);

/* Generate byte offset you need to add to p so that
 * it is an exact multiple of size (which is a power of 2).
 *
 * (it's critical that this is branch-free)
 */

#define ivy_align_o(p, size) (((long)(size) - 1) & -(long)(p))

/* Align frag to next multiple of n */

void ivy_align_frag(Ivy_frag *f, int n);

/* Append data to a fragment: return byte offset to data */

int ivy_emitc(Ivy_frag *f, int c);
int ivy_emitl(Ivy_frag *f, long long n);
int ivy_emitn(Ivy_frag *f, int n);
int ivy_emitd(Ivy_frag *f, double d);
int ivy_emitp(Ivy_frag *f, void *p);
int ivy_emits(Ivy_frag *f, char *s, int len);

/* Access data in a fragment */

#define ivy_fragc(f, ofst) (*((f)->begcode + (ofst)))
#define ivy_fragn(f, ofst) (*(int *)((f)->begcode + (ofst)))
#define ivy_fragd(f, ofst) (*(double *)((f)->begcode + (ofst)))
#define ivy_fragp(f, ofst) (*(void **)((f)->begcode + (ofst)))
