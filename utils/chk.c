#include <stdio.h>

/* Calculate Xilinx checksum on binary file */

unsigned char buf[1024*1024];

int main(int argc,char *argv[])
 {
 FILE *f;
 int size=0;
 int x;
 int checksum=0;
 if(argc!=2)
  {
  fprintf(stderr,"chk binary_file   - compute checksum of Xilinx binary file\n");
  return -1;
  }
 f=fopen(argv[1],"r");
 for(x=0;x!=1024*1024;++x)
  buf[x]=0xFF;
 if(f) size=fread(buf,1,1024*1024,f);
 else
  {
  fprintf(stderr,"chk: Couldn't open file %s\n",argv[1]);
  return -1;
  }
 for(x=0;x!=1024*1024;++x) checksum+=buf[x];
 printf("chk: size=%d bytes\n",size);
 printf("chk: checksum=%x\n",checksum&0xFFFF);
 return 0;
 }
