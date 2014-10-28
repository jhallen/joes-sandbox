/* Main header file
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

#include "hash.h"

/* Meta data table item */

struct meta
  {
  int code;
  char *str;
  struct meta *link;
  };

/* Compiled schema */

struct schema
  {
  HASH *meta_hash;
  struct meta metadata[];
  };

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

/* Convert schema into C types */

/* A structure */
#define STRUCT(NAME,CONTENTS) \
  struct NAME { struct NAME *next; struct base *mom; char *_name; struct meta *_meta; CONTENTS };

/* A sub-structure */
#define SUBSTRUCT(NAME,TYPE) struct TYPE *NAME;

/* A list of structures (structure member) */
#define LIST(NAME,TYPE) struct TYPE *NAME;

/* A string (structure member) */
#define STRING(NAME) char *NAME;

/* An integer (structure member) */
#define INTEGER(NAME) ptrdiff_t NAME;


void xml_print(FILE *f,char *name,int i,struct base *b);

void lisp_print(FILE *f,char *name,int i,struct base *b);

void lisp_print_untagged(FILE *f,char *name,int i,struct base *b);

void json_print(FILE *f,char *name,int i,struct base *b,int comma);

void indent_print(FILE *f,char *name,int i,struct base *b);

void indent_print_untagged(FILE *f,char *name,int i,struct base *b);

struct base *xml_parse(FILE *f,char *name,struct schema *schema, struct meta *expect,int require);

void *mk(struct schema *schema, char *type_name);	/* Create object of given type */

void *mkraw(struct meta *m);

struct meta *metafind(struct schema *schema, char *name);
