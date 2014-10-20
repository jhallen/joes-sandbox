/* B-Tree library
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

#define BTPGSIZE 1024	/* This must be a power of 2 less than or equal to
			   PGSIZE (see 'vfile.h') */

/* B-Tree file format--

A B-Tree is broken up into pages of size BTPGSIZE.  See 'vfile.h'

'Address' in this context refers to offset into the btree file.
'zstring' is just a normal zero-terminated C-string.

Header-page (always the first page in the file):
	long		Address of root Tree-page or zero if none
	long		Address of first Free-page or zero if none
	long		Total number of entries in the B-Tree
	zstring		Column names
	zstring		Column attributes

Free-page:
	long		Address of next free page or zero if this is the
			last Free-page

Tree-page
	long		Address of page to left of this or zero if none.
	short		Offset from start of page to data area / end of headers
	short		Offset from start of page to end of data area
	
	A variable length table of entry headers, one for each entry in
	this page:

	    short		Offset from start of page to data
	    long		Address of page to right of this entry or zero
	    			if none

	The rest of the page is used to store the entries.  They appear
	in the same order as the headers.

	char		Data area

*/

#ifndef _Ibtree
#define _Ibtree 1

#include "config.h"
#include "vfile.h"

typedef struct btree BTREE;

struct btree
 {
 VFILE *fd;			/* Open file */

 /* This data is copied from the header page */
 char **colnames;		/* Column names */
 char **colattribs;		/* Column attributes */
 long top;			/* Top-level btree page */
 long free;			/* First page in free list */
 long nrecs;			/* Total number of records */

 /* A stack of pointers leading to the current entry.  Each pointer is
  * the sum of the file address to the tree page plus the page offset to
  * the entry header.
  */
 int stkptr;			/* Stack pointer */
 				/* Current position is stk[stkptr-1] */
 long stk[42];			/* Position stack */
 };

/* BTREE *btopen(char *name);
 *
 * Open an existing btree file.  Return NULL if couldn't open file.
 *
 * Once a btree file is open, the column name and attribute fields (see
 * btcreat) become available in the 'colnames' and 'colattribs' arrays.  The
 * number of fields in each of these arrays can be read with the aLEN macro
 * from va.h.  For example:
 *
 * BTREE *f=btopen("test");
 * number_of_columns=aLEN(f->colnames);
 * for(x=0;x!=number_of_columns;++x)
 *  printf("Column %d's name is %s\n",x,f->colnames[x]);
 */
BTREE *btopen();

/* void btcreat(char *name,char *colnames,char *colattribs);
 *
 * Create a btree file with the given column name and attribute strings.  These
 * are strings of colon seperated fields like "name:addr:no" and 
 * "string:string:int".  These strings are simply stored in the btree file
 * header and are not used for anything.  They are however, available for
 * reading once a database is opened: see 'btopen'.
 */
void btcreat();

/* void btclose(BTREE *tree);
 *
 * Close a btree file.
 */
void btclose();

/* char *btread(BTREE *tree,char *buf);
 *
 * Read the btree entry at the current position.  The variable length string
 * 'buf' is used to store the entry (see vs.h).  If the current position is
 * the end of the btree file, NULL is returned.  Otherwise the updated 'buf'
 * is returned.
 *
 * For example:
 *  /* Print entire btree file */
 *  char *buf=0;
 *  BTREE *f=btopen("test");
 *  while(buf=btread(f,buf)) printf("%s\n",buf), btfwrd(f);
 *  vsrm(buf); /* Free 'buf' */
 *  btclose(f);
 */
char *btread();

/* int btfwrd(BTREE *tree);
 *
 * Step the current position forwards to the next entry.  1 is returned for
 * success, 0 is returned if we were at the end of the file.
 */
int btfwrd();

/* int btbkwd(BTREE *tree);
 *
 * Step the current position backwards to the previous entry.  1 is returned
 * for success, 0 is returned if we were at the beginning of the file.
 */
int btbkwd();

/* int btfirst(BTREE *tree);
 *
 * Set the position to the first entry in the btree.  1 is returned for
 * success, 0 is returned if there was no entries in the btree.
 */
int btfirst();

/* int btlast(BTREE *tree);
 *
 * Set the current position to the end of btree (after the last entry).  1
 * is returned for success, 0 is returned if there was no entries in the
 * btree.
 */
int btlast();

/* int btsearch(BTREE *tree,
               int (*cmp)(void *cmpptr,char *a,int asz,char *b,int bsz),
               void *cmpptr,
               char *data,int datasz);
 *
 * Search for data/datasz and set the current position to the position where
 * it was found or the position where it should be placed.  1 is returned if
 * the entry was found.  0 is returned if it wasn't.
 *
 * 'cmp' is used as the comparison function.  It should return
 * -1 for less than, 0 for equal and 1 for greater.  a/asz and b/bsz are
 * the arguments to be compared.  'cmpptr' is simply passed as the first
 * argument to the compare function and can be used to pass configuration
 * information (such as which field in the record is the key-field) to a 
 * comparison function.
 *
 * For example:
 *
 * /* Comparison function */
 * int cmp(obj,a,az,b,bz)
 * void *obj;
 * char *a, *b;
 *  {
 *  int x;
 *  for(x=0;x!=az && x!=bz;++x)
 *   if(a[x]>b[x]) return 1;
 *   else if(a[x]<b[x]) return -1;
 *  if(az>bz) return 1;
 *  else if(az<bz) return -1;
 *  return 0;
 *  }
 *
 * BTREE *f=btopen("test");
 * if(btsearch(f,cmp,NULL,"Joe Allen",strlen("Joe Allen")))
 *  printf("Record found!\n");
 * btclose(f);
 */
int btsearch();

/* void btins(BTREE *tree,char *data,int size);
 *
 * Insert a record at the current position.
 *
 * Call btsearch() first to set the current position to the correct place.
 * If the current position is already at the correct place, btsearch()
 * does not have to be called.
 *
 * The maximum size of the record is (BTPGSIZE-16).
 *
 * Each record inserted can be a different size.
 */
void btins();

/* int btdel(BTREE *tree);
 *
 * Delete the entry at the current position.  1 is returned for success, 0
 * is returned if we were at the end of the file.
 */
int btdel();

#endif
