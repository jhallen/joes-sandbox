/* Image widget 
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

static void imagest(v,data)
Image *v;
unsigned char *data;
 {
 int x, y;
 int w=gtwidth(v);
 int h=gtheight(v);
 int d=DefaultDepth(dsp,DefaultScreen(dsp));
 /* printf("depth=%d width=%d height=%d\n",d,w,h); */

 v->data=data;

 if(v->image) XDestroyImage(v->ximage);

 if(d<=8) v->image=malloc(w*h);
 else if(d<=16) v->image=malloc(w*h*2);
 else v->image=malloc(w*h*4);

 v->ximage=XCreateImage(dsp,DefaultVisual(dsp,DefaultScreen(dsp)),
                        d,ZPixmap,0,(char *)v->image,w,h,8,0);
 v->ximage->byte_order=LSBFirst;

 if(d<=8)
  for(x=0;x!=w*h;++x) v->image[x]=v->map[v->data[x]];
 else if(d<=16)
  for(x=0;x!=w*h;++x) *((unsigned short *)v->image+x)=v->map[v->data[x]];
 else if(d<=32)
  for(x=0;x!=w*h;++x) *((unsigned long *)v->image+x)=v->map[v->data[x]];

 redraw(v);
 }

static int imagedraw(v,ev)
Image *v;
XExposeEvent *ev;
 {
 if(v->image)
  {
  XPutImage(dsp,gtwin(v),gtfgnd(v),v->ximage,ev->x,ev->y,ev->x,ev->y,
            ev->width,ev->height);
  }
 return 0;
 }

struct imagefuncs imagefuncs;

Image *mkImage(v)
Image *v;
 {
 int z;
 int d=DefaultDepth(dsp,DefaultScreen(dsp));
 Colormap cmap;
 mkWidget(v);
 if(!imagefuncs.on)
  {
  mcpy(&imagefuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  imagefuncs.expose=imagedraw;
  imagefuncs.st=imagest;
  }
 v->funcs= &imagefuncs;
 v->data=0;
 stw(v,640);
 sth(v,480);
 v->image=0;
 v->ximage=0;
 /* Allocate colors */
 v->map=malloc(256*4);
 cmap=DefaultColormap(dsp,0);
 if(d>=16)
  {
  unsigned long prev=0;
  for(z=0;z!=256;++z)
   {
   XColor xc;
   xc.green=(z+2)<<8;
   xc.red=(z+2)<<8;
   xc.blue=(z+2)<<8;
   xc.flags=(DoRed|DoGreen|DoBlue);
   printf("(%d %d)\n",z,xc.pixel);
   if(!XAllocColor(dsp,cmap,&xc))
    {
/*    printf("(%d %d) ",z,xc.pixel); */
    xc.pixel=prev;
    }
   printf("(%d %d)\n",z,xc.pixel);
   v->map[z]=xc.pixel;
   prev=xc.pixel;
   }
  v->map[245]=v->map[253];
  v->map[255]=v->map[253];
  }
 else
  {
  unsigned long prev=0;
  for(z=0;z!=256;z+=4)
   {
   XColor xc;
   xc.green=(z+2)<<8;
   xc.red=(z+2)<<8;
   xc.blue=(z+2)<<8;
   xc.flags=(DoRed|DoGreen|DoBlue);
   printf("(%d %d)\n",z,xc.pixel);
   if(!XAllocColor(dsp,cmap,&xc))
    {
/*    printf("(%d %d) ",z,xc.pixel); */
    xc.pixel=prev;
    }
   printf("(%d %d)\n",z,xc.pixel);
   prev=xc.pixel;
   v->map[z]=xc.pixel;
   v->map[z+1]=xc.pixel;
   v->map[z+2]=xc.pixel;
   v->map[z+3]=xc.pixel;
   }
  }
 return v;
 }
