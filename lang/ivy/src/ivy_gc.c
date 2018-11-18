/* Garbage collector

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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include "ivy.h"

int alloc_count;

Val *mark_val(Val *val)
{
	switch (val->type) {
		case tSTR: {
			mark_str(val->u.str);
			break;
		} case tOBJ: {
			mark_obj(val->u.obj);
			break;
		} case tFUN: {
			mark_fun(val->u.fun);
			break;
		} case tRET_IVY: {
			struct callfunc *c = val->u.callfunc;
			mark_val(&c->val);
			mark_fun(c->o);
			mark_obj(c->ovars);
			mark_var(c->argv);
			return val - 1;
		} case tRET_NEXT_INIT: {
			if (val->u.fun)
				mark_fun(val->u.fun);
			return val - 1;
		} case tRET_SIMPLE: {
			if (val->u.obj)
				mark_obj(val->u.obj);
			if (val[-1].var) {
				struct callfunc *c = (struct callfunc *)val[-1].var;
				mark_val(&c->val);
				mark_fun(c->o);
				mark_obj(c->ovars);
				mark_var(c->argv);
			}
			if (val[-2].u.fun)
				mark_fun(val[-2].u.fun);
			return val - 3;
		} default: {
			break;
		}
	}
	if (val->var)
		mark_var(val->var);
	return val - 1;
}

extern Ivy *ivys;

void mark_protected()
{
	mark_protected_strs();
	mark_protected_objs();
	mark_protected_funs();
	mark_protected_vars();
}

/* Collect garbage */

extern int mark_fun_count;
extern int mark_str_count;
extern int mark_obj_count;
extern int mark_var_count;

extern int free_var_count;
extern int alloc_var_count;

void collect()
{
	Val *p;
	Ivy *ivy;

	alloc_count = 0;

	// printf("Collecting garbage: Marking... "); fflush(stdout);

	mark_fun_count = 0;
	mark_str_count = 0;
	mark_var_count = 0;
	mark_obj_count = 0;

	mark_protected();

	for (ivy = ivys; ivy; ivy = ivy->next) {
		/* Mark */
		p = ivy->sp;
		while (p != ivy->sptop) {
			p = mark_val(p);
		}
		if (ivy->glblvars) {
			// printf("mark globals %d\n", ivy->glblvars->objno);
			mark_obj(ivy->glblvars);
		}
		if (ivy->vars) {
			// printf("mark scope %d\n", ivy->vars->objno);
			mark_obj(ivy->vars);
		}
		mark_val(&ivy->stashed);
	}

	// printf("mark_fun_count = %d\n", mark_fun_count);
	// printf("mark_str_count = %d\n", mark_str_count);
	// printf("mark_var_count = %d\n", mark_var_count);
	//printf("mark_obj_count = %d\n", mark_obj_count);

	// printf("Sweeping... "); fflush(stdout);

	// printf("alloc_var_count=%d\n", alloc_var_count);

	free_var_count = 0;
	alloc_var_count = 0;

	/* Sweep variables */
	sweep_vars();

	// printf("Found free vars = %d\n", free_var_count);
	// printf("Found allocated vars = %d\n", alloc_var_count);

	/* Sweep strings */
	sweep_strs();

	/* Sweep objects */
	sweep_objs();

	/* Sweep funs */
	sweep_funs();
	// printf("Done.\n");
}

void clear_protected()
{
	clear_protected_objs();
	clear_protected_strs();
	clear_protected_vars();
	clear_protected_funs();
}
