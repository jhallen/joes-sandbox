/* Keyboard macros
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

#ifndef _Imacro
#define _Imacro 1

#include "config.h"
#include "cmd.h"

struct macro
 {
 /* Simple command */
 int k;		/* Keycode */
 CMD *cmd;	/* Command address */

 /* A bunch of commands */
 int n;		/* Number of steps */
 int size;	/* Malloc size of steps */
 MACRO **steps;	/* Block */
 };

/* Macro construction functions */
extern MACRO *mkmacro();
extern void addmacro();
extern MACRO *dupmacro();
extern void rmmacro();
extern MACRO *macstk();

extern void chmac();

/* Text to macro / Macro to text */
extern MACRO *mparse();
extern char *mtext();

/* Execute a macro */
extern MACRO *curmacro;
extern int exemac();
extern int exmacro();

#endif
