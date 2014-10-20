/* Convert binary file into C initializer */

#include <stdio.h>

main(argc,argv)
char *argv[];
 {
 unsigned char *buf;
 int y, x;
 FILE *f;
 int start;
 int size;
 if(argc!=4)
  {
  fprintf(stderr,"cvt file start size\n");
  return -1;
  }
 f=fopen(argv[1],"rb");
 if(!f)
  {
  fprintf(stderr,"couldn't open file\n");
  return -1;
  }
 sscanf(argv[2],"%x",&start);
 sscanf(argv[3],"%x",&size);
 fseek(f,start,SEEK_SET);
 buf=malloc(size);
 if(size!=fread(buf,1,size,f))
  {
  fprintf(stderr,"couldn't read file\n");
  return -1;
  }
 for(y=0;y!=size;y+=16)
  {
  for(x=0;x!=16;++x) printf("%3d,",buf[y+x]);
  printf("\n");
  }
 }
