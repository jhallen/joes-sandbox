/* Placement manager 
   Copyright (C) 1999 Joseph H. Allen

This file is part of Notif

Notif is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation; either version 1, or (at your option) any later version.  

Notif is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.  

You should have received a copy of the GNU General Public License along with 
Notif; see the file COPYING.  If not, write to the Free Software Foundation, 
675 Mass Ave, Cambridge, MA 02139, USA.  */ 

#define PMGRFUNCS \
 void (*rm)();		/* Delete the placement manager */		\
 void (*calcx)();	/* Calculate X positions */			\
 void (*calcy)();	/* Calculate Y positions */			\
 void (*calcw)();	/* Calculate width */				\
 void (*calch)();	/* Calculate height */				\
 void (*add)();		/* New widget added to widget */		\
 void (*rmv)();		/* Widget has been removed from a widget */	\
 void (*sttarget)();	/* Set target */				\

#define PMGRVARS \
 Lith *in;			/* Widget we're controlling */			\
 int lrel, rrel, wid, hmode, hofst;	/* Horz. positioning mode */	\
 int alrel, arrel, awid, ahmode, ahofst;/* Aux horz. positioning mode */\
 int trel, brel, hgt, vmode, vofst;	/* Vert. positioning mode */	\
 int atrel, abrel, ahgt, avmode, avofst;/* Aux Vert. positioning mode */

extern struct pmgrfuncs { PMGRFUNCS } pmgrfuncs;
struct pmgrvars { struct pmgrfuncs *funcs; PMGRVARS };

Pmgr *mkPmgr();		/* Create a placement manager */
