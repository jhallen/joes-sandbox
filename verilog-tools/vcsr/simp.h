// Simplify an expression (hopefully to a single 'NUM')
struct expr *simp(struct param *params,struct expr *e);

// Convert 'NUM' to integer
int val(struct expr *e);

// Extract field out of a constant
struct expr *slice(struct expr *r,int start,int size);
