/* A variable */

struct var {
	Var *next_free;
	Val val;		/* The value of the variable */
};

Var *alloc_var();
void mark_var(Var *var);
void mark_protected_vars();
void sweep_vars();
void clear_protected_vars();
