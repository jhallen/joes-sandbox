#ifndef _Irc
#define _Irc 1

#include "kbd.h"
#include "macro.h"

/* KMAP *getcontext(char *name);
 * Find and return the KMAP for a given context name.  If none is found, an
 * empty kmap is created, bound to the context name, and returned.
 */
extern KMAP *getcontext();

/* int procrc(char *name);  Process an rc file
   Returns 0 for success
          -1 for file not found
           1 for syntax error (error messages written to stderr)
*/
extern int procrc();

#endif
