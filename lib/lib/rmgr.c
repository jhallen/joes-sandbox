/* Record manager */

#include <stdio.h>
#include "blocks.h"
#include "zstr.h"
#include "rmgr.h"

Db *freedbs;			/* Free Db structures */
Table *freetables;		/* Free Table structures */
Db dbs={&dbs, &dbs};		/* List of open databases */

/* Get length of a record, including terminator */

int reclen(s)
char *s;
 {
 return mchr(s,cheor)-s+1;
 }

/* Get length of a field, including terminator */

int fieldlen(s)
char *s;
 {
 int x;
 for(x=0;s[x]&chmask;++x);
 return x+1;
 }

/* Count no. columns in a record */

int countcols(s)
char *s;
 {
 int ncols;
 for(ncols=1;*s!=cheor;++s) if(!(*s&chmask)) ++ncols;
 return ncols;
 }

/* Skip to next column */

char *nextcol(s)
char *s;
 {
 while(*s++&chmask);
 return s;
 }

/* Get address of specified column */

char *getcol(rec,col)
char *rec;
 {
 while(col--) rec=nextcol(rec);
 return rec;
 }

/* Get column symbol value */

char *getinfo(t,col,symbol)
Table *t;
char *symbol;
 {
 static char buf[btlen];
 char *s;
 /* Find column */
 s=getcol(t->colinfo,col);
 /* Find symbol */
 while(*s&chmask)
  {
  int x;
  for(x=0;s[x]==symbol[x];++x);	/* Compare symbols */
  if(s[x]=='=' && !symbol[x])	/* Do we have a match? */
   { /* We found the symbol! */
   s+=x+1;
   /* Copy value into static buffer */
   for(x=0;s[x]&chmask && s[x]!=' ';++x) buf[x]=s[x];
   buf[x]=0;
   return buf;
   }
  for(s+=x;*s&chmask && *s!=' ';++s);	/* Skip to next symbol */
  if(*s&chmask) ++s;
  }
 return 0;
 }

/* Page allocation system.  Newpage and freepage are currently designed to
 * use a single page for an allocation bitmap.  Each bit tells whether a
 * corresponding page is free (if the bit is clear) or allocated (if the bit
 * is set).
 *
 * The maximum size which this scheme can handle is based on the page size:
 *
 * Page-Size   Maximum Database Size
 *   1K             8MB
 *   2K            32MB
 *   4K           128MB
 *   8K           512MB
 *  16K             2GB
 *  32K             8GB
 *  64K            32GB
 */

/* Allocate a page */

char *newpage(db)
Db *db;
 {
 unsigned char *bitmap=db->bitmap;
 unsigned long x, y, z;
 unsigned long addr;

 /* Scan bit-map for a free page */

 for(x=0;bitmap[x]==0xFF;++x);		/* Find byte containing a zero bit */
 for(y=0,z=1;bitmap[x]&z;++y,z<<=1);	/* Find first zero bit */

 bitmap[x]|=z;				/* Mark page as allocated */

 addr=btlen*(x*8+y);			/* Address of free page */

 if(addr>=vsize(db->vfile))		/* Expand file if necessary */
  valloc(db->vfile,btlen);

 return vlock(db->vfile,addr);		/* Get physical address of page */
 }

/* Free a page */

void freepage(db,page)
Db *db;
char *page;
 {
 unsigned char *bitmap=db->bitmap;
 unsigned long addr=vofst(page);	/* Page's file offset */
 unsigned long bit=addr/btlen;		/* Bit number */
 bitmap[bit/8]&=~(1<<(bit&7));		/* Mark page as free */
 page[btid]=idfree;			/* Mark page as free */
 }

/* Intrapage search/insert/delete/update functions.
 */

/* Insert a record into a page.  Return address of record for success
 * or 0 if there was not enough space.
 */

char *simpleins(addr,rec,len)
char *addr, *rec;
 {
 int ofst=OF(addr);				/* Offset into page */
 char *page=addr-ofst;				/* Start of page */
 int alloc=GETW(page+btalloc);			/* Allocation pointer */
 if(btlen-alloc-1<len) return 0;		/* Return if no room */
 mcpy(addr+len,addr,alloc-ofst+1);		/* Move existing records */
 mcpy(addr,rec,len);				/* Insert ours */
 alloc+=len;					/* Adjust allocation pointer */
 PUTW(page+btalloc,alloc);			/* Save new allocation ptr */
 return addr;					/* Return addr of rec */
 }

/* Delete a record from a page */

char *simpledel(addr,len)
char *addr;
 {
 int ofst=OF(addr);				/* Offset into page */
 char *page=addr-ofst;				/* Start of page */
 int alloc=GETW(page+btalloc);			/* Allocation pointer */
 mcpy(addr,addr+len,alloc-(ofst+len)+1);	/* Delete record */
 alloc-=len;					/* Adjust alloc pointer */
 PUTW(page+btalloc,alloc);			/* Save new alloc pointer */
 return addr;					/* Return addr of next rec */
 }

/* Replace a record */

char *simplemod(addr,olen,rec,len)
char *addr, *rec;
 {
 int ofst=OF(addr);				/* Offset into page */
 char *page=addr-ofst;				/* Start of page */
 int alloc=GETW(page+btalloc);			/* End of data area */
 if(len>olen)					/* Insert */
  {
  if(btlen-alloc-1<len-olen) return 0;		/* Return if no room */
  mcpy(addr+len-olen,addr,alloc-ofst+1);	/* Move existing records */
  alloc+=len;
  PUTW(page+btalloc,alloc);
  }
 else if(len<olen)				/* Delete */
  {
  mcpy(addr,addr+olen-len,alloc-(ofst+olen-len)+1);	/* Move records */
  alloc-=len;
  PUTW(page+btalloc,alloc);
  }
 mcpy(addr,rec,len);				/* Copy record */
 return addr;
 }

/* Compare first field of two records (which is the "key" field for us) */

int cmp(a,b)
unsigned char *a, *b;
 {
 /* Advance over strings while they match and we don't hit a terminator */
 while(*a==*b && chmask&*a) ++a, ++b;

 if(*a&chmask || *b&chmask)	/* If either didn't land on a terminator... */
  if(*a>*b) return 1;	/* Higher valued or longer string is greater */
  else return -1;
 else return 0;		/* Both landed on a terminator, so they match */
 }

/* Search a page for data */

char *simplesrch(page,rec)
char *page, *rec;
 {
 char *a, *b, *c, *z;
 a=page+btdata;				/* Start of data */
 c=page+GETW(page+btalloc);		/* End of data */
 b=c;
 /* Binary search of variable length items */
 while(b!=(char *)(((unsigned long)a+(unsigned long)c)/2))
  {
  b=(char *)(((unsigned long)a+(unsigned long)c)/2);
  for(z=b;*(z-1)!=cheor;--z);		/* Go to beginning of record */
  if(cmp(z,rec)<0) a=b=mchr(b,cheor)+1;	/* Too low: move past it */
  else b=c=z;				/* Too high: use it as end */
  }
 return c;		/* Address of record or where it should be inserted */
 }

/* Get pointer value from an index entry */

/* table of adjustment values */

unsigned long adjtab[]=
 {
 0x00000000, 0x00000080, 0x00008000, 0x00008080,
 0x00800000, 0x00800080, 0x00808000, 0x00808080,
 0x80000000, 0x80000080, 0x80008000, 0x80008080,
 0x80800000, 0x80800080, 0x80808000, 0x80808080
 };

char *getptr(t,item)
Table *t;
unsigned char *item;
 {
 /* Skip to 2nd field */
 item=nextcol(item);
 
 /* Read adjusted pointer */
 return vlock(t->db->vfile,GETL(item)^adjtab[item[-1]&15]);
 }

/* Get pointer from specific level */

char *getptrlvl(t,level)
Table *t;
 {
 if(level==t->pathlen) return t->root;
 else return getptr(t,t->path[level]);
 }

/* Generate an index entry from a record and a file offset */

char *genptr(buf,rec,ofst)
char *buf, *rec;
unsigned long ofst;
 {
 int x;
 int adj=chbits;
 for(x=0;rec[x]&chmask;++x) buf[x]=rec[x];	/* Copy key field */

 /* Make needed adjustments and record fact of any adjustment in flag */
 if(      !(ofst&chmask)) ofst^=0x00000080, adj|=1;
 if( !(ofst&(chmask<<8))) ofst^=0x00008000, adj|=2;
 if(!(ofst&(chmask<<16))) ofst^=0x00800000, adj|=4;
 if(!(ofst&(chmask<<24))) ofst^=0x80000000, adj|=8;

 buf[x+0]=adj;			/* Write seperator/adjustment */
 PUTL(buf+x+1,ofst);		/* Write adjusted offset */
 buf[x+5]=cheor;		/* Write terminator */
 return buf;
 }

/* Get address of next entry in a node or leaf.  Don't call this if we're
   pointing past the end of the last entry */

char *next(s)
char *s;
 {
 return mchr(s,cheor)+1;
 }

/* Get address of previous entry in a node or leaf.  Don't call this if
 * we're pointing to the first entry. */

char *prev(s)
char *s;
 {
 for(--s;*(s-1)!=cheor;--s);
 return s;
 }

/* Find first entry in tree or data node */

char *first(page)
char *page;
 {
 return page+btdata;
 }

/* Find last entry */

char *last(page)
char *page;
 {
 return prev(page+GETW(page+btalloc));
 }

/* Open a table */

Table *opentable(db,oheader)
Db *db;
unsigned long oheader;
 {
 Table *t=alitem(&freetables,sizeof(Table));
 int len;
 ENQUEB(&db->tables,t);
 t->db=db;
 t->oheader=oheader;
 t->header=vlock(db->vfile,oheader);
 t->oroot=GETL(t->header+btroot);
 if(t->oroot)
  {
  char *addr;
  t->root=vlock(db->vfile,t->oroot);
  /* Compute current path length */
  for(t->pathlen=1,addr=t->root;addr[btid]==idnode;++t->pathlen)
   addr=getptr(t,addr+btdata);
  }
 else t->pathlen=0;
 len=reclen(t->header+btname);
 t->name=malloc(len);
 mcpy(t->name,t->header+btname,len);
 t->name[len-1]=0;
 t->colinfo=next(t->header+btname);
 t->ncols=countcols(t->colinfo);
 t->colnames=malloc(sizeof(char *)*t->ncols);
 for(len=0;len!=t->ncols;++len)
  t->colnames[len]=zdup(getinfo(t,len,"name"));
 return t;
 }

/* Open a database */

Db *opendb(name)
char *name;
 {
 VFILE *vfile;
 Db *db;
 for(db=dbs.next;db!=&dbs;db=db->next)
  if(!zcmp(db->name,name)) return db;
 if(vfile=vopen(name))
  {
  unsigned long oheader;
  char *addr;
  /* Open database */
  db=alitem(&freedbs,sizeof(Db));
  db->vfile=vfile;
  ENQUEB(&dbs,db);
  db->bitmap=vlock(db->vfile,0);
  db->dir=vlock(db->vfile,btlen);
  db->name=zdup(name);
  IZQUE(&db->tables);
  /* Open existing tables */
  for(addr=db->dir+btdir;oheader=GETL(addr);addr+=4)
   opentable(db,oheader);
  return db;
  }
 return 0;
 }

/* Close a database */

void closedb(db)
Db *db;
 {
 vclose(db->vfile);
 while(!QEMPTY(&db->tables))
  {
  int x;
  Table *t=db->tables.next;
  free(t->name);
  for(x=0;x!=t->ncols;++x) free(t->colnames[x]);
  free(t->colnames);
  fritem(&freetables,DEQUE(t));
  }
 free(db->name);
 fritem(&dbs,db);
 }

/* Find a table */

Table *findtable(db,name)
Db *db;
char *name;
 {
 Table *t;
 for(t=db->tables.next;t!=(Table *)&db->tables;t=t->next)
  if(!zcmp(t->name,name)) return t;
 return 0;
 }

/* Create a new database */

int mkdb(name)
char *name;
 {
 FILE *f;
 char buf[btlen*2];
 mset(buf,0,btlen*2);
 buf[0]=3;
 buf[btlen+btid]=iddir;
 if(f=fopen(name,"w"))
  {
  fwrite(buf,1,btlen*2,f);
  fclose(f);
  return 0;
  }
 else return -1;
 }

/* Create a new table */

void firstroot(t)
Table *t;
 {
 t->root=newpage(t->db);
 t->oroot=vofst(t->root);
 t->root[btid]=idleaf;
 t->root[btstop]=cheor;
 PUTL(t->root+btnext,t->oheader);
 PUTL(t->root+btprev,t->oheader);
 PUTL(t->header+btnext,t->oroot);
 PUTL(t->header+btprev,t->oroot);
 PUTL(t->root+btowner,t->oheader);
 PUTW(t->root+btalloc,btdata);
 t->root[btdata]=0;
 PUTL(t->header+btroot,t->oroot);
 t->pathlen=1;
 t->path[0]=t->root+btdata;
 }

Table *mktable(db,name,info)
Db *db;
char *name, *info;
 {
 int x;
 /* Find slot for new table in directory */
 for(x=btdir;GETL(db->dir+x);x+=4);
 if(x+4<=btlen-4)
  {
  Table *t;
  unsigned long oheader;
  int len=zlen(name);
  int ilen=zlen(info);
  char *header=newpage(db);
  oheader=vofst(header);
  PUTL(db->dir+x,oheader);
  header[btid]=idtable;
  mcpy(header+btname,name,len);
  header[btname+len]=cheor;
  mcpy(header+btname+len+1,info,ilen);
  header[btname+len+1+ilen]=cheor;
  PUTL(header+btroot,0);
  t=opentable(db,oheader);
  firstroot(t);
  return t;
  }
 else return 0;
 }

/* Advance path to item.  If we're at end of file, do nothing.  If we're
 * at the end of a node (but it's not the end of the last leaf node), we're
 * advanced to the first item of the next node.
 */

void pathfwrd(t,level)
Table *t;
 {
 int n;
 char *q;
 for(n=level;n!=t->pathlen && (!*t->path[n] || *(q=next(t->path[n])));++n);
 if(n!=t->pathlen)
  {
  t->path[n]=q;
  while(n--!=level) t->path[n]=getptr(t,t->path[n+1])+btdata;
  }
 else if(!level && *t->path[0]) t->path[0]=next(t->path[0]);
 }

/* Search for insertion point */

void dosearch(t,rec)
Table *t;
char *rec;
 {
 int n=t->pathlen;
 while(n--) t->path[n]=simplesrch(getptrlvl(t,n+1),rec);
 }

/* Search.  Return 0 if found, -1 if not found and we're at end of file or
 * 1 if not found and we're not at end of file.
 */

int search(t,rec)
Table *t;
char *rec;
 {
 dosearch(t,rec);		/* Find where we would insert record */
 if(!*t->path[0]) pathfwrd(t,0);/* If we're at end of a node, advance to next */
 if(!*t->path[0]) return -1;	/* Still at end?  Must be end of file */
 return cmp(t->path[0],rec);	/* Compare record for search result */
 }

/* Change last item of a node into a Z record */

void zlast(page)
char *page;
 {
 int alloc=GETW(page+btalloc);
 char *l;
 int ofst;
 int dlen;
 if(alloc!=btdata)
  {
  l=last(page);
  ofst=OF(l);
  dlen=alloc-ofst-7;
  mcpy(l,l+dlen,8);
  *l=0xFF;
  alloc-=dlen;
  PUTW(page+btalloc,alloc);
  }
 }

/* Split node into two */

char *doinsert();
char zrec[]={ 0xFF, cheor };

void split(t,level)
Table *t;
 {
 long ptr;
 char buf[btlen/2];
 char *newptr;
 char *page=PG(t->path[level]);			/* Page base */
 int alloc=GETW(page+btalloc);			/* End of data area */
 int ofst=prev(page+btdata+(alloc-btdata)/2)-page;	/* Split-point */
 char *new=newpage(t->db);			/* Allocate new page */
 PUTL(new+btowner,t->oheader);
 /* Doubly-linked list insert */
 ptr=vofst(page); PUTL(new+btnext,ptr);
 ptr=GETL(page+btprev); PUTL(new+btprev,ptr);
 if(ptr) newptr=vlock(t->db->vfile,ptr); else newptr=0;
 ptr=vofst(new); PUTL(page+btprev,ptr);
 if(newptr) PUTL(newptr+btnext,ptr);
 new[btid]=page[btid];				/* New page is same type */
 new[btstop]=cheor;
 mcpy(new+btdata,page+btdata,ofst-btdata);	/* Copy data to new page */
 new[ofst]=0;					/* End of data in new page */
 PUTW(new+btalloc,ofst);			/* Alloc pointer in new page */
 mcpy(page+btdata,page+ofst,alloc-ofst+1);	/* Delete from original page */
 PUTW(page+btalloc,alloc-ofst+btdata);		/* New allocation pointer */
 if(level!=t->pathlen-1)
  { /* Insert new key for new page */
  newptr=doinsert(t,level+1,genptr(buf,last(new),vofst(new)));
  }
 else
  { /* We just split root */
  char *newroot=newpage(t->db);
  int alloc;
  PUTL(newroot+btowner,t->oheader);
  newroot[btid]=idnode;
  newroot[btstop]=cheor;
  genptr(newroot+btdata,last(new),vofst(new));			/* New page */
  if(new[btid]!=idleaf) zlast(new);
  genptr(newptr=next(newroot+btdata),zrec,vofst(page));		/* Old root */
  alloc=next(newptr)-newroot;
  PUTW(newroot+btalloc,alloc);
  t->root=newroot;
  t->oroot=vofst(newroot);
  PUTL(t->header+btroot,t->oroot);
  t->path[t->pathlen++]=newroot+btdata;
  }
 /* Adjust path */
 if(OF(t->path[level])<ofst)
  /* We're in new page: newly inserted key is correct */
  t->path[level]+=new-page;
 else
  { /* We're in original page: we have to advance to parent pointer */
  pathfwrd(t,level+1);
  t->path[level]-=ofst-btdata;
  }
 }

/* Insertion function */

char *doinsert(t,level,item)
Table *t;
char *item;
 {
 int len=reclen(item);					/* Length of item */
 char *addr=simpleins(t->path[level],item,len);		/* Insert */
 if(!addr)
  {
  split(t,level);					/* Split the node */
  addr=simpleins(t->path[level],item,len);		/* Insert */
  }
 return addr;
 }

/* Insert a record */

char *insert(t,rec)
Table *t;
char *rec;
 {
 dosearch(t,rec);
 return doinsert(t,0,rec);
 }

/* Modify (replace) an item.  Note that the item might move if the
 * the node has to be split, so you need the return address */

char *domodify(t,level,item)
Table *t;
char *item;
 {
 int newlen=reclen(item);
 int oldlen=reclen(t->path[level]);
 char *addr=simplemod(t->path[level],oldlen,item,newlen);
 if(!addr)
  { /* Not enough space */
  split(t,level);
  addr=simplemod(t->path[level],oldlen,item,newlen);
  }
 return addr;
 }

/* Delete an item from its page */

void dodelete(t,level)
Table *t;
 {
 if(level==t->pathlen) firstroot(t);	/* Oops, we just deleted root */
 else
  {
  char *addr=t->path[level];		/* Address of rec */
  char *page=PG(addr);			/* Page we're deleting in */
  int ofst=addr-page;			/* Offset to deletion pt */
  int len=reclen(addr);			/* Length of rec to del */
  simpledel(addr,len);			/* Delete the item */
  if(!*addr && ofst==btdata)		/* If leaf or node became empty */
   {
   unsigned long no=GETL(page+btnext);
   unsigned long po=GETL(page+btprev);
   char *p;
   /* Doubly-linked list deletion */
   if(no) p=vlock(t->db->vfile,no), PUTL(p+btprev,po), t->path[level]=p+btdata;
   if(po)
    {
    p=vlock(t->db->vfile,po), PUTL(p+btnext,no);
    if(!no)
     if(level) t->path[level]=last(p);
     else t->path[level]=p+GETL(p+btalloc);
    }
   freepage(t->db,page);		/* Free the page */
   dodelete(t,level+1);			/* Delete entry from parent */
   }
  else if(!*addr && page[btid]==idnode)	/* If last item of node was deleted */
   zlast(page);				/* Convert last item into a Z item */
  }
 }

/* Delete record at path */

void delete(t)
Table *t;
 {
 dodelete(t,0);
 } 

/* Get address of first record */

char *firstrec(t)
Table *t;
 {
 long ofst=GETL(t->header+btnext);
 char *p=vlock(t->db->vfile,ofst+btdata);
 if(*p) return p;
 else return 0;
 }

/* Get address of next record */

char *nextrec(t,s)
Table *t;
char *s;
 {
 s=next(s);
 if(!*s)
  {
  char *page=PG(s);
  long nxt=GETL(page+btnext);
  if(nxt!=t->oheader) s=vlock(t->db->vfile,nxt+btdata);
  else s=0;
  }
 return s;
 }

/* View tree structure */

void doview(t,page,level)
Table *t;
char *page;
 {
 char *s;
 int x;
 if(page[btid]==idnode)
  for(s=page+btdata;*s;s=next(s))
   {
   for(x=0;x!=level;++x) printf(" ");
   if(s[0]==-1) printf("<End>");
   else for(x=0;s[x]&chmask && x+level!=79;++x) printf("%c",s[x]);
   printf("\n");
   doview(t,getptr(t,s),level+1);
   }
 }

void viewpath(t)
Table *t;
 {
 doview(t,t->root,0);
 }
