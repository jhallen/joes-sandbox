/* Drawing functions
   Copyright (C) 1999 Joseph H. Allen

This file is part of Notif

Notif is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation; either version 1, or (at your option) any later version.  

Notif is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.  

You should have received a copy of the GNU General Public License along with 
Notif; see the file COPYING.  If not, write to the Free Software Foundation, 
675 Mass Ave, Cambridge, MA 02139, USA.  */ 

#include "notif.h"

#include "../lib/hash.h"
#include "../lib/int.h"

static Colormap cmap;		/* Default color map */
static HASH *cntab;		/* Named color database */
static HASH *fntab;		/* Font database */

/* Linked list of font-id to fontstruct bindings */

static struct fontinfo
 {
 struct fontinfo *next;
 Font fid;
 XFontStruct *fs;
 } *fontinfo=0;

/* Linked list of read-only GCs */

static struct rdonlygc
 {
 struct rdonlygc *next;	/* Next item in list */
 unsigned long pixel;	/* Pixel value of this GC */
 Font font;		/* Font value of this GC */
 GC gc;			/* The read-only GC */
 } *rdonlygcs=0;

/* Get pixel for r,g,b value */
unsigned long getrgb(r,g,b)
unsigned short r, g, b;
 {
 XColor xc;
 xc.red=r; xc.blue=b; xc.green=g;
 if(!cntab)
  {
  cntab=htmk(256);
  cmap=DefaultColormap(dsp,0);
  }
 if(!XAllocColor(dsp,cmap,&xc))
  {
  printf("Couldn't allocate color %d %d %d\n",r,g,b);
  exit(1);
  }
 return xc.pixel;
 }

/* Get pixel for named color */
unsigned long getcolor(s)
char *s;
 {
 XColor junk;
 XColor *xc;
 if(!cntab)
  {
  cntab=htmk(256);
  cmap=DefaultColormap(dsp,0);
  }
 if(xc=htfind(cntab,s)) return xc->pixel;
 xc=(XColor *)malloc(sizeof(XColor));
 if(!XAllocNamedColor(dsp,cmap,s,xc,&junk))
  {
  printf("Couldn't find color '%s'\n",s);
  exit(1);
  }
 htadd(cntab,s,xc);
 return xc->pixel;
 }

/* Get font struct for a font */
XFontStruct *getfs(fid)
Font fid;
 {
 struct fontinfo *f;
 for(f=fontinfo;f->fid!=fid;f=f->next);
 return f->fs;
 }

/* Get font by name */
Font getfont(s)
char *s;
 {
 struct fontinfo *f;
 Font xc;
 if(!s) return 0;
 if(!fntab) fntab=htmk(256);
 if(xc=(Font)htfind(fntab,s)) return xc;
 if(!(xc=XLoadFont(dsp,s)))
  {
  printf("Couldn't find font '%s'\n",s);
  exit(1);
  }
 htadd(fntab,s,xc);
 f=malloc(sizeof(struct fontinfo));
 f->fid=xc;
 f->fs=XQueryFont(dsp,f->fid);
 f->next=fontinfo;
 fontinfo=f;
 return xc;
 }

/* Get GC for a specific Font id and pixel value.  Leave font==0 if you
 * don't care what the font is */

GC getfidpixel(font,pixel)
Font font;
unsigned long pixel;
 {
 struct rdonlygc *gc;
 for(gc=rdonlygcs;gc;gc=gc->next)
  if((!font || font==gc->font) && pixel==gc->pixel) return gc->gc;
 gc=malloc(sizeof(struct rdonlygc));
 gc->next=rdonlygcs;
 rdonlygcs=gc;
 gc->gc=XCreateGC(dsp,gtwin(root),0,0);
 gc->font=font;
 gc->pixel=pixel;
 XSetForeground(dsp,gc->gc,pixel);
 if(font) XSetFont(dsp,gc->gc,font);
 return gc->gc;
 }

/* Get GC for a specific font and named color.  Give NULL as font,
 * if you don't care what the font is */

GC getgc(font,color)
char *font, *color;
 {
 return getfidpixel(getfont(font),getcolor(color));
 }

/* Get GC for a specific font and RGB value.  Give NULL as font,
 * if you don't care what the font is */

GC getgcrgb(font,r,g,b)
char *font;
 {
 return getfidpixel(getfont(font),getrgb(r,g,b));
 }

/* Get font from a GC */

Font gcfid(gc)
GC gc;
 {
 XGCValues rtn;
 XGetGCValues(dsp,gc,GCFont,&rtn);
 return rtn.font;
 }

/* Get color pixel from a GC */

unsigned long gcpixel(gc)
GC gc;
 {
 XGCValues rtn;
 XGetGCValues(dsp,gc,GCForeground,&rtn);
 return rtn.foreground;
 }

/* Drawing functions */

int txth(gc)
GC gc;
 {
 XGCValues v;
 XFontStruct *fs;
 XGetGCValues(dsp,gc,GCFont,&v);
 fs=getfs(v.font);
 return fs->ascent+fs->descent+3;
 }

int txtb(gc)
GC gc;
 {
 XGCValues v;
 XFontStruct *fs;
 XGetGCValues(dsp,gc,GCFont,&v);
 fs=getfs(v.font);
 return fs->ascent+1;
 }

int txtw(gc,s)
GC gc;
char *s;
 {
 XGCValues v;
 XFontStruct *fs;
 XGetGCValues(dsp,gc,GCFont,&v);
 fs=getfs(v.font);
 return XTextWidth(fs,s,strlen(s));
 }

void drawline(x,gc,x1,y1,x2,y2)
Widget *x;
GC gc;
 {
 XDrawLine(dsp,gtwin(x),gc,x1,y1,x2,y2);
 }

void drawrect(x,gc,x1,y1,w,h)
Widget *x;
GC gc;
 {
 XDrawRectangle(dsp,gtwin(x),gc,x1,y1,w,h);
 }

void drawbox(x,x1,y1,w,h,size,gc)
Widget *x;
GC gc;
 {
 int n;
 for(n=0;n!=size;++n) drawrect(x,gc,x1+n,y1+n,w-n*2-1,h-n*2-1);
 }

void drawfilled(x,gc,x1,y1,w,h)
Widget *x;
GC gc;
 {
 XFillRectangle(dsp,gtwin(x),gc,x1,y1,w,h);
 }

void drawdot(x,gc,x1,y1)
Widget *x;
GC gc;
 {
 XDrawLine(dsp,gtwin(x),gc,x1,y1,x1,y1);
 }

void drawtxt(x,gc,x1,y1,str)
Widget *x;
GC gc;
char *str;
 {
 XDrawString(dsp,gtwin(x),gc,x1,y1,str,strlen(str));
 }

void drawclr(x)
Widget *x;
 {
 XClearWindow(dsp,gtwin(x));
 }

/* Scalable icon system */

/* Draw a line */

void lineto(buf,width,height,x,y,i,j)
char *buf;
 {
 int dist;	/* Distance */
 int incx;	/* X increment value */
 int incy;	/* Y increment value */
 int accu;	/* Accumulator */

 /* Check limits (sorry no clipping...) */
 if(x<0) return;
 if(y<0) return;
 if(x>=width) return;
 if(y>=height) return;
 if(x+i<0) return;
 if(y+j<0) return;
 if(x+i>=width) return;
 if(y+j>=height) return;

 /* Draw end-points */
 buf[x+y*width]=1;
 if(i==0 && j==0) return;
 buf[x+i+(y+j)*width]=1;

 /* Line drawing algorithm */
 if(Iabs(i)<Iabs(j))
  {
  if(i>0) incx=1;
  else incx= -1, i= -i;
  if(j>0) dist=j, incy=1;
  else dist= -j, incy= -1, j= -j;
  accu=j/2;
  do
   {
   buf[x+y*width]=1;
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
   buf[x+y*width]=1;
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

/* A prerendered icon */

struct lin
 {
 struct lin *next;
 short x,y,len;
 GC gc;
 };

struct rpic
 {
 struct rpic *next;	/* Linked list */
 int *pic;		/* Source */
 int w;			/* Width */
 int h;			/* Height */
 struct lin *lines;	/* Lines */
 };

static struct rpic *rpics;	/* Cache of rendered icons */

/* Recursive area filler */

void dofill(buf,w,h,x,y)
char *buf;
 {
 if(x>=0 && x<w && y>=0 && y<h && !buf[x+y*w])
  {
  buf[x+y*w]=255;
  dofill(buf,w,h,x-1,y);
  dofill(buf,w,h,x+1,y);
  dofill(buf,w,h,x,y-1);
  dofill(buf,w,h,x,y+1);
  }
 }

/* Draw cubic bezier */

double bezier(a,b,c,d,t)
double a,b,c,d, t;
 {
 double nt=1.0-t;
 double nt2=nt*nt;
 double t2=t*t;
 return a*nt2*nt + b*3.0*t*nt2 + c*3.0*t2*nt + d*t2*t;
 }

void bezdraw(buf,w,h,xx,yy)
char *buf;
int *xx;
int *yy;
 {
 int x=xx[0], y=yy[0];
 double xxx[4];
 double yyy[4];
 double t;
 xxx[0]=xx[0]; xxx[1]=xx[1]; xxx[2]=xx[2]; xxx[3]=xx[3];
 yyy[0]=yy[0]; yyy[1]=yy[1]; yyy[2]=yy[2]; yyy[3]=yy[3];
 for(t=0.0;t<1.0;t+=.01)
  {
  int nx=bezier(xxx[0],xxx[1],xxx[2],xxx[3],t)+.5;
  int ny=bezier(yyy[0],yyy[1],yyy[2],yyy[3],t)+.5;
  lineto(buf,w,h,x,y,nx-x,ny-y);
  x=nx;
  y=ny;
  }
 lineto(buf,w,h,x,y,xx[3]-x,yy[3]-y);
 }

/* Render an icon */

static struct rpic *render(pic,w,h)
int *pic;
 {
 struct rpic *r=(struct rpic *)malloc(sizeof(struct rpic));
 GC gc;
 GC *outbuf=(GC *)calloc(w*h,sizeof(GC));
 unsigned char *buf=(char *)calloc(w,h);
 int x, y, wid, hgt;
 r->w=w;
 r->h=h;
 r->pic=pic;
 r->lines=0;
 for(;;++pic) switch(gtpiccmd(*pic))
  {
  case picend:
   goto done;

  case picfil:
   {
   for(x=0;x!=w;++x) dofill(buf,w,h,x,0);
   for(x=0;x!=w;++x) dofill(buf,w,h,x,h-1);
   for(y=0;y!=h;++y) dofill(buf,w,h,0,y);
   for(y=0;y!=h;++y) dofill(buf,w,h,w-1,y);
   for(x=0;x!=w*h;++x)
    if(buf[x]==255) buf[x]=0;
    else buf[x]=1;
   break;
   }

  case picmrg:
   {
   gc=getgcrgb(NULL,gtpicr(*pic)*257,gtpicg(*pic)*257,gtpicb(*pic)*257);
   for(y=0;y!=h;++y)
    for(x=0;x!=w;++x)
     if(buf[x+y*w]) outbuf[x+y*w]=gc;
   for(y=0;y!=w*h;++y) buf[y]=0;
   break;
   }

  case picusr:
   {
   for(y=0;y!=h;++y)
    for(x=0;x!=w;++x)
     if(buf[x+y*w]) outbuf[x+y*w]=(GC)1;
   for(y=0;y!=w*h;++y) buf[y]=0;
   break;
   }

  case picbez:
   {
   int xx[4];
   int yy[4];
   ++pic;
   xx[0]=gtpicx(*pic)*w/10000; yy[0]=gtpicy(*pic)*h/10000;
   ++pic;
   xx[1]=gtpicx(*pic)*w/10000; yy[1]=gtpicy(*pic)*h/10000;
   ++pic;
   xx[2]=gtpicx(*pic)*w/10000; yy[2]=gtpicy(*pic)*h/10000;
   ++pic;
   xx[3]=gtpicx(*pic)*w/10000; yy[3]=gtpicy(*pic)*h/10000;
   bezdraw(buf,w,h,xx,yy);
   break;
   }

  case piclin:
   {
   ++pic;
   x=gtpicx(*pic); y=gtpicy(*pic);
   ++pic;
   wid=gtpicx(*pic); hgt=gtpicy(*pic);
   wid=wid-x; hgt=hgt-y;
   /* Scale */
   x=x*w/10000;
   y=y*h/10000;
   wid=wid*w/10000;
   hgt=hgt*h/10000;
   /* Draw line */
   lineto(buf,w,h,x,y,wid,hgt);
   for(y=0;y!=h*w;++y) buf[y]&=127;
   break;
   }
  }

 done:
 free(buf);
 /* Run length encode the output buffer */
 for(y=0;y!=h;++y)
  for(x=0;x!=w;)
   if(gc=outbuf[x+w*y])
    {
    struct lin *lin;
    int len;
    for(len=0;x+len!=w && outbuf[x+w*y+len]==gc;++len);
    lin=(struct lin *)malloc(sizeof(struct lin));
    lin->x=x;
    lin->y=y;
    lin->len=len;
    if(gc==(GC)1) lin->gc=0;
    else lin->gc=gc;
    lin->next=r->lines;
    r->lines=lin;
    x+=len;
    }
   else ++x;
 free(outbuf);
 return r;
 }

/* Find a rendered icon */

static struct rpic *picfind(pic,w,h)
int *pic;
 {
 struct rpic *r;
 for(r=rpics;r;r=r->next)
  if(r->pic==pic && r->w==w && r->h==h) return r;
 r=render(pic,w,h);
 r->next=rpics;
 rpics=r;
 return r;
 }

/* Draw a scalable icon */

void drawpic(x,xpos,ypos,w,h,gc,pic)
Widget *x;
GC gc;
int *pic;
 {
 struct lin *lin;
 struct rpic *rpic=picfind(pic,w,h);
 for(lin=rpic->lines;lin;lin=lin->next)
  drawline(x,lin->gc?lin->gc:gc,
           xpos+lin->x,ypos+lin->y,xpos+lin->x+lin->len-1,ypos+lin->y);
 }

/* Draw a pretty 3d-effect outline */

void draw3dbox(x,xpos,ypos,w,h,thick,tedge,bedge)
Widget *x;
GC tedge, bedge;
 {
 int q;
 for(q=0;q!=thick;++q)
  {
  drawline(x,tedge,xpos+q,ypos+q,xpos+w-1-q,ypos+q);
  drawline(x,tedge,xpos+q,ypos+q,xpos+q,ypos+h-1-q);
  drawline(x,bedge,xpos+w-1-q,ypos+q,xpos+w-1-q,ypos+h-1-q);
  drawline(x,bedge,xpos+q,ypos+h-1-q,xpos+w-1-q,ypos+h-1-q);
  }
 }

/* Force redraw of a window */

void redraw(w)
Widget *w;
 {
 if(gtflg(w)&flgon)
  XClearArea(dsp,gtwin(w),0,0,0,0,1);
 }
