/* Convert binary file into C initializer */

#include <stdio.h>

main(argc,argv)
char *argv[];
 {
 unsigned char buf[512];
 int y, x;
 FILE *f=fopen(argv[1],"rb");
 fread(buf,1,512,f);
 for(y=0;y!=512;y+=16)
  {
  for(x=0;x!=16;++x) printf("%3d,",buf[y+x]);
  printf("\n");
  }
 }
