/* Program to list contents of archive */

#include <stdio.h>

arseek(stream,amount)
FILE *stream;
unsigned long amount;
{
char *buf=malloc(16384);
while(amount>16384) fread(buf,1,16384,stream), amount-=16384;
if(amount) fread(buf,1,amount,stream);
free(buf);
}

int main(argc,argv)
int argc;
char *argv[];
{
char *name;
unsigned long sz;
char type;
up:
if(1!=fread(&type,1,1,stdin))
 {
 printf("Error in archive\n");
 return 1;
 } 
switch(type)
 {
case 'L':
 printf("Link: ");
 fread(&sz,sizeof(long),1,stdin);
 name=malloc(sz+1);
 fread(name,1,sz,stdin);
 name[sz]=0;
 printf("%s -> ",name);
 free(name);
 fread(&sz,sizeof(long),1,stdin);
 name=malloc(sz+1);
 fread(name,1,sz,stdin);
 name[sz]=0;
 printf("%s\n",name);
 free(name);
 break;
case 'S':
 printf("Special: ");
 fread(&sz,sizeof(long),1,stdin);
 name=malloc(sz+1);
 fread(name,1,sz,stdin);
 name[sz]=0;
 printf("%s\n",name);
 free(name);
 fread(&sz,sizeof(long),1,stdin);
 arseek(stdin,sz);
 break;
case 'E':
 return 0;
case 'F':
 printf("File: ");
 fread(&sz,sizeof(long),1,stdin);
 name=malloc(sz+1);
 fread(name,1,sz,stdin);
 name[sz]=0;
 printf("%s\n",name);
 free(name);
 fread(&sz,sizeof(long),1,stdin);
 arseek(stdin,sz);
 fread(&sz,sizeof(long),1,stdin);
 arseek(stdin,sz);
 break;
default:
 printf("Error in archive\n");
 return 1;
 }
goto up;
}
