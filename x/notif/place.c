/* Placement manager 
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

static XContext rels;			/* Widget -> struct relpos database */

struct relpos
 {
 int lofst, rofst, tofst, bofst;	/* Rel position */
 };

/* hmode values: 0=ltor, 1=ledge, 2=lsame, 3=rtol, 4=redge, 5=rsame, 6=center */
/* vmode values: 0=ttob, 1=tedge, 2=tsame, 3=btot, 4=bedge, 5=bsame, 6=center */

static void pmgrcalcw(y)
Pmgr *y;
 {
 Lith *x=y->in;
 LINK *l;
 int max= -1;
 for(l=x->kids->next;l!=x->kids;l=l->next)
  {
  Widget *w=(Widget *)l->data;
  int this=gtw(w);
  struct relpos *rel;
  if(!XFindContext(dsp,(int)w,rels,(XPointer *)&rel))
   {
   if(rel->lofst!=-1) this+=rel->lofst;
   if(rel->rofst!=-1) this+=rel->rofst;
   if(this>max) max=this;
   }
  else if(ckx(w))
   {
   this+=gtx(w);
   if(this>max) max=this;
   }
  }
 if(max==-1) max=16;
 stwidth(x,max);
 }

static void pmgrcalch(y)
Pmgr *y;
 {
 Lith *x=y->in;
 LINK *l;
 int max= -1;
 for(l=x->kids->next;l!=x->kids;l=l->next)
  {
  Widget *w=(Widget *)l->data;
  int this=gth(w);
  struct relpos *rel;
  if(!XFindContext(dsp,(int)w,rels,(XPointer *)&rel))
   {
   if(rel->tofst!=-1) this+=rel->tofst;
   if(rel->bofst!=-1) this+=rel->bofst;
   if(this>max) max=this;
   }
  else if(cky(w))
   {
   this+=gty(w);
   if(this>max) max=this;
   }
  }
 if(max==-1) max=16;
 stheight(x,max);
 }

static void dox(w)
Widget *w;
 {
 struct relpos *rel;
 if(!XFindContext(dsp,(int)w,rels,(XPointer *)&rel))
  {
  if(rel->lofst!=-1 && rel->rofst!=-1)
   stw(w,gtwidth(w->in)-rel->lofst-rel->rofst);
  if(rel->lofst!=-1) stx(w,rel->lofst);
  else if(rel->rofst!=-1) stx(w,gtwidth(w->in)-rel->rofst-gtw(w));
  else stx(w,gtwidth(w->in)/2-gtw(w)/2);
  }
 }

static void pmgrcalcx(y)
Pmgr *y;
 {
 Lith *x=y->in;
 LINK *l;
 for(l=x->kids->next;l!=x->kids;l=l->next) dox(l->data);
 }

static void doy(w)
Widget *w;
 {
 struct relpos *rel;
 if(!XFindContext(dsp,(int)w,rels,(XPointer *)&rel))
  {
  if(rel->tofst!=-1 && rel->bofst!=-1)
   sth(w,gtheight(w->in)-rel->tofst-rel->bofst);
  if(rel->tofst!=-1) sty(w,rel->tofst);
  else if(rel->bofst!=-1) sty(w,gtheight(w->in)-rel->bofst-gth(w));
  else sty(w,gtheight(w->in)/2-gth(w)/2);
  }
 }

static void pmgrcalcy(y)
Pmgr *y;
 {
 Lith *x=y->in;
 LINK *l;
 for(l=x->kids->next;l!=x->kids;l=l->next) doy(l->data);
 }

static void pmgradd(in,x)
Pmgr *in;
Widget *x;
 {
 struct relpos *rel;
 rel=(struct relpos *)malloc(sizeof(struct relpos));
 XSaveContext(dsp,(int)x,rels,(XPointer)rel);
 rel->lofst=rel->rofst=rel->tofst=rel->bofst= -1;
 if(!ckx(x))
  {
  if(in->lrel!=-1)
   switch(in->hmode)
    {
    case 0: rel->lofst=in->lrel+in->wid+in->hofst; rel->rofst= -1; break;
    case 1: rel->lofst=in->hofst; rel->rofst= -1; break;
    case 2: rel->lofst=in->lrel+in->hofst; rel->rofst= -1; in->hofst=0; break;
    case 3: rel->lofst=in->lrel-gtw(x)-in->hofst; rel->rofst= -1; break;
    case 4: rel->rofst=in->hofst; rel->lofst= -1; break;
    case 5: rel->lofst=in->lrel+in->wid-gtw(x)-in->hofst; rel->rofst= -1; in->hofst=0; break;
    case 6: rel->lofst=rel->rofst= -1; break;
    }
  else
   switch(in->hmode)
    {
    case 0: if(in->rrel!=-1)
              { rel->rofst=in->rrel-gtw(x)-in->hofst; rel->lofst= -1; break; }
            else
              { rel->lofst=0; rel->rofst= -1; break; }
    case 1: rel->lofst=in->hofst; rel->rofst= -1; break;
    case 2: rel->rofst=in->rrel+in->wid-gtw(x)-in->hofst; rel->lofst= -1; in->hofst=0; break;
    case 3: if(in->rrel!=-1)
              { rel->rofst=in->rrel+in->wid+in->hofst; rel->lofst= -1; break; }
            else
              { rel->rofst=0; rel->lofst= -1; break; }
    case 4: rel->rofst=in->hofst; rel->lofst= -1; break;
    case 5: rel->rofst=in->rrel+in->hofst; rel->lofst= -1; in->hofst=0; break;
    case 6: rel->lofst=rel->rofst= -1; break;
    }
  if(in->alrel!=-1)
   switch(in->ahmode)
    {
    case 0: rel->lofst=in->alrel+in->awid+in->ahofst; break;
    case 1: rel->lofst=in->ahofst; break;
    case 2: rel->lofst=in->alrel+in->ahofst; break;
    case 3: rel->lofst=in->alrel-gtw(x)-in->ahofst; break;
    case 4: rel->rofst=in->ahofst; break;
    case 5: rel->lofst=in->alrel+in->awid-gtw(x)-in->ahofst; break;
    }
  else
   switch(in->ahmode)
    {
    case 0: if(in->arrel!=-1)
              { rel->rofst=in->arrel-gtw(x)-in->ahofst; break; }
            else
              { rel->lofst=0; break; }
    case 1: rel->lofst=in->ahofst; break;
    case 2: rel->rofst=in->arrel+in->awid-gtw(x)-in->ahofst; break;
    case 3: if(in->arrel!=-1)
              { rel->rofst=in->arrel+in->awid+in->ahofst; break; }
            else
              { rel->rofst=0; break; }
    case 4: rel->rofst=in->ahofst; break;
    case 5: rel->rofst=in->arrel+in->ahofst; break;
    }
  }
 else
  {
  rel->lofst=gtx(x);
  rel->rofst= -1;
  }
 in->lrel=rel->lofst;
 in->rrel=rel->rofst;
 in->wid=gtw(x);
 if(ckw(in->in)) dox(x);

 if(!cky(x))
  {
  if(in->trel!=-1)
   switch(in->vmode)
    {
    case 0: rel->tofst=in->trel+in->hgt+in->vofst; rel->bofst= -1; break;
    case 1: rel->tofst=in->vofst; rel->bofst= -1; break;
    case 2: rel->tofst=in->trel+in->vofst; rel->bofst= -1; in->vofst=0; break;
    case 3: rel->tofst=in->trel-gth(x)-in->vofst; rel->bofst= -1; break;
    case 4: rel->bofst=in->vofst; rel->tofst= -1; break;
    case 5: rel->tofst=in->trel+in->hgt-gth(x)-in->vofst; rel->bofst= -1; in->vofst=0; break;
    case 6: rel->tofst=rel->bofst= -1; break;
    }
  else
   switch(in->vmode)
    {
    case 0: if(in->brel!=-1)
              { rel->bofst=in->brel-gth(x)-in->vofst; rel->tofst= -1; break; }
            else
              { rel->tofst=0; rel->bofst= -1; break; }
    case 1: rel->tofst=in->vofst; rel->bofst= -1; break;
    case 2: rel->bofst=in->brel+in->hgt-gth(x)-in->vofst; rel->tofst= -1; in->vofst=0; break;
    case 3: if(in->brel!=-1)
              { rel->bofst=in->brel+in->hgt+in->vofst; rel->tofst= -1; break; }
            else
              { rel->bofst=0; rel->tofst= -1; break; }
    case 4: rel->bofst=in->vofst; rel->tofst= -1; break;
    case 5: rel->bofst=in->brel+in->vofst; rel->tofst= -1; in->vofst=0; break;
    case 6: rel->tofst=rel->bofst= -1; break;
    }
  if(in->atrel!=-1)
   switch(in->avmode)
    {
    case 0: rel->tofst=in->atrel+in->ahgt+in->avofst; break;
    case 1: rel->tofst=in->avofst; break;
    case 2: rel->tofst=in->atrel+in->avofst; break;
    case 3: rel->tofst=in->atrel-gth(x)-in->avofst; break;
    case 4: rel->bofst=in->avofst; break;
    case 5: rel->tofst=in->atrel+in->ahgt-gth(x)-in->avofst; break;
    }
  else
   switch(in->avmode)
    {
    case 0: if(in->abrel!=-1)
              { rel->bofst=in->abrel-gth(x)-in->avofst; break; }
            else
              { rel->tofst=0; break; }
    case 1: rel->tofst=in->avofst; break;
    case 2: rel->bofst=in->abrel+in->ahgt-gth(x)-in->avofst; break;
    case 3: if(in->abrel!=-1)
              { rel->bofst=in->abrel+in->ahgt+in->avofst; break; }
            else
              { rel->bofst=0; break; }
    case 4: rel->bofst=in->avofst; break;
    case 5: rel->bofst=in->abrel+in->avofst; break;
    }
  }
 else
  {
  rel->tofst=gty(x);
  rel->bofst= -1;
  }
 in->trel=rel->tofst;
 in->brel=rel->bofst;
 in->hgt=gth(x);
 if(ckh(in->in)) doy(x);
 }

static void pmgrrmv(in,x)
Pmgr *in;
Widget *x;
 {
 struct relpos *rel;
 if(!XFindContext(dsp,(int)x,rels,(XPointer *)&rel))
  {
  XDeleteContext(dsp,(int)x,rels);
  free(rel);
  }
 }

static void pmgrrm(x)
Pmgr *x;
 {
 free(x);
 }

static void pmgrsttarget(x,w)
Pmgr *x;
Lith *w;
 {
 x->in=w;
 }

struct pmgrfuncs pmgrfuncs=
 {
 pmgrrm, pmgrcalcx, pmgrcalcy, pmgrcalcw, pmgrcalch, pmgradd, pmgrrmv,
 pmgrsttarget
 };

Pmgr *mkPmgr(x)
Pmgr *x;
 {
 x->funcs= &pmgrfuncs;
 if(!rels) rels=XUniqueContext();
 x->lrel= -1; x->rrel= -1; x->trel= -1; x->brel= -1;
 x->wid=0; x->hgt=0;
 x->hmode=0; x->vmode=1;
 x->hofst=0; x->vofst=0;
 x->ahmode=6; x->avmode=6;
 return x;
 }

void ltor(w,rel,ofst)
Widget *w, *rel;
 {
 Pmgr *p=w->pmgr;
 struct relpos *pos;
 if(rel) XFindContext(dsp,(int)rel,rels,(XPointer *)&pos);
 p->hmode=0; p->ahmode=6;
 p->hofst=ofst;
 if(rel) p->lrel=pos->lofst, p->rrel=pos->rofst, p->wid=gtw(rel);
 else p->lrel=p->rrel= -1;
 }

void ledge(w,ofst)
Widget *w;
 {
 Pmgr *p=w->pmgr;
 p->hmode=1; p->ahmode=6;
 p->hofst=ofst;
 p->lrel=p->rrel= -1;
 }

void lsame(w,rel,ofst)
Widget *w, *rel;
 {
 Pmgr *p=w->pmgr;
 struct relpos *pos;
 if(rel) XFindContext(dsp,(int)rel,rels,(XPointer *)&pos);
 p->hmode=2; p->ahmode=6;
 p->hofst=ofst;
 if(rel) p->lrel=pos->lofst, p->rrel=pos->rofst, p->wid=gtw(rel);
 else p->lrel=p->rrel= -1;
 }

void rtol(w,rel,ofst)
Widget *w, *rel;
 {
 Pmgr *p=w->pmgr;
 struct relpos *pos;
 if(rel) XFindContext(dsp,(int)rel,rels,(XPointer *)&pos);
 p->hmode=3; p->ahmode=6;
 p->hofst=ofst;
 if(rel) p->lrel=pos->lofst, p->rrel=pos->rofst, p->wid=gtw(rel);
 else p->lrel=p->rrel= -1;
 }

void redge(w,ofst)
Widget *w;
 {
 Pmgr *p=w->pmgr;
 p->hmode=4; p->ahmode=6;
 p->hofst=ofst;
 p->lrel=p->rrel= -1;
 }

void rsame(w,rel,ofst)
Widget *w, *rel;
 {
 Pmgr *p=w->pmgr;
 struct relpos *pos;
 if(rel) XFindContext(dsp,(int)rel,rels,(XPointer *)&pos);
 p->hmode=5; p->ahmode=6;
 p->hofst=ofst;
 if(rel) p->lrel=pos->lofst, p->rrel=pos->rofst, p->wid=gtw(rel);
 else p->lrel=p->rrel= -1;
 }

void hcenter(w)
Widget *w;
 {
 Pmgr *p=w->pmgr;
 p->hmode=6; p->ahmode=6;
 p->hofst=0;
 p->lrel=p->rrel= -1;
 }

void ttob(w,rel,ofst)
Widget *w, *rel;
 {
 Pmgr *p=w->pmgr;
 struct relpos *pos;
 if(rel) XFindContext(dsp,(int)rel,rels,(XPointer *)&pos);
 p->vmode=0; p->avmode=6;
 p->vofst=ofst;
 if(rel) p->trel=pos->tofst, p->brel=pos->bofst, p->hgt=gth(rel);
 else p->trel=p->brel= -1;
 }

void tedge(w,ofst)
Widget *w;
 {
 Pmgr *p=w->pmgr;
 p->vmode=1; p->avmode=6;
 p->vofst=ofst;
 p->trel=p->brel= -1;
 }

void tsame(w,rel,ofst)
Widget *w, *rel;
 {
 Pmgr *p=w->pmgr;
 struct relpos *pos;
 if(rel) XFindContext(dsp,(int)rel,rels,(XPointer *)&pos);
 p->vmode=2; p->avmode=6;
 p->vofst=ofst;
 if(rel) p->trel=pos->tofst, p->brel=pos->bofst, p->hgt=gth(rel);
 else p->trel=p->brel= -1;
 }

void btot(w,rel,ofst)
Widget *w, *rel;
 {
 Pmgr *p=w->pmgr;
 struct relpos *pos;
 if(rel) XFindContext(dsp,(int)rel,rels,(XPointer *)&pos);
 p->vmode=3; p->avmode=6;
 p->vofst=ofst;
 if(rel) p->trel=pos->tofst, p->brel=pos->bofst, p->hgt=gth(rel);
 else p->trel=p->brel= -1;
 }

void bedge(w,ofst)
Widget *w;
 {
 Pmgr *p=w->pmgr;
 p->vmode=4; p->avmode=6;
 p->vofst=ofst;
 p->trel=p->brel= -1;
 }

void bsame(w,rel,ofst)
Widget *w, *rel;
 {
 Pmgr *p=w->pmgr;
 struct relpos *pos;
 if(rel) XFindContext(dsp,(int)rel,rels,(XPointer *)&pos);
 p->vmode=5; p->avmode=6;
 p->vofst=ofst;
 if(rel) p->trel=pos->tofst, p->brel=pos->bofst, p->hgt=gth(rel);
 else p->trel=p->brel= -1;
 }

void vcenter(w)
Widget *w;
 {
 Pmgr *p=w->pmgr;
 p->vmode=6; p->avmode=6;
 p->vofst=0;
 p->trel=p->brel= -1;
 }

void auxltor(w,rel,ofst)
Widget *w, *rel;
 {
 Pmgr *p=w->pmgr;
 struct relpos *pos;
 if(rel) XFindContext(dsp,(int)rel,rels,(XPointer *)&pos);
 p->ahmode=0;
 p->ahofst=ofst;
 if(rel) p->alrel=pos->lofst, p->arrel=pos->rofst, p->awid=gtw(rel);
 else p->alrel=p->arrel= -1;
 }

void auxledge(w,ofst)
Widget *w;
 {
 Pmgr *p=w->pmgr;
 p->ahmode=1;
 p->ahofst=ofst;
 p->alrel=p->arrel= -1;
 }

void auxlsame(w,rel,ofst)
Widget *w, *rel;
 {
 Pmgr *p=w->pmgr;
 struct relpos *pos;
 if(rel) XFindContext(dsp,(int)rel,rels,(XPointer *)&pos);
 p->ahmode=2;
 p->ahofst=ofst;
 if(rel) p->alrel=pos->lofst, p->arrel=pos->rofst, p->awid=gtw(rel);
 else p->alrel=p->arrel= -1;
 }

void auxrtol(w,rel,ofst)
Widget *w, *rel;
 {
 Pmgr *p=w->pmgr;
 struct relpos *pos;
 if(rel) XFindContext(dsp,(int)rel,rels,(XPointer *)&pos);
 p->ahmode=3;
 p->ahofst=ofst;
 if(rel) p->alrel=pos->lofst, p->arrel=pos->rofst, p->awid=gtw(rel);
 else p->alrel=p->arrel= -1;
 }

void auxredge(w,ofst)
Widget *w;
 {
 Pmgr *p=w->pmgr;
 p->ahmode=4;
 p->ahofst=ofst;
 p->alrel=p->arrel= -1;
 }

void auxrsame(w,rel,ofst)
Widget *w, *rel;
 {
 Pmgr *p=w->pmgr;
 struct relpos *pos;
 if(rel) XFindContext(dsp,(int)rel,rels,(XPointer *)&pos);
 p->ahmode=5;
 p->ahofst=ofst;
 if(rel) p->alrel=pos->lofst, p->arrel=pos->rofst, p->awid=gtw(rel);
 else p->alrel=p->arrel= -1;
 }

void auxttob(w,rel,ofst)
Widget *w, *rel;
 {
 Pmgr *p=w->pmgr;
 struct relpos *pos;
 if(rel) XFindContext(dsp,(int)rel,rels,(XPointer *)&pos);
 p->avmode=0;
 p->avofst=ofst;
 if(rel) p->atrel=pos->tofst, p->abrel=pos->bofst, p->ahgt=gth(rel);
 else p->atrel=p->abrel= -1;
 }

void auxtedge(w,ofst)
Widget *w;
 {
 Pmgr *p=w->pmgr;
 p->avmode=1;
 p->avofst=ofst;
 p->atrel=p->abrel= -1;
 }

void auxtsame(w,rel,ofst)
Widget *w, *rel;
 {
 Pmgr *p=w->pmgr;
 struct relpos *pos;
 if(rel) XFindContext(dsp,(int)rel,rels,(XPointer *)&pos);
 p->avmode=2;
 p->avofst=ofst;
 if(rel) p->atrel=pos->tofst, p->abrel=pos->bofst, p->ahgt=gth(rel);
 else p->atrel=p->abrel= -1;
 }

void auxbtot(w,rel,ofst)
Widget *w, *rel;
 {
 Pmgr *p=w->pmgr;
 struct relpos *pos;
 if(rel) XFindContext(dsp,(int)rel,rels,(XPointer *)&pos);
 p->avmode=3;
 p->avofst=ofst;
 if(rel) p->atrel=pos->tofst, p->abrel=pos->bofst, p->ahgt=gth(rel);
 else p->atrel=p->abrel= -1;
 }

void auxbedge(w,ofst)
Widget *w;
 {
 Pmgr *p=w->pmgr;
 p->avmode=4;
 p->avofst=ofst;
 p->atrel=p->abrel= -1;
 }

void auxbsame(w,rel,ofst)
Widget *w, *rel;
 {
 Pmgr *p=w->pmgr;
 struct relpos *pos;
 if(rel) XFindContext(dsp,(int)rel,rels,(XPointer *)&pos);
 p->avmode=5;
 p->avofst=ofst;
 if(rel) p->atrel=pos->tofst, p->abrel=pos->bofst, p->ahgt=gth(rel);
 else p->atrel=p->abrel= -1;
 }
