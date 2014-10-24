/* Date functions */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include "date.h"

/* The basic facts */

/* Months of the year */
static char *months[]={ "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

/* Days of the week */
static char *days[]=  { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

/* Days in each month */
static int dpm[]=     { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/* Days in each month on a leap year */
static int dpmleap[]= { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/* Seconds per day */
#define spd (60*60*24)

/* Return true if year is a leap-year */

static int isleapyear(int year)
  {
  if(year%4 || year%100==0 && year%400) return 0;
  else return 1;
  }

/* Get today's date in YYYYMMDD\0 format */

void today(char *date)
  {
  long now=time(NULL);
  struct tm *tm=localtime(&now);
  sprintf(date,"%4.4d%2.2d%2.2d",tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday);
  }

/* Convert (year,day) to (year,month,day) */
/* Add day to Jan 1 of given year and return resulting date.
 * Negative values are ok.
 */

static void dateofday(int year,int day,int *rtn_year,int *rtn_month,int *rtn_day)
  {
  int x;
  int flg;
  while(day<0)
    {
    --year;
    if(isleapyear(year)) day+=366;
    else day+=365;
    }
  while(flg=isleapyear(year), day>=366 || !flg && day>=365)
    {
    ++year;
    if(flg) day-=366;
    else day-=365;
    }
  if(isleapyear(year))
    {
    for(x=0;dpmleap[x]<=day;++x) day-=dpmleap[x];
    }
  else
    {
    for(x=0;dpmleap[x]<=day;++x) day-=dpm[x];
    }
  *rtn_year = year;
  *rtn_month = x;
  *rtn_day = day;
  }

/* Convert year,month,day to year,day */

static int dayofyear(int year,int month,int day)
  {
  int d=0;
  int x;
  if(isleapyear(year)) for(x=0;x!=month;++x) d+=dpmleap[x];
  else for(x=0;x!=month;++x) d+=dpm[x];
  d+=day;
  return d;
  }

/* Determine day of week (0-6, 0 is sunday) from date */

static int dayofweek(int year,int month,int day)
  {
  int dow=0;
  int x;
  if(year<1899)
    {
    fprintf(stderr,"Sorry, day of week function is broken for year before 1899\n");
    exit(-1);
    }
  /* Jan 1, 1899 is a sunday */
  for(x=1899;x!=year;++x)
    if(isleapyear(x)) dow+=2;
    else dow+=1;
  dow+=dayofyear(year,month,day);
  return dow%7;
  }

/* Determine no. days from start of year to first business week */

static int getdfw(int year)
  {
  int d=dayofweek(year,0,0);
  if(d>4) return 7-d;
  else return -d;
  }

/* Business week of year functions.
 * Year is broken into quarters such that the first month of each quarter
 * has 5 weeks.  Each quarter is 13 weeks except sometimes, the last which
 * can have 14 weeks.
 *
 * So this function can return 0 - 52.
 *
 * This definition is designed so that you can compare equal length quarters
 * from year to year.
 */

/* Convert year,month,day to year,week,day */

static void weekofyear(int year,int month,int day,
                int *rtn_year,int *rtn_week,int *rtn_day)
  {
  int firstday=dayofweek(year,0,0);
  int doy=dayofyear(year,month,day);
  if(firstday>=5)
    {
    firstday-=7;
    if(firstday+doy<0)
      {
      weekofyear(year-1,12-1,31-1,
                 rtn_year,rtn_week,rtn_day);
      *rtn_day+=doy+1;
      return;
      }
    }
  if(firstday!=4 && (firstday+doy)/7>=52)
    {
    *rtn_year=year+1;
    *rtn_week=0;
    *rtn_day=(firstday+doy)-52*7;
    }
  else
    {
    *rtn_year = year;
    *rtn_week = (firstday+doy)/7;
    *rtn_day = (firstday+doy)%7;
    }
  }

/* Convert year,week,day to year,month,day */

void dateofweek(int year,int week,int day,
                int *rtn_year,int *rtn_month,int *rtn_day)
  {
  int dfw=getdfw(year);	/* Day of first week */
  dateofday(year,week*7+day+dfw,rtn_year,rtn_month,rtn_day);
  }

/* Return -1 if date is invalid */

static int checkdate(int year,int month,int day)
  {
  if(month<0 || month>11) return -1;
  if(day<0) return -1;
  if(isleapyear(year))
    if(day>=dpmleap[month]) return -1;
    else return 0;
  else
    if(day>=dpm[month]) return -1;
    else return 0;
  }

/* Convert date into year, month, day. Returns -1 if date is invalid. */

static void numericdate(const char *date,int *year_rtn,int *month_rtn,int *day_rtn)
  {
  int x;
  int year=0;
  int month=0;
  int day=0;
  for(x=0;x!=8;++x)
    if(date[x]<'0' || date[x]>'9')
      {
      fprintf(stderr,"Bad date format\n");
      exit(-1);
      }
  year=year*10+*date++-'0';
  year=year*10+*date++-'0';
  year=year*10+*date++-'0';
  year=year*10+*date++-'0';
  month=month*10+*date++-'0';
  month=month*10+*date++-'0';
  day=day*10+*date++-'0';
  day=day*10+*date++-'0';
  --month;
  --day;
  if(checkdate(year,month,day))
    {
    fprintf(stderr,"Bad date format\n");
    exit(-1);
    }
  *year_rtn=year;
  *month_rtn=month;
  *day_rtn=day;
  }

/* Pretty print date: Sat Feb 14 */

void printdate1(char *buf,const char *date)
  {
  int year;
  int month;
  int day;
  int dow;
  buf[0]=0; if(!date[0]) return;
  numericdate(date,&year,&month,&day);
  dow=dayofweek(year,month,day);
  sprintf(buf,"%s %s %d",days[dow],months[month],day+1);
  }

/* Sat Feb 14 wk3 */

void printdate1w(char *buf,const char *date)
  {
  int year;
  int month;
  int day;
  int dow;
  int yr;
  int wkn;
  int dny;
  buf[0]=0; if(!date[0]) return;
  numericdate(date,&year,&month,&day);
  dow=dayofweek(year,month,day);
  weekofyear(year,month,day,&yr,&wkn,&dny);
  sprintf(buf,"%s %s %d wk%d",days[dow],months[month],day+1,wkn+1);
  }

/* 3 */

void printweek(char *buf,const char *date)
  {
  int year;
  int month;
  int day;
  int yr;
  int wkn;
  int dny;
  buf[0]=0; if(!date[0]) return;
  numericdate(date,&year,&month,&day);
  weekofyear(year,month,day,&yr,&wkn,&dny);
  sprintf(buf,"%d",wkn+1);
  }

/* Pretty print date: 05/07/81 */

void printdate2(char *buf,const char *date)
  {
  int year;
  int month;
  int day;
  int dow;
  buf[0]=0; if(!date[0]) return;
  numericdate(date,&year,&month,&day);
  sprintf(buf,"%2.2d/%2.2d/%2.2d",month+1,day+1,year%100);
  }

/* Pretty print date: Jan 14, 1980 */

void printdate3(char *buf,char *date)
  {
  int year;
  int month;
  int day;
  int dow;
  buf[0]=0; if(!date[0]) return;
  numericdate(date,&year,&month,&day);
  sprintf(buf,"%s %d, %d",months[month],day+1,year);
  }

/* Pretty print date: Sat Jan 14, 1980 */

void printdate4(char *buf,const char *date)
  {
  int year;
  int month;
  int day;
  int dow;
  int wkn;
  int yr;
  int dny;
  buf[0]=0; if(!date[0]) return;
  numericdate(date,&year,&month,&day);
  weekofyear(year,month,day,&yr,&wkn,&dny);
  dow=dayofweek(year,month,day);
/*  sprintf(buf,"%s %s %d, %d",days[dow],months[month],day+1,year); */
  sprintf(buf,"%s %d, %d w%d",months[month],day+1,year,wkn+1);
  }

/* Pretty print date: Sat Jan 14, 1980 (week 7) */

void printdate5(char *buf,char *date)
  {
  int year;
  int month;
  int day;
  int dow;
  int doy;
  int wkn;
  int yr;
  int dny;
  int ryear;
  int rmonth;
  int rday;
  buf[0]=0; if(!date[0]) return;
  numericdate(date,&year,&month,&day);
  dow=dayofweek(year,month,day);
  weekofyear(year,month,day,&yr,&wkn,&dny);
  sprintf(buf,"%s %s %d, %d (week %d)",
          days[dow],months[month],day+1,year,
          wkn+1);
  }

/* Parse user entry date into YYYYMMDD\0 format. */

int parsedate(char *date,const char *s)
  {
  int year= -1, month= -1, day= -1;
  int week=0;
  long x;
  char mbuf[32];
  date[0]=0;
  while(*s==' ' || *s=='\t' || *s=='\n' || *s==',' || *s=='/' || *s=='\'') s++;
  if(!*s) return -1; 

  if((*s=='w' || *s=='W') && (s[1]==' ' || s[1]=='\t' || s[1]>='0' && s[1]<='9'))
    {
    ++s;
    week=1;
    }

  /* Look for month (or skip over day of week) */
  if(!week)
    {
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

    /* Look for month again */
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
    }

  while(*s==' ' || *s=='\t' || *s=='\n' || *s==',' || *s=='/' || *s=='\'') s++;
  if(!*s) return -1; 
  if(month!= -1) goto gotmonth;

  /* Look for month */
  if(*s>='0' && *s<='9')
   {
   month=0;
   while(*s>='0' && *s<='9') month=month*10+*(s++)-'0';
   }
  while(*s==' ' || *s=='\t' || *s=='\n' || *s==',' || *s=='/' || *s=='\'') s++;
/*  if(!*s) return -1; */

  gotmonth:

  /* Look for day */
  if(!week)
    {
    if(*s>='0' && *s<='9')
      {
      day=0;
      while(*s>='0' && *s<='9') day=day*10+*(s++)-'0';
      }
    while(*s==' ' || *s=='\t' || *s=='\n' || *s==',' || *s=='/' || *s=='\'')
      s++;
    }

  /* Look for year */
  if(*s>='0' && *s<='9')
   {
   year=0;
   while(*s>='0' && *s<='9') year=year*10+*(s++)-'0';
   if(year<50) year+=2000;
   if(year<100) year+=1900;
   }

  /* Fill in current year if none */
  if(year== -1)
   {
   long t=time(NULL);
   struct tm *tm=localtime(&t);
   year=tm->tm_year+1900;
   }

  --month;
  --day;
  if(week) dateofweek(year,month,1,&year,&month,&day);
  if(checkdate(year,month,day)) return -1;
  sprintf(date,"%4.4d%2.2d%2.2d",year,month+1,day+1);
  return 0;
  }

/*
main(int argc,char *argv[])
  {
  char buf[1024];
  int year;
  char date[10];
  today(date);
  parsedate(date,argv[1]);
  printdate1(buf,date);  printf("%s\n",buf);
  printdate2(buf,date);  printf("%s\n",buf);
  printdate3(buf,date);  printf("%s\n",buf);
  printdate4(buf,date);  printf("%s\n",buf);
  printdate5(buf,date);  printf("%s\n",buf);
  }
*/
