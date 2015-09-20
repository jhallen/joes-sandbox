extern I err;
LST *parse(int prec);
void genlist(LST *n, SYM **ll, I *nn);
double ev(LST *n);
LST *simplify(LST *n, int r);
