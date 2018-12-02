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

int ivy_alloc_count;
extern int ivy_mark_str_count;
extern int ivy_mark_obj_count;

Ivy_val *ivy_mark_val(Ivy_val *val)
{
	switch (val->type) {
		case ivy_tSTR: {
			ivy_mark_str(val->u.str);
			break;
		} case ivy_tOBJ: case ivy_tSCOPE: {
			ivy_mark_obj(val->u.obj);
			break;
		} case ivy_tCLOSURE: {
			ivy_mark_obj(val->u.closure.env);
			break;
		} case ivy_tRET_IVY: case ivy_tRET_IVY_THUNK: case ivy_tRET_NEXT_INIT: {
			struct ivy_callstate *c = val->idx.callstate;
			ivy_mark_val(&c->val);
			ivy_mark_obj(c->o.env);
			ivy_mark_obj(c->ovars);
			if (c->argv)
				ivy_mark_obj(c->argv);
			return val - 1;
		} case ivy_tRET_SIMPLE: case ivy_tRET_SIMPLE_THUNK: {
			struct ivy_callstate *c = val->idx.callstate;
			ivy_mark_val(&c->val);
			ivy_mark_obj(c->o.env);
			ivy_mark_obj(c->ovars);
			ivy_mark_obj(c->argv);
			if (val[-1].u.obj) {
				ivy_mark_obj(val[-1].u.obj);
			}
			return val - 2;
		} default: {
			break;
		}
	}
	if (val->origin)
		ivy_mark_obj(val->origin);
	return val - 1;
}

void ivy_mark_protected()
{
	ivy_mark_protected_strs();
	ivy_mark_protected_objs();
}

/* Collect garbage */

void ivy_collect()
{
	Ivy_val *p;
	Ivy *ivy;

	ivy_alloc_count = 0;

	// printf("Collecting garbage: Marking... "); fflush(stdout);

	ivy_mark_str_count = 0;
	ivy_mark_obj_count = 0;

	ivy_mark_protected();

	for (ivy = ivy_list->next; ivy != ivy_list; ivy = ivy->next) {
		/* Mark */
		p = ivy->sp;
		while (p != ivy->sptop) {
			p = ivy_mark_val(p);
		}
		if (ivy->glblvars) {
			// printf("mark globals %d\n", ivy->glblvars->objno);
			ivy_mark_obj(ivy->glblvars);
		}
		if (ivy->vars) {
			// printf("mark scope %d\n", ivy->vars->objno);
			ivy_mark_obj(ivy->vars);
		}
		ivy_mark_val(&ivy->stashed);
	}

	// printf("mark_fun_count = %d\n", mark_fun_count);
	// printf("mark_str_count = %d\n", mark_str_count);
	// printf("mark_var_count = %d\n", mark_var_count);
	//printf("mark_obj_count = %d\n", mark_obj_count);

	// printf("Sweeping... "); fflush(stdout);

	// printf("alloc_var_count=%d\n", alloc_var_count);

	// printf("Found free vars = %d\n", free_var_count);
	// printf("Found allocated vars = %d\n", alloc_var_count);

	/* Sweep strings */
	ivy_sweep_strs();

	/* Sweep objects */
	ivy_sweep_objs();

	// printf("Done.\n");
}

