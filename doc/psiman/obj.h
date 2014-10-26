#ifndef _Iobj
#define _Iobj 1

typedef struct object OBJ;

/* An object */

struct object
 {
 int n;			/* Number of globs */
 int ind;		/* Indentation of last line */
 int *glob[128];	/* The globs */
 int start[128];	/* Starting column of each glob */
 int siz[128];		/* Malloc size of each glob */
 int len[128];		/* Length of each glob */
 };

/* Character attributes */

#define BOLD 0x100
#define ITAL 0x200

OBJ *mkobj(void);
void rmobj(OBJ *obj);
int nwords(int *text);
void setattr(int *text, int attr);
int *resize(int *ary,int len, int *siz);
int isletter(int x);
int len(int *ary);
void append(int **ary, int *len, int *siz, int c);
OBJ *merge(OBJ *a,OBJ *b);

#endif
