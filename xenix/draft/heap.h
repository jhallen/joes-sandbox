/* Fast space-efficient heap manager
 *
 * By: Joseph H. Allen
 *     Aug 7, 1992
 *
 * - Uses "best-fit" algorithm- but maintains a fast skip-list database of each
 *   set of equal sized free blocks.  This eliminates the long linked-list
 *   search delay which plagues other space efficient algorithms. 
 * - Contiguous free blocks get coalesced
 * - Gives memory back to operating system when it can
 * - Overhead for allocated blocks: sizeof(int)
 * - Minimum allocated block size is 2*sizeof(int) (not including overhead)
 * - Blocks are always aligned on integer boundaries
 * - Macro provided for reading block size
 * - 'memalign' routine provided for allocating blocks on a given alignment
 *
 * Limitations:
 * - You may not use 'sbrk' or 'brk' after the first time any of these routines
 *   are called.
 *
 * - Maximum block size is maximum signed integer size (MAXINT)
 */

#ifndef _Iheap
#define _Iheap 1

#include "config.h"

/* void (*mtrap)(int size);
 *
 * If 'mtrap' is set to 0 (it's default value), then if the system runs
 * out of memory, an error message is sent to stderr and the program is
 * aborted.
 *
 * If 'mtrap' is set to the address of a function, then that function gets
 * one chance (mtrap will only ever be called once for each out of memory
 * condition and it will never be reentered) to free up at least 'size' bytes
 * of memory.  If 'mtrap' doesn't free up the memory or if there isn't enough
 * contiguous space to satisfy the allocation request, the default action is
 * taken.  The 'mtrap' function may call the memory allocation routines (but
 * hopefully with smaller requests :-).
 */
extern void (*mtrap)();

/* void *malloc(int size);
 *
 * Allocate a block of at least 'size' bytes from the heap.  If 'size' is zero,
 * a small block is allocated.
 *
 * The allocated block will begin on an address which is a multiple of
 * sizeof(int).
 */
void *malloc();

/* void *calloc(int a,int b);
 *
 * Allocate a block of at least 'a'*'b' bytes from the heap.  The first 'a'*'b'
 * bytes are cleared.  If 'a'*'b' is zero, a small block is allocated (and it
 * is not cleared).
 *
 * The allocated block will begin on an address which is a multiple of
 * sizeof(int).
 */
void *calloc();

/* void *realloc(void *blk,int size);
 *
 * Modify amount allocated to 'blk' to 'size' and return pointer to block
 * (which might be at a different address).  If 'size' is zero, a small amount
 * is left allocated to the block.  If 'blk' is null, a new block is allocated.
 *
 * The allocated block will begin on an address which is a multiple of
 * sizeof(int).
 */
void *realloc();

/* void free(void *blk);
 *
 * Return a block to the heap so that it can be reused.  If 'blk'==0,
 * nothing happens.
 */
void free();

/* void *memalign(int align,int size);
 *
 * Allocate a block of at least 'size' bytes beginning on an address which
 * is a multiple of 'align'.  'align' is rounded up to a multiple of
 * sizeof(int).  If 'size' is zero, a small block is allocated.
 */
void *memalign();

/* int msize(void *blk);
 *
 * Return size of a block returned by malloc/realloc/calloc/memalign.  Returns
 * zero if 'blk' is 0.  Warning: this is an unsafe macro (blk can be
 * accessed more than once).
 */
#define msize(blk) ((blk)?*((int *)(blk)-1)-(1+sizeof(int)):0)

#endif
