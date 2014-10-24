// Placement manager 

#include "widget.h"

/* hmode values: 0=ltor, 1=ledge, 2=lsame, 3=rtol, 4=redge, 5=rsame, 6=center */
/* vmode values: 0=ttob, 1=tedge, 2=tsame, 3=btot, 4=bedge, 5=bsame, 6=center */

void Pmgr::calcw(Widget *self)
  {
  int max= -1;
  for(Widget *w=self->kids.next;w;w=w->link.next)
    {
    int x=w->gtw();
    if(w->lofst!=-1) x+=w->lofst;
    if(w->rofst!=-1) x+=w->rofst;
    if(x>max) max=x;
    }
  if(max==-1) max=16;
  self->stwidth(max);
  }

void Pmgr::calch(Widget *self)
  {
  int max= -1;
  for(Widget *w=self->kids.next;w;w=w->link.next)
   {
   int y=w->gth();
   if(w->tofst!=-1) y+=w->tofst;
   if(w->bofst!=-1) y+=w->bofst;
   if(y>max) max=y;
   }
  if(max==-1) max=16;
  self->stheight(max);
  }

void Pmgr::dox(Widget *w)
  {
  if(w->lofst!=-1 && w->rofst!=-1)
    w->stw(w->in->gtwidth()-lofst-rofst);
  if(w->lofst!=-1) w->stx(w->lofst);
  else if(w->rofst!=-1) w->stx(w->in->gtwidth()-w->rofst-w->gtw());
  else w->stx(w->in->gtwidth()/2-w->gtw()/2);
  }

void Pmgr::calcx(Widget *self)
  {
  for(Widget *w=self->kids.next;w;w=w->link.next)
    dox(w);
  }

void Pmgr::doy(Widget *w)
  {
  if(w->tofst!=-1 && w->bofst!=-1)
    w->sth(w->in->gtheight()-w->tofst-w->bofst);
  if(w->tofst!=-1) w->sty(w->tofst);
  else if(w->bofst!=-1) w->sty(w->in->gtheight()-w->bofst-w->gth());
  else w->sty(w->in->gtheight()/2-w->gth()/2);
  }

void Pmgr::calcy(Widget *self)
  {
  for(Widget *w=self->link.next;w;w=w->link.next)
    doy(w);
  }

void Pmgr::pmgr_add(Widget *self,Widget *w)
  {
  w->lofst=w->rofst=w->tofst=w->bofst= -1;

  if(!w->ckx())
    {
    if(lrel!=-1)
      switch(hmode)
        {
        case 0:
          w->lofst=lrel+wid+hofst; w->rofst= -1; break;
        case 1:
          w->lofst=hofst; w->rofst= -1; break;
        case 2:
          w->lofst=lrel+hofst; w->rofst= -1; hofst=0; break;
        case 3:
          w->lofst=lrel-w->gtw()-hofst; w->rofst= -1; break;
        case 4:
          w->rofst=hofst; w->lofst= -1; break;
        case 5:
          w->lofst=lrel+wid-w->gtw()-hofst; w->rofst= -1; hofst=0; break;
        case 6:
          w->lofst=w->rofst= -1; break;
        }
    else
      switch(hmode)
        {
        case 0:
          if(rrel!=-1)
            { w->rofst=rrel-w->gtw()-hofst; w->lofst= -1; break; }
          else
            { w->lofst=0; w->rofst= -1; break; }
        case 1:
          w->lofst=hofst; w->rofst= -1; break;
        case 2:
          w->rofst=rrel+wid-w->gtw()-hofst; w->lofst= -1; hofst=0; break;
        case 3:
          if(rrel!=-1)
            { w->rofst=rrel+wid+hofst; w->lofst= -1; break; }
          else
            { w->rofst=0; w->lofst= -1; break; }
        case 4:
          w->rofst=hofst; w->lofst= -1; break;
        case 5:
          w->rofst=rrel+hofst; w->lofst= -1; hofst=0; break;
        case 6:
          w->lofst=w->rofst= -1; break;
        }
    if(alrel!=-1)
      switch(ahmode)
        {
        case 0:
          w->lofst=alrel+awid+ahofst; break;
        case 1:
          w->lofst=ahofst; break;
        case 2:
          w->lofst=alrel+ahofst; break;
        case 3:
          w->lofst=alrel-w->gtw()-ahofst; break;
        case 4:
          w->rofst=ahofst; break;
        case 5:
          w->lofst=alrel+awid-w->gtw()-ahofst; break;
        }
    else
      switch(ahmode)
        {
        case 0:
          if(arrel!=-1)
            { w->rofst=arrel-w->gtw()-ahofst; break; }
          else
            { w->lofst=0; break; }
        case 1:
          w->lofst=ahofst; break;
        case 2:
          w->rofst=arrel+awid-w->gtw()-ahofst; break;
        case 3:
          if(arrel!=-1)
            { w->rofst=arrel+awid+ahofst; break; }
          else
            { w->rofst=0; break; }
        case 4:
          w->rofst=ahofst; break;
        case 5:
          w->rofst=arrel+ahofst; break;
        }
    }
  else
    {
    w->lofst=w->gtx();
    w->rofst= -1;
    }
  lrel=w->lofst;
  rrel=w->rofst;
  wid=w->gtw();
  if(self->ckw()) dox(w);

  if(!w->cky())
    {
    if(trel!=-1)
      switch(vmode)
        {
        case 0:
          w->tofst=trel+hgt+vofst; w->bofst= -1; break;
        case 1:
          w->tofst=vofst; w->bofst= -1; break;
        case 2:
          w->tofst=trel+vofst; w->bofst= -1; vofst=0; break;
        case 3:
          w->tofst=trel-w->gth()-vofst; w->bofst= -1; break;
        case 4:
          w->bofst=vofst; w->tofst= -1; break;
        case 5:
          w->tofst=trel+hgt-w->gth()-vofst; w->bofst= -1; vofst=0; break;
        case 6:
          w->tofst=w->bofst= -1; break;
        }
    else
      switch(vmode)
        {
        case 0:
          if(brel!=-1)
            { w->bofst=brel-w->gth()-vofst; w->tofst= -1; break; }
          else
            { w->tofst=0; w->bofst= -1; break; }
        case 1:
          w->tofst=vofst; w->bofst= -1; break;
        case 2:
          w->bofst=brel+hgt-w->gth()-vofst; w->tofst= -1; vofst=0; break;
        case 3:
          if(brel!=-1)
            { w->bofst=brel+hgt+vofst; w->tofst= -1; break; }
          else
            { w->bofst=0; w->tofst= -1; break; }
        case 4:
          w->bofst=vofst; w->tofst= -1; break;
        case 5:
          w->bofst=brel+vofst; w->tofst= -1; vofst=0; break;
        case 6:
          w->tofst=w->bofst= -1; break;
        }
    if(atrel!=-1)
      switch(avmode)
        {
        case 0:
          w->tofst=atrel+ahgt+avofst; break;
        case 1:
          w->tofst=avofst; break;
        case 2:
          w->tofst=atrel+avofst; break;
        case 3:
          w->tofst=atrel-w->gth()-avofst; break;
        case 4:
          w->bofst=avofst; break;
        case 5:
          w->tofst=atrel+ahgt-w->gth()-avofst; break;
        }
    else
      switch(avmode)
        {
        case 0:
          if(abrel!=-1)
            { w->bofst=abrel-w->gth()-avofst; break; }
          else
            { w->tofst=0; break; }
        case 1:
          w->tofst=avofst; break;
        case 2:
          w->bofst=abrel+ahgt-w->gth()-avofst; break;
        case 3:
          if(abrel!=-1)
            { w->bofst=abrel+ahgt+avofst; break; }
          else
            { w->bofst=0; break; }
        case 4:
          w->bofst=avofst; break;
        case 5:
          w->bofst=abrel+avofst; break;
        }
    }
  else
    {
    w->tofst=w->gty();
    w->bofst= -1;
    }
  trel=w->tofst;
  brel=w->bofst;
  hgt=w->gth();
  if(self->ckh()) doy(w);
  }

Pmgr::Pmgr()
  {
  lrel= -1; rrel= -1; trel= -1; brel= -1;
  wid=0; hgt=0;
  hmode=0; vmode=1;
  hofst=0; vofst=0;
  ahmode=6; avmode=6;
  }

Pmgr::~Pmgr()
  {
  }

void Pmgr::ltor(Widget *rel,int ofst)
  {
  hmode=0; ahmode=6;
  hofst=ofst;
  if(rel) lrel=rel->lofst, rrel=rel->rofst, wid=rel->gtw();
  else lrel=rrel= -1;
  }

void Pmgr::ledge(int ofst)
  {
  hmode=1; ahmode=6;
  hofst=ofst;
  lrel=rrel= -1;
  }

void Pmgr::lsame(Widget *rel,int ofst)
  {
  hmode=2; ahmode=6;
  hofst=ofst;
  if(rel) lrel=rel->lofst, rrel=rel->rofst, wid=rel->gtw();
  else lrel=rrel= -1;
  }

void Pmgr::rtol(Widget *rel,int ofst)
  {
  hmode=3; ahmode=6;
  hofst=ofst;
  if(rel) lrel=rel->lofst, rrel=rel->rofst, wid=rel->gtw();
  else lrel=rrel= -1;
  }

void Pmgr::redge(int ofst)
  {
  hmode=4; ahmode=6;
  hofst=ofst;
  lrel=rrel= -1;
  }

void Pmgr::rsame(Widget *rel,int ofst)
  {
  hmode=5; ahmode=6;
  hofst=ofst;
  if(rel) lrel=rel->lofst, rrel=rel->rofst, wid=rel->gtw();
  else lrel=rrel= -1;
  }

void Pmgr::hcenter()
  {
  hmode=6; ahmode=6;
  hofst=0;
  lrel=rrel= -1;
  }

void Pmgr::ttob(Widget *rel,int ofst)
  {
  vmode=0; avmode=6;
  vofst=ofst;
  if(rel) trel=rel->tofst, brel=rel->bofst, hgt=rel->gth();
  else trel=brel= -1;
  }

void Pmgr::tedge(int ofst)
  {
  vmode=1; avmode=6;
  vofst=ofst;
  trel=brel= -1;
  }

void Pmgr::tsame(Widget *rel,int ofst)
  {
  vmode=2; avmode=6;
  vofst=ofst;
  if(rel) trel=rel->tofst, brel=rel->bofst, hgt=rel->gth();
  else trel=brel= -1;
  }

void Pmgr::btot(Widget *rel,int ofst)
  {
  vmode=3; avmode=6;
  vofst=ofst;
  if(rel) trel=rel->tofst, brel=rel->bofst, hgt=rel->gth();
  else trel=brel= -1;
  }

void Pmgr::bedge(int ofst)
  {
  vmode=4; avmode=6;
  vofst=ofst;
  trel=brel= -1;
  }

void Pmgr::bsame(Widget *rel,int ofst)
  {
  vmode=5; avmode=6;
  vofst=ofst;
  if(rel) trel=rel->tofst, brel=rel->bofst, hgt=rel->gth();
  else trel=brel= -1;
  }

void Pmgr::vcenter()
  {
  vmode=6; avmode=6;
  vofst=0;
  trel=brel= -1;
  }

void Pmgr::auxltor(Widget *rel,int ofst)
  {
  ahmode=0;
  ahofst=ofst;
  if(rel) alrel=rel->lofst, arrel=rel->rofst, awid=rel->gtw();
  else alrel=arrel= -1;
  }

void Pmgr::auxledge(int ofst)
  {
  ahmode=1;
  ahofst=ofst;
  alrel=arrel= -1;
  }

void Pmgr::auxlsame(Widget *rel,int ofst)
  {
  ahmode=2;
  ahofst=ofst;
  if(rel) alrel=rel->lofst, arrel=rel->rofst, awid=rel->gtw();
  else alrel=arrel= -1;
  }

void Pmgr::auxrtol(Widget *rel,int ofst)
  {
  ahmode=3;
  ahofst=ofst;
  if(rel) alrel=rel->lofst, arrel=rel->rofst, awid=rel->gtw();
  else alrel=arrel= -1;
  }

void Pmgr::auxredge(int ofst)
  {
  ahmode=4;
  ahofst=ofst;
  alrel=arrel= -1;
  }

void Pmgr::auxrsame(Widget *rel,int ofst)
  {
  ahmode=5;
  ahofst=ofst;
  if(rel) alrel=rel->lofst, arrel=rel->rofst, awid=rel->gtw();
  else alrel=arrel= -1;
  }

void Pmgr::auxttob(Widget *rel,int ofst)
  {
  avmode=0;
  avofst=ofst;
  if(rel) atrel=rel->tofst, abrel=rel->bofst, ahgt=rel->gth();
  else atrel=abrel= -1;
  }

void Pmgr::auxtedge(int ofst)
  {
  avmode=1;
  avofst=ofst;
  atrel=abrel= -1;
  }

void Pmgr::auxtsame(Widget *rel,int ofst)
  {
  avmode=2;
  avofst=ofst;
  if(rel) atrel=rel->tofst, abrel=rel->bofst, ahgt=rel->gth();
  else atrel=abrel= -1;
  }

void Pmgr::auxbtot(Widget *rel,int ofst)
  {
  avmode=3;
  avofst=ofst;
  if(rel) atrel=rel->tofst, abrel=rel->bofst, ahgt=rel->gth();
  else atrel=abrel= -1;
  }

void Pmgr::auxbedge(int ofst)
  {
  avmode=4;
  avofst=ofst;
  atrel=abrel= -1;
  }

void Pmgr::auxbsame(Widget *rel,int ofst)
  {
  avmode=5;
  avofst=ofst;
  if(rel) atrel=rel->tofst, abrel=rel->bofst, ahgt=rel->gth();
  else atrel=abrel= -1;
  }
