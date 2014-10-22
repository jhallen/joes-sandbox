/* Zero terminated strings
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

#ifndef _Izstr
#define _Izstr 1

#include "config.h"

/* int zlen(char *s); Return length of z-string */
extern int zlen();

/* char *zchr(char *s,char c); Return address of first 'c' in 's', or NULL if
 * the end of 's' was found first */
extern char *zchr();

/* char *zrchr(char *s,char c); Return address of last 'c' in 's', or NULL if
 * there are none.
 */
extern char *zrchr();

/* char *zcpy(char *d,char *s); Copy z-string at s to d */
extern char *zcpy();

/* char *zcat(char *d,char *s); Append s onto d */
extern char *zcat();

/* char *zdup(char *s); Duplicate z-string into an malloc block */
extern char *zdup();

/* int zcmp(char *l,char *r); Compare the strings.  Return -1 if l is
 * less than r; return 0 if l equals r; and return 1 if l is greater than r.
 */
extern int zcmp();

/* int bcmp(char *l,char *r); Compare the string at r with the beginning
 * n characters of the string at l, where n is the number of characters
 * in r.  Returns -1, 0, or 1 as in zcmp. */
extern int bcmp();

/* int bicmp(char *l,char *r); Same as bcmp(), but case insensitive.
 */
extern int bicmp();

/* int zicmp(char *l,char *r); Compare the strings, case insensitive.  Return
 * -1 if l is less than r; return 0 if l equals r; and return 1 if l is greater
 * than r.
 */
extern int zicmp();

/* int fields(char *s,char **fields,char sep); Break up z-string containing
 * fields into its componant fields.  This is done by setting the field
 * seperator characters to zero- thereby changing the fields into z-strings,
 * and by storing the starting address of each field in the array 'fields'
 * (which must be large enough to store the field pointers).
 *
 * The number of fields which were found in s is returned.
 */
extern int fields();

/* int nfields(char *s,char sep); Assuming 'sep' is the field seperator
 * character, count the number of fields in z-string s.  If no 'sep'
 * characters are in 's', the number of fields is 1.
 */
extern int nfields();

#endif
