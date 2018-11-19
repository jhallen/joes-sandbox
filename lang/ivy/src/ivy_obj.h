/* A hash table entry */

struct entry {
	char *name;		/* String or interned string */
	Val val;		/* Value */
};

/* An object (a hash table) */

struct obj {
	Obj *next_free;
				/* Next outer scoping level is in .mom */

	Entry *nam_tab;		/* Symbol table (interned string hash table) */
	int nam_tab_mask;	/* Allocation size of nam_tab - 1 */
	int nam_tab_count;	/* No. of entries actually used */

	Entry *str_tab;		/* Non-interned string hash table */
	int str_tab_mask;	/* Allocation size of str_tab */
	int str_tab_count;	/* No. of entries actually used */

	Val *ary;		/* Automatic array of values */
	int ary_size;		/* Size of malloc block array is in */
	int ary_len;		/* One plus highest numbered member in array */

	int visit;		/* Visit flag */

	int objno;		/* Number of this object (incremented for each new one allocated) */
};


Val *get_by_symbol(Obj *t, char *name);	/* Find address of value or return NULL if it doesn't exist */
Val *set_by_symbol(Obj *t, char *name); /* Find address of value: create if it doesn't exist */

Val *get_by_string(Obj *t, char *name);
Val *set_by_string(Obj *t, char *name);

Val *get_by_number(Obj *t, long long num);
Val *set_by_number(Obj *t, long long num);

Obj *alloc_obj(int nam_size, int str_size, int ary_size);
void mark_obj(Obj *o);
void mark_protected_objs();
void sweep_objs();
void clear_protected_objs();
void protect_obj(Obj *o);

Obj *dupobj(Obj *, void *, int, int);		/* Duplicate an object */
