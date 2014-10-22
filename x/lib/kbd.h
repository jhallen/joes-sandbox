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
 int k;			/* Key code shifted left by one.  LSB set = submap */
 int (*func)();		/* Function bound to key */
 void *arg;		/* Arg for function */
 };

/* A map of keycode to command/sub-map bindings */

struct kmap
 {
 KEY *keys;		/* KEYs */
 int len;		/* No. keys */
 int siz;		/* Malloc size of keys */
 void *obj;		/* First arg for rm and dup */
 void (*rm)();		/* rm(obj,func,arg) Eliminate bindings within a key */
 void (*set)();		/* set(obj,KEY *,func,arg,code) Set a keycode */
 };

/** A keyboard event handler **/

struct kbd
 {
 KMAP *curmap;		/* Current keymap */
 KMAP *topmap;		/* Top-level keymap */
 int seq[16];		/* Current sequence of keys */
 int x;			/* No. keys we have so far in this sequence */
 };

/* KMAP *mkkmap(void (*set)(),void (*rm)(),void *obj);
 * Create an empty keymap
 */
KMAP *mkkmap();

/* void rmkmap(KMAP *kmap);
 * Free a key map
 */
void rmkmap();

/* int kadd(KMAP *kmap,CAP *cap,char *seq,int (*func)(),void *arg);
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
 *    sp, Sp or SP                 32 (space character)
 *
 *    ! - ~			   Key for printable ASCII character
 *
 *    .xx			   Termcap string sequence
 *
 *    xxxxx			   X key sequence name
 *
 * In addition, the last key of a key sequence may be replaced with
 * a range-fill of the form: <KEY> TO <KEY>
 *
 * So for example, if the sequence: ^K A TO Z
 * is speicified, then the key sequences
 * ^K A, ^K B, ^K C, ... ^K Z are all bound.
 */
int kadd();

/* void kcpy(KMAP *dest,KMAP *src);
 * Copy all of the entries in the 'src' keymap into the 'dest' keymap
 */
void kcpy();

/* int kdel(KMAP *kmap,CAP *cap,char *seq);
 * Delete a binding from a keymap
 *
 * Returns 0 for success
 *        -1 if the given key sequence was invalid
 */
int kdel();

/* KBD *mkkbd(KMAP *kmap);
   Create a keyboard handler which uses the given keymap
*/
KBD *mkkbd();

/* void rmkbd(KBD *);
 *
 * Eliminate a keyboard handler
 */
void rmkbd();

/* KEY *dokey(KBD *kbd,int k);
   Handle a key for a KBD:

     Returns NULL for unknown key
     Returns &keytaken for a prefix key
     Returns binding for a completed key sequence
*/
KEY keytaken;
KEY *dokey();

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
