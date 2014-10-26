/* Meta data table and utilities
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "meta.h"
#include "hash.h"

/* Macros which convert schema.h into meta-data table */

#define STRUCT(name,contents) { tSTRUCT, #name }, contents { 0, 0 },

#define LIST(name,type) { tLIST, #name }, { -1, #type },
#define SUBSTRUCT(name,type) { tSTRUCT, #name } { -1, #type },
#define STRING(name) { tSTRING, #name },
#define INTEGER(name) { tINTEGER, #name },

// #define PTR(name) { 5, #name },

struct meta metadata[]=
  {
#include "schema.h"
    { 0, 0 }
  };

#undef STRUCT
#undef SUBSTRUCT
#undef LIST
#undef STRING
#undef INTEGER

/* Search for metadata structure definition table entry */

static HASH *meta_hash;

struct meta *metafind(char *name)
  {
  if(!meta_hash)
    {
    int x;
    meta_hash=htmk(64);
    for(x=0;metadata[x].code;)
      {
      htadd(meta_hash,metadata[x].str,metadata+x);
      while(metadata[x++].code);
      }
    }
  return htfind(meta_hash,name);
  }

/* Return allocation size of structure */

int structsize(struct meta *m)
  {
  int x;
  /* Count no. members of structure */
  for(x=0,++m;m->code;++x)
    switch(m->code)
      {
      case tSTRUCT: m+=2; break;
      case tLIST: m+=2; break;
      case tSTRING: m+=1; break;
      case tINTEGER: m+=1; break;
      }

  /* Add in size for name and next pointers.  Multiply by member size. */
  return x*sizeof(void *)+sizeof(struct base);
  }

/* Create record */

void *mkraw(struct meta *m)
  {
  struct base *b=malloc(structsize(m));
  void **t=(void **)((char *)b+sizeof(struct base));
  b->_name=0;
  b->mom=0;
  b->_meta=m;
  b->next=0;
  for(++m;m->code;)
    {
    *t++=0;
    switch(m->code)
      {
      case tSTRUCT: m+=2; break;
      case tLIST: m+=2; break;
      case tSTRING: m+=1; break;
      case tINTEGER: m+=1; break;
      }
    }
  return b;
  }

/* Create record by name */

void *mk(char *name)
  {
  return mkraw(metafind(name));
  }
