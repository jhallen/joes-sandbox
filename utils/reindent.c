/* Re-indentor */

#include <stdio.h>
#include <string.h>

int intabwidth=8;
int outtabwidth=8;
int space=0;
int mul=1;
int divby=1;

int main(int argc,char *argv[])
 {
 char buf[1024];
 int z;
 for(z=1;z!=argc;++z)
  if(!strcmp("-mul",argv[z]))
   {
   sscanf(argv[z+1],"%d",&mul);
   ++z;
   }
  else if(!strcmp("-div",argv[z]))
   {
   sscanf(argv[z+1],"%d",&divby);
   ++z;
   }
  else if(!strcmp("-tospace",argv[z]))
   {
   space=1;
   }
  else if(!strcmp("-intabwidth",argv[z]))
   {
   sscanf(argv[z+1],"%d",&intabwidth);
   ++z;
   }
  else if(!strcmp("-outtabwidth",argv[z]))
   {
   sscanf(argv[z+1],"%d",&outtabwidth);
   ++z;
   }
  else
   {
   fprintf(stderr,"Re-indent a source file\n\
\n\
  -intabwidth nnn               Assumed tab width of input\n\
  -outtabwidth nnn              Assumed tab width of output\n\
  -tospace                      No tabs on output\n\
  -mul nnn                      Multiply indentation by\n\
  -div nnn                      Divide indentation by\n\
");
   return -1;
   }
 while(gets(buf))
  {
  int x;
  int col=0;
  for(x=0;buf[x];++x)
   if(buf[x]==' ') ++col;
   else if(buf[x]=='\t') while(++col%intabwidth);
   else break;
  col*=mul;
  col/=divby;
  if(!space)
   while(col>=outtabwidth)
    {
    putchar('\t');
    col-=outtabwidth;
    }
  while(col)
   {
   putchar(' ');
   --col;
   }
  puts(buf+x);
  }
 return 0;
 }
