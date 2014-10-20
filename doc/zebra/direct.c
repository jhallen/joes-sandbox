#include <stdio.h>
#include <string.h>
#include "zstr.h"

struct
 {
 char *code;
 char *plant;
 char *variety;
 char *output;
 } index[1024];
int indexi=0; 

main(argc,argv)
char *argv[];
 {
 char buf[4096];
 char *ptr[5];
 FILE *f;
 int i;
 int w=0;
 sprintf(buf,"form <%s",argv[1]);
 f=popen(buf,"r");
 while(fgets(buf,4095,f))
  {
  fields(buf,ptr,':');
  index[indexi].code=strdup(ptr[0]);
  fprintf(stderr,"%s ",index[indexi].code);
  w+=strlen(index[indexi].code)+1;
  if(w>=70) fprintf(stderr,"\n"), w=0;
  index[indexi].plant=strdup(ptr[1]);
  index[indexi].variety=strdup(ptr[2]);
  fgets(buf,4095,f);
  index[indexi].output=strdup(buf);
  ++indexi;
  }
 if(w) fprintf(stderr,"\n");
 pclose(f);
 tt:
 fprintf(stderr,"Icode: ");
 if(!gets(buf)) return;
 if(!buf[0]) return;
 for(i=0;i!=indexi;++i)
  if(!strcmp(buf,index[i].code))
   {
   int n;
   fprintf(stderr,"%s %s\n",index[i].plant,index[i].variety);
   t1:
   fprintf(stderr,"Amount: ");
   if(!gets(buf)) return;
   n=0;
   sscanf(buf,"%d",&n);
   if(n)
    {
    printf("^XA^PQ%d",n);
    printf("%s",index[i].output);
    goto t1;
    }
   else goto tt;
   }
 fprintf(stderr,"Not found\n");
 goto tt;
 }
