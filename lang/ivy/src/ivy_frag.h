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

typedef struct frag Frag;

struct frag {
	unsigned char *begcode;
	int code;
	int codesize;

	int rtn;
	int scopelvl;
	struct looplvl *looplvls;
};

/* Initialize a fragment */

void init_frag(Frag *);

/* Generate byte offset you need to add to p so that
 * it is an exact multiple of size (which is a power of 2).
 *
 * (it's critical that this is branch-free)
 */

#define align_o(p, size) (((long)(size) - 1) & -(long)(p))

/* Align frag to next multiple of n */

void align_frag(Frag *f, int n);

/* Append data to a fragment: return byte offset to data */

int emitc(Frag *f, int c);
int emitl(Frag *f, long long n);
int emitn(Frag *f, int n);
int emitd(Frag *f, double d);
int emitp(Frag *f, void *p);
int emits(Frag *f, char *s, int len);

/* Access data in a fragment */

#define fragc(f, ofst) (*((f)->begcode + (ofst)))
#define fragn(f, ofst) (*(int *)((f)->begcode + (ofst)))
#define fragd(f, ofst) (*(double *)((f)->begcode + (ofst)))
#define fragp(f, ofst) (*(void **)((f)->begcode + (ofst)))
