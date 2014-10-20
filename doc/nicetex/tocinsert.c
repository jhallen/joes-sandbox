/* Insert table of contents into postscript file for .pdf generation */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

FILE *f;

struct entry
 {
 struct entry *up;	/* Entry we're subordinate to */
 struct entry *next;	/* Next entry at this level */
 struct entry *first;	/* Sub-entries */
 struct entry *last;
 int count;		/* No. sub-entries */
 char title[256];		/* Title */
 int page;			/* Page no. */
 };

struct entry entries;
struct entry *current= &entries;	/* Points to level above last
					   entry we added */
int curlevel;				/* Level of last entry */

/* Emit table of contents */

void dotoc(struct entry *e)
 {
 struct entry *f;
 for(f=e->first;f;f=f->next)
  if(f->count)
   {
   printf("[ /Page %d /Count %d /View [/XYZ 44 730 1.0] /Title (%s) /OUT pdfmark\n",f->page,f->count,f->title);
   dotoc(f);
   }
  else
   printf("[ /Page %d /View [/XYZ 44 730 1.0] /Title (%s) /OUT pdfmark\n",f->page,f->title);
 }

void unfix(char *d,char *s)
  {
  while(*s)
    if(!strncmp(s,"{\\^{}}",6))
      {
      *d++ = '^';
      s+=6;
      }
    else if(!strncmp(s,"$\\backslash$",12))
      {
      *d++ = '\\';
      *d++ = '\\';
      s+=12;
      }
    else if(!strncmp(s,"$>$",3))
      {
      *d++ = '>';
      s+=3;
      }
    else if(!strncmp(s,"$<$",3))
      {
      *d++ = '<';
      s+=3;
      }
    else if(!strncmp(s,"$|$",3))
      {
      *d++ = '|';
      s+=3;
      }
    else if(!strncmp(s,"$\\{$",4))
      {
      *d++ = '{';
      s+=4;
      }
    else if(!strncmp(s,"$\\}$",4))
      {
      *d++ = '}';
      s+=4;
      }
    else if(!strncmp(s,"{\\~{}}",6))
      {
      *d++ = '~';
      s+=6;
      }
    else
      *d++ = *s++;
  *d = 0;
  }

/* Insert TOC entry */

void insert(int level,int page,char *title)
 {
 struct entry *e;
 while(level<curlevel)
  {
  --curlevel;
  current=current->up;
  }
 while(level>curlevel)
  {
  ++curlevel;
  if(current->last) current=current->last;
  else
   { /* This should not happen */
   e=malloc(sizeof(struct entry));
   e->up=current;
   e->next=0;
   e->first=0;
   e->last=0;
   e->count=0;
   e->title[0]=0;
   e->page=1;
   if(current->first)
    current->last->next=e, current->last=e;
   else
    current->last=current->first=e;
   ++current->count;
   current=e;
   }
  }
 e=malloc(sizeof(struct entry));
 e->up=current;
 e->first=0;
 e->last=0;
 e->next=0;
 e->count=0;
 unfix(e->title,title);
 e->page=page;
 if(current->last)
  current->last->next=e, current->last=e;
 else
  current->first=current->last=e;
 ++current->count;
 }

/* Extract field and advance over separator */

char *field(char *line,char sep)
 {
 while(*line && *line!=sep) ++line;
 if(*line) *line++=0;
 return line;
 }

char *rfind(char *line,char *s)
 {
 char *e=line+strlen(line);
 int l=strlen(s);
 while(e!=line)
   if(!strncmp(e,s,l))
     {
     *e++ =0;
     return e;
     }
   else
     --e;
 return 0;
 }

int main(int argc,char *argv[])
 {
 FILE *f;
 int ofst=0;
 char buf[1024];
 f=fopen(argv[1],"r");
 sscanf(argv[2],"%d",&ofst);
 while(fgets(buf,1023,f))
  {
  char *ptr=buf+3;
  char *secno;
  char *secname;
  char buf1[1024];
  int pageno;
  int level;
  secno=ptr; ptr=field(ptr,':');
  secname=ptr; ptr=rfind(ptr,"\\page");
  sscanf(ptr+4,"%d",&pageno);
  if(buf[1]=='c' && buf[2]=='h') level=0;
  else if(buf[1]=='s' && buf[2]=='e') level=1;
  else level=2;
  sprintf(buf1,"%s %s",secno,secname);

  insert(level,pageno+ofst-1,buf1);
  }
 fclose(f);
 /* Skip header */
 while(gets(buf) && buf[0]=='%') puts(buf);
 /* Show bookmarks */
 printf("[ /PageMode /UseOutlines /DOCVIEW pdfmark\n");
 /* Insert table of contents */
 dotoc(&entries);
 /* Rest of postscript */
 puts(buf);
 while(gets(buf)) puts(buf);
 return 0;
 }
