#include <stdio.h>
#include <stdlib.h>

/* Convert .hex file to binary file.
   by: Joe Allen 2/18/00

   A .hex file is an ASCII text representation of binary data.  It contains
   a single line in the following format:

     HHHHHH

   where each HH pair is an ascii hex byte.

   The line is newline terminated.

*/

int hexval(int c)
 {
 if(c>='0' && c<='9') return c-'0';
 else if(c>='a' && c<='f') return c-'a'+10;
 else if(c>='A' && c<='F') return c-'A'+10;
 else fprintf(stderr,"hex: bad hex digit\n"), exit(-1);
 }

int main(int argc,char *argv[])
 {
 FILE *f, *g;
 int size=0;
 int c, d;
 int checksum=0;
 int x;

 if(argc!=3)
  {
  fprintf(stderr,"hex input.hex output.bin\n");
  return -1;
  }

 f=fopen(argv[1],"r");
 if(!f)
  {
  fprintf(stderr,"hex: couldn't open .hex file %s\n",argv[1]);
  return -1;
  }

 g=fopen(argv[2],"w");
 if(!g)
  {
  fprintf(stderr,"hex: couldn't open .bin file %s\n",argv[2]);
  return -1;
  }

 while((c=fgetc(f))!=-1)  
  if(c=='\n') ; /* Ignore newline */
  else if(c=='\r') ; /* Ignore carriage return */
  else if((d=fgetc(f))==-1)
   fprintf(stderr,"hex: odd number of hex digits?\n"), exit(-1);
  else if(fputc(hexval(c)*16+hexval(d),g)==-1)
   fprintf(stderr,"hex: write error\n"), exit(-1);
  else
   {
   checksum+=hexval(c)*16+hexval(d);
   ++size;
   }

 for(x=size;x!=1024*1024;++x)
  checksum+=0xFF;

 if(fclose(g))
  fprintf(stderr,"hex: close error\n"), exit(-1);
 else
  {
  fprintf(stderr,"hex: wrote %d bytes\n",size);
  fprintf(stderr,"hex: checksum=%x\n",checksum&0xFFFF);
  }

 return 0;
 }
