#include <stdio.h>
#include <stdlib.h>

#define maxsize 4096

/* Convert .exo file to binary file.
   by: Joe Allen, 2/18/00

   A .exo file is an ASCII text representation of binary data.  Each line
   is record in the following format:

     SNLLAAAAAADDDDDDDDCC

   where:
          S is literal character S
          N is ascii digit giving record type: 2=data record, 8=end of file
         LL is number of bytes in remainder of record in ascii hex
     AAAAAA is address of data bytes contained in this record in
            ascii hex (MSB first).
         DD are the data bytes in ascii hex.
         CC is byte-wide checksum.  All hex bytes from LL to the checksum
            should add up to FF.

   The end of file record contains no data.
   
   Up to 16MB may be stored in a .exo file.
*/

int hexval(char *s)
 {
 int val=0;
 if(*s>='0' && *s<='9') val=16*(*s-'0');
 else if(*s>='a' && *s<='f') val=16*(*s-'a'+10);
 else if(*s>='A' && *s<='F') val=16*(*s-'A'+10);
 else fprintf(stderr,"exo: bad hex value\n"), exit(-1);
 ++s;
 if(*s>='0' && *s<='9') val+=*s-'0';
 else if(*s>='a' && *s<='f') val+=*s-'a'+10;
 else if(*s>='A' && *s<='F') val+=*s-'A'+10;
 else fprintf(stderr,"exo: bad hex value\n"), exit(-1);
 return val;
 }

int main(int argc,char *argv[])
 {
 FILE *f, *g;
 char buf[1024];
 unsigned char *image;
 int size=0;
 int x;

 if(argc!=3)
  {
  fprintf(stderr,"exo input.exo output.bin\n");
  return -1;
  }

 image=malloc(maxsize*1024);
 if(!image)
  {
  fprintf(stderr,"exo: couldn't allocate buffer\n");
  return -1;
  }
 for(x=0;x!=maxsize*1024;++x) image[x]=0xFF;

 f=fopen(argv[1],"r");
 if(!f)
  {
  fprintf(stderr,"exo: couldn't open .exo file %s\n",argv[1]);
  return -1;
  }

 while(fgets(buf,1023,f))
  if(buf[0]!='S')
   {
   fprintf(stderr,"exo: Huh?  Line does not begin with 'S'\n");
   return -1;
   }
  else
   {
   int offset;
   int cksum;
   int len;
   int x;
   len=hexval(buf+2);
   
   /* Verify checksum */
   for(x=0,cksum=0;x!=len+1;++x) cksum+=hexval(x*2+2+buf);
   if((cksum&0xFF)!=0xFF) fprintf(stderr,"exo: bad checksum\n"), exit(-1);

   offset=hexval(buf+4)*65536+hexval(buf+6)*256+hexval(buf+8);
 
   if(offset+len-4>maxsize*1024)
    fprintf(stderr,"exo: can only handle up to 1MB, sorry.\n"), exit(-1);

   if(buf[1]=='2')
    { /* Copy data */
    for(x=0;x!=len-4;++x)
     image[offset+x]=hexval(buf+10+x*2);
    if(offset+x>size)
     size=offset+x;
    }
   else if(buf[1]=='8')
    { /* The end */
    g=fopen(argv[2],"w");
    if(!g)
     {
     fprintf(stderr,"exo: couldn't open .bin file %s\n",argv[2]);
     return -1;
     }
    if(size!=fwrite(image,1,size,g))
     fprintf(stderr,"exo: write returned error\n"), exit(-1);
    if(fclose(g))
     fprintf(stderr,"exo: close returned error\n"), exit(-1);
    else
     {
     int checksum=0;
     fprintf(stderr,"exo: wrote %d bytes\n",size);
     for(x=0;x!=maxsize*1024;++x) checksum+=image[x];
     fprintf(stderr,"exo: checksum=%x\n",checksum&0xFFFF);
     }
    return 0;
    }
   else
    { /* Huh? */
    fprintf(stderr,"exo: unknown record type\n");
    return -1;
    }
   }
 fprintf(stderr,"exo: end of file encountered before end record\n");
 return -1;
 }
