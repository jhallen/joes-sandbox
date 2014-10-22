#include <stdlib.h>
#include "widget.h"

Screen screen;

// User focus commands

void Widget::ufocusnext(int c)
  {
  Widget *n;
  if(screen.focus) n=screen.focusnext(screen.focus);
  if(n) screen.stfocus(n);
  }

void Widget::ufocusprev(int c)
  {
  Widget *n;
  if(screen.focus) n=screen.focusprev(screen.focus);
  if(n) screen.stfocus(n);
  }

Screen::Screen() :
  stdkmap(scrn)
  {
  int y, x;
  scrn_buf=(int *)malloc(sizeof(int)*scrn.width()*scrn.height());
  for(y=0;y!=scrn.height();++y)
    for(x=0;x!=scrn.width();++x)
      scrn_buf[y*scrn.width()+x]=' ';
  cur_x=0;
  cur_y=0;
  focus=0;
  focus_rtn=0;
  root=new Widget();
    root->stw(scrn.width());
    root->sth(scrn.height());
    root->on_flag=1;
    root->force_recalc();
    root->redraw();
  stdkmap.add("^I",&Widget::ufocusnext);
  }

Screen::~Screen()
  {
  delete root;
  free(scrn_buf);
  }

Kmap< void (Widget::*)(int) > *Screen::gtstdkmap()
  {
  return &stdkmap;
  }

// Make terminal match scrn_buf

void Screen::update()
  {
  if(needs_redraw)
    {
    int y, x, yidx;
    update_dirty(root);	// Send redraw events
    for(y=0, yidx=0;y!=scrn.height();++y,yidx+=scrn.width())
      {
      int count;
      int maxx;
      /* Use eraeol() if it will save from writing 5 characters */
      count=0;
      for(x=scrn.width()-1;x>=0;--x)
        if(scrn_buf[yidx+x]!=' ') break;
        else if(scrn.scrn[yidx+x]!=' ') ++count;
      ++x;
      if(count>4)
        {
        scrn.eraeol(x,y);
        maxx=x;
        }
      else
        maxx=scrn.width();

      /* Character by character update */
      for(x=0;x!=maxx;++x)
        if(scrn_buf[yidx+x]!=scrn.scrn[yidx+x])
          scrn.outatr(x,y,scrn_buf[yidx+x]);
      }
    needs_redraw=0;
    }
  if(focus) scrn.cpos(focus->x_cur+focus->x_scrn,focus->y_cur+focus->y_scrn);
  else scrn.cpos(cur_x,cur_y);
  }

// Recursively update dirty Widgets

void Screen::update_dirty(Widget *w)
  {
  // cout << "Update " << (int)w << " \n";
  if(w->needs_recalc)
    {
    if(w->in)
      {
      w->x_scrn=w->gtx()+w->in->x_scrn;
      w->y_scrn=w->gty()+w->in->y_scrn;
      }
    w->rects=w->clip(0,0,w->gtwidth(),w->gtheight());
    w->needs_recalc=0;
    }
  if(w->needs_full_redraw)
    {
    w->draw(0,0,w->gtwidth(),w->gtheight());
    w->needs_redraw=0;
    w->needs_full_redraw=0;
    }
  else if(w->needs_redraw)
    {
    w->draw(w->redraw_rect.x,w->redraw_rect.y,
      w->redraw_rect.w,w->redraw_rect.h);
    w->needs_redraw=0;	// Mark as no longer dirty
    }
  for(Widget *i=w->kids.next;i;i=i->link.next) update_dirty(i);
  }

// Focus managment

void Screen::focuspos(Widget *w)
  {
  while(w->gtmom())
    {
    w->gtmom()->focus_path=w->gtmom()->find(w);
    w=w->gtmom();
    }
  }

Widget *Screen::focusnext(Widget *w)
  {
  int wrap=0;
  Widget *last=0;
  if(!w->gtmom())
    {
    w->focus_path= -1;
    goto down;
    }

  up: // Leave current node
  w=w->gtmom();
  if(w==last) return w;

  down: // Find next child
  if(w->focus_path==w->no_kids()-1)
    {
    if(!w->gtmom())
      {
      w->focus_path= -1;
      if(++w->focus_path==2) return 0;
      }
    else goto up;
    }

  ++w->focus_path;
  w=w->nth_kid(w->focus_path);
  w->focus_path= -1;
  if(w->ckon() && w->ckfocusable())
    {
    last=w;
    if(w->no_kids()) goto down;
    else return w;
    }
  else goto down;
  }

Widget *Screen::focusprev(Widget *w)
  {
  int wrap=0;
  Widget *last=0;
  if(!w->gtmom())
    {
    w->focus_path=w->no_kids();
    goto down;
    }

  up: // Leave current node
  w=w->gtmom();
  if(w==last) return w;

  down: // Find next child
  if(w->focus_path<=0)
    {
    if(!w->gtmom())
      {
      w->focus_path=w->no_kids();
      if(++wrap==2) return 0;
      }
    else goto up;
    }

  --w->focus_path;
  w=w->nth_kid(w->focus_path);

  w->focus_path=w->no_kids();

  if(w->ckon() && w->ckfocusable())
    {
    last=w;
    if(w->no_kids()) goto down;
    else return w;
    }
  else goto down;
  }

Widget *Screen::gtfocus()
  {
  return focus;
  }

void Screen::clfocus()
  {
  if(focus) focus->unfocus();
  if(focus_rtn) focus_rtn->unfocusrtn();
  focus=0;
  focus_rtn=0;
  }

void Screen::stfocus(Widget *w)
  {
  if(focus!=w)
    {
    clfocus();
    focus=w;
    focus_rtn=w;
    focuspos(w);
    focus->focus();
    focus_rtn->focusrtn();
    }
  }

void Screen::stfocus_rtn(Widget *w)
  {
  if(focus_rtn!=w)
    {
    focus_rtn->unfocusrtn();
    focus_rtn=w;
    focus_rtn->focusrtn();
    }
  }

// Handle keypress

void Screen::keypress(int c)
  {
  if(c=='\r')
    {
    if(screen.focus_rtn) screen.focus_rtn->user(c);
    }
  else if(screen.focus)
    {
    if(screen.focus) screen.focus->user(c);
    }
  }

// Return true if a point is contained within one of the
// rectangles in vector<Rect> (and therefore isn't clipped).

bool Widget::not_clipped(vector<Rect>& rects,int x,int y)
  {
  vector<Rect>::iterator p;
  for(p=rects.begin();p!=rects.end();++p)
    if(x>=p->x && x<p->x+p->w && y>=p->y && y<p->y+p->h)
      return 1;
  return 0;
  }

// Given a rectangle (x,y,w,h) specified in the Widget's
// coordinate system, return a list of sub-rectangles which
// are not clipped (i.e., are directly on the screen).  The
// returned list is in the top-level screen's coordinate
// system.

vector<Rect> Widget::clip(int x,int y,int w,int h)
  {
  vector<Rect> rects;

  // Clip within our boundaries
  if(x<0) w+=x, x=0;
  if(y<0) h+=y, y=0;
  if(x+w>width) w=width-x;
  if(y+h>height) h=height-y;

  // Check our kids
  sibclip(rects,this,kids.next,x,y,w,h);

  return rects;
  }

// i points to a child of Widget 'parent'.  (x,y,w,h) is a rectangle in
// 'parent's coordinate system.  sibclip checks i and all of the windows
// which follow in the sibling list for interference with the rectangle.
// Once that is complete, siblings of the parent are checked, and so on.

void Widget::sibclip(vector<Rect>& rects,Widget *parent,Widget *i,int x,int y,int w,int h)
  {
  for(;;)
    {
    // Check for interfering siblings
    for(;i;i=i->link.next)
      // First do a quick check.  This should be the case
      // for most windows.
      if(i->x_pos>=x+w || i->x_pos+i->width<=x ||
         i->y_pos>=y+h || i->y_pos+i->height<=y)
        { // Completely non-interfering
        }
      // It obscures us in some way.  Find out how...
      else if(i->x_pos+i->width>=x+w)
        // Extends right
        if(i->x_pos<=x)
          // Extends right+left
          if(i->y_pos+i->height>=y+h)
            // Extends right+left+below
            if(i->y_pos<=y)
              { // Extends right+left+below+above (totally obscured)
              return;
              }
            else
              { // Extends right+left+below+!above
              // Bottom must be clipped
              h=i->y_pos-y;
              }
          else
            // Extends right+left+!below
            if(i->y_pos<=y)
              { // Extends right+left+!below+above
              // Top must be clipped
              h=y+height-(i->y_pos+i->height);
              y=i->y_pos+i->height;
              }
            else
              { // Extends right+left+!below+!above
              // Horz band
              // Above
              sibclip(rects,parent,i->link.next,x,y,w,i->y_pos-y);
              // Below
              h=y+h-(i->y_pos+i->height);
              y=i->y_pos+i->height;
              }
        else
          // Extends right+!left
          if(i->y_pos+i->height>=y+h)
            // Extends right+!left+below
            if(i->y_pos<=y)
              { // Extends right+!left+below+above
              // Right must be clipped
              w=i->x_pos-x;
              }
            else
              { // Extends right+!left+below+!above
              // Lower right corner
              // Upper band
              sibclip(rects,parent,i->link.next,x,y,w,i->y_pos-y);
              // Lower left rect
              h=y+height-i->y_pos;
              y=i->y_pos;
              w=i->x_pos-x;
              }
          else
            // Extends right+!left+!below
            if(i->y_pos<=y)
              { // Extends right+!left+!below+above
              // Upper right corner
              // Lower band
              sibclip(rects,parent,i->link.next,x,i->y_pos+i->height,w,y+h-(i->y_pos+i->height));
              // Upper left rect
              h=i->y_pos-y;
              w=i->x_pos-x;
              }
            else
              { // Extends right+!left+!below+!above
              // Right notch
              // Upper band
              sibclip(rects,parent,i->link.next,x,y,w,i->y_pos-y);
              // Lower band
              sibclip(rects,parent,i->link.next,x,i->y_pos+i->height,w,y+h-(i->y_pos+i->height));
              // Left rect
              h=i->height;
              y=i->y_pos;
              w=i->x_pos-x;
              }
      else
        // Extends !right
        if(i->x_pos<=x)
          // Extends !right+left
          if(i->y_pos+i->height>=y+h)
            // Extends !right+left+below
            if(i->y_pos<=y)
              { // Extends !right+left+below+above
              // Left must be clipped
              w=x+width-(i->x_pos+i->width);
              x=i->x_pos+i->width;
              }
            else
              { // Extends !right+left+below+!above
              // Lower left corner
              // Upper band
              sibclip(rects,parent,i->link.next,x,y,w,i->y_pos-y);
              // Lower right rect
              h=y+height-i->y_pos;
              y=i->y_pos;
              w=x+w-(i->x_pos+i->width);
              x=i->x_pos+i->width;
              }
          else
            // Extends !right+left+!below
            if(i->y_pos<=y)
              { // Extends !right+left+!below+above
              // Upper left corner
              // Lower band
              sibclip(rects,parent,i->link.next,x,i->y_pos+i->height,w,y+h-(i->y_pos+i->height));
              // Upper right rect
              h=i->y_pos+i->height-y;
              w=w+x-(i->x_pos+i->width);
              x=i->x_pos+i->width;
              }
            else
              { // Extends !right+left+!below+!above
              // Left notch
              // Upper band
              sibclip(rects,parent,i->link.next,x,y,w,i->y_pos-y);
              // Lower band
              sibclip(rects,parent,i->link.next,x,i->y_pos+i->height,w,y+h-(i->y_pos+i->height));
              y=i->y_pos;
              h=i->height;
              w=x+w-(i->x_pos+i->width);
              x=i->x_pos+i->width;
              }
        else
          // Extends !right+!left
          if(i->y_pos+i->height>=y+h)
            // Extends !right+!left+below
            if(i->y_pos<=y)
              { // Extends !right+!left+below+above
              // Veritcal band
              // Left side
              sibclip(rects,parent,i->link.next,x,y,i->x_pos-x,h);
              // Right side
              w=x+w-(i->x_pos+i->width);
              x=i->x_pos+i->width;
              }
            else
              { // Extends !right+!left+below+!above
              // Lower notch
              // Upper band
              sibclip(rects,parent,i->link.next,x,y,w,i->y_pos-y);
              // Lower left rect
              sibclip(rects,parent,i->link.next,x,i->y_pos,i->x_pos-x,y+h-i->y_pos);
              // Lower right rect
              w=x+w-(i->x_pos+i->width);
              x=i->x_pos+i->width;
              h=y+h-i->y_pos;
              y=i->y_pos;
              }
          else
            // Extends !right+!left+!below
            if(i->y_pos<=y)
              { // Extends !right+!left+!below+above
              // Upper notch
              // Lower band
              sibclip(rects,parent,i->link.next,x,i->y_pos+i->height,w,y+h-(i->y_pos+i->height));
              // Upper left rect
              sibclip(rects,parent,i->link.next,x,y,i->x_pos-x,i->y_pos+i->height-y);
              // Upper right rect
              w=x+w-(i->x_pos+i->width);
              x=i->x_pos+i->width;
              h=i->y_pos+i->height-y;
              }
            else
              { // Extends !right+!left+!below+!above
              // Center notch
              // Upper band
              sibclip(rects,parent,i->link.next,x,y,w,i->y_pos-y);
              // Lower band
              sibclip(rects,parent,i->link.next,x,i->y_pos+i->height,w,y+h-(i->y_pos+i->height));
              // Left rect
              sibclip(rects,parent,i->link.next,x,i->y_pos,i->x_pos-x,i->height);
              // Right rect
              w=x+w-(i->x_pos+i->width);
              x=i->x_pos+i->width;
              h=i->height;
              y=i->y_pos;
              }
    if(parent->in)
      {
      // Transform into parent's coordinate system
      x+=parent->x_pos;
      y+=parent->y_pos;

      // Switch to parent
      parent=parent->in;

      // Clip within parent's boundaries
      if(x<0) w+=x, x=0;
      if(y<0) h+=y, y=0;
      if(x+w>parent->width) w=parent->width-x;
      if(y+h>parent->height) h=parent->height-y;

      // Check for overlap from parent's siblings
      i=parent->link.next;
      }
    else
      break;
    }

  // Add clear rectangle to list
  rects.push_back(Rect(x,y,w,h));
  }

// User drawing and information functions

int Widget::txth(Gc gc)
  {
  return 1;
  }

int Widget::txtb(Gc gc)
  {
  return 0;
  }

int Widget::txtw(Gc gc,String s)
  {
  return s.size();
  }

void Widget::drawdot(Gc c,int x,int y)
  {
  x+=x_scrn;
  y+=y_scrn;
  if(not_clipped(rects,x,y)) screen.scrn_buf[screen.scrn.width()*y+x]=c;
  }

void Widget::drawtxt(Gc c,int x,int y,String text)
  {
  for(int j=0;j!=text.size();++j) drawdot(c|text[j],x+j,y);
  }

void Widget::drawfield(Gc c,int w,int x,int y,String text)
  {
  for(int j=0;j!=text.size() && j!=w && j+x<gtwidth();++j) drawdot(c|text[j],x+j,y);
  }

void Widget::drawline(Gc c,int x,int y,int w,int h)
  {
  if(w==1)
    for(int j=0;j!=h;++j) drawdot(c,x,y+j);
  else if(h==1)
    for(int i=0;i!=w;++i) drawdot(c,x+i,y);
  else
    { // Full Bresenham
    }
  }

void Widget::drawrect(Gc c,int x,int y,int w,int h)
  {
  drawline(c,x,y,w,1);
  drawline(c,x,y+h-1,w,1);
  drawline(c,x,y,1,h);
  drawline(c,x+w-1,y,1,h);
  }

void Widget::drawbox(Gc c,int x,int y,int w,int h,int size)
  {
  for(int n=0;n!=size;++n) drawrect(c,x+n,y+n,w-n*2,h-n*2);
  }

void Widget::drawfilled(Gc c,int x,int y,int w,int h)
  {
  for(int j=y;j!=y+h;++j)
    for(int i=x;i!=x+w;++i)
      drawdot(c,i,j);
  }

void Widget::drawclr()
  {
  for(int j=0;j!=gtheight();++j)
    for(int i=0;i!=gtwidth();++i)
      drawdot(gtbknd(),i,j);
  }

void Widget::drawclr(int x,int y,int w,int h)
  {
  drawfilled(gtbknd(),x,y,w,h);
  }

// Redraw entire widget

void Widget::redraw()
  {
  needs_full_redraw=1;
  screen.needs_redraw=1;
  }

// Redraw a subset of it

void Widget::redraw(int new_x,int new_y,int new_w,int new_h)
  {
  if(needs_full_redraw) return;
  else if(!needs_redraw)
    {
    needs_redraw=1;
    screen.needs_redraw=1;
    redraw_rect.x=new_x;
    redraw_rect.y=new_y;
    redraw_rect.w=new_w;
    redraw_rect.h=new_h;
    }
  else
    {
    if(new_x<redraw_rect.x)
      {
      redraw_rect.w+=redraw_rect.x-new_x;
      redraw_rect.x=new_x;
      }
    if(new_x+new_w>redraw_rect.x+redraw_rect.w)
      redraw_rect.w=new_x+new_w-redraw_rect.x;
    if(new_y<redraw_rect.y)
      {
      redraw_rect.h+=redraw_rect.y-new_y;
      redraw_rect.y=new_y;
      }
    if(new_y+new_h>redraw_rect.y+redraw_rect.h)
      redraw_rect.h=new_y+new_h-redraw_rect.y;
    }
  }

void Widget::focus()
  {
  }

void Widget::focusrtn()
  {
  }

void Widget::unfocus()
  {
  }

void Widget::unfocusrtn()
  {
  }

void Widget::resize(int new_w,int new_h)
  {
  }

void Widget::force_recalc()
  {
  Widget *w;
  for(w=kids.next;w;w=w->link.next) w->force_recalc();
  needs_recalc=1;
  }

void Widget::on()
  {
  if(ckenabled() && !ckon())
    {
    Widget *w;
    gtwidth();
    gtheight();
    on_flag=1;
    force_recalc();
    for(w=link.prev;w;w=w->link.prev) w->force_recalc();
    gtmom()->needs_recalc=1;
    for(w=kids.next;w;w=w->link.next) w->on();
    screen.needs_redraw=1;
    redraw();
    }
  }

void Widget::off()
  {
  if(ckon())
    {
    if(screen.gtfocus()==this) screen.clfocus();
    for(Widget *w=kids.next;w;w=w->link.next) w->off();
    for(Widget *w=link.prev;w;w=w->link.prev) w->force_recalc();
    if(gtmom()) gtmom()->needs_recalc=1;
    screen.needs_redraw=1;
    on_flag=0;
    }
  }

void Widget::add(Widget *w)
  {
  kids.push_back(w);
  w->in=this;
  pmgr_add(this,w);
  if(ckon()) w->on();
  }

void Widget::rmv()
  {
  Widget *m=gtmom();
  off();
  in=0;
  if(m) m->kids.deque(this);
  }

int Widget::no_kids()
  {
  int n;
  Widget *w;
  for(w=kids.next,n=0;w;w=w->link.next) ++n;
  return n;
  }

int Widget::any_kids()
  {
  return kids.next!=0;
  }

Widget *Widget::nth_kid(int n)
  {
  Widget *w;
  for(w=kids.next;n;w=w->link.next) --n;
  return w;
  }

int Widget::find(Widget *w)
  {
  int n;
  Widget *t;
  for(t=kids.next,n=0;t!=w && t->link.next;t=t->link.next) ++n;
  return n;
  }

void Widget::stwidth(int new_w)
  {
  if(new_w!=width)
    {
    width=new_w;
    calcx(this);
    }
  }

void Widget::stiw(int new_w)
  {
  stwidth(new_w);
  }

void Widget::stw(int new_w)
  {
  stwidth(new_w);
  }

void Widget::stheight(int new_h)
  {
  if(new_h!=height)
    {
    height=new_h;
    calcy(this);
    }
  }

void Widget::stih(int new_h)
  {
  stheight(new_h);
  }

void Widget::sth(int new_h)
  {
  stheight(new_h);
  }

void Widget::stx(int new_x)
  {
  x_pos = new_x;
  }

void Widget::sty(int new_y)
  {
  y_pos = new_y;
  }

void Widget::enable()
  {
  enable_flag=1;
  if(gtmom() && gtmom()->ckon()) on();
  }

void Widget::disable()
  {
  enable_flag=0;
  off();
  }

void Widget::focusable()
  {
  focusable_flag=1;
  }

void Widget::unfocusable()
  {
  focusable_flag=0;
  }

int Widget::gtwidth()
  {
  if(!ckw()) calcw(this);
  return width;
  }

int Widget::gtiw()
  {
  return gtwidth();
  }

int Widget::gtw()
  {
  return gtwidth();
  }

int Widget::gtheight()
  {
  if(!ckh()) calch(this);
  return height;
  }

int Widget::gtih()
  {
  return gtheight();
  }

int Widget::gth()
  {
  return gtheight();
  }

int Widget::gtx()
  {
  if(x_pos!=0x7FFFFFFF) return x_pos;
  else return 0;
  }

int Widget::gty()
  {
  if(y_pos!=0x7FFFFFFF) return y_pos;
  else return 0;
  }

Widget *Widget::gtmom()
  {
  return in;
  }

int Widget::ckenabled()
  {
  return enable_flag;
  }

int Widget::ckfocusable()
  {
  return focusable_flag;
  }

int Widget::ckon()
  {
  return on_flag;
  }

int Widget::ckx()
  {
  return x_pos!=0x7FFFFFFF;
  }

int Widget::cky()
  {
  return y_pos!=0x7FFFFFFF;
  }

int Widget::ckw()
  {
  return width!=-1;
  }

int Widget::ckh()
  {
  return height!=-1;
  }

Gc Widget::gtbknd()
  {
  return bknd_gc;
  }

void Widget::stbknd(Gc gc)
  {
  bknd_gc=gc;
  if(ckon()) redraw();
  }

Gc Widget::gtfgnd()
  {
  return fgnd_gc;
  }

void Widget::stfgnd(Gc gc)
  {
  fgnd_gc=gc;
  if(ckon()) redraw();
  }

// Handle user input

void Widget::user(int c)
  {
  void (Widget::*fn)(int);
  if(kbd.dokey(&fn,c)==2) (this->*fn)(c);
  }

Widget::Widget() :
  screen(::screen)
  {
  in=0;
  x_pos=0x7FFFFFFF;
  y_pos=0x7FFFFFFF;
  x_scrn=0;
  y_scrn=0;
  width= -1;
  height= -1;
  on_flag=0;
  enable_flag=1;
  focusable_flag=0;
  focus_path=0;
  needs_redraw=0;
  needs_full_redraw=1;
  needs_recalc=1;
  bknd_gc=screen.gtstdbknd();
  fgnd_gc=screen.gtstdfgnd();
  on_left=0;
  on_right=0;
  on_up=0;
  on_down=0;
  on_tab=0;
  on_rtn=0;
  stkmap(screen.gtstdkmap());
  x_cur=0;
  y_cur=0;
  }

void Widget::stkmap(Kmap< void (Widget::*)(int) > *new_kmap)
  {
  kmap=new_kmap;
  kbd.stkmap(kmap);
  }

Widget::~Widget()
  {
  rmv();
  }

void showrects(vector<Rect>& r)
  {
  vector<Rect>::iterator p;
  for(p=r.begin();p!=r.end();++p)
    {
    cout << "rect x=" << p->x << " y=" << p->y << " w=" << p->w << " h=" << p->h << "\n";
    }
  }

void Widget::draw(int new_x,int new_y,int new_w,int new_h)
  {
  // cout << "draw width=" << gtwidth() << " height=" << gtheight() << "\n";
  // showrects(rects);
  drawfilled(gtbknd(),0,0,gtwidth(),gtheight());
  }
