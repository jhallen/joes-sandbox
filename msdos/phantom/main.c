#include <stdio.h>
#include <fcntl.h>
#include "lz78.h"

#define BFSIZE (16384-512)

unsigned char in[BFSIZE+5];
unsigned char out[BFSIZE+5];

main(argc,argv)
char *argv[];
 {
 int len;
 int x;
 char *inname=0;
 char *outname=0;
 int flg=0;
 FILE *inf, *outf;
 if(izcmp())
  {
  fprintf(stderr,"Not enough memory\n");
  exit(1);
  }
 for(x=1;x!=argc;++x)
  if(!strcmp(argv[x],"-d")) flg=1;
  else if(inname) outname=argv[x];
  else inname=argv[x];
 inf=fopen(inname,"rb");
 if(!inf)
  {
  fprintf(stderr,"Couldn't open input file\n");
  exit(1);
  }
 outf=fopen(outname,"wb");
 if(!outf)
  {
  fprintf(stderr,"Couldn't open output file\n");
  exit(1);
  }
 if(flg)
  {
  while((len=fgetc(inf))!=-1)
   {
   len=len+(fgetc(inf)<<8);
   fread(in,1,len,inf);
   len=ucmp(out,in,len);
   fwrite(out,1,len,outf);
   }
  }
 else
  while(len=fread(in,1,BFSIZE,inf))
   {
   len=cmp(out,in,len);
   fputc(len,outf);
   fputc(len>>8,outf);
   fwrite(out,1,len,outf);
   }
 fclose(inf);
 fclose(outf);
 }
