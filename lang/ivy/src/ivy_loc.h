/* A location in a source file

   Copyright (C) 1993 Joseph H. Allen

This file is part of IVY

IVY is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation; either version 1, or (at your option) any later version.  

IVY is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.  


IVY; see the file COPYING.  If not, write to the Free Software Foundation, 
675 Mass Ave, Cambridge, MA 02139, USA.  */

#ifndef _Iivy_loc
#define _Iivy_loc 1

#include "ivy_free_list.h"

typedef struct ivy_loc Ivy_loc;		/* Input location */

/* A location */

struct ivy_loc {
	const char *ptr;
	int col;
	int lvl;
	int line;
	const char *name;
	int eof;
	Ivy_free_list *free_list;
};

#endif
