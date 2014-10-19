#include <stdio.h>
#include <stdlib.h>

int main(int argc,char *argv[])
 {
 FILE *f, *g;
 unsigned char *image;
 int size;
 int x;
 int col;
 if(argc!=3)
  {
  fprintf(stderr,"bintoc input.bin output.c\n");
  return -1;
  }
 image=malloc(1024*1024);
 for(x=0;x!=1024*1024;++x) image[x]=0xFF;

 f=fopen(argv[1],"r");
 if(!f)
  {
  fprintf(stderr,"bintoc: couldn't open .bin file %s\n",argv[1]);
  return -1;
  }

 size=fread(image,1,1024*1024,f);
 fclose(f);
 g=fopen(argv[2],"w");
 if(!g)
  {
  fprintf(stderr,"bintoc: couldn't open .c file %s\n",argv[2]);
  return -1;
  }

 fprintf(g,"\
/* FPGA Configuration bitstream */\n\
\n\
int fpgasize=%d; /* Size in 32-bit words */\n\
unsigned int fpga[]= {\n\
",(size+3)/4);
 col=0;
 for(x=0;x<size;x+=4)
  {
  if(!col) fprintf(g,"  ");
  fprintf(g,"0x%8.8x",(image[x+3]<<24)+(image[x+2]<<16)+(image[x+1]<<8)+image[x+0]);
  if(x+4<size)
   fprintf(g,",");
  if(++col==8)
   {
   fprintf(g,"\n");
   col=0;
   }
  else
   fprintf(g," ");
  }
 if(col) fprintf(g,"\n");
 fprintf(g,"\
};\n");
 fclose(g);
 return 0;
 }
