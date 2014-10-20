/* Input buffer pointer */

#include <stdio.h>
#include <string.h>

char inbuf[1024];
char *in;

int line;

error(line,text)
char *text;
 {
 fprintf(stderr,"%d: %s\n",line+1,text);
 }

/* Output buffer and pointer */

char obuf[8192];
char *out=obuf;

oprintf(s,a0,a1,a2,a3,a4)
char *s;
 {
 sprintf(out,s,a0,a1,a2,a3,a4);
 out+=strlen(out);
 } 

/* Name binding system */

/* A hash table entry */

struct saved
 {
 struct saved *next;	/* Next entry with same hash value */
 char *name;		/* Name of this entry */
 
 char *text;		/* If text is bound to the name */

 int (*func)();		/* If an internal function is bound to the name */

 char **args;		/* Named arguments to be passed to the function */
 int nargs;		/* No. of arguments expected */
 };

/* A Scoping level */

struct level
 {
 struct saved *htab[256];	/* Hash table of bindings */
 struct level *next;		/* Next outer scoping level */
 char **args;			/* Args passed to this level */
 int nargs;
 } *top, *root;

int hash(s)
char *s;
 {
 unsigned long accu=0;
 while(*s) accu=accu*34857+*s++;
 return accu&0x7FFFFFFF;
 }

struct saved *get(name)
char *name;
 {
 struct level *l=top;
 struct saved *s;
 int h=hash(name);
 do
  for(s=l->htab[h%256];s;s=s->next) if(!strcmp(s->name,name)) return s;
  while(l=l->next);
 return 0;
 }

set(lvl,name,text,func,argc,args) 
struct level *lvl;
int (*func)();
char *name, *text;
char **args;
 {
 struct saved *s;
 int h=hash(name);
 int x;
 for(s=lvl->htab[h%256];s;s=s->next) if(!strcmp(s->name,name)) break;
 if(s)
  {
  if(s->text) free(s->text);
  for(x=0;x!=s->nargs;++x) free(s->args[x]);
  if(s->nargs) free(s->args);
  }
 else
  {
  s=(struct saved *)malloc(sizeof(struct saved));
  s->name=strdup(name);
  s->next=lvl->htab[h%256];
  lvl->htab[h%256]=s;
  }
 s->text=text;
 s->func=func;
 s->nargs=argc;
 if(argc) s->args=(char **)malloc(sizeof(char *)*argc);
 for(x=0;x!=argc;++x) s->args[x]=strdup(args[x]);
 }

push(nargs,args)
char **args;
 {
 struct level *l=(struct level *)malloc(sizeof(struct level));
 int x;
 for(x=0;x!=256;++x) l->htab[x]=0;
 l->next=top;
 l->args=args;
 l->nargs=nargs;
 top=l;
 }

pop()
 {
 int x;
 struct saved *s, *n;
 struct level *t=top;
 for(x=0;x!=256;++x)
  for(s=top->htab[x];s;s=n)
   {
   int y;
   n=s->next;
   free(s->name);
   if(s->text) free(s->text);
   for(y=0;y!=s->nargs;++y) free(s->args[y]);
   if(s->nargs) free(s->args);
   }
 top=top->next;
 free(t);
 }

popall()
 {
 while(top->next) pop();
 }

/** Text generator **/

/* 8 dots/mm */

/* Character widths in dots if font width of 8782 is used */

int varfont[]=
 {
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

 2602, 2602, 4228, 4228, 4228, 7969, 5367, 2602,
 2602, 2602, 4228, 7969, 2602, 2602, 2602, 2602,
 4228, 4228, 4228, 4228, 4228, 4228, 4228, 4228,
 4228, 4228, 2602, 2602, 8782, 7969, 8782, 3903,

 7969, 4879, 4879, 4716, 5204, 4391, 4391, 5204,
 5367, 2439, 3903, 4879, 4228, 6668, 5367, 5042,
 4879, 5042, 5204, 4716, 4391, 5367, 4716, 7156,
 4879, 4879, 4391, 2602, 2602, 2602, 4391, 4391,

 2602, 4066, 4391, 3903, 4391, 4228, 2439, 4391,
 4391, 2277, 2277, 3903, 2277, 6668, 4391, 4228,
 4391, 4391, 2927, 3740, 2439, 4391, 3903, 5855,
 3903, 3903, 3415, 4391, 4391, 4391, 8782, 8782,
 
 4716, 4391, 4228, 4066, 4066, 4066, 4066, 3903,
 4228, 4228, 4228, 2277, 2277, 2277, 4879, 4879, 
 4391, 6343, 7644, 4228, 4228, 4228, 4391, 4391,
 3903, 5042, 5367, 4228, 4228, 5042, 7969, 4228, 

 4066, 2277, 4228, 4391, 4391, 5367, 3253, 3253,
 3903, 4391, 7969, 7969, 7969, 2602, 3578, 3578,
 7318, 8782, 8782, 8782, 8782, 4879, 4879, 4879,
 4391, 8782, 8782, 8782, 8782, 4228, 4228, 8782,

 8782, 8782, 8782, 8782, 8782, 8782, 4066, 4879,
 8782, 8782, 8782, 8782, 8782, 8782, 8782, 4228,
 4228, 5204, 4391, 4391, 4391, 2277, 2439, 2439,
 2439, 8782, 8782, 8782, 8782, 4391, 2439, 8782,
 
 5042, 4554, 5042, 5042, 4228, 5042, 5204, 4391,
 4879, 5367, 5367, 5367, 3903, 4879, 4228, 4228,
 2602, 7969, 4391, 7969, 4228, 4228, 7969, 4228,
 4228, 4228,  813, 3253, 3253, 3253, 4391,    0
 };

struct font
 {
 int id;		/* Font I.D. letter */
 int width;		/* Width of font */
 int sep;		/* Seperation */
 int height;		/* Height of font */
 int base;		/* Font's baseline */
 int vsep;		/* Line seperation */
 int voff;		/* Vertical placement adjustment */
 } fonts[]=
 {
  { 'A',  5, 1,  9,  7,  2, 0 },
  { 'B',  7, 2, 11, 11,  8, 0 },
  { 'D', 10, 2, 18, 14,  5, 1 },
  { 'E', 15, 5, 28, 23,  5, 1 },
  { 'F', 13, 3, 26, 21,  5, 1 },
  { 'G', 40, 8, 60, 46,  8, 0 },
  { 'H', 13, 6, 21, 21, 10, 1 },
  { '0',  0, 0,  0,  0,  0, 2 },	/* Proportional space */
  {   0,  0, 0,  0,  0,  0, 2 },	/* (c) (r) */
  { 'X', 24, 0, 24, 24,  0, 0 }		/* Ivy font */
 };

int fontsel= 7;		/* Font select (7 is variable font) */
int fwidth=24;		/* Font width */
int fheight=24;		/* Font height */
int y=0;		/* Current y position */
int ind=2;		/* Indent value */

int mode;		/* 0 for wrap, 1 for center, 2 for upc code */

int fillmode;		/* 1 if text should be right-justified */

int outbuf[8192];	/* Buffer where text is built */
int outx;		/* Index into above buffer */

int lwidth=290;		/* Label width */
int loffset=0;		/* Left offset */

/* Append text to outbuf/outx - add current font metrics */

void gentext(s)
char *s;
 {
 int x,y;
 for(x=0,y=0;s[x];++x,++y)
  {
  int c=s[x];
  if(c=='\\' && s[x+1])
   switch(c=s[++x])
    {
    case '2': c=171; break;
    case '3': c=243; break;
    case '4': c=172; break;
    case '7': c=251; break;
    case '8': c=253; break;
    case '9': c=252; break;
    case '$': c=189; break;
    case '\'': c=167; break;
    case '\"': c=248; break;
    case '.': c=250; break;
    case '*': c=254; break;
    case '+': c=241; break;
    case 'r': c=169; break;
    case 'c': c=184; break;
    case '?': c=168; break;
    case '!': c=173; break;
    case 'a': c=160; break;
    case 'e': c=130; break;
    case 'i': c=161; break;
    case 'o': c=162; break;
    case 'u': c=163; break;
    case 'A': c=181; break;
    case 'E': c=144; break;
    case 'I': c=214; break;
    case 'O': c=224; break;
    case 'U': c=233; break;
    case 'n': c=164; break;
    case 'N': c=165; break;
    }
  outbuf[outx+y]=c+(fheight<<8)+(fwidth<<16)+(fontsel<<24);
  }
 outx+=y;
 }

/* Compute width of text segment */

int width(outbuf,len)
int *outbuf;
 {
 int x;
 int w=0;
 for(x=0;x!=len;++x)
  {
  int f=(outbuf[x]>>24);
  int wth=(255&(outbuf[x]>>16));
  int c=(255&outbuf[x]);
  if(fonts[f].width) w+=wth*(fonts[f].width+fonts[f].sep);
  else if(f==8) w+=wth;
  else w+=(wth*(varfont[c]+284))/8782;
  }
 return w;
 }

maxheight(outbuf,len)
int *outbuf;
 {
 int x;
 int b=0;
 for(x=0;x!=len;++x)
  {
  int f=(outbuf[x]>>24);
  int hgt=(255&(outbuf[x]>>8));
  int bas;
  if(fonts[f].width) bas=(fonts[f].height+fonts[f].vsep)*hgt;
  else bas=hgt+hgt/6;
  if(bas>b) b=bas;
  }
 return b;
 }

/* Compute height above baseline of text segment */

maxabove(outbuf,len)
int *outbuf;
 {
 int x;
 int b=0;
 for(x=0;x!=len;++x)
  {
  int f=(outbuf[x]>>24);
  int hgt=(255&(outbuf[x]>>8));
  int bas;
  if(fonts[f].width) bas=fonts[f].base*hgt;
  else bas=(hgt*3)/4;
  if(bas>b) b=bas;
  }
 return b;
 }

/* Compute height below baseline of text segment */

maxbelow(outbuf,len)
int *outbuf;
 {
 int x;
 int b=0;
 for(x=0;x!=len;++x)
  {
  int f=(outbuf[x]>>24);
  int hgt=(255&(outbuf[x]>>8));
  int bas;
  if(fonts[f].width) bas=(fonts[f].height-fonts[f].base)*hgt;
  else bas=hgt/4;
  if(bas>b) b=bas;
  }
 return b;
 }

/* Output a text segment */

int pf= -1, ee=0, ox= -1, oy= -1;

void tgen(x,base,from,to)
 {
 int z;
 x+=loffset;
 for(z=from;z!=to;)
  {
  int f=(outbuf[z]&0xFFFFFF00);
  int fon=(outbuf[z]>>24);
  int h=((outbuf[z]>>8)&255);
  int w=((outbuf[z]>>16)&255);
  int vadj=fonts[fon].voff;
  int ny=y+vadj;
  if(fonts[fon].width)
   w*=fonts[fon].width, h*=fonts[fon].height;

  if(ee)
   {
   ee=0;
   if(ox!=x || ny!=oy || pf!=f)
    {
    if(oy!=ny) oprintf("^FS^FT%d,%d",x,ny+base);
    else oprintf("^FS^FT%d,",x);
    if(pf!=f)
     if(fon==8) oprintf("^GSN,%d,%d^FD",h,w);
     else oprintf("^CF%c,%d,%d^FD",fonts[fon].id,h,w);
    else oprintf("^FD");
    }
   }
  else
   {
   if(oy!=ny) oprintf("^FT%d,%d",x,ny+base);
   else oprintf("^FT%d,",x);
   if(pf!=f)
    if(fon==8) oprintf("^GSN,%d,%d^FD",h,w);
    else oprintf("^CF%c,%d,%d^FD",fonts[fon].id,h,w);
   else oprintf("^FD");
   }

  while(z!=to && f==(outbuf[z]&0xFFFFFF00))
   {
   oprintf("%c",outbuf[z]&0xFF);
   x+=width(outbuf+z,1);
   ++z;
   }

  ee=1; ox=x; oy=ny, pf=f;
  }
 }

/* Fill text */

int pad, psu;
int nad;

int fillt(x,base,from,to,flg)
 {
 int sw=width(outbuf+from,to-from);	/* Width of source */
 int dw=lwidth-x;			/* Width of destination */
 int ad=dw-sw;				/* Amount of additional space needed */ 
 int su=(to-from)-1;			/* No. spaces we can add space */
 if(flg)
  {
  if(nad) pad/=nad, psu/=nad, pad=pad*su/psu;
  else pad=0;
  if(pad<ad) ad=pad;
  }
 else pad+=ad, psu+=su, ++nad;
 if(ad>0)
  {
  int q;
  int acc=ad;
  for(q=from;q!=to;++q)
   {
   int w=width(outbuf+q,1);
   tgen(x,base,q,q+1);
   while((acc-=su)>=0) ++x;
   acc+=su+ad;
   x+=w;
   }
  }
 else tgen(x,base,from,to);
 }

cntrit(m,from,to,b)
 {
 int w=width(outbuf+from,to-from);
 tgen(m-w/2,b,from,to);
 }

outtext()
 {
 int x;
 int flg=0;
 for(x=0;x!=outx;++x) if((outbuf[x]&255)=='~') flg=1;
 if(flg)
  {
  int l=0,r=outx;
  int b=maxabove(outbuf,outx);
  int h=maxheight(outbuf,outx);
  int ll;
  if((outbuf[0]&255)!='~')
   {
   for(l=0;(outbuf[l]&255)!='~';++l);
   tgen(0,b,0,l);
   }
  if((outbuf[outx-1]&255)!='~')
   {
   for(r=outx;(outbuf[r-1]&255)!='~';--r);
   tgen(lwidth-width(outbuf+r,outx-r),b,r,outx);
   }
  for(x=l,flg=0;x!=r;++x) if((outbuf[x]&255)=='~') ++flg;
  if(flg>1)
   {
   ll=lwidth/flg;
   for(x=0;x!=flg-1;++x)
    {
    int zz;
    ++l;
    for(zz=l;(outbuf[zz]&255)!='~';++zz);
    cntrit(ll,l,zz,b);
    l=zz;
    ll+=lwidth/flg;
    }
   }
  y+=h;
  }
 else if(mode==2) ;
 else
  {
  int start=0;
  nad=0; psu=0; pad=0;
  while(start!=outx)
   {
   int lz, w, z=start, ow;
   if(start) w=width(outbuf,ind);
   else w=0;
   do
    {
    lz=z; ow=w;
    while(z!=outx && (outbuf[z]&0xFF)==32) w+=width(outbuf+z++,1);
    while(z!=outx && (outbuf[z]&0xFF)!=32) w+=width(outbuf+z++,1);
    } while(w<=lwidth && z!=outx);
   if(w<=lwidth) lz=z;
   if(lz!=start)
    {
    int b=maxabove(outbuf+start,lz-start);
    int h=maxheight(outbuf+start,lz-start);
    if(!fillmode)
     if(start) tgen(width(outbuf,ind),b,start,lz);
     else tgen(0,b,start,lz);
    else
     if(start) fillt(width(outbuf,ind),b,start,lz,lz==outx);
     else fillt(0,b,start,lz,lz==outx);
    y+=h;
    }
   while(lz!=outx && (outbuf[lz]&0xFF)==32) ++lz;
   start=lz;
   }
  }
 outx=0;
 ind=2;
 mode=0;
 fillmode=0;
 }

/* Interpret text which appears at 'in' */

interpret()
 {
 while(in[0])
  if(in[0]=='[')
   {
   char *argv[10];
   char *at;
   int argc=0;
   int c;
   /* Parse command and arguments */
   ++in;

   loop:
   at=in;
   while(*in!=' ' && *in!=0 && *in!=']') ++in;
   c= *in;
   *in=0;
   if(*at=='[' && c==']')
    {
    struct saved *cmd=get(at+1);
    if(cmd && cmd->text) argv[argc++]=cmd->text;
    else argv[argc++]=strdup(at);
    }
   else
    if(at!=in) argv[argc++]=strdup(at);
   *in =c;
   if(*at=='[' && c==']') ++in;
   while(*in==' ') ++in;
   if(*in!=0 && *in!=']') goto loop;
   if(*in==0) error(line,"Missing ]");
   else ++in;
   if(argc==0) error(line,"Empty command");
   else
    {
    struct saved *cmd=get(argv[0]);
    if(!cmd) error(line,"Unknown command"), error(line,argv[0]);
    else if(cmd->nargs>argc-1) error(line,"Incorrect no. of args");
    else
     {
     int x;
     push(argc,argv);
     for(x=0;x!=cmd->nargs;++x) set(top,cmd->args[x],strdup(argv[x+1]),0,0,0);
     if(cmd->func) cmd->func();
     else
      {
      char *cur=in;
      in=cmd->text;
      interpret();
      in=cur;
      }
     for(x=0;x!=argc;++x) free(argv[x]);
     pop();
     }
    }
   }
  else if(*in=='\n')
   {
   ++in;
   outtext();
   }
  else
   {
   char *at=in;
   int c;
   /* Generate text */
   while(*in!=0 && *in!='[' && *in!='\n') ++in;
   c=*in; *in=0;
   gentext(at);
   *in=c;
   }
 }

/* Process a line of input */

char *sectname=0;

done()
 {
 *out=0;
 if(out!=obuf && sectname)
  {
  printf("%s",sectname);
  if(ee) oprintf("^FS"), ee=0;
  printf("%s^XZ\n",obuf);
  }
 out=obuf;
 fwidth=24;
 fheight=24;
 fontsel=7;
 loffset=0;
 lwidth=290;
 pf= -1;
 ox= -1;
 oy= -1;
 y=0;
 }

int fields(s,fields,sep)
char *s, **fields, sep;
 {
 int y=1;
 fields[0]=s;
 while(*s)
  {
  if(*s==sep) fields[y++]=s+1, *s=0;
  ++s;
  }
 return y;
 }

process()
 {
 char *ary[20];
 if(in[0]==':')
  {
  int n, x;
  char *ary[20];
  done();
  sectname=strdup(in+1);
  for(x=0;in[x] && in[x]!='\n';++x);
  in[x]=0;
  n=fields(in+1,ary,':');
  for(x=0;x!=n;++x)
   {
   char bbb[10];
   sprintf(bbb,"arg%d",x);
   set(top,bbb,strdup(ary[x]),0,0,0);
   }
  }
 else interpret();
 }

save()
 {
 char buf[1024];
 char *bp=buf;
 int cnt=1;
 loop:
 while(*in!=0 && *in!='[') *bp++=*in++;
 if(*in=='[')
  {
  ++in;
  if(*in!=']')
   {
   if(in[0]=='s' && in[1]=='a' && in[2]=='v' && in[3]=='e' && in[4]==' ') ++cnt;
   *bp++='[';
   goto loop;
   }
  else if(--cnt)
   {
   *bp++='[';
   goto loop;
   }
  else
   {
   *bp=0;
   ++in;
   set(root,get("name")->text,strdup(buf),0,top->nargs-2,top->args+2);
   }
  }
 else
  {
  in=fgets(inbuf,1023,stdin), ++line;
  if(!in) error(line,"File ended before save");
  goto loop;
  }
 }

font()
 {
 sscanf(get("font")->text, "%d",&fontsel);
 sscanf(get("width")->text,"%d",&fwidth);
 sscanf(get("height")->text,"%d",&fheight);
 }

indent()
 {
 sscanf(get("amnt")->text,"%d",&ind);
 }

soffset()
 {
 sscanf(get("amnt")->text,"%d",&loffset);
 }

swidth()
 {
 sscanf(get("amnt")->text,"%d",&lwidth);
 }

upc()
 {
 if(ee) oprintf("^FS"), ee=0;
 ox= -1; oy= -1;
 oprintf("^FO%d,%d^BUN,100^FD%s^FS",lwidth/2-190/2+loffset,y,get("code")->text);
 y+=100;
 mode=2;
 }

vupc()
 {
 if(ee) oprintf("^FS"), ee=0;
 ox= -1; oy= -1;
 oprintf("^FO%d,%d^BUR,100^FD%s^FS",lwidth/2-100/2+loffset,y+14,get("code")->text);
 y+=218;
 mode=2;
 }

fill()
 {
 fillmode=1;
 }

struct init
 {
 char *name;
 int (*func)();
 int nargs;
 char *args[10];
 } initab[]=
 {
  { "save", save, 1, {"name"} },
  { "font", font, 3, {"font", "width", "height"} },
  { "upc",  upc,  1, {"code"} },
  { "vupc", vupc, 1, {"code"} },
  { "indent", indent, 1, {"amnt"} },
  { "offset", soffset, 1, {"amnt"} },
  { "width", swidth, 1, {"amnt"} },
  { "fill", fill, 0 }
 };

init()
 {
 int x;
 push();
 root=top;
 for(x=0;x!=sizeof(initab)/sizeof(struct init);++x)
  set(top,initab[x].name,0,initab[x].func,initab[x].nargs,initab[x].args);
 }

main()
 {
 char buf[100];
 FILE *f=fopen("fnt.parms","r");
 int w,h;
 fgets(buf,100,f);
 fclose(f);
 sscanf(buf,"%d,%d",&w,&h);
 fonts[9].width=w;
 fonts[9].height=h;
 fonts[9].base=h;
 
 init();
 while(in=(++line, fgets(inbuf,1023,stdin))) process();
 done();
 }
