/* Convert schema into user's structures or types
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

#undef STRUCT
#undef SUBSTRUCT
#undef LIST
#undef STRING
#undef INTEGER

/* Macros which convert schema.h into meta-data table */

#define STRUCT(name,contents) { tSTRUCT, #name }, contents { 0, 0 },

#define SUBSTRUCT(name,type) { tSTRUCT, #name }, { -1, #type },

#define LIST(name,type) { tLIST, #name }, { -1, #type },

#define STRING(name) { tSTRING, #name },

#define INTEGER(name) { tINTEGER, #name },
