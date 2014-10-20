/* Stuff pin assignments from XACT report file into OrCAD lib source file */

#include <stdio.h>
#include <string.h>

extern void *malloc();
extern void *realloc();

struct pin
 {
 char *name;
 char *num;
 } *pins;	/* List of pins */

int npins;	/* No. pins on part */
int pinssiz;	/* Malloc size of pins */

/* Compare first strlen(b) chars of a */

int cmp(a,b)
char *a, *b;
 {
 while(*b && *a==*b) ++a, ++b;
 if(!*b) return 0;
 else return 1;
 }

main(argc,argv)
char *argv[];
 {
 FILE *f, *g;
 char buf[1024];
 char bf[128];
 int x, y, z, q;
 if(argc!=4)
  {
  fprintf(stderr,"pins decompiled-library-name part-name report-file-name\n");
  return 1;
  }
 /* Pass 1 through library: find part and collect pin names */
 /* Part begins with 'part-name' at beginning of line */
 sprintf(bf,"\'%s\'",argv[2]);
 /* Allocate space for pins */
 pins=malloc((pinssiz=128)*sizeof(struct pin));
 /* Open library */
 if(!(f=fopen(argv[1],"r")))
  {
  fprintf(stderr,"Couldn't open %s\n",argv[1]);
  return 1;
  }
 /* Scan library */
 while(fgets(buf,1023,f))
  if(!cmp(buf,bf))
   break;
 if(cmp(buf,bf))
  {
  fprintf(stderr,"Part %s not found\n",argv[2]);
  return 1;
  }
 /* Skip header line */
 fgets(buf,1023,f);
 /* Collect pin names */
 while(fgets(buf,1023,f) && strlen(buf)>23)
  {
  for(x=23;buf[x] && buf[x]!='\'';++x);
  buf[x]=0;
  if(npins==pinssiz) pins=realloc(pins,(pinssiz*=2)*sizeof(struct pin));
  pins[npins].name=strcpy(malloc(strlen(buf+23)+1),buf+23);
  pins[npins].num=0;
  ++npins;
  }
 fclose(f);

 /* Scan report file */
 f=fopen(argv[3],"r");
 if(!f)
  {
  fprintf(stderr,"Couldn't open report file\n");
  return 1;
  }
 /* Find lines with a number and a pin name on them */
 while(fgets(buf,1023,f))
  {
  for(x=0;buf[x];++x)
   if(buf[x]>='0' && buf[x]<='9')
    { /* Check if one of our pins was mentioned in this line... */
    for(q=x;buf[q]>='0' && buf[q]<='9';++q);
    if(buf[q]!=0 && buf[q]!='\n' && buf[q]!=' ' && buf[q]!='\t' &&
       buf[q]!=':' && buf[q]!=',' && buf[q]!='|' && buf[q]!=';')
     {
     x=q;
     goto next;
     }
    for(y=0;buf[y];++y)
     for(z=0;z!=npins;++z)
      /* Search for name seperated with non-identifier character on
         the left and whitespace on the right */
      if((!y || !(buf[y-1]>='a' && buf[y-1]<='z' ||
                  buf[y-1]>='A' && buf[y-1]<='Z' ||
                  buf[y-1]>='0' && buf[y-1]<='9' ||
                  buf[y-1]=='_' || buf[y-1]=='$')) &&
         !cmp(buf+y,pins[z].name) &&
         (buf[y+strlen(pins[z].name)]==' ' ||
          buf[y+strlen(pins[z].name)]=='\t' ||
          buf[y+strlen(pins[z].name)]=='|' ||
          buf[y+strlen(pins[z].name)]==';' ||
          buf[y+strlen(pins[z].name)]==',' ||
          buf[y+strlen(pins[z].name)]==':' ||
          buf[y+strlen(pins[z].name)]=='\n'))
       { /* Pin name was found! */
       /* Make sure number we found is not in pin name */
       if(q>=y && q<=y+strlen(pins[z].name))
        {
        x=q;
        goto next;
        }
       /* Skip if we already found pin number */
       if(pins[z].num) goto done;
       /* Store pin number */
       buf[q]=0;
       pins[z].num=strcpy(malloc(strlen(buf+x)+1),buf+x);
       goto done;
       }
    next:;
    }
  done:;
  }
 /* Print pin assignments */
 for(x=0;x!=npins;++x)
  {
  printf("%15s %3s",pins[x].name,pins[x].num?pins[x].num:"---");
  if((x&3)==3) printf("\n");
  else printf(" ");
  }
 if(x&3) printf("\n");
 /* Pass 2 through library - copy it to tmp.tmp as we scan */
 if(!(f=fopen(argv[1],"r")))
  {
  fprintf(stderr,"Couldn't open %s\n",argv[1]);
  return 1;
  }
 if(!(g=fopen("tmp.tmp","w")))
  {
  fprintf(stderr,"Couldn't open tmp.tmp\n");
  return 1;
  }
 /* Find part in library */
 while(fgets(buf,1023,f))
  {
  fputs(buf,g);
  if(!cmp(buf,bf)) break;
  }
 if(cmp(buf,bf))
  {
  fprintf(stderr,"Part not found on second pass... huh?\n");
  return 1;
  }
 fgets(buf,1023,f);
 fputs(buf,g);
 /* Overlay pin numbers */
 while(fgets(buf,1023,f))
  if(strlen(buf)>23)
   {
   for(y=0;y!=npins;++y)
    {
    sprintf(bf,"%s'",pins[y].name);
    if(!cmp(buf+23,bf))
     {
     char ff[10];
     sprintf(ff,"%3s",pins[y].num);
     if(pins[y].num)
      {
      buf[6]=ff[0];
      buf[7]=ff[1];
      buf[8]=ff[2];
      }
     break;
     }
    }
   fputs(buf,g);
   }
  else
   {
   fputs(buf,g);
   break;
   }
 /* Copy rest of file */
 while(fgets(buf,1023,f)) fputs(buf,g);
 if(ferror(f) || fclose(f))
  {
  fprintf(stderr,"Error reading file\n");
  return 1;
  }
 if(ferror(g) || fclose(g))
  {
  fprintf(stderr,"Error writing file\n");
  return 1;
  }
 /* Now copy temporary file into library */
 if(!(f=fopen("tmp.tmp","r")))
  {
  fprintf(stderr,"Couldn't open tmp.tmp\n");
  return 1;
  }
 if(!(g=fopen(argv[1],"w")))
  {
  fprintf(stderr,"Couldn't open %s\n",argv[1]);
  return 1;
  }
 while(fgets(buf,1023,f)) fputs(buf,g);
 fclose(f); fclose(g);
 unlink("tmp.tmp");
 fprintf(stderr,"Pin assignments complete\n");
 return 0;
 }
