#include <int.h>
#include <dos.h>
#include <stdlib.h>
#include <stdio.h>
#include <fg.h>

/* The game of tetris */

#define twidth 10
#define theight 20
#define soffset ((80/2-twidth/2)*2)

unsigned long org;
int eflg=0;

char *t;

/* Values in t */

#define tvempty 0
#define tvdead 1
#define tvmoving 2

char *ts;
unsigned score=0;
int del=9;
#define Smovedown 0	/* Normal move down state */
#define Sstopped 1	/* Last movedown didn't occure */
#define Schecking 2
int perlevel=12;
int tstate=Smovedown;
int ttime;
int ln=12;
int nxt;

/* Peices */

#define npeices 7
char *peices[]=
{
"    ****",
" **  ** ",
"  ** ** ",
" **   **",
" *** *  ",
" *   ***",
" ***  * "
};

/* Screen parameters */

char brick[]={0xff,0xff,0xf0,0x0f,0xf0,0x0f,0xc0,0x03,0xc0,0x03,0xc0,0x03,
0xc0,0x03,0xc0,0x03,0xc0,0x03,0xc0,0x03,0xf0,0x0f,0xf0,0x0f,0xff,0xff};
fg_box_t box={0,0,15,12};

fg_box_t outline;
fg_box_t soutline;

void tcls(void)
{
fg_box_t tmp;

outline[FG_X1]=(fg_displaybox[FG_X2]+1)/2-((1+box[FG_X2])*twidth)/2;
outline[FG_X2]=outline[FG_X1]+(1+box[FG_X2])*twidth-1;
outline[FG_Y1]=(fg_displaybox[FG_Y2]+1)/2-((1+box[FG_Y2])*theight)/2;
outline[FG_Y2]=outline[FG_Y1]+(1+box[FG_Y2])*theight-1;

soutline[FG_Y2]=outline[FG_Y2];
soutline[FG_Y1]=soutline[FG_Y2]-(1+box[FG_Y2])*2+1;
soutline[FG_X1]=fg_displaybox[FG_X1]+24;
soutline[FG_X2]=soutline[FG_X1]+(1+box[FG_X2])*12-1;

tmp[FG_X1]=soutline[FG_X1]-2;
tmp[FG_X2]=soutline[FG_X2]+2;
tmp[FG_Y1]=soutline[FG_Y1]-2;
tmp[FG_Y2]=soutline[FG_Y2]+2;
fg_drawbox(FG_WHITE,FG_MODE_SET,~0,FG_LINE_SOLID,tmp,fg_displaybox);

tmp[FG_X1]=outline[FG_X1]-2;
tmp[FG_X2]=outline[FG_X2]+2;
tmp[FG_Y1]=outline[FG_Y1]-2;
tmp[FG_Y2]=outline[FG_Y2]+2;
fg_drawbox(FG_WHITE,FG_MODE_SET,~0,FG_LINE_SOLID,tmp,fg_displaybox);
tmp[FG_X1]-=2;
tmp[FG_X2]+=2;
tmp[FG_Y1]-=2;
tmp[FG_Y2]+=2;
fg_drawbox(FG_WHITE,FG_MODE_SET,~0,FG_LINE_SOLID,tmp,fg_displaybox);
fg_drawbox(FG_WHITE,FG_MODE_SET,~0,FG_LINE_SOLID,fg_displaybox,fg_displaybox);
}

/* Update the physical screen */

fg_box_t box1;

void tupdate(void)
{
char *tp, *tsp;
int x,y;
for(tp=t, tsp=ts, y=0;y<theight*(box[FG_Y2]+1);y+=box[FG_Y2]+1)
 for(x=0;x<twidth*(box[FG_X2]+1);x+=(box[FG_X2]+1),tp++, tsp++)
  if((!*tp)!=(!*tsp))
   {
   *tsp= *tp;
   box1[FG_X1]=outline[FG_X1]+x;
   box1[FG_X2]=box1[FG_X1]+box[FG_X2];
   box1[FG_Y1]=1+outline[FG_Y2]-y-(box[FG_Y2]+1);
   box1[FG_Y2]=box1[FG_Y1]+box[FG_Y2];
   if(*tp) fg_drawmatrix(FG_WHITE,FG_MODE_SET,~0,FG_ROT0,x+outline[FG_X1],
                        1+outline[FG_Y2]-y-(box[FG_Y2]+1),brick,box,outline);
   else fg_fillbox(FG_BLACK,FG_MODE_SET,~0,box1);
   }
}

/* Show score */

itoaa(int n,char *s)
{
int flg=0;
char accu;
int sub=10000;
while(sub)
 {
 accu='0';
 while(n-sub>=0) accu++, n-=sub, flg=1;
 if(flg) *(s++)=accu;
 sub=sub/10;
 }
if(!flg) *(s++)='0';
*s=0;
}

char tmps[30]={0};
char tmps1[30]={0};

tscore()
{
fg_puts(FG_BLACK,FG_MODE_SET,~0,FG_ROT0,soutline[FG_X1]+4,soutline[FG_Y1],tmps,soutline);
strcpy(tmps,"Level: ");
itoaa(10-del,tmps+strlen(tmps));
fg_puts(FG_WHITE,FG_MODE_SET,~0,FG_ROT0,soutline[FG_X1]+4,soutline[FG_Y1],tmps,soutline);

fg_puts(FG_BLACK,FG_MODE_SET,~0,FG_ROT0,soutline[FG_X1]+4,soutline[FG_Y1]+12,tmps1,soutline);
strcpy(tmps1,"Score: ");
itoaa(score,tmps1+strlen(tmps1));
fg_puts(FG_WHITE,FG_MODE_SET,~0,FG_ROT0,soutline[FG_X1]+4,soutline[FG_Y1]+12,tmps1,soutline);
}

/* Initialize */

tiz()
{
int x,y;
t=malloc(twidth*theight);
ts=malloc(twidth*theight);
for(y=0;y<theight;y++)
 for(x=0;x<twidth;x++)
  ts[x+y*twidth]=0, t[x+y*twidth]=0;
}

/* Check for completed rows made of dead peices.  Delete them */

int tcheck()
{
int x,y;
for(y=0;y<theight;y++)
 {
 for(x=0;x<twidth;x++)
  if(t[x+twidth*y]==tvempty) goto nxt;
 for(;y>=0;y--)
  for(x=0;x<twidth;x++)
   if(y) t[y*twidth+x]=t[(y-1)*twidth+x];
   else t[x]=tvempty;
 return 1;
 nxt:;
 }
return 0;
}

/* New peice.  Changes old moving peice into a dead peice. */

int tnew()
{
int x,y,n=nxt;
nxt=rand()/(32768/npeices);
for(y=0;y<2;y++)
 for(x=0;x<4;x++)
  {
  if(peices[n][y*4+x]=='*' && t[x+y*twidth+twidth/2-2]) return 1;
  t[x+y*twidth+twidth/2-2]=((peices[n][y*4+x]=='*')?tvmoving:tvempty);
  }
for(y=0;y<2;y++)
 for(x=0;x<4;x++)
  {
  box1[FG_X1]=soutline[FG_X1]+(x+8)*(box[FG_X2]+1);
  box1[FG_X2]=box1[FG_X1]+box[FG_X2];
  box1[FG_Y1]=1+soutline[FG_Y2]-(1+y)*(box[FG_Y2]+1);
  box1[FG_Y2]=box1[FG_Y1]+box[FG_Y2];
  if(peices[nxt][x+y*4]=='*')
   fg_drawmatrix(FG_WHITE,FG_MODE_SET,~0,FG_ROT0,(x+8)*(box[FG_X2]+1)+soutline[FG_X1],
                 1+soutline[FG_Y2]-y*(box[FG_Y2]+1)-(box[FG_Y2]+1),brick,box,soutline);
  else fg_fillbox(FG_BLACK,FG_MODE_SET,~0,box1);
  }
return 0;
}

/* Convert moving peices to dead peices */

tcnvt()
{
int x,y;
for(y=0;y<theight;y++)
 for(x=0;x<twidth;x++)
  if(t[x+y*twidth]==tvmoving) t[x+y*twidth]=tvdead;
}

/* Drop down 1 */

int tdown()
{
char *tp;
for(tp=t;tp<twidth*(theight-1)+t;tp++)
 if(*tp==tvmoving) if(*(tp+twidth)==tvdead) return 0;
for(tp=t+twidth*(theight-1);tp<twidth*theight+t;tp++)
 if(*tp==tvmoving) return 0;
for(tp=t+twidth*theight-1;tp>=twidth+t;tp--)
 if(*(tp-twidth)==tvmoving) *(tp-twidth)=tvempty, *tp=tvmoving;
return 1;
}

/* Move right */

tright()
{
int x,y;
for(y=0;y<theight;y++)
 for(x=0;x<twidth-1;x++)
  if(t[x+y*twidth]==tvmoving)
   if(t[x+y*twidth+1]==tvdead) return 0;
for(y=0;y<theight;y++)
 if(t[twidth-1+y*twidth]==tvmoving) return 0;
for(y=0;y<theight;y++)
 for(x=twidth-1;x;x--)
  if(t[x+y*twidth-1]==tvmoving)
   t[x+y*twidth-1]=tvempty, t[x+y*twidth]=tvmoving;
return 1;
}

/* Move left */

tleft()
{
int x,y;
for(y=0;y<theight;y++)
 for(x=1;x<twidth;x++)
  if(t[x+y*twidth]==tvmoving)
   if(t[x+y*twidth-1]==tvdead) return 0;
for(y=0;y<theight;y++)
 if(t[y*twidth]==tvmoving) return 0;
for(y=0;y<theight;y++)
 for(x=0;x<twidth-1;x++)
  if(t[x+y*twidth+1]==tvmoving)
   t[x+y*twidth+1]=tvempty, t[x+y*twidth]=tvmoving;
return 1;
}

/* Rotate */

trot()
{
int x,y;
int ytop=theight, xleft=twidth;
int ybot= -1, xright= -1;
int yc, xc;
char ary[4][4];
char nary[4][4];
/* Find the peice */
for(y=0;y<theight;y++)
 for(x=0;x<twidth;x++)
  if(t[x+y*twidth]==tvmoving)
   {
   if(x<xleft) xleft=x;
   if(y<ytop) ytop=y;
   if(x>xright) xright=x;
   if(y>ybot) ybot=y;
   }
yc=(ytop+ybot)/2-1;
xc=(xright+xleft)/2-1;
for(y=0;y<4;y++)
 for(x=0;x<4;x++)
  {
  ary[x][y]=0;
  if(y+yc<theight && x+xc<twidth && y+yc>=0 && x+xc>=0)
   if(t[x+y*twidth+xc+yc*twidth]==tvmoving)
    {
    t[x+y*twidth+xc+yc*twidth]=tvempty;
    ary[x][y]=1;
    }
  }
up:
for(y=0;y<4;y++)
 for(x=0;x<4;x++)
  nary[y][3-x]=ary[x][y];
for(y=0;y<4;y++)
 for(x=0;x<4;x++)
  ary[x][y]=nary[x][y];
for(y=0;y<4;y++)
 for(x=0;x<4;x++)
  {
  if(y+yc<theight && x+xc<twidth && x+xc>=0 && y+yc>=0)
   {
   if(ary[x][y])
    if(t[x+y*twidth+yc*twidth+xc]!=tvempty) goto up;
   }
  else if(ary[x][y]) goto up;
  }
for(y=0;y<4;y++)
 for(x=0;x<4;x++)
  {
  if(y+yc<theight && x+xc<twidth)
   {
   if(ary[x][y])
    t[x+y*twidth+yc*twidth+xc]=tvmoving;
   }
  }
}

/* Drop complety */

tdrop()
{
while(tdown())
 {
 tupdate();
 ttime=del;
 }
}

tquit()
{
int_restore(8);
eflg=1;
}

kbd()
{
int x;
if(!bioskey(1)) return 0;
x=bioskey(0);
int_off();
switch(x)
 {
case 0x4800:
 trot();
 break;
case 0x4D00:
 tright();
 break;
case 0x4B00:
 tleft();
 break;
case 0x5000:
 tdrop();
 break;
case 0x1051:
case 0x1071:
 tquit();
 break;
default:
 return 0;
 }
return 1;
}

tick1()
{
if(!ttime)
 {
 ttime=del;
 switch(tstate)
  {
 case Smovedown:
  if(tdown())
   {
   tupdate();
   return;
   }
  tstate=Sstopped;
  return;
 case Sstopped:
  if(tdown())
   {
   tstate=Smovedown;
   tupdate();
   return;
   }
  score+=9*(10-del);
  tscore();
  tcnvt();
 case Schecking:
  if(tcheck())
   {
   tstate=Schecking;
   if(!--ln)
    {
    ln=(perlevel+=2);
    if(del) del--;
    tscore();
    }
   }
  else
   {
   tstate=Smovedown;
   if(tnew())
    tquit();
   }
  tupdate();
  break;
  }
 }
else
 ttime--;
}

tick()
{
tick1();
return 0;
}

/* Top Scores manager */

topscores(unsigned score)
{
int x,y;
FILE *f=fopen("tetris.top","r");
char *a=malloc(32*10);
if(f) fread(a,320,1,f);
else
 for(x=0;x<320;x++) a[x]=0;
if(f) fclose(f);
for(x=0;x<320;x+=32)
 {
 if(score>*(unsigned *)(a+x))
  {
  for(y=319;y!=x+31;y--)
   a[y]=a[y-32];
  *(unsigned *)(a+x)=score;
  printf("You made the top ten list!  Enter your name: ");
  fflush(stdout);
  fgets(a+x+2,30,stdin);
  *(x+a+1+strlen(x+a+2))=0;
  f=fopen("tetris.top","w");
  if(f) fwrite(a,320,1,f);
  if(f) fclose(f);
  break;
  }
 }
printf("Your score: %u\n",score);
printf("\t\tRank\tScore\tName\n");
for(x=0;x<320;x+=32)
 if(*(unsigned *)(x+a)) printf("\t\t%d\t%u\t%s\n\n",x/32+1,*(unsigned *)(x+a),x+a+2);
 else printf("\n\n");
}

main(int argc,char *argv[])
{
unsigned n;

if(argc==2)
 {
 n=atoi(argv[1]);
 if(n>=1 && n<=10)
  {
  del=10-atoi(argv[1]);
  perlevel=12+2*(atoi(argv[1])-1);
  }
 }
fg_init_all();
tcls();
tiz();
ttime=del;
nxt=rand()/(32768/npeices);
tnew();
tupdate();
tscore();

int_intercept(8,tick,256);

while(!eflg)
 {
 int_off();
 if(kbd()) tupdate();
 int_on();
 }
fg_term();
printf("Hello?\n");
printf("\n\n\n\nHi there?\n");
/* topscores(score); */
}

