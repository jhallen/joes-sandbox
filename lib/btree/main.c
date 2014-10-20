/* Btree example routines */

#include "btree.h"

/* Junk */

edupd(){}
dostaupd(){}
ttsig(){}
int mid;
char **mainenv;

/* Comparison function */

int cmp(obj,a,az,b,bz)
void *obj;
char *a, *b;
 {
 int x;
 for(x=0;x!=az && x!=bz;++x)
  if(a[x]>b[x]) return 1;
  else if(a[x]<b[x]) return -1;
 if(az>bz) return 1;
 else if(az<bz) return -1;
 return 0;
 }

main(argc,argv)
char *argv[];
 {
 BTREE *f=0;
 char nam[100];
 char buf[1024];
 nam[0]=0;
 mainenv=argv;
 printf("Btree demo program.  Type '?' for help\n");
 loop: printf("%s>",nam);
 if(!gets(buf))
  {
  if(f) btclose(f);
  return;
  }
 switch(buf[0])
  {
  case 'e': /* Open a btree file */
  if(!buf[1] || !buf[2])
   {
   printf("Syntax error\n");
   break;
   }
  if(f) btclose(f), nam[0]=0;
  f=btopen(buf+2);
  if(f) printf("File %s open\n",buf+2), strcpy(nam,buf+2);
  else printf("Couldn't open file.  Create it first with 'c'\n");
  break;

  case 'c': /* Create file */
  if(!buf[1] || !buf[2])
   {
   printf("Syntax error\n");
   break;
   }
  btcreat(buf+2,"","");
  printf("File %s created\n",buf+2);
  break;

  case 'x': /* Exit */
  case 'q':
  if(f) btclose(f);
  return;

  case 'i': /* Insert */
  if(!buf[1] || !buf[2])
   {
   printf("Syntax error\n");
   break;
   }
  if(!f)
   {
   printf("No file open\n");
   break;
   }
  btsearch(f,cmp,NULL,buf+2,strlen(buf+2));
  btins(f,buf+2,strlen(buf+2));
  goto show;

  case 'd': /* Delete */
  if(!f)
   {
   printf("No file open\n");
   break;
   }
  if(btdel(f)) goto show;
  else printf("End of file\n");
  break;

  case 'f': /* Find */
  if(!buf[1] || !buf[2])
   {
   printf("Syntax error\n");
   break;
   }
  if(!f)
   {
   printf("No file open\n");
   break;
   }
  else
   {
   if(btsearch(f,cmp,NULL,buf+2,strlen(buf+2))) printf("Record found\n");
   goto show;
   }  

  case 'p': /* Print previous record */
  if(!f)
   {
   printf("No file open\n");
   break;
   }
  if(btbkwd(f)) goto show;
  else printf("Beginning of file\n");
  break;

  case 'n': /* Print next record */
  if(!f)
   {
   printf("No file open\n");
   break;
   }
  if(btfwrd(f)) goto show;
  else printf("End of file\n");
  break;

  case 't': /* Print current record */
  show:
  if(!f)
   {
   printf("No file open\n");
   break;
   }
   {
   char *s=btread(f,NULL);
   if(s) printf("-->%s\n",s), vsrm(s);
   else printf("-->End of file\n");
   }
  break;

  case 'u': /* Goto top of file */
  if(!f)
   {
   printf("No file open\n");
   break;
   }
  btfirst(f);
  goto show;
  
  case 'v': /* Goto end of file */
  if(!f)
   {
   printf("No file open\n");
   break;
   }
  btlast(f);
  goto show;

  case '?': /* Help */
  case 'h':
  printf("\
Commands:\n\
\n\
 c name		Create a database file\n\
 e name		Edit/Open a database file\n\
 q		Quit\n\
\n\
 u		Go to first record of file\n\
 v		Go to last record of file\n\
 t		Show current record\n\
 p		Go to previous record\n\
 n		Go to next record\n\
\n\
 i text		Insert a record\n\
 d		Delete current record\n\
 f text		Find a record\n\
\n\
");
  }
 goto loop;
 }
