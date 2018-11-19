/* A function in its context */

struct closure {
	Closure *next_free;
	Func *f;		/* Actual function */
	Obj *scope;		/* Context function was created in */
	Val *init_vals;		/* Initialization values */
	int x;			/* The init value we're up to */
};

Closure *alloc_closure(Func *func, Obj *scope);
void mark_closure(Closure *str);
void mark_protected_closures();
void sweep_closures();
void clear_protected_closures();
void protect_closure(Closure *closure);
