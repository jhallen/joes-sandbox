/* A function in its context */

struct fun {
	Fun *next_free;
	Func *f;		/* Actual function */
	Obj *scope;		/* Context function was created in */
	Val *init_vals;		/* Initialization values */
	int x;			/* The init value we're up to */
};

Fun *alloc_fun(Func *func, Obj *scope);
void mark_fun(Fun *str);
void mark_protected_funs();
void sweep_funs();
void clear_protected_funs();
void protect_fun(Fun *fun);
