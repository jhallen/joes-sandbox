/* Display generation and windowing */

#include <stdio.h>
#include "disp.h"
#include "object.h"
#include "tty.h"
#include "sc.h"

/* Create a window */

W *mkw(x,y,wi,h)
{
W *w=(W *)malloc(sizeof(W));
w->x=x; w->y=y; w->w=wi; w->h=h;
w->iw=BASE; w->ih=BASE;
w->ix=0x20000000-w->iw/2; w->iy=0x20000000-w->ih/2;
w->dx= BASE/64; w->dy= BASE/32;
return w;
}

/* Generate an object on the screen */

void genw(w,o,m,ox,oy)
W *w;
O *o;
int ox,oy;
{
if(o)
 {
 if(o->what==OLINE)
  {
  int x,y,xto,yto,adj;
  x=o->x+ox;
  y=o->y+oy;
  xto=o->v.line.x;
  yto=o->v.line.y;

  /* Do nothing if line is completely out of window */
  if(x<w->ix && x+xto<w->ix) return;
  if(x>=w->ix+w->iw && x+xto>=w->ix+w->iw) return;
  if(y<w->iy && y+yto<w->iy) return;
  if(y>=w->iy+w->ih && y+yto>=w->iy+w->ih) return;
 
  /* Clip  line */
  if(x<w->ix)
   {
   if(xto) adj=(yto*(w->ix-x))/xto, y+=adj, yto-=adj;
   else return;
   xto-=w->ix-x;
   x=w->ix;
   }

  if(x>=w->ix+w->iw)
   {
   if(xto) adj=(yto*(w->ix+w->iw-x))/xto, y+=adj, yto-=adj;
   else return;
   xto-=w->ix+w->iw-x;
   x=w->ix+w->iw;
   }

  if(y<w->iy)
   {
   if(yto) adj=(xto*(w->iy-y))/yto, x+=adj, xto-=adj;
   else return;
   yto-=w->iy-y;
   y=w->iy;
   }

  if(y>=w->iy+w->ih)
   {
   if(yto) adj=(xto*(w->iy+w->ih-y))/yto, x+=adj, xto-=adj;
   else return;
   yto-=w->iy+w->ih-y;
   y=w->iy+w->ih;
   }

  if(x+xto<w->ix)
   {
   if(xto) yto-=(yto*((x+xto)-w->ix))/xto;
   else return;
   xto=w->ix-x;
   }

  if(x+xto>=w->ix+w->iw)
   {
   if(xto) yto=(yto*(w->ix+w->iw-x))/xto;
   else return;
   xto=w->ix+w->iw-x;
   }

  if(y+yto<w->iy)
   {
   if(yto) xto-=(xto*((y+yto)-w->iy))/yto;
   else return;
   yto=w->iy-y;
   }

  if(y+yto>=w->iy+w->ih)
   {
   if(yto) xto=(xto*(w->iy+w->ih-y))/yto;
   else return;
   yto=w->iy+w->ih-y;
   }
 
  /* Draw line */
  lineto( ((x-w->ix)*w->w)/w->iw+w->x,
          ((y-w->iy)*w->h)/w->ih+w->y,
          (xto*w->w)/w->iw,
          (yto*w->h)/w->ih,
          m
        );
  }
 else
  {
  O *p;
  if(p=o->v.list) do
   {
   ttflsh();
   if(have) return;
   genw(w,p,m,ox,oy);
   }
   while((p=p->link.next)!=o->v.list);
  }
 }
}

/* Cursor */

void cursor(w,x,y,m)
W *w;
{
int i,j;
i=((x-w->ix)*w->w)/w->iw+w->x;
j=((y-w->iy)*w->h)/w->ih+w->y;
lineto(i,j,10,10,m);
lineto(i,j,10,-10,m);
lineto(i,j,-10,10,m);
lineto(i,j,-10,-10,m);
}

void mark(w,x,y,m)
W *w;
{
int i,j;
i=((x-w->ix)*w->w)/w->iw+w->x;
j=((y-w->iy)*w->h)/w->ih+w->y;
lineto(i-5,j-5,10,0,m);
lineto(i-5,j+5,10,0,m);
lineto(i-5,j-5,0,10,m);
lineto(i+5,j-5,0,10,m);
}

/* Mark */

/* Clear screen */

void clrs()
{
ttflsh();
if(have) return;
mset(hscreen,0,0x2000*3+90*87);
ttflsh();
if(have) return;
lineto(0,0,width-1,0,SET);
lineto(0,height-1,width-1,0,SET);
lineto(0,0,0,height-1,SET);
lineto(width-1,0,0,height-1,SET);
}

/* Write grid points on the screen */

void gridw(w,m)
W *w;
{
int x,y;
for(y=w->iy+w->dy-1-(w->iy+w->dy-1)%w->dy;y<w->iy+w->ih;y+=w->dy)
 {
 ttflsh();
 if(have) return;
 for(x=w->ix+w->dx-1-(w->ix+w->dx-1)%w->dx;x<w->ix+w->iw;x+=w->dx)
  dot(w->x+((x-w->ix)*w->w)/w->iw,
      w->y+((y-w->iy)*w->h)/w->ih,
      m);
 }
}

/* Hp laser image generator */

#define lpwidth 1490
#define lpheight 1200
unsigned char image[lpwidth*lpheight/8];


lpclr()
{
mset(image,0,lpwidth*lpheight/8);
}

lpdot(y,x)
{
image[y*(lpheight/8)+(x>>3)]|=(0x80>>(x&7));
}

lplineto(x,y,i,j)
{
int dist;	/* Distance */
int incx;	/* X increment value */
int incy;	/* Y increment value */
int accu;	/* Accumulator */
if(x<0) return;
if(y<0) return;
if(x>=lpwidth) return;
if(y>=lpheight) return;
if(x+i<0) return;
if(y+j<0) return;
if(x+i>=lpwidth) return;
if(y+j>=lpheight) return;
lpdot(x,y);
if(i==0 && j==0) return;
lpdot(x+i,y+j);
if(Iabs(i)<Iabs(j))
 {
 if(i>0) incx=1;
 else incx= -1, i= -i;
 if(j>0) dist=j, incy=1;
 else dist= -j, incy= -1, j= -j;
 accu=j/2;
 do
  {
  lpdot(x,y);
  y+=incy;
  accu+=i;
  if(accu>=j)
   {
   accu-=j;
   x+=incx;
   }
  } while(--dist);
 }
else
 {
 if(i>0) dist=i, incx=1;
 else dist= -i, incx= -1, i= -i;
 if(j>0) incy=1;
 else incy= -1, j= -j;
 accu=i/2;
 do
  {
  lpdot(x,y);
  x+=incx;
  accu+=j;
  if(accu>=i)
   {
   accu-=i;
   y+=incy;
   }
  } while(--dist);
 }
}

lpout()
{
FILE *p=fopen("/dev/lp2","w");
int y,x;
fprintf(p,"\033*t150R");
fprintf(p,"\033*1A");
for(y=0;y!=lpwidth;++y)
 {
 fprintf(p,"\033*b%dW",lpheight/8);
 for(x=0;x!=lpheight/8;++x) fputc(image[x+y*(lpheight/8)],p);
 }
fprintf(p,"\033*rB\f");
fclose(p);
}

lpgen(o,ix,iy,iw,ih)
O *o;
{
if(o)
 {
 if(o->what==OLINE)
  {
  int x,y,xto,yto,adj;
  x=o->x;
  y=o->y;
  xto=o->v.line.x;
  yto=o->v.line.y;

  /* Do nothing if line is completely out of window */
  if(x<ix && x+xto<ix) return;
  if(x>=ix+iw && x+xto>=ix+iw) return;
  if(y<iy && y+yto<iy) return;
  if(y>=iy+ih && y+yto>=iy+ih) return;
 
  /* Clip  line */
  if(x<ix)
   {
   if(xto) adj=(yto*(ix-x))/xto, y+=adj, yto-=adj;
   else return;
   xto-=ix-x;
   x=ix;
   }

  if(x>=ix+iw)
   {
   if(xto) adj=(yto*(ix+iw-x))/xto, y+=adj, yto-=adj;
   else return;
   xto-=ix+iw-x;
   x=ix+iw;
   }

  if(y<iy)
   {
   if(yto) adj=(xto*(iy-y))/yto, x+=adj, xto-=adj;
   else return;
   yto-=iy-y;
   y=iy;
   }

  if(y>=iy+ih)
   {
   if(yto) adj=(xto*(iy+ih-y))/yto, x+=adj, xto-=adj;
   else return;
   yto-=iy+ih-y;
   y=iy+ih;
   }

  if(x+xto<ix)
   {
   if(xto) yto-=(yto*((x+xto)-ix))/xto;
   else return;
   xto=ix-x;
   }

  if(x+xto>=ix+iw)
   {
   if(xto) yto=(yto*(ix+iw-x))/xto;
   else return;
   xto=ix+iw-x;
   }

  if(y+yto<iy)
   {
   if(yto) xto-=(xto*((y+yto)-iy))/yto;
   else return;
   yto=iy-y;
   }

  if(y+yto>=iy+ih)
   {
   if(yto) xto=(xto*(iy+ih-y))/yto;
   else return;
   yto=iy+ih-y;
   }
 
  /* Draw line */
  lplineto( ((x-ix)*lpwidth)/iw,
          ((y-iy)*lpheight)/ih,
          (xto*lpwidth)/iw,
          (yto*lpheight)/ih);
  }
 else
  {
  O *p;
  if(p=o->v.list) do
   lpgen(p,ix,iy,iw,ih);
   while((p=p->link.next)!=o->v.list);
  }
 }
}
