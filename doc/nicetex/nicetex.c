/* Convert text to TeX */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define INSTALLPATH "/usr/share/nicetex"

char main_name[80];

char inbuf[8192];

int body;

/* Fix identifier (TeX only likes letter in names) */

int idescape(char *z,int c)
  {
  int x=0;
  if((c>='a' && c<='z') || (c>='A' && c<='W') || c=='Y' || c=='Z')
    {
    z[x++]=c;
    }
  else
    {
    z[x++]='X';
    z[x++]='a'+(c/16);
    z[x++]='a'+(c&15);
    }
  return x;
  }

char *fixid(char *s)
  {
  char z[8192];
  int x=0;

  /* Nicetex common prefix */
  z[x++]='N';
  z[x++]='t';

  while(*s)
    x+=idescape(z+x,*s++);

  z[x]=0;

  return strdup(z);
  }

/* Fix */

int append(char *dest,int idx,char *src)
  {
  while(*src)
    dest[idx++]= *src++;
  return idx;
  }

char *fix(char *s)
 {
 char z[8192];
 int x=0;

 while(*s)
  {
  switch(*s)
    {
    case '_':
      {
      if(s[1]=='!')
        {
        x=append(z,x,"|");
        ++s;
        }
      else
        x=append(z,x,"\\_");
      break;
      }
    case '#': x=append(z,x,"\\#"); break;
    case '$': x=append(z,x,"\\$"); break;
    case '&': x=append(z,x,"\\&"); break;
    case '%': x=append(z,x,"\\%"); break;
    case '>': x=append(z,x,"$>$"); break;
    case '<': x=append(z,x,"$<$"); break;
    case '^': x=append(z,x,"{\\^{}}"); break;
    case '~': x=append(z,x,"{\\~{}}"); break;
/*
    case '^': x=append(z,x,"\\^{}\\kern -2pt"); break;
    case '~': x=append(z,x,"\\~{}\\kern -2pt"); break;
*/
    case '|': x=append(z,x,"$|$"); break;
    case '\\':
      {
      if(s[1]=='\\')
        {
        x=append(z,x,"$\\backslash$");
        ++s;
        }
      else if(s[1]=='$')
        {
        ++s;
        z[x++]= *s++;
        while(*s && *s!='$') z[x++]= *s++;
        if(*s=='$') z[x++]='$';
        else --s;
        }
      else
        {
        while(*s && *s!=' ') z[x++]= *s++;
        --s;
        }
      break;
      }
    case '{': x=append(z,x,"$\\{$"); break;
    case '}': x=append(z,x,"$\\}$"); break;
    default: z[x++]= *s;
    }
  ++s;
  }
 z[x]=0;
 return strdup(z);
 }

/* Flag.  Set when first paragraph is emitted. */

int first;

/* Current paragraph */

char para[65536];
int paraidx;

/* Append text to current paragraph */

void addpara(char *s)
 {
 if(paraidx) para[paraidx++]=' ';
 while(*s)
  {
  switch(*s)
    {
    case '_': paraidx=append(para,paraidx,"\\_"); break;
    case '#': paraidx=append(para,paraidx,"\\#"); break;
    case '$': paraidx=append(para,paraidx,"\\$"); break;
    case '&': paraidx=append(para,paraidx,"\\&"); break;
    case '%': paraidx=append(para,paraidx,"\\%"); break;
    case '>': paraidx=append(para,paraidx,"$>$"); break;
    case '<': paraidx=append(para,paraidx,"$<$"); break;
    case '^': paraidx=append(para,paraidx,"{\\^{}}"); break;
    case '~': paraidx=append(para,paraidx,"{\\~{}}"); break;
/*
    case '^': paraidx=append(para,paraidx,"\\^{}\\kern -2pt"); break;
    case '~': paraidx=append(para,paraidx,"\\~{}\\kern -2pt"); break;
*/
    case '|': paraidx=append(para,paraidx,"$|$"); break;
    case '\\':
      {
      if(s[1]=='\\')
        {
        paraidx=append(para,paraidx,"$\\backslash$");
        ++s;
        }
      else if(s[1]=='$')
        {
        ++s;
        para[paraidx++]= *s++;
        while(*s && *s!='$') para[paraidx++]= *s++;
        if(*s=='$') para[paraidx++]='$';
        else --s;
        }
      else
        { /* Take everything up to next whitespace */
        if(!strncmp(s,"\\ref{",5))
          { /* Fix reference identifiers */
          s+=5;
          strcpy(para+paraidx,"\\ref{Nt"); paraidx+=strlen(para+paraidx);
          while(*s && *s!='}')
            paraidx+=idescape(para+paraidx,*s++);
          if(*s=='}') ++s;
          para[paraidx++]='}';
          --s;
          }
        else if(!strncmp(s,"\\pageref{",9))
          { /* Fix reference identifiers */
          s+=9;
          strcpy(para+paraidx,"\\pageref{Nt"); paraidx+=strlen(para+paraidx);
          while(*s && *s!='}')
            paraidx+=idescape(para+paraidx,*s++);
          if(*s=='}') ++s;
          para[paraidx++]='}';
          --s;
          }
        else
          {
          while(*s && *s!=' ')
            para[paraidx++]= *s++;
          --s;
          }
        }
      break;
      }
    case '{': paraidx=append(para,paraidx,"$\\{$"); break;
    case '}': paraidx=append(para,paraidx,"$\\}$"); break;
    default: para[paraidx++]= *s;
    }
  ++s;
  }
 para[paraidx]=0;
 }

/* Clear current paragraph */

void clearpara()
 {
 para[paraidx=0]=0;
 }

/* Emit header */

void emitstart()
 {
 char buf[1024];
 FILE *f;
 if(first) return;
 first=1;
 f=fopen("nice.tex","r");
 if (!f) {
  sprintf(buf,"%s/etc/nice.tex",getenv("HOME"));
  f=fopen(buf,"r");
 }
 if (!f) {
  sprintf(buf,"%s/nice.tex",INSTALLPATH);
  f=fopen(buf,"r");
 }
 if(!f)
  {
  fprintf(stderr,"Couldn't open cvtstart.tex\n");
  exit(1);
  }
 while(fgets(buf,1023,f)) fputs(buf,stdout);
 fclose(f);
 }

/* Output current paragraph */

void emitpara()
 {
 if(paraidx)
  {
  if(!first)
   {
   emitstart();
   first=1;
   }
  printf("%s\n\n",para);
  }
 clearpara();
 }

/* Extract field and advance over whitespace */

char *fields(char *line)
 {
 while(*line && *line!=' ' && *line!='\t') ++line;
 if(*line)
  {
  *line++=0;
  while(*line==' ' || *line=='\t') ++line;
  }
 return line;
 }

void rowfields(char **data,int ncols,char *s)
 {
 char *q;
 int col=0;				/* Current column number */

 if(*s=='|') ++s;			/* Skip first field sep. */

 loop:
 while(*s==' ' || *s=='\t') ++s;	/* Skip leading whitespace */
 data[col]=s;				/* Start of field */
 while(*s && *s!='|') ++s;		/* Skip to field sep. */

 if(!*s && !data[col][0])
   data[col]=0;

 for(q=s;--q>=data[col];)		/* Kill trailing whitespace */
  if(*q==' ' || *q=='\t') *q=0;
  else break;
 ++col;
 if(*s)
  {
  *s++=0;
  if(col!=ncols) goto loop;
  }

 while(col!=ncols) data[col++]=0;	/* Columns not given are empty */
 }

/* Parse a table */

struct row
 {
 struct row *next;	/* Next row */
 char **data;		/* Data fields */
 } *firstrow, *lastrow;	/* Rows */

int *fmt;	/* Column format (0=left, 1=center, 2=right) */
double *wid;	/* Column widths in mils */
int ncols;	/* No. columns in table */
int nrows;	/* No. of rows in table */

void gettable()
 {
 char *s;
 int x;
 ncols=0;
 nrows=0;
 firstrow=lastrow=0;

 if(!gets(inbuf)) return;
 s=inbuf;
 while(*s==' ' || *s=='\t') ++s;
 if(!*s) return;	/* Blank line signals end of table */

 /* Calculate no. columns */
 for(x=0;s[x];++x)
  if(s[x]=='|') ++ncols;
 if(!ncols) return;
 if(!--ncols) return;
 
 /* Allocate space for column widths and formats */
 wid=malloc(sizeof(double)*ncols);
 fmt=malloc(sizeof(int)*ncols);

 /* Now read in rows */
 do
  {
  struct row *row;
  row=malloc(sizeof(struct row));
  row->data=malloc(sizeof(char *)*ncols);
  row->next=0;
  /* Break row up into fields */
  rowfields(row->data,ncols,strdup(s));

  /* Save row */
  if(firstrow) lastrow->next=row, lastrow=row;
  else firstrow=lastrow=row;
  ++nrows;

  /* Next row */
  if(gets(inbuf))
   {
   s=inbuf;
   while(*s==' ' || *s=='\t') ++s;
   }
  else
   break;
  }
  while(*s);
 }

/* Database of tables */

struct table
  {
  struct table  *next;
  char *name;
  int nrows;
  int ncols;
  double *wid;
  int *fmt;
  struct row *firstrow;
  struct row *lastrow;
  } *tables;

void savetable(char *name)
  {
  struct table *t;
  for(t=tables;t;t=t->next)
    if(!strcmp(t->name,name))
      {
      fprintf(stderr,"Duplicate table '%s'\n",name);
      exit(-1);
      }
  t=malloc(sizeof(struct table));
  t->next=tables;
  tables=t;
  t->name=strdup(name);
  t->nrows=nrows;
  t->ncols=ncols;
  t->wid=wid;
  t->fmt=fmt;
  t->firstrow=firstrow;
  t->lastrow=lastrow;
/*
  nrows=0;
  ncols=0;
  wid=0;
  fmt=0;
  firstrow=0;
  lastrow=0;
*/
  }

void findtable(char *name)
  {
  struct table *t;
  for(t=tables;t;t=t->next)
    if(!strcmp(t->name,name))
      break;
  if(!t)
    {
    fprintf(stderr,"Table %s not found\n",name);
    exit(-1);
    }
  nrows=t->nrows;
  ncols=t->ncols;
  firstrow=t->firstrow;
  lastrow=t->lastrow;
  wid=t->wid;
  fmt=t->fmt;
  }

/* Add a register to database */

struct a_reg
  {
  struct a_reg *next;
  char *name;
  char *addr;
  char *def;
  } *first_reg, *last_reg;

struct a_field
  {
  struct a_field *next;
  char *name;
  char *start;
  char *size;
  char *type;
  };

struct a_def
  {
  struct a_def *next;
  char *name;
  struct a_field *fields;
  } *a_defs;

struct a_field *reverse(struct a_field *have,struct a_field *a)
  {
  if(a)
    {
    struct a_field *n=a->next;
    a->next=have;
    if(n)
      return reverse(a,n);
    else
      return a;
    }
  else
    return 0;
  }

void parse_fields()
  {
  struct a_field *a;
  struct row *row;
  int y;
  for(y=0,row=firstrow;y<nrows;++y,row=row->next)
    if(y>=2 && row->data && row->data[2] && row->data[2][0])
      {
      char buf[10];
      int i, j;
      a=malloc(sizeof(struct a_field));
      a->next=a_defs->fields;
      a_defs->fields=a;

      a->name=strdup(row->data[2]);

      if(!strcmp(row->data[1],"R"))
        a->type="_RdOnly";
      else if(!strcmp(row->data[1],"R/W"))
        a->type="_RdWr";
      else if(!strcmp(row->data[1],"R/W1TC"))
        a->type="_Wr1TC";
      else
        a->type="unknown";

      if(strchr(row->data[0],':'))
        {
        sscanf(row->data[0],"%d:%d",&i,&j);
        sprintf(buf,"%d",i-j+1);
        a->size=strdup(buf);
        sprintf(buf,"%d",j);
        a->start=strdup(buf);
        }
      else
        {
        i=atoi(row->data[0]);
        sprintf(buf,"1");
        a->size=strdup(buf);
        sprintf(buf,"%d",i);
        a->start=strdup(buf);
        }
      }
  };

void add_def(char *name)
  {
  struct a_def *a=malloc(sizeof(struct a_def));
  a->name=strdup(name);
  a->fields=0;
  a->next = a_defs;
  a_defs=a;
  parse_fields();
  }

void add_reg(char *name,char *type_name,char *description,int make_type)
  {
  struct a_reg *a=(struct a_reg *)malloc(sizeof(struct a_reg));
  int n;
  char buf[50];
  a->next=0;
  if(first_reg)
    {
    last_reg->next = a;
    last_reg=a;
    }
  else
    {
    last_reg=first_reg=a;
    }
  a->name=strdup(name+3);
  sscanf(description+2,"%x",&n);
  sprintf(buf,"0x%x",n);
  a->addr=strdup(buf);
  sprintf(buf,"%s_t",type_name+3);
  a->def=strdup(buf);
  if(make_type) add_def(buf);
  }

void dump_regs()
  {
  struct a_reg *a;
  struct a_def *b;

  printf("#include \"../../../verifshared/bin/register_attr.h\"\n\n");

  for(b=a_defs;b;b=b->next)
    {
    struct a_field *c;
    printf("struct %s {\n",b->name);
    b->fields=reverse(NULL,b->fields);
    for(c=b->fields;c;c=c->next)
      {
      int x;
      for(x=0;c->name[x];x++)
        if(c->name[x]=='[')
          {
          c->name[x]=0;
          break;
          }
      printf("  %s(%s, %s, %s);\n",c->type,c->name,c->size,c->start);
      }
    printf("};\n\n");
    }

  printf("struct %s_t CHIP_COLLECTION {\n",main_name);
  for(a=first_reg;a;a=a->next)
    {
    printf("  REGISTER(%s, %s, %s);\n",a->name,a->def,a->addr);
    }
  printf("} %s_top;\n",main_name);
  }

double calcwidth(double remain,int x,struct row *row)
  {
  if(wid[x]==0.0)
    return remain;
  else
    {
    double w=wid[x]-.15;
/*
    int y;
    for(y=x+1;y!=ncols;++y)
      if(!row->data[y])
        if(wid[y]==0.0)
          w+=remain;
        else
          w+=wid[y];
      else
        break;
*/
    return w;
    }
  }

int firstchap=0;

void gentable(int flg,char *name,char *text)
 {
 struct row *row;
 double remain;
 int x;
 row=firstrow;
 for(x=0;x!=ncols;++x)
  {
  if(row->data[x][0]=='l') fmt[x]=0;
  else if(row->data[x][0]=='c') fmt[x]=1;
  else if(row->data[x][0]=='r') fmt[x]=2;
  else fmt[x]=0;
  wid[x]=0.0;
  if(row->data[x][0]) sscanf(row->data[x]+1,"%lf",wid+x);
  }
 remain=6.5;
 for(x=0;x!=ncols;++x)
  remain-=wid[x]+.075;
 printf("\\medskip\n");
 if(!flg)
  {
  if(body)
   printf("\\table{%s}{Table \\the\\chnum.\\the\\tablenum \\ %s}\\tablew{%s}\n",name,text,text);
  else
   printf("%s\n",text);
  }
 printf("\\smallskip\n");
 printf("\\vbox{\\lineskiplimit=1000pt\n");
 printf("\\def\\strut{\\vrule height 10.5pt depth 5.5pt width 0pt}\n");
 printf("\\tabskip=0pt\\halign to \\hsize{\n");

 /* Column definitions */
 /* Left side */
 printf("\\strut#");
 if(flg)
  printf("&#");
 else
  printf("&\\vrule width 1pt#");

 for(x=0;x!=ncols;++x)
  {
  switch(fmt[x])
   {
   case 0: /* Left */
    if(wid[x]==0.0) printf("&\\quad#\\hfil");
    else printf("&\\hbox to %gin {\\quad#\\hfil}",wid[x]);
    break;

   case 1: /* Center */
    if(wid[x]==0.0) printf("&\\hfil#\\hfil");
    else printf("&\\hbox to %gin {\\hfil#\\hfil}",wid[x]);
    break;

   case 2: /* Right */
    if(wid[x]==0.0) printf("&\\hfil#\\quad");
    else printf("&\\hbox to %gin {\\hfil#\\quad}",wid[x]);
    break;
   }

  /* Inside vrule */
  if(x+1!=ncols)
   {
   if(flg)
    printf("&#");
   else
    printf("&\\null\\hskip .25pt\\vrule width .5pt\\hskip .25pt\\null#");
   }
  }

 /* Right side */
 printf("\\tabskip 0in plus 10in");
 if(flg)
  printf("&#\\tabskip=0pt\\cr\n");
 else
  printf("&\\vrule width 1pt#\\tabskip=0pt\\cr\n");

 /* Top rule */
 if(!flg)
  printf("\\noalign{\\hrule height 1pt}\n");

 /* Title row */
 if(!flg)
  {
  row=row->next;
  for(x=0;x!=ncols;++x)
    switch(fmt[x])
     {
     case 0: /* Left */
      printf("&&{$\\vcenter{\\hsize=%gin\\raggedright\\smallskip\\parskip=0pt\\lineskiplimit=0pt \\bold %s\\vrule height 0pt depth 3pt width 0pt\\par}$}",wid[x]==0.0?remain:wid[x]-.15,fix(row->data[x]));
      break;

     case 1: /* Center */
      printf("&&{$\\vcenter{\\hsize=%gin\\raggedboth\\smallskip\\parskip=0pt\\lineskiplimit=0pt \\bold %s\\vrule height 0pt depth 3pt width 0pt\\par}$}",wid[x]==0.0?remain:wid[x]-.15,fix(row->data[x]));
      break;

     case 2: /* Right */
      printf("&&{$\\vcenter{\\hsize=%gin\\raggedleft\\smallskip\\parskip=0pt\\lineskiplimit=0pt \\bold %s\\vrule height 0pt depth 3pt width 0pt\\par}$}",wid[x]==0.0?remain:wid[x]-.15,fix(row->data[x]));
      break;
     }
  printf("&\\cr\n");
  if(row->next)
   printf("\\noalign{\\hrule height 1pt}\n");
  }

 /* Data rows */
 for(row=row->next;row;row=row->next)
  {
  for(x=0;x!=ncols;++x)
   if(row->data[x])
     switch(fmt[x])
      {
      case 0: /* Left */
       printf("&&{$\\vcenter{\\hsize=%gin\\emergencystretch=.9\\hsize\\raggedright\\smallskip\\parskip=0pt\\lineskiplimit=0pt %s\\vrule height 0pt depth 3pt width 0pt\\par}$}",calcwidth(remain,x,row),fix(row->data[x]));
       break;

      case 1: /* Center */
       printf("&&{$\\vcenter{\\hsize=%gin\\emergencystretch=.9\\hsize\\raggedboth\\smallskip\\parskip=0pt\\lineskiplimit=0pt %s\\vrule height 0pt depth 3pt width 0pt\\par}$}",calcwidth(remain,x,row),fix(row->data[x]));
       break;

      case 2: /* Right */
       printf("&&{$\\vcenter{\\hsize=%gin\\emergencystretch=.9\\hsize\\raggedleft\\smallskip\\parskip=0pt\\lineskiplimit=0pt %s\\vrule height 0pt depth 3pt width 0pt\\par}$}",calcwidth(remain,x,row),fix(row->data[x]));
       break;
      }
   else
     printf("&&\\omit");

  printf("&\\cr\n");
  if(row->next && !flg)
   printf("\\noalign{\\hrule}\n");
  }

 /* Bottom rule */
 if(!flg)
  printf("\\noalign{\\hrule height 1pt}\n");
 printf("}}\n");
 }

int main()
 {
 char *s;		/* Input pointer */

 while(gets(inbuf))
  if(inbuf[0]=='.')
   { /* Special command */
   char *cmd;
   emitpara();
   s=inbuf+1;
   cmd=s;
   s=fields(s);
   if(!strcmp(cmd,""))
    { /* Section */
    char *fixeds=fix(s);
    emitstart();
    if(firstchap)
      {
      printf("\\chapter{\\vfill\\eject\\forceodd\\chapterfont \\bigbreak\\bigskip\\noindent\\the\\chnum. %s \\bigskip\\nobreak}\\chapterw{%s}\n",fixeds,fixeds);
      }
    else
      {
      printf("\\chapter{\\chapterfont \\bigbreak\\bigskip\\noindent\\the\\chnum. %s \\bigskip\\nobreak}\\chapterw{%s}\n",fixeds,fixeds);
      firstchap=1;
      }
    }
   else if(!strcmp(cmd,"#"))
    { /* Comment */
    }
   else if(!strcmp(cmd,"."))
    { /* Subsection */
    char *fixeds;
    emitstart();
    fixeds=fix(s);
    printf("\\section{\\sectionfont\\medbreak\\smallskip\\noindent\\the\\chnum.\\the\\snum\\ %s \\smallskip\\nobreak}\\sectionw{%s}\n",fixeds,fixeds);
    }
   else if(!strcmp(cmd,".."))
    { /* Subsubsection */
    char *fixeds=fix(s);
    emitstart();
    printf("\\subsection{\\ssectionfont\\medbreak\\smallskip\\noindent\\the\\chnum.\\the\\snum.\\the\\ssnum\\ %s \\nobreak\\smallskip\\nobreak}\\subsectionw{%s}\n",fixeds,fixeds);
    }
   else if(!strcmp(cmd,"..."))
    { /* Subsubsubsection (you've got to be kidding...) */
    char *fixeds=fix(s);
    emitstart();
    printf("\\subsubsection{\\ssectionfont\\medbreak\\smallskip\\noindent\\the\\chnum.\\the\\snum.\\the\\ssnum.\\the\\sssnum\\ %s \\nobreak\\smallskip\\nobreak}\\subsubsection{%s}\n",fixeds,fixeds);
    }
   else if(!strcmp(cmd,"set"))
    { /* Set a variable */
    s=fields(cmd=s);
    printf("\\newtoks\\%s\n\\%s={%s}\n",cmd,cmd,fix(s));
    }
   else if(!strcmp(cmd,"name"))
    { /* Document name for attr */
    strcpy(main_name,s);
    }
   else if(!strcmp(cmd,"list"))
    { /* A list */
    emitstart();
    }
   else if(!strcmp(cmd,"table"))
    { /* A table */
    char *name;
    char *description;
    s=fields(name=s);
    description=s;
    name=strdup(name);
    description=fix(description);
    emitstart();
    gettable();

    savetable(name);
    fprintf(stderr,"table %s\n",name);

    if(nrows && ncols) gentable(0,fixid(name),description);


    if(name[0]=='r' && name[1]=='e' && name[2]=='g')
      {
      // regIIC483 in name
      // 0x68 IIC for OC-48... in description
      add_reg(name,name,description,1); /* Create type */
      }
    }
   else if(!strcmp(cmd,"copy_table"))
    { /* A table */
    char *name;
    char *name_from;
    char *description;

    s=fields(name=s);
    s=fields(name_from=s);

    description=s;
    name=strdup(name);
    description=fix(description);
    emitstart();

    findtable(name_from);

    /* savetable(name); */

    if(nrows && ncols) gentable(0,fixid(name),description);

    if(name[0]=='r' && name[1]=='e' && name[2]=='g')
      {
      // regIIC483 in name
      // 0x68 IIC for OC-48... in description
      add_reg(name,name_from,description,0); /* Use existing type */
      }
    }
   else if(!strcmp(cmd,"alignment"))
    { /* An alignment */
    emitstart();
    gettable();
    if(nrows && ncols) gentable(1,"","");
    }
   else if(!strcmp(cmd,"page"))
    {
    emitstart();
    printf("\\null\\vfil\\eject\n");
    }
   else if(!strcmp(cmd,"centered"))
    {
    emitstart();
    printf("\\centerline{%s}\n",fix(s));
    }
   else if(!strcmp(cmd,"span"))
    {
    emitstart();
    printf("\\vfill\n");
    }
   else if(!strcmp(cmd,"space"))
    {
    emitstart();
    printf("\\vbox to %sin{}\n",s);
    }
   else if(!strcmp(cmd,"figure"))
    { /* A figure */
    char *name;
    char *file;
    char *description;
    s=fields(name=s);
    s=fields(file=s);
    description=s;
    emitstart();
    name=fixid(name);
    description=fix(description);
    printf("\\advance\\fignum by 1\n");
    printf("\
\\bigskip\n\
\\line{\\hss\\vbox{\\hbox{\\epsffile{%s}\\hfill}\\hbox{\\figure{%s}{Figure \\the\\chnum.\\the\\fignum \\ %s}\\figurew{%s}\\hfill}}\\hss}\n\
",file,name,description,description);
    }
   else if(!strcmp(cmd,"contents"))
    {
    emitstart();
    printf("\
\\newread\\toc \\immediate\\openin\\toc=\\jobname.toc\n\
\\ifeof\\toc\n\
 \\message{! No file \\jobname.toc;}\n\
\\else\n\
 \\vfil\\eject\\forceodd\\chapterfont\\noindent Contents\\par\\textfont  \\input\\jobname.toc\\vfil\\eject\n\
\\fi\n\
\\newwrite\\toc\n\
\\immediate\\openout\\toc=\\jobname.toc\n\
");
    }
   else if(!strcmp(cmd,"figures"))
    {
    emitstart();
    printf("\
\\newread\\lof \\immediate\\openin\\lof=\\jobname.lof\n\
\\ifeof\\lof\n\
 \\message{! No file \\jobname.lof;}\n\
\\else\n\
 \\vfil\\eject\\forceodd\\chapterfont\\noindent Figures\\par\\textfont  \\input\\jobname.lof\\vfil\\eject\n\
\\fi\n\
\\newwrite\\lof\n\
\\immediate\\openout\\lof=\\jobname.lof\n\
");
    }
   else if(!strcmp(cmd,"tables"))
    {
    emitstart();
    printf("\
\\newread\\lot \\immediate\\openin\\lot=\\jobname.lot\n\
\\ifeof\\lot\n\
 \\message{! No file \\jobname.lot;}\n\
\\else\n\
 \\vfil\\eject\\forceodd\\chapterfont\\noindent Tables\\par\\textfont  \\input\\jobname.lot\\vfil\\eject\n\
\\fi\n\
\\newwrite\\lot\n\
\\immediate\\openout\\lot=\\jobname.lot\n\
");
    }
   else if(!strcmp(cmd,"body"))
    {
    emitstart();

/* \\immediate\\write\\bdy{\\the\\count0}\n\   correct way to write page no. to body file */

/* \\pageno=1\n\   put this on line before textfont to start page numbers after intro */

    if(!body)
     printf("\
\\forceodd\n\
\\immediate\\write\\bdy{1}\n\
\\evenpagehead={\\textfont\\folio\\hfil \\the\\header}\n\
\\oddpagehead={\\textfont \\the\\header\\hfil\\folio}\n\
\\footline={\\textfont\\hfil \\the\\footer \\hfil}\n\
\\textfont\n\
");
    body=1;
    }
   }
  else
   {
   int x;
   /* Skip leading whitespace */
   for(x=0;inbuf[x]==' ' || inbuf[x]=='\t';++x);

   /* Test if have a paragraph or a blank line */
   if(!inbuf[x])
    /* It's a blank line... emit current paragraph */
    emitpara();
   else
    /* Line is not blank, append text to current paragraph */
    addpara(inbuf+x);
   }
 emitpara();
 printf("\\bye\n");
 sprintf(inbuf,"%s.attr",main_name);
 freopen(inbuf,"w",stdout);
 dump_regs();
 return 0;
 }
