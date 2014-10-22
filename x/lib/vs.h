/* Dynamic string library
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

#ifndef _Ivs
#define _Ivs

#include "config.h"

/***
 *
 * This is a dynamic string library which supports strings which automatically
 * resize themselves when needed.  The strings know their own size, so getting
 * the length of a string is a fast operation and storing zeroes in the
 * strings is permissable.
 *
 * The strings are stored in malloc blocks and have the following format:
 *
 *   <bksize><length><string><zero>
 *
 * 'bksize' and 'length' are ints which give the size of the malloc block
 * and the length of the string.  A zero character always follows the string
 * for compatibility with normal C zero-terminated strings.  The zero is not
 * counted in the string length.
 *
 * To further the compatibility with C strings, the address of a dynamic string
 * is at <string> above, not at <bksize> (whose address is the start of the
 * malloc block).  Thus, dynamic strings can be passed as arguments to UNIX
 * operating system functions and C library function, but they can not be freed
 * with free()- a special function is provided in this library for freeing
 * dynamic strings.
 *
 * The primary dynamic string function is:
 *
 * char *vsncpy(char *d,int off,char *s,int len);
 *                              Copy a block of characters at address 's' of
 *				length 'len' onto the dynamic string 'd' at
 *				offset 'off'.  The dynamic string is expanded
 *				to handle any values of 'len' and 'off' which
 *				might be given.  If 'off' is greater than the
 *				length of the string, SPACEs are placed in the
 *				gap.  If 'd' is NULL, a string is created.  If
 *				'len' is 0, no copying or string expansion
 *				occurs.
 *
 * Three important macros are provided for helping with vsncpy():
 *
 * sc("Hello")   Gives -->  "Hello",sizeof("Hello")-1
 * sz(s)         Gives -->  s,zlen(s)
 * sv(d)         Gives -->  d,sLEN(d)
 *
 * These are used to build arguments for vsncpy().  Many functions
 * can be created with combinations of sc/sz/sv with vsncpy:
 *
 *    s=vsncpy(NULL,0,NULL,0);		Create an empty dynamic string
 *
 *    s=vsncpy(NULL,0,sc("Hello"));	Create a dynamic string initialized
 *					with "Hello"
 *
 *    d=vsncpy(NULL,0,sv(s));		Duplicate a dynamic string
 *
 *    d=vsncpy(NULL,0,sz(s));		Convert a C string into a dynamic
 *                                      string.
 *
 *    d=vsncpy(sv(d),sv(s));		Append dynamic string s onto d.
 *
 *    d=vsncpy(sv(d),sc(".c"));		Append a ".c" extension to d.
 *
 *
 * These lesser functions are also provided:
 *
 * void vsrm(char *s);		Free a string.  Do nothing if 's' is NULL.
 *
 * int sLEN(char *s);		Return the length of the string 's'.  If 's'
 *				is NULL, return 0.
 *
 * char *vstrunc(char *d,int len);
 *				Set the length of a string.  Expand the string
 *				with blanks if necessary.
 *
 * char *vsensure(char *d,int len);
 *				Expand the malloc block for the string if
 *				necessary so that a string of 'len' chars can
 *				fit in it.
 *
 * sLen(s)=10;			Set the length indicator of the string to 10.
 *
 * char *vsins(char *d,int off,int len);
 *				Insert a gap into a string.
 *
 * char *vsdel(char *d,int off,int len);
 *				Delete characters from a string.
 *
 * Other function are provided as well.  Look through the rest of the header
 * file.  The header file is kind of weird looking because it is intended to
 * handle dynamic arrays of any type with only a few changes.
 */

/* Functions and global variable you have to define.  Replace these with
 * macros or defines here if they are not to be actual functions 
 */

/* An element with name 'a' */
#define sELEMENT(a) char a

/* Cast something to element type */
#define sCAST char

/* Duplicate an element */
/* sELEMENT(sdup()); */
#define sdup(a) (a)

/* Delete an element */
/* sELEMENT(sdel()); */
#define sdel(a) 0

/* Compare a single element */
/* int scmp(); */
#define scmp(a,b) ((a)>(b)?1:((a)==(b)?0:-1))

/* Compare a single element- case insensitive */
int sicmp();

/* A blank element */
/* extern sELEMENT(sblank); */
#define sblank ' '

/* A termination element */
/* extern sELEMENT(sterm); */
#define sterm '\0'

/************************/
/* Creation/Destruction */
/************************/

/* sELEMENT(*vsmk(int len));
 * Create a variable length array.  Space for 'len' elements is preallocated.
 */
sELEMENT(*vsmk());

/* void vsrm(sELEMENT(*vary));
 * Free an array and everything which is in it.  Does nothing if 'vary' is
 * 0.
 */
void vsrm();

/********************/
/* Space management */
/********************/

/* int sSIZ(sELEMENT(*vary));
 * int sSiz(sELEMENT(*vary));
 * Access size part of array.  This int indicates the number of elements which
 * can fit in the array before realloc needs to be called.  It does not include
 * the extra space needed for the terminator and the header.
 *
 * sSIZ returns 0 if you pass it 0.  sSiz does not do this checking,
 * but can be used as an lvalue.
 */
#define sSIZ(a) ((a)?*((int *)(a)-2):0)
#define sSiz(a) (*((int *)(a)-2))

/* int sLEN(sELEMENT(*vary));
 * int sLen(sELEMENT(*vary));
 * Access length part of array.  This int indicates the number of elements
 * currently in the array (not including the terminator).  This should be
 * used primarily for reading the size of the array.  It can be used for
 * setting the size of the array, but it must be used with care since it
 * does not eliminate elements (if the size decreases) or make sure there's
 * enough room (if the size increases).  See vensure and vtrunc.
 *
 * sLEN return a length of zero if 'vary' is 0.
 * sLen doesn't do this checking, but can be used as an lvalue
 */
#define sLEN(a) ((a)?*((int *)(a)-1):0)
#define sLen(a) (*((int *)(a)-1))

/* int slen(sELEMENT(*ary));
 * Compute length of char or variable length array by searching for termination
 * element.  Returns 0 if 'vary' is 0.
 */
int slen();

/* sELEMENT(*vsensure(sELEMENT(*vary),int len));
 * Make sure there's enough space in the array for 'len' elements.  Whenever
 * vsensure reallocs the array, it allocates 25% more than the necessary
 * minimum space in anticipation of future expansion.  If 'vary' is 0,
 * it creates a new array.
 */
sELEMENT(*vsensure());

/* sELEMENT(*vstrunc(sELEMENT(*vary),int len));
 * Truncate array to indicated size.  This zaps or expands with blank elements
 * and sets the LEN() of the array.  A new array is created if 'vary' is 0.
 */
sELEMENT(*vstrunc());

/************************************/
/* Function which write to an array */
/************************************/

/* sELEMENT(*vsfill(sELEMENT(*vary),int pos,sELEMENT(el),int len));
 * Set 'len' element of 'vary' beginning at 'pos' to duplications of 'el'.
 * Ok, if pos/len are past end of array.  If 'vary' is 0, a new array is
 * created.
 *
 * This does not zap previous values.  If you need that to happen, call
 * vszap first.  It does move the terminator around properly though.
 */
sELEMENT(*vsfill());

/* sELEMENT(*vsncpy(sELEMENT(*vary),int pos,sELEMENT(*array),int len));
 * Copy 'len' elements from 'array' onto 'vary' beginning at position 'pos'.
 * 'array' can be a normal char array since the length is passed seperately.  The
 * elements are copied, not duplicated.  A new array is created if 'vary' is
 * 0.  This does not zap previous elements.
 */
sELEMENT(*vsncpy());

/* sELEMENT(*vsndup(sELEMENT(*vary),int pos,sELEMENT(*array),int len));
 * Duplicate 'len' elements from 'array' onto 'vary' beginning at position
 * 'pos'.  'array' can be a char array since its length is passed seperately.  A
 * new array is created if 'vary' is 0.
 */
sELEMENT(*vsndup());

/* sELEMENT(*vsdup(sELEMENT(*vary)));
 * Duplicate array.  This is just a functionalized version of:
 *
 *   vsndup(NULL,0,vary,sLEN(vary));
 *
 * but since you need to be able to refer to this particular function by
 * address often it's given here.
 *
 * (actually, there's bazillions of these simple combinations of the above
 * functions and the macros of the next section.  You'll probably want to make
 * functionalized instances of the ones you use most often - especially since
 * the macros aren't safe).
 */ 
sELEMENT(*vsdup());

/* sELEMENT(*vsset(sELEMENT(*vary),int pos,sELEMENT(element)));
 * Set an element in an array.  Any value of 'pos' is valid.  A new array
 * is created if 'vary' is 0.  The previous contents of the position is
 * deleted.    This does not duplicate 'element'.  If you need 'element'
 * duplicated, call: vsset(vary,pos,sdup(element));
 */
sELEMENT(*_vsset());

#define vsset(v,p,el)  \
 (!(v) || (p)>sLen(v) || (p)>=sSiz(v) ?  \
  _vsset((v),(p),(el)) \
 : \
  ((p)==sLen(v) ? \
   ((v)[(p)+1]=0, sLen(v)=(p)+1, (v)[p]=(el), (v)) \
  : \
   ((v)[p]=(el), (v)) \
  ) \
 )   

/* sELEMENT(*vsadd(sELEMENT(*vary),sELEMENT(element)));
 * Concatenate a single element to the end of 'vary'.  A new array is created
 * if 'vary' is 0.  This does not duplicate element: call
 * vsadd(vary,sdup(element));  If you need it duplicated.
 */
#define vsadd(v,el) \
 (!(v) || sLen(v)==sSiz(v) ? \
  _vsset((v),sLEN(v),(el)) \
 : \
  ((v)[sLen(v)+1]=0, (v)[sLen(v)]=(el), sLen(v)=sLen(v)+1, (v)) \
 )

/**************************************/
/* Functions which read from an array */
/**************************************/

/* These macros are used to generate the address/size pairs which get
 * passed to the functions of the previous section.
 */

/* { sELEMENT(*),int } sv(sELEMENT(*array));
 * Return array,size pair.  Uses sLEN to get size.
 */
#define sv(a) (a),sLEN(a)

/* { sELEMENT(*),int } sz(sELEMENT(*array));
 * Return array,size pair.  Uses slen to get size.
 */
#define sz(a) (a),slen(a)

/* { sELEMENT(*),int } sc(sELEMENT(*array));
 * Return array,size pair.  Uses 'sizeof' to get size.
 */
#define sc(a) (a),(sizeof(a)/sizeof(sCAST)-1)

/* { sELEMENT(*),int } srest(sELEMENT(*vary),int pos);
 * Return array,size pair of rest of array beginning at pos.  If
 * pos is past end of array, gives size of 0.
 */
#define srest(a,p) ((a)+(p)),(((p)>sLEN(a))?0:sLen(a)-(p))

/* { sELEMENT(*),int } spart(sELEMENT(*vary),int pos,int len);
 * Return array,size pair of 'len' elements of array beginning with pos.  If
 * pos is past end of array, gives size of 0.  If pos+len is past end of array,
 * returns number of elements to end of array.
 */
#define spart(a,p,l) \
 ((a)+(p)),((p)>=sLEN(a)?0:((p)+(l)>sLen(a)?sLen(a)-(p):(l)))

/* sELEMENT(vsget(sELEMENT(*vary),int pos));
 * Get an element from an array.  Any value of pos is valid; if it's past the
 * end of the array or if 'vary' is 0, the terminator is returned.  This
 * does not make a duplicate of the returned element.  If you want that, pass
 * the return value of this to sdup.
 */
#define vsget(a,p) ((p)>=sLEN(a)?sterm:(a)[p])

/**********************/
/* Insertion/Deletion */
/**********************/
#ifdef junk
/* sELEMENT(*vsins(sELEMENT(*vary),int pos,int n));
 * Insert n empty slots into the array.  If 'pos' >= the length of the array,
 * the array is simply extended.  The new slots are not set to anything.
 * This does not set the elements in the created hole to any particular
 * value: use vsfill if you need that to occur.
 */
sELEMENT(*vsins());

/* sELEMENT(*vsdel(sELEMENT(*vary),int pos,int n));
 * Delete n slots from the array.  This does not zap the elements first; call
 * vszap first if you need this to happen.
 */
sELEMENT(*vsdel());

/*************************/
/* Searching and Sorting */
/*************************/

/* sELEMENT(*vssort(sELEMENT(*ary),int len))
 * Sort the elements of an array (char or variable length) using qsort().
 */
sELEMENT(*vssort());
#endif
/* int vsbsearch(sELEMENT(*ary),int len,sELEMENT(element));
 * Do a binary search on a sorted variable length or char array.  Returns position
 * of matching element or the position where the element should be if it was
 * not found.  (You should test with scmp to find out which).
 *
 * Hmm... this should really indicate whether or not the element was found.
 */
int vsbsearch();
#ifdef junk
/* int vsfirst(sELEMENT(*ary),int len,sELEMENT(element));
 * Find offset to first matching element in 'vary' or return ~0 if not found.
 */
int vsfirst();

/* int vslast(sELEMENT(*ary),int len,sELEMENT(element));
 * Find offset to last matching element in 'vary' or return ~0 if none found.
 */
int vslast();

/* int vss(sELEMENT(*a),int alen,sELEMENT(*b),int blen);
 * Do a substring search on 'a'.  Return offset from 'a' to first matching
 * occurance of 'b' in 'a' or return ~0 if none found.
 */
int vss();
#endif
/* int vscmpn(sELEMENT(*a),int alen,sELEMENT(*b),int blen);
 *
 * Compare two arrays using scmp.  If 'a' > 'b', return 1.  If 'a' == 'b',
 * return 0.  If 'a' < 'b', return -1.  Longer strings are > shorter ones if
 * their beginning match.
 */
int vscmpn();

/* int vscmp(sELEMENT(*a),sELEMENT(*b));
 *
 * Functionalized version of: vscmpn(sv(a),sv(b));
 */
int vscmp();
#ifdef junk
/* int vsicmpn(sELEMENT(*a),int alen,sELEMENT(*b),int blen);
 *
 * Compare two arrays using sicmp.  If 'a' > 'b', return 1.  If 'a' == 'b',
 * return 0.  If 'a' < 'b', return -1.  Longer strings are > shorter ones if
 * their beginning match.
 *
 * This is same as vscmpn except that it is case insensitive.
 */
int vsicmpn();

/* int vsicmp(sELEMENT(*a),sELEMENT(*b));
 *
 * Functionalized version of: vsicmpn(sv(a),sv(b));
 */
int vsicmp();
#endif
/* int vsscan(sELEMENT(*a),int alen,sELEMENT(*b),int blen);
 * Find offset of first matching element in 'a' which matches any
 * of the elements passed in 'b'.  Array 'b' must be sorted.
 *
 * Hmm... this really needs to return what the found element is.
 */
int vsscan();

/* int vsspan(sELEMENT(*a),int alen,sELEMENT(*b),int blen);
 * Find offset of first matching element in 'a' which does not match any
 * of the elements passed in 'b'.  Array 'b' must be sorted.
 */
int vsspan();

/***************/
/* Other stuff */
/***************/
#ifdef junk
/* char *vsread(char *d,int p,int (*getC)(void *ptr),void *ptr);
 * Replace 'd' with next line read from read-character function 'getC'.  If 
 * 'd' is 0, a new string is allocated.  If there is no more input, the string
 * is freed and 0 is returned.  The \n is deleted from the entered line.
 *
 * 'ptr' is passed as the first arg to 'getC'.  'getC' should return -1 if
 * there is no more input.
 */
char *vsread();

/* char *vwords(char *s,char **a,int len,char t);
 *
 * Generate a 't'-seperated word list from the words in the zero-terminated
 * array of zero-terminated strings 'a'.  For example a simple 'echo.c':
 *
 * main(argc,argv)
 * char *argv[];
 * {
 * printf("%s\n",vwords(NULL,argv,argc,' ')):
 * }
 *
 */
char *vwords();
#endif
#endif
