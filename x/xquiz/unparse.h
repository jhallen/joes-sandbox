void show(LST *n);
extern int mousex, mousey, basex, basey;
void unparse(DSPOBJ *obj, int x, int y, LST **l, LST **sel, int prec);
LST **findmouse(int x, int y, LST **lst, int prec);
void unparsetext(C **str, LST *n, int prec);
int unbase(LST *n, int prec);
int unheight(LST *n, int prec);


