#include <stdio.h>
#include "bipopen.h"

BIP *bipopen(cmd)
char *cmd;
 {
 BIP *bip=malloc(sizeof(bip));
 int to[2];
 int from[2];
 pipe(to);
 pipe(from);
 if(!(bip->pid=fork()))
  {
  close(to[1]);
  close(from[0]);
  close(0);
  close(1);
  dup(to[0]); close(to[0]);
  dup(from[1]); close(from[1]);
  system(cmd);
  exit(0);
  }
 close(to[0]);
 close(from[1]);
 bip->outfd=to[1];
 bip->infd=from[0];
 return bip;
 }

void bipclose(bip)
BIP *bip;
 {
 int status;
 close(bip->outfd);
 close(bip->infd);
 free(bip);
 wait(NULL);
 }

void bipputs(bip,s)
BIP *bip;
char *s;
 {
 write(bip->outfd,s,zlen(s));
 write(bip->outfd,"\n",1);
 }

char *bipgets(bip,buf)
BIP *bip;
char *buf;
 {
 char *s=buf;
 char c;
 while(1==read(bip->infd,&c,1) && c!='\n') *s++=c;
 *s++=0;
 return buf;
 }
