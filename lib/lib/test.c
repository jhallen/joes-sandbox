#include <stdio.h>
#include "rmgr.h"

Db *testdb;
Table *it;

main()
 {
 FILE *f;
 char buf[80];
 char *s;

 mkdb("testdb");
 testdb=opendb("testdb");
 it=mktable(testdb,"it","name=id");

 f=fopen("inserts","r");
 while(fgets(buf,79,f)) insert(it,buf);
 fclose(f);

 f=fopen("deletes","r");
 while(fgets(buf,79,f))
  if(!search(it,buf)) delete(it);
  else printf("Couldn't find %s",buf);
 fclose(f);

 for(s=firstrec(it);s;s=nextrec(it,s))
  {
  int x;
  for(x=0;s[x]!='\n';++x) putchar(s[x]);
  putchar('\n');
  }

 viewpath(it);
 closedb(testdb);
 }
