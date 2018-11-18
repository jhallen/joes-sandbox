#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include "ivy.h"

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

void protect_fun(Fun *fun)
{
	struct fun_protect *prot = (struct fun_protect *)malloc(sizeof(struct fun_protect));
	prot->next = fun_protect_list;
	prot->fun = fun;
	fun_protect_list = prot;
}

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
		// printf("New fun page\n");
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
	protect_fun(fun);
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

int mark_fun_count;

void mark_fun(Fun *fun)
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

void mark_protected_funs()
{
	struct fun_protect *fp;
	for (fp = fun_protect_list; fp; fp = fp->next)
		mark_fun(fp->fun);
}

void sweep_funs()
{
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
}

void clear_protected_funs()
{
	struct fun_protect *fp;
	while (fun_protect_list) {
		fp = fun_protect_list;
		fun_protect_list = fp->next;
		free(fp);
	}
}
