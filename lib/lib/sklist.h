/*
 *	Generic skip-list primitives
 *
 *	Copyright
 *		(C) 2006 Joseph H. Allen
 */

/* Include this in your structure, like this:
 *
 * struct my_struct {
 *         Skip_link(struct my_struct) link; // Has to be called link
 *         my_key_type my_key;
 *         my_value_type my_value;
 * };
 */

#define Skip_link(type_t) struct { \
	type_t **next; \
}

/* Set this to log2(max nodes) */

#define MAX_NODE_HEIGHT 20

/* Base of a skip-list.  Nothing in the structure is malloced, but the
 * the 'next' array in Skip_link (in each item) is malloced, so all nodes
 * should be deleted before Skip_base goes out of scope.
 *
 * It is OK to pass a Skip_base by value (the items are of course not
 * duplicated), but the current path becomes invalid.
 */

#define Skip_base(type_t) struct { \
	type_t *root[MAX_NODE_HEIGHT]; /* Pointers to first node of each height */ \
	type_t **next; /* Points to root- meant to look like 'next' in Skip_link. */ \
	type_t *path[MAX_NODE_HEIGHT]; /* Path to nodes which point to current item */ \
	int max_node_height; /* Height of tallest node */ \
}

/* Get address of non-existant node before first node (root node), given a
   base_p.  There is no such node, but root->link.next[*] accesses the root
   array in base.
   */

#define skip_base_root(type_t, base_p) \
  (type_t *)((char *)&base_p->next - ((char *)&((type_t *)0)->link.next - (char *)((type_t *)0)))

/* Initialize a skip_list base */

#define iz_skip(type_t, base_p) do { \
	int x; \
	for (x = 0; x != MAX_NODE_HEIGHT; ++x) \
		base_p->root[x] = 0; \
	base_p->max_node_height = 0; \
	base_p->next = base_p->root; \
} while (0)

/* Search for first item with key >= key given in expression.  Leaves
 * path in _base_p pointing to nodes which point to found node.
 *
 * Expression should be in terms of variable a, which is a pointer
 * to a node.  The expression should return true if a's key is less
 * than the search key.
 *
 * For example:
 *
 * skip_find(struct my_struct, my_list, (a->key < 10));
 * skip_current(my_list) now returns address of first node key >= 10
 */

#define skip_find(_type_t, _base_p, _a_lt_key_e) do { \
	int _y; \
	_type_t *_t = skip_base_root(_type_t, _base_p); \
	_base_p->next = _base_p->root; \
	for (_y = _base_p->max_node_height; --_y >= 0;) { \
		_type_t *a; \
		while ((a = _t->link.next[_y]) && (_a_lt_key_e)) \
			_t = a; \
		_base_p->path[_y] = _t; \
	} \
} while (0)

/* Set path to first item in list */

#define skip_find_first(type_t, base_p) do { \
	int y; \
	type_t *t = skip_base_root(type_t, base_p); \
	base_p->next = base_p->root; \
	for (y = base_p->max_node_height; --y >= 0;) { \
		base_p->path[y] = t; \
	} \
} while (0)

/* Get current item (can be NULL) */

#define skip_current(base_p) \
	((base_p)->max_node_height ? (base_p)->path[0]->link.next[0] : 0)

/* Get first item in list (can be NULL) */

#define skip_first(base_p) ((base_p)->root[0])

/* Get next node after this one (can be NULL) */

#define skip_next(item_p) (item_p)->link.next[0]

/* Insert a node (item_p) at the current path: use malloc  */

#define skip_insert(type_t, base_p, item_p) do { \
	int x; \
	int level; \
	/* Choose height of new node */ \
	for (level = 1; level < MAX_NODE_HEIGHT && random() < 0x29000000; ++level); \
	/* Allocate and install vector of node pointers into item */ \
	item_p->link.next = (type_t **)malloc(sizeof(type_t *) * level); \
	/* Increase height of skiplist if this node is the largest */ \
	while (level > base_p->max_node_height) \
		base_p->path[base_p->max_node_height++] = skip_base_root(type_t, base_p); \
	/* Insert node */ \
	for (x = 0; x != level; ++x) { \
		item_p->link.next[x] = base_p->path[x]->link.next[x]; \
		base_p->path[x]->link.next[x] = item_p; \
	} \
} while (0)

/* Insert a node (item_p) at the current path: use ob_malloc  */

#define skip_insert_ob(ob, type_t, base_p, item_p) do { \
	int x; \
	int level; \
	/* Choose height of new node */ \
	for (level = 1; level < MAX_NODE_HEIGHT && random() < 0x29000000; ++level); \
	/* Allocate and install vector of node pointers into item */ \
	item_p->link.next = (type_t **)ob_malloc(ob, sizeof(type_t *) * level); \
	/* Increase height of skiplist if this node is the largest */ \
	while (level > base_p->max_node_height) \
		base_p->path[base_p->max_node_height++] = skip_base_root(type_t, base_p); \
	/* Insert node */ \
	for (x = 0; x != level; ++x) { \
		item_p->link.next[x] = base_p->path[x]->link.next[x]; \
		base_p->path[x]->link.next[x] = item_p; \
	} \
} while (0)

/* Remove node at current path. */

#define skip_delete(type_t, base_p) do { \
	int x; \
	type_t *t = base_p->next[0]->link.next[0]; \
	/* Remove node from list */ \
	for (x = 0; x != base_p->max_node_height && base_p->path[x]->link.next[x] == t; ++x) \
		base_p->path[x]->link.next[x] = t->link.next[x]; \
	/* Reduce height of tree if this was tallest node */ \
	while (base_p->max_node_height && !base_p->next[base_p->max_node_height - 1]) \
		--base_p->max_node_height; \
	/* Free pointers */ \
	free(t->link.next); \
} while (0)

/* Remove all remaining nodes which have keys < key in expression */

#define skip_delete_to(_type_t, _base_p, _a_lt_key_e) do { \
	_type_t *a; \
	while ((a = skip_current(_base_p)) && (_a_lt_key_e)) \
		skip_delete(_type_t, _base_p); \
} while (0)

/* Remove all remaining nodes */

#define skip_delete_rest(type_t, base_p) do { \
	type_t *a; \
	while ((a = skip_current(base_p)) \
		skip_delete(type_t, base_p); \
} while (0)
