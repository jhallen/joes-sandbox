char *ary[256];
long random();
/*
void *sbrk(long);
void free(void *);
void *malloc(long);
void *realloc(void *,long);
*/
#include "config.h"
main()
 {
 int x, z;
 char *beg=sbrk(0);
 for(x=0;x!=1024000;++x)
  switch((z=(random()&255)), (random()&7))
   {
   case 0:
   case 1:
   if(ary[z]) free(ary[z]), ary[z]=0;
   break;
   case 2:
   case 3:
   case 4:
   case 5:
   if(ary[z]) ary[z]=realloc(ary[z],(random()&1023)+1);
   else ary[z]=malloc((random()&1023)+1);
   break;
   case 6:
   case 7:
   if(ary[z]) free(ary[z]);
   ary[z]=malloc((random()&1023)+1);
   break;
   }
 printf("%d\n",(char *)sbrk(0)-beg);
 }
