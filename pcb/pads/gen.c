/* Generate a pqfp */

#include <stdio.h>
#include <string.h>

int line=0;	/* Current line number */

/* Ascii to integer */

int atoi(s)
char *s;
 {
 int flg=0;
 int n=0;
 if(*s=='-') flg=1, ++s;
 while(*s>='0' && *s<='9') n=n*10+*s++-'0';
 if(*s)
  {
  fprintf(stderr,"%d: bad number\n");
  exit(1);
  }
 return flg?-n:n;
 }

/* Convert integer time 1000 into ascii floating point */

char *ntos(n)
long n;
 {
 static char buf[20];
 int flg=0;
 int x=0;
 if(n<0) buf[x++]='-', n= -n;

 /* Round number so that it's one past decimal place */
 if(n%10>=5) n+=10;
 if(n%100>=50) n+=100;
 n=n/100;
 n=n*100;

 if(n>=1000000000 || flg)
  {
  flg=1;
  buf[x]='0';
  while(n>=1000000000) ++buf[x], n-=1000000000;
  ++x;
  }
 if(n>=100000000 || flg)
  {
  flg=1;
  buf[x]='0';
  while(n>=100000000) ++buf[x], n-=100000000;
  ++x;
  }
 if(n>=10000000 || flg)
  {
  flg=1;
  buf[x]='0';
  while(n>=10000000) ++buf[x], n-=10000000;
  ++x;
  }
 if(n>=1000000 || flg)
  {
  flg=1;
  buf[x]='0';
  while(n>=1000000) ++buf[x], n-=1000000;
  ++x;
  }
 if(n>=100000 || flg)
  {
  flg=1;
  buf[x]='0';
  while(n>=100000) ++buf[x], n-=100000;
  ++x;
  }
 if(n>=10000 || flg)
  {
  flg=1;
  buf[x]='0';
  while(n>=10000) ++buf[x], n-=10000;
  ++x;
  }
 buf[x]='0';
 while(n>=1000) ++buf[x], n-=1000;
 ++x;
 if(n)
  {
  buf[x++]='.';
  buf[x]='0';
  while(n>=100) ++buf[x], n-=100;
  ++x;
  if(n)
   {
   buf[x]='0';
   while(n>=10) ++buf[x], n-=10;
   ++x;
   if(n)
    buf[x++]='0'+n;
   }
  }

 buf[x]=0;
 return buf;
 }

/* Convert ascii number into integer times 1000 */

long ston(s)
char *s;
 {
 int flg=0;
 long n=0;
 if(*s=='-') flg=1, ++s;
 while(*s>='0' && *s<='9') n=n*10+*s++-'0';
 if(*s=='.') ++s;
 n=n*10; if(*s>='0' && *s<='9') n=n+*s++-'0';
 n=n*10; if(*s>='0' && *s<='9') n=n+*s++-'0';
 n=n*10; if(*s>='0' && *s<='9') n=n+*s++-'0';
 if(*s)
  {
  fprintf(stderr,"%d: incorrect number\n",line);
  exit(1);
  }
 return flg?-n:n;
 }

int unit=0;	/* Units.  0=mils, 1=mm */

/* Convert to mils given current units */

long cvt(n)
long n;
 {
 if(unit)
  {
  n*=3937;
  if(n%10>=5) n+=10;
  if(n%100>=50) n+=100;
  return n/100;
  }
 else return n;
 }

struct pin
 {
 long x, y;	/* Pin position in tenths of mils */
 int o;		/* Orientation of pad: 0=right, 1=up, 2=left, 3=down */
 long width;
 long inside;
 long outside;

 /* Pads coords */
 /* size of pad is 'width' above */
 /* length of pad is 'inside+outside' above */
 long padsx, padsy;	/* Coords for pads */
 int ori;		/* Pads orientation 0 or 90 */
 };

struct pin pin[1024];
int npins=0;

/* Default pad stack */
int ori= -1;
long size= -1;
long length= -1;

char *name="TEST";	/* Name of part */

/* Pad definition */
long width=0;
long inside=0;
long outside=0;

main(argc,argv)
char *argv[];
 {
 char buf[1024];
 char *fields[128];
 int nfields;
 int x;
 int nstacks;
 if(argc!=2)
  {
  fprintf(stderr,"gen file\n");
  return 1;
  }
 if(!freopen(argv[1],"r",stdin))
  {
  fprintf(stderr,"Couldn't open file\n");
  return 1;
  }
 while(gets(buf))
  {
  ++line;
  nfields=0;
  x=0;
  loop:
  while(buf[x]==' ' || buf[x]=='\t') ++x;
  if(!buf[x] || buf[x]==';') goto done;
  fields[nfields++]=buf+x;
  while(buf[x] && buf[x]!=' ' && buf[x]!='\t') ++x;
  if(buf[x])
   {
   buf[x]=0;
   ++x;
   goto loop;
   }
  done:
  if(nfields)
   {
   if(!strcmp(fields[0],"name"))
    {
    if(nfields!=2)
     {
     fprintf(stderr,"%d: name command missing parameter\n",line);
     return 1;
     }
    name=strdup(fields[1]);
    }
   else if(!strcmp(fields[0],"units"))
    {
    if(nfields!=2)
     {
     fprintf(stderr,"%d: units command missing parameter\n",line);
     return 1;
     }
    if(!strcmp(fields[1],"mm")) unit=1;
    else if(!strcmp(fields[1],"mil")) unit=0;
    else
     {
     fprintf(stderr,"%d: invalid parameter given to units command\n",line);
     return 1;
     }
    }
   else if(!strcmp(fields[0],"rect"))
    {
    if(nfields!=4)
     {
     fprintf(stderr,"%d: incorrect no parameter to rect command\n",line);
     return 1;
     }
    width=cvt(ston(fields[1]));
    inside=cvt(ston(fields[2]));
    outside=cvt(ston(fields[3]));
    }
   else if(!strcmp(fields[0],"r"))
    {
    int ori;
    long xpos;
    long ypos;
    int n;
    long inc;
    if(nfields!=6)
     {
     fprintf(stderr,"%d: Missing arguments for r command\n",line);
     return 1;
     }
    if(!strcmp(fields[1],"r")) ori=0;
    else if(!strcmp(fields[1],"u")) ori=1;
    else if(!strcmp(fields[1],"l")) ori=2;
    else if(!strcmp(fields[1],"d")) ori=3;
    else
     {
     fprintf(stderr,"%d: Bad orientation\n",line);
     return 1;
     }
    xpos=ston(fields[2]);
    ypos=ston(fields[3]);
    n=atoi(fields[4]);
    inc=ston(fields[5]);
    /* Create pins */
    for(x=0;x!=n;++x)
     {
     pin[npins].x=cvt(xpos);
     pin[npins].y=cvt(ypos);
     pin[npins].o=ori;
     pin[npins].width=width;
     pin[npins].inside=inside;
     pin[npins].outside=outside;
     xpos+=inc;
     ++npins;
     }
    }
   else if(!strcmp(fields[0],"u"))
    {
    int ori;
    long xpos;
    long ypos;
    int n;
    long inc;
    if(nfields!=6)
     {
     fprintf(stderr,"%d: Missing arguments for u command\n",line);
     return 1;
     }
    if(!strcmp(fields[1],"r")) ori=0;
    else if(!strcmp(fields[1],"u")) ori=1;
    else if(!strcmp(fields[1],"l")) ori=2;
    else if(!strcmp(fields[1],"d")) ori=3;
    else
     {
     fprintf(stderr,"%d: Bad orientation\n",line);
     return 1;
     }
    xpos=ston(fields[2]);
    ypos=ston(fields[3]);
    n=atoi(fields[4]);
    inc=ston(fields[5]);
    /* Create pins */
    for(x=0;x!=n;++x)
     {
     pin[npins].x=cvt(xpos);
     pin[npins].y=cvt(ypos);
     pin[npins].o=ori;
     pin[npins].width=width;
     pin[npins].inside=inside;
     pin[npins].outside=outside;
     ypos+=inc;
     ++npins;
     }
    }
   else if(!strcmp(fields[0],"l"))
    {
    int ori;
    long xpos;
    long ypos;
    int n;
    long inc;
    if(nfields!=6)
     {
     fprintf(stderr,"%d: Missing arguments for l command\n",line);
     return 1;
     }
    if(!strcmp(fields[1],"r")) ori=0;
    else if(!strcmp(fields[1],"u")) ori=1;
    else if(!strcmp(fields[1],"l")) ori=2;
    else if(!strcmp(fields[1],"d")) ori=3;
    else
     {
     fprintf(stderr,"%d: Bad orientation\n",line);
     return 1;
     }
    xpos=ston(fields[2]);
    ypos=ston(fields[3]);
    n=atoi(fields[4]);
    inc=ston(fields[5]);
    /* Create pins */
    for(x=0;x!=n;++x)
     {
     pin[npins].x=cvt(xpos);
     pin[npins].y=cvt(ypos);
     pin[npins].o=ori;
     pin[npins].width=width;
     pin[npins].inside=inside;
     pin[npins].outside=outside;
     xpos-=inc;
     ++npins;
     }
    }
   else if(!strcmp(fields[0],"d"))
    {
    int ori;
    long xpos;
    long ypos;
    int n;
    long inc;
    if(nfields!=6)
     {
     fprintf(stderr,"%d: Missing arguments for d command\n",line);
     return 1;
     }
    if(!strcmp(fields[1],"r")) ori=0;
    else if(!strcmp(fields[1],"u")) ori=1;
    else if(!strcmp(fields[1],"l")) ori=2;
    else if(!strcmp(fields[1],"d")) ori=3;
    else
     {
     fprintf(stderr,"%d: Bad orientation\n",line);
     return 1;
     }
    xpos=ston(fields[2]);
    ypos=ston(fields[3]);
    n=atoi(fields[4]);
    inc=ston(fields[5]);
    /* Create pins */
    for(x=0;x!=n;++x)
     {
     pin[npins].x=cvt(xpos);
     pin[npins].y=cvt(ypos);
     pin[npins].o=ori;
     pin[npins].width=width;
     pin[npins].inside=inside;
     pin[npins].outside=outside;
     ypos-=inc;
     ++npins;
     }
    }
   else
    {
    fprintf(stderr,"%d: unknown command\n");
    return 1;
    }
   }
  }

 /* Convert our coord format into pads format */
 for(x=0;x!=npins;++x)
  {
  switch(pin[x].o)
   {
   case 0: /* Right */
    pin[x].ori=0;
    pin[x].padsy=pin[x].y;
    pin[x].padsx=pin[x].x+(pin[x].inside+pin[x].outside)/2-pin[x].inside;
    break;
   case 1: /* Up */
    pin[x].ori=90;
    pin[x].padsx=pin[x].x;
    pin[x].padsy=pin[x].y+(pin[x].inside+pin[x].outside)/2-pin[x].inside;
    break;
   case 2: /* Left */
    pin[x].ori=0;
    pin[x].padsy=pin[x].y;
    pin[x].padsx=pin[x].x-(pin[x].inside+pin[x].outside)/2+pin[x].inside;
    break;
   case 3: /* Down */
    pin[x].ori=90;
    pin[x].padsx=pin[x].x;
    pin[x].padsy=pin[x].y-(pin[x].inside+pin[x].outside)/2+pin[x].inside;
    break;
   }
  }
 if(!npins)
  {
  fprintf(stderr,"%d: there were no pins!\n",line);
  return 1;
  }

 /* Calculate no. pad stacks we're going to need */
 nstacks=1;
 ori=pin[0].ori;
 size=pin[0].width;
 length=pin[0].inside+pin[0].outside;

 for(x=0;x!=npins;++x)
  if(pin[x].ori!=ori || pin[x].width!=size ||
     pin[x].inside+pin[x].outside!=length) ++nstacks;

 /* Output header */
 printf("%s I -23000 -23000 0 0 0 0 %d %d\n",name,npins,nstacks);

 /* Output pins */
 for(x=0;x!=npins;++x)
  {
  printf("T%s ",ntos(pin[x].padsx));
  printf("%s ",ntos(pin[x].padsy));
  printf("%s ",ntos(pin[x].padsx));
  printf("%s\n",ntos(pin[x].padsy));
  }

 /* Output pad stacks */
 printf("PAD 0 3\n");
 printf("-2 %d RF %d %d 0 0 N\n",(int)(size/1000),ori,(int)(length/1000));
 printf("-1 0 R\n");
 printf("0 0 R\n");

 /* Output all other stacks */
 for(x=0;x!=npins;++x)
  if(pin[x].ori!=ori || pin[x].width!=size ||
     pin[x].inside+pin[x].outside!=length)
   {
   printf("PAD %d 3\n",x+1);
   printf("-2 %d RF %d %d 0 0 N\n",(int)(pin[x].width/1000),pin[x].ori,
          (int)((pin[x].inside+pin[x].outside)/1000));
   printf("-1 0 R\n");
   printf("0 0 R\n");
   }
 }
