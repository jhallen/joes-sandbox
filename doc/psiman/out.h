#ifndef _Iout
#define _Iout 1

/* Variables */

extern int cols;	/* Width of text body */
extern int rows;	/* Length of page in lines */
extern int left;	/* Left margin */
extern int right;	/* Right margin */
extern int top;		/* Top margin */
extern int bottom;	/* Bottom margin */

extern int row;		/* Current row */
extern int page;	/* Current page */
extern int paging;	/* Set for paging */

extern int device;

extern char *header;	/* Header at top of page */

void emit();
void finish();

#endif
