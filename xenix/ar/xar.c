/* Program to list contents of archive */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

arseek(stream,amount)
FILE *stream;
unsigned long amount;
{
char *buf=malloc(16384);
while(amount>16384) fread(buf,1,16384,stream), amount-=16384;
if(amount) fread(buf,1,amount,stream);
free(buf);
}

arread(stream,amount,name)
FILE *stream;
unsigned long amount;
char *name;
{
char *buf=malloc(16384);
FILE *fd=fopen(name,"w");
while(amount>16384)
 {
 fread(buf,1,16384,stream), amount-=16384;
 fwrite(buf,1,16384,fd);
 }
if(amount)
 {
 fread(buf,1,amount,stream);
 fwrite(buf,1,amount,fd);
 }
free(buf);
fclose(fd);
}

int main(argc,argv)
int argc;
char *argv[];
{
char *name;
char *lname;
unsigned long sz;
unsigned long lsz;
char type;
struct stat buf;
up:
if(1!=fread(&type,1,1,stdin))
 {
 printf("Error in archive\n");
 return 1;
 } 
switch(type)
 {
case 'L':
 fread(&sz,sizeof(long),1,stdin);
 name=malloc(sz+1);
 fread(name,1,sz,stdin);
 name[sz]=0;
 fread(&lsz,sizeof(long),1,stdin);
 lname=malloc(lsz+1);
 fread(lname,1,lsz,stdin);
 lname[lsz]=0;
 link(lname,name);
 free(name);
 free(lname);
 break;
case 'S':
 fread(&sz,sizeof(long),1,stdin);
 name=malloc(sz+1);
 fread(name,1,sz,stdin);
 name[sz]=0;
 fread(&lsz,sizeof(long),1,stdin);
 fread(&buf,lsz,1,stdin);

 unlink(name);
 if(buf.st_mode&040000)
  {
  char *s=malloc(14+strlen(name));
  strcpy(s,"/bin/mkdir \'");
  strcat(s,name);
  strcat(s,"\'");
  system(s);
  free(s);
  }
 else mknod(name,buf.st_mode,buf.st_dev);
 chown(name,buf.st_uid,buf.st_gid);
 utime(name,&buf.st_atime);

 free(name);
 break;
case 'E':
 return 0;
case 'F':
 fread(&sz,sizeof(long),1,stdin);
 name=malloc(sz+1);
 fread(name,1,sz,stdin);
 name[sz]=0;
 fread(&lsz,sizeof(long),1,stdin);
 fread(&buf,lsz,1,stdin);
 fread(&lsz,sizeof(long),1,stdin);
 unlink(name);
 mknod(name,buf.st_mode,buf.st_dev);
 chown(name,buf.st_uid,buf.st_gid);
 arread(stdin,lsz,name);
 utime(name,&buf.st_atime);
 free(name);
 break;
default:
 printf("Error in archive\n");
 return 1;
 }
goto up;
}
