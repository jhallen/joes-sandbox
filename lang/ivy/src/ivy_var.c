#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include "ivy.h"

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
		// printf("New var page\n");
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

int mark_var_count;

void mark_var(Var *var)
{
	if (var->next_free != var) {
		var->next_free = var;
		++mark_var_count;
		mark_val(&var->val);
	}
}

void mark_protected_vars()
{
	struct var_protect *vp;
	for (vp = var_protect_list; vp; vp = vp->next)
		mark_var(vp->var);
}

void sweep_vars()
{
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
}

void clear_protected_vars()
{
	struct var_protect *vp;
	while (var_protect_list) {
		vp = var_protect_list;
		var_protect_list = vp->next;
		free(vp);
	}
}
