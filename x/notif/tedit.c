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

static void teditrm(t)
Tedit *t;
 {
 int x;
 cancel(t->fn); t->fn=0;
 cancel(t->blink);
 if(t->fields)
  {
  for(x=0;x!=t->nfields;++x)
   free(t->fields[x]);
  free(t->fields);
  }
 if(t->lens) free(t->lens);
 if(t->sizs) free(t->sizs);
 if(t->wids) free(t->wids);
 if(t->poss) free(t->poss);
 if(t->flgs) free(t->flgs);
 if(t->data0) free(t->data0);
 if(t->data1) free(t->data1);
 if(t->data2) free(t->data2);
 (widgetfuncs.rm)(t);
 }

static int getnparm(str,x,data)
char *str;
int *data;
 {
 while(str[x]==' ') ++x;
 if(str[x]>='0' && str[x]<='9')
  *data=atoi(str+x);
 else
  *data= -1;
 while(str[x]>='0' && str[x]<='9') ++x;
 return x;
 }

static void teditstfield(t,f,val)
Tedit *t;
char *val;
 {
 strcpy(t->fields[f],val);
 t->lens[f]=strlen(val);
 }

static void teditst(t,str)
Tedit *t;
char *str;
 {
 int x;
 int cnt;

 /* Delete existing fields */
 if(t->fields)
  {
  for(x=0;x!=t->nfields;++x)
   free(t->fields[x]);
  free(t->fields);
  }
 if(t->lens) free(t->lens);
 if(t->sizs) free(t->sizs);
 if(t->wids) free(t->wids);
 if(t->poss) free(t->poss);
 if(t->flgs) free(t->flgs);
 if(t->data0) free(t->data0);
 if(t->data1) free(t->data1);
 if(t->data2) free(t->data2);

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
 t->nfields=cnt;
 t->fields=(char **)malloc(sizeof(char *)*t->nfields);
 t->lens=(int *)malloc(sizeof(int)*t->nfields);
 t->sizs=(int *)malloc(sizeof(int)*t->nfields);
 t->wids=(int *)malloc(sizeof(int)*t->nfields);
 t->poss=(int *)malloc(sizeof(int)*t->nfields);
 t->flgs=(int *)malloc(sizeof(int)*t->nfields);
 t->data0=(int *)malloc(sizeof(int)*t->nfields);
 t->data1=(int *)malloc(sizeof(int)*t->nfields);
 t->data2=(int *)malloc(sizeof(int)*t->nfields);

 /* Scan fields */
 for(cnt=0,x=0;cnt!=t->nfields;++cnt)
  if(str[x]=='{')
   {
   int parm;
   /* Parse edit field */
   if(str[x+1]=='d') t->flgs[cnt]=1;	/* Decimal w/ range */
   x=getnparm(str,x+2,t->data0+cnt);
   x=getnparm(str,x,t->data1+cnt);
   x=getnparm(str,x,t->data2+cnt);
   while(str[x] && str[x]!='}') ++x;
   if(str[x]=='}') ++x;
   /* Set up buffer */
   t->lens[cnt]=0;
   t->sizs[cnt]=t->data0[cnt]+1;
   t->fields[cnt]=(char *)malloc(t->sizs[cnt]);
   t->fields[cnt][0]=0;
   t->wids[cnt]=txtw(gtfgnd(t),"0")*t->data0[cnt];
   if(cnt) t->poss[cnt]=t->poss[cnt-1]+t->wids[cnt-1];
   else t->poss[cnt]=0;
   }
  else
   { /* Field is just a constant */
   int y;
   for(y=0;str[x+y] && str[x+y]!='{';++y);
   t->flgs[cnt]=0;
   t->lens[cnt]=y;
   t->sizs[cnt]=y+1;
   t->fields[cnt]=(char *)malloc(t->sizs[cnt]);
   for(y=0;y!=t->lens[cnt];++y)
    t->fields[cnt][y]=str[x+y];
   t->fields[cnt][y]=0;
   t->wids[cnt]=txtw(gtfgnd(t),t->fields[cnt]);
   if(cnt) t->poss[cnt]=t->poss[cnt-1]+t->wids[cnt-1];
   else t->poss[cnt]=0;
   x+=y;
   }
 redraw(t);
 }

static char **teditgt(t)
Tedit *t;
 {
 return t->fields;
 }

/* Calculate text offset and cursor position.  Returns true if text offset
   changed */

static int teditpos(t)
Tedit *t;
 {
 int winwid=gtwidth(t)-t->xpos*2;
 int ofst=t->ofst;
 char c;

 if(t->havefocus)
  {
  if(t->pushmode)
   {
   t->curpos=txtw(gtfgnd(t),"0")*t->data0[t->curfield]+t->poss[t->curfield];
   }
  else
   {
   c=t->fields[t->curfield][t->cur];
   t->fields[t->curfield][t->cur]=0;
   t->curpos=txtw(gtfgnd(t),t->fields[t->curfield])+t->poss[t->curfield];
   t->fields[t->curfield][t->cur]=c;
   }
  if(t->curpos-ofst<0)
   ofst=t->curpos;
  else if(t->curpos-ofst>winwid)
   ofst=t->curpos-winwid;
  }
 else ofst=0;
 if(ofst!=t->ofst)
  {
  t->ofst=ofst;
  return 1;
  }
 else return 0;
 }

static int teditdraw(t)
Tedit *t;
 {
 GC gc=gtfgnd(t);
 int cnt;
 int yy=t->ypos-txtb(gtfgnd(t),"M");
 int yyh=txth(gtfgnd(t),"M");
 drawclr(t);
 teditpos(t);
 for(cnt=0;cnt!=t->nfields;++cnt)
  {
  int txtwid=txtw(gc,t->fields[cnt]);
  if(t->havefocus && !t->tediting && t->flgs[cnt])
   { /* Everything is highlighted */
   if(t->pushmode)
    {
    drawfilled(t,gc,t->xpos-t->ofst+t->poss[cnt]+(t->data0[cnt]-t->lens[cnt])*txtw(gtfgnd(t),"0"),yy,txtwid,yyh);
    drawtxt(t,getfidpixel(gcfid(gc),getcolor("white")),
            t->xpos-t->ofst+t->poss[cnt]+(t->data0[cnt]-t->lens[cnt])*txtw(gtfgnd(t),"0"),t->ypos,t->fields[cnt]);
    }
   else
    {
    drawfilled(t,gc,t->xpos-t->ofst+t->poss[cnt],yy,txtwid,yyh);
    drawtxt(t,getfidpixel(gcfid(gc),getcolor("white")),
            t->xpos-t->ofst+t->poss[cnt],t->ypos,t->fields[cnt]);
    }
   }
  else
   {
   if(t->pushmode && t->flgs[cnt])
    drawtxt(t,gc,t->xpos-t->ofst+t->poss[cnt]+(t->data0[cnt]-t->lens[cnt])*txtw(gtfgnd(t),"0"),t->ypos,t->fields[cnt]);
   else
    drawtxt(t,gc,t->xpos-t->ofst+t->poss[cnt],t->ypos,t->fields[cnt]);
   }
  }
 if(t->havefocus==1)
  {
  drawline(t,gc,t->xpos+t->curpos-t->ofst,yy,
           t->xpos+t->curpos-t->ofst,yy+yyh);
  }
 return 0;
 }

/* Erase cursor */

static void teditcuroff(t)
Tedit *t;
 {
 int yy=t->ypos-txtb(gtfgnd(t),"M");
 drawline(t,gtbknd(t),t->xpos+t->curpos-t->ofst,yy,
                      t->xpos+t->curpos-t->ofst,yy+txth(gtfgnd(t),"M"));
 }

static int teditdoblink(stat,t)
Tedit *t;
 {
 if(!stat)
  {
  if(t->havefocus==2) t->havefocus=1;
  else if(t->havefocus==1) t->havefocus=2;
  if(gtflg(t)&flgon)
   {
   teditcuroff(t); teditdraw(t);
   }
  submit(650000,fn1(t->blink,teditdoblink,t));
  }
 return stat;
 }

static int teditfocus(t)
Tedit *t;
 {
 (widgetfuncs.focus)(t);
 if(t->focusrtn) stfocusrtn(t->focusrtn);
 if(!t->havefocus)
  {
  int x, y;
  t->havefocus=2;
  if(t->pushmode)
   {
   for(x=0;x!=t->nfields;++x)
    if(t->flgs[x]) t->curfield=x, t->cur=t->lens[x];
   }
  else
   /* Put cursor at end of text */
   for(x=0;x!=t->nfields;++x)
    if(t->flgs[x] && t->lens[x]) t->curfield=x, t->cur=t->lens[x];
  redraw(t);
  teditdoblink(0,t);
  }
 return 0;
 }

/* Update tedit window.  Erase cursor, calculate new cursor and text offsets,
 * draw window and cursor, start cursor blink timer */

static void teditupdate(t)
Tedit *t;
 {
 cancel(t->blink);
 teditcuroff(t);
 t->update|=teditpos(t);
 t->havefocus=2;
 teditdoblink(0,t);
 if(t->update) redraw(t), t->update=0;
 else teditdraw(t);
 }

static int teditunfocus(t)
Tedit *t;
 {
 (widgetfuncs.unfocus)(t);
 t->havefocus=0;
 t->tediting=0;
 cancel(t->blink);
 redraw(t);
 return 0;
 }

/* Tedit functions */

static int ubacks(stat,t,junk,key,state,x,y,org)
Tedit *t;
 {
 if(!stat)
  {
  if(!t->tediting) t->update=1;
  t->tediting=1;
  /* Backspace */
  if(t->pushmode)
   {
   int x;
   key= -1;
   for(x=0;x!=t->nfields;++x)
    if(t->flgs[x])
     {
     int bumpkey;
     if(t->lens[x])
      {
      bumpkey=t->fields[x][t->lens[x]-1];
      if(key!=-1)
       {
       if(t->lens[x]-1) memmove(t->fields[x]+1,t->fields[x],t->lens[x]-1);
       t->fields[x][0]=key;
       }
      else
       {
       --t->lens[x];
       t->fields[x][t->lens[x]]=0;
       }
      key=bumpkey;
      }
     }
   teditupdate(t);
   }
  else
   {
   if(t->cur)
    {
    --t->cur;
    t->fields[t->curfield][t->cur]=0;
    t->lens[t->curfield]=t->cur;
    teditupdate(t);
    }
   else if(t->curfield)
    {
    int x;
    for(x=t->curfield-1;x && !t->flgs[x];--x);
    if(t->flgs[x])
     {
     t->curfield=x;
     t->cur=t->lens[t->curfield];
     teditupdate(t);
     }
    }
   }
  }
 return stat;
 }

static int utype(stat,t,junk,key,state,x,y,org)
Tedit *t;
 {
 if(!stat)
  {
  char k=key;
  if(!t->tediting)
   { /* Replace with new text */
   int x;
   t->tediting=1;
   t->update=1;
   /* Blank-out text; clear cursor */
   if(t->pushmode)
    {
    for(x=0;x!=t->nfields;++x)
     if(t->flgs[x])
      t->curfield=x;
    }
   else
    {
    for(x=t->nfields-1;x>=0;--x)
     if(t->flgs[x])
      t->curfield=x;
    }
   t->cur=0;
   for(x=0;x!=t->nfields;++x)
    if(t->flgs[x])
     t->fields[x][0]=0,
     t->lens[x]=0;
   }
  /* Numeric field */
  if(key>='0' && key<='9')
   {
   if(t->pushmode)
    {
    for(x=t->nfields-1;x>=0;--x)
     if(t->flgs[x])
      if(t->lens[x]==t->data0[x])
       {
       int bumpkey;
       bumpkey=t->fields[x][0];
       if(t->lens[x]-1) memmove(t->fields[x],t->fields[x]+1,t->lens[x]-1);
       t->fields[x][t->lens[x]-1]=key;
       key=bumpkey;
       }
      else
       {
       t->fields[x][t->lens[x]++]=key;
       t->fields[x][t->lens[x]]=0;
       if(x==t->curfield) ++t->cur;
       break;
       }
    teditupdate(t);
    }
   else
    {
    /* Append character */
    if(t->cur!=t->data0[t->curfield])
     {
     t->fields[t->curfield][t->cur++]=key;
     t->fields[t->curfield][t->cur]=0;
     t->lens[t->curfield]=t->cur;
     /* Check against max limit */
     if(atoi(t->fields[t->curfield])>t->data2[t->curfield])
      { /* Oops, too high.  Do not accept */
      t->fields[t->curfield][--t->cur]=0;
      t->lens[t->curfield]=t->cur;
      }
     else if(t->cur==t->data0[t->curfield])
      { /* Advance to next field if not too low */
      if(atoi(t->fields[t->curfield])>=t->data1[t->curfield])
       {
       int x;
       for(x=t->curfield+1;x!=t->nfields && !t->flgs[x];++x);
       if(x!=t->nfields)
        {
        t->curfield=x;
        t->cur=0;
        }
       }
      }
     teditupdate(t);
     }
    }
   }
  }
 return stat;
 }

static int urtn(stat,t,junk,key,state,x,y,org)
Tedit *t;
 {
 if(!stat)
  {
  int x;
  /* Allow return only if all fields are in range */
  for(x=0;x!=t->nfields;++x)
   if(t->flgs[x])
    {
    int n=atoi(t->fields[x]);
    if(n<t->data1[x] || n>t->data2[x])
     return -1;
    else
     {
     char buf[10];
     switch(t->data0[x])
      {
      case 1: sprintf(buf,"%1.1d",atoi(t->fields[x])); stfield(t,x,buf); break;
      case 2: sprintf(buf,"%2.2d",atoi(t->fields[x])); stfield(t,x,buf); break;
      case 3: sprintf(buf,"%3.3d",atoi(t->fields[x])); stfield(t,x,buf); break;
      case 4: sprintf(buf,"%4.4d",atoi(t->fields[x])); stfield(t,x,buf); break;
      case 5: sprintf(buf,"%5.5d",atoi(t->fields[x])); stfield(t,x,buf); break;
      case 6: sprintf(buf,"%6.6d",atoi(t->fields[x])); stfield(t,x,buf); break;
      case 7: sprintf(buf,"%7.7d",atoi(t->fields[x])); stfield(t,x,buf); break;
      }
     }
    }
  clfocus(t);
  cont0(t->fn); t->fn=0;
  return 0;
  }
 return stat;
 }

static void teditstfn(t,c)
Tedit *t;
TASK *c;
 {
 t->fn=c;
 }

static void teditstrtn(t,w)
Tedit *t;
Widget *w;
 {
 t->focusrtn=w;
 }

static int teditgtheight(t)
Tedit *t;
 {
 if(t->ypos== -1)
  {
  if(!ckh(t))
   stheight(t,txth(gtfgnd(t),"M"));
  t->xpos=txtw(gtfgnd(t),"M")/2+1;
  t->ypos=(widgetfuncs.gtheight)(t)/2-
          txth(gtfgnd(t),"M")/2+txtb(gtfgnd(t),"M");
  }
 return (widgetfuncs.gtheight)(t);
 }

static int upress1(stat,t,junk,key,state,x,y,org)
Edit *t;
 {
 if(!stat)
  {
  if(!t->havefocus)
   stfocus(t);
  }
 return stat;
 }

struct teditfuncs teditfuncs;

Tedit *mkTedit(t)
Tedit *t;
 {
 mkWidget(t);
 if(!teditfuncs.on)
  {
  mcpy(&teditfuncs,&widgetfuncs,sizeof(struct widgetfuncs));
  teditfuncs.st=teditst;
  teditfuncs.gt=teditgt;
  teditfuncs.expose=teditdraw;
  teditfuncs.unfocus=teditunfocus;
  teditfuncs.focus=teditfocus;
  teditfuncs.stfn=teditstfn;
  teditfuncs.strtn=teditstrtn;
  teditfuncs.gtheight=teditgtheight;
  teditfuncs.stfield=teditstfield;
  teditfuncs.kmap=mkkmap(NULL,NULL,NULL);
  teditfuncs.rm=teditrm;
  kcpy(teditfuncs.kmap,widgetfuncs.kmap);
  kadd(teditfuncs.kmap,NULL,"Press1",upress1,NULL);
  kadd(teditfuncs.kmap,NULL,"^H",ubacks,NULL);
  kadd(teditfuncs.kmap,NULL,"^?",ubacks,NULL);
  kadd(teditfuncs.kmap,NULL,"SP TO ~",utype,NULL);
  kadd(teditfuncs.kmap,NULL,"^M",urtn,NULL);
  }
 t->funcs= &teditfuncs;
 stkmap(t,teditfuncs.kmap);
 stborder(t,mk(Eborder));
 stwidth(t,250);
 iztask(t->blink);
 t->ypos= -1;
 t->havefocus=0;
 t->nfields=0;
 t->fields=0;
 t->lens=0;
 t->sizs=0;
 t->wids=0;
 t->poss=0;
 t->flgs=0;
 t->data0=0;
 t->data1=0;
 t->data2=0;
 t->cur=0;
 t->curfield=0;
 t->curpos=0;
 t->update=0;
 t->ofst=0;
 t->fn=0;
 t->pushmode=0;
 t->tediting=0;
 t->focusrtn=0;
 stflg(t,gtflg(t)|flgfocus);
 return t;
 }
