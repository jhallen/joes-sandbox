#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include "ivy.h"

/* String allocation page */

static struct str_page {
	struct str_page *next;
	Ivy_string strs[1024];
} *str_pages;

/* String free list */

static Ivy_string *free_strs;

/* Protect from gc list */

struct ivy_str_protect *ivy_str_protect_list, *ivy_str_protect_ptr;
int ivy_str_protect_idx;

void ivy_protect_str(Ivy_string *str)
{
	if (!ivy_str_protect_ptr) {
		ivy_str_protect_list = (struct ivy_str_protect *)calloc(1, sizeof(struct ivy_str_protect));
		ivy_str_protect_ptr = ivy_str_protect_list;
		ivy_str_protect_idx = 0;
	}

	ivy_str_protect_ptr->list[ivy_str_protect_idx++] = str;

	if (ivy_str_protect_idx == IVY_OBJ_PROTECT_SIZE) {
		if (!ivy_str_protect_ptr->next)
			ivy_str_protect_ptr->next = (struct ivy_str_protect *)calloc(1, sizeof(struct ivy_str_protect));
		ivy_str_protect_ptr = ivy_str_protect_ptr->next;
		ivy_str_protect_idx = 0;
	}
}

extern int ivy_alloc_count;

Ivy_string *ivy_alloc_str(char *s, size_t len)
{
	Ivy_string *str;
	if (++ivy_alloc_count == GC_COUNT)
		ivy_collect();
	if (!free_strs) {
		struct str_page *op = (struct str_page *)calloc(1, sizeof(struct str_page));
		int x;
		for (x = 0; x != sizeof(op->strs) / sizeof(Ivy_string); ++x) {
			op->strs[x].next_free = free_strs;
			free_strs = &op->strs[x];
		}
		op->next = str_pages;
		str_pages = op;
		// printf("New str page %p\n", op);
	}
	str = free_strs;
	free_strs = str->next_free;
	str->next_free = 0;
	str->len = len;
	str->s = s;
	ivy_protect_str(str);
	return str;
}

static void free_str(Ivy_string *s)
{
	if (s->s) {
		free(s->s);
		s->s = 0;
	}
	s->next_free = free_strs;
	free_strs = s;
}

int ivy_mark_str_count;

void ivy_mark_str(Ivy_string *str)
{
	if (str->next_free != str) {
		str->next_free = str;
		++ivy_mark_str_count;
	}
}

void ivy_sweep_strs()
{
	struct str_page *sp;
	free_strs = 0;
	for (sp = str_pages; sp; sp = sp->next) {
		int x;
		for (x = 0; x != sizeof(sp->strs) / sizeof(Ivy_string); ++x) {
			if (sp->strs[x].next_free == &sp->strs[x])
				sp->strs[x].next_free = 0;
			else
				free_str(&sp->strs[x]);
		}
	}
}

void ivy_mark_protected_strs()
{
	struct ivy_str_protect *sp;
	int x;
	for (sp = ivy_str_protect_list; sp; sp = sp->next)
		if (sp == ivy_str_protect_ptr) {
			for (x = 0; x != ivy_str_protect_idx; ++x)
				ivy_mark_str(sp->list[x]);
			break;
		} else {
			for (x = 0; x != IVY_STR_PROTECT_SIZE; ++x)
				ivy_mark_str(sp->list[x]);
		}
}
