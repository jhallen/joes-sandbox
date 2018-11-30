#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include "ivy_hash.h"
#include "ivy.h"

/* Find hash table index which should be used for name.  Indicated Entry either has matching name or NULL. */

static int slot_for_symbol(Ivy_obj *t, char *name)
{
        int x;

        for (
        	x = ivy_fib_hash(name, t->nam_tab_shift);
        	t->nam_tab[x].name != name && t->nam_tab[x].name;
        	x = ((x + 1) & t->nam_tab_mask)
	);

        return x;
}

/* Get variable bound to symbol in an object.  If no variable is bound to the symbol, return NULL. */

Ivy_val *ivy_get_by_symbol(Ivy_obj *t, char *name)
{
        int x;

	x = ivy_fib_hash(name, t->nam_tab_shift);

	for (;;) {

		if (t->nam_tab[x].name == name)
			return &t->nam_tab[x].val;

		if (!t->nam_tab[x].name)
			return NULL;
		
        	x = ((x + 1) & t->nam_tab_mask);
	}
}

/* Expand hash table to avoid clashes */

static void expand_symbol_tab(Ivy_obj *t)
{
	int x, y;
	int new_tab_size = (t->nam_tab_mask + 1) * 2;
	int new_tab_mask = new_tab_size - 1;
	int new_tab_shift = t->nam_tab_shift - 1;

	Ivy_entry *new_tab = (Ivy_entry *)calloc(new_tab_size, sizeof(Ivy_entry));

	for (y = 0; y != t->nam_tab_mask + 1; ++y) {
		if (t->nam_tab[y].name) {
			for (x = (ivy_fib_hash(t->nam_tab[y].name, new_tab_shift) & new_tab_mask); new_tab[x].name; x = ((x + 1) & new_tab_mask));
			new_tab[x].name = t->nam_tab[y].name;
			new_tab[x].val = t->nam_tab[y].val;
		}
	}

	free(t->nam_tab);

	t->nam_tab = new_tab;
	t->nam_tab_mask = new_tab_mask;
	t->nam_tab_shift = new_tab_shift;
}

/* Get variable bound to symbol in an object.  If no variable is bound to the symbol create one. */

Ivy_val *ivy_set_by_symbol(Ivy_obj *t, char *name)
{
	int x = slot_for_symbol(t, name);

	if (t->nam_tab[x].name)
		return &t->nam_tab[x].val;
	else {
		if (t->nam_tab_count == t->nam_tab_mask) {
			expand_symbol_tab(t);
			x = slot_for_symbol(t, name);
		}
		++t->nam_tab_count;
		t->nam_tab[x].name = name;
		return &t->nam_tab[x].val;
	}
}

/* Find hash table index which should be used for name.  Indicated Ivy_entry either has matching name or NULL. */

static int slot_for_string(Ivy_obj *t, char *name)
{
        int x;

        for (
        	x = (ivy_hash(name) & t->str_tab_mask);
        	t->str_tab[x].name && strcmp(t->str_tab[x].name, name);
        	x = ((x + 1) & t->str_tab_mask)
	);

        return x;
}

/* Get variable bound to symbol in an object.  If no variable is bound to the symbol, return NULL. */

Ivy_val *ivy_get_by_string(Ivy_obj *t, char *name)
{
        int x = slot_for_string(t, name);
        if (t->str_tab[x].name)
        	return &t->str_tab[x].val;
	else
		return NULL;
}

/* Expand hash table to avoid clashes */

static void expand_string_tab(Ivy_obj *t)
{
	int x, y;
	int new_tab_size = (t->str_tab_mask + 1) * 2;
	int new_tab_mask = new_tab_size - 1;

	Ivy_entry *new_tab = (Ivy_entry *)calloc(new_tab_size, sizeof(Ivy_entry));

	for (y = 0; y != t->str_tab_mask + 1; ++y) {
		if (t->str_tab[y].name) {
			for (x = (ivy_hash(t->str_tab[y].name) & new_tab_mask); new_tab[x].name; x = ((x + 1) & new_tab_mask));
			new_tab[x].name = t->str_tab[y].name;
			new_tab[x].val = t->str_tab[y].val;
		}
	}

	free(t->str_tab);

	t->str_tab = new_tab;
	t->str_tab_mask = new_tab_mask;
}

/* Get variable bound to symbol in an object.  If no variable is bound to the symbol create one. */

Ivy_val *ivy_set_by_string(Ivy_obj *t, char *name)
{
	int x = slot_for_string(t, name);

	if (t->str_tab[x].name)
		return &t->str_tab[x].val;
	else {
		if (t->str_tab_count == t->str_tab_mask) {
			expand_string_tab(t);
			x = slot_for_string(t, name);
		}
		++t->str_tab_count;
		t->str_tab[x].name = strdup(name);
		return &t->str_tab[x].val;
	}
}

/* Get variable bouond to number in an object.  If no variable bound, return NULL */

Ivy_val *ivy_get_by_number(Ivy_obj * t, long long num)
{
	if (num >= t->ary_len || num < 0)
		return NULL;
	else
		return &t->ary[num];
}

Ivy_val *ivy_set_by_number(Ivy_obj * t, long long num)
{
	if (num < 0)
		return NULL;

	if (num >= t->ary_len) {
		if (num >= t->ary_size) {
			t->ary = (Ivy_val *)realloc(t->ary, sizeof(Ivy_val) * (num + 16));
			memset(t->ary + t->ary_size, 0, sizeof(Ivy_val) * (num + 16 - t->ary_size));
			t->ary_size = num + 16;
		}
		t->ary_len = num + 1;
	}

	return &t->ary[num];
}

/* Duplicate an object non-recursively, create new variables to hold any values */

Ivy_obj *ivy_dup_obj(Ivy_obj * o, void *ref_who, int ref_type, int line)
{
	Ivy_obj *n;
	int x;

	n = ivy_alloc_obj(o->nam_tab_mask + 1, o->str_tab_mask + 1, o->ary_len + 16);

	for (x = 0; x != o->nam_tab_mask + 1; ++x) {
		n->nam_tab[x].name = o->nam_tab[x].name;
		n->nam_tab[x].val = o->nam_tab[x].val;
	}
	n->nam_tab_count = o->nam_tab_count;

	for (x = 0; x != o->str_tab_mask + 1; ++x) {
		if (o->str_tab[x].name) {
			n->str_tab[x].name = strdup(o->str_tab[x].name);
			n->str_tab[x].val = o->str_tab[x].val;
		}
	}
	n->str_tab_count = o->str_tab_count;

	for (x = 0; x != o->ary_len; ++x)
		n->ary[x] = o->ary[x];
	n->ary_len = o->ary_len;

	return n;
}

static struct obj_page {
	struct obj_page *next;
	Ivy_obj objs[128];
} *obj_pages;

static Ivy_obj *free_objs;

/* Protect from gc list */

static int next_obj_no;

struct ivy_obj_protect *ivy_obj_protect_list;
struct ivy_obj_protect *ivy_obj_protect_ptr;
int ivy_obj_protect_idx;

void ivy_protect_obj(Ivy_obj *o)
{
	if (!ivy_obj_protect_ptr) {
		ivy_obj_protect_list = (struct ivy_obj_protect *)calloc(1, sizeof(struct ivy_obj_protect));
		ivy_obj_protect_ptr = ivy_obj_protect_list;
		ivy_obj_protect_idx = 0;
	}

	ivy_obj_protect_ptr->list[ivy_obj_protect_idx++] = o;

	if (ivy_obj_protect_idx == IVY_OBJ_PROTECT_SIZE) {
		if (!ivy_obj_protect_ptr->next)
			ivy_obj_protect_ptr->next = (struct ivy_obj_protect *)calloc(1, sizeof(struct ivy_obj_protect));
		ivy_obj_protect_ptr = ivy_obj_protect_ptr->next;
		ivy_obj_protect_idx = 0;
	}
}

Ivy_obj *ivy_alloc_obj(int nam_size, int str_size, int ary_size)
{
	Ivy_obj *o;

	if (++ivy_alloc_count == GC_COUNT)
		ivy_collect();

	if (!free_objs) {
		struct obj_page *op = (struct obj_page *)calloc(1, sizeof(struct obj_page));
		int x;
		for (x = 0; x != sizeof(op->objs) / sizeof(Ivy_obj); ++x) {
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
	o->ary = (Ivy_val *)calloc(o->ary_size, sizeof(Ivy_val));

	o->nam_tab_shift = 64 - 2; // shift is (64 - log2(size))- what fib_hash() needs
	o->nam_tab_mask = (1 << (64 - o->nam_tab_shift)) - 1;
	o->nam_tab = (Ivy_entry *)calloc(o->nam_tab_mask + 1, sizeof(Ivy_entry));
	o->nam_tab_count = 0;

	o->str_tab_mask = str_size - 1;
	o->str_tab = (Ivy_entry *)calloc(str_size, sizeof(Ivy_entry));
	o->str_tab_count = 0;

	o->visit = 0;
	o->objno = next_obj_no++;

	ivy_protect_obj(o);
	return o;
}

static void free_obj(Ivy_obj *o)
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

int ivy_mark_obj_count;

void ivy_mark_obj(Ivy_obj *o)
{
	if (o->next_free != o) {
		int x;
		o->next_free = o;
		++ivy_mark_obj_count;
		for (x = 0; x != o->ary_len; ++x)
			ivy_mark_val(&o->ary[x]);
		for (x = 0; x != (o->nam_tab_mask + 1); ++x) {
			if (o->nam_tab[x].name)
				ivy_mark_val(&o->nam_tab[x].val);
		}
		for (x = 0; x != (o->str_tab_mask + 1); ++x) {
			if (o->str_tab[x].name)
				ivy_mark_val(&o->str_tab[x].val);
		}
	}
}

void ivy_mark_protected_objs()
{
	struct ivy_obj_protect *op;
	int x;
	for (op = ivy_obj_protect_list; op; op = op->next)
		if (op == ivy_obj_protect_ptr) {
			for (x = 0; x != ivy_obj_protect_idx; ++x)
				ivy_mark_obj(op->list[x]);
				break;
		} else
			for (x = 0; x != IVY_OBJ_PROTECT_SIZE; ++x)
				ivy_mark_obj(op->list[x]);
}

void ivy_sweep_objs()
{
	struct obj_page *op;
	free_objs = 0;
	for (op = obj_pages; op; op = op->next) {
		int x;
		for (x = 0; x != sizeof(op->objs) / sizeof(Ivy_obj); ++x) {
			if (op->objs[x].next_free == &op->objs[x])
				op->objs[x].next_free = 0;
			else
				free_obj(&op->objs[x]);
		}
	}
}
