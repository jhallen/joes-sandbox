/* Convert schema into user's structures
   Copyright (C) 2005 Joseph H. Allen

This file is part of SDU (Structured Data Utilities)

SDU is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation; either version 1, or (at your option) any later version.  

SDU is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.  

You should have received a copy of the GNU General Public License along with 
SDU; see the file COPYING.  If not, write to the Free Software Foundation, 
675 Mass Ave, Cambridge, MA 02139, USA.  */ 

/* Meta data table item */

struct meta
  {
  int code;
  char *str;
  };

extern struct meta metadata[];

/* Type codes */

#define tSTRUCT 1
#define tLIST 2
#define tSTRING 3
#define tINTEGER 4

/* Base class */

struct base
  {
  struct base *next;	/* Next in list */
  struct base *mom;	/* Parent */
  char *_name;		/* Name of */
  struct meta *_meta;	/* Definition of */
  /* Schema defined members end up here */
  };

/* Macros which convert schema into set of C structure definitions */

/* A structure */
#define STRUCT(NAME,CONTENTS) \
  struct NAME { struct NAME *next; struct base *mom; char *_name; struct meta *_meta; CONTENTS };

/* A list of structures (structure member) */
#define LIST(NAME,TYPE) struct TYPE *NAME;

/* A string (structure member) */
#define STRING(NAME) char *NAME;

/* An integer (structure member) */
#define INTEGER(NAME) ptrdiff_t NAME;

/* A sub-structure */
#define SUBSTRUCT(NAME,TYPE) struct TYPE *NAME;

/* For these, the 'char *' is initialized to "name" (structure members) */
#define PTR(NAME) char *NAME;
#define FILE(NAME) char *NAME;
#define DIR(NAME) char *NAME;

#include "schema.h"

#undef STRUCT
#undef SUBSTRUCT
#undef LIST
#undef STRING
#undef INTEGER
#undef PTR
#undef FILE
#undef DIR

void xml_print(FILE *f,int i,struct base *b);
struct base *xml_parse(FILE *f,struct meta *expect);
void *mk(char *type_name);			/* Create object of given type */
void *mkraw(struct meta *m);
struct meta *metafind(char *name);
