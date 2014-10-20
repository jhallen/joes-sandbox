/* Record manager */

#ifndef _Irmgr
#define _Irmgr 1

#include "queue.h"
#include "vfile.h"

typedef struct table Table;
typedef struct database Db;

struct database
 {
 QITEM(Db);		/* Doubly linked list of databases */
 QBASE(Table) tables;	/* Doubly linked list of tables in this database */
 VFILE *vfile;			/* VFILE for open database */
 char *name;			/* Name of database */
 char *dir;			/* Address of directory page */
 char *bitmap;			/* Address of bit-map page */
 };

struct table
 {
 QITEM(Table);	/* Doubly linked list of tables */
 char *name;			/* Name of this table */
 Db *db;			/* Database we're part of */
 char *header;			/* Memory address of header page */
 unsigned long oheader;		/* File offset to header page */
 char *root;			/* Memory address of root page */
 unsigned long oroot;		/* File offset to root page */
 int ncols;			/* No. columns */
 char **colnames;		/* Column names */
 char *colinfo;			/* Column definition record */
 char *path[32];		/* Path to current record */
 int pathlen;			/* Tree depth */
 };

/* Macros for reading and writing words (16-bits) and longs (32-bits) to
 * files in a portable way */

#define GETL(y) \
 ( \
  (((unsigned long)((unsigned char *)(y))[0]))+ \
  (((unsigned long)((unsigned char *)(y))[1])<<8)+ \
  (((unsigned long)((unsigned char *)(y))[2])<<16)+ \
  (((unsigned long)((unsigned char *)(y))[3])<<24) \
 )

#define GETW(y) \
 ( \
  (((unsigned)((unsigned char *)(y))[0]))+ \
  (((unsigned)((unsigned char *)(y))[1])<<8) \
 )

#define PUTL(x,y) \
 ( \
  ((unsigned char *)(x))[0]=((y)), \
  ((unsigned char *)(x))[1]=((y)>>8), \
  ((unsigned char *)(x))[2]=((y)>>16), \
  ((unsigned char *)(x))[3]=((y)>>24) \
 )

#define PUTW(x,y) \
 ( \
  ((unsigned char *)(x))[0]=((y)), \
  ((unsigned char *)(x))[1]=((y)>>8) \
 )

/* Offsets into different types of pages */

/* Common to all pages */
#define btid 14			/* char: Page type code */
#define btlen 4096		/* Page size */

/* Node and leaf pages */
#define btowner 0		/* Long: address of table header page */
#define btnext 4		/* Long: address of next page in list */
#define btprev 8		/* Long: address of previous page in list */
#define btalloc 12		/* Word: offset to free space in page */
#define btstop 15		/* Offset to stop character (cheor) */
#define btdata 16		/* Start of data area of page */

/* Table header pages */
#define btroot 0		/* Long: address of root page */
#define btname 16		/* Start of table name string */

/* Directory page */
#define btdir 16		/* Start of directory entries */

/* Page type I.D.s */
#define idleaf 1		/* A leaf page */
#define idnode 2		/* A node page */
#define idfree 3		/* A free page */
#define idtable 4		/* A table header page */
#define iddir 5			/* The database directory page */

#define cheor '\n'		/* End of record character */
#define chfield '\t'		/* Field seperator character */
#define chmask 0xE0		/* Field terminator detection mask */
#define chbits 0x10		/* Character base for 4-bit encoder */

/* Macro for accessing page information */

/* Get base address of page from any address within the page */
#define PG(x) ((char *)((unsigned long)(x)&~(btlen-1)))

/* Get offset from page base to given address */
#define OF(x) ((unsigned long)(x)&(btlen-1))

/* Functions */

int mkdb();			/* Create a new database */
Db *opendb();			/* Open a database */
void closedb();			/* Close a database */

Table *mktable();		/* Create a new table in the database */
Table *findtable();		/* Find table */
void rmtable();			/* Delete a table from the database */

int search();			/* Search for a record or insertion point */
char *insert();			/* Insert a record */
void modify();			/* Replace record at path */
void delete();			/* Delete record at path */

char  *firstrec();		/* Get address of first record of table */
char *nextrec();		/* Get address of next record */

#endif
