// Edit widget

#include "edit.h"

Kmap< void (Widget::*)(int) > *edit_kmap;

void Edit::st(String val)
  {
  buf.bof->del(buf.eof);
  cursor->inss(val);
  repos();
  }

String Edit::gt()
  {
  return buf.bof->rs(buf.eof->byte);
  }

void Edit::draw(int new_x,int new_y,int new_w,int new_h)
  {
  // Check for cursor position change
  repos();
  if(!buf.eof->byte)
    { // Buffer is empty, so just display title
    drawclr();
    drawtxt(gtfgnd(),0,0,title);
    }
  else
    { // Display edit buffer
    int y;
    String bf(new_w);
    Pointer *p=top->clone();
    for(y=0;y!=gtheight();++y)
      if(y>=new_y && y<new_y+new_h)
        { // We need to draw this line
        long col;
        int c;
        drawclr(new_x,y,new_w,1);
        col=0;

        while(col<new_x+new_w+loffset)
          {
          c=p->getc();
          if(c==-1) break;
          if(c=='\n') break;
          if(c=='\t')
            {
            do
              {
              if(col>=new_x+loffset && col<new_x+new_w+loffset)
                bf[col-loffset-new_x]=' ';
              ++col;
              }
              while(col&7);
            }
          else
            {
            if(col>=new_x+loffset && col<new_x+new_w+loffset)
              bf[col-loffset-new_x]=c;
            ++col;
            }
          }

        while(col<loffset+new_x+new_w)
          {
          if(col>=loffset+new_x) bf[col-loffset-new_x]=' ';
          ++col;
          }

        // Draw the buffer
        drawtxt(gtfgnd(),new_x,y,bf);

        // Move to next line if we're not already there
        if(c==-1 || (c!='\n' && !p->nextl()))
          {
          ++y;
          break;
          }
        }
      else if(!p->nextl())
        {
        ++y;
        break;
        }
    for(;y!=gtheight();++y)
      if(y>=new_y && y<new_y+new_h)
        { // We need to erase this line
        drawclr(new_x,y,new_w,1);
        }
    p->rm();
    }
  }

void Edit::focus()
  {
  stbknd(' '+INVERSE);
  stfgnd(INVERSE);
  }

void Edit::unfocus()
  {
  stbknd('_');
  stfgnd(0);
  }

void Edit::repos()
  {
  if(cursor->xcol<loffset)
    {
    loffset=cursor->xcol;
    redraw();
    }
  if(cursor->xcol>=loffset+gtwidth())
    {
    loffset=cursor->xcol-gtwidth()+1;
    redraw();
    }
  if(cursor->line<top->line)
    {
    top->set(cursor);
    top->bol();
    redraw();
    }
  if(cursor->line>=top->line+gtheight())
    {
    top->goto_line(cursor->line-gtheight()+1);
    redraw();
    }
  y_cur=cursor->line-top->line;
  x_cur=cursor->xcol-loffset;
  }

void Edit::type(int c)
  {
  if(cursor->xcol!=cursor->gtcol())
    {
    cursor->xcol=cursor->gtcol();
    }
  cursor->insc(c);
  redraw(cursor->gtcol()-loffset,cursor->line-top->line,gtwidth()-(cursor->gtcol()-loffset),1);
  cursor->getc();
  cursor->xcol=cursor->gtcol();
  repos();
  }

void Edit::delch(int c)
  {
  Pointer *p;
  if(cursor->xcol!=cursor->gtcol())
    {
    cursor->xcol=cursor->gtcol();
    return;
    }
  if(cursor->iseof()) return;
  redraw(cursor->gtcol()-loffset,cursor->line-top->line,gtwidth()-(cursor->gtcol()-loffset),1);
  p=cursor->clone();
  if(p->getc()=='\n' && cursor->line-top->line+1!=gtheight())
    redraw(0,cursor->line-top->line+1,gtwidth(),gtheight()-(cursor->line-top->line+1));
  cursor->del(p);
  p->rm();
  }

void Edit::backs(int c)
  {
  if(cursor->xcol!=cursor->gtcol())
    {
    cursor->xcol=cursor->gtcol();
    return;
    }
  if(cursor->rgetc()!=-1)
    {
    Pointer *p;
    if(cursor->iseof()) return;
    redraw(cursor->gtcol()-loffset,cursor->line-top->line,gtwidth()-(cursor->gtcol()-loffset),1);
    p=cursor->clone();
    if(p->getc()=='\n' && cursor->line-top->line+1!=gtheight())
      redraw(0,cursor->line-top->line+1,gtwidth(),gtheight()-(cursor->line-top->line+1));
    cursor->del(p);
    p->rm();
    cursor->xcol=cursor->gtcol();
    repos();
    }
  }

void Edit::bol(int c)
  {
  cursor->bol();
  cursor->xcol=cursor->gtcol();
  repos();
  }

void Edit::eol(int c)
  {
  cursor->eol();
  cursor->xcol=cursor->gtcol();
  repos();
  }

void Edit::ltarw(int c)
  {
  if(cursor->xcol!=cursor->gtcol())
    {
    cursor->xcol=cursor->gtcol();
    repos();
    return;
    }
  cursor->rgetc();
  cursor->xcol=cursor->gtcol();
  repos();
  }

void Edit::rtarw(int c)
  {
  if(cursor->xcol!=cursor->gtcol())
    {
    cursor->xcol=cursor->gtcol();
    }
  cursor->getc();
  cursor->xcol=cursor->gtcol();
  repos();
  }

void Edit::uparw(int c)
  {
  if(cursor->line)
    {
    cursor->prevl();
    cursor->goto_col(cursor->xcol);
    repos();
    }
  }

void Edit::dnarw(int c)
  {
  if(cursor->line!=buf.eof->line)
    {
    cursor->nextl();
    cursor->goto_col(cursor->xcol);
    repos();
    }
  }

void Edit::dellin(int c)
  {
  Pointer *p=cursor->clone();
  redraw(0,cursor->line-top->line,gtwidth(),gtheight()-(cursor->line-top->line));
  cursor->bol();
  p->nextl();
  cursor->del(p);
  p->rm();
  cursor->xcol=cursor->gtcol();
  repos();
  }

void Edit::rtn(int c)
  {
  cursor->insc('\n');
  redraw(0,cursor->line-top->line,gtwidth(),gtheight()-(cursor->line-top->line));
  cursor->getc();
  cursor->xcol=cursor->gtcol();
  repos();
  }

void Edit::sttitle(String val)
  {
  title=val;
  redraw();
  }

Edit::Edit()
  {

  if(!edit_kmap)
    {
    edit_kmap=new Kmap< void (Widget::*)(int) >(screen.scrn);
    edit_kmap->merge(*screen.gtstdkmap());
    edit_kmap->add("^A",(void (Widget::*)(int))&Edit::bol);
    edit_kmap->add(".kh",(void (Widget::*)(int))&Edit::bol);
    edit_kmap->add("^E",(void (Widget::*)(int))&Edit::eol);
    edit_kmap->add(".kH",(void (Widget::*)(int))&Edit::eol);
    edit_kmap->add("^F",(void (Widget::*)(int))&Edit::rtarw);
    edit_kmap->add(".kr",(void (Widget::*)(int))&Edit::rtarw);
    edit_kmap->add("^B",(void (Widget::*)(int))&Edit::ltarw);
    edit_kmap->add(".kl",(void (Widget::*)(int))&Edit::ltarw);
    edit_kmap->add("^P",(void (Widget::*)(int))&Edit::uparw);
    edit_kmap->add(".ku",(void (Widget::*)(int))&Edit::uparw);
    edit_kmap->add("^N",(void (Widget::*)(int))&Edit::dnarw);
    edit_kmap->add(".kd",(void (Widget::*)(int))&Edit::dnarw);
    edit_kmap->add("^Y",(void (Widget::*)(int))&Edit::dellin);
    edit_kmap->add("^D",(void (Widget::*)(int))&Edit::delch);
    edit_kmap->add(".kD",(void (Widget::*)(int))&Edit::delch);
    edit_kmap->add("^H",(void (Widget::*)(int))&Edit::backs);
    edit_kmap->add("^?",(void (Widget::*)(int))&Edit::backs);
    edit_kmap->add("^M",(void (Widget::*)(int))&Edit::rtn);
    edit_kmap->add("sp TO ~",(void (Widget::*)(int))&Edit::type);
    }

  stheight(txth(gtfgnd())*8);
  stwidth(txtw(gtfgnd(),"M")*32);
  stkmap(edit_kmap);
  cursor=buf.bof->clone();
  top=buf.bof->clone();
  loffset=0;
  stbknd('_');
  focusable();
  }
