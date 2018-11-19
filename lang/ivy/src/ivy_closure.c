#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include "ivy.h"

static struct closure_page {
	struct closure_page *next;
	Closure closures[128];
} *closure_pages;

static Closure *free_closures;

/* Protect from gc list */

static struct closure_protect {
	struct closure_protect *next;
	Closure *closure;
} *closure_protect_list;

void protect_closure(Closure *closure)
{
	struct closure_protect *prot = (struct closure_protect *)malloc(sizeof(struct closure_protect));
	prot->next = closure_protect_list;
	prot->closure = closure;
	closure_protect_list = prot;
}

Closure *alloc_closure(Func *func, Obj *scope)
{
	Closure *closure;
	if (++alloc_count == GC_COUNT)
		collect();
	if (!free_closures) {
		struct closure_page *op = (struct closure_page *)calloc(1, sizeof(struct closure_page));
		int x;
		for (x = 0; x != sizeof(op->closures) / sizeof(Closure); ++x) {
			op->closures[x].next_free = free_closures;
			free_closures = &op->closures[x];
		}
		op->next = closure_pages;
		closure_pages = op;
		// printf("New fun page\n");
	}
	closure = free_closures;
	free_closures = closure->next_free;
	closure->next_free = 0;
	closure->f = func;
	closure->scope = scope;
	if (func->nargs) {
		int x;
		closure->init_vals = (Val *)malloc(sizeof(Val) * func->nargs);
		for (x = 0; x != func->nargs; ++x)
			mkval(&closure->init_vals[x], tVOID);
	} else
		closure->init_vals = 0;
	closure->x = 0;
	protect_closure(closure);
	return closure;
}

static void free_closure(Closure *closure)
{
	if (closure->init_vals) {
		free(closure->init_vals);
		closure->init_vals = 0;
	}
	closure->next_free = free_closures;
	free_closures = closure;
}

int mark_closure_count;

void mark_closure(Closure *closure)
{
	if (closure->next_free != closure) {	
		int x;
		closure->next_free = closure;
		++mark_closure_count;
		mark_obj(closure->scope);
		for (x = 0; x != closure->f->nargs; ++x)
			mark_val(&closure->init_vals[x]);
	}
}

void mark_protected_closures()
{
	struct closure_protect *fp;
	for (fp = closure_protect_list; fp; fp = fp->next)
		mark_closure(fp->closure);
}

void sweep_closures()
{
	struct closure_page *fp;
	free_closures = 0;
	for (fp = closure_pages; fp; fp = fp->next) {
		int x;
		for (x = 0; x != sizeof(fp->closures) / sizeof(Closure); ++x) {
			if (fp->closures[x].next_free == &fp->closures[x])
				fp->closures[x].next_free = 0;
			else
				free_closure(&fp->closures[x]);
		}
	}
}

void clear_protected_closures()
{
	struct closure_protect *fp;
	while (closure_protect_list) {
		fp = closure_protect_list;
		closure_protect_list = fp->next;
		free(fp);
	}
}
