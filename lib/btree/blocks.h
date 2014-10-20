/* Fast block move/copy subroutines
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

/* char *mmove(char *d,char *s,int sz); Copy 'sz' bytes from 's' to 'd'.
 * Chooses either mbkwd or mfwrd to do this such that the data won't get
 * clobbered.
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
 * Count the number of occurances a character appears in a block
 */
int mcnt();

#ifdef junk
/* char *mchr(char *s,char c);
 *
 * Return address of first 'c' following 's'.
 */
char *mchr();
#endif
#endif
