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

OBJ *mkobj();
int nwords();
void setattr();
int *resize();
int isletter();
int len();
void append();
OBJ *merge();

#endif
