/* Configuration file */

#ifndef _Iconfig
#define _Iconfig 1

#define BASE 0x10000

/* Integer size quantities
 * MAXINT Maximum signed integer
 * ISIZ   Number of chars in an int
 * SHFT   LOG2 of ISIZ
 */

#define MAXINT 0x7FFFFFFF	/* Maximum signed int */
#define ISIZ 4			/* Number of chars per int */
#define SHFT 2			/* LOG2 of above number */

#define physical(a) ((long)(a))	/* Convert to a linear address */
#define normsub(a,b) ((a)-(b))
#define normalize(a) (a)	/* Normalize a segmented address */

#define BITS 8			/* Number of bits in a char */
#define MAXLONG 0x7FFFFFFF

/* Uncomment the following line if your compiler has trouble with void */
/* #define void int */

#ifndef NULL
#define NULL ((void *)0)
#endif

/* These are for optimizing blocks.c */
/* #define AUTOINC */	/* Define this if CPU can autoincrement faster than
			   it can do [reg+offset] addressing */
/* #define ALIGNED */	/* Define this if CPU can access unaligned ints */
			/* (tries to align ints even if defined) */

/* System calls we use */
char *getenv();
char *getcwd();
long time();
/*
int chdir();
int creat();
int open();
int close();
int read();
int write();
int lseek();
*/

#endif
