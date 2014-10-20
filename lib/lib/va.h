/* Variable length arrays of strings
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

#ifndef _Iva
#define _Iva

#include "config.h"
#include "vs.h"

/* Functions and global variable you have to define.  Replace these with
 * macros or defines here if they are not to be actual functions 
 */

#define aELEMENT(a) char *a
#define aCAST char *
/* aELEMENT(adup()); */
#define adup(s) vsdup(s)
/* aELEMENT(adel()); */
#define adel(s) vsrm(s)
/* int acmp(); */
#define acmp(a,b) vscmp((a),(b))
/* extern aELEMENT(ablank); */
#define ablank NULL
/* extern aELEMENT(aterm); */
#define aterm NULL

/************************/
/* Creation/Destruction */
/************************/

/* aELEMENT(*vamk(int len));
 * Create a variable length array.  Space for 'len' elements is preallocated.
 */
aELEMENT(*vamk());

/* void varm(aELEMENT(*vary));
 * Free an array and everything which is in it.  Does nothing if 'vary' is
 * 0.
 */
void varm();

/********************/
/* Space management */
/********************/

/* int aSIZ(aELEMENT(*vary));
 * int aSiz(aELEMENT(*vary));
 * Access size part of array.  This int indicates the number of elements which
 * can fit in the array before realloc needs to be called.  It does not include
 * the extra space needed for the terminator and the header.
 *
 * aSIZ returns 0 if you pass it 0.  aSiz does not do this checking,
 * but can be used as an lvalue.
 */
#define aSIZ(a) ((a)?*((int *)(a)-2):0)
#define aSiz(a) (*((int *)(a)-2))

/* int aLEN(aELEMENT(*vary));
 * int aLen(aELEMENT(*vary));
 * Access length part of array.  This int indicates the number of elements
 * currently in the array (not including the terminator).  This should be
 * used primarily for reading the size of the array.  It can be used for
 * setting the size of the array, but it must be used with care since it
 * does not eliminate elements (if the size decreases) or make sure there's
 * enough room (if the size increases).  See vensure and vtrunc.
 *
 * aLEN return a length of zero if 'vary' is 0.
 * aLen doesn't do this checking, but can be used as an lvalue
 */
#define aLEN(a) ((a)?*((int *)(a)-1):0)
#define aLen(a) (*((int *)(a)-1))

/* int alen(aELEMENT(*ary));
 * Compute length of char or variable length array by searching for termination
 * element.  Returns 0 if 'vary' is 0.
 */
int alen();

/* aELEMENT(*vaensure(aELEMENT(*vary),int len));
 * Make sure there's enough space in the array for 'len' elements.  Whenever
 * vaensure reallocs the array, it allocates 25% more than the necessary
 * minimum space in anticipation of future expansion.  If 'vary' is 0,
 * it creates a new array.
 */
aELEMENT(*vaensure());

/* aELEMENT(*vazap(aELEMENT(*vary),int pos,int n));
 * Destroy n elements from an array beginning at pos.  Is ok if pos/n go
 * past end of array.  This does not change the aLEN() value of the array.
 * This does nothing and returns 0 if 'vary' is 0.  Note that this
 * function does not actually write to the array.  This does not stop if
 * a aterm is encountered.
 */
aELEMENT(*vazap());

/* aELEMENT(*vatrunc(aELEMENT(*vary),int len));
 * Truncate array to indicated size.  This zaps or expands with blank elements
 * and sets the LEN() of the array.  A new array is created if 'vary' is 0.
 */
aELEMENT(*vatrunc());

/************************************/
/* Function which write to an array */
/************************************/

/* aELEMENT(*vafill(aELEMENT(*vary),int pos,aELEMENT(el),int len));
 * Set 'len' element of 'vary' beginning at 'pos' to duplications of 'el'.
 * Ok, if pos/len are past end of array.  If 'vary' is 0, a new array is
 * created.
 *
 * This does not zap previous values.  If you need that to happen, call
 * vazap first.  It does move the terminator around properly though.
 */
aELEMENT(*vafill());
#ifdef junk
/* aELEMENT(*vancpy(aELEMENT(*vary),int pos,aELEMENT(*array),int len));
 * Copy 'len' elements from 'array' onto 'vary' beginning at position 'pos'.
 * 'array' can be a normal char array since the length is passed seperately.  The
 * elements are copied, not duplicated.  A new array is created if 'vary' is
 * 0.  This does not zap previous elements.
 */
aELEMENT(*vancpy());
#endif
/* aELEMENT(*vandup(aELEMENT(*vary),int pos,aELEMENT(*array),int len));
 * Duplicate 'len' elements from 'array' onto 'vary' beginning at position
 * 'pos'.  'array' can be a char array since its length is passed seperately.  A
 * new array is created if 'vary' is 0.
 */
aELEMENT(*vandup());

/* aELEMENT(*vadup(aELEMENT(*vary)));
 * Duplicate array.  This is just a functionalized version of:
 *
 *   vandup(NULL,0,vary,aLEN(vary));
 *
 * but since you need to be able to refer to this particular function by
 * address often it's given here.
 *
 * (actually, there's bazillions of these simple combinations of the above
 * functions and the macros of the next section.  You'll probably want to make
 * functionalized instances of the ones you use most often - especially since
 * the macros aren't safe).
 */ 
aELEMENT(*vadup());

/* aELEMENT(*vaset(aELEMENT(*vary),int pos,aELEMENT(element)));
 * Set an element in an array.  Any value of 'pos' is valid.  A new array
 * is created if 'vary' is 0.  The previous contents of the position is
 * deleted.    This does not duplicate 'element'.  If you need 'element'
 * duplicated, call: vaset(vary,pos,adup(element));
 */
aELEMENT(*_vaset());

#define vaset(v,p,el)  \
 (!(v) || (p)>aLen(v) || (p)==aLen(v) && (p)==aSiz(v) ?  \
  _vaset((v),(p),(el)) \
 : \
  ((p)==aLen(v) ? \
   ((v)[(p)+1]=(v)[p], (v)[p]=(el), aLen(v)=(p)+1, (v)) \
  : \
   (adel((v)[p]), (v)[p]=(el), (v)) \
  ) \
 )   

/* aELEMENT(*vaadd(aELEMENT(*vary),aELEMENT(element)));
 * Concatenate a single element to the end of 'vary'.  A new array is created
 * if 'vary' is 0.  This does not duplicate element: call
 * vaadd(vary,adup(element));  If you need it duplicated.
 */
#define vaadd(v,el) \
 (!(v) || aLen(v)==aSiz(v) ? \
  _vaset((v),aLEN(v),(el)) \
 : \
  ((v)[aLen(v)+1]=(v)[aLen(v)], (v)[aLen(v)]=(el), \
   aLen(v)=aLen(v)+1, (v)) \
 )

/**************************************/
/* Functions which read from an array */
/**************************************/

/* These macros are used to generate the address/size pairs which get
 * passed to the functions of the previous section.
 */

/* { aELEMENT(*),int } av(aELEMENT(*array));
 * Return array,size pair.  Uses aLEN to get size.
 */
#define av(a) (a),aLEN(a)

/* { aELEMENT(*),int } az(aELEMENT(*array));
 * Return array,size pair.  Uses alen to get size.
 */
#define az(a) (a),alen(a)

/* { aELEMENT(*),int } ac(aELEMENT(*array));
 * Return array,size pair.  Uses 'sizeof' to get size.
 */
#define ac(a) (a),(sizeof(a)/sizeof(aCAST))

/* { aELEMENT(*),int } arest(aELEMENT(*vary),int pos);
 * Return array,size pair of rest of array beginning at pos.  If
 * pos is past end of array, gives size of 0.
 */
#define arest(a,p) ((a)+(p)),(((p)>aLEN(a))?0:aLen(a)-(p))

/* { aELEMENT(*),int } apart(aELEMENT(*vary),int pos,int len);
 * Return array,size pair of 'len' elements of array beginning with pos.  If
 * pos is past end of array, gives size of 0.  If pos+len is past end of array,
 * returns number of elements to end of array.
 */
#define apart(a,p,l) \
 ((a)+(p)),((p)>=aLEN(a)?0:((p)+(l)>aLen(a)?aLen(a)-(p):(l)))

/* aELEMENT(vaget(aELEMENT(*vary),int pos));
 * Get an element from an array.  Any value of pos is valid; if it's past the
 * end of the array or if 'vary' is 0, the terminator is returned.  This
 * does not make a duplicate of the returned element.  If you want that, pass
 * the return value of this to adup.
 */
#define vaget(a,p) ((p)>=aLEN(a)?aterm:(a)[p])

/*************************/
/* Searching and Sorting */
/*************************/

/* aELEMENT(*vasort(aELEMENT(*ary),int len))
 * Sort the elements of an array (char or variable length) using qsort().
 */
aELEMENT(*vasort());

/* aELEMENT(*vawords(aELEMENT(*a),char *s,int len,char *sep,int seplen));
 * Generate list of strings out of words in 's' seperated with the characters
 * in 'sep'.  The characters in 'sep' must be sorted.
 */
aELEMENT(*vawords());

#endif
