/* Super-fast block copy and fill functions
 *
 * by: Joseph H. Allen
 * on: January 10, 1992
 */

#ifndef _Iblocks
#define _Iblocks 1

#include "config.h"

/* Warning: Don't use mfwrd or mbkwd for filling an area.  Use them only
 * for copying, inserting or deleting.  Since they copy ints at a time, they
 * will not work for filling.
 */

/* char *mfwrd(char *d,char *s,int sz); Copy 'sz' bytes from 's' to 'd'.
   The copy occures in the forward direction (use for deletes)
   Nothing happens if 'd'=='s' or 'sz'==0
   Returns original value of 'd'
 */
char *mfwrd();

/* char *mbkwd(char *d,char *s,int sz); Copy 'sz' bytes from 's' to 'd'.
   The copy occures in the backward direction (use for inserts)
   Nothing happens if 'd'=='s' or 'sz'==0
   Returns original value of 'd'
 */
char *mbkwd();

/* char *mmove(char *d,char *s,int sz); Copy 'sz' bytes from 's' to 'd'.  Chooses
 * either mbkwd or mfwrd to do this such that the data won't get clobbered.
 */
char *mmove();

/* Use this for non-overlapping copies */
#define mcpy mbkwd

/* char *mset(char *d,char c,int sz); Set 'sz' bytes at 'd' to 'c'.
 * If 'sz'==0 nothing happens
 * Return original value of 'd'
 */
char *mset();

/* int *msetI(int *d,int c,int sz); Set 'sz' ints at 'd' to 'c'.
 * If 'sz'==0 nothing happens
 * Returns orininal value of 'd'
 */

int *msetI();

/* int mcnt(char *blk,char c,int size);
 *
 * Count number of occurances a character appears in a block
 */
int mcnt();

#endif
