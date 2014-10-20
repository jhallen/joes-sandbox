/* Berkeley random number generator */

#ifndef _Irandom
#define _Irandom 1

#include "config.h"

/* x**31 + x**3 + 1.  */
#define	DEG 31
#define	SEP 3

struct state
 {
 long state[DEG];
 long *fptr;
 long *rptr;
 };

/* State information.  You can save and restore this variable */
extern struct state state;

/* long random(void);
 * Return 31 bit pseudo-random number
 */
long random();

/* void srandom(int seed);
 *
 * Initialize random number generator with specified seed
 *
 * Note that values returned by 'random' are not the entire state, and
 * therefore can not be used as seeds to 'srandom' to revert to a previous
 * state.  Instead, save and restore the entire variable 'state' for this
 * purpose.
 */
void srandom();

#endif
