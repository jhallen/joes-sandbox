#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include "hash.h"
#include "ivy.h"

/* Find hash table index which should be used for name.  Indicated Entry either has matching name or NULL. */

static int slot_for_symbol(Obj *t, char *name)
{
        int x;

        for (
        	x = (ahash(name) & t->nam_tab_mask);
        	t->nam_tab[x].name != name && t->nam_tab[x].name;
        	x = ((x + 1) & t->nam_tab_mask)
	);

        return x;
}

/* Get variable bound to symbol in an object.  If no variable is bound to the symbol, return NULL. */

Var *get_by_symbol(Obj *t, char *name)
{
        int x = slot_for_symbol(t, name);
        if (t->nam_tab[x].name)
        	return t->nam_tab[x].var;
	else
		return NULL;
}

/* Expand hash table to avoid clashes */

void expand_symbol_tab(Obj *t)
{
	int x, y;
	int new_tab_size = (t->nam_tab_mask + 1) * 2;
	int new_tab_mask = new_tab_size - 1;

	Entry *new_tab = (Entry *)calloc(new_tab_size, sizeof(Entry));

	for (y = 0; y != t->nam_tab_mask + 1; ++y) {
		if (t->nam_tab[y].name) {
			for (x = (ahash(t->nam_tab[y].name) & new_tab_mask); new_tab[x].name; x = ((x + 1) & new_tab_mask));
			new_tab[x].name = t->nam_tab[y].name;
			new_tab[x].var = t->nam_tab[y].var;
		}
	}

	free(t->nam_tab);

	t->nam_tab = new_tab;
	t->nam_tab_mask = new_tab_mask;
}

/* Get variable bound to symbol in an object.  If no variable is bound to the symbol create one. */

Var *set_by_symbol(Obj *t, char *name)
{
	int x = slot_for_symbol(t, name);

	if (t->nam_tab[x].name)
		return t->nam_tab[x].var;
	else {
		if (t->nam_tab_count == t->nam_tab_mask) {
			expand_symbol_tab(t);
			x = slot_for_symbol(t, name);
		}
		++t->nam_tab_count;
		t->nam_tab[x].name = name;
		return t->nam_tab[x].var = alloc_var();
	}
}

/* Bind already existing variable to symbol in an object. */

Var *set_by_symbol_ref(Obj *t, char *name, Var *repl_var)
{
	int x = slot_for_symbol(t, name);

	if (t->nam_tab[x].name)
		return t->nam_tab[x].var = repl_var;
	else {
		if (t->nam_tab_count == t->nam_tab_mask) {
			expand_symbol_tab(t);
			x = slot_for_symbol(t, name);
		}
		++t->nam_tab_count;
		t->nam_tab[x].name = name;
		return t->nam_tab[x].var = repl_var;
	}
}

/* Find hash table index which should be used for name.  Indicated Entry either has matching name or NULL. */

static int slot_for_string(Obj *t, char *name)
{
        int x;

        for (
        	x = (hash(name) & t->str_tab_mask);
        	t->str_tab[x].name && strcmp(t->str_tab[x].name, name);
        	x = ((x + 1) & t->str_tab_mask)
	);

        return x;
}

/* Get variable bound to symbol in an object.  If no variable is bound to the symbol, return NULL. */

Var *get_by_string(Obj *t, char *name)
{
        int x = slot_for_string(t, name);
        if (t->str_tab[x].name)
        	return t->str_tab[x].var;
	else
		return NULL;
}

/* Expand hash table to avoid clashes */

void expand_string_tab(Obj *t)
{
	int x, y;
	int new_tab_size = (t->str_tab_mask + 1) * 2;
	int new_tab_mask = new_tab_size - 1;

	Entry *new_tab = (Entry *)calloc(new_tab_size, sizeof(Entry));

	for (y = 0; y != t->str_tab_mask + 1; ++y) {
		if (t->str_tab[y].name) {
			for (x = (hash(t->str_tab[y].name) & new_tab_mask); new_tab[x].name; x = ((x + 1) & new_tab_mask));
			new_tab[x].name = t->str_tab[y].name;
			new_tab[x].var = t->str_tab[y].var;
		}
	}

	free(t->str_tab);

	t->str_tab = new_tab;
	t->str_tab_mask = new_tab_mask;
}

/* Get variable bound to symbol in an object.  If no variable is bound to the symbol create one. */

Var *set_by_string(Obj *t, char *name)
{
	int x = slot_for_string(t, name);

	if (t->str_tab[x].name)
		return t->str_tab[x].var;
	else {
		if (t->str_tab_count == t->str_tab_mask) {
			expand_string_tab(t);
			x = slot_for_string(t, name);
		}
		++t->str_tab_count;
		t->str_tab[x].name = strdup(name);
		return t->str_tab[x].var = alloc_var();
	}
}

/* Bind already existing variable to symbol in an object. */

Var *set_by_string_ref(Obj *t, char *name, Var *repl_var)
{
	int x = slot_for_string(t, name);

	if (t->str_tab[x].name)
		return t->str_tab[x].var = repl_var;
	else {
		if (t->str_tab_count == t->str_tab_mask) {
			expand_string_tab(t);
			x = slot_for_string(t, name);
		}
		++t->str_tab_count;
		t->str_tab[x].name = strdup(name);
		return t->str_tab[x].var = repl_var;
	}
}

/* Get variable bouond to number in an object.  If no variable bound, return NULL */

Var *get_by_number(Obj * t, long long num)
{
	if (num >= t->ary_len || num < 0)
		return NULL;
	else
		return t->ary[num];
}

Var *set_by_number(Obj * t, long long num)
{
	if (num < 0)
		return NULL;

	if (num >= t->ary_len) {
		if (num >= t->ary_size) {
			int x;
			t->ary = (Var **) realloc(t->ary, sizeof(Var *) * (num + 16));
			for (x = t->ary_size; x != num + 16; ++x)
				t->ary[x] = 0;
			t->ary_size = num + 16;
		}
		t->ary_len = num + 1;
	}

	if (t->ary[num])
		return t->ary[num];
	else
		return t->ary[num] = alloc_var();
}

/* Put variable 'o' numbered 'num' in t */

Var *set_by_number_ref(Obj *t, long long num, Var *repl_var)
{
	if (num < 0)
		return NULL;

	if (num >= t->ary_len) {
		if (num >= t->ary_size) {
			int x;
			t->ary = (Var **) realloc(t->ary, sizeof(Var *) * (num + 16));
			for (x = t->ary_size; x != num + 16; ++x)
				t->ary[x] = 0;
			t->ary_size = num + 16;
		}
		t->ary_len = num + 1;
	}

	return t->ary[num] = repl_var;
}


/* Duplicate an object non-recursively, create new variables to hold any values */

Obj *dupobj(Obj * o, void *ref_who, int ref_type, int line)
{
	Obj *n;
	int x;

	n = alloc_obj(o->nam_tab_mask + 1, o->str_tab_mask + 1, o->ary_len + 16);

	for (x = 0; x != o->nam_tab_mask + 1; ++x) {
		n->nam_tab[x].name = o->nam_tab[x].name;
		if (o->nam_tab[x].var) {
			n->nam_tab[x].var = alloc_var();
			dupval(&n->nam_tab[x].var->val, &o->nam_tab[x].var->val);
		}
	}

	for (x = 0; x != o->str_tab_mask + 1; ++x) {
		if (o->str_tab[x].name) {
			n->str_tab[x].name = strdup(o->str_tab[x].name);
			if (o->str_tab[x].var) {
				n->str_tab[x].var = alloc_var();
				dupval(&n->str_tab[x].var->val, &o->str_tab[x].var->val);
			}
		}
	}

	for (x = 0; x != o->ary_len; ++x)
		if (o->ary[x]) {
			n->ary[x] = alloc_var();
			dupval(&n->ary[x]->val, &o->ary[x]->val);
		}

	return n;
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

Obj *alloc_obj(int nam_size, int str_size, int ary_size)
{
	Obj *o;

	if (++alloc_count == GC_COUNT)
		collect();

	if (!free_objs) {
		struct obj_page *op = (struct obj_page *)calloc(1, sizeof(struct obj_page));
		int x;
		for (x = 0; x != sizeof(op->objs) / sizeof(Obj); ++x) {
			op->objs[x].next_free = free_objs;
			free_objs = &op->objs[x];
		}
		op->next = obj_pages;
		obj_pages = op;
		// printf("New obj page\n");
	}
	o = free_objs;
	free_objs = o->next_free;
	o->next_free = 0;

	o->ary_size = ary_size;
	o->ary_len = 0;
	o->ary = (Var **)calloc(o->ary_size, sizeof(Var *));

	o->nam_tab_mask = nam_size - 1;
	o->nam_tab = (Entry *)calloc(nam_size, sizeof(Entry));
	o->nam_tab_count = 0;

	o->str_tab_mask = str_size - 1;
	o->str_tab = (Entry *)calloc(str_size, sizeof(Entry));
	o->str_tab_count = 0;

	o->visit = 0;
	o->objno = next_obj_no++;

	protect_obj(o);
	return o;
}

static void free_obj(Obj *o)
{
	if (o->nam_tab) {
		free(o->nam_tab);
		o->nam_tab = 0;
	}
	if (o->str_tab) {
		free(o->str_tab);
		o->str_tab = 0;
	}
	if (o->ary) {
		free(o->ary);
		o->ary = 0;
	}
	o->next_free = free_objs;
	free_objs = o;
}

int mark_obj_count;

void mark_obj(Obj *o)
{
	if (o->next_free != o) {
		int x;
		o->next_free = o;
		++mark_obj_count;
		for (x = 0; x != o->ary_len; ++x)
			if (o->ary[x]) {
				mark_var(o->ary[x]);
			}
		for (x = 0; x != (o->nam_tab_mask + 1); ++x) {
			if (o->nam_tab[x].var)
				mark_var(o->nam_tab[x].var);
		}
		for (x = 0; x != (o->str_tab_mask + 1); ++x) {
			if (o->str_tab[x].var)
				mark_var(o->str_tab[x].var);
		}
	}
}

void mark_protected_objs()
{
	struct obj_protect *op;
	for (op = obj_protect_list; op; op = op->next)
		mark_obj(op->obj);
}

void sweep_objs()
{
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
}

void clear_protected_objs()
{
	struct obj_protect *op;
	while (obj_protect_list) {
		op = obj_protect_list;
		obj_protect_list = op->next;
		free(op);
	}
}
