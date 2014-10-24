#include <iostream>
#include "termcap.h"
#include "util.h"
#include "scrn.h"
#include "tty.h"

// Need ttgtssz() to be in here.

// Set attributes

void Scrn::attr(int c)
  {
  int e;
  c&=~255;
  e=(AT_MASK&cur_attr&~c);
  if(e) // If any attributes go off, switch them all off: fixes bug on PCs
    {
    if(me) exec(me,1);
    else
      {
      if(ue) exec(ue,1);
      if(se) exec(se,1);
      }
    cur_attr=0;
    }
  e=(c&~cur_attr);
  if(e&INVERSE)
    if(mr) exec(mr,1);
    else if(so) exec(so,1);
  if(e&UNDERLINE)
    if(us) exec(us,1);
  if(e&BLINK)
    if(mb) exec(mb,1);
  if(e&BOLD)
    if(md) exec(md,1);
  if(e&DIM)
    if(mh) exec(mh,1);
  if((cur_attr&FG_MASK)!=(c&FG_MASK))
    if(Sf) exec(Sf,1,7-(((c&FG_MASK)>>FG_SHIFT)));
  if((cur_attr&BG_MASK)!=(c&BG_MASK))
    if(Sb) exec(Sb,1,((c&BG_MASK)>>BG_SHIFT));
  cur_attr=c;
  }

// Set scrolling region

void Scrn::setregn(int top,int bot)
  {
  if(!cs)
    {
    cur_top=top;
    cur_bot=bot;
    return;
    }
  if(cur_top!=top || cur_bot!=bot)
    {
    cur_top=top;
    cur_bot=bot;
    exec(cs,1,top,bot-1);
    cur_x= -1; cur_y= -1;
    }
  }

// Enter insert mode

void Scrn::setins(int x)
  {
  if(cur_ins!=1 && im)
    {
    cur_ins=1;
    exec(im,1,x);
    }
  }

// Exit insert mode

void Scrn::clrins()
  {
  if(cur_ins!=0)
    {
    exec(ei,1);
    cur_ins=0;
    }
  }

// Erase from given screen coordinate to end of line

void Scrn::eraeol(int x,int y)
  {
  int *s, *ss;
  int w=co-x;			// was -1 here:Don't worry about last column
  if(w<=0) return;
  s=scrn+y*co+x;
  ss=s+w;
  do if(*--ss!=' ') { ++ss; break; } while(ss!=s);
  if((ss-s>3 || s[w]!=' ') && ce)
    {
    cpos(x,y);
    attr(0);
    exec(ce,1);
    msetI(s,' ',w);
    }
  else if(s!=ss)
    {
    if(cur_ins) clrins();
    if(cur_x!=x || cur_y!=y) cpos(x,y);
    if(cur_attr) attr(0);
    while(s!=ss)
      {
      *s=' '; out << ' '; ++cur_x; ++s;
      }
    }
  }

// Output a character with attributes

void Scrn::outatr(int x,int y,int c)
  {
  unsigned char ch;
  if(c== -1) c=' ';
  if(cur_ins) clrins();
  ch=c;
  if(cur_x!=x || cur_y!=y) cpos(x,y);
  if(c-ch!=cur_attr) attr(c-ch);
  if(haz && ch=='~') ch='\\';
  out << ch;
  scrn[co*y+x]=c;
  ++cur_x;
  }

// As above but useable in insert mode
// The cursor position must already be correct

void Scrn::outatri(int x,int y,int c)
  {
  unsigned char ch;
  if(c== -1) c=' ';
  ch=c; c-=ch;
  if(c!=cur_attr) attr(c);
  if(haz && ch=='~') ch='\\';
  out << ch;
  ++cur_x;
  }

// Calculate cost of positioning the cursor using only relative cursor
// positioning functions: (lf, DO, up, UP, bs, LE, RI, ta, bt) and rewriting
// characters (to move right)
//
// This doesn't use the am and bw capabilities although it probably could.

int Scrn::relcost(int x,int y,int ox,int oy)
  {
  int cost=0;

  // If we don't know the cursor position, force use of absolute positioning
  if(oy== -1 || ox== -1) return 10000;

  // First adjust row
  if(y>oy)
    {
    int dist=y-oy;
    // Have to go down
    if(lf)
      {
      int mult=dist*clf;
      if(dist<10 && cDO<mult) cost+=cDO;
      else if(dist>=10 && cDO+1<mult) cost+=cDO+1;
      else cost+=mult;
      }
    else if(DO)
      if(dist<10) cost+=cDO;
      else cost+=cDO+1;
    else return 10000;
    }
  else if(y<oy)
    {
    int dist=oy-y;
    // Have to go up
    if(up)
      {
      int mult=dist*cup;
      if(dist<10 && cUP<mult) cost+=cUP;
      else if(dist>=10 && cUP<mult) cost+=cUP+1;
      else cost+=mult;
      }
    else if(UP)
      if(dist<10) cost+=cUP;
      else cost+=cUP+1;
    else return 10000;
    }

  // Now adjust column

  // Use tabs
  if(x>ox && ta)
    {
    int dist=x-ox;
    int ntabs=(dist+ox%tw)/tw;
    int cstunder=x%tw+cta*ntabs, cstover;
    if(x+tw<co && bs) cstover=cbs*(tw-x%tw)+cta*(ntabs+1);
    else cstover=10000;
    if(dist<10 && cstunder<cRI && cstunder<x-ox && cstover>cstunder)
      return cost+cstunder;
    else if(cstunder<cRI+1 && cstunder<x-ox && cstover>cstunder)
      return cost+cstunder;
    else if(dist<10 && cstover<cRI && cstover<x-ox) return cost+cstover;
    else if(cstover<cRI+1 && cstover<x-ox) return cost+cstover;
    }
  else if(x<ox && bt)
    {
    int dist=ox-x; 
    int ntabs=(dist+tw-ox%tw)/tw;
    int cstunder,cstover;
    if(bs) cstunder=cbt*ntabs+cbs*(tw-x%tw); else cstunder=10000;
    if(x-tw>=0) cstover=cbt*(ntabs+1)+x%tw; else cstover=10000;
    if(dist<10 && cstunder<cLE && (bs?cstunder<(ox-x)*cbs:1) && cstover>cstunder)
      return cost+cstunder;
    if(cstunder<cLE+1 && (bs?cstunder<(ox-x)*cbs:1) && cstover>cstunder)
      return cost+cstunder;
    else if(dist<10 && cstover<cRI && (bs?cstover<(ox-x)*cbs:1)) return cost+cstover;
    else if(cstover<cRI+1 && (bs?cstover<(ox-x)*cbs:1)) return cost+cstover;
    }

  // Use simple motions
  if(x<ox)
    {
    int dist=ox-x;
    // Have to go left
    if(bs)
      {
      int mult=dist*cbs;
      if(cLE<mult && dist<10) cost+=cLE;
      else if(cLE+1<mult) cost+=cLE+1;
      else cost+=mult;
      }
    else if(LE) cost+=cLE;
    else return 10000;
    }
  else if(x>ox)
    {
    int dist=x-ox;
    // Have to go right
    // Hmm.. this should take into account possible attribute changes
    if(cRI<dist && dist<10) cost+=cRI;
    else if(cRI+1<dist) cost+=cRI+1;
    else cost+=dist;
    }

  return cost;
  }

// Find optimal set of cursor positioning commands to move from the current
// cursor row and column (either or both of which might be unknown) to the
// given new row and column and execute them.

void Scrn::cposs(int x,int y)
  {
  int bestcost,cost;
  int bestway;
  int hy;
  int hl;

  // Home y position is usually 0, but it is 'top' if we have scrolling region
  
  if(rr) hy=cur_top, hl=cur_bot-1;
  else hy=0, hl=li-1;

  // Assume best way is with only using relative cursor positioning

  bestcost=relcost(x,y,cur_x,cur_y); bestway=0;

  // Now check if combinations of absolute cursor positioning functions are
  // better (or necessary in case one or both cursor positions are unknown)
  

  if(ccm<bestcost)
    {
    cost=tcost(cm,1,y,x);
    if(cost<bestcost) bestcost=cost, bestway=6;
    }
  if(ccr<bestcost)
    {
    cost=relcost(x,y,0,cur_y)+ccr;
    if(cost<bestcost) bestcost=cost, bestway=1;
    }
  if(cho<bestcost)
    {
    cost=relcost(x,y,0,hy)+cho;
    if(cost<bestcost) bestcost=cost, bestway=2;
    }
  if(cll<bestcost)
    {
    cost=relcost(x,y,0,hl)+cll;
    if(cost<bestcost) bestcost=cost, bestway=3;
    }
  if(cch<bestcost && x!=cur_x)
    {
    cost=relcost(x,y,x,cur_y)+tcost(ch,1,x);
    if(cost<bestcost) bestcost=cost, bestway=4;
    }
  if(ccv<bestcost && y!=cur_y)
    {
    cost=relcost(x,y,cur_x,y)+tcost(cv,1,y);
    if(cost<bestcost) bestcost=cost, bestway=5;
    }
  if(ccV<bestcost)
    {
    cost=relcost(x,y,0,y)+tcost(cV,1,y);
    if(cost<bestcost) bestcost=cost, bestway=13;
    }
  if(cch+ccv<bestcost && x!=cur_x && y!=cur_y)
    {
    cost=tcost(cv,1,y-hy)+tcost(ch,1,x);
    if(cost<bestcost) bestcost=cost, bestway=7;
    }
  if(ccv+ccr<bestcost && y!=cur_y)
    {
    cost=tcost(cv,1,y)+tcost(cr,1)+
         relcost(x,y,0,y);
    if(cost<bestcost) bestcost=cost, bestway=8;
    }
  if(cll+cch<bestcost)
    {
    cost=tcost(ll,1)+tcost(ch,1,x)+
         relcost(x,y,x,hl);
    if(cost<bestcost) bestcost=cost, bestway=9;
    }
  if(cll+ccv<bestcost)
    {
    cost=tcost(ll,1)+tcost(cv,1,y)+
         relcost(x,y,0,y);
    if(cost<bestcost) bestcost=cost, bestway=10;
    }
  if(cho+cch<bestcost)
    {
    cost=tcost(ho,1)+tcost(ch,1,x)+
         relcost(x,y,x,hy);
    if(cost<bestcost) bestcost=cost, bestway=11;
    }
  if(cho+ccv<bestcost)
    {
    cost=tcost(ho,1)+tcost(cv,1,y)+
         relcost(x,y,0,y);
    if(cost<bestcost) bestcost=cost, bestway=12;
    }

  // Do absolute cursor positioning if we don't know the cursor position or
  // if it is faster than doing only relative cursor positioning

  switch(bestway)
    {
    case 1:
      exec(cr,1); cur_x=0; break;
    case 2:
      exec(ho,1); cur_x=0; cur_y=hy; break;
    case 3:
      exec(ll,1); cur_x=0; cur_y=hl; break;
    case 9:
      exec(ll,1); cur_x=0; cur_y=hl; goto doch;
    case 11:
      exec(ho,1); cur_x=0; cur_y=hy;
    doch:
    case 4:
      exec(ch,1,x); cur_x=x; break;
    case 10:
      exec(ll,1); cur_x=0; cur_y=hl; goto docv;
    case 12:
      exec(ho,1); cur_x=0; cur_y=hy; goto docv;
    case 8:
      exec(cr,1); cur_x=0;
    docv:
    case 5:
      exec(cv,1,y); cur_y=y; break;
    case 6:
      exec(cm,1,y,x); cur_y=y, cur_x=x; break;
    case 7:
      exec(cv,1,y); cur_y=y;
      exec(ch,1,x); cur_x=x;
      break;
    case 13:
      exec(cV,1,y); cur_y=y; cur_x=0; break;
    }

  // Use relative cursor position functions if we're not there yet

  // First adjust row
  if(y>cur_y)
    // Have to go down
    if(!lf || cDO<(y-cur_y)*clf)
      exec(DO,1,y-cur_y), cur_y=y;
    else while(y>cur_y) exec(lf,1), ++cur_y;
  else if(y<cur_y)
    // Have to go up
    if(!up || cUP<(cur_y-y)*cup)
      exec(UP,1,cur_y-y), cur_y=y;
    else while(y<cur_y) exec(up,1), --cur_y;

  // Use tabs
  if(x>cur_x && ta)
    {
    int ntabs=(x-cur_x+cur_x%tw)/tw;
    int cstunder=x%tw+cta*ntabs, cstover;
    if(x+tw<co && bs) cstover=cbs*(tw-x%tw)+cta*(ntabs+1);
    else cstover=10000;
    if(cstunder<cRI && cstunder<x-cur_x && cstover>cstunder)
      {
      if(ntabs)
        {
        cur_x=x-x%tw;
        do exec(ta,1); while(--ntabs);
        }
      }
    else if(cstover<cRI && cstover<x-cur_x)
      {
      cur_x=tw+x-x%tw;
      ++ntabs;
      do exec(ta,1); while(--ntabs);
      }
    }
  else if(x<cur_x && bt)
    {
    int ntabs=((cur_x+tw-1)-(cur_x+tw-1)%tw-
               ((x+tw-1)-(x+tw-1)%tw))/tw;
    int cstunder,cstover;
    if(bs) cstunder=cbt*ntabs+cbs*(tw-x%tw); else cstunder=10000;
    if(x-tw>=0) cstover=cbt*(ntabs+1)+x%tw; else cstover=10000;
    if(cstunder<cLE && (bs?cstunder<(cur_x-x)*cbs:1) && cstover>cstunder)
      {
      if(ntabs)
        {
        do exec(bt,1); while(--ntabs);
        cur_x=x+tw-x%tw;
        }
      }
    else if(cstover<cRI && (bs?cstover<(cur_x-x)*cbs:1))
      {
      cur_x=x-x%tw; ++ntabs;
      do exec(bt,1); while(--ntabs);
      }
    }

  // Now adjust column
  if(x<cur_x)
    // Have to go left
    if(!bs || cLE<(cur_x-x)*cbs)
      exec(LE,1,cur_x-x), cur_x=x;
    else while(x<cur_x) exec(bs,1), --cur_x;
  else if(x>cur_x)
    // Have to go right
    // Hmm.. this should take into account possible attribute changes
    if(cRI<x-cur_x) exec(RI,1,x-cur_x), cur_x=x;
    else
      {
      int *s=scrn+cur_x+cur_y*co;
      if(cur_ins) clrins();
      while(x>cur_x)
        {
        int a, c;
        if(*s==-1) c=' ', a=0;
        else c=(0xFF&*s), a=(~0xFF&*s);
        if(a!=cur_attr) attr(a);
        out << (char)c;
        ++s; ++cur_x;
        }
      }
  }

int Scrn::cpos(int x,int y)
  {
  if(y==cur_y)
   {
   if(x==cur_x) return 0;
   if(x>cur_x && x-cur_x<4 && !cur_ins)
     {
     int *cs=scrn+cur_x+co*cur_y;
     if(cur_ins) clrins();
     do
       {
       int c, a;
       if(*cs==-1) c=' ', a=0;
       else c=(0xFF& *cs), a=(~0xFF& *cs);
       if(a!=cur_attr) attr(a);
       out << (char)c;
       ++cs; ++cur_x;
       } while(x!=cur_x);
     return 0;
     }
   }
  if((!ms && cur_attr&(INVERSE|UNDERLINE|BG_MASK)) || 
     (ut && cur_attr&BG_MASK))
    attr(cur_attr&~(INVERSE|UNDERLINE|BG_MASK));
  if(y<cur_top || y>=cur_bot) setregn(0,li);
  cposs(x,y);
  return 0;
  }

void Scrn::doinschr(int x,int y,int *s,int n)
  {
  int a;
  if(x<0) s-=x, x=0;
  if(x>=co-1 || n<=0) return;
  if(im || ic || IC)
    {
    cpos(x,y);
    if(n==1 && ic || !IC)
      {
      if(!ic) setins(x);
      for(a=0;a!=n;++a)
        {
        exec(ic,1,x);
        outatri(x+a,y,s[a]);
        exec(ip,1,x);
        }
      if(!mi) clrins();
      }
    else
      {
      exec(IC,1,n);
      for(a=0;a!=n;++a) outatri(x+a,y,s[a]);
      }
    }
  mcpy(scrn+x+co*y+n,scrn+x+co*y,(co-(x+n))*sizeof(int));
  mcpy(scrn+x+co*y,s,n*sizeof(int));
  }

void Scrn::dodelchr(int x,int y,int n)
  {
  int a;
  if(x<0) x=0;
  if(!n || x>=co-1) return;
  if(dc || DC)
    {
    cpos(x,y);
    exec(dm,1,x);		// Enter delete mode
    if(n==1 && dc || !DC)
      for(a=n;a;--a) exec(dc,1,x);
    else exec(DC,1,n);
    exec(ed,1,x);		// Exit delete mode
    }
  mcpy(scrn+co*y+x,scrn+co*y+x+n,(co-(x+n))*sizeof(int));
  msetI(scrn+co*y+co-n,' ',n);
  }

// Insert/Delete within line

void Scrn::magic(int y,int *cs,int *s,int placex)
  {
  struct hentry *htab=magic_htab;
  int *ofst=magic_ofst;
  int aryx=1;
  int x;
  if(!(im || ic || IC) ||
     !(dc || DC)) return;
  mset((char *)htab,0,256*sizeof(struct hentry));
  msetI(ofst,0,co);

  // Build hash table
  for(x=0;x!=co-1;++x)
    magic_ary[aryx].next=htab[cs[x]&255].next,
    magic_ary[aryx].loc=x,
    ++htab[cs[x]&255].loc,
    htab[cs[x]&255].next=aryx++;

  // Build offset table
  for(x=0;x<co-1;)
   if(htab[s[x]&255].loc>=15) ofst[x++]= co-1;
   else
     {
     int aryy;
     int maxaryy;
     int maxlen=0;
     int best=0;
     int bestback=0;
     int z;
     for(aryy=htab[s[x]&255].next;aryy;aryy=magic_ary[aryy].next)
       {
       int amnt,back;
       int tsfo=magic_ary[aryy].loc-x;
       int cst= -Iabs(tsfo);
       int pre=32;
       for(amnt=0;x+amnt<co-1 && x+tsfo+amnt<co-1;++amnt)
         {
         if(cs[x+tsfo+amnt]!=s[x+amnt]) break;
         else if(s[x+amnt]&255!=32 || pre!=32) ++cst;
         pre=s[x+amnt]&255;
         }
       pre=32;
       for(back=0;back+x>0 && back+tsfo+x>0;--back)
         {
         if(cs[x+tsfo+back-1]!=s[x+back-1]) break;
         else if(s[x+back-1]&255!=32 || pre!=32) ++cst;
         pre=s[x+back-1]&255;
         }
       if(cst>best) maxaryy=aryy, maxlen=amnt, best=cst, bestback=back;
       }
     if(!maxlen) ofst[x]=co-1, maxlen=1;
     else if(best<2) for(z=0;z!=maxlen;++z) ofst[x+z]=co-1;
     else for(z=0;z!=maxlen-bestback;++z)
       ofst[x+z+bestback]=magic_ary[maxaryy].loc-x;
     x+=maxlen;
     }

  // Apply scrolling commands

  for(x=0;x!=co-1;++x)
    {
    int q=ofst[x];
    if(q && q!=co-1)
      if(q>0)
        {
        int z,fu;
        for(z=x;z!=co-1 && ofst[z]==q;++z);
        while(s[x]==cs[x] && x<placex) ++x;
        dodelchr(x,y,q);
        for(fu=x;fu!=co-1;++fu) if(ofst[fu]!=co-1) ofst[fu]-=q;
        x=z-1;
        }
      else
        {
        int z,fu;
        for(z=x;z!=co-1 && ofst[z]==q;++z);
        while(s[x+q]==cs[x+q] && x-q<placex) ++x;
        doinschr(x+q,y,s+x+q,-q);
        for(fu=x;fu!=co-1;++fu) if(ofst[fu]!=co-1) ofst[fu]-=q;
        x=z-1;
        }
    }
  }

void Scrn::doupscrl(int top,int bot,int amnt)
  {
  int a=amnt;
  if(!amnt) return;
  attr(0);
  if(top==0 && bot==li && (sf || SF))
    {
    setregn(0,li);
    cpos(0,li-1);
    if(amnt==1 && sf || !SF) while(a--) exec(sf,1,li-1);
    else exec(SF,a,a);
    goto done;
    }
  if(bot==li && (dl || DL))
    {
    setregn(0,li);
    cpos(0,top);
    if(amnt==1 && dl || !DL) while(a--) exec(dl,1,top);
    else exec(DL,a,a);
    goto done;
    }
  if(cs && ( sf || SF ))
    {
    setregn(top,bot);
    cpos(0,bot-1);
    if(amnt==1 && sf || !SF) while(a--) exec(sf,1,bot-1);
    else exec(SF,a,a);
    goto done;
    }
  if((dl || DL) && (al || AL))
    {
    cpos(0,top);
    if(amnt==1 && dl || !DL) while(a--) exec(dl,1,top);
    else exec(DL,a,a);
    a=amnt;
    cpos(0,bot-amnt);
    if(amnt==1 && al || !AL) while(a--) exec(al,1,bot-amnt);
    else exec(AL,a,a);
    goto done;
    }
  msetI(updtab+top,1,bot-top);
  return;

  done:
  mcpy(scrn+top*co,scrn+(top+amnt)*co,
       (bot-top-amnt)*co*sizeof(int));
  if(bot==li && db)
    {
    msetI(scrn+(li-amnt)*co,-1,amnt*co);
    msetI(updtab+li-amnt,1,amnt);
    }
  else msetI(scrn+(bot-amnt)*co,' ',amnt*co);
  }

void Scrn::dodnscrl(int top,int bot,int amnt)
  {
  int a=amnt;
  if(!amnt) return;
  attr(0);
  if(top==0 && bot==li && (sr || SR))
    {
    setregn(0,li);
    cpos(0,0);
    if(amnt==1 && sr || !SR)
      while(a--) exec(sr,1,0);
    else exec(SR,a,a);
    goto done;
    }
  if(bot==li && (al || AL))
    {
    setregn(0,li);
    cpos(0,top);
    if(amnt==1 && al || !AL)
      while(a--) exec(al,1,top);
    else exec(AL,a,a);
    goto done;
    }
  if(cs && (sr || SR))
    {
    setregn(top,bot);
    cpos(0,top);
    if(amnt==1 && sr || !SR)
      while(a--) exec(sr,1,top);
    else exec(SR,a,a);
    goto done;
    }
  if((dl || DL) && (al || AL))
    {
    cpos(0,bot-amnt);
    if(amnt==1 && dl || !DL)
      while(a--) exec(dl,1,bot-amnt);
    else exec(DL,a,a);
    a=amnt;
    cpos(0,top);
    if(amnt==1 && al || !AL)
      while(a--) exec(al,1,top);
    else exec(AL,a,a);
    goto done;
    }
  msetI(updtab+top,1,bot-top);
  return;
  done:
  mcpy(scrn+(top+amnt)*co,scrn+top*co,
        (bot-top-amnt)*co*sizeof(int));
  if(!top && da)
    {
    msetI(scrn,-1,amnt*co);
    msetI(updtab,1,amnt);
    }
  else msetI(scrn+co*top,' ',amnt*co);
  }

void Scrn::scroll()
  {
  int y,z,q,r,p;
  for(y=0;y!=li;++y)
    {
    q=sary[y];
    // if(have) return;
    if(q && q!=li)
      if(q>0)
        {
        for(z=y;z!=li && sary[z]==q;++z) sary[z]=0;
        doupscrl(y,z+q,q), y=z-1;
        }
      else
        {
        for(r=y;r!=li && (sary[r]<0 || sary[r]==li);++r);
        p=r-1; do
          {
          q=sary[p];
          if(q && q!=li)
            {
            for(z=p;sary[z]=0, (z && sary[z-1]==q);--z);
            dodnscrl(z+q,p+1,-q);
            p=z+1;
            }
          }
          while(p--!=y);
        y=r-1;
        }
    }
  msetI(sary,0,li);
  }

void Scrn::scrldn(int top,int bot,int amnt)
  {
  int x;
  if(!amnt || top>=bot || bot>li) return;
  if(amnt<bot-top && bot-top-amnt<amnt/2 || !scroll_flg) amnt=bot-top;
  if(amnt<bot-top)
    {
    for(x=bot;x!=top+amnt;--x)
      sary[x-1]=(sary[x-amnt-1]==li?li:sary[x-amnt-1]-amnt),
      updtab[x-1]=updtab[x-amnt-1];
    for(x=top;x!=top+amnt;++x) updtab[x]=1;
    }
  if(amnt>bot-top) amnt=bot-top;
  msetI(sary+top,li,amnt);
  if(amnt==bot-top) msetI(updtab+top,1,amnt);
  }

void Scrn::scrlup(int top,int bot,int amnt)
  {
  int x;
  if(!amnt || top>=bot || bot>li) return;
  if(amnt<bot-top && bot-top-amnt<amnt/2 || !scroll_flg) amnt=bot-top;
  if(amnt<bot-top)
    {
    for(x=top+amnt;x!=bot;++x)
      sary[x-amnt]=(sary[x]==li?li:sary[x]+amnt),
      updtab[x-amnt]=updtab[x];
    for(x=bot-amnt;x!=bot;++x) updtab[x]=1;
    }
  if(amnt>bot-top) amnt=bot-top;
  msetI(sary+bot-amnt,li,amnt);
  if(amnt==bot-top) msetI(updtab+bot-amnt,1,amnt);
  }

void Scrn::partial()
  {
  attr(0);
  clrins();
  setregn(0,li);
  }

// Suspend screen editing and set up terminal for shell prompt

void Scrn::suspend()
  {
  partial();
  cpos(0,li-1);
  eraeol(0,li-1);
  if(te) exec(te,1);
  }

// Close screen

void Scrn::close()
  {
  partial();
  cpos(0,li-1);
  if(te) exec(te,1);
  }

// Resume screen editing after a close() or suspend().

void Scrn::resume()
  {
  if(ti) exec(ti,1);
  redraw();
  }

// Invalidate everything so that everything is redrawn

void Scrn::redraw()
  {
  msetI(scrn,' ',co*skiptop);
  msetI(scrn+skiptop*co,-1,(li-skiptop)*co);
  msetI(sary,0,li);
  msetI(updtab+skiptop,-1,li-skiptop);
  cur_x= -1;
  cur_y= -1;
  cur_top=li;
  cur_bot=0;
  cur_attr= -1;
  cur_ins= -1;
  attr(0);
  clrins();
  setregn(0,li);

  /* Erase screen on redraw.  Don't like how this looks, even if it is
     faster.
  if(!skiptop)
    if(cl)
      {
      exec(cl,1,0);
      cur_x=0; cur_y=0;
      msetI(scrn,' ',li*co);
      }
    else if(cd)
      {
      cpos(0,0);
      exec(cd,1,0);
      msetI(scrn,' ',li*co);
      }
  */
  }

// Change size of screen

void Scrn::resize(int w,int h)
  {
  if(h<4) h=4;
  if(w<8) w=8;
  li=h;
  co=w;
  if(sary) free(sary);
  if(updtab) free(updtab);
  if(scrn) free(scrn);
  if(compose) free(compose);
  if(magic_ofst) free(magic_ofst);
  if(magic_ary) free(magic_ary);
  scrn=(int *)malloc(li*co*sizeof(int));
  sary=(int *)calloc(li,sizeof(int));
  updtab=(int *)malloc(li*sizeof(int));
  compose=(int *)malloc(co*sizeof(int));
  magic_ofst=(int *)malloc(co*sizeof(int));
  magic_ary=(struct hentry *)malloc(co*sizeof(struct hentry));
  redraw();
  }

void Scrn::attach()
  {
  String xx;
  int x,y;

  li=getnum("li"); if(li<1) li=24;
  co=getnum("co"); if(co<2) co=80;
  x=y=0;
  // Get terminal size
  ttgtsz(&x,&y);
  if(x>7 && y>3) li=y, co=x;

  haz=getflag("hz");
  os=getflag("os");
  eo=getflag("eo");
  if(getflag("hc")) os=1;
  if(os || getflag("ul")) ul=1;
  else ul=0;

  xn=getflag("xn");
  am=getflag("am");

  ti=getstr("ti");
  cl=getstr("cl");
  cd=getstr("cd");

  te=getstr("te");

  ut=getflag("ut");
  Sb=getstr("AB");
  if(!Sb) Sb=getstr("Sb");
  Sf=getstr("AF");
  if(!Sf) Sf=getstr("Sf");

  avattr=0;
  if(me=getstr("me"))
    {
    if((mb=getstr("mb"))) avattr|=BLINK;
    if((md=getstr("md"))) avattr|=BOLD;
    if((mh=getstr("mh"))) avattr|=DIM;
    if((mr=getstr("mr"))) avattr|=INVERSE;
    }

  if(getnum("sg")<=0 && !mr && getstr("se"))
    {
    if(so=getstr("so")) avattr|=INVERSE;
    se=getstr("se");
    }
  if(getflag("xs") || getflag("xt")) so=xx;

  if(getnum("ug")<=0 && getstr("ue"))
    {
    if(us=getstr("us")) avattr|=UNDERLINE;
    ue=getstr("ue");
    }

  if(!(uc=getstr("uc"))) if(ul) uc="_";
  if(uc) avattr|=UNDERLINE;

  ms=getflag("ms");

  da=getflag("da");
  db=getflag("db");
  cs=getstr("cs");
  rr=getflag("rr");
  sf=getstr("sf");
  sr=getstr("sr");
  SF=getstr("SF");
  SR=getstr("SR");
  al=getstr("al");
  dl=getstr("dl");
  AL=getstr("AL");
  DL=getstr("DL");
  if(!getflag("ns") && !sf) sf="\12";

  if(!getflag("in") && baud<38400 )
    {
    dc=getstr("dc");
    DC=getstr("DC");
    dm=getstr("dm");
    ed=getstr("ed");

    im=getstr("im");
    ei=getstr("ei");
    ic=getstr("ic");
    IC=getstr("IC");
    ip=getstr("ip");
    mi=getflag("mi");
    }
  else
    {
    mi=1;
    }

  if(getstr("bc")) bs=getstr("bc");
  else if(getstr("le")) bs=getstr("le");
  if(getflag("bs")) bs="\10";

  cbs=tcost(bs,1,2,2);

  lf="\12";
  if(getstr("do")) lf=getstr("do");
  clf=tcost(lf,1,2,2);

  up=getstr("up");
  cup=tcost(up,1,2,2);

  nd=getstr("nd");

  tw=8;
  if(getnum("it")>0) tw=getnum("it");
  else if(getnum("tw")>0) tw=getnum("tw");

  if(!(ta=getstr("ta"))) if(getflag("pt")) ta="\11";
  bt=getstr("bt");
  if(getflag("xt")) ta=xx, bt=xx;

  cta=tcost(ta,1,2,2);
  cbt=tcost(bt,1,2,2);

  ho=getstr("ho");
  cho=tcost(ho,1,2,2);
  ll=getstr("ll");
  cll=tcost(ll,1,2,2);

  cr="\15";
  if(getstr("cr")) cr=getstr("cr");
  if(getflag("nc") || getflag("xr")) cr=xx;
  ccr=tcost(cr,1,2,2);

  cRI=tcost(RI=getstr("RI"),1,2,2);
  cLE=tcost(LE=getstr("LE"),1,2,2);
  cUP=tcost(UP=getstr("UP"),1,2,2);
  cDO=tcost(DO=getstr("DO"),1,2,2);
  cch=tcost(ch=getstr("ch"),1,2,2);
  ccv=tcost(cv=getstr("cv"),1,2,2);
  ccV=tcost(cV=getstr("cV"),1,2,2);
  ccm=tcost(cm=getstr("cm"),1,2,2);

  cce=tcost(ce=getstr("ce"),1,2,2);

  // Make sure terminal can do absolute positioning
  if(!cm &&
     !(ch && cv) &&
     !(ho && (lf || DO || cv)) &&
     !(ll && (up || UP || cv)) &&
     !(cr && cv))
    {
    cerr << "Sorry, your terminal can't do absolute cursor positioning.\nIt's broken\n";
    exit(-1);
    }

  // Determine if we can scroll
  if((sr || SR) && (sf || SF) && cs ||
     (al || AL) && (dl || DL))
    scroll_flg=1;
  else
    scroll_flg=0;

  // Determine if we can ins/del within lines
  if((im || ic || IC) && (dc || DC)) insdel_flg=1;
  else insdel_flg=0;

  // Adjust for high baud rates
  if(baud>=38400) scroll_flg=0, insdel_flg=0;

  // Initialize variable screen size dependant vars
  resize(co,li);

  // Send out terminal initialization string
  // if(ti) exec(ti,1);
  }

Scrn::Scrn(ostream& new_out,char *name,unsigned ibaud,int idopadding) :
  Cap(new_out,name,ibaud,idopadding)
  {
  magic_htab=(struct hentry *)malloc(256*sizeof(struct hentry));
  skiptop=0;
  sary=0;
  updtab=0;
  scrn=0;
  compose=0;
  magic_ofst=0;
  magic_ary=0;
  attach();
  }

Scrn::~Scrn()
  {
  free(magic_htab);
  if(sary) free(sary);
  if(updtab) free(updtab);
  if(scrn) free(scrn);
  if(compose) free(compose);
  if(magic_ofst) free(magic_ofst);
  if(magic_ary) free(magic_ary);
  }
