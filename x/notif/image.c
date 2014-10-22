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

static void imagerm(v)
Image *v;
 {
 free(v->map);
 if(v->image) free(v->image);
 if(v->ximage) XDestroyImage(v->ximage);
 (widgetfuncs.rm)(v);
 }

static void imagest(v,data)
Image *v;
unsigned char *data;
 {
 int x;
 int z;
 int w=gtwidth(v);
 int h=gtheight(v);
 int d;
 Screen *scn;
 scn=DefaultScreenOfDisplay(dsp);
 d=DefaultDepthOfScreen(scn)/8;
 printf("%d %d %d\n",w,h,d);
 v->data=data;
 if(v->image)
  {
  free(v->image);
  XDestroyImage(v->ximage);
  }
 v->image=malloc(w*h*4);
 v->ximage=XCreateImage(dsp,DefaultVisual(dsp,DefaultScreen(dsp)),
                        d*8,ZPixmap,0,(char *)v->image,w,h,32,0);
 v->ximage->byte_order=LSBFirst;
 d=4;
 for(x=0;x!=w*h;++x)
  {
  unsigned pix=v->map[v->data[x]];
  for(z=0;z!=4;++z)
    v->image[x*4+z]=(pix>>(z*8));
  }
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
 Screen *scn;
 Colormap cmap;
 mkWidget(v);
 if(!imagefuncs.on)
  {
  mcpy(&imagefuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  imagefuncs.expose=imagedraw;
  imagefuncs.st=imagest;
  imagefuncs.rm=imagerm;
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
 for(z=0;z!=256;++z)
  {
  XColor xc;
  xc.green=(z+2)<<8;
  xc.red=(z+2)<<8;
  xc.blue=(z+2)<<8;
  xc.flags=(DoRed|DoGreen|DoBlue);
  if(!XAllocColor(dsp,cmap,&xc))
   {
   printf("(%d %d) ",z,xc.pixel);
   }
  v->map[z]=xc.pixel;
  }
 v->map[245]=v->map[253];
 v->map[255]=v->map[253];

 /* Screen information */
 scn=DefaultScreenOfDisplay(dsp);
 printf("defaultdepthofscreen=%d\n",DefaultDepthOfScreen(scn));

 return v;
 }
