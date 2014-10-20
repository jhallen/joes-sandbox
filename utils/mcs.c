#include <stdio.h>
#include <stdlib.h>

/* Convert .mcs file to binary file.
   by: Joe Allen 2/18/00

   A .mcs file is an ASCII text representation of binary data.  Each line
   is record in the following format:

     :NNAAAATTDDDDDDDDCC

   where:
     each pair of characters is an ascii hex byte ranging from 00 to FF.
       NN: number of data bytes contained in this line.
     AAAA: segment offset of the data contained in this line (MSB first).
       TT: type code: 00=data, 02=start of segment, 01=end of file.
       DD: data bytes: (up to 16 I think).
       CC: byte-wide checksum.  All bytes on the line, including NN, AAAA,
           TT, DDs, and CC should add up to 00.

   The end of file record contains no data.
   
   The start of segment record contains two data bytes: the segment number
   of the data records which follow.

   The linear address of any data byte equals the segment number multiplied
   by 16 plus the offset value.  Up to 1MB of data may be contained in
   a .mcs file.
*/

int hexval(char *s)
 {
 int val=0;
 if(*s>='0' && *s<='9') val=16*(*s-'0');
 else if(*s>='a' && *s<='f') val=16*(*s-'a'+10);
 else if(*s>='A' && *s<='F') val=16*(*s-'A'+10);
 else fprintf(stderr,"mcs: bad hex value\n"), exit(-1);
 ++s;
 if(*s>='0' && *s<='9') val+=*s-'0';
 else if(*s>='a' && *s<='f') val+=*s-'a'+10;
 else if(*s>='A' && *s<='F') val+=*s-'A'+10;
 else fprintf(stderr,"mcs: bad hex value\n"), exit(-1);
 return val;
 }

int main(int argc,char *argv[])
 {
 FILE *f, *g;
 char buf[1024];
 unsigned char *image;
 int size=0;
 int segment=0;
 int x;

 if(argc!=3)
  {
  fprintf(stderr,"mcs input.mcs output.bin\n");
  return -1;
  }

 image=malloc(1024*1024);
 if(!image)
  {
  fprintf(stderr,"mcs: couldn't allocate buffer\n");
  return -1;
  }
 for(x=0;x!=1024*1024;++x) image[x]=0xFF;

 f=fopen(argv[1],"r");
 if(!f)
  {
  fprintf(stderr,"mcs: couldn't open .mcs file %s\n",argv[1]);
  return -1;
  }

 while(fgets(buf,1023,f))
  if(buf[0]!=':')
   {
   fprintf(stderr,"mcs: Huh?  Line does not begin with ':'\n");
   return -1;
   }
  else
   {
   int offset;
   int cksum;
   int len;
   int type;
   int x;
   len=hexval(buf+1);
   
   /* Verify checksum */
   for(x=0,cksum=0;x!=len+5;++x) cksum+=hexval(x*2+1+buf);
   if((cksum&0xFF)!=0) fprintf(stderr,"mcs: bad checksum\n"), exit(-1);

   type=hexval(buf+7);
   offset=hexval(buf+3)*256+hexval(buf+5);

   if(type==0)
    { /* Copy data */
    for(x=0;x!=len;++x)
     image[offset+segment*16+x]=hexval(buf+9+x*2);
    if(offset+segment*16+x>size)
     size=offset+segment*16+x;
    }
   else if(type==2)
    { /* Set segment value */
    segment=hexval(buf+9)*256+hexval(buf+11);
    }
   else if(type==4)
    { /* Set segment value */
    segment=(hexval(buf+9)*256+hexval(buf+11))*4096;
    }
   else if(type==1)
    { /* The end */
    g=fopen(argv[2],"w");
    if(!g)
     {
     fprintf(stderr,"mcs: couldn't open .bin file %s\n",argv[2]);
     return -1;
     }
    if(size!=fwrite(image,1,size,g))
     fprintf(stderr,"mcs: write returned error\n"), exit(-1);
    if(fclose(g))
     fprintf(stderr,"mcs: close returned error\n"), exit(-1);
    else
     {
     int checksum=0;
     fprintf(stderr,"mcs: wrote %d bytes\n",size);
     for(x=0;x!=1024*1024;++x) checksum+=image[x];
     fprintf(stderr,"mcs: checksum=%x\n",checksum&0xFFFF);
     }
    return 0;
    }
   else
    { /* Huh? */
    fprintf(stderr,"mcs: unknown record type\n");
    return -1;
    }
   }
 fprintf(stderr,"mcs: end of file encountered before end record\n");
 return -1;
 }
