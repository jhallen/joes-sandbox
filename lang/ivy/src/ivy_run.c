/* Byte code interpreter

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

// #define ENABLE_DYNAMIC 1

/* Scope debugging */

#if 0
#define SCOPE_PRINTF(a) printf(a)
#define SCOPE_PRINTF1(a,b) printf((a),(b))
#define SCOPE_PRINTF2(a,b,c) printf((a),(b),(c))
#define SCOPE_PRINTF3(a,b,c,d) printf((a),(b),(c),(d))
#else
#define SCOPE_PRINTF(a)
#define SCOPE_PRINTF1(a,b)
#define SCOPE_PRINTF2(a,b,c)
#define SCOPE_PRINTF3(a,b,c,d)
#endif

#if 0
#define MEM_PRINTF(a) printf(a)
#define MEM_PRINTF1(a,b) printf((a),(b))
#define MEM_PRINTF2(a,b,c) printf((a),(b),(c))
#define MEM_PRINTF3(a,b,c,d) printf((a),(b),(c),(d))
#else
#define MEM_PRINTF(a)
#define MEM_PRINTF1(a,b)
#define MEM_PRINTF2(a,b,c)
#define MEM_PRINTF3(a,b,c,d)
#endif

char *ivy_a_symbol;
char *ivy_b_symbol;
char *ivy_mom_symbol;
char *ivy_dynamic_symbol;
char *ivy_argv_symbol;

Ivy_obj *ivy_globals;

/*
Ivy calling C: returns with call_me set to function to call.

  does it make any sense to have a this pointer for the called function?  Function
  might be a member function...

C calling Ivy: returns to top level with function to call- then top level
should call ivy.  When ivy is done: a C continuation function is called along with
a continuation object.


calling-func:
-------------
  push args onto stack

  call ivy_callstate()... this sets up the call
    an argument should give the continuation C call (and object).

  return to top.  top should call pexe which will eventually return to top with
  call_me set to continuation C call.

  A c function doesnt have scope: instead it has passed object pointer.

  The continuation function should pop the one argument off the stack.
*/

/* Delete a value: returns with pointer to after value deleted */

/* Duplicate a value: returns with pointer before value duplicated */

Ivy_val *ivy_dup_val(Ivy_val *n, Ivy_val *v)
{
	n->idx_type = v->idx_type;
	n->origin = v->origin;
	n->idx = v->idx;

	switch (v->type) {
		case ivy_tLST: {
			long long y = v->u.num, x, z;
			Ivy_obj *obj;
			ivy_obj_val(n, obj = ivy_alloc_obj(16, 4, 4));
			--v;
			for (x = 0, z = 0; x != y; ++x)
				if (v->type == ivy_tPAIR) {
					--v;
					if (v->type == ivy_tNAM)
						v = ivy_dup_val(ivy_set_by_symbol(obj, v[0].u.name), v - 1);
					else if (v->type == ivy_tSTR)
						v = ivy_dup_val(ivy_set_by_string(obj, v[0].u.str->s), v - 1);
					else if (v->type == ivy_tNUM)
						v = ivy_dup_val(ivy_set_by_number(obj, v[0].u.num), v - 1);
					else {
						fprintf(stderr, "Invalid index on stack after tPAIR\n");
						exit(-1);
					}
				} else
					v = ivy_dup_val(ivy_set_by_number(obj, z++), v);
			return v;
		} case ivy_tSTR: {
		        n->type = v->type;
		        n->u.str = v->u.str;
			return v - 1;
		} case ivy_tNAM: {
	                n->type = v->type;
	                n->u.name = v->u.name;
	                return v - 1;
		} case ivy_tOBJ: {
		        n->type = v->type;
		        n->u.obj = v->u.obj;
			return v - 1;
		} case ivy_tCLOSURE: {
		        n->type = v->type;
		        n->u.closure = v->u.closure;
			return v - 1;
		} case ivy_tVOID: {
		        n->type = v->type;
			return v - 1;
		} case ivy_tNUM: {
		        n->type = v->type;
		        n->u.num = v->u.num;
			return v - 1;
		} case ivy_tFP: {
		        n->type = v->type;
		        n->u.fp = v->u.fp;
			return v - 1;
		} default: {
			fprintf(stderr,"dupval of unknown or invalid type code %d\n",v->type);
			exit(-1);
			return v;
		}
	}
}

Ivy_val *ivy_rmval(Ivy_val *v, int line)
{
	switch (v->type) {
		case ivy_tLST: {
			long long y = v->u.num, x;
			--v;
			// printf("rmval list with %d\n", y);
			for (x = 0; x != y; ++x)
				v = ivy_rmval(v, __LINE__);
			break;
		} case ivy_tPAIR: {
			--v;
			v = ivy_rmval(v, __LINE__);
			v = ivy_rmval(v, __LINE__);
			break;
		} default: {
			--v;
		}
	}
	return v;
}

void ivy_expand_stack(Ivy *ivy)
{
	int org_size = ivy->spend - ivy->sptop;
	int new_size = org_size + 1024;
	ivy->sptop = (Ivy_val *) realloc(ivy->sptop, sizeof(Ivy_val) * new_size);
	ivy->sp = ivy->sptop + org_size;
	ivy->spend = ivy->sptop + new_size;
}

/* Add a new level of local variables */

void ivy_scope_push(Ivy *ivy, Ivy_obj *dyn)
{
	Ivy_obj *o;

	o = ivy_alloc_obj(16, 4, 4);

	ivy_obj_val(ivy_set_by_symbol(o, ivy_mom_symbol), ivy->vars);

#ifdef ENABLE_DYNAMIC
	ivy_obj_val(ivy_set_by_symbol(o, ivy_dynamic_symbol), dyn);
#endif

	ivy->vars = o;
	SCOPE_PRINTF1("scope_push, new scope is %p\n", ivy->vars);
}

void ivy_scope_pop(Ivy *ivy)
{
	SCOPE_PRINTF1("scope_pop, pop amount = %d\n", amnt + 1);
	Ivy_obj *o = ivy_get_mom(ivy->vars);
	SCOPE_PRINTF2("scope_pop, pop scope %p restored to %p\n", ivy->vars, o);
	ivy->vars = o;
}

/* Remove a level of local variables */

Ivy_obj *ivy_get_mom(Ivy_obj *o)
{
	Ivy_val *u = ivy_get_by_symbol(o, ivy_mom_symbol);
	Ivy_obj *rtn = 0;
	if (u && u->type == ivy_tOBJ)
		rtn = u->u.obj;
	return rtn;
}

/* Lookup a variable.  Check all scoping levels for the variable */

Ivy_val *ivy_getv_by_symbol_obj(Ivy_obj *o, char *name)
{
	Ivy_obj *next;
	Ivy_val *e;
	do {
		if ((e = ivy_get_by_symbol(o, name))) {
			return e;
		}
		next = ivy_get_mom(o);
	} while ((o = next));
	return 0;
}

Ivy_val *ivy_getv_by_symbol(Ivy *ivy, char *name)
{
	Ivy_val *e = ivy_getv_by_symbol_obj(ivy->vars, name);
	return e;
}

/* Lookup a variable.  Check all scoping levels for the variable.
   If none found, create it in the inner-most level. */

Ivy_val *ivy_setv_by_symbol(Ivy_obj *o, char *name)
{
	Ivy_val *v = ivy_getv_by_symbol_obj(o, name);
	if (!v) {
		v = ivy_set_by_symbol(o, name);
	}
	return v;
}

/* Call a function closure with no args (used for arguments) */

static void call_simple_func(Ivy *ivy, Ivy_closure o, void (*func)(Ivy *,struct ivy_callstate *), struct ivy_callstate *t)
{
	Ivy_obj *ovars = ivy->vars;	/* Save caller's scope */

	// printf("call_simple_func %p\n", o);

	SCOPE_PRINTF2("call_simple_fnuc Switch to closure's scope %p (prev was %p)\n", o.env, ivy->vars);
	ivy_protect_obj(ivy->vars);
	ivy->vars = o.env;	/* Switch to closure's scope */

	if (!o.func->thunk)	/* This should always be a thunk */
		ivy_scope_push(ivy, ovars);	/* Make scoping level for function */

	ivy_push_void(ivy);
	ivy->sp[0].idx.func = func;
	ivy->sp[0].u.obj = ovars;

	if (!o.func->thunk)
		ivy_val(ivy_push(ivy), ivy_tRET_SIMPLE);
	else
		ivy_val(ivy_push(ivy), ivy_tRET_SIMPLE_THUNK);
	ivy->sp[0].idx.callstate = t;
	ivy->sp[0].u.pc = ivy->pc;

	/* Set new program counter value... */
	ivy->pc = o.func->code;
}

static void copy_next_arg(Ivy *ivy, struct ivy_callstate *t);

static void save_arg_result(Ivy *ivy,struct ivy_callstate *t)
{
	ivy_pop(t->result, ivy);
	copy_next_arg(ivy, t);
}

static void call_next_init(Ivy *ivy, struct ivy_callstate *t)
{
	while (t->x != t->o.func->nargs) {
		Ivy_val *a = ivy_get_by_symbol(ivy->vars, t->o.func->args[t->x]);
		if (!a) {
			if (t->o.func->inits[t->x]) {
				ivy_val(ivy_push(ivy), ivy_tRET_NEXT_INIT);
				ivy->sp[0].idx.callstate = t;
				ivy->sp[0].u.pc = ivy->pc;
				ivy->pc = t->o.func->inits[t->x];
				return;
			} else {
				ivy_error_0(ivy->errprn, "Missing arguments");
				longjmp(ivy->err, 1);
				// Is this necessary?
				//a = ivy_set_by_symbol(ivy->vars, t->o.func->args[t->x]);
				//ivy_void_val(a);
				//*ivy_set_by_number(t->argv, t->x) = *a;
			}
		}
		++t->x;
	}
        /* Set new program counter value... */
        if (t->o.func->code)
                ivy->pc = t->o.func->code;
        else
                ivy->call_me = t->o.func->cfunc;
}

static void copy_next_arg(Ivy *ivy, struct ivy_callstate *t)
{
	while (t->x != ivy->sp[0].u.num) {
		Ivy_closure f = { 0, 0 }; // Set to function to call if not quoting
		if (t->q->type != ivy_tPAIR) { /* It's not a named argument */
			if (t->argn >= t->o.func->nargs) { /* Past end of declared arg list */
				if (t->argv) {
					t->result = ivy_set_by_number(t->argv, t->argn - t->o.func->nargs); // Create slot for argument
					t->q = ivy_dup_val(t->result, t->q); // Copy quoted argument from stack
					if (!t->o.func->argv_quote)
						f = t->result->u.closure;
					++t->argn;
				} else {
					// Error
                                        ivy_error_0(ivy->errprn, "Too many arguments for function");
                                        longjmp(ivy->err, 1);
				}
			} else { /* Unnamed arg */
				t->result = ivy_set_by_symbol(ivy->vars, t->o.func->args[t->argn]);
				t->q = ivy_dup_val(t->result, t->q);

				// nope: format args not in argv
				// t->argv_result = ivy_set_by_number(t->argv, t->argn);
				// *t->argv_result = *t->scope_result; // Also save it in argv

				if (!t->o.func->quote[t->argn])
				        f = t->result->u.closure;
				++t->argn;
			}
		} else { /* Named arg */
			int z;
			--t->q; /* Argument name */
			t->result = ivy_set_by_symbol(ivy->vars, t->q->u.name);

			/* Find where it should go in argv */
			// It doesn't go in argv
			//for (z = 0; z != t->o.func->nargs; ++z)
			//	if (!strcmp(t->o.func->args[z], t->q->u.name)) {
			//		t->argv_result = ivy_set_by_number(t->argv, z);
			//		break;
			//	}
			for (z = 0; z != t->o.func->nargs; ++z)
				if (t->o.func->args[z] == t->q->u.name) {
					break;
				}
			// It's fun to not have this...
			//if (z == t->o.func->nargs) {
			//	ivy_error_0(ivy->errprn, "Unknown argument");
			//	longjmp(ivy->err, 1);
			//}

			--t->q; /* Skip over argument name, point to value */
			t->q = ivy_dup_val(t->result, t->q); /* Copy from stack to slot */
			//if (t->argv_result)
			//	*t->argv_result = *t->scope_result; /* Save in argv also */
			
			if (z == t->o.func->nargs || !t->o.func->quote[z])
				f = t->result->u.closure;
		}
		++t->x;
		if (f.func) {
			call_simple_func(ivy, f, save_arg_result, t);
			return;
		}
	}
	/* All arguments done */
        /* Remove argument list */
        ivy->sp = ivy_rmval(ivy->sp, __LINE__);

        /* Push return address */
        if (!t->o.func->thunk)
	        ivy_val(ivy_push(ivy), ivy_tRET_IVY);
	else
	        ivy_val(ivy_push(ivy), ivy_tRET_IVY_THUNK);
//        ivy->sp[0].u.obj = t->ovars;
        ivy->sp[0].idx.callstate = t;
        ivy->sp[0].u.pc = ivy->pc;

        /* Initializers... */
        t->x = 0;
        call_next_init(ivy, t);
}

static void callfunc(Ivy *ivy, Ivy_closure o)
{
	struct ivy_callstate *t;

	t = (struct ivy_callstate *)calloc(1, sizeof(struct ivy_callstate));
	t->o = o;
	// printf("callstate %p fun=%p\n", t, o);
	ivy_void_val(&t->val);
	t->ovars = ivy->vars;	/* Save caller's scope */

	SCOPE_PRINTF2("callstate, switched to closure's scope %p (prev was %p)\n", o.env, ivy->vars);
	ivy_protect_obj(ivy->vars);
	ivy->vars = o.env;	/* Switch to closure's scope */

	if (!o.func->thunk)
		ivy_scope_push(ivy, t->ovars);	/* Make scoping level for function */

	if (o.func->argv)
		ivy_obj_val(ivy_set_by_symbol(ivy->vars, o.func->argv), t->argv = ivy_alloc_obj(16, 4, 4));

	t->x = 0; /* Count of args we've completed */
	t->argn = 0; /* Next arg number to use for unnamed */
	t->q = ivy->sp - 1; /* Arg we're working on now */

	copy_next_arg(ivy, t);
}

/* Functions for "calling" strings and objects */

static void copy_next_str_arg(Ivy *ivy, struct ivy_callstate *t);

static void save_str_arg_result(Ivy *ivy,struct ivy_callstate *t)
{
	/* Save result of arg evaluation */
	ivy_pop(t->result, ivy);
	copy_next_str_arg(ivy, t);
}

void copy_next_str_arg(Ivy *ivy, struct ivy_callstate *t)
{
	while (t->x != ivy->sp[0].u.num) {
		Ivy_closure f = { 0, 0 };
		if (t->q->type != ivy_tPAIR) { /* Unnamed arg */
                        t->result = ivy_set_by_number(t->argv, t->argn); /* Put in argv */
                        t->q = ivy_dup_val(t->result, t->q);
                        f = t->result->u.closure;
                        ++t->argn;
		} else { /* Named arg */
			--t->q;
			t->result = ivy_set_by_symbol(ivy->vars, t->q->u.name); /* Put in scope */
			--t->q; /* Skip arg name, get to value */
			t->q = ivy_dup_val(t->result, t->q);
			f = t->result->u.closure;
		}
		++t->x;
		if (f.func) {
			call_simple_func(ivy, f, save_str_arg_result, t);
			return;
		}
	}
	/* All arguments done */
        /* Remove argument list */
        ivy->sp = ivy_rmval(ivy->sp, __LINE__);

        /* We now have the string or object arguments in argv */
        if (t->val.type == ivy_tSTR) { /* Access a string  */
                Ivy_string *str = t->val.u.str;
                Ivy_val *first_index = ivy_get_by_number(t->argv, 0);
                Ivy_val *second_index = ivy_get_by_number(t->argv, 1);
                if (!first_index) { /* No args, just return the string */
                        SCOPE_PRINTF("copy_next_str_arg (str0):\n");
                        ivy_scope_pop(ivy);
                        *ivy_push(ivy) = t->val;
                } else if (first_index && !second_index) { /* Get single char from string */
                        if (first_index->type == ivy_tNUM) {
                                long long a = first_index->u.num;
                                if (a < 0)
                                        a = str->len + a;
                                if (a >= str->len || a < 0) {
                                        ivy_error_0(ivy->errprn, "Index is past end or beginning of string..");
                                        longjmp(ivy->err, 1);
                                } else { /* OK, finally we can index the string */
                                        int chr = str->s[a];
                                        SCOPE_PRINTF("copy_next_str_arg (str):\n");
                                        ivy_scope_pop(ivy);
                                        ivy_push_int(ivy, chr);
                                }
                        } else {
                                ivy_error_0(ivy->errprn, "Non numeric index to string...");
                                longjmp(ivy->err, 1);
                        }
                } else if (second_index) { /* Get substring from string */
                        if (first_index->type == ivy_tNUM) {
                                long long a = first_index->u.num;
                                if (second_index->type == ivy_tNUM) { /* OK, finally we can access the string */
                                        long long b = second_index->u.num;
                                        char *s;
                                        if (a < 0)
                                                a = str->len + a + 1;
                                        if (b < 0)
                                                b = str->len + b + 1;
                                        if (a > str->len)
                                                a = str->len;
                                        if (b > str->len)
                                                b = str->len;
                                        if (a < 0)
                                                a = 0;
                                        if (b < 0)
                                                b = 0;
                                        if (b < a)
                                                b = a;
                                        s = (char *) malloc(b - a + 1);
                                        memcpy(s, str->s + a, b - a);
                                        s[b - a] = 0;
                                        SCOPE_PRINTF("copy_next_str_arg (str2):\n");
                                        ivy_scope_pop(ivy);
                                        ivy_string_val(ivy_push(ivy), ivy_alloc_str(s, b-a));
                                } else {
                                        ivy_error_0(ivy->errprn, "Non numeric second index to string...");
                                        longjmp(ivy->err, 1);
                                }
                        } else {
                                ivy_error_0(ivy->errprn, "Non numeric first index to string...");
                                longjmp(ivy->err, 1);
                        }
                } else {
                        ivy_error_0(ivy->errprn, "Proper string index is missing...");
                        longjmp(ivy->err, 1);
                }
        } else { /* Access an object */
                Ivy_obj *obj = t->val.u.obj;
                Ivy_val *index = ivy_get_by_number(t->argv, 0);
                if (!index) { /* No args, just return the object */
                        SCOPE_PRINTF("copy_next_str_arg (str0):\n");
                        ivy_scope_pop(ivy);
                        *ivy_push(ivy) = t->val;
                } else {
                        if (index->type == ivy_tNUM) {
                                long long a = index->u.num;
                                Ivy_val *o = ivy_get_by_number(obj, a);
                                SCOPE_PRINTF("copy_next_str_arg (obj):\n");
                                ivy_scope_pop(ivy);

                                if (o)
	                                *ivy_push(ivy) = *o;
				else
					ivy_push_void(ivy);

                                ivy->sp[0].origin = obj;
                                ivy->sp[0].idx_type = ivy_tNUM;
                                ivy->sp[0].idx.num = a;

                        } else if (index->type == ivy_tSTR) {
                                Ivy_string *str = index->u.str;
                                Ivy_val *o = ivy_get_by_string(obj, str->s);
                                SCOPE_PRINTF("copy_next_str_arg (obj2):\n");
                                ivy_scope_pop(ivy);

                                if (o)
                                	*ivy_push(ivy) = *o;
				else
					ivy_push_void(ivy);

                                ivy->sp[0].origin = obj;
                                ivy->sp[0].idx_type = ivy_tSTR;
                                ivy->sp[0].idx.str = str;

                                /* If we just looked up a function, change scope to object it was found in */
                                if (ivy->sp[0].type == ivy_tCLOSURE) {
                                	ivy->sp[0].u.closure.env = obj;
                                }
			} else if (index->type == ivy_tNAM) {
                                char *name = index->u.name;
                                Ivy_val *o = ivy_getv_by_symbol_obj(obj, name);
                                SCOPE_PRINTF("copy_next_str_arg (obj2):\n");
                                ivy_scope_pop(ivy);

                                if (o)
                                	*ivy_push(ivy) = *o;
				else
					ivy_push_void(ivy);

                                ivy->sp[0].origin = obj;
                                ivy->sp[0].idx_type = ivy_tNAM;
                                ivy->sp[0].idx.name = name;

                                /* If we just looked up a function, change scope to object it was found in */
                                /* But only if obj has a mom... */
                                if (ivy->sp[0].type == ivy_tCLOSURE && ivy_get_by_symbol(obj, ivy_mom_symbol)) {
                                	ivy->sp[0].u.closure.env = obj;
                                }
                        } else {
                                ivy_error_0(ivy->errprn, "Invalid object index type...");
                                longjmp(ivy->err, 1);
                        }
                }
        }

        /* All done */
        ivy_rmval(&t->val, __LINE__);
        free(t);
}

static void callval(Ivy *ivy, Ivy_val val)
{
	struct ivy_callstate *t;

	t = (struct ivy_callstate *)calloc(1, sizeof(struct ivy_callstate));
	t->o.func = 0;
	t->o.env = 0;
	t->val = val;
	t->ovars = ivy->vars;	/* Save caller's scope */

	SCOPE_PRINTF("call str:\n");
	ivy_scope_push(ivy, t->ovars);	/* Make scoping level for function */

	ivy_obj_val(ivy_set_by_symbol(ivy->vars, ivy_argv_symbol), t->argv = ivy_alloc_obj(16, 4, 4));

	t->x = 0; /* Count of args we've completed */
	t->argn = 0; /* Next arg number to use for unnamed */
	t->q = ivy->sp - 1; /* Arg we're working on now */

	copy_next_str_arg(ivy, t);
}

/* Return from a subroutine */

static int retfunc(Ivy *ivy)
{
	/* This used to just return */
	Ivy_obj *restore_vars;
	Ivy_pseudo *restore_pc;
	Ivy_val rtn_val;

	/* Pop return value for a sec... */
	/* (we push it back on after we pop return address) */
	ivy_pop(&rtn_val, ivy);

	if (ivy->sp == ivy->sptop) {
		/* Nothing to return to?  We must be done! */
		*ivy_push(ivy) = rtn_val;
		return 0;
	}

	/* Return and call a continuation function */
	if (ivy->sp[0].type == ivy_tRET_NEXT_INIT) {
		struct ivy_callstate *t;
		Ivy_val *a;
		SCOPE_PRINTF("ret_next_init:\n");
		ivy->pc = ivy->sp[0].u.pc;
		t = ivy->sp[0].idx.callstate;
		--ivy->sp;

		a = ivy_set_by_symbol(ivy->vars, t->o.func->args[t->x]);
		*a = rtn_val;
		// *ivy_set_by_number(t->argv, t->x) = *a;
		++t->x;
		call_next_init(ivy, t);

		return 1;
	} else if (ivy->sp[0].type == ivy_tRET_SIMPLE_THUNK) {
		void (*func)(Ivy *,struct ivy_callstate *);
		struct ivy_callstate *t;
		SCOPE_PRINTF("retfunc simple:\n");
		ivy->pc = ivy->sp[0].u.pc;
		t = ivy->sp[0].idx.callstate;

		--ivy->sp;
		func = ivy->sp[0].idx.func;
		if (ivy->sp[0].u.obj) {
                        SCOPE_PRINTF2("retfunc RET_SIMPLE_THUNK Restore scope to %p (was %p)\n", ivy->sp[0].u.obj, ivy->vars);
			ivy->vars = ivy->sp[0].u.obj;
                }

		ivy->sp[0] = rtn_val;

		func(ivy, t);

		return 1;
	} else if (ivy->sp[0].type == ivy_tRET_SIMPLE) {
		void (*func)(Ivy *,struct ivy_callstate *);
		struct ivy_callstate *t;
		SCOPE_PRINTF("retfunc simple:\n");
		ivy->pc = ivy->sp[0].u.pc;
		t = ivy->sp[0].idx.callstate;

		--ivy->sp;
		func = ivy->sp[0].idx.func;
		if (ivy->sp[0].u.obj) {
                        ivy_scope_pop(ivy);
                        SCOPE_PRINTF2("retfunc RET_SIMPLE Restore scope to %p (was %p)\n", ivy->sp[0].u.obj, ivy->vars);
			ivy->vars = ivy->sp[0].u.obj;
                }

		ivy->sp[0] = rtn_val;

		func(ivy, t);

		return 1;
	}

	/* Get old program counter and scope */
	if (ivy->sp[0].type == ivy_tRET_IVY) { /* Return to Ivy code */
	        struct ivy_callstate *t;
		SCOPE_PRINTF("retfunc ivy:\n");
                ivy_scope_pop(ivy);
		restore_pc = ivy->sp[0].u.pc;
		t = ivy->sp[0].idx.callstate;
                restore_vars = t->ovars;

		/* Put return value back on stack */
		ivy->sp[0] = rtn_val;

		/* Continue... */
		ivy->pc = restore_pc;
		SCOPE_PRINTF2("retfunc RET_IVY, Restore scope to %p (was %p)\n", restore_vars, ivy->vars);
		ivy->vars = restore_vars;

		free(t);
	} else if (ivy->sp[0].type == ivy_tRET_IVY_THUNK) { /* Return to Ivy code */
	        struct ivy_callstate *t;
		SCOPE_PRINTF("retfunc ivy:\n");
		restore_pc = ivy->sp[0].u.pc;
		t = ivy->sp[0].idx.callstate;
                restore_vars = t->ovars;

		/* Put return value back on stack */
		ivy->sp[0] = rtn_val;

		/* Continue... */
		ivy->pc = restore_pc;
		SCOPE_PRINTF2("retfunc RET_IVY, Restore scope to %p (was %p)\n", restore_vars, ivy->vars);
		ivy->vars = restore_vars;

		free(t);
	} else {
		ivy_error_0(ivy->errprn, "Error: bad subroutine return point?");
		longjmp(ivy->err, 1);
	}
	return 1;
}

static void doSET(Ivy *ivy, Ivy_val *dest, Ivy_val *src)
{
	if (!dest->origin) {
		/* Maybe a multi-assignment */
		if (dest->type == ivy_tOBJ && src->type == ivy_tOBJ) {
			Ivy_obj *to = dest->u.obj;
			Ivy_obj *from = src->u.obj;
			int x;
			for (x = 0; x != to->ary_len; ++x) {
				if (x == from->ary_len) {
					ivy_error_0(ivy->errprn, "Incorrect no. of args");
					longjmp(ivy->err, 1);
                                } else {
					doSET(ivy, &to->ary[x], &from->ary[x]);
                                }
                        }
		} else {
			ivy_error_0(ivy->errprn, "Improper L-value");
                        longjmp(ivy->err, 1);
		}
	} else {
		switch (dest->idx_type) {
			case ivy_tNUM: {
				ivy_dup_val(ivy_set_by_number(dest->origin, dest->idx.num), src);
				break;
			} case ivy_tNAM: {
				ivy_dup_val(ivy_set_by_symbol(dest->origin, dest->idx.name), src);
				break;
			} case ivy_tSTR: {
				ivy_dup_val(ivy_set_by_string(dest->origin, dest->idx.str->s), src);
				break;
			} default: {
				ivy_error_0(ivy->errprn, "Unknown L-value type?");
	                        longjmp(ivy->err, 1);
			}
		}
	}
}

/* Run some code...
 *
 * Returns: 0 for end of program
 *          1 for there is a C function to call in ivy->call_me
 *            ...after it returns call pexe again to continue
 */

/* Show the stack */

void ivy_showstack(Ivy *ivy)
{
        Ivy_val *sp;
        for (sp = ivy->sp; sp != ivy->sptop;) {
                switch(sp->type) {
                        case ivy_tNUM: {
                                fprintf(ivy->out, "%d:	Integer = %lld\n", (int)(sp - ivy->sptop), sp->u.num);
                                sp--;
                                break;
                        } case ivy_tSTR: {
                                fprintf(ivy->out, "%d:	String = \"%s\"\n", (int)(sp - ivy->sptop), sp->u.str->s);
                                sp--;
                                break;
                        } case ivy_tNAM: {
                                fprintf(ivy->out, "%d:	Symbol = %s\n", (int)(sp - ivy->sptop), sp->u.name);
                                sp--;
                                break;
                        } case ivy_tOBJ: {
                                fprintf(ivy->out, "%d:	Object = %p\n", (int)(sp - ivy->sptop), sp->u.obj);
                                --sp;
                                break;
                        } case ivy_tCLOSURE: {
                                fprintf(ivy->out, "%d:	CLOSURE\n", (int)(sp - ivy->sptop));
                                --sp;
                                break;
			} case ivy_tPAIR: {
                                fprintf(ivy->out, "%d:	Pair\n", (int)(sp - ivy->sptop));
                                --sp;
				break;
                        } case ivy_tLST: {
                                fprintf(ivy->out, "%d:	LST = %lld\n", (int)(sp - ivy->sptop), sp->u.num);
                                --sp;
                                break;
                        } case ivy_tVOID: {
                                fprintf(ivy->out, "%d:	Void\n", (int)(sp - ivy->sptop));
                                --sp;
                                break;
                        } case ivy_tFP: {
                                fprintf(ivy->out, "%d:	Float = %lg\n", (int)(sp - ivy->sptop), sp->u.fp);
                                --sp;
                                break;
                        } case ivy_tRET_IVY: {
                                fprintf(ivy->out, "%d:	RET_IVY (pc = %p, callstate = %p)\n", (int)(sp - ivy->sptop), sp->u.pc, sp->idx.callstate);
                                --sp;
                                break;
                        } case ivy_tRET_IVY_THUNK: {
                                fprintf(ivy->out, "%d:	RET_IVY_THUNK (pc = %p, callstate = %p)\n", (int)(sp - ivy->sptop), sp->u.pc, sp->idx.callstate);
                                --sp;
                                break;
                        } case ivy_tRET_SIMPLE: {
                                fprintf(ivy->out, "%d:	RET_SIMPLE (pc = %p, callstate = %p", (int)(sp - ivy->sptop), sp->u.pc, sp->idx.callstate);
                                --sp;
                                fprintf(ivy->out, "func = %p, obj = %p)\n", sp->idx.func, sp->u.obj);
                                --sp;
                                break;
                        } case ivy_tRET_SIMPLE_THUNK: {
                                fprintf(ivy->out, "%d:	RET_SIMPLE_THUNK (pc = %p, callstate = %p", (int)(sp - ivy->sptop), sp->u.pc, sp->idx.callstate);
                                --sp;
                                fprintf(ivy->out, "func = %p, obj = %p)\n", sp->idx.func, sp->u.obj);
                                --sp;
                                break;
                        } case ivy_tRET_NEXT_INIT: {
                                fprintf(ivy->out, "%d:	RET_NEXT_INIT (pc = %p, callstate = %p)\n", (int)(sp - ivy->sptop), sp->u.pc, sp->idx.callstate);
                                --sp;
                                break;
			} case ivy_tSCOPE: {
                                fprintf(ivy->out, "%d:	SCOPE (obj = %p)\n", (int)(ivy->sp - ivy->sptop), ivy->sp->u.obj);
				--sp;
				break;
                        } default: {
                                fprintf(ivy->out, "%d:	Unknown type = %d\n", (int)(sp - ivy->sptop), sp->type);
                                --sp;
                                break;
                        }
                }
        }
}

static int pexe(Ivy *ivy, int trace)
{
	Ivy_pseudo *pc = ivy->pc;

	for (;;) { /* if (trace) fprintf(ivy->out,"-----\n"), ivy_showstack(ivy), ivy_disasm(ivy->out, pc, 0, 1);  */ switch (*pc++) {
                case ivy_iBRA: {	/* Branch unconditionally */
                        pc += ivy_align_o(pc, sizeof(int));
                        pc += *(int *)pc;
			ivy_clear_protected();
			break;
                } case ivy_iBEQ: {	/* Branch if zero or void */
                        pc += ivy_align_o(pc, sizeof(int));
                        if (ivy->sp->type == ivy_tNUM)
                                if (ivy->sp->u.num == 0) {
                                        ivy->sp = ivy_rmval(ivy->sp, __LINE__);
                                        pc += *(int *)pc;
                                } else {
                                        ivy->sp = ivy_rmval(ivy->sp, __LINE__);
                                        pc += sizeof(int);
                                }
                        else if (ivy->sp->type == ivy_tVOID) {
                                ivy->sp = ivy_rmval(ivy->sp, __LINE__);
                                pc += *(int *)pc;
                        } else {
                                ivy->sp = ivy_rmval(ivy->sp, __LINE__);
                                pc += sizeof(int);
                        }
			ivy_clear_protected();
			break;
		} case ivy_iBNE: {	/* Branch if non-zero or non-void */
			pc += ivy_align_o(pc, sizeof(int));
			if (ivy->sp->type == ivy_tVOID) {
                                ivy->sp = ivy_rmval(ivy->sp, __LINE__);
				pc += sizeof(int);
			} else if (ivy->sp->type != ivy_tNUM) {
				ivy->sp = ivy_rmval(ivy->sp, __LINE__);
				pc += *(int *)pc;
			} else if (ivy->sp->u.num) {
                                ivy->sp = ivy_rmval(ivy->sp, __LINE__);
				pc += *(int *)pc;
			} else {
                                ivy->sp = ivy_rmval(ivy->sp, __LINE__);
				pc += sizeof(int);
			}
			ivy_clear_protected();
			break;
		} case ivy_iBGT: {	/* Branch if > 0 */
			pc += ivy_align_o(pc, sizeof(int));
			if (ivy->sp->type == ivy_tVOID) {
                                ivy->sp = ivy_rmval(ivy->sp, __LINE__);
				pc+= *(int *)pc;
			} else if (ivy->sp->type != ivy_tNUM) {
			        ivy_error_0(ivy->errprn, "Argument for branch is not a number?");
				longjmp(ivy->err, 1);
			} else if (ivy->sp->u.num > 0) {
                                ivy->sp = ivy_rmval(ivy->sp, __LINE__);
                                pc += *(int *)pc;
			} else {
                                ivy->sp = ivy_rmval(ivy->sp, __LINE__);
				pc += sizeof(int);
                        }
			ivy_clear_protected();
			break;
		} case ivy_iBLT: {	/* Branch if < 0 */
			pc += ivy_align_o(pc, sizeof(int));
			if (ivy->sp->type == ivy_tVOID) {
                                ivy->sp = ivy_rmval(ivy->sp, __LINE__);
				pc += sizeof(int);
			} else if (ivy->sp->type != ivy_tNUM) {
			        ivy_error_0(ivy->errprn, "Argument for branch is not a number?");
				longjmp(ivy->err, 1);
			} else if (ivy->sp->u.num < 0) {
                                ivy->sp = ivy_rmval(ivy->sp, __LINE__);
				pc += *(int *)pc;
			} else {
                                ivy->sp = ivy_rmval(ivy->sp, __LINE__);
				pc += sizeof(int);
                        }
			ivy_clear_protected();
			break;
		} case ivy_iBGE: {	/* Branch if >= 0 */
			pc += ivy_align_o(pc, sizeof(int));
			if (ivy->sp->type == ivy_tVOID) {
                                ivy->sp = ivy_rmval(ivy->sp, __LINE__);
				pc += *(int *)pc;
			} else if (ivy->sp->type != ivy_tNUM) {
			        ivy_error_0(ivy->errprn, "Argument for branch is not a number?");
				longjmp(ivy->err, 1);
			} else if (ivy->sp->u.num >= 0) {
                                ivy->sp = ivy_rmval(ivy->sp, __LINE__);
				pc += *(int *)pc;
			} else {
                                ivy->sp = ivy_rmval(ivy->sp, __LINE__);
				pc += sizeof(int);
                        }
			ivy_clear_protected();
			break;
		} case ivy_iBLE: {	/* Branch if <= 0 */
			pc += ivy_align_o(pc, sizeof(int));
			if (ivy->sp->type == ivy_tVOID) {
                                ivy->sp = ivy_rmval(ivy->sp, __LINE__);
				pc += *(int *)pc;
			} else if (ivy->sp->type != ivy_tNUM) {
			        ivy_error_0(ivy->errprn, "Argument for branch is not a number?");
				longjmp(ivy->err, 1);
			} else if (ivy->sp->u.num <= 0) {
                                ivy->sp = ivy_rmval(ivy->sp, __LINE__);
				pc += *(int *)pc;
			} else {
                                ivy->sp = ivy_rmval(ivy->sp, __LINE__);
				pc += sizeof(int);
                        }
			ivy_clear_protected();
			break;
		} case ivy_iCOM: {	/* 1's complement */
			if (ivy->sp->type == ivy_tNUM)
				ivy->sp->u.num = ~ivy->sp->u.num;
			else {
			        ivy_error_0(ivy->errprn, "Argument for complement is not a number?");
				longjmp(ivy->err, 1);
                        }
			break;
		} case ivy_iNEG: {	/* 2's complement */
			if (ivy->sp->type == ivy_tNUM)
				ivy->sp->u.num = -ivy->sp->u.num;
			else if (ivy->sp->type == ivy_tFP)
				ivy->sp->u.fp = -ivy->sp->u.fp;
			else {
			        ivy_error_0(ivy->errprn, "Argument for negate is not a number?");
				longjmp(ivy->err, 1);
                        }
			break;
		} case ivy_iSHL: {	/* Shift left */
			if (ivy->sp[0].type == ivy_tNUM && ivy->sp[-1].type == ivy_tNUM)
				ivy->sp[-1].u.num <<= ivy->sp[0].u.num, --ivy->sp;
			else {
			        ivy_error_0(ivy->errprn, "Improper types for shift-left");
				longjmp(ivy->err, 1);
                        }
			break;
                } case ivy_iSHR: {	/* Shift right */
			if (ivy->sp[0].type == ivy_tNUM && ivy->sp[-1].type == ivy_tNUM)
				ivy->sp[-1].u.num >>= ivy->sp[0].u.num, --ivy->sp;
			else {
			        ivy_error_0(ivy->errprn, "Improper types for shift-right");
				longjmp(ivy->err, 1);
                        }
			break;
                } case ivy_iMUL: {	/* Multiply */
			if (ivy->sp[0].type == ivy_tNUM && ivy->sp[-1].type == ivy_tNUM)
				ivy->sp[-1].u.num *= ivy->sp[0].u.num, --ivy->sp;
			else if (ivy->sp[0].type == ivy_tFP && ivy->sp[-1].type == ivy_tFP)
				ivy->sp[-1].u.fp *= ivy->sp[0].u.fp, --ivy->sp;
			else if (ivy->sp[-1].type == ivy_tFP && ivy->sp[0].type == ivy_tNUM)
				ivy->sp[-1].u.fp *= ivy->sp[0].u.num, --ivy->sp;
			else if (ivy->sp[-1].type == ivy_tNUM && ivy->sp[0].type == ivy_tFP) {
				ivy->sp[-1].u.fp = ivy->sp[-1].u.num * ivy->sp[0].u.fp;
				ivy->sp[-1].type = ivy_tFP;
				--ivy->sp;
			} else {
			        ivy_error_0(ivy->errprn, "Improper types for multiply");
				longjmp(ivy->err, 1);
                        }
			break;
                } case ivy_iDIV: {	/* Divide */
			if (ivy->sp[0].type == ivy_tNUM && ivy->sp[-1].type == ivy_tNUM)
				ivy->sp[-1].u.num /= ivy->sp[0].u.num, --ivy->sp;
			else if (ivy->sp[0].type == ivy_tFP && ivy->sp[-1].type == ivy_tFP)
				ivy->sp[-1].u.fp /= ivy->sp[0].u.fp, --ivy->sp;
			else if (ivy->sp[-1].type == ivy_tFP && ivy->sp[0].type == ivy_tNUM)
				ivy->sp[-1].u.fp /= ivy->sp[0].u.num, --ivy->sp;
			else if (ivy->sp[-1].type == ivy_tNUM && ivy->sp[0].type == ivy_tFP) {
				ivy->sp[-1].u.fp = ivy->sp[-1].u.num / ivy->sp[0].u.fp;
				ivy->sp[-1].type = ivy_tFP;
				--ivy->sp;
			} else {
			        ivy_error_0(ivy->errprn, "Improper types for divide");
				longjmp(ivy->err, 1);
			}
			break;
                } case ivy_iMOD: {	/* Remainder */
			if (ivy->sp[0].type == ivy_tNUM && ivy->sp[-1].type == ivy_tNUM)
				ivy->sp[-1].u.num %= ivy->sp[0].u.num, --ivy->sp;
			else {
			        ivy_error_0(ivy->errprn, "Improper types for remainder");
				longjmp(ivy->err, 1);
                        }
			break;
                } case ivy_iAND: {	/* Bit-wise AND */
			if (ivy->sp[0].type == ivy_tNUM && ivy->sp[-1].type == ivy_tNUM)
				ivy->sp[-1].u.num &= ivy->sp[0].u.num, --ivy->sp;
			else {
			        ivy_error_0(ivy->errprn, "Improper types for bit-wise and");
				longjmp(ivy->err, 1);
                        }
			break;
                } case ivy_iADD: {
			if (ivy->sp[0].type == ivy_tNUM && ivy->sp[-1].type == ivy_tNUM) {	/* Add numbers */
			        long long a = ivy->sp[-1].u.num;
			        a += ivy->sp[0].u.num;
                                ivy->sp = ivy_rmval(ivy->sp, __LINE__);
                                ivy->sp = ivy_rmval(ivy->sp, __LINE__);
                                ivy_push_int(ivy, a);
				// ivy->sp[-1].u.num += ivy->sp[0].u.num, --ivy->sp;
			} else if (ivy->sp[0].type == ivy_tFP && ivy->sp[-1].type == ivy_tFP)
				ivy->sp[-1].u.fp += ivy->sp[0].u.fp, --ivy->sp;
			else if (ivy->sp[-1].type == ivy_tFP && ivy->sp[0].type == ivy_tNUM)
				ivy->sp[-1].u.fp += ivy->sp[0].u.num, --ivy->sp;
			else if (ivy->sp[-1].type == ivy_tNUM && ivy->sp[0].type == ivy_tFP) {
				ivy->sp[-1].u.fp = ivy->sp[-1].u.num + ivy->sp[0].u.fp;
				ivy->sp[-1].type = ivy_tFP;
				--ivy->sp;
			} else if (ivy->sp[0].type == ivy_tSTR && ivy->sp[-1].type == ivy_tSTR) {	/* Concat. strings */
				int len =
				    ivy->sp[0].u.str->len + ivy->sp[-1].u.str->len;
				char *s = (char *) malloc(len + 1);
				Ivy_string *str = ivy_alloc_str(s, len);
				memcpy(s, ivy->sp[-1].u.str->s, ivy->sp[-1].u.str->len);
				memcpy(s + ivy->sp[-1].u.str->len, ivy->sp[0].u.str->s, ivy->sp[0].u.str->len);
				s[len] = 0;
				ivy->sp = ivy_rmval(ivy->sp = ivy_rmval(ivy->sp, __LINE__), __LINE__);
				ivy_string_val(++ivy->sp, str);
			} else {	/* Append element to object */
				Ivy_val newv;
				ivy_pop(&newv, ivy);
				if (ivy->sp[0].type == ivy_tOBJ) {
					Ivy_obj *n = ivy_dup_obj(ivy->sp[0].u.obj, ivy->sp, 0, __LINE__);
					ivy->sp = ivy_rmval(ivy->sp, __LINE__);
					ivy_obj_val(++ivy->sp, n);
					*ivy_set_by_number(n, ivy->sp[0].u.obj->ary_len) = newv;
				} else {
                                        ivy_error_0(ivy->errprn, "Improper types for add");
					longjmp(ivy->err, 1);
                                }
			}
			break;
                } case ivy_iSUB: {	/* Subtract */
			if (ivy->sp[0].type == ivy_tNUM && ivy->sp[-1].type == ivy_tNUM) {
			        long long a = ivy->sp[-1].u.num;
			        a -= ivy->sp[0].u.num;
                                ivy->sp = ivy_rmval(ivy->sp, __LINE__);
                                ivy->sp = ivy_rmval(ivy->sp, __LINE__);
                                ivy_push_int(ivy, a);
				// ivy->sp[-1].u.num -= ivy->sp[0].u.num, --ivy->sp;
			} else if (ivy->sp[0].type == ivy_tFP && ivy->sp[-1].type == ivy_tFP)
				ivy->sp[-1].u.fp -= ivy->sp[0].u.fp, --ivy->sp;
			else if (ivy->sp[-1].type == ivy_tFP && ivy->sp[0].type == ivy_tNUM)
				ivy->sp[-1].u.fp -= ivy->sp[0].u.num, --ivy->sp;
			else if (ivy->sp[-1].type == ivy_tNUM && ivy->sp[0].type == ivy_tFP) {
				ivy->sp[-1].u.fp = ivy->sp[-1].u.num - ivy->sp[0].u.fp;
				ivy->sp[-1].type = ivy_tFP;
				--ivy->sp;
			} else {
			        ivy_error_0(ivy->errprn, "Improper types for subtract");
				longjmp(ivy->err, 1);
                        }
			break;
                } case ivy_iOR: {
			if (ivy->sp[0].type == ivy_tNUM && ivy->sp[-1].type == ivy_tNUM)	/* Bit-wise or */
				ivy->sp[-1].u.num |= ivy->sp[0].u.num, --ivy->sp;
			else {
				if (ivy->sp[0].type == ivy_tOBJ && ivy->sp[-1].type == ivy_tOBJ) {	/* Union objects */
					int x;
					Ivy_obj *t = ivy->sp[0].u.obj;
					int a = ivy->sp[-1].u.obj->ary_len;
					Ivy_obj *newo = ivy_dup_obj(ivy->sp[-1].u.obj, &ivy->sp[-1], 0, __LINE__);
					ivy_rmval(&ivy->sp[-1], __LINE__);
					ivy_obj_val(ivy->sp - 1, newo);
					for (x = 0; x != t->ary_len; ++x) {	/* Append array elements */
						*ivy_set_by_number(ivy->sp[-1].u.obj, x + a) = t->ary[x];
					}
					for (x = 0; x != (t->nam_tab_mask + 1); ++x)	/* Union symbols */
						if (t->nam_tab[x].name) {
							*ivy_set_by_symbol(ivy->sp[-1].u.obj, t->nam_tab[x].name) = t->nam_tab[x].val;
						}
					for (x = 0; x != (t->str_tab_mask + 1); ++x)	/* Union strings */
						if (t->str_tab[x].name) {
							*ivy_set_by_string(ivy->sp[-1].u.obj, t->str_tab[x].name) = t->str_tab[x].val;
						}
					ivy->sp = ivy_rmval(ivy->sp, __LINE__);
				} else {
				        ivy_error_0(ivy->errprn, "Improper types for union");
					longjmp(ivy->err, 1);
                                }
			}
			break;
                } case ivy_iXOR: {	/* Exclusive or */
			if (ivy->sp[0].type == ivy_tNUM && ivy->sp[-1].type == ivy_tNUM)
				ivy->sp[-1].u.num ^= ivy->sp[0].u.num, --ivy->sp;
			else {
			        ivy_error_0(ivy->errprn, "Improper types for exclusive or");
				longjmp(ivy->err, 1);
                        }
			break;
                } case ivy_iCMP: {
			if (ivy->sp[0].type == ivy_tNUM && ivy->sp[-1].type == ivy_tNUM) {	/* Compare numbers */
				if (ivy->sp[-1].u.num == ivy->sp[0].u.num)
					ivy->sp[-1].u.num = 0;
				else if (ivy->sp[-1].u.num > ivy->sp[0].u.num)
					ivy->sp[-1].u.num = 1;
				else
					ivy->sp[-1].u.num = -1;
				--ivy->sp;
			} else if (ivy->sp[0].type == ivy_tVOID && ivy->sp[-1].type == ivy_tVOID) {
				ivy->sp = ivy_rmval(ivy->sp, __LINE__);
				ivy->sp = ivy_rmval(ivy->sp, __LINE__);
				ivy_int_val(++ivy->sp, 0);
			} else if (ivy->sp[0].type == ivy_tVOID
				   || ivy->sp[-1].type == ivy_tVOID) {
				ivy->sp = ivy_rmval(ivy->sp, __LINE__);
				ivy->sp = ivy_rmval(ivy->sp, __LINE__);
				ivy_int_val(++ivy->sp, 1);
			} else if (ivy->sp[0].type == ivy_tFP && ivy->sp[-1].type == ivy_tFP) {
				if (ivy->sp[-1].u.fp == ivy->sp[0].u.fp)
					ivy->sp[-1].u.num = 0;
				else if (ivy->sp[-1].u.fp > ivy->sp[0].u.fp)
					ivy->sp[-1].u.num = 1;
				else
					ivy->sp[-1].u.num = -1;
				ivy->sp[-1].type = ivy_tNUM;
				--ivy->sp;
			} else if (ivy->sp[-1].type == ivy_tNUM
				   && ivy->sp[0].type == ivy_tFP) {
				if (ivy->sp[-1].u.num == ivy->sp[0].u.fp)
					ivy->sp[-1].u.num = 0;
				else if (ivy->sp[-1].u.num > ivy->sp[0].u.fp)
					ivy->sp[-1].u.num = 1;
				else
					ivy->sp[-1].u.num = -1;
				--ivy->sp;
			} else if (ivy->sp[-1].type == ivy_tFP
				   && ivy->sp[0].type == ivy_tNUM) {
				if (ivy->sp[-1].u.fp == ivy->sp[0].u.num)
					ivy->sp[-1].u.num = 0;
				else if (ivy->sp[-1].u.fp > ivy->sp[0].u.num)
					ivy->sp[-1].u.num = 1;
				else
					ivy->sp[-1].u.num = -1;
				ivy->sp[-1].type = ivy_tNUM;
				--ivy->sp;
			} else if (ivy->sp[0].type == ivy_tSTR && ivy->sp[-1].type == ivy_tSTR) {	/* Compare strings */
				int c = strcmp(ivy->sp[-1].u.str->s, ivy->sp[0].u.str->s);
				ivy->sp = ivy_rmval(ivy->sp = ivy_rmval(ivy->sp, __LINE__), __LINE__);
				ivy_int_val(++ivy->sp, c);
			} else if (ivy->sp[0].type == ivy_tOBJ && ivy->sp[-1].type == ivy_tOBJ) {	/* Compare objects */
				int c;
				if (ivy->sp[0].u.obj == ivy->sp[-1].u.obj)
					c = 0;
				else
					c = 1;
				ivy->sp = ivy_rmval(ivy->sp = ivy_rmval(ivy->sp, __LINE__), __LINE__);
				ivy_int_val(++ivy->sp, c);
			} else if (ivy->sp[0].type == ivy_tNAM && ivy->sp[-1].type == ivy_tNAM) {
				int c = !(ivy->sp[0].u.name == ivy->sp[-1].u.name);
				ivy->sp = ivy_rmval(ivy->sp = ivy_rmval(ivy->sp, __LINE__), __LINE__);
				ivy_int_val(++ivy->sp, c);
			} else {
			        ivy_error_0(ivy->errprn, "Improper types for compare");
				longjmp(ivy->err, 1);
                        }
			break;
                } case ivy_iBEG: {	/* New scoping level */
		        SCOPE_PRINTF("iBEG:\n");
		        ivy_push_scope(ivy, ivy->vars); // Save old scope on stack
			ivy_scope_push(ivy,ivy->vars); // Create new scope
			break;
                } case ivy_iEND: {	/* Eliminate scoping level */
		        SCOPE_PRINTF("iEND\n");
		        ivy->vars = ivy->sp->u.obj; // Restore scope
		        --ivy->sp;
			break;
                } case ivy_iLOC: {	/* Create local variables */
                        long long y = ivy->sp--->u.num, x; /* must be tLST */
                        for (x = 0; x != y; ++x) {
                        	ivy_set_by_symbol(ivy->vars, ivy->sp[0].u.name); /* must be tNAM */
				ivy->sp = ivy_rmval(ivy->sp, __LINE__);
			}
			break;
                } case ivy_iGET: {	/* Replace variable's name with its value */
			if (ivy->sp->type != ivy_tNAM) {
			        ivy_error_0(ivy->errprn, "Incorrect argument for iGET (supposed to be a string)");
				longjmp(ivy->err, 1);
			} else {
				char *name = ivy->sp[0].u.name;
				Ivy_obj *o = ivy->vars;
				Ivy_val *e;
				do {
					if ((e = ivy_get_by_symbol(o, name)))
						break;
				} while ((o = ivy_get_mom(o)));
				
                                if (e) { /* We found it */
                                	*ivy->sp = *e;
					ivy->sp[0].origin = o;
					ivy->sp[0].idx_type = ivy_tNAM;
					ivy->sp[0].idx.name = name;
                                } else { /* It's new */
                                	ivy_void_val(ivy->sp);
					ivy->sp[0].origin = ivy->vars;
					ivy->sp[0].idx_type = ivy_tNAM;
					ivy->sp[0].idx.name = name;
                                }
			}
			break;
                } case ivy_iGETF: {	/* Replace variable's name with its value, force current scope */
			if (ivy->sp->type != ivy_tNAM) {
			        ivy_error_0(ivy->errprn, "Improper argument for iGETF (supposed to be a name)");
				longjmp(ivy->err, 1);
			} else {
				char *name = ivy->sp[0].u.name;
				Ivy_val *o = ivy_set_by_symbol(ivy->vars, name);
				ivy->sp[0] = *o;
				ivy->sp[0].origin = ivy->vars;
				ivy->sp[0].idx_type = ivy_tNAM;
				ivy->sp[0].idx.name = name;
			}
			break;
		} case ivy_iENV: {
			if (ivy->sp->type != ivy_tOBJ || ivy->sp[-1].type != ivy_tCLOSURE) {
			        ivy_error_0(ivy->errprn, "Improper argument for ::");
				longjmp(ivy->err, 1);
			} else {
				ivy->sp[-1].u.closure.env = ivy->sp[0].u.obj;
				--ivy->sp;
			}
			break;
                } case ivy_iAT: { /* This is the same as ivy_iCALL except no args */
			if (ivy->sp->type != ivy_tCLOSURE) {
			        ivy_error_0(ivy->errprn, "Improper argument for *");
				longjmp(ivy->err, 1);
			} else {
				Ivy_closure f;
				ivy->pc = pc;
				f = ivy->sp->u.closure;
				ivy_lst_val(ivy->sp, 0);
				callfunc(ivy, f);
				pc = ivy->pc;
				if (ivy->call_me) // Call a C-function...
					return 1;
				else
					break;
			}
			break;
                } case ivy_iSET: {	/* Set variable to value */
			doSET(ivy, ivy->sp, ivy->sp - 1);
			ivy->sp = ivy_rmval(ivy->sp, __LINE__);
			break;
		} case ivy_iCALL: {
                        /* If left side is VOID and it's from a variable,
                           create a new object */
/*
                        if (ivy->sp->type == tVOID && ivy->sp->origin) {
                        	Ivy_val newv = mkpval(tOBJ, ivy_alloc_obj(16, 4, 4));
                        	doSET(ivy, ivy->sp, &newv);
                        	ivy->sp[0].type = newv.type;
                        	ivy->sp[0].u = newv.u;
                        }
*/
                        if (ivy->sp->type == ivy_tCLOSURE) {
                                ivy->pc = pc;
                                callfunc(ivy, ivy->sp--->u.closure);
                                pc = ivy->pc;
                                if (ivy->call_me) // Call a C-function...
                                        return 1;
                                else
                                        break;
                        } else if (ivy->sp->type == ivy_tSTR || ivy->sp->type == ivy_tOBJ) {
                                ivy->pc = pc;
                                callval(ivy, *ivy->sp--);
                                pc = ivy->pc;
                                if (ivy->call_me)
                                        return 1;
                                else
                                        break;
                        } else {
                                /* If item is not a function, string or object, just return
                                 * it: allow you to type 'a' and see result in calculator mode */
                                if (ivy->sp[-1].u.num) {
                                        ivy_error_0(ivy->errprn, "No args allowed");
                                        longjmp(ivy->err, 1);
                                }
                                ivy->sp[-1] = ivy->sp[0];
                                --ivy->sp;
                        }
			break;
		} case ivy_iRTS: {	/* Return from function */
			ivy_clear_protected();
			ivy->pc = pc;
			if (!retfunc(ivy))
				return 0;
			if (ivy->call_me)
				return 1;
                        pc = ivy->pc;
			break;
		} case ivy_iSTASH: { /* Save top of stack off to the side */
			//printf("sp=%p sptop=%p\n", ivy->sp, ivy->sptop);
			ivy_pop(&ivy->stashed, ivy);
			//pr(stdout, &ivy->stashed, 0);
			break;
		} case ivy_iUNSTASH: {
			*ivy_push(ivy) = ivy->stashed;
			ivy_void_val(&ivy->stashed);
			break;
		} case ivy_iPOP: {	/* Pop something off of stack */
			ivy->sp = ivy_rmval(ivy->sp, __LINE__);
			break;
                } case ivy_iPSH_VOID: {
			ivy_push_void(ivy);
			break;
                } case ivy_iPSH_THIS: {
			ivy_obj_val(ivy_push(ivy), ivy->vars);
			break;
		} case ivy_iPSH_NUM: {
			pc += ivy_align_o(pc, sizeof(long long));
			ivy_push_int(ivy, *(long long *)pc);
			pc += sizeof(long long);
			break;
                } case ivy_iPSH_LST: {
			pc += ivy_align_o(pc, sizeof(int));
			ivy_lst_val(ivy_push(ivy), *(int *)pc);
			pc += sizeof(int);
			break;
                } case ivy_iPSH_FP: {
			pc += ivy_align_o(pc, sizeof(double));
			ivy_push_double(ivy, *(double *)pc);
			pc += sizeof(double);
			break;
                } case ivy_iPSH_STR: {
			int len;
			pc += ivy_align_o(pc, sizeof(int));
			len = *(int *)pc;
			pc += sizeof(int);
			char *ns = (char *)malloc(len + 1);
			memcpy(ns, pc, len + 1);
			Ivy_string *st = ivy_alloc_str(ns, len);
			ivy_string_val(ivy_push(ivy), st);
			pc += len + 1;
			break;
                } case ivy_iPSH_NAM: {
                        char *s;
			pc += ivy_align_o(pc, sizeof(char *));
			s = *(char **)pc;
			pc += sizeof(char *);
			ivy_symbol_val(ivy_push(ivy), s);
			break;
		} case ivy_iPSH_FUNC: { /* A function without context: record context now */
			pc += ivy_align_o(pc, sizeof(void *));
			// printf("iPSH_FUNC: ivy->vars=%p\n", ivy->vars);
			ivy_closure_val(ivy_push(ivy), *(Ivy_func **)pc, ivy->vars);
			pc += sizeof(void *);
			break;
                } case ivy_iPSH_PAIR: {
			ivy_val(ivy_push(ivy), ivy_tPAIR);
			break;
		} case ivy_iFOREACH: { /* List iteration */
                        Ivy_val newv;
                        Ivy_obj *o = ivy->sp[-2].u.obj;
                        long long which = ivy->sp[-1].u.num;
                        long long n = ivy->sp[0].u.num;
                        pc += ivy_align_o(pc, sizeof(int));
                        if (which == 2)
                        	goto next_string_entry;
			else if (which == 1)
				goto next_symbol_entry;
                        else {
                        	// Next array entry
	                        ++n;
	                        if (n == o->ary_len) {
	                        	which = 1;
	                        	n = -1;
	                        	next_symbol_entry:
	                        	/* Find first named entry */
	                        	while (++n != (o->nam_tab_mask + 1)) {
	                        		if (o->nam_tab[n].name)
	                        			break;
					}
					if (n == (o->nam_tab_mask + 1)) {
						which = 2;
						n = -1;
						next_string_entry:
						/* Find first string entry */
						while (++n != (o->str_tab_mask + 1)) {
							if (o->str_tab[n].name)
								break;
						}
						if (n == (o->str_tab_mask + 1)) {
							pc += sizeof(int);
							goto bye;
						}
					}
	                        }
                        }

                        // Foreach
                        if (which == 2)
                        	ivy_dup_val(&newv, &o->str_tab[n].val);
			else if (which == 1)
				ivy_dup_val(&newv, &o->nam_tab[n].val);
                        else
	                        ivy_dup_val(&newv, &o->ary[n]);
                        // FIXME add checking here: is it really a variable?
                        doSET(ivy, &ivy->sp[-3], &newv);
                        ivy->sp[0].u.num = n;
                        ivy->sp[-1].u.num = which;
                        pc += *(int *)pc;
                        bye:;
			break;
		} case ivy_iFORINDEX: { /* List iteration */
                        Ivy_val newv;
                        Ivy_obj *o = ivy->sp[-2].u.obj;
                        long long which = ivy->sp[-1].u.num;
                        long long n = ivy->sp[0].u.num;
                        pc += ivy_align_o(pc, sizeof(int));

                        if (which == 2)
                        	goto next_string_entry_1;
			else if (which == 1)
				goto next_symbol_entry_1;
                        else {
                        	// Next array entry
	                        ++n;
	                        if (n == o->ary_len) {
	                        	which = 1;
	                        	n = -1;
	                        	next_symbol_entry_1:
	                        	/* Find first named entry */
	                        	while (++n != (o->nam_tab_mask + 1)) {
	                        		if (o->nam_tab[n].name)
	                        			break;
					}
					if (n == (o->nam_tab_mask + 1)) {
						which = 2;
						n = -1;
						next_string_entry_1:
						/* Find first string entry */
						while (++n != (o->str_tab_mask + 1)) {
							if (o->str_tab[n].name)
								break;
						}
						if (n == (o->str_tab_mask + 1)) {
							pc += sizeof(int);
							goto bye1;
						}
					}
	                        }
                        }
                        // Foreach
                        if (which == 2) {
				ivy_string_val(&newv, ivy_alloc_str(o->str_tab[n].name, strlen(o->str_tab[n].name)));
			} else if (which == 1) {
				// Hmm...
				ivy_symbol_val(&newv, o->nam_tab[n].name);
			} else {
				ivy_int_val(&newv, n);
			}
                        // FIXME add checking here: is it really a variable?
                        doSET(ivy, &ivy->sp[-3], &newv);
                        ivy->sp[0].u.num = n;
                        ivy->sp[-1].u.num = which;
                        pc += *(int *)pc;
                        bye1:;
			break;
		} case ivy_iFIX: { /* Convert stack list into an array */
			Ivy_val newv;
                        ivy_pop(&newv, ivy);
                        *++ivy->sp = newv;
			break;
		} default: {
                        ivy_error_0(ivy->errprn, "Unknown instruction?");
                        longjmp(ivy->err, 1);
		}
        } }
}

/* Unwind stack */

static void popall(Ivy *ivy)
{
        while (ivy->sp != ivy->sptop) {
                switch(ivy->sp->type) {
                        case ivy_tNUM: {
                                fprintf(ivy->out, "%d:	Integer = %lld\n", (int)(ivy->sp - ivy->sptop), ivy->sp->u.num);
                                ivy->sp = ivy_rmval(ivy->sp, __LINE__);
                                break;
                        } case ivy_tSTR: {
                                fprintf(ivy->out, "%d:	String = \"%s\"\n", (int)(ivy->sp - ivy->sptop), ivy->sp->u.str->s);
                                ivy->sp = ivy_rmval(ivy->sp, __LINE__);
                                break;
                        } case ivy_tNAM: {
                                fprintf(ivy->out, "%d:	Symbol = %s\n", (int)(ivy->sp - ivy->sptop), ivy->sp->u.name);
                                ivy->sp = ivy_rmval(ivy->sp, __LINE__);
                                break;
                        } case ivy_tOBJ: {
                                fprintf(ivy->out, "%d:	Object = %p\n", (int)(ivy->sp - ivy->sptop), ivy->sp->u.obj);
                                ivy->sp = ivy_rmval(ivy->sp, __LINE__);
                                break;
                        } case ivy_tCLOSURE: {
                                fprintf(ivy->out, "%d:	CLOSURE\n", (int)(ivy->sp - ivy->sptop));
                                --ivy->sp;
                                break;
                        } case ivy_tLST: {
                                fprintf(ivy->out, "%d:	LST = %lld\n", (int)(ivy->sp - ivy->sptop), ivy->sp->u.num);
                                --ivy->sp;
                                break;
                        } case ivy_tPAIR: {
                                fprintf(ivy->out, "%d:	Pair\n", (int)(ivy->sp - ivy->sptop));
                                ivy->sp = ivy_rmval(ivy->sp, __LINE__);
                                break;
                        } case ivy_tVOID: {
                                fprintf(ivy->out, "%d:	Void\n", (int)(ivy->sp - ivy->sptop));
                                ivy->sp = ivy_rmval(ivy->sp, __LINE__);
                                break;
                        } case ivy_tFP: {
                                fprintf(ivy->out, "%d:	Float = %lg\n", (int)(ivy->sp - ivy->sptop), ivy->sp->u.fp);
                                ivy->sp = ivy_rmval(ivy->sp, __LINE__);
                                break;
                        } case ivy_tRET_IVY: {
                                fprintf(ivy->out, "%d:	RET_IVY (pc = %p, callstate = %p)\n", (int)(ivy->sp - ivy->sptop), ivy->sp->u.pc, ivy->sp->idx.callstate);
                                ivy->vars = ivy->sp->idx.callstate->ovars;
                                free(ivy->sp->idx.callstate);
                                --ivy->sp;
                                break;
                        } case ivy_tRET_IVY_THUNK: {
                                fprintf(ivy->out, "%d:	RET_IVY_THUNK (pc = %p, callstate = %p)\n", (int)(ivy->sp - ivy->sptop), ivy->sp->u.pc, ivy->sp->idx.callstate);
                                ivy->vars = ivy->sp->idx.callstate->ovars;
                                free(ivy->sp->idx.callstate);
                                --ivy->sp;
                                break;
                        } case ivy_tRET_SIMPLE: {
                                fprintf(ivy->out, "%d:	RET_SIMPLE (pc = %p, callstate = %p", (int)(ivy->sp - ivy->sptop), ivy->sp->u.pc, ivy->sp->idx.callstate);
                                --ivy->sp;
                                fprintf(ivy->out, "func = %p, obj = %p)\n", ivy->sp->idx.func, ivy->sp->u.obj);
                                ivy->vars = ivy->sp->u.obj;
                                --ivy->sp;
                                break;
                        } case ivy_tRET_SIMPLE_THUNK: {
                                fprintf(ivy->out, "%d:	RET_SIMPLE_THUNK (pc = %p, callstate = %p", (int)(ivy->sp - ivy->sptop), ivy->sp->u.pc, ivy->sp->idx.callstate);
                                --ivy->sp;
                                fprintf(ivy->out, "func = %p, obj = %p)\n", ivy->sp->idx.func, ivy->sp->u.obj);
                                ivy->vars = ivy->sp->u.obj;
                                --ivy->sp;
                                break;
                        } case ivy_tRET_NEXT_INIT: {
                                fprintf(ivy->out, "%d:	RET_NEXT_INIT (pc = %p, callstate = %p)\n", (int)(ivy->sp - ivy->sptop), ivy->sp->u.pc, ivy->sp->idx.callstate);
                                --ivy->sp;
                                break;
			} case ivy_tSCOPE: {
                                fprintf(ivy->out, "%d:	SCOPE (obj = %p)\n", (int)(ivy->sp - ivy->sptop), ivy->sp->u.obj);
				ivy->vars = ivy->sp->u.obj;
				--ivy->sp;
				break;
                        } default: {
                                fprintf(ivy->out, "%d:	Unknown type = %d\n", (int)(ivy->sp - ivy->sptop), ivy->sp->type);
                                --ivy->sp;
                                break;
                        }
                }
        }
}

/* Print a value */

void ivy_pr_func(FILE *out, Ivy_func *f)
{
	int x;
	if (f->code)
		fprintf(out, "{ %p pseudo = %p", f, f->code);
	else
		fprintf(out, "{ %p c-func = %p", f, f->cfunc);
	if (f->argv) {
		if (f->argv_quote)
			fprintf(out, " argv=&%s", f->argv);
		else
			fprintf(out, " argv=%s", f->argv);
	}
	if (f->thunk)
		fprintf(out, " thunk");
	fprintf(out, " (");
	for (x = 0; x != f->nargs; ++x) {
		if (f->quote[x])
			fprintf(out, " &%s", f->args[x]);
		else
			fprintf(out, " %s", f->args[x]);
		if (f->inits[x])
			fprintf(out, "=%p", f->inits[x]);
	}
	fprintf(out, " ) }");
}

Ivy_val *ivy_pr(Ivy *ivy, FILE *out, Ivy_val * v, int lvl)
{
	switch (v->type) {
	        case ivy_tNUM: {
        		fprintf(out, "%lld", v->u.num);
        		return v + 1;
		} case ivy_tNAM: {
			fprintf(out, "`%s", v->u.name);
			return v + 1;
                } case ivy_tFP: {
        		fprintf(out, "%g", v->u.fp);
        		return v + 1;
                } case ivy_tSTR: {
        		fprintf(out, "\"%s\"", v->u.str->s);
        		return v + 1;
                } case ivy_tVOID: {
        		fprintf(out, "void");
        		return v + 1;
                } case ivy_tCLOSURE: {
        		Ivy_val w;
        		fprintf(out, "Ivy_closure func=");
        		ivy_pr_func(out, v->u.closure.func);
        		fprintf(out, " env=");
        		w.type = ivy_tOBJ;
        		w.u.obj = v->u.closure.env;
        		ivy_pr(ivy, out, &w,lvl+4);
        		return v + 1;
                } case ivy_tOBJ: {
			int x;
			if (v->u.obj->visit)
				fprintf(out, "[ %d at 0x%p (previously shown) ]", v->u.obj->objno, v->u.obj);
			else if (v->u.obj == ivy_globals && lvl != 0)
				fprintf(out, "[ %d at 0x%p (globals) ]", v->u.obj->objno, v->u.obj);
			else {
				v->u.obj->visit = 1;
				fprintf(out, "[ %d at 0x%p\n", v->u.obj->objno, v->u.obj);
				for (x = 0; x != (v->u.obj->nam_tab_mask + 1); ++x)
					if (v->u.obj->nam_tab[x].name) {
						/* if (e->var->val.type != tFUN || !e->var->val.u.fun->f->cfunc) */ {
							/* if (first)
								fprintf(out, " ");
							else
								first = 1; */
							ivy_indent(out, lvl+4);
							fprintf(out, "`%s=", v->u.obj->nam_tab[x].name);
							ivy_pr(ivy, out, &v->u.obj->nam_tab[x].val, lvl+4);
							fprintf(out, "\n");
						}
					}
				for (x = 0; x != (v->u.obj->str_tab_mask + 1); ++x)
					if (v->u.obj->str_tab[x].name) {
						/* if (e->var->val.type != tFUN || !e->var->val.u.fun->f->cfunc) */ {
							/* if (first)
								fprintf(out, " ");
							else
								first = 1; */
							ivy_indent(out, lvl+4);
							fprintf(out, "`\"%s\"=", v->u.obj->str_tab[x].name);
							ivy_pr(ivy, out, &v->u.obj->str_tab[x].val, lvl+4);
							fprintf(out, "\n");
						}
					}
				for (x = 0; x != v->u.obj->ary_len; ++x) {
/*
					if (first)
						fprintf(out, " ");
					else
						first = 1; */
					//if (v->u.obj->ary[x])
						ivy_indent(out, lvl+4), fprintf(out, "`%d=", x),
						    ivy_pr(ivy, out, &v->u.obj->ary[x], lvl+4), fprintf(out, "\n");
				}
				ivy_indent(out, lvl);
				fprintf(out, "]");
				v->u.obj->visit = 0;
			}
			return v + 1;
		} default: {
	        	fprintf(out, "unknown type %d",v->type);
	        	return v+1;
        	}
	}
}

/* Add a C function to the table */

void ivy_add_cfunc(Ivy *ivy, Ivy_obj *vars, const char *name, const char *argstr, void (*cfunc) (Ivy *))
{
	Ivy_node *args;
	Ivy_func *o;
	int argc;
	char **argv;
	char *quote;
	int ellipsis = 0;
	Ivy_pseudo **initv;
	args = ivy_compargs(ivy, argstr);
	argc = ivy_cntlst(args);
	quote = (char *) calloc(argc, 1);
	argv = (char **) malloc(argc * sizeof(char *));
	initv = (Ivy_pseudo **) malloc(argc * sizeof(Ivy_pseudo *));
	ivy_genlst(ivy->errprn, argv, initv, quote, args, &ellipsis);
	if (!ellipsis)
		o = ivy_create_func(NULL, argc, argv, initv, quote, 0, 0, 0, 0);
	else
		o = ivy_create_func(NULL, argc - 1, argv, initv, quote, 0, argv[argc-1], initv[argc-1], quote[argc-1]);
	o->cfunc = cfunc;
	/* Put new function in table */
	ivy_closure_val(ivy_set_by_symbol(vars, ivy_symbol_add(name)), o, vars);
}

/* Initialize global variables and symbols*/

static Ivy_obj *ivy_alloc_globals(Ivy *ivy)
{
	Ivy_obj *o;
	int x;

	ivy_a_symbol = ivy_symbol_add("a");
	ivy_b_symbol = ivy_symbol_add("b");
	ivy_mom_symbol = ivy_symbol_add("mom");
	ivy_dynamic_symbol = ivy_symbol_add("dynamic");
	ivy_argv_symbol = ivy_symbol_add("argv");

	o = ivy_alloc_obj(128, 4, 4);

	for (x = 0; ivy_builtins[x].name; ++x)
		ivy_add_cfunc(ivy, o, ivy_builtins[x].name, ivy_builtins[x].args, ivy_builtins[x].cfunc);
	return o;
}

Ivy ivy_list[1] = { { ivy_list, ivy_list } };

/* Initialize an interpreter */

void ivy_setup(Ivy *ivy, void (*err_print)(void *obj, char *), void *err_obj, FILE *in, FILE *out)
{
	ivy->next = ivy_list->next;
	ivy->prev = ivy_list;
	ivy->next->prev = ivy;
	ivy->prev->next = ivy;

        ivy->errprn->error_flag = 0;
        ivy->errprn->error_obj = err_obj;
        ivy->errprn->error_print = err_print;
        int spsize = 1024;
	ivy->sptop = ivy->sp = (Ivy_val *) malloc(sizeof(Ivy_val) * (spsize));
	ivy->spend = ivy->sptop + spsize;
	ivy->vars = ivy->glblvars = 0;
	ivy->pc = 0;
	ivy->call_me = 0;
	ivy->call_me_obj = 0;
	ivy->out = out;
	ivy->in = in;
	ivy_void_val(&ivy->stashed);

	if (!ivy_globals) {
		ivy_globals = ivy_alloc_globals(ivy);
	}

        ivy->vars = ivy->glblvars = ivy_globals;

	ivy_scope_push(ivy, ivy->vars);
	ivy->glblvars = ivy->vars;
}

void ivy_shutdown(Ivy *ivy)
{
	ivy->next->prev = ivy->prev;
	ivy->prev->next = ivy->next;

	while (ivy->vars != ivy->glblvars)
		ivy_scope_pop(ivy);

	popall(ivy);
	free(ivy->sptop);
}

/* Run some compiled code */

Ivy_val ivy_run(Ivy *ivy, Ivy_pseudo *code, int ptop, int trace)
{
	Ivy_val rtn;
	ivy_void_val(&rtn);
	if (!setjmp(ivy->err)) {
	        ivy->pc = code;
		while (pexe(ivy, trace)) {
			void (*call_me)(Ivy *);
			call_me = ivy->call_me;
			ivy->call_me = 0; // Clear it before we call it.
			// If we clear after call_me, then it would still be set if call_me longjmps, which screws things
			// up for next time.
			call_me(ivy);
			retfunc(ivy);
		}
		ivy_pop(&rtn, ivy);
		if (ptop) {
       			ivy_pr(ivy, ivy->out, &rtn,0);
       			fprintf(ivy->out, "\n");
		}
		if (ivy->sp != ivy->sptop) {
			ivy_error_0(ivy->errprn, "Stack not empty?");
			longjmp(ivy->err, 1);
		}
	} else {
		fprintf(ivy->out, "\nError stop\n");
		fprintf(ivy->out, "Stack:\n");
		popall(ivy);
		if (ivy->vars != ivy->glblvars) {
			ivy_error_0(ivy->errprn, "Scope incorrect after stack unwound\n");
		}
        }
	return rtn;
}
