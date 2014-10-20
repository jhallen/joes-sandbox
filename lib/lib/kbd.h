/* Key-map handler
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

#ifndef _Ikbd
#define _Ikbd 1

#include "config.h"

typedef struct kmap KMAP;
typedef struct kbd KBD;
typedef struct key KEY;

/* Key code modifiers */

#define Shift  0x00010000
#define Ctrl   0x00040000
#define Alt    0x00080000

/* A key binding */

struct key
 {
 int k;			/* Key code & flags */
 union
  {
  void *bind;		/* What key is bound to */
  KMAP *submap;		/* Sub KMAP address (for prefix keys) */
  } value;
 };

/* A map of keycode to command/sub-map bindings */

struct kmap
 {
 KEY *keys;		/* KEYs */
 int len;		/* No. keys */
 int siz;		/* Malloc size of keys */
 void *obj;		/* First arg for rm and dup */
 void (*rm)();		/* Eliminate a binding value */
 void (*dup)();		/* Duplicate a binding value */
 };

/** A keyboard event handler **/

struct kbd
 {
 KMAP *curmap;		/* Current keymap */
 KMAP *topmap;		/* Top-level keymap */
 int seq[16];		/* Current sequence of keys */
 int x;			/* What we're up to */
 };

/* KMAP *mkkmap(void);
 * Create an empty keymap
 */
EXTERN KMAP *mkkmap();

/* void rmkmap(KMAP *kmap);
 * Free a key map
 */
EXTERN void rmkmap();

/* int kadd(KMAP *kmap,CAP *cap,char *seq,void *bind);
 * Add a key sequence binding to a key map
 *
 * Returns 0 for success
 *        -1 for for invalid key sequence
 *
 * A valid key sequence is one or more keys seperated with spaces.  A key
 * is a single character or one of the following strings:
 *
 *    ^?	                   127 (DEL)
 *
 *    ^@   -   ^_                  Control characters
 *
 *    SP                           32 (space character)
 *
 *    ! - ~			   Key for printable ASCII character
 *
 *    .xx			   Termcap string sequence
 *
 * In addition, the last key of a key sequence may be replaced with
 * a range-fill of the form: <KEY> TO <KEY>
 *
 * So for example, if the sequence: ^K A TO Z
 * is speicified, then the key sequences
 * ^K A, ^K B, ^K C, ... ^K Z are all bound.
 */
EXTERN int kadd();

/* void kcpy(KMAP *dest,KMAP *src);
 * Copy all of the entries in the 'src' keymap into the 'dest' keymap
 */
EXTERN void kcpy();

/* int kdel(KMAP *kmap,CAP *cap,char *seq);
 * Delete a binding from a keymap
 *
 * Returns 0 for success
 *        -1 if the given key sequence was invalid
 *         1 if the given key sequence did not exist
 */
EXTERN int kdel();

/* KBD *mkkbd(KMAP *kmap);
   Create a keyboard handler which uses the given keymap
*/
EXTERN KBD *mkkbd();

/* void rmkbd(KBD *);
 *
 * Eliminate a keyboard handler
 */
EXTERN void rmkbd();

/* void *dokey(KBD *kbd,int k);
   Handle a key for a KBD:

     Returns 0 for invalid or prefix keys

     Returns binding for a completed key sequence
*/
EXTERN void *dokey();

/* int seqlen(KBD *kbd);
 * Return the no. of keys processed so far.
 */
#define seqlen(kbd) ((kbd)->x)

/* int nthkey(KBD *kbd,int n);
 * Return the value of the nth key.
 */
#define nthkey(kbd,n) ((kbd)->seq[n])

/* void clrkbd(KBD *kbd);
 * Clear any partially recognized keysequence from a keyboard handler
 */
#define clrkbd(kbd) ((kbd)->curmap=(kbd)->topmap)

#endif
