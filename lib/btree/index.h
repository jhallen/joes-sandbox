/* Dynamic hash index file routines
   Copyright (C) 1992 Joseph H. Allen

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 1, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
more details.

You should have received a copy of the GNU General Public License along with
this program; see the file COPYING.  If not, write to the Free Software
Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Index file manager */

#define IPGSIZE 1024				/* Index page size (in bytes) */
#define IENTRIES (IPGSIZE/sizeof(long)/2-1)	/* Entries/page */

/* An open index file */

typedef struct index INDEX;
struct index
 {
 VFILE *fd;			/* The index file */
 int bits;			/* Number of bits to shift to get hash index */
 unsigned long base;		/* Address of hash table */
 unsigned long start;		/* Offset (in longs) to hash entry 0 */
 unsigned long size;		/* Size of hash table in longs */
 };

INDEX *iopen(char *name);	/* Open index file */

void iclose(INDEX *ndx);	/* Close index file */

void iadd(INDEX *ndx,unsigned long hash,unsigned long addr);
				/* Add entry to index table */

void idel(INDEX *ndx,unsigned long hash,unsigned long addr);
				/* Delete index table entry */

unsigned long *ifind(INDEX *ndx,unsigned long hash,
                     unsigned long *buf,int *bufsiz,int *buflen);
				/* Generate list of entries
				which have matching hash */

void imk(char *name);		/* Create/truncate index file */

void irm(char *name);		/* Delete index file */

/* iadd and idel take a pair of entries 'hash' and 'addr':
 *
 * 'hash' should be a 32 bit hash value for your record's key.  The more
 *        random the hash, the faster the search.
 *
 * 'addr' should be the offset into your datafile to where the record
 *        is stored
 *
 * ifind searches an index file and returns a list of 'addr' values
 * for possible record matches.  After an ifind, you should check each
 * record ifind() returns as a possible match until you you find the one
 * you want.
 *
 * Ifind uses a malloced array of longs which it automatically resizes if
 * needed.  Call ifind like this:
 *
 * unsigned long *buf=0;
 * int siz=0;	/* Malloc size of buf */
 * int len=0;	/* No. entries found */
 *
 * buf=ifind(f,hash1,buf,&siz,&len);
 *
 * ...
 *
 * buf=ifind(f,hash2,buf,&siz,&len);
 *
 */
