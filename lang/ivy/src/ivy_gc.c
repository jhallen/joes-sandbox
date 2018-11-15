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
#include "atom.h"
#include "ivy_frag.h"
#include "ivy_tree.h"
#include "ivy.h"
#include "ivy_gc.h"

int alloc_count; /* Number of times an allocation function has been called */
#define GC_COUNT 102 /* Collect garbage after this many calls */

/* String allocation page */

static struct str_page {
	struct str_page *next;
	Str strs[1024];
} *str_pages;

/* String free list */

static Str *free_strs;

/* Protect from gc list */

static struct str_protect {
	struct str_protect *next;
	Str *str;
} *str_protect_list;

static void collect();

Str *alloc_str(char *s, size_t len)
{
	Str *str;
	if (++alloc_count == GC_COUNT)
		collect();
	if (!free_strs) {
		struct str_page *op = (struct str_page *)calloc(1, sizeof(struct str_page));
		int x;
		for (x = 0; x != sizeof(op->strs) / sizeof(Str); ++x) {
			op->strs[x].next_free = free_strs;
			free_strs = &op->strs[x];
		}
		op->next = str_pages;
		str_pages = op;
		printf("New str page %p\n", op);
	}
	str = free_strs;
	free_strs = str->next_free;
	str->next_free = 0;
	str->len = len;
	str->s = s;
	struct str_protect *prot = (struct str_protect *)malloc(sizeof(struct str_protect));
	prot->next = str_protect_list;
	prot->str = str;
	str_protect_list = prot;
	return str;
}

static void free_str(Str *s)
{
	if (s->s) {
		free(s->s);
		s->s = 0;
	}
	s->next_free = free_strs;
	free_strs = s;
}

static struct var_page {
	struct var_page *next;
	Var vars[1024];
	
} *var_pages;

static Var *free_vars;

/* Protect from gc list */

static struct var_protect {
	struct var_protect *next;
	Var *var;
} *var_protect_list;

int alloc_var_count;
int free_var_count;

void check_var_freelist()
{
	free_var_count = 0;
	Var *v;
	for (v = free_vars; v; v = v->next_free)
		++free_var_count;
	printf("Freelist has %d vars\n", free_var_count);
}

Var *alloc_var()
{
	Var *v;
	if (++alloc_count == GC_COUNT)
		collect();
	if (!free_vars) {
		struct var_page *op = (struct var_page *)calloc(1, sizeof(struct var_page));
		int x;
		for (x = 0; x != sizeof(op->vars) / sizeof(Var); ++x) {
			op->vars[x].next_free = free_vars;
			free_vars = &op->vars[x];
		}
		op->next = var_pages;
		var_pages = op;
		printf("New var page\n");
	}
	v = free_vars;
	free_vars = v->next_free;
	v->next_free = 0;
	v->val.var = 0;
	v->val.type = tVOID;
	v->val.u.name = 0;
	struct var_protect *prot = (struct var_protect *)malloc(sizeof(struct var_protect));
	prot->next = var_protect_list;
	prot->var = v;
	var_protect_list = prot;
	++alloc_var_count;
	return v;
}

static void free_var(Var *v)
{
	v->next_free = free_vars;
	free_vars = v;
	++free_var_count;
}

static struct entry_page {
	struct entry_page *next;
	Entry entries[1024];
} *entry_pages;

static Entry *free_entries;

Entry *alloc_entry()
{
	Entry *e;
	if (++alloc_count == GC_COUNT)
		collect();
	if (!free_entries) {
		struct entry_page *op = (struct entry_page *)calloc(1, sizeof(struct entry_page));
		int x;
		for (x = 0; x != sizeof(op->entries) / sizeof(Entry); ++x) {
			op->entries[x].next = free_entries;
			free_entries = &op->entries[x];
		}
		op->next = entry_pages;
		entry_pages = op;
		printf("New entry page\n");
	}
	e = free_entries;
	free_entries = e->next;
	e->name = 0;
	e->var = 0;
	e->next = 0;
	return e;
}

static void free_entry(Entry *e)
{
	e->next = free_entries;
	free_entries = e;
}

static struct obj_page {
	struct obj_page *next;
	Obj objs[128];
} *obj_pages;

static Obj *free_objs;

/* Protect from gc list */

static struct obj_protect {
	struct obj_protect *next;
	Obj *obj;
} *obj_protect_list;

int next_obj_no;

void protect_obj(Obj *o)
{
	struct obj_protect *prot = (struct obj_protect *)malloc(sizeof(struct obj_protect));
	prot->next = obj_protect_list;
	prot->obj = o;
	obj_protect_list = prot;
}

Obj *alloc_obj(int size)
{
	Obj *o;
	if (++alloc_count == GC_COUNT)
		collect();
	if (size < 16)
		size = 16;
	if (!free_objs) {
		struct obj_page *op = (struct obj_page *)calloc(1, sizeof(struct obj_page));
		int x;
		for (x = 0; x != sizeof(op->objs) / sizeof(Obj); ++x) {
			op->objs[x].next_free = free_objs;
			free_objs = &op->objs[x];
		}
		op->next = obj_pages;
		obj_pages = op;
		printf("New obj page\n");
	}
	o = free_objs;
	free_objs = o->next_free;
	o->next_free = 0;
	o->arysiz = 16;
	o->ary = (Var **)calloc(o->arysiz, sizeof(Var *));
	o->nitems = 0;
	o->size = size;
	o->tab = (Entry **)calloc(o->size, sizeof(Entry *));
	o->visit = 0;
	o->objno = next_obj_no++;
	protect_obj(o);
	return o;
}

static void free_obj(Obj *o)
{
	if (o->tab) {
		int x;
		for (x = 0; x != o->size; ++x) {
			Entry *e, *n;
			for (e = o->tab[x]; e; e = n) {
				n = e->next;
				free_entry(e);
			}
		}
		free(o->tab);
		o->tab = 0;
	}
	if (o->ary) {
		free(o->ary);
		o->ary = 0;
	}
	o->next_free = free_objs;
	free_objs = o;
}

static struct fun_page {
	struct fun_page *next;
	Fun funs[128];
} *fun_pages;

static Fun *free_funs;

/* Protect from gc list */

static struct fun_protect {
	struct fun_protect *next;
	Fun *fun;
} *fun_protect_list;

Fun *alloc_fun(Func *func, Obj *scope)
{
	Fun *fun;
	if (++alloc_count == GC_COUNT)
		collect();
	if (!free_funs) {
		struct fun_page *op = (struct fun_page *)calloc(1, sizeof(struct fun_page));
		int x;
		for (x = 0; x != sizeof(op->funs) / sizeof(Fun); ++x) {
			op->funs[x].next_free = free_funs;
			free_funs = &op->funs[x];
		}
		op->next = fun_pages;
		fun_pages = op;
		printf("New fun page\n");
	}
	fun = free_funs;
	free_funs = fun->next_free;
	fun->next_free = 0;
	fun->f = func;
	fun->scope = scope;
	if (func->nargs) {
		int x;
		fun->init_vals = (Val *)malloc(sizeof(Val) * func->nargs);
		for (x = 0; x != func->nargs; ++x)
			mkval(&fun->init_vals[x], tVOID);
	} else
		fun->init_vals = 0;
	fun->x = 0;
	struct fun_protect *prot = (struct fun_protect *)malloc(sizeof(struct fun_protect));
	prot->next = fun_protect_list;
	prot->fun = fun;
	fun_protect_list = prot;
	return fun;
}

static void free_fun(Fun *fun)
{
	if (fun->init_vals) {
		free(fun->init_vals);
		fun->init_vals = 0;
	}
	fun->next_free = free_funs;
	free_funs = fun;
}

static void mark_str(Str *str);
static void mark_obj(Obj *obj);
static void mark_fun(Fun *fun);
static void mark_var(Var *var);

static Val *mark_val(Val *val)
{
	switch (val->type) {
		case tNARG: case tSTR: {
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

int mark_fun_count;

static void mark_fun(Fun *fun)
{
	if (fun->next_free != fun) {	
		int x;
		fun->next_free = fun;
		++mark_fun_count;
		mark_obj(fun->scope);
		for (x = 0; x != fun->f->nargs; ++x)
			mark_val(&fun->init_vals[x]);
	}
}

int mark_str_count;

static void mark_str(Str *str)
{
	if (str->next_free != str) {
		str->next_free = str;
		++mark_str_count;
	}
}

int mark_var_count;

static void mark_var(Var *var)
{
	if (var->next_free != var) {
		var->next_free = var;
		++mark_var_count;
		mark_val(&var->val);
	}
}

int mark_obj_count;

static void mark_obj(Obj *o)
{
	if (o->next_free != o) {
		int x;
		o->next_free = o;
		++mark_obj_count;
		for (x = 0; x != o->nitems; ++x)
			if (o->ary[x]) {
				mark_var(o->ary[x]);
			}
		for (x = 0; x != o->size; ++x) {
			Entry *e;
			for (e = o->tab[x]; e; e = e->next) {
				if (e->var) {
					mark_var(e->var);
				}
			}
		}
	}
}

extern Ivy *ivys;

void mark_protected()
{
	struct str_protect *sp;
	struct obj_protect *op;
	struct fun_protect *fp;
	struct var_protect *vp;
	for (sp = str_protect_list; sp; sp = sp->next)
		mark_str(sp->str);

	for (op = obj_protect_list; op; op = op->next)
		mark_obj(op->obj);

	for (fp = fun_protect_list; fp; fp = fp->next)
		mark_fun(fp->fun);

	for (vp = var_protect_list; vp; vp = vp->next)
		mark_var(vp->var);
}

/* Collect garbage */

void collect()
{
	Val *p;
	Ivy *ivy;

	alloc_count = 0;

	printf("Mark\n");

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
			printf("mark globals %d\n", ivy->glblvars->objno);
			mark_obj(ivy->glblvars);
		}
		if (ivy->vars) {
			printf("mark scope %d\n", ivy->vars->objno);
			mark_obj(ivy->vars);
		}
		mark_val(&ivy->stashed);
	}

	printf("mark_fun_count = %d\n", mark_fun_count);
	printf("mark_str_count = %d\n", mark_str_count);
	printf("mark_var_count = %d\n", mark_var_count);
	printf("mark_obj_count = %d\n", mark_obj_count);

	printf("Sweep\n");

	printf("alloc_var_count=%d\n", alloc_var_count);

	free_var_count = 0;
	alloc_var_count = 0;

	/* Sweep variables */
	struct var_page *vp;
	free_vars = 0;
	for (vp = var_pages; vp; vp = vp->next) {
		int x;
		for (x = 0; x != sizeof(vp->vars) / sizeof(Var); ++x) {
			if (vp->vars[x].next_free == &vp->vars[x]) {
				vp->vars[x].next_free = 0;
				alloc_var_count++;
			} else
				free_var(&vp->vars[x]);
		}
	}

	printf("Found free vars = %d\n", free_var_count);
	printf("Found allocated vars = %d\n", alloc_var_count);

	/* Sweep strings */
	struct str_page *sp;
	free_strs = 0;
	for (sp = str_pages; sp; sp = sp->next) {
		int x;
		for (x = 0; x != sizeof(sp->strs) / sizeof(Str); ++x) {
			if (sp->strs[x].next_free == &sp->strs[x])
				sp->strs[x].next_free = 0;
			else
				free_str(&sp->strs[x]);
		}
	}

	/* Sweep objects */
	struct obj_page *op;
	free_objs = 0;
	for (op = obj_pages; op; op = op->next) {
		int x;
		for (x = 0; x != sizeof(op->objs) / sizeof(Obj); ++x) {
			if (op->objs[x].next_free == &op->objs[x])
				op->objs[x].next_free = 0;
			else
				free_obj(&op->objs[x]);
		}
	}

	/* Sweep funs */
	struct fun_page *fp;
	free_funs = 0;
	for (fp = fun_pages; fp; fp = fp->next) {
		int x;
		for (x = 0; x != sizeof(fp->funs) / sizeof(Fun); ++x) {
			if (fp->funs[x].next_free == &fp->funs[x])
				fp->funs[x].next_free = 0;
			else
				free_fun(&fp->funs[x]);
		}
	}
	printf("Done.\n");
}

void gc_protect_done()
{
	struct str_protect *sp;
	struct obj_protect *op;
	struct fun_protect *fp;
	struct var_protect *vp;
	while (str_protect_list) {
		sp = str_protect_list;
		str_protect_list = sp->next;
		free(sp);
	}
	while (obj_protect_list) {
		op = obj_protect_list;
		obj_protect_list = op->next;
		free(op);
	}
	while (fun_protect_list) {
		fp = fun_protect_list;
		fun_protect_list = fp->next;
		free(fp);
	}
	while (var_protect_list) {
		vp = var_protect_list;
		var_protect_list = vp->next;
		free(vp);
	}
}
