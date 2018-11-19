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

/** Stack grow function **/

Val *psh(Ivy *ivy)
{
	int ss;
	if ((ss = ++ivy->sp - ivy->sptop) == ivy->spsize) {
		ivy->sptop = (Val *) realloc(ivy->sptop, sizeof(Val) * (ivy->spsize += 1024));
		ivy->sp = ivy->sptop + ss;
	}
	return ivy->sp;
}

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

  call callfunc()... this sets up the call
    an argument should give the continuation C call (and object).

  return to top.  top should call pexe which will eventually return to top with
  call_me set to continuation C call.

  A c function doesnt have scope: instead it has passed object pointer.

  The continuation function should pop the one argument off the stack.
*/

void mkclosure_init_next(Ivy *ivy, Closure *closure);

void mkclosure_init(Ivy *ivy, Closure *closure)
{
	while (closure->x != closure->f->nargs) {
		/* Call initializer */
		if (closure->f->inits[closure->x]) {
			mkval(psh(ivy), tRET_NEXT_INIT);
			ivy->sp[0].idx.closure = closure;
			ivy->sp[0].u.pc = ivy->pc;

			ivy->pc = closure->f->inits[closure->x];
			return;
		} else
			mkval(&closure->init_vals[closure->x++], tVOID);
	}
}

void mkclosure_init_next(Ivy *ivy, Closure *closure)
{
	closure->init_vals[closure->x++] = popval(ivy);
	mkclosure_init(ivy, closure);
}

/* Delete a value: returns with pointer to after value deleted */

Val *rmval(Val *v, int line)
{
	switch (v->type) {
		case tLST: {
			long long y = v->u.num, x;
			--v;
			// printf("rmval list with %d\n", y);
			for (x = 0; x != y; ++x)
				v = rmval(v, __LINE__);
			break;
		} case tPAIR: {
			--v;
			v = rmval(v, __LINE__);
			v = rmval(v, __LINE__);
			break;
		} default: {
			--v;
		}
	}
	return v;
}

/* Duplicate a value: returns with pointer before value duplicated */

Val *dupval(Val *n, Val *v)
{
	n->idx_type = v->idx_type;
	n->origin = v->origin;
	n->idx = v->idx;

	switch (v->type) {
		case tLST: {
			long long y = v->u.num, x, z;
			Obj *obj;
			*n = mkpval(tOBJ, obj = alloc_obj(16, 4, 4));
			--v;
			for (x = 0, z = 0; x != y; ++x)
				if (v->type == tPAIR) {
					--v;
					if (v->type == tNAM)
						v = dupval(set_by_symbol(obj, v[0].u.name), v - 1);
					else if (v->type == tSTR)
						v = dupval(set_by_string(obj, v[0].u.str->s), v - 1);
					else if (v->type == tNUM)
						v = dupval(set_by_number(obj, v[0].u.num), v - 1);
					else {
						fprintf(stderr, "Invalid index on stack after tPAIR\n");
						exit(-1);
					}
				} else
					v = dupval(set_by_number(obj, z++), v);
			return v;
		} case tSTR: {
		        n->type = v->type;
		        n->u.str = v->u.str;
			return v - 1;
		} case tNAM: {
	                n->type = v->type;
	                n->u.name = v->u.name;
	                return v - 1;
		} case tOBJ: {
		        n->type = v->type;
		        n->u.obj = v->u.obj;
			return v - 1;
		} case tCLOSURE: {
		        n->type = v->type;
		        n->u.closure = v->u.closure;
			return v - 1;
		} case tVOID: {
		        n->type = v->type;
			return v - 1;
		} case tNUM: {
		        n->type = v->type;
		        n->u.num = v->u.num;
			return v - 1;
		} case tFP: {
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

/* Pop a value off of the stack and return it */

Val popval(Ivy *ivy)
{
	Val v;
	dupval(&v, ivy->sp);
	ivy->sp = rmval(ivy->sp, __LINE__);
	return v;
}

/* Make a value */

void mkval(Val *v, int type)
{
	v->type = type;
	v->origin = 0;
	v->idx_type = tVOID;
	v->idx.num = 0;
	v->u.num = 0;
}

Val mkival(int type, long long i)
{
	Val v;
	v.type = type;
	v.origin = 0;
	v.idx_type = tVOID;
	v.idx.num = 0;
	v.u.num = i;
	return v;
}

Val mkdval(int type, double d)
{
	Val v;
	v.type = type;
	v.origin = 0;
	v.idx_type = tVOID;
	v.idx.num = 0;
	v.u.fp = d;
	return v;
}

Val mkpval(int type, void *u)
{
	Val v;
	v.type = type;
	v.origin = 0;
	v.idx_type = tVOID;
	v.idx.num = 0;
	v.u.closure = u;
	return v;
}

/* Add a new level of local variables */

void addlvl(Ivy *ivy, Obj *dyn)
{
	Obj *o;

	o = alloc_obj(16, 4, 4);

	*set_by_symbol(o, mom_symbol) = mkpval(tOBJ, ivy->vars);

	*set_by_symbol(o, dynamic_symbol) = mkpval(tOBJ, dyn);

	ivy->vars = o;
	SCOPE_PRINTF1("addlvl, new scope is %p\n", ivy->vars);
}

void rmvlvl(Ivy *ivy)
{
	SCOPE_PRINTF1("rmvlvl, pop amount = %d\n", amnt + 1);
	Obj *o = get_mom(ivy->vars);
	SCOPE_PRINTF2("rmvlvl, pop scope %p restored to %p\n", ivy->vars, o);
	ivy->vars = o;
}

/* Remove a level of local variables */

Obj *get_mom(Obj *o)
{
	Val *u = get_by_symbol(o, mom_symbol);
	Obj *rtn = 0;
	if (u && u->type == tOBJ)
		rtn = u->u.obj;
	return rtn;
}

/* Lookup a variable.  Check all scoping levels for the variable */

Val *getv_by_symbol_obj(Obj *o, char *name)
{
	Obj *next;
	Val *e;
	do {
		if ((e = get_by_symbol(o, name)))
			return e;
		next = get_mom(o);
	} while ((o = next));
	return 0;
}

Val *getv_by_symbol(Ivy *ivy, char *name)
{
	return getv_by_symbol_obj(ivy->vars, name);
}

/* Lookup a variable.  Check all scoping levels for the variable.
   If none found, create it in the inner-most level. */

Val *setv_by_symbol(Obj *o, char *name)
{
	Val *v = getv_by_symbol_obj(o, name);
	if (!v) {
		v = set_by_symbol(o, name);
	}
	return v;
}

/* Call a function closure with no args */

void call_simple_func(Ivy *ivy, Closure * o, void (*func)(Ivy *,struct callfunc *), struct callfunc *t)
{
	Obj *ovars = ivy->vars;	/* Save caller's scope */

	// printf("call_simple_func %p\n", o);

	SCOPE_PRINTF2("call_simple_fnuc Switch to closure's scope %p (prev was %p)\n", o->scope, ivy->vars);
	protect_obj(ivy->vars);
	ivy->vars = o->scope;	/* Switch to closure's scope */

	addlvl(ivy,ovars);	/* Make scoping level for function */

	mkval(psh(ivy), tVOID);
	ivy->sp[0].idx.func = func;
	ivy->sp[0].u.obj = ovars;

	mkval(psh(ivy), tRET_SIMPLE);
	ivy->sp[0].idx.callfunc = t;
	ivy->sp[0].u.pc = ivy->pc;

	/* Set new program counter value... */
	ivy->pc = o->f->code;
}

void copy_next_arg(Ivy *ivy, struct callfunc *t);

void save_arg_result(Ivy *ivy,struct callfunc *t)
{
	Val v = popval(ivy);

	if (t->scope_result) {
		*t->scope_result = v;
	}

	if (t->argv_result) {
		*t->argv_result = v;
		
	}

	copy_next_arg(ivy, t);
}

void copy_next_arg(Ivy *ivy, struct callfunc *t)
{
        int x;
        Val *a;
	while (t->x != ivy->sp[0].u.num) {
		Closure *f = 0; // Set to function to call if not quoting
		if (t->q->type != tPAIR) { /* It's not a named argument */
			if (t->argn >= t->o->f->nargs) { /* Past end of declared arg list */
				t->argv_result = set_by_number(t->argv, t->argn); // Create slot for argument
				t->scope_result = 0;
				t->q = dupval(t->argv_result, t->q); // Copy quoted argument from stack
				/* Quote extra args if last formal arg was quoted */
				if (!t->o->f->nargs || !t->o->f->quote[t->o->f->nargs - 1])
					f = t->argv_result->u.closure;
				++t->argn;
			} else { /* Unnamed arg */
				t->scope_result = set_by_symbol(ivy->vars, t->o->f->args[t->argn]);
				t->q = dupval(t->scope_result, t->q);

				t->argv_result = set_by_number(t->argv, t->argn);

				*t->argv_result = *t->scope_result; // Also save it in argv

				if (!t->o->f->quote[t->argn])
				        f = t->argv_result->u.closure;
				++t->argn;
			}
		} else { /* Named arg */
			int z;
			--t->q; /* Argument name */
			t->scope_result = set_by_symbol(ivy->vars, t->q->u.name);
			t->argv_result = 0;

			/* Find where it should go in argv */
			for (z = 0; z != t->o->f->nargs; ++z)
				if (!strcmp(t->o->f->args[z], t->q->u.name)) {
					t->argv_result = set_by_number(t->argv, z);
					break;
				}

			--t->q; /* Skip over argument name, point to value */
			t->q = dupval(t->scope_result, t->q); /* Copy from stack to slot */
			if (t->argv_result)
				*t->argv_result = *t->scope_result; /* Save in argv also */
			
			if (z == t->o->f->nargs || !t->o->f->quote[z])
				f = t->scope_result->u.closure;
		}
		++t->x;
		if (f) {
			call_simple_func(ivy, f, save_arg_result, t);
			return;
		}
	}
	/* All arguments done */
        /* Remove argument list */
        ivy->sp = rmval(ivy->sp, __LINE__);

        /* Push return address */
        mkval(psh(ivy), tRET_IVY);
//        ivy->sp[0].u.obj = t->ovars;
        ivy->sp[0].idx.callfunc = t;
        ivy->sp[0].u.pc = ivy->pc;

        /* Set new program counter value... */
        if (t->o->f->code)
                ivy->pc = t->o->f->code;
        else
                ivy->call_me = t->o->f->cfunc;
        /* Initializers... */
        for (x = t->o->f->nargs - 1; x >= 0; --x) {
                /* Did caller set this arg? */
                a = get_by_symbol(ivy->vars, t->o->f->args[x]);
                if (!a) { /* Nope: copy initializer */
                        a = set_by_symbol(ivy->vars, t->o->f->args[x]);
                        /* Copy initializer */
                        dupval(a, &t->o->init_vals[x]);
                        /* Put missing variable in arg vector */
                        *set_by_number(t->argv, x) = *a;
                }
        }
        /* Ok, function is now ready to be called */
// Do it on return side
//        free(t);
}

void callfunc(Ivy *ivy, Closure *o)
{
	struct callfunc *t;

	t = calloc(1, sizeof(struct callfunc));
	t->o = o;
	// printf("callfunc %p fun=%p\n", t, o);
	mkval(&t->val, tVOID);
	t->ovars = ivy->vars;	/* Save caller's scope */

	SCOPE_PRINTF2("callfunc, switched to closure's scope %p (prev was %p)\n", o->scope, ivy->vars);
	protect_obj(ivy->vars);
	ivy->vars = o->scope;	/* Switch to closure's scope */

	addlvl(ivy, t->ovars);	/* Make scoping level for function */

	*set_by_symbol(ivy->vars, argv_symbol) = mkpval(tOBJ, t->argv = alloc_obj(16, 4, 4));

	t->x = 0; /* Count of args we've completed */
	t->argn = 0; /* Next arg number to use for unnamed */
	t->q = ivy->sp - 1; /* Arg we're working on now */

	copy_next_arg(ivy, t);
}

/* Functions for "calling" strings and objects */

void copy_next_str_arg(Ivy *ivy, struct callfunc *t);

void save_str_arg_result(Ivy *ivy,struct callfunc *t)
{
	/* Save result of arg evaluation */
	Val v = popval(ivy);

	if (t->scope_result) {
		*t->scope_result = v;
	}

	if (t->argv_result) {
		*t->argv_result = v;
		
	}
	copy_next_str_arg(ivy, t);
}

void copy_next_str_arg(Ivy *ivy, struct callfunc *t)
{
	while (t->x != ivy->sp[0].u.num) {
		Closure *f = 0;
		if (t->q->type != tPAIR) { /* Unnamed arg */
                        t->argv_result = set_by_number(t->argv, t->argn); /* Put in argv */
                        t->scope_result = 0;
                        t->q = dupval(t->argv_result, t->q);
                        f = t->argv_result->u.closure;
                        ++t->argn;
		} else { /* Named arg */
			--t->q;
			t->scope_result = set_by_symbol(ivy->vars, t->q->u.name); /* Put in scope */
			t->argv_result = 0;
			--t->q; /* Skip arg name, get to value */
			t->q = dupval(t->scope_result, t->q);
			f = t->scope_result->u.closure;
		}
		++t->x;
		if (f) {
			call_simple_func(ivy, f, save_str_arg_result, t);
			return;
		}
	}
	/* All arguments done */
        /* Remove argument list */
        ivy->sp = rmval(ivy->sp, __LINE__);

        /* We now have the string or object arguments in argv */
        if (t->val.type == tSTR) { /* Access a string  */
                Str *str = t->val.u.str;
                Val *first_index = get_by_number(t->argv, 0);
                Val *second_index = get_by_number(t->argv, 1);
                if (!first_index) { /* No args, just return the string */
                        SCOPE_PRINTF("copy_next_str_arg (str0):\n");
                        rmvlvl(ivy);
                        *psh(ivy) = t->val;
                } else if (first_index && !second_index) { /* Get single char from string */
                        if (first_index->type == tNUM) {
                                long long a = first_index->u.num;
                                if (a < 0)
                                        a = str->len + a;
                                if (a >= str->len || a < 0) {
                                        error_0(ivy->errprn, "Index is past end or beginning of string..");
                                        longjmp(ivy->err, 1);
                                } else { /* OK, finally we can index the string */
                                        int chr = str->s[a];
                                        SCOPE_PRINTF("copy_next_str_arg (str):\n");
                                        rmvlvl(ivy);
                                        *psh(ivy) = mkival(tNUM, chr);
                                }
                        } else {
                                error_0(ivy->errprn, "Non numeric index to string...");
                                longjmp(ivy->err, 1);
                        }
                } else if (second_index) { /* Get substring from string */
                        if (first_index->type == tNUM) {
                                long long a = first_index->u.num;
                                if (second_index->type == tNUM) { /* OK, finally we can access the string */
                                        long long b = second_index->u.num;
                                        char *s;
                                        Val v;
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
                                        rmvlvl(ivy);
                                        v = mkpval(tSTR, alloc_str(s, b-a));
                                        *psh(ivy) = v;
                                } else {
                                        error_0(ivy->errprn, "Non numeric second index to string...");
                                        longjmp(ivy->err, 1);
                                }
                        } else {
                                error_0(ivy->errprn, "Non numeric first index to string...");
                                longjmp(ivy->err, 1);
                        }
                } else {
                        error_0(ivy->errprn, "Proper string index is missing...");
                        longjmp(ivy->err, 1);
                }
        } else { /* Access an object */
                Obj *obj = t->val.u.obj;
                Val *index = get_by_number(t->argv, 0);
                if (!index) { /* No args, just return the object */
                        SCOPE_PRINTF("copy_next_str_arg (str0):\n");
                        rmvlvl(ivy);
                        *psh(ivy) = t->val;
                } else {
                        if (index->type == tNUM) {
                                long long a = index->u.num;
                                Val *o = get_by_number(obj, a);
                                SCOPE_PRINTF("copy_next_str_arg (obj):\n");
                                rmvlvl(ivy);

                                if (o)
	                                *psh(ivy) = *o;
				else
					mkval(psh(ivy), tVOID);

                                ivy->sp[0].origin = obj;
                                ivy->sp[0].idx_type = tNUM;
                                ivy->sp[0].idx.num = a;

                        } else if (index->type == tSTR) {
                                Str *str = index->u.str;
                                Val *o = get_by_string(obj, str->s);
                                SCOPE_PRINTF("copy_next_str_arg (obj2):\n");
                                rmvlvl(ivy);

                                if (o)
                                	*psh(ivy) = *o;
				else
					mkval(psh(ivy), tVOID);

                                ivy->sp[0].origin = obj;
                                ivy->sp[0].idx_type = tSTR;
                                ivy->sp[0].idx.str = str;

                                /* If we just looked up a function, change scope to object it was found in */
                                if (ivy->sp[0].type == tCLOSURE) {
                                	ivy->sp[0].u.closure->scope = obj;
                                }
			} else if (index->type == tNAM) {
                                char *name = index->u.name;
                                Val *o = getv_by_symbol_obj(obj, name);
                                SCOPE_PRINTF("copy_next_str_arg (obj2):\n");
                                rmvlvl(ivy);

                                if (o)
                                	*psh(ivy) = *o;
				else
					mkval(psh(ivy), tVOID);

                                ivy->sp[0].origin = obj;
                                ivy->sp[0].idx_type = tNAM;
                                ivy->sp[0].idx.name = name;

                                /* If we just looked up a function, change scope to object it was found in */
                                if (ivy->sp[0].type == tCLOSURE) {
                                	ivy->sp[0].u.closure->scope = obj;
                                }
                        } else {
                                error_0(ivy->errprn, "Invalid object index type...");
                                longjmp(ivy->err, 1);
                        }
                }
        }

        /* All done */
        rmval(&t->val, __LINE__);
        free(t);
}

void callval(Ivy *ivy, Val val)
{
	struct callfunc *t;

	t = calloc(1, sizeof(struct callfunc));
	t->o = 0;
	t->val = val;
	t->ovars = ivy->vars;	/* Save caller's scope */

	SCOPE_PRINTF("call str:\n");
	addlvl(ivy, t->ovars);	/* Make scoping level for function */

	*set_by_symbol(ivy->vars, argv_symbol) = mkpval(tOBJ, t->argv = alloc_obj(16, 4, 4));

	t->x = 0; /* Count of args we've completed */
	t->argn = 0; /* Next arg number to use for unnamed */
	t->q = ivy->sp - 1; /* Arg we're working on now */

	copy_next_str_arg(ivy, t);
}

/* Return from a subroutine */

int retfunc(Ivy *ivy)
{
	/* This used to just return */
	Obj *restore_vars;
	Pseudo *restore_pc;
	Val rtn_val;

	/* Pop return value for a sec... */
	/* (we push it back on after we pop return address) */
	rtn_val = popval(ivy);

	if (ivy->sp == ivy->sptop) {
		/* Nothing to return to?  We must be done! */
		*psh(ivy) = rtn_val;
		return 0;
	}

	/* Return and call a continuation function */
	if (ivy->sp[0].type == tRET_NEXT_INIT) {
		Closure *closure;
		SCOPE_PRINTF("ret_next_init:\n");
		ivy->pc = ivy->sp[0].u.pc;
		closure = ivy->sp[0].idx.closure;

		ivy->sp[0] = rtn_val;

		mkclosure_init_next(ivy, closure);

		return 1;
	} else if (ivy->sp[0].type == tRET_SIMPLE) {
		void (*func)(Ivy *,struct callfunc *);
		struct callfunc *t;
		SCOPE_PRINTF("retfunc simple:\n");
		ivy->pc = ivy->sp[0].u.pc;
		t = ivy->sp[0].idx.callfunc;

		--ivy->sp;
		func = ivy->sp[0].idx.func;
		if (ivy->sp[0].u.obj) {
                        rmvlvl(ivy);
                        SCOPE_PRINTF2("retfunc RET_SIMPLE Restore scope to %p (was %p)\n", ivy->sp[0].u.obj, ivy->vars);
			ivy->vars = ivy->sp[0].u.obj;
                }

		ivy->sp[0] = rtn_val;

		func(ivy, t);

		return 1;
	}

	/* Get old program counter and scope */
	if (ivy->sp[0].type == tRET_IVY) { /* Return to Ivy code */
	        struct callfunc *t;
		SCOPE_PRINTF("retfunc ivy:\n");
                rmvlvl(ivy);
		restore_pc = ivy->sp[0].u.pc;
		t = ivy->sp[0].idx.callfunc;
                restore_vars = t->ovars;

		/* Put return value back on stack */
		ivy->sp[0] = rtn_val;

		/* Continue... */
		ivy->pc = restore_pc;
		SCOPE_PRINTF2("retfunc RET_IVY, Restore scope to %p (was %p)\n", restore_vars, ivy->vars);
		ivy->vars = restore_vars;

		free(t);
	} else {
		error_0(ivy->errprn, "Error: bad subroutine return point?");
		longjmp(ivy->err, 1);
	}
	return 1;
}

void doSET(Ivy *ivy, Val *dest, Val *src)
{
	if (!dest->origin) {
		/* Maybe a multi-assignment */
		if (dest->type == tOBJ && src->type == tOBJ) {
			Obj *to = dest->u.obj;
			Obj *from = src->u.obj;
			int x;
			for (x = 0; x != to->ary_len; ++x) {
				if (x == from->ary_len) {
					error_0(ivy->errprn, "Incorrect no. of args");
					longjmp(ivy->err, 1);
                                } else {
					doSET(ivy, &to->ary[x], &from->ary[x]);
                                }
                        }
		} else {
			error_0(ivy->errprn, "Improper L-value");
                        longjmp(ivy->err, 1);
		}
	} else {
		switch (dest->idx_type) {
			case tNUM: {
				dupval(set_by_number(dest->origin, dest->idx.num), src);
				break;
			} case tNAM: {
				dupval(set_by_symbol(dest->origin, dest->idx.name), src);
				break;
			} case tSTR: {
				dupval(set_by_string(dest->origin, dest->idx.str->s), src);
				break;
			} default: {
				error_0(ivy->errprn, "Unknown L-value type?");
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

void showstack(Ivy *ivy)
{
        Val *sp;
        for (sp = ivy->sp; sp != ivy->sptop;) {
                switch(sp->type) {
                        case tNUM: {
                                fprintf(ivy->out, "%d:	Integer = %lld\n", (int)(sp - ivy->sptop), sp->u.num);
                                sp--;
                                break;
                        } case tSTR: {
                                fprintf(ivy->out, "%d:	String = \"%s\"\n", (int)(sp - ivy->sptop), sp->u.str->s);
                                sp--;
                                break;
                        } case tNAM: {
                                fprintf(ivy->out, "%d:	Symbol = %s\n", (int)(sp - ivy->sptop), sp->u.name);
                                sp--;
                                break;
                        } case tOBJ: {
                                fprintf(ivy->out, "%d:	Object = %p\n", (int)(sp - ivy->sptop), sp->u.obj);
                                --sp;
                                break;
                        } case tCLOSURE: {
                                fprintf(ivy->out, "%d:	CLOSURE\n", (int)(sp - ivy->sptop));
                                --sp;
                                break;
			} case tPAIR: {
                                fprintf(ivy->out, "%d:	Pair\n", (int)(sp - ivy->sptop));
                                --sp;
				break;
                        } case tLST: {
                                fprintf(ivy->out, "%d:	LST = %lld\n", (int)(sp - ivy->sptop), sp->u.num);
                                --sp;
                                break;
                        } case tVOID: {
                                fprintf(ivy->out, "%d:	Void\n", (int)(sp - ivy->sptop));
                                --sp;
                                break;
                        } case tFP: {
                                fprintf(ivy->out, "%d:	Float = %lg\n", (int)(sp - ivy->sptop), sp->u.fp);
                                --sp;
                                break;
                        } case tRET_IVY: {
                                fprintf(ivy->out, "%d:	RET_IVY (pc = %p, callfunc = %p)\n", (int)(sp - ivy->sptop), sp->u.pc, sp->idx.callfunc);
                                --sp;
                                break;
                        } case tRET_SIMPLE: {
                                fprintf(ivy->out, "%d:	RET_SIMPLE (pc = %p, callfunc = %p", (int)(sp - ivy->sptop), sp->u.pc, sp->idx.callfunc);
                                --sp;
                                fprintf(ivy->out, "func = %p, obj = %p)\n", sp->idx.func, sp->u.obj);
                                --sp;
                                break;
                        } case tRET_NEXT_INIT: {
                                fprintf(ivy->out, "%d:	RET_NEXT_INIT (pc = %p, closure = %p)\n", (int)(sp - ivy->sptop), sp->u.pc, sp->idx.closure);
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

int pexe(Ivy *ivy, int trace)
{
	Pseudo *pc = ivy->pc;

	for (;;) { if (trace) fprintf(ivy->out,"-----\n"), showstack(ivy), disasm(ivy->out, pc, 0, 1); clear_protected(); switch (*pc++) {
                case iBRA: {	/* Branch unconditionally */
                        pc += align_o(pc, sizeof(int));
                        pc += *(int *)pc;
                        break;
                } case iBEQ: {	/* Branch if zero or void */
                        pc += align_o(pc, sizeof(int));
                        if (ivy->sp->type == tNUM)
                                if (ivy->sp->u.num == 0) {
                                        ivy->sp = rmval(ivy->sp, __LINE__);
                                        pc += *(int *)pc;
                                } else {
                                        ivy->sp = rmval(ivy->sp, __LINE__);
                                        pc += sizeof(int);
                                }
                        else if (ivy->sp->type == tVOID) {
                                ivy->sp = rmval(ivy->sp, __LINE__);
                                pc += *(int *)pc;
                        } else {
                                ivy->sp = rmval(ivy->sp, __LINE__);
                                pc += sizeof(int);
                        }
                        break;
		} case iBNE: {	/* Branch if non-zero or non-void */
			pc += align_o(pc, sizeof(int));
			if (ivy->sp->type == tVOID) {
                                ivy->sp = rmval(ivy->sp, __LINE__);
				pc += sizeof(int);
			} else if (ivy->sp->type != tNUM) {
				ivy->sp = rmval(ivy->sp, __LINE__);
				pc += *(int *)pc;
			} else if (ivy->sp->u.num) {
                                ivy->sp = rmval(ivy->sp, __LINE__);
				pc += *(int *)pc;
			} else {
                                ivy->sp = rmval(ivy->sp, __LINE__);
				pc += sizeof(int);
			}
			break;
		} case iBGT: {	/* Branch if > 0 */
			pc += align_o(pc, sizeof(int));
			if (ivy->sp->type == tVOID) {
                                ivy->sp = rmval(ivy->sp, __LINE__);
				pc+= *(int *)pc;
			} else if (ivy->sp->type != tNUM) {
			        error_0(ivy->errprn, "Argument for branch is not a number?");
				longjmp(ivy->err, 1);
			} else if (ivy->sp->u.num > 0) {
                                ivy->sp = rmval(ivy->sp, __LINE__);
                                pc += *(int *)pc;
			} else {
                                ivy->sp = rmval(ivy->sp, __LINE__);
				pc += sizeof(int);
                        }
			break;
		} case iBLT: {	/* Branch if < 0 */
			pc += align_o(pc, sizeof(int));
			if (ivy->sp->type == tVOID) {
                                ivy->sp = rmval(ivy->sp, __LINE__);
				pc += sizeof(int);
			} else if (ivy->sp->type != tNUM) {
			        error_0(ivy->errprn, "Argument for branch is not a number?");
				longjmp(ivy->err, 1);
			} else if (ivy->sp->u.num < 0) {
                                ivy->sp = rmval(ivy->sp, __LINE__);
				pc += *(int *)pc;
			} else {
                                ivy->sp = rmval(ivy->sp, __LINE__);
				pc += sizeof(int);
                        }
			break;
		} case iBGE: {	/* Branch if >= 0 */
			pc += align_o(pc, sizeof(int));
			if (ivy->sp->type == tVOID) {
                                ivy->sp = rmval(ivy->sp, __LINE__);
				pc += *(int *)pc;
			} else if (ivy->sp->type != tNUM) {
			        error_0(ivy->errprn, "Argument for branch is not a number?");
				longjmp(ivy->err, 1);
			} else if (ivy->sp->u.num >= 0) {
                                ivy->sp = rmval(ivy->sp, __LINE__);
				pc += *(int *)pc;
			} else {
                                ivy->sp = rmval(ivy->sp, __LINE__);
				pc += sizeof(int);
                        }
			break;
		} case iBLE: {	/* Branch if <= 0 */
			pc += align_o(pc, sizeof(int));
			if (ivy->sp->type == tVOID) {
                                ivy->sp = rmval(ivy->sp, __LINE__);
				pc += *(int *)pc;
			} else if (ivy->sp->type != tNUM) {
			        error_0(ivy->errprn, "Argument for branch is not a number?");
				longjmp(ivy->err, 1);
			} else if (ivy->sp->u.num <= 0) {
                                ivy->sp = rmval(ivy->sp, __LINE__);
				pc += *(int *)pc;
			} else {
                                ivy->sp = rmval(ivy->sp, __LINE__);
				pc += sizeof(int);
                        }
			break;
		} case iCOM: {	/* 1's complement */
			if (ivy->sp->type == tNUM)
				ivy->sp->u.num = ~ivy->sp->u.num;
			else {
			        error_0(ivy->errprn, "Argument for complement is not a number?");
				longjmp(ivy->err, 1);
                        }
			break;
		} case iNEG: {	/* 2's complement */
			if (ivy->sp->type == tNUM)
				ivy->sp->u.num = -ivy->sp->u.num;
			else if (ivy->sp->type == tFP)
				ivy->sp->u.fp = -ivy->sp->u.fp;
			else {
			        error_0(ivy->errprn, "Argument for negate is not a number?");
				longjmp(ivy->err, 1);
                        }
			break;
		} case iSHL: {	/* Shift left */
			if (ivy->sp[0].type == tNUM && ivy->sp[-1].type == tNUM)
				ivy->sp[-1].u.num <<= ivy->sp[0].u.num, --ivy->sp;
			else {
			        error_0(ivy->errprn, "Improper types for shift-left");
				longjmp(ivy->err, 1);
                        }
			break;
                } case iSHR: {	/* Shift right */
			if (ivy->sp[0].type == tNUM && ivy->sp[-1].type == tNUM)
				ivy->sp[-1].u.num >>= ivy->sp[0].u.num, --ivy->sp;
			else {
			        error_0(ivy->errprn, "Improper types for shift-right");
				longjmp(ivy->err, 1);
                        }
			break;
                } case iMUL: {	/* Multiply */
			if (ivy->sp[0].type == tNUM && ivy->sp[-1].type == tNUM)
				ivy->sp[-1].u.num *= ivy->sp[0].u.num, --ivy->sp;
			else if (ivy->sp[0].type == tFP && ivy->sp[-1].type == tFP)
				ivy->sp[-1].u.fp *= ivy->sp[0].u.fp, --ivy->sp;
			else if (ivy->sp[-1].type == tFP && ivy->sp[0].type == tNUM)
				ivy->sp[-1].u.fp *= ivy->sp[0].u.num, --ivy->sp;
			else if (ivy->sp[-1].type == tNUM && ivy->sp[0].type == tFP) {
				ivy->sp[-1].u.fp = ivy->sp[-1].u.num * ivy->sp[0].u.fp;
				ivy->sp[-1].type = tFP;
				--ivy->sp;
			} else {
			        error_0(ivy->errprn, "Improper types for multiply");
				longjmp(ivy->err, 1);
                        }
			break;
                } case iDIV: {	/* Divide */
			if (ivy->sp[0].type == tNUM && ivy->sp[-1].type == tNUM)
				ivy->sp[-1].u.num /= ivy->sp[0].u.num, --ivy->sp;
			else if (ivy->sp[0].type == tFP && ivy->sp[-1].type == tFP)
				ivy->sp[-1].u.fp /= ivy->sp[0].u.fp, --ivy->sp;
			else if (ivy->sp[-1].type == tFP && ivy->sp[0].type == tNUM)
				ivy->sp[-1].u.fp /= ivy->sp[0].u.num, --ivy->sp;
			else if (ivy->sp[-1].type == tNUM && ivy->sp[0].type == tFP) {
				ivy->sp[-1].u.fp = ivy->sp[-1].u.num / ivy->sp[0].u.fp;
				ivy->sp[-1].type = tFP;
				--ivy->sp;
			} else {
			        error_0(ivy->errprn, "Improper types for divide");
				longjmp(ivy->err, 1);
			}
			break;
                } case iMOD: {	/* Remainder */
			if (ivy->sp[0].type == tNUM && ivy->sp[-1].type == tNUM)
				ivy->sp[-1].u.num %= ivy->sp[0].u.num, --ivy->sp;
			else {
			        error_0(ivy->errprn, "Improper types for remainder");
				longjmp(ivy->err, 1);
                        }
			break;
                } case iAND: {	/* Bit-wise AND */
			if (ivy->sp[0].type == tNUM && ivy->sp[-1].type == tNUM)
				ivy->sp[-1].u.num &= ivy->sp[0].u.num, --ivy->sp;
			else {
			        error_0(ivy->errprn, "Improper types for bit-wise and");
				longjmp(ivy->err, 1);
                        }
			break;
                } case iADD: {
			if (ivy->sp[0].type == tNUM && ivy->sp[-1].type == tNUM) {	/* Add numbers */
			        long long a = ivy->sp[-1].u.num;
			        a += ivy->sp[0].u.num;
                                ivy->sp = rmval(ivy->sp, __LINE__);
                                ivy->sp = rmval(ivy->sp, __LINE__);
                                *psh(ivy) = mkival(tNUM, a);
				// ivy->sp[-1].u.num += ivy->sp[0].u.num, --ivy->sp;
			} else if (ivy->sp[0].type == tFP && ivy->sp[-1].type == tFP)
				ivy->sp[-1].u.fp += ivy->sp[0].u.fp, --ivy->sp;
			else if (ivy->sp[-1].type == tFP && ivy->sp[0].type == tNUM)
				ivy->sp[-1].u.fp += ivy->sp[0].u.num, --ivy->sp;
			else if (ivy->sp[-1].type == tNUM && ivy->sp[0].type == tFP) {
				ivy->sp[-1].u.fp = ivy->sp[-1].u.num + ivy->sp[0].u.fp;
				ivy->sp[-1].type = tFP;
				--ivy->sp;
			} else if (ivy->sp[0].type == tSTR && ivy->sp[-1].type == tSTR) {	/* Concat. strings */
				int len =
				    ivy->sp[0].u.str->len + ivy->sp[-1].u.str->len;
				char *s = (char *) malloc(len + 1);
				Str *str = alloc_str(s, len);
				memcpy(s, ivy->sp[-1].u.str->s, ivy->sp[-1].u.str->len);
				memcpy(s + ivy->sp[-1].u.str->len, ivy->sp[0].u.str->s, ivy->sp[0].u.str->len);
				s[len] = 0;
				ivy->sp = rmval(ivy->sp = rmval(ivy->sp, __LINE__), __LINE__);
				*++ivy->sp = mkpval(tSTR, str);
			} else {	/* Append element to object */

				Val newv = popval(ivy);
				if (ivy->sp[0].type == tOBJ) {
					Obj *n = dupobj(ivy->sp[0].u.obj, ivy->sp, 0, __LINE__);
					ivy->sp = rmval(ivy->sp, __LINE__);
					*++ivy->sp = mkpval(tOBJ, n);
					*set_by_number(n, ivy->sp[0].u.obj->ary_len) = newv;
				} else {
                                        error_0(ivy->errprn, "Improper types for add");
					longjmp(ivy->err, 1);
                                }
			}
			break;
                } case iSUB: {	/* Subtract */
			if (ivy->sp[0].type == tNUM && ivy->sp[-1].type == tNUM) {
			        long long a = ivy->sp[-1].u.num;
			        a -= ivy->sp[0].u.num;
                                ivy->sp = rmval(ivy->sp, __LINE__);
                                ivy->sp = rmval(ivy->sp, __LINE__);
                                *psh(ivy) = mkival(tNUM, a);
				// ivy->sp[-1].u.num -= ivy->sp[0].u.num, --ivy->sp;
			} else if (ivy->sp[0].type == tFP && ivy->sp[-1].type == tFP)
				ivy->sp[-1].u.fp -= ivy->sp[0].u.fp, --ivy->sp;
			else if (ivy->sp[-1].type == tFP && ivy->sp[0].type == tNUM)
				ivy->sp[-1].u.fp -= ivy->sp[0].u.num, --ivy->sp;
			else if (ivy->sp[-1].type == tNUM && ivy->sp[0].type == tFP) {
				ivy->sp[-1].u.fp = ivy->sp[-1].u.num - ivy->sp[0].u.fp;
				ivy->sp[-1].type = tFP;
				--ivy->sp;
			} else {
			        error_0(ivy->errprn, "Improper types for subtract");
				longjmp(ivy->err, 1);
                        }
			break;
                } case iOR: {
			if (ivy->sp[0].type == tNUM && ivy->sp[-1].type == tNUM)	/* Bit-wise or */
				ivy->sp[-1].u.num |= ivy->sp[0].u.num, --ivy->sp;
			else {
				if (ivy->sp[0].type == tOBJ && ivy->sp[-1].type == tOBJ) {	/* Union objects */
					int x;
					Obj *t = ivy->sp[0].u.obj;
					int a = ivy->sp[-1].u.obj->ary_len;
					Obj *new = dupobj(ivy->sp[-1].u.obj, &ivy->sp[-1], 0, __LINE__);
					rmval(&ivy->sp[-1], __LINE__);
					ivy->sp[-1] = mkpval(tOBJ, new);
					for (x = 0; x != t->ary_len; ++x) {	/* Append array elements */
						*set_by_number(ivy->sp[-1].u.obj, x + a) = t->ary[x];
					}
					for (x = 0; x != (t->nam_tab_mask + 1); ++x)	/* Union symbols */
						if (t->nam_tab[x].name) {
							*set_by_symbol(ivy->sp[-1].u.obj, t->nam_tab[x].name) = t->nam_tab[x].val;
						}
					for (x = 0; x != (t->str_tab_mask + 1); ++x)	/* Union strings */
						if (t->str_tab[x].name) {
							*set_by_string(ivy->sp[-1].u.obj, t->str_tab[x].name) = t->str_tab[x].val;
						}
					ivy->sp = rmval(ivy->sp, __LINE__);
				} else {
				        error_0(ivy->errprn, "Improper types for subtract");
					longjmp(ivy->err, 1);
                                }
			}
			break;
                } case iXOR: {	/* Exclusive or */
			if (ivy->sp[0].type == tNUM && ivy->sp[-1].type == tNUM)
				ivy->sp[-1].u.num ^= ivy->sp[0].u.num, --ivy->sp;
			else {
			        error_0(ivy->errprn, "Improper types for exclusive or");
				longjmp(ivy->err, 1);
                        }
			break;
                } case iCMP: {
			if (ivy->sp[0].type == tVOID && ivy->sp[-1].type == tVOID) {
				ivy->sp = rmval(ivy->sp, __LINE__);
				ivy->sp = rmval(ivy->sp, __LINE__);
				*++ivy->sp = mkival(tNUM, 0);
			} else if (ivy->sp[0].type == tVOID
				   || ivy->sp[-1].type == tVOID) {
				ivy->sp = rmval(ivy->sp, __LINE__);
				ivy->sp = rmval(ivy->sp, __LINE__);
				*++ivy->sp = mkival(tNUM, 1);
			} else if (ivy->sp[0].type == tNUM && ivy->sp[-1].type == tNUM) {	/* Compare numbers */
				if (ivy->sp[-1].u.num == ivy->sp[0].u.num)
					ivy->sp[-1].u.num = 0;
				else if (ivy->sp[-1].u.num > ivy->sp[0].u.num)
					ivy->sp[-1].u.num = 1;
				else
					ivy->sp[-1].u.num = -1;
				--ivy->sp;
			} else if (ivy->sp[0].type == tFP && ivy->sp[-1].type == tFP) {
				if (ivy->sp[-1].u.fp == ivy->sp[0].u.fp)
					ivy->sp[-1].u.num = 0;
				else if (ivy->sp[-1].u.fp > ivy->sp[0].u.fp)
					ivy->sp[-1].u.num = 1;
				else
					ivy->sp[-1].u.num = -1;
				ivy->sp[-1].type = tNUM;
				--ivy->sp;
			} else if (ivy->sp[-1].type == tNUM
				   && ivy->sp[0].type == tFP) {
				if (ivy->sp[-1].u.num == ivy->sp[0].u.fp)
					ivy->sp[-1].u.num = 0;
				else if (ivy->sp[-1].u.num > ivy->sp[0].u.fp)
					ivy->sp[-1].u.num = 1;
				else
					ivy->sp[-1].u.num = -1;
				--ivy->sp;
			} else if (ivy->sp[-1].type == tFP
				   && ivy->sp[0].type == tNUM) {
				if (ivy->sp[-1].u.fp == ivy->sp[0].u.num)
					ivy->sp[-1].u.num = 0;
				else if (ivy->sp[-1].u.fp > ivy->sp[0].u.num)
					ivy->sp[-1].u.num = 1;
				else
					ivy->sp[-1].u.num = -1;
				ivy->sp[-1].type = tNUM;
				--ivy->sp;
			} else if (ivy->sp[0].type == tSTR && ivy->sp[-1].type == tSTR) {	/* Compare strings */
				int c = strcmp(ivy->sp[-1].u.str->s, ivy->sp[0].u.str->s);
				ivy->sp = rmval(ivy->sp = rmval(ivy->sp, __LINE__), __LINE__);
				*++ivy->sp = mkival(tNUM, c);
			} else if (ivy->sp[0].type == tOBJ && ivy->sp[-1].type == tOBJ) {	/* Compare objects */
				int c;
				if (ivy->sp[0].u.obj == ivy->sp[-1].u.obj)
					c = 0;
				else
					c = 1;
				ivy->sp = rmval(ivy->sp = rmval(ivy->sp, __LINE__), __LINE__);
				*++ivy->sp = mkival(tNUM, c);
			} else if (ivy->sp[0].type == tNAM && ivy->sp[-1].type == tNAM) {
				int c = !(ivy->sp[0].u.name == ivy->sp[-1].u.name);
				ivy->sp = rmval(ivy->sp = rmval(ivy->sp, __LINE__), __LINE__);
				*++ivy->sp = mkival(tNUM, c);
			} else {
			        error_0(ivy->errprn, "Improper types for compare");
				longjmp(ivy->err, 1);
                        }
			break;
                } case iBEG: {	/* New scoping level */
		        SCOPE_PRINTF("iBEG:\n");
			addlvl(ivy,ivy->vars);
			break;
                } case iEND: {	/* Eliminate scoping level */
		        SCOPE_PRINTF("iEND\n");
			rmvlvl(ivy);
			break;
                } case iLOC: {	/* Local variables */
                        long long y = ivy->sp--->u.num, x;
                        for (x = 0; x != y; ++x)
                                if (ivy->sp->type == tSTR) {
                                        // set(ivy->vars, ivy->sp[0].u.str->s);
                                        ivy->sp = rmval(ivy->sp, __LINE__);
                                }
			break;
                } case iGET: {	/* Replace variable's name with its value */
			if (ivy->sp->type != tNAM) {
			        error_0(ivy->errprn, "Incorrect argument for iGET (supposed to be a string)");
				longjmp(ivy->err, 1);
			} else {
				char *name = ivy->sp[0].u.name;
				Obj *o = ivy->vars;
				Obj *next;
				Val *e;
				do {
					if ((e = get_by_symbol(o, name)))
						break;
					next = get_mom(o);
				} while ((o = next));
				
                                if (e) { /* We found it */
                                	*ivy->sp = *e;
					ivy->sp[0].origin = o;
					ivy->sp[0].idx_type = tNAM;
					ivy->sp[0].idx.name = name;
                                } else { /* It's new */
                                	mkval(ivy->sp, tVOID);
					ivy->sp[0].origin = ivy->vars;
					ivy->sp[0].idx_type = tNAM;
					ivy->sp[0].idx.name = name;
                                }
			}
			break;
                } case iGETF: {	/* Replace variable's name with its value, force current scope */
			if (ivy->sp->type != tNAM) {
			        error_0(ivy->errprn, "Improper argument for iGETF (supposed to be a name)");
				longjmp(ivy->err, 1);
			} else {
				char *name = ivy->sp[0].u.name;
				Val *o = set_by_symbol(ivy->vars, name);
				ivy->sp[0] = *o;
				ivy->sp[0].origin = ivy->vars;
				ivy->sp[0].idx_type = tNAM;
				ivy->sp[0].idx.name = name;
			}
			break;
                } case iAT: {
			if (ivy->sp->type != tCLOSURE) {
			        error_0(ivy->errprn, "Improper argument for *");
				longjmp(ivy->err, 1);
			} else {
				Closure *f;
				ivy->pc = pc;
				f = ivy->sp->u.closure;
				ivy->sp[0] = mkival(tLST, 0);
				callfunc(ivy, f);
				pc = ivy->pc;
				if (ivy->call_me) // Call a C-function...
					return 1;
				else
					break;
			}
			break;
                } case iSET: {	/* Set variable to value */
			doSET(ivy, ivy->sp, ivy->sp - 1);
			ivy->sp = rmval(ivy->sp, __LINE__);
			break;
		} case iCALL: {
                        /* If left side is VOID and it's from a variable,
                           create a new object */
/*
                        if (ivy->sp->type == tVOID && ivy->sp->origin) {
                        	Val newv = mkpval(tOBJ, alloc_obj(16, 4, 4));
                        	doSET(ivy, ivy->sp, &newv);
                        	ivy->sp[0].type = newv.type;
                        	ivy->sp[0].u = newv.u;
                        }
*/
                        if (ivy->sp->type == tCLOSURE) {
                                ivy->pc = pc;
                                callfunc(ivy, ivy->sp--->u.closure);
                                pc = ivy->pc;
                                if (ivy->call_me) // Call a C-function...
                                        return 1;
                                else
                                        break;
                        } else if (ivy->sp->type == tSTR || ivy->sp->type == tOBJ) {
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
                                        error_0(ivy->errprn, "No args allowed");
                                        longjmp(ivy->err, 1);
                                }
                                ivy->sp[-1] = ivy->sp[0];
                                --ivy->sp;
                        }
			break;
		} case iRTS: {	/* Return from function */
			*psh(ivy) = ivy->stashed;
			mkval(&ivy->stashed, tVOID);
			ivy->pc = pc;
			if (!retfunc(ivy))
				return 0;
			if (ivy->call_me)
				return 1;
                        pc = ivy->pc;
			break;
		} case iSTASH: { /* Pop return value */
			//printf("sp=%p sptop=%p\n", ivy->sp, ivy->sptop);
			ivy->stashed = popval(ivy);
			//pr(stdout, &ivy->stashed, 0);
			break;
		} case iPOP: {	/* Pop something off of stack */
			ivy->sp = rmval(ivy->sp, __LINE__);
			break;
                } case iPSH_VOID: {
			mkval(psh(ivy),tVOID);
			break;
                } case iPSH_THIS: {
			*psh(ivy) = mkpval(tOBJ, ivy->vars);
			break;
		} case iPSH_NUM: {
			pc += align_o(pc, sizeof(long long));
			*psh(ivy) = mkival(tNUM, *(long long *)pc);
			pc += sizeof(long long);
			break;
                } case iPSH_LST: {
			pc += align_o(pc, sizeof(int));
			*psh(ivy) = mkival(tLST, *(int *)pc);
			pc += sizeof(int);
			break;
                } case iPSH_FP: {
			pc += align_o(pc, sizeof(double));
			*psh(ivy) = mkdval(tFP, *(double *)pc);
			pc += sizeof(double);
			break;
                } case iPSH_STR: {
			int len;
			Val v;
			pc += align_o(pc, sizeof(int));
			len = *(int *)pc;
			pc += sizeof(int);
			char *ns = (char *)malloc(len + 1);
			memcpy(ns, pc, len + 1);
			Str *st = alloc_str(ns, len);
			v = mkpval(tSTR, st);
			*psh(ivy) = v;
			pc += len + 1;
			break;
                } case iPSH_NAM: {
                        char *s;
			Val v;
			pc += align_o(pc, sizeof(char *));
			s = *(char **)pc;
			pc += sizeof(char *);
			v = mkpval(tNAM, s);
			*psh(ivy) = v;
			break;
		} case iPSH_FUNC: { /* A function without context: record context now */
			Closure *closure;
			Val v;
			pc += align_o(pc, sizeof(void *));
			// printf("iPSH_FUNC: ivy->vars=%p\n", ivy->vars);
			v = mkpval(tCLOSURE, (closure = alloc_closure(*(void **)pc, ivy->vars)));
			*psh(ivy) = v;
			pc += sizeof(void *);
			/* Call initializers */
			ivy->pc = pc; mkclosure_init(ivy,closure); pc = ivy->pc;
			break;
                } case iPSH_PAIR: {
			mkval(psh(ivy), tPAIR);
			break;
		} case iFOREACH: { /* List iteration */
                        Val newv;
                        Obj *o = ivy->sp[-2].u.obj;
                        long long which = ivy->sp[-1].u.num;
                        long long n = ivy->sp[0].u.num;
                        pc += align_o(pc, sizeof(int));
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
                        	dupval(&newv, &o->str_tab[n].val);
			else if (which == 1)
				dupval(&newv, &o->nam_tab[n].val);
                        else
	                        dupval(&newv, &o->ary[n]);
                        // FIXME add checking here: is it really a variable?
                        doSET(ivy, &ivy->sp[-3], &newv);
                        ivy->sp[0].u.num = n;
                        ivy->sp[-1].u.num = which;
                        pc += *(int *)pc;
                        bye:;
			break;
		} case iFORINDEX: { /* List iteration */
                        Val newv;
                        Obj *o = ivy->sp[-2].u.obj;
                        long long which = ivy->sp[-1].u.num;
                        long long n = ivy->sp[0].u.num;
                        pc += align_o(pc, sizeof(int));

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
				newv = mkpval(tSTR, alloc_str(o->str_tab[n].name, strlen(o->str_tab[n].name)));
			} else if (which == 1) {
				// Hmm...
				newv = mkpval(tNAM, o->nam_tab[n].name);
			} else {
				newv = mkival(tNUM, n);
			}
                        // FIXME add checking here: is it really a variable?
                        doSET(ivy, &ivy->sp[-3], &newv);
                        ivy->sp[0].u.num = n;
                        ivy->sp[-1].u.num = which;
                        pc += *(int *)pc;
                        bye1:;
			break;
		} case iFIX: { /* Convert stack list into an array */
                        Val newv = popval(ivy);
                        *++ivy->sp = newv;
			break;
		} default: {
                        error_0(ivy->errprn, "Unknown instruction?");
                        longjmp(ivy->err, 1);
		}
        } }
}

/* Unwind stack */

void popall(Ivy *ivy)
{
        while (ivy->sp != ivy->sptop) {
                switch(ivy->sp->type) {
                        case tNUM: {
                                fprintf(ivy->out, "%d:	Integer = %lld\n", (int)(ivy->sp - ivy->sptop), ivy->sp->u.num);
                                ivy->sp = rmval(ivy->sp, __LINE__);
                                break;
                        } case tSTR: {
                                fprintf(ivy->out, "%d:	String = \"%s\"\n", (int)(ivy->sp - ivy->sptop), ivy->sp->u.str->s);
                                ivy->sp = rmval(ivy->sp, __LINE__);
                                break;
                        } case tNAM: {
                                fprintf(ivy->out, "%d:	Symbol = %s\n", (int)(ivy->sp - ivy->sptop), ivy->sp->u.name);
                                ivy->sp = rmval(ivy->sp, __LINE__);
                                break;
                        } case tOBJ: {
                                fprintf(ivy->out, "%d:	Object = %p\n", (int)(ivy->sp - ivy->sptop), ivy->sp->u.obj);
                                ivy->sp = rmval(ivy->sp, __LINE__);
                                break;
                        } case tCLOSURE: {
                                fprintf(ivy->out, "%d:	CLOSURE\n", (int)(ivy->sp - ivy->sptop));
                                --ivy->sp;
                                break;
                        } case tLST: {
                                fprintf(ivy->out, "%d:	LST = %lld\n", (int)(ivy->sp - ivy->sptop), ivy->sp->u.num);
                                --ivy->sp;
                                break;
                        } case tPAIR: {
                                fprintf(ivy->out, "%d:	Pair\n", (int)(ivy->sp - ivy->sptop));
                                ivy->sp = rmval(ivy->sp, __LINE__);
                                break;
                        } case tVOID: {
                                fprintf(ivy->out, "%d:	Void\n", (int)(ivy->sp - ivy->sptop));
                                ivy->sp = rmval(ivy->sp, __LINE__);
                                break;
                        } case tFP: {
                                fprintf(ivy->out, "%d:	Float = %lg\n", (int)(ivy->sp - ivy->sptop), ivy->sp->u.fp);
                                ivy->sp = rmval(ivy->sp, __LINE__);
                                break;
                        } case tRET_IVY: {
                                fprintf(ivy->out, "%d:	RET_IVY (pc = %p, callfunc = %p)\n", (int)(ivy->sp - ivy->sptop), ivy->sp->u.pc, ivy->sp->idx.callfunc);
                                --ivy->sp;
                                break;
                        } case tRET_SIMPLE: {
                                fprintf(ivy->out, "%d:	RET_SIMPLE (pc = %p, callfunc = %p", (int)(ivy->sp - ivy->sptop), ivy->sp->u.pc, ivy->sp->idx.callfunc);
                                --ivy->sp;
                                fprintf(ivy->out, "func = %p, obj = %p)\n", ivy->sp->idx.func, ivy->sp->u.obj);
                                --ivy->sp;
                                break;
                        } case tRET_NEXT_INIT: {
                                fprintf(ivy->out, "%d:	RET_NEXT_INIT (pc = %p, closure = %p)\n", (int)(ivy->sp - ivy->sptop), ivy->sp->u.pc, ivy->sp->idx.closure);
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

Val *pr(FILE *out, Val * v, int lvl)
{
	switch (v->type) {
	        case tNUM: {
        		fprintf(out, "%lld", v->u.num);
        		return v + 1;
		} case tNAM: {
			fprintf(out, "`%s", v->u.name);
			return v + 1;
                } case tFP: {
        		fprintf(out, "%g", v->u.fp);
        		return v + 1;
                } case tSTR: {
        		fprintf(out, "\"%s\"", v->u.str->s);
        		return v + 1;
                } case tVOID: {
        		fprintf(out, "void");
        		return v + 1;
                } case tCLOSURE: {
        		Val w;
        		fprintf(out, "Closure %p f=%p scope=%p: ", v->u.closure, v->u.closure->f, v->u.closure->scope);
        		w.type = tOBJ;
        		w.u.obj = v->u.closure->scope;
        		pr(out, &w,lvl+4);
        		return v + 1;
                } case tOBJ: {
			int x;
			if (v->u.obj->visit)
				fprintf(out, "{ %d at 0x%p } (previously shown)", v->u.obj->objno, v->u.obj);
			else {
				v->u.obj->visit = 1;
				fprintf(out, "{ %d at 0x%p\n", v->u.obj->objno, v->u.obj);
				for (x = 0; x != (v->u.obj->nam_tab_mask + 1); ++x)
					if (v->u.obj->nam_tab[x].name) {
						/* if (e->var->val.type != tFUN || !e->var->val.u.fun->f->cfunc) */ {
							/* if (first)
								fprintf(out, " ");
							else
								first = 1; */
							indent(out, lvl+4);
							fprintf(out, "`%s=", v->u.obj->nam_tab[x].name);
							pr(out, &v->u.obj->nam_tab[x].val, lvl+4);
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
							indent(out, lvl+4);
							fprintf(out, "`\"%s\"=", v->u.obj->str_tab[x].name);
							pr(out, &v->u.obj->str_tab[x].val, lvl+4);
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
						indent(out, lvl+4), fprintf(out, "`%d=", x),
						    pr(out, &v->u.obj->ary[x], lvl+4), fprintf(out, "\n");
				}
				indent(out, lvl);
				fprintf(out, "}");
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

void add_cfunc(Ivy *ivy, Obj *vars, char *name, char *argstr, void (*cfunc) ())
{
	char bf[1024];
	Node *args;
	Func *o;
	int argc;
	char **argv;
	char *quote;
	Pseudo **initv;
	strcpy(bf, argstr);
	args = compargs(ivy, bf);
	argc = cntlst(args);
	quote = (char *) calloc(argc, 1);
	argv = (char **) malloc(argc * sizeof(char *));
	initv = (Pseudo **) malloc(argc * sizeof(Pseudo *));
	genlst(ivy->errprn, argv, initv, quote, args);
	o = mkfunc(NULL, argc, argv, initv, quote);
	o->cfunc = cfunc;
	/* Put new function in table */
	*set_by_symbol(vars, symbol_add(name)) = mkpval(tCLOSURE, alloc_closure(o, vars));
}

/* Initialize global variables and symbols*/

char *a_symbol;
char *b_symbol;
char *mom_symbol;
char *dynamic_symbol;
char *argv_symbol;

Obj *mk_globals(Ivy *ivy)
{
	Obj *o;
	int x;

	a_symbol = symbol_add("a");
	b_symbol = symbol_add("b");
	mom_symbol = symbol_add("mom");
	dynamic_symbol = symbol_add("dynamic");
	argv_symbol = symbol_add("argv");

	o = alloc_obj(128, 4, 4);

	for (x = 0; builtins[x].name; ++x)
		add_cfunc(ivy, o, builtins[x].name, builtins[x].args, builtins[x].cfunc);
	return o;
}

/* Initialize an interpreter */

Ivy *ivys;

void mk_ivy(Ivy *ivy, void (*err_print)(void *obj, char *), void *err_obj, FILE *in, FILE *out)
{
	ivy->next = ivys;
	ivys = ivy;
        ivy->errprn->error_flag = 0;
        ivy->errprn->error_obj = err_obj;
        ivy->errprn->error_print = err_print;
	ivy->sptop = ivy->sp = (Val *) malloc(sizeof(Val) * (ivy->spsize = 1024));
	ivy->vars = ivy->glblvars = 0;
	ivy->pc = 0;
	ivy->call_me = 0;
	ivy->call_me_obj = 0;
	ivy->out = out;
	ivy->in = in;
	mkval(&ivy->stashed, tVOID);
}

void set_globals(Ivy *ivy, Obj *globals)
{
	SCOPE_PRINTF1("Initial scope = %p\n", globals);
        ivy->vars = ivy->glblvars = globals;
}

/* Run some compiled code */

Val run(Ivy *ivy, Pseudo *code, int ptop, int trace)
{
	Val rtn;
	rtn.type = tVOID;
	if (!setjmp(ivy->err)) {
	        ivy->pc = code;
		while (pexe(ivy, trace)) {
			ivy->call_me(ivy);
			ivy->call_me = 0;
			retfunc(ivy);
		}
		rtn = popval(ivy);
		if (ptop) {
       			pr(ivy->out, &rtn,0);
       			fprintf(ivy->out, "\n");
		}
		if (ivy->sp != ivy->sptop) {
		        fprintf(ivy->out, "Oops, stack not empty?\n");
		        fprintf(ivy->out, "Stack:\n");
		        popall(ivy);
		}
		if (ivy->vars != ivy->glblvars) {
		        fprintf(ivy->out, "Oops, current scope level is not global?\n");
                        fprintf(ivy->out, "Scope:\n");
                        while (ivy->vars != ivy->glblvars) {
                                fprintf(ivy->out, "Scope = %p\n", ivy->vars);
                                rmvlvl(ivy);
                        }
		}
	} else {
		fprintf(ivy->out, "\nError stop\n");
		fprintf(ivy->out, "Stack:\n");
		popall(ivy);
		fprintf(ivy->out, "Scope:\n");
		while (ivy->vars != ivy->glblvars) {
		        fprintf(ivy->out, "Scope = %p\n", ivy->vars);
		        rmvlvl(ivy);
                }
        }
	return rtn;
}
