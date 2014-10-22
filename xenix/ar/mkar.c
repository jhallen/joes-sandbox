/* Program to create an archive */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <ftw.h>

struct record
 {
 ino_t st_ino;
 dev_t st_dev;
 char *name;
 } *records;
unsigned nrecords;
unsigned srecords;

int ckrec(buf)
struct stat *buf;
{
unsigned x;
for(x=0;x<nrecords;x++)
 if(records[x].st_ino==buf->st_ino && records[x].st_dev==buf->st_dev)
  return records[x].name;
return 0;
}

adrec(buf,name)
struct stat *buf;
char *name;
{
if(nrecords==srecords)
records=realloc(records,(srecords+=1024)*sizeof(struct record));
records[nrecords].st_ino=buf->st_ino;
records[nrecords].st_dev=buf->st_dev;
records[nrecords].name=strdup(name);
nrecords++;
}

izrec()
{
nrecords=0;
srecords=1024;
records=malloc(sizeof(struct record)*1024);
}

int walk(name,buf,x)
char *name;
struct stat *buf;
{
char *link;
if(name[1]==0 && (name[0]=='.' || name[0]=='/')) return 0;
switch(x)
 {
case FTW_F:
case FTW_D:
 if(link=ckrec(buf))
  {
  unsigned long sz=strlen(name);
  fputc('L',stdout);
  fwrite(&sz,sizeof(unsigned long),1,stdout);
  fwrite(name,1,sz,stdout);
  sz=strlen(link);
  fwrite(&sz,sizeof(unsigned long),1,stdout);
  fwrite(link,1,sz,stdout);
  }
 else
  {
  adrec(buf,name);
  /* Save file, directory or node */
  if(buf->st_mode&070000)
   {
   unsigned long sz=strlen(name);
   fputc('S',stdout);
   fwrite(&sz,sizeof(unsigned long),1,stdout);
   fwrite(name,1,sz,stdout);
   sz=sizeof(struct stat);
   fwrite(&sz,sizeof(unsigned long),1,stdout);
   fwrite(buf,sz,1,stdout);
   }
  else
   {
   unsigned long sz=strlen(name);
   FILE *fi=fopen(name,"r");
   char *f;
   if(!fi)
    {
    fprintf(stderr,"mkar: Error opening file \'%s\'\n",name);
    fprintf(stderr,"mkar: \'%s\' not saved\n",name);
    break;
    }
   f=malloc(16384L);
   fputc('F',stdout);
   fwrite(&sz,sizeof(unsigned long),1,stdout);
   fwrite(name,1,sz,stdout);
   sz=sizeof(struct stat);
   fwrite(&sz,sizeof(unsigned long),1,stdout);
   fwrite(buf,sz,1,stdout);
   sz=buf->st_size;
   fwrite(&sz,sizeof(unsigned long),1,stdout);
   if(!fi) fprintf(stderr,"mkar: Error opening file \'%s\'\n",fi);
   else
    {
    while(sz>16384)
     {
     if(16384!=fread(f,1,16384,fi))
      {
      fprintf(stderr,"mkar: Error reading file \'%s\'\n",name);
      fprintf(stderr,"mkar: File will be saved but with errors\n");
      while(sz>16384) fwrite(f,16384,1,stdout), sz-=16384;
      if(sz) fwrite(f,sz,1,stdout);
      goto down;
      }
     fwrite(f,16384,1,stdout);
     sz-=16384;
     }
    if(sz)
     {
     if(sz!=fread(f,1,sz,fi))
      {
      fprintf(stderr,"mkar: Error reading file \'%s\'\n",name);
      fprintf(stderr,"mkar: File will be saved but with errors\n");
      }
     fwrite(f,sz,1,stdout);
     }
    }   
   down:
   fclose(fi);
   free(f);
   }
  }
 break;
case FTW_DNR:
 fprintf(stderr,"mkar: Couldn\'t read directory \'%s\'\n",name);
 fprintf(stderr,"mkar: \'%s\' and its descendants not saved\n",name);
 break;
case FTW_NS:
 fprintf(stderr,"mkar: Couldn\'t stat \'%s\'\n",name);
 fprintf(stderr,"mkar: \'%s\' not saved\n",name);
 break;
 }
return 0;
}

int main(argc,argv)
int argc;
char *argv[];
{
int x;
if(argc<2)
 {
 fprintf(stderr,"mkar files\nFiles will be saved in archive sent to stdout\n");
 return 1;
 }
izrec();
for(x=1;x<argc;x++)
 if(-1==ftw(argv[x],walk,8)) fprintf(stderr,"mkar: error saving \'%s\'\n",argv[x]);
fputc('E',stdout);
return 0;
}
