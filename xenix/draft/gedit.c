/* Graphics editor */

#include <stdio.h>
#include "sc.h"
#include "queue.h"
#include "object.h"
#include "disp.h"
#include "tty.h"
#include "gedit.h"

int curx=0x20000000, cury=0x20000000;
int markon=0;
int markx, marky;
W *mainw;
int arg=0;
int scale=0;
int upd=0;
char name[256];

O *pic=0;

int lsb(x)
{
int y;
for(y=1;!(x&y);y<<=1);
return y;
}

void redraw()
{
clrs();
genw(mainw,pic,SET,0,0);
gridw(mainw,SET);
if(!have) upd=0;
}

void center()
{
mainw->ix=curx-mainw->iw/2;
mainw->iy=cury-mainw->ih/2;
upd=1;
}

void lockto()
{
curx=curx+mainw->dx/2-((curx+mainw->dx/2)%mainw->dx);
cury=cury+mainw->dy/2-((cury+mainw->dy/2)%mainw->dy);
mainw->ix=mainw->ix+mainw->dx/2-((mainw->dx/2+mainw->ix)%mainw->dx);
mainw->iy=mainw->iy+mainw->dy/2-((mainw->iy+mainw->dy/2)%mainw->dy);
}

void follow()
{
int x=curx;
int y=cury;
x-=x%mainw->dx;
y-=y%mainw->dy;
if(x<mainw->ix) mainw->ix=x, upd=1;
if(x>=mainw->ix+mainw->iw) mainw->ix=x-mainw->iw+mainw->dx, upd=1;
if(y<mainw->iy) mainw->iy=y, upd=1;
if(y>=mainw->iy+mainw->ih) mainw->iy=y-mainw->ih+mainw->dy, upd=1;
}

void zoomin()
{
int l;
if(mainw->iw==4 || mainw->ih==4) return;
mainw->ix+=mainw->iw/2; mainw->iy+=mainw->ih/2;

l=lsb(mainw->iw);
if(l==mainw->iw) mainw->iw-=mainw->iw/4;
else mainw->iw-=l;

l=lsb(mainw->ih);
if(l==mainw->ih) mainw->ih-=mainw->ih/4;
else mainw->ih-=l;

mainw->ix-=mainw->iw/2; mainw->iy-=mainw->ih/2;

if(mainw->iw/mainw->dx<4) mainw->dx=mainw->iw/32, mainw->dy=mainw->ih/16;
upd=1;
}

void zoomout()
{
int l;
if(mainw->iw>=2000000) return;	/* Max width 2GB/720 */
mainw->ix+=mainw->iw/2; mainw->iy+=mainw->ih/2;

l=lsb(mainw->iw);
if(l==mainw->iw) mainw->iw+=mainw->iw/2;
else mainw->iw+=l;

l=lsb(mainw->ih);
if(l==mainw->ih) mainw->ih+=mainw->ih/2;
else mainw->ih+=l;

mainw->ix-=mainw->iw/2; mainw->iy-=mainw->ih/2;

if(mainw->iw/mainw->dx>64) mainw->dx=mainw->iw/64, mainw->dy=mainw->ih/32;
upd=1;
}

void all()
{
if(pic->up)
 {
 int h;
 int q;
 curx=(pic->left+pic->right)/2;
 cury=(pic->up+pic->down)/2;
 if(pic->up-pic->down>pic->right-pic->left) h=pic->up-pic->down;
 else h=pic->right-pic->left;
 for(q=0x40000000;q>=h;q/=2);
 if(h<=q+q/2) h=q+q/2;
 else h=q*2;
 mainw->ih=mainw->iw=h;
 mainw->dx=mainw->iw/32;
 mainw->dy=mainw->ih/16;
 center();
 lockto();
 }
}

void more()
{
int l;
if(mainw->dx==1) return;
if(mainw->iw/mainw->dx>64) return;
l=lsb(mainw->dx);
if(l==mainw->dx) mainw->dx-=mainw->dx/4;
else mainw->dx-=l;
l=lsb(mainw->dy);
if(l==mainw->dy) mainw->dy-=mainw->dy/4;
else mainw->dy-=l;
upd=1;
}

void less()
{
int l;
if(mainw->dx>=1000000) return;
if(mainw->iw/mainw->dx<4) return;
l=lsb(mainw->dx);
if(l==mainw->dx) mainw->dx+=mainw->dx/2;
else mainw->dx+=l;
l=lsb(mainw->dy);
if(l==mainw->dy) mainw->dy+=mainw->dy/2;
else mainw->dy+=l;
upd=1;
}

void stmark()
{
markon=1, markx=curx, marky=cury;
}

void rtarw()
{
if(arg==0) arg=1;
if(curx+mainw->dx*arg>=0x40000000) return;
curx+=mainw->dx*arg;
}

void ltarw()
{
if(arg==0) arg=1;
if(curx-mainw->dx*arg<0) return;
curx-=mainw->dx*arg;
}

void uparw()
{
if(arg==0) arg=1;
if(cury+mainw->dy*arg>=0x40000000) return;
cury+=mainw->dy*arg;
}

void dnarw()
{
if(arg==0) arg=1;
if(cury-mainw->dy*arg<0) return;
cury-=mainw->dy*arg;
}

void line()
{
if(markon)
 {
 O *p;
 p=mkline(pic,markx,marky,curx-markx,cury-marky);
 genw(mainw,p,SET,0,0);
 stmark();
 }
}

void undo()
{
if(pic->v.list)
 {
 if(pic->v.list->link.prev->what==OLIST)
  {
  markon=0;
  curx=pic->v.list->link.prev->x;
  cury=pic->v.list->link.prev->y;
  }
 else
  {
  markon=1;
  markx=pic->v.list->link.prev->x;
  marky=pic->v.list->link.prev->y;
  curx=markx+pic->v.list->link.prev->v.line.x;
  cury=marky+pic->v.list->link.prev->v.line.y;
  }
 del(rm(pic->v.list->link.prev));
 upd=1;
 }
}

void dosave(f,o,i)
FILE *f;
O *o;
{
int x;
if(o)
 {
 if(o->what==OLINE)
  {
  for(x=0;x!=i;++x) fprintf(f," ");
  fprintf(f,"%d,%d %d,%d\n",o->x,o->y,o->v.line.x,o->v.line.y);
  }
 else if(o->what==OLIST)
  {
  O *p;
  for(x=0;x!=i;++x) fprintf(f," ");
  fprintf(f,"{ %d,%d\n",o->x,o->y);
  if(p=o->v.list) do
   dosave(f,p,i+1);
   while((p=p->link.next)!=o->v.list);
  for(x=0;x!=i;++x) fprintf(f," ");
  fprintf(f,"}\n");
  }
 }
}

void save()
{
FILE *f=fopen(name,"w");
dosave(f,pic,0);
fclose(f);
}

char buf[1024];

O *doloadlist(p,f,a,b)
O *p;
FILE *f;
{
O *o=mklist(p,a,b,NULL);
int x;
int i,j,ito,jto;
loop:
fgets(buf,1024,f);
for(x=0;buf[x] && (buf[x]==' ' || buf[x]=='\t');++x);
if(buf[x]=='}') return o;
if(buf[x]=='{')
 {
 sscanf(buf+x+2,"%d,%d",&i,&j);
 doloadlist(o,f,i,j);
 }
else
 {
 sscanf(buf+x,"%d,%d %d,%d",&i,&j,&ito,&jto);
 mkline(o,i,j,ito,jto);
 }
goto loop;
}

void load()
{
FILE *f=fopen(name,"r");
if(f)
 {
 int x;
 if(fgets(buf,1024,f))
  {
  for(x=0;buf[x] && (buf[x]==' ' || buf[x]=='\t');++x);
  if(buf[x]=='{')
   {
   int i,j;
   sscanf(buf+x+2,"%d,%d",&i,&j);
   pic=doloadlist(NULL,f,i,j);
   }
  }
 fclose(f);
 }
if(!pic) pic=mklist(NULL,0,0,NULL);
upd=1;
}

O *font[128];
int fontabove=0;
int fontbelow=0;
int fontleft[128];
int fontright[128];

void loadfont()
{
O *fnt;
FILE *f=fopen("/usr/local/lib/draftfont","r");
if(f)
 {
 int x;
 while(fgets(buf,1024,f))
  {
  for(x=0;buf[x] && (buf[x]==' ' || buf[x]=='\t');++x);
  if(buf[x]=='{')
   {
   int chr,above,below,left,right;
   sscanf(buf+x+2,"%d,%d,%d,%d,%d",&chr,&above,&below,&left,&right);
   fontleft[chr]=left;
   fontright[chr]=right;
   if(above>fontabove) fontabove=above;
   if(below>fontbelow) fontbelow=below;
   font[chr]=doloadlist(NULL,f,0,0);
   }
  }
 fclose(f);
 }
}

void type()
{
int c;
int ocurx=curx;
int ocury=cury;
O *txt;
char enter[128];
int x=0;
txt=mklist(pic,curx,cury,NULL);

arg=0;

loop:
if(upd) redraw();

/* Display cursor and mark */
cursor(mainw,curx,cury,INV);
if(markon) mark(mainw,markx,marky,INV);

up:
c=ttgetc();
if(c==27) goto up;
if(c=='[') goto up;

cursor(mainw,curx,cury,INV);
if(markon) mark(mainw,markx,marky,INV);

switch(c)
 {
default:
 if(font[c])
  {
  O *new;
  enter[x++]=c;
  curx+=fontleft[c]*mainw->dx/64;
  new=dup(txt,font[c],curx,cury,mainw->dx/64,mainw->dy/64);
  curx+=fontright[c]*mainw->dx/64;
  genw(mainw,new,SET,0,0);
  }
 break;

case 8:
case 127:
 if(txt->v.list)
  {
  --x;
  curx-=fontleft[enter[x]]*mainw->dx/64+fontright[enter[x]]*mainw->dx/64;
  genw(mainw,txt->v.list->link.prev,CLR,0,0);
  del(rm(txt->v.list->link.prev));
  }
 break;

case 13:
 cury-=(fontabove+fontbelow)*mainw->dy/64;
 curx=ocurx;
 return;
 }

arg=0;
follow();
goto loop;
}

void print()
{
lpclr();
lpgen(pic,mainw->ix,mainw->iy,mainw->iw,mainw->ih);
lpout();
}

void delete()
{
O *p;
if(p=pic->v.list) do
 if(curx==p->x && cury==p->y)
  {
  del(rm(p));
  upd=1;
  return;
  }
 while((p=p->link.next)!=pic->v.list);
if(p=pic->v.list) do
 if(curx>=p->left &&
    curx<=p->right &&
    cury>=p->down &&
    cury<=p->up)
  {
  curx=p->x;
  cury=p->y;
  break;
  }
 while((p=p->link.next)!=pic->v.list);
}

void edit()
{
int c;
arg=0;

all();
loop:
if(upd) redraw();

/* Display cursor and mark */
cursor(mainw,curx,cury,INV);
if(markon) mark(mainw,markx,marky,INV);

up:
c=ttgetc();
if(c==27) goto up;
if(c=='[') goto up;

cursor(mainw,curx,cury,INV);
if(markon) mark(mainw,markx,marky,INV);

switch(c)
 {
case '0': case '1': case '2': case '3': case '4':
case '5': case '6': case '7': case '8': case '9':
 arg=arg*10+(int)(c-'0');
 goto loop;

default:
 goto loop;

case '+': zoomin(); break;

case '-': zoomout(); break;

case 'R'-'@': upd=1; break;

case 'a': all(); break;

case 'A': uparw(); break;

case 'B': dnarw(); break;

case 'C': rtarw(); break;

case 'D': ltarw(); break;

case 'd': delete(); break;

case 'p': print(); break;

case 'l': line(); break;

case 'g': more(); break;

case 't': less(); break;

case 'u': undo(); break;

case 'c': center(); break;

case 'i': type(); break;

case ' ':
 stmark();
 break;

case 'x':
 save();
 return;

case 'q':
 if(ttgetc()=='y') return;
 break;
 }

arg=0;
lockto();
follow();
goto loop;
}
