/* Tedit widget 
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

static int getnparm(char *str,int x,int *data)
 {
 while(str[x]==' ') ++x;
 if(str[x]>='0' && str[x]<='9')
  *data=atoi(str+x);
 else
  *data= -1;
 while(str[x]>='0' && str[x]<='9') ++x;
 return x;
 }

void Tedit::stfield(int f,char *val)
 {
 strcpy(fields[f],val);
 lens[f]=strlen(val);
 }

void Tedit::st(char *str)
 {
 int x;
 int cnt;

 /* Calculate no. of fields */
 for(cnt=0,x=0;str[x];++cnt)
  if(str[x]=='{')
   { /* Skip over edit field */
   while(str[x] && str[x]!='}') ++x;
   if(str[x]=='}') ++x;
   }
  else
   { /* Skip over constant field */
   while(str[x] && str[x]!='{') ++x;
   }

 /* Allocate space for fields */
 nfields=cnt;
 fields=(char **)malloc(sizeof(char *)*nfields);
 lens=(int *)malloc(sizeof(int)*nfields);
 sizs=(int *)malloc(sizeof(int)*nfields);
 wids=(int *)malloc(sizeof(int)*nfields);
 poss=(int *)malloc(sizeof(int)*nfields);
 flgs=(int *)malloc(sizeof(int)*nfields);
 data0=(int *)malloc(sizeof(int)*nfields);
 data1=(int *)malloc(sizeof(int)*nfields);
 data2=(int *)malloc(sizeof(int)*nfields);

 /* Scan fields */
 for(cnt=0,x=0;cnt!=nfields;++cnt)
  if(str[x]=='{')
   {
   int parm;
   /* Parse edit field */
   if(str[x+1]=='d') flgs[cnt]=1;	/* Decimal w/ range */
   x=getnparm(str,x+2,data0+cnt);
   x=getnparm(str,x,data1+cnt);
   x=getnparm(str,x,data2+cnt);
   while(str[x] && str[x]!='}') ++x;
   if(str[x]=='}') ++x;
   /* Set up buffer */
   lens[cnt]=0;
   sizs[cnt]=data0[cnt]+1;
   fields[cnt]=(char *)malloc(sizs[cnt]);
   fields[cnt][0]=0;
   wids[cnt]=txtw(gtfgnd(),"0")*data0[cnt];
   if(cnt) poss[cnt]=poss[cnt-1]+wids[cnt-1];
   else poss[cnt]=0;
   }
  else
   { /* Field is just a constant */
   int y;
   for(y=0;str[x+y] && str[x+y]!='{';++y);
   flgs[cnt]=0;
   lens[cnt]=y;
   sizs[cnt]=y+1;
   fields[cnt]=(char *)malloc(sizs[cnt]);
   for(y=0;y!=lens[cnt];++y)
    fields[cnt][y]=str[x+y];
   fields[cnt][y]=0;
   wids[cnt]=txtw(gtfgnd(),fields[cnt]);
   if(cnt) poss[cnt]=poss[cnt-1]+wids[cnt-1];
   else poss[cnt]=0;
   x+=y;
   }
 redraw(this);
 }

char **Tedit::gt()
 {
 return fields;
 }

/* Calculate text offset and cursor position.  Returns true if text offset
   changed */

int Tedit::pos()
 {
 int winwid=gtwidth()-xpos*2;
 int tofst=ofst;
 char c;

 if(havefocus)
  {
  if(pushmode)
   {
   curpos=txtw(gtfgnd(),"0")*data0[curfield]+poss[curfield];
   }
  else
   {
   c=fields[curfield][cur];
   fields[curfield][cur]=0;
   curpos=txtw(gtfgnd(),fields[curfield])+poss[curfield];
   fields[curfield][cur]=c;
   }
  if(curpos-tofst<0)
   tofst=curpos;
  else if(curpos-tofst>winwid)
   tofst=curpos-winwid;
  }
 else tofst=0;
 if(tofst!=ofst)
  {
  ofst=tofst;
  return 1;
  }
 else return 0;
 }

int Tedit::expose(XEvent *ev)
 {
 GC gc=gtfgnd();
 int cnt;
 int yy=ypos-txtb(gtfgnd());
 int yyh=txth(gtfgnd());
 drawclr(this);
 pos();
 for(cnt=0;cnt!=nfields;++cnt)
  {
  int txtwid=txtw(gc,fields[cnt]);
  if(havefocus && !tediting && flgs[cnt])
   { /* Everything is highlighted */
   if(pushmode)
    {
    drawfilled(this,gc,xpos-ofst+poss[cnt]+(data0[cnt]-lens[cnt])*txtw(gtfgnd(),"0"),yy,txtwid,yyh);
    drawtxt(this,getfidpixel(gcfid(gc),getcolor("white")),
            xpos-ofst+poss[cnt]+(data0[cnt]-lens[cnt])*txtw(gtfgnd(),"0"),ypos,fields[cnt]);
    }
   else
    {
    drawfilled(this,gc,xpos-ofst+poss[cnt],yy,txtwid,yyh);
    drawtxt(this,getfidpixel(gcfid(gc),getcolor("white")),
            xpos-ofst+poss[cnt],ypos,fields[cnt]);
    }
   }
  else
   {
   if(pushmode && flgs[cnt])
    drawtxt(this,gc,xpos-ofst+poss[cnt]+(data0[cnt]-lens[cnt])*txtw(gtfgnd(),"0"),ypos,fields[cnt]);
   else
    drawtxt(this,gc,xpos-ofst+poss[cnt],ypos,fields[cnt]);
   }
  }
 if(havefocus==1)
  {
  drawline(this,gc,xpos+curpos-ofst,yy,
           xpos+curpos-ofst,yy+yyh);
  }
 return 0;
 }

/* Erase cursor */

void Tedit::curoff()
 {
 int yy=ypos-txtb(gtfgnd());
 drawline(this,gtbknd(),xpos+curpos-ofst,yy,
                      xpos+curpos-ofst,yy+txth(gtfgnd()));
 }

static void doblink(Tedit *t)
 {
  if(t->havefocus==2) t->havefocus=1;
  else if(t->havefocus==1) t->havefocus=2;
  if(t->on_flag)
   {
   t->curoff();
   t->expose(NULL);
   }
  submit(650000,t->blink=new Fn1_0<void,Tedit *>(doblink,t));
 }

int Tedit::focus()
 {
 Widget::focus();
 if(focusrtn) stfocusrtn(focusrtn);
 if(!havefocus)
  {
  int x, y;
  havefocus=2;
  if(pushmode)
   {
   for(x=0;x!=nfields;++x)
    if(flgs[x]) curfield=x, cur=lens[x];
   }
  else
   /* Put cursor at end of text */
   for(x=0;x!=nfields;++x)
    if(flgs[x] && lens[x]) curfield=x, cur=lens[x];
  redraw(this);
  doblink(this);
  }
 return 0;
 }

/* Update tedit window.  Erase cursor, calculate new cursor and text offsets,
 * draw window and cursor, start cursor blink timer */

void Tedit::update()
 {
 if(blink) cancel(blink), blink=0;
 curoff();
 update_flag|=pos();
 havefocus=2;
 doblink(this);
 if(update_flag) redraw(this), update_flag=0;
 else expose(NULL);
 }

int Tedit::unfocus()
 {
 Widget::unfocus();
 havefocus=0;
 tediting=0;
 if(blink) cancel(blink), blink=0;
 redraw(this);
 return 0;
 }

/* Tedit functions */

int Tedit::ubacks(int key,int state,int xx,int y,Time time,Widget *org)
 {
  if(!tediting) update_flag=1;
  tediting=1;
  /* Backspace */
  if(pushmode)
   {
   int x;
   key= -1;
   for(x=0;x!=nfields;++x)
    if(flgs[x])
     {
     int bumpkey;
     if(lens[x])
      {
      bumpkey=fields[x][lens[x]-1];
      if(key!=-1)
       {
       if(lens[x]-1) memmove(fields[x]+1,fields[x],lens[x]-1);
       fields[x][0]=key;
       }
      else
       {
       --lens[x];
       fields[x][lens[x]]=0;
       }
      key=bumpkey;
      }
     }
   update();
   }
  else
   {
   if(cur)
    {
    --cur;
    fields[curfield][cur]=0;
    lens[curfield]=cur;
    update();
    }
   else if(curfield)
    {
    int x;
    for(x=curfield-1;x && !flgs[x];--x);
    if(flgs[x])
     {
     curfield=x;
     cur=lens[curfield];
     update();
     }
    }
   }
 return 0;
 }

int Tedit::utype(int key,int state,int xx,int y,Time time,Widget *org)
 {
  char k=key;
  if(!tediting)
   { /* Replace with new text */
   int x;
   tediting=1;
   update_flag=1;
   /* Blank-out text; clear cursor */
   if(pushmode)
    {
    for(x=0;x!=nfields;++x)
     if(flgs[x])
      curfield=x;
    }
   else
    {
    for(x=nfields-1;x>=0;--x)
     if(flgs[x])
      curfield=x;
    }
   cur=0;
   for(x=0;x!=nfields;++x)
    if(flgs[x])
     fields[x][0]=0,
     lens[x]=0;
   }
  /* Numeric field */
  if(key>='0' && key<='9')
   {
   if(pushmode)
    {
    for(x=nfields-1;x>=0;--x)
     if(flgs[x])
      if(lens[x]==data0[x])
       {
       int bumpkey;
       bumpkey=fields[x][0];
       if(lens[x]-1) memmove(fields[x],fields[x]+1,lens[x]-1);
       fields[x][lens[x]-1]=key;
       key=bumpkey;
       }
      else
       {
       fields[x][lens[x]++]=key;
       fields[x][lens[x]]=0;
       if(x==curfield) ++cur;
       break;
       }
    update();
    }
   else
    {
    /* Append character */
    if(cur!=data0[curfield])
     {
     fields[curfield][cur++]=key;
     fields[curfield][cur]=0;
     lens[curfield]=cur;
     /* Check against max limit */
     if(atoi(fields[curfield])>data2[curfield])
      { /* Oops, too high.  Do not accept */
      fields[curfield][--cur]=0;
      lens[curfield]=cur;
      }
     else if(cur==data0[curfield])
      { /* Advance to next field if not too low */
      if(atoi(fields[curfield])>=data1[curfield])
       {
       int x;
       for(x=curfield+1;x!=nfields && !flgs[x];++x);
       if(x!=nfields)
        {
        curfield=x;
        cur=0;
        }
       }
      }
     update();
     }
    }
   }
 return 0;
 }

int Tedit::urtn(int key,int state,int xx,int y,Time time,Widget *org)
 {
  int x;
  /* Allow return only if all fields are in range */
  for(x=0;x!=nfields;++x)
   if(flgs[x])
    {
    int n=atoi(fields[x]);
    if(n<data1[x] || n>data2[x])
     return -1;
    else
     {
     char buf[10];
     switch(data0[x])
      {
      case 1: sprintf(buf,"%1.1d",atoi(fields[x])); stfield(x,buf); break;
      case 2: sprintf(buf,"%2.2d",atoi(fields[x])); stfield(x,buf); break;
      case 3: sprintf(buf,"%3.3d",atoi(fields[x])); stfield(x,buf); break;
      case 4: sprintf(buf,"%4.4d",atoi(fields[x])); stfield(x,buf); break;
      case 5: sprintf(buf,"%5.5d",atoi(fields[x])); stfield(x,buf); break;
      case 6: sprintf(buf,"%6.6d",atoi(fields[x])); stfield(x,buf); break;
      case 7: sprintf(buf,"%7.7d",atoi(fields[x])); stfield(x,buf); break;
      }
     }
    }
  clfocus(this);
  if (fn) fn->cont(fn);
  return 0;
 }

void Tedit::stfn(Fn_0<void> *c)
 {
 fn=c;
 }

void Tedit::strtn(Widget *w)
 {
 focusrtn=w;
 }

int Tedit::gtheight()
 {
 if(ypos== -1)
  {
  if(!ckh())
   stheight(txth(gtfgnd()));
  xpos=txtw(gtfgnd(),"M")/2+1;
  ypos=Widget::gtheight()/2-
          txth(gtfgnd())/2+txtb(gtfgnd());
  }
 return Widget::gtheight();
 }

int Tedit::upress1(int key,int state,int xx,int y,Time time,Widget *org)
 {
  if(!havefocus)
   stfocus(this);
 return 0;
 }

Kmap<Key> teditkmap;
int teditkmap_flag;

Tedit::Tedit()
 {
 if(!teditkmap_flag)
  {
  teditkmap_flag = 1;
  teditkmap.merge(stdkmap);
  teditkmap.add("^H",new Mfn0_6<int,Tedit,int,int,int,int,Time,Widget *>(&Tedit::ubacks,NULL));
  teditkmap.add("^?",new Mfn0_6<int,Tedit,int,int,int,int,Time,Widget *>(&Tedit::ubacks,NULL));
  teditkmap.add("Press1",new Mfn0_6<int,Tedit,int,int,int,int,Time,Widget *>(&Tedit::upress1,NULL));
  teditkmap.add("SP TO ~",new Mfn0_6<int,Tedit,int,int,int,int,Time,Widget *>(&Tedit::utype,NULL));
  teditkmap.add("^M",new Mfn0_6<int,Tedit,int,int,int,int,Time,Widget *>(&Tedit::urtn,NULL));
  }
 stkmap(&teditkmap);
 stborder(new Eborder());
 stwidth(250);
 ypos= -1;
 havefocus=0;
 nfields=0;
 fields=0;
 lens=0;
 sizs=0;
 wids=0;
 poss=0;
 flgs=0;
 data0=0;
 data1=0;
 data2=0;
 cur=0;
 curfield=0;
 curpos=0;
 update_flag=0;
 ofst=0;
 fn=0;
 pushmode=0;
 tediting=0;
 blink=0;
 focusrtn=0;
 stfocusable();
 }

Tedit::~Tedit()
  {
  if (fields)
    {
    int z;
    for (z = 0; z != nfields; ++z)
      free(fields[z]);
    free(fields);
    }
  if (lens) free(lens);
  if (sizs) free(sizs);
  if (wids) free(wids);
  if (poss) free(poss);
  if (flgs) free(flgs);
  if (data0) free(data0);
  if (data1) free(data1);
  if (data2) free(data2);
  }
