/* Date conversion functions */

#include <sys/types.h>
#include <time.h>

#include "tim.h"

char *months[]={ "Jan","Feb","Mar","Apr","May","Jun",
                 "Jul","Aug","Sep","Oct","Nov","Dec" };

char *days[]=  { "Sun","Mon","Tue","Wed","Thu","Fri","Sat" };

int dpm[]=     {31,28,31,30,31,30,31,31,30,31,30,31};

int dpmleap[]= {31,29,31,30,31,30,31,31,30,31,30,31};

#define spd (60*60*24)

/* Returns date in the following 10 character format: Sat Feb 14 */

char *dntos1(char *buf,long n)
 {
 struct tm *tm=localtime(&n);
 sprintf(buf,"%s %s %d",days[tm->tm_wday],months[tm->tm_mon],tm->tm_mday);
 return buf;
 }

/* Returns date in the following 8 character format: 05/07/81 */

char *dntos2(char *buf,long n)
 {
 struct tm *tm=localtime(&n);
 sprintf(buf,"%2.2d/%2.2d/%2.2d",tm->tm_mon+1,tm->tm_mday,tm->tm_year%100);
 return buf;
 }

/* Returns date in the following 12 character format: Jan 14, 1980 */

char *dntos3(char *buf,long n)
 {
 struct tm *tm=localtime(&n);
 sprintf(buf,"%s %d, %d",months[tm->tm_mon],tm->tm_mday,tm->tm_year+1900);
 return buf;
 }

/* Returns date in the following 16 character format: Sat Jan 14, 1980 */

char *dntos4(char *buf,long n)
 {
 struct tm *tm;
 if(n== -1 || n==0) buf[0]=0;
 else
  {
  tm=localtime(&n);
  sprintf(buf,"%s %s %d, %d",days[tm->tm_wday],
         months[tm->tm_mon],tm->tm_mday,tm->tm_year+1900);
  }
 return buf;
 }

/* Day Month Year -> seconds */

long dmyton(int day,int month,int year)
 {
 long clock=0;
 int x;

 /* Range check */

 if(year<70) year+=2000;
 if(year<1970) year+=1900;
 if(month<1 || month>12) month=1;
 if(day<1 || day>31) day=1;

 /* Adjust for year */

 for(x=1970;x!=year;x++)
  if(x&3) clock+=spd*365;
  else clock+=spd*366;

 /* Adjust for month */

 if(year&3) for(x=0;x!=month-1;x++) clock+=spd*dpm[x];
 else for(x=0;x!=month-1;x++) clock+=spd*dpmleap[x];

 /* Adjust for day */

 clock+=spd*(day-1);

 /* Adjust for timezone */

 ctime(&clock);
 clock+=timezone;

 return clock;
 }

/* Convert any of the above string back into seconds */

long dston(char *s)
 {
 int year= -1, month= -1, day= -1;
 long x;
 char mbuf[32];
 while(*s==' ' || *s=='\t' || *s=='\n' || *s==',' || *s=='/' || *s=='\'') s++;
 if(!*s) return -1; 
 if(*s>='a' && *s<='z' || *s>='A' && *s<='Z')
  {
  for(x=0;
      (*s>='A' && *s<='Z' || *s>='a' && *s<='z') && x!=3;
      x++,s++)
   mbuf[x]=(0x5F&*s)+'a'-'A';
  mbuf[x]=0; mbuf[0]&=0x5F;
  while(*s>='A' && *s<='Z' || *s>='a' && *s<='z') s++;
  for(x=0;x!=12;x++) if(!strcmp(mbuf,months[x]))
   {
   month=x+1;
   break;
   }
  }
 while(*s==' ' || *s=='\t' || *s=='\n' || *s==',' || *s=='/' || *s=='\'') s++;
 if(!*s) return -1; 
 if(*s>='a' && *s<='z' || *s>='A' && *s<='Z')
  {
  for(x=0;
      (*s>='A' && *s<='Z' || *s>='a' && *s<='z') && x!=3;
      x++,s++)
   mbuf[x]=(0x5F&*s)+'a'-'A';
  mbuf[x]=0; mbuf[0]&=0x5F;
  while(*s>='A' && *s<='Z' || *s>='a' && *s<='z') s++;
  for(x=0;x!=12;x++) if(!strcmp(mbuf,months[x]))
   {
   month=x+1;
   break;
   }
  }
 while(*s==' ' || *s=='\t' || *s=='\n' || *s==',' || *s=='/' || *s=='\'') s++;
 if(!*s) return -1; 
 if(month!= -1) goto gotmonth;
 if(*s>='0' && *s<='9')
  {
  month=0;
  while(*s>='0' && *s<='9') month=month*10+*(s++)-'0';
  }
 while(*s==' ' || *s=='\t' || *s=='\n' || *s==',' || *s=='/' || *s=='\'') s++;
 if(!*s) return -1; 
 gotmonth:
 if(*s>='0' && *s<='9')
  {
  day=0;
  while(*s>='0' && *s<='9') day=day*10+*(s++)-'0';
  }
 while(*s==' ' || *s=='\t' || *s=='\n' || *s==',' || *s=='/' || *s=='\'') s++;
 if(*s>='0' && *s<='9')
  {
  year=0;
  while(*s>='0' && *s<='9') year=year*10+*(s++)-'0';
  }

 /* We got at least month and day */
 if(year== -1)
  {
  long t=time(0);
  struct tm *tm=localtime(&t);
  year=tm->tm_year+1900;
  }

 return dmyton(day,month,year);
 }


char *ntos(char *buf,long n)
 {
 sprintf(buf,"%ld",n);
 return buf;
 }

long ston(char *s)
 {
 long r=0;
 sscanf(s,"%ld",&r);
 return r;
 }
