/* Software virtual memory system
   Copyright (C) 1992 Joseph H. Allen

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
675 Mass Ave, Cambridge, MA 02139, USA.  */ 

/* Additions:
 *
 * Should we remove size checking from rc()?  Would make it faster...
 *
 * Should be able to open more than one stream on a file so that vseek
 * doesn't have to get called so much when more than one user is involed
 *
 * Also should have dupopen call to make more streams for a file
 *
 * Make vputs faster
 *
 * Should have a version which will use memory mapped files, if they exist
 * in the os.
 *
 * Would be nice if we could transparantly open non-file streams and pipes.
 * Should there be an buffering option for that?  So we can seek on pipes to
 * get previously read data?
 */

#ifndef _Ivfile
#define _Ivfile 1

#include "config.h"
#include "queue.h"

typedef struct vpage VPAGE;
typedef struct vfile VFILE;

/* These are now defined in config.h */
#ifdef junk
/* Minimum page size for MS-DOS is 128 (for 32K vheaders table) or 256 (for
 * 64K vheaders table) */
#define PGSIZE 512              /* Page size in bytes (Must be power of 2) */
#define LPGSIZE 9               /* LOG base 2 of PGSIZE */
#define ILIMIT (PGSIZE*128L)    /* Max amount to buffer */
#define HTSIZE 128              /* Entries in hash table.  Must be pwr of 2 */
#endif

#define INC 16                  /* Pages to allocate each time */

/* Page header */

struct vpage
 {
 VPAGE *next;			/* Next page with same hash value */
 VFILE *vfile;			/* Owner vfile */
 long addr;			/* Address of this page */
 int count;			/* Reference count */
 int dirty;			/* Set if page changed */
 char *data;			/* The data in the page */
 };

/* File structure */

struct vfile
 {
 LINK(VFILE) link;		/* Doubly linked list of vfiles */
 long size;			/* Number of bytes in physical file */
 long alloc;			/* Number of bytes allocated to file */
 int fd;			/* Physical file */
 int writeable;			/* Set if we can write */
 char *name;			/* File name.  0 if unnamed */
 int flags;			/* Set if this is only a temporary file */

 /* For array I/O */
 char *vpage1;			/* Page address */
 long addr;			/* File address of above page */

 /* For stream I/O */
 char *bufp;			/* Buffer pointer */
 char *vpage;			/* Buffer pointer points in here */
 int left;			/* Space left in bufp */
 int lv;			/* Amount of append space at end of buffer */
 };

extern char *vbase;		/* Data first entry in vheader refers to */
extern VPAGE **vheaders;	/* Array of headers */

/* VFILE *vtmp(V);
 *
 * Open a temporary virtual file.  File goes away when closed.  No actual
 * file is generated if everything fits in memory.
 */
VFILE *vtmp();

/* VFILE *vopen(char *name);
 *
 * Open a file for reading and if possible, writing.  If the file could not
 * be opened, NULL is returned.
 */
VFILE *vopen();

/* long vsize(VFILE *);
 *
 * Return size of file
 */

#define vsize(vfile) \
	( \
	  (vfile)->left<(vfile)->lv ? \
	    (vfile)->alloc+(vfile)->lv-(vfile)->left \
	  : \
	    (vfile)->alloc \
	)

/* void vclose(VFILE *vfile);
 *
 * Close a file.
 */
void vclose();

/* void vlimit(long amount);
 *
 * Set limit (in bytes) on amount of memory the virtual file system may
 * use.  This limit can be exceeded if all existing vpages are being referenced
 * and a new vpage is requested.
 *
 * When vlimit is called, the limit is immediatly enforced by elimiting
 * non-referenced vpages.
 */

void vlimit();

/* void vflsh(void); 
 *
 * Write all changed pages to the disk
 */

void vflsh();

/* void vflshf(VFILE *vfile);
 *
 * Write changed pages for a specific file to the disk
 */

void vflshf();

/* char *vlock(VFILE *vfile,long addr);
 *
 * Translate virtual address to physical address.  'addr' does not have
 * to be on any particular alignment, but if you wish to access more than
 * a single byte, you have to be aware of where page boundaries are (virtual
 * address multiples of PGSIZE).
 *
 * The page containing the data is locked in memory (so that it won't be
 * freed or used for something else) until 'vunlock' is used.
 *
 * Warning:  If you allocate more than one page and use (change) them out of
 * order, vflsh will screw up if writing past the end of a file is illegal
 * in the host filesystem.
 *
 * Also:  This function does not allocate space to the file.  Use valloc()
 * for that.  You can vlock() pages past the allocated size of the file, but
 * be careful when you do this (you normally shouldn't- the only time you
 * ever might want to is to implement your own version of valloc()).
 */

char *vlock();

/* VPAGE *vheader(char *);
 * Return address of page header for given page
 */

#define vheader(p) (vheaders[(physical((char *)(p))-physical(vbase))>>LPGSIZE])

/* void vchanged(char *);
 *
 * Indicate that a vpage was changed so that it will be written back to the
 * file.  Any physical address which falls within the page may be given.
 */

#define vchanged(vpage) ( vheader(vpage)->dirty=1 )

/* void vunlock(char *);
 * Unreference a vpage (call one vunlock for every vlock)
 * Any physical address which falls within the page may be given.
 */

#define vunlock(vpage)  ( --vheader(vpage)->count )

/* void vupcount(char *);
 * Indicate that another reference is being made to a vpage
 */

#define vupcount(vpage) ( ++vheader(vpage)->count )

/* long valloc(VFILE *vfile,long size);
 *
 * Allocate space at end of file
 *
 * Returns file address of beginning of allocated space
 */

long valloc();

/******************************************************************************
 * The folloing functions implement stream I/O on top of the above software   *
 * virtual memory system                                                      *
 ******************************************************************************/

/* void vseek(VFILE *vfile,long addr);
 *
 * Seek to a file address.  Allocates space to the file if you seek past the
 * end.
 */
void vseek();

/* int vrgetc(VFILE *);
 * int vgetc(VFILE *);
 *
 * Get next character / Get previous character functions.
 * They return MAXINT for end of file / beginning of file.
 */

int _vgetc();
int _vrgetc();

#define vrgetc(v) \
        ( (v)->left!=PGSIZE ? ( ++(v)->left, (int)(unsigned)*(--(v)->bufp) ) : _vrgetc(v) )

#define vgetc(v) \
	( (v)->left>(v)->lv ? ( --(v)->left, (int)(unsigned)*((v)->bufp++) ) : _vgetc(v) )

/* int vputc(VFILE *,I);
 *
 * Put character.  Returns character which is written.
 */

int _vputc();

#define vputc(v,c) \
	( \
	  (v)->left ? \
	   ( \
	   --(v)->left, \
	   vchanged((v)->vpage), \
 	   (int)(unsigned)(*((v)->bufp++)=(c)) \
	   ) \
	  : \
	   _vputc((v),(c)) \
	)

/* long vtell(VFILE *);
 *
 * Return current file position
 */

#define vtell(v) \
	( \
	 (v)->vpage ? \
	  ( vheader((v)->vpage)->addr+(v)->bufp-(v)->vpage ) \
	 : \
	  0L \
	)

/* long vgetl(VFILE *);
 *
 * Get long.  No alignment requirements.  Returns -1 if goes past end of file.
 */

long vgetl();

/* short vgetw(VFILE *);
 *
 * Get short.  No alignment requirements.  Returns -1 if goes past end of file.
 */

short vgetw();

/* long vputl(VFILE *,long);
 *
 * Put long.  No alignment requirements.
 * Returns value written.
 */

long vputl();

/* short vputw(VFILE *,short);
 *
 * Put long.  No alignement requirements.
 * Returns value written.
 */

short vputw();

/* char *vgets(VFILE *v,char *s);
 *
 * Read up to next '\n' or end of file into a variable length string.  If 's'
 * is 0, a new string is created.  The \n is not copied into the string.
 *
 * Eliminates the variable length string and returns NULL if
 * vgets is called on the end of the file.
 *
 * This requires that you use the 'vs.h' / 'vs.c' library.
 */

char *vgets();

/* void vputs(VFILE *v,char *s);
 *
 * Write zero terminated string. \n is not appended */

void vputs();

/* void vread(VFILE *,char *,int size);
 *
 * Read bytes from a virtual file into a local data block
 */

void vread();

/* void vwrite(VFILE *,char *,int size);
 *
 * Write bytes from a local data block into a virtual file
 */

void vwrite();

/*************************************************************************** 
 * The following functions implement array I/O on top of the above virtual *
 * memory system (cheap memory mapped files)                               *
 ***************************************************************************/

/* int rc(VFILE *vfile,long addr);
 *
 * Read character.  Returns MAXINT if past end of file.
 */

int _rc();

#define rc(v,a) \
	( \
	  (a)>=vsize(v) ? MAXINT : \
	    ( \
	      (v)->addr==((a)&~(PGSIZE-1)) ? \
	       (v)->vpage1[(a)&(PGSIZE-1)] \
	      : \
	       _rc((v),(a)) \
	    ) \
	)

/* int wc(VFILE *vfile,long addr,char c);
 *
 * Write character.  Return character written.  This automatically allocates
 * space to the file.
 */

int _wc();

#define wc(v,a,c) \
	( \
	  (v)->addr==((a)&~(PGSIZE-1)) ? \
	   ( \
	   vheader((v)->vpage1)->dirty=1, \
	   ((a)+1>vsize(v) && valloc(v,(a)+1-vsize(v))), \
	   (v)->vpage1[(a)&(PGSIZE-1)]=(c) \
	   ) \
	  : \
	   _wc((v),(a),(c)) \
	)

/* long rl(VFILE *vfile,long addr);
 * Read big-endian long.  No alignment requirements.  Returns -1 if goes past
 * end of file.
 */

long rl();

/* long wl(VFILE *vfile,long addr,long c);
 * Write big-endian long.  No alignment requirements.  Automatically expands
 * file if necessary.
 */

long wl();

/* short rw(VFILE *vfile,long addr);
 * Read big-endian short.  No alignment requirements.  Returns -1 if goes past
 * end of file.
 */

short rw();

/* short ww(VFILE *vfile,long addr,short c);
 * Write big-endian short.  No alignment requirements.  Automatically expands
 * file if necessary.
 */

short ww();

#endif
