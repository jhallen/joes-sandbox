#include <stdio.h>

/* Joe's High Quality Print Utility */

FILE *printer;
int lines_per_page=60;
int text_lines_per_page=56;
int starting_text_line=4;
int title_line=2;
int left_margin=0;

void doprint(unsigned char *name)
{
FILE *handle=fopen(name,"r");
unsigned char line[81];
int lines=0;
int pages;
int y;
unsigned char *x;
int curlin=0;
int curpage=0;
if(!handle)
 {
 fprintf(stderr,"Error opening file: %s\r\n",name);
 return;
 }
/* First pass... count the lines */
do
 {
 int ch=fgetc(handle);
 if(ferror(handle))
  {
  fprintf(stderr,"Error reading file: %s\r\n",name);
  return;
  }
 if(feof(handle))
  break;
 if(ch=='\n') lines++;
 }
 while(1);
if(fseek(handle,0,SEEK_SET))
 {
 fprintf(stderr,"Error rewinding file: %s\r\n",name);
 return;
 }
pages=lines/text_lines_per_page+(lines%text_lines_per_page ? 1 : 0);
do
 {
 for(curlin=0;curlin<lines_per_page;curlin++)
  {
  if(curlin==0) fprintf(printer,"---");
  if(curlin==title_line)
   {
   for(y=0;y<80;y++) line[y]=' ';
   line[y]=0;
   line[sprintf(line,"*** File: %s",name)]=' ';
   line[65+sprintf(line+65,"Page %d of %d",curpage+1,pages)]=' ';
   fprintf(printer,"%s",line);
   }
  if(curlin>=starting_text_line && curlin<starting_text_line+
     text_lines_per_page)
   {
   if(curpage==pages-1)
    {
    if(curlin-starting_text_line==lines%text_lines_per_page)
     {
     while(curlin<lines_per_page)
      {
      fprintf(printer,"\r\n");
      curlin++;
      }
     break;
     }
    }
   for(y=0;y<80;y++)
    {
    int a=fgetc(handle);
    if(ferror(handle))
     {
     fprintf(stderr,"Error reading file: %s\r\n",name);
     while(curlin<lines_per_page)
      {
      fprintf(printer,"\r\n");
      curlin++;
      }
     return;
     }
    if(feof(handle))
     {
     line[y]=0;
     goto over;
     }
    if(a=='\n')
     {
     line[y]=0;
     goto over;
     }
    line[y]=a;
    }
   do
    {
    int a=fgetc(handle);
    if(ferror(handle))
     {
     fprintf(stderr,"Error reading file: %s\r\n",name);
     while(curlin<lines_per_page)
      {
      fprintf(printer,"\r\n");
      curlin++;
      }
     return;
     }
    if(feof(handle)) break;
    if(a=='\n') break;
    } while(1);
over:
   fprintf(printer,"%s",line);
   }
  fprintf(printer,"\r\n");
  }
 curpage++;
 } while(curpage!=pages);
fclose(handle);
}

void main(int argc,char **argv)
{
int x;
if(argc<2)
 {
 fprintf(stderr,"Joe's High Quality Print Utility\r\n");
 fprintf(stderr,"Command format: pr file[,...]\r\n");
 exit(1);
 }
printer=fopen("PRN","wb");
if(!printer)
 {
 fprintf(stderr,"Couldn't open printer device\r\n");
 exit(1);
 }
for(x=1;x<argc;x++)
 {
 doprint(argv[x]);
 }
fclose(printer);
}
