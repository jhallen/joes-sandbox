/* rc file parser */

#include <stdio.h>
#include "zstr.h"
#include "macro.h"
#include "cmd.h"
#include "vs.h"
#include "va.h"
#include "path.h"
#include "termcap.h"
#include "rc.h"

static struct context
 {
 struct context *next;
 char *name;
 KMAP *kmap;
 } *contexts=0;		/* List of named contexts */

/* Find a context of a given name- if not found, one with an empty kmap
 * is created.
 */

KMAP *getcontext(name)
char *name;
 {
 struct context *c;
 for(c=contexts;c;c=c->next) if(!zcmp(c->name,name)) return c->kmap;
 c=(struct context *)malloc(sizeof(struct context));
 c->next=contexts;
 c->name=zdup(name);
 contexts=c;
 return c->kmap=mkkmap();
 }

/* Process rc file
 * Returns 0 if the rc file was succefully processed
 *        -1 if the rc file couldn't be opened
 *         1 if there was a syntax error in the file
 */

int procrc(cap,name)
CAP *cap;
char *name;
 {
 KMAP *context=0;		/* Current context */
 unsigned char buf[1024];	/* Input buffer */
 FILE *fd;			/* rc file */
 int line=0;			/* Line number */
 int err=0;			/* Set to 1 if there was a syntax error */
 ossep(zcpy(buf,name));
#ifdef __MSDOS__
 fd=fopen(buf,"rt");
#else
 fd=fopen(buf,"r");
#endif
 
 if(!fd) return -1;		/* Return if we couldn't open the rc file */
 
 fprintf(stderr,"Processing '%s'...",name); fflush(stderr);
 
 while(++line, fgets(buf,1024,fd))
  switch(buf[0])
   {
   case ' ': case '\t': case '\n': case '\f': case 0:
   break;	/* Skip comment lines */

   case ':':	/* Select context */
    {
    int x, c;
    for(x=1;!cwhitef(buf[x]);++x);
    c=buf[x]; buf[x]=0;
    if(x!=1)
     if(!zcmp(buf+1,"def"))
      {
      int y;
      for(buf[x]=c;cwhite(buf[x]);++x);
      for(y=x;!cwhitef(buf[y]);++y);
      c=buf[y]; buf[y]=0;
      if(y!=x)
       {
       int sta;
       MACRO *m;
       if(cwhite(c) && (m=mparse(NULL,buf+y+1,&sta)))
        addcmd(buf+x,m);
       else
        {
        err=1;
        fprintf(stderr,"\n%s %d: macro missing from :def",name,line);
        }
       }
      else
       {
       err=1;
       fprintf(stderr,"\n%s %d: command name missing from :def",name,line);
       }
      }
     else if(!zcmp(buf+1,"inherit"))
      if(context)
       {
       for(buf[x]=c;cwhite(buf[x]);++x);
       for(c=x;!cwhitef(buf[c]);++c);
       buf[c]=0;
       if(c!=x) kcpy(context,getcontext(buf+x));
       else
        {
        err=1;
        fprintf(stderr,"\n%s %d: context name missing from :inherit",name,line);
        }
       }
      else
       {
       err=1;
       fprintf(stderr,"\n%s %d: No context selected for :inherit",name,line);
       }
     else if(!zcmp(buf+1,"include"))
      {
      for(buf[x]=c;cwhite(buf[x]);++x);
      for(c=x;!cwhitef(buf[c]);++c);
      buf[c]=0;
      if(c!=x)
       {
       switch(procrc(buf+x))
        {
        case 1: err=1; break;
        case -1: fprintf(stderr,"\n%s %d: Couldn't open %s",name,line,buf+x);
                 err=1; break;
        }
       context=0;
       }
      else
       {
       err=1;
       fprintf(stderr,"\n%s %d: :include missing file name",name,line);
       }
      }
     else if(!zcmp(buf+1,"delete"))
      if(context)
       {
       int y;
       for(buf[x]=c;cwhite(buf[x]);++x);
       for(y=x;buf[y]!=0 && buf[y]!='\t' && buf[y]!='\n' &&
               (buf[y]!=' ' || buf[y+1]!=' ');++y);
       buf[y]=0;
       kdel(context,buf+x);
       }
      else
       {
       err=1;
       fprintf(stderr,"\n%s %d: No context selected for :delete",name,line);
       }
     else context=getcontext(buf+1);
    else
     {
     err=1;
     fprintf(stderr,"\n%s %d: Invalid context name",name,line);
     }
    }
   break;

   default:	/* Get key-sequence to macro binding */
    {
    int x, y, c;
    MACRO *m;
    if(!context)
     {
     err=1;
     fprintf(stderr,"\n%s %d: No context selected for macro to key-sequence binding",name,line);
     break;
     }

    m=0;
    macroloop:
    m=mparse(m,buf,&x);
    if(x== -1)
     {
     err=1;
     fprintf(stderr,"\n%s %d: Unknown command in macro",name,line);
     break;
     }
    else if(x== -2)
     {
     fgets(buf,1024,fd);
     goto macroloop;
     }
    if(!m) break;

    /* Skip to end of key sequence */
    for(y=x;buf[y]!=0 && buf[y]!='\t' && buf[y]!='\n' && 
            (buf[y]!=' ' || buf[y+1]!=' ');++y);
    buf[y]=0;

    /* Add binding to context */
    if(kadd(cap,context,buf+x,m)== -1)
     {
     fprintf(stderr,"\n%s %d: Bad key sequence '%s'",name,line,buf+x);
     err=1;
     }
    }
   break;
   }
 fclose(fd);			/* Close rc file */

 /* Print proper ending string */
 if(err) fprintf(stderr,"\ndone\n");
 else fprintf(stderr,"done\n");

 return err;			/* 0 for success, 1 for syntax error */
 }
