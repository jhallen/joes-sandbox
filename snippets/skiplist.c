/* Skip-list library */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct skipnode {
	int key;		/* Key for value */
	int nptrs;		/* No. node pointers in this node */
	void *value;		/* Value */
	struct skipnode *ptrs[1];	/* Node pointers */
};

/* Max number of pointers in a skiplist node */
#define SKIPDPTH 16

struct skipnode *top;		/* Skip list root node */
struct skipnode nil;		/* Ending skiplist node */
struct skipnode *update[SKIPDPTH];	/* Search path record */

void init()
{
	int x;
	nil.key = 0x7FFFFFFF;
	nil.nptrs = 0;
	nil.value = 0;
	nil.ptrs[0] = &nil;
	top = (struct skipnode *) malloc(sizeof(struct skipnode) +
					 SKIPDPTH *
					 sizeof(struct skipnode *));
	top->key = 0;
	top->nptrs = 0;
	top->value = 0;
	for (x = 0; x != SKIPDPTH; ++x)
		top->ptrs[x] = &nil;
}

struct skipnode *find(int key)
{
	int y;
	struct skipnode *t = top;
	for (y = top->nptrs; --y >= 0;) {
		while (t->ptrs[y]->key < key)
			t = t->ptrs[y];
		update[y] = t;
	}
	return t->ptrs[0];
}

void showall()
{
	struct skipnode *t = top->ptrs[0];
	while (t != &nil) {
		printf("%s %d %d\n", (char *)t->value, t->key, t->nptrs);
		t = t->ptrs[0];
	}
}

void insert(int key, void *value)
{
	struct skipnode *t;
	int level, x;
	find(key);
	for (level = 1; random() < 0x29000000; ++level);
	if (level > (SKIPDPTH - 1))
		level = SKIPDPTH - 1;
	t = (struct skipnode *) malloc(sizeof(struct skipnode) +
				       level * sizeof(struct skipnode *));
	while (level > top->nptrs)
		update[top->nptrs++] = top;
	t->nptrs = level;
	for (x = 0; x != level; ++x)
		t->ptrs[x] = update[x]->ptrs[x], update[x]->ptrs[x] = t;
	t->key = key;
	t->value = value;
}

void delete(struct skipnode *t)
{
	int x;
	for (x = 0; x != top->nptrs && update[x]->ptrs[x] == t; ++x)
		update[x]->ptrs[x] = t->ptrs[x];
	while (top->nptrs && top->ptrs[top->nptrs - 1] == &nil)
		--top->nptrs;
	free(t);
}

void delete_range(int key1, int key2)
{
	find(key1);
	while (update[0]->ptrs[0]->key < key2) {
		printf("%d %s\n", update[0]->ptrs[0]->key,
		       (char *)update[0]->ptrs[0]->value);
		delete(update[0]->ptrs[0]);
	}
}

int main()
{
	char buf[256];
	int key, key1, key2;
	struct skipnode *t;
	init();
	while (gets(buf))
		switch (buf[0]) {
		case 'q':
			return 0;

		case 'a':
			sscanf(buf + 2, "%d", &key);
			insert(key, strdup(buf + 2));
			break;

		case 'f':
			sscanf(buf + 2, "%d", &key);
			t = find(key);
			if (t && t->value)
				puts(t->value);
			else
				printf("Not found\n");
			break;

		case 'd':
			sscanf(buf + 2, "%d", &key);
			t = find(key);
			if (t && t->value && t->key == key)
				delete(t);
			else
				printf("Not found\n");
			break;
		case 's':
			showall();
			break;
		case 'r':
			sscanf(buf + 2, "%d %d", &key1, &key2);
			delete_range(key1, key2);
			break;
		}
        return 0;
}
