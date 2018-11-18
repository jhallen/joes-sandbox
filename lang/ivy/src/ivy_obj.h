/* A hash table entry */

struct entry {
	char *name;		/* String or interned string */
	Var *var;		/* Variable containing Value assigned to this member */
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

	Var **ary;		/* Automatic array of variables */
	int ary_size;		/* Size of malloc block array is in */
	int ary_len;		/* One plus highest numbered member in array */

	int visit;		/* Visit flag */

	int objno;		/* Number of this object (incremented for each new one allocated) */
};

Var *get_by_symbol(Obj *t, char *name);
Var *set_by_symbol(Obj *t, char *name);
Var *set_by_symbol_ref(Obj *t, char *name, Var *repl_var);

Var *get_by_string(Obj *t, char *name);
Var *set_by_string(Obj *t, char *name);
Var *set_by_string_ref(Obj *t, char *name, Var *repl_var);

Var *get_by_number(Obj *t, long long num);
Var *set_by_number(Obj *t, long long num);
Var *set_by_number_ref(Obj *t, long long num, Var *repl_var);

Obj *alloc_obj(int nam_size, int str_size, int ary_size);
void mark_obj(Obj *o);
void mark_protected_objs();
void sweep_objs();
void clear_protected_objs();
void protect_obj(Obj *o);

Var *get(Obj *, char *);	/* Get named member from an object */
Var *getn(Obj *, int);		/* Get numbered member from an object */
Var *set(Obj *, char *);	/* Set named member of an an object */
Var *setn(Obj *, int);		/* Set numbered member of an object */
Obj *dupobj(Obj *, void *, int, int);		/* Duplicate an object */
