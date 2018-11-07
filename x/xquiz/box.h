/* Box junk */

struct lst {
	LST *r;
	LST *d;
};

struct num {
	double n;
};

struct sym {
	SYM *r;			/* Next symbol with same hash value */
	C *s;			/* Name of this symbol */
	I cnt;			/* References to this symbol */
	I prec;			/* Precidence of this symbol */
	I type;			/* Type of this symbol */
	NUM *bind;		/* Value of this symbol */
	I ass;			/* Set for right associative */
	I extra2;
};

#define ALOCSIZE ((ptrdiff_t)(4096))

/* Return the type of the box at the indicated address */

#define typ(x) (*(ptrdiff_t *)((ptrdiff_t)(x)&~(ALOCSIZE-1)))

/* BOX types */

#define tLST 0			/* A list node */
#define tNUM 1			/* A number */
#define tSYM 2			/* A symbol */
#define tEOF 3
#define tINFIX 4
#define tPREFIX 5
#define tPOSTFIX 6
#define tLPAREN 7
#define tRPAREN 8
#define tCOMMA 9
#define tUNKNOWN 10

/* LST *newlst(void);  Allocate a new LST box.  Both r and d in the new box
   will be set to zero */

LST *newlst();

/* NUM *newnum(void);  Allocate a new NUM box.  Both r and d in the new box
   will be set to zero */

NUM *newnum(double d);

/* SYM *newsym(void);  Allocate a new SYM */

SYM *newsym();

/* void discard(LST *box);  Recursively free boxes */

void discard(LST *box);
void discardnum(NUM *num);
void discardsym(SYM *sym);

/* LST *reverse(LST *box);  Reverse a list (the r part of it).  The new first
   box of the list is returned */

LST *reverse(LST *box);

/* LST *dup(LST *box);  Recusively duplicate a list */

LST *dup(LST *box);
NUM *dupnum(NUM *num);
SYM *dupsym(SYM *sym);

/* LST *subst(LST *box,SYM *for,LST *with); Subtitute a symbol with with */

LST *subst(LST *box, SYM *fr, LST *with);

/* LST *cons(LST *a,...); Construct a list */

LST *cons(LST *a,...);

LST *ncons(int sz, ...);
