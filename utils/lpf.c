/* Convert PCL to Postscript, or pass Postscript right through */

#include <stdio.h>

/* Set-up file parameters */
int fontsize=12;	/* Font size */
int ncopies=1;

/* Current state */

int res=720;		/* Resolution (dots per inch) */
int pres=72;		/* Post-script units */
int state=0;		/* Input state */
int num=0;		/* Input number */
int rel=0;
int cursor_y;		/* Cursor y position */
int cursor_x;		/* Cursor x position */
int height;		/* Character height */
int width;		/* Character width */
int lmargin;
int tmargin;

void izpcl()
 {
 printf("%%!PS-Adobe-2.0\n");
 printf("/#copies %d def\n",ncopies);
 printf("/Courier-Bold findfont %d scalefont setfont\n",fontsize);
 lmargin=res/4;
 tmargin=res/2+res/6;
 cursor_x=0;
 cursor_y=0;
 height=res/6;
 width=res/10;
 }

char line[1024];
int linep=0;

void flshopen()
 {
 printf("%d %d moveto (",(cursor_x+lmargin)*pres/res,
                        (res*11-tmargin-cursor_y)*pres/res);
 }

void flshc(int c)
 {
 if(c=='\\') putchar('\\'), putchar('\\');
 else if(c=='(') putchar('\\'), putchar('(');
 else if(c==')') putchar('\\'), putchar(')');
 else putchar(c);
 cursor_x+=width;
 }

void flshclose()
 {
 printf(") show\n");
 }

void flshline()
 {
 int x;
 int state=0;
 for(x=0;x!=linep;++x)
  if(line[x]=='\b')
   {
   if(state) flshclose(), state=0;
   cursor_x-=width;
   }
  else
   {
   if(!state) flshopen(), state=1;
   flshc(line[x]);
   }
 if(state) flshclose();
 linep=0;
 }

void flshpage()
 {
 flshline();
 printf("showpage\n");
 cursor_y=0;
 }

void nextline()
 {
 flshline();
 cursor_y+=height;
 cursor_x=0;
 if(cursor_y>=10*res) flshpage();
 }

void pcl(int c)
 {
 switch(state)
  {
  case 0:
   if(c>=' ' && c<='~') line[linep++]=c;
   else if(c=='\b') line[linep++]=c;
   else if(c=='\t') do line[linep++]=' '; while(linep&7);
   else if(c=='\n') nextline();
   else if(c=='\r') { flshline(); cursor_x=0; }
   else if(c=='\f') flshpage();
   else if(c=='\033') state=1;
   break;

  case 1: /* ESC */
   if(c=='&') state=2;
   else if(c=='(') state=3;
   else if(c==')') state=4;
   else if(c=='*') state=5;
   else if(c==27) state=1;
   else state=0;
   break;

  case 2: /* ESC & */
   if(c=='a') state=6, num=0, rel=0;
   else if(c==27) state=1;
   else state=0;
   break;

  case 3: /* ESC ( */
   if(c=='s') state=7, num=0, rel=0;
   else if(c==27) state=1;
   else state=0;
   break;

  case 4: /* ESC ) */

  case 5: /* ESC * */
  
  case 6: /* ESC & a */
   if(c>='0' && c<='9') num=num*10+c-'0';
   else if(c=='C')
    {
    flshline();
    if(rel==0) cursor_x=width*num;
    else if(rel==1) cursor_x+=width*num;
    else if(rel== -1) cursor_x-=width*num;
    state=0;
    }
   else if(c==27) state=1;
   else if(c=='-') rel= -1;
   else if(c=='+') rel=1;
   else state=0;
   break;

  case 7: /* ESC ( s */
   if(c>='0' && c<='9') num=num*10+c-'0';
   else if(c=='-') rel= -1;
   else if(c=='+') rel= 1;
   else if(c==27) state=1;
   else if(c=='P')
    {
    flshline();
    if(num==0) printf("/Courier-Bold findfont %d scalefont setfont\n",fontsize);
    else if(num==1) printf("/Times-Roman findfont %d scalefont setfont\n",fontsize-fontsize/6);
    state=0;
    }
   else state=0;
   break;
  }
 }

void pclflsh()
 {
 if(cursor_y) flshpage();
 }

int main(int argc, char *argv[])
 {
 FILE *f;
 int a, b;
 /* Load configuration */
 f=fopen("/usr/local/lib/lpfsetup","r");
 if(f)
  {
  fscanf(f,"size=%d copies=%d",&fontsize,&ncopies);
  fclose(f);
  }

 a=getchar();
 if(a==4) a=getchar();
 b=getchar();
 if((a=='%' && b=='!') || (a=='\033' && b=='%'))
  { /* Input is postscript.  Pass is right through */
  int state=0;
  putchar(a); putchar(b);
  if(ncopies==1) while((a=getchar())!= -1) putchar(a);
  else while((a=getchar())!= -1)
   switch(state)
    {
    case 0: if(a=='N') ++state;
            else putchar(a);
            break;

    case 1: if(a=='u') ++state;
            else fputs("N",stdout), putchar(a), state=0;
            break;

    case 2: if(a=='m') ++state;
            else fputs("Nu",stdout), putchar(a), state=0;
            break;

    case 3: if(a=='C') ++state;
            else fputs("Num",stdout), putchar(a), state=0;
            break;

    case 4: if(a=='o') ++state;
            else fputs("NumC",stdout), putchar(a), state=0;
            break;

    case 5: if(a=='p') ++state;
            else fputs("NumCo",stdout), putchar(a), state=0;
            break;
            
    case 6: if(a=='i') ++state;
            else fputs("NumCop",stdout), putchar(a), state=0;
            break;

    case 7: if(a=='e') ++state;
            else fputs("NumCopi",stdout), putchar(a), state=0;
            break;

    case 8: if(a=='s')
             {
             while((a=getchar())!=-1) if(a=='p') break;
             if(a!=-1) printf("NumCopies %d p",ncopies);
             state=0;
             }
            else fputs("NumCopie",stdout), putchar(a), state=0;
            break;
    }
  }
 else
  { /* Input is PCL.  Give it to PCL interpreter */
  izpcl();
  pcl(a);
  pcl(b);
  while((a=getchar())!= -1) pcl(a);
  pclflsh();
  }
 return 0;
 }
