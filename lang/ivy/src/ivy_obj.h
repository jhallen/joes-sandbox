/* A hash table entry */

struct ivy_entry {
	char *name;		/* String or interned string */
	Ivy_val val;		/* Value */
};

/* An object (a hash table) */

struct ivy_obj {
	Ivy_obj *next_free;
				/* Next outer scoping level is in .mom */

	Ivy_entry *nam_tab;		/* Symbol table (interned string hash table) */
	int nam_tab_mask;	/* Allocation size of nam_tab - 1 */
	int nam_tab_shift;	/* Log2 of nam_tab size */
	int nam_tab_count;	/* No. of entries actually used */

	Ivy_entry *str_tab;		/* Non-interned string hash table */
	int str_tab_mask;	/* Allocation size of str_tab */
	int str_tab_count;	/* No. of entries actually used */

	Ivy_val *ary;		/* Automatic array of values */
	int ary_size;		/* Size of malloc block array is in */
	int ary_len;		/* One plus highest numbered member in array */

	int visit;		/* Visit flag */

	int objno;		/* Number of this object (incremented for each new one allocated) */
};


Ivy_val *ivy_get_by_symbol(Ivy_obj *t, char *name);	/* Find address of value or return NULL if it doesn't exist */
Ivy_val *ivy_set_by_symbol(Ivy_obj *t, char *name); /* Find address of value: create if it doesn't exist */

Ivy_val *ivy_get_by_string(Ivy_obj *t, char *name);
Ivy_val *ivy_set_by_string(Ivy_obj *t, char *name);

Ivy_val *ivy_get_by_number(Ivy_obj *t, long long num);
Ivy_val *ivy_set_by_number(Ivy_obj *t, long long num);

Ivy_obj *ivy_alloc_obj(int nam_size, int str_size, int ary_size);
void ivy_mark_obj(Ivy_obj *o);
void ivy_mark_protected_objs();
void ivy_sweep_objs();

#define IVY_OBJ_PROTECT_SIZE 128

extern struct ivy_obj_protect {
	struct ivy_obj_protect *next;
	Ivy_obj *list[IVY_OBJ_PROTECT_SIZE];
} *ivy_obj_protect_list;

struct ivy_obj_protect *ivy_obj_protect_ptr;
extern int ivy_obj_protect_idx;

static inline void ivy_clear_protected_objs()
{
	ivy_obj_protect_ptr = ivy_obj_protect_list;
	ivy_obj_protect_idx = 0;
}

void ivy_protect_obj(Ivy_obj *o);

Ivy_obj *ivy_dup_obj(Ivy_obj *, void *, int, int);		/* Duplicate an object */
