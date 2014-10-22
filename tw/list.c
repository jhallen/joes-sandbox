#include <stdio.h>
#include "date.h"
#include "list.h"

// Review autoincrement stuff

Kmap< void (Widget::*)(int) > *list_kmap;

void List::repos(void)
  {
  if(row<top)
    {
    top=row;
    no_scroll=0;
    redraw();
    }
  if(row>=top+gtheight()-1)
    {
    top=row-(gtheight()-2);
    no_scroll=0;
    redraw();
    }
  if(col<left)
    {
    left=0;
    no_scroll=0;
    redraw();
    }
  if(start[col]+widths[col]-start[left]>gtwidth())
    {
    while(start[col]+widths[col]-start[left]>gtwidth())
      left++;
    no_scroll=0;
    redraw();
    }
  if(no_scroll && (row_prev!=row || col_prev!=col))
    {
    redraw(start[col]-start[left],row-top+1,widths[col],1);
    redraw(start[col_prev]-start[left],row_prev-top+1,widths[col_prev],1);
    }
  row_prev=row;
  col_prev=col;
  if(emode)
    if(ebuf.size()>widths[col])
      x_cur=widths[col]+start[col]-start[left], y_cur=row-top+1;
    else
      x_cur=ebuf.size()+start[col]-start[left], y_cur=row-top+1;
  else
    x_cur=start[col]-start[left], y_cur=row-top+1;
  no_scroll=1;
  }

// Redraw cell being edited

void List::redraw_edit(void)
  {
  redraw(start[col]-start[left],row-top+1,widths[col],1);
  }

void List::draw(int new_x,int new_y,int new_w,int new_h)
  {
  int x,y;

  // Title

  if(new_y==0)
    {
    drawclr(0,0,gtwidth(),1);

    for(x=left;start[x]-start[left]<gtwidth() && x<v->ncols();++x)
      drawfield(UNDERLINE,widths[x],start[x]-start[left],0,v->colname(x));
    }

  // Body

  for(y=1;y<gtheight();++y)
    if(y>=new_y && y<new_y+new_h)
      {
      // y is screen row
      // y+top-1 is view row
      drawclr(0,y,gtwidth(),1);

      if(y+top-1<v->nrows())
        {
        for(x=left;start[x]-start[left]<gtwidth() && x<v->ncols();++x)
          {
          // x is view column
          // x-left is screen column
          char datebuf[80];
          int mode;
          char buf[40];
          sprintf(buf,"%d",nextno);

          if(top+y-1==row && x==col)
            // Cursor is in this field
            if(emode)
              mode=6;
            else if(v->coltype(x)==1)
              mode=4;
            else if(v->coltype(x)==2 && y+top-1==v->nrows())
              mode=5;
            else
              mode=3;
          else
            // Cursor is not in this field
            if(v->coltype(x)==1)
              mode=1;
            else if(v->coltype(x)==2 && y+top-1==v->nrows())
              mode=2;
            else
              mode=0;

          switch(mode)
            {
            case 0: // Normal
              {
              drawfield(gtfgnd(),widths[x],start[x],y,v->gt(x,top+y-1));
              break;
              }
            case 1: // Date
              {
              printdate1w(datebuf,v->gt(x,top+y-1).c_str());
              drawfield(gtfgnd(),widths[x],start[x],y,datebuf);
              break;
              }
            case 2: // Autonumber
              {
              drawfield(gtfgnd(),widths[x],start[x],y,buf);
              break;
              }
            case 3: // Cursor
              {
              drawfield(INVERSE,widths[x],start[x],y,v->gt(x,top+y-1));
              break;
              }
            case 4: // Cursor + Date
              {
              printdate1w(datebuf,v->gt(x,top+y-1).c_str());
              drawfield(INVERSE,widths[x],start[x],y,datebuf);
              break;
              }
            case 5: // Cursor + Autonumber
              {
              drawfield(INVERSE,widths[x],start[x],y,buf);
              break;
              }
            case 6: // Editing
              {
              if(ebuf.size()>widths[x])
                drawfield(INVERSE,widths[x],start[x],y,ebuf.substr(ebuf.size()-widths[x],widths[x]));
              else
                drawfield(INVERSE,widths[x],start[x],y,ebuf);
              break;
              }
            }
          }
        }
      }
  }

void List::echk(void)
  {
  char date[10];
  if(emode)
    {
    if(eplusminus==0)
      v->st(col,row,ebuf);
    else if(eplusminus==1)
      {
      int n=atoi(v->gt(col,row).c_str());
      // v->st(col,row,ntos(atoi(ebuf)+n));
      }
    else if(eplusminus==5)
      {
      // parsedate(date,ebuf);
      v->st(col,row,date);
      }
    else
      {
      int n=atoi(v->gt(col,row).c_str());
      // v->st(col,row,ntos(n-atoi(ebuf.c_str())));
      }
    if(row!=v->nrows())
      {
      // v->mod(row); // Write modified row
      }
    else
      {
      char buf[40];
      if(incflag)
        {
        sprintf(buf,"%d",nextno++);
        v->st(col,row,buf);
        }
      // v->add(row);
      }
    redraw_edit();
    emode=0;
    }
  }

void List::uparw(int c)
  {
  echk();
  if(row) row--;
  repos();
  }

void List::dnarw(int c)
  {
  echk();
  if(row!=v->nrows()) row++;
  repos();
  }

void List::rtarw(int c)
  {
  echk();
  if(col+1!=v->ncols())
    col++;
  else if(row!=v->nrows())
    {
    col=0;
    row++;
    }
  repos();
  }

void List::ltarw(int c)
  {
  echk();
  if(col) col--;
  else if(row)
    {
    row--;
    col=v->ncols()-1;
    }
  repos();
  }

void List::bof(int c)
  {
  echk();
  row=0;
  col=0;
  repos();
  }

void List::eof(int c)
  {
  echk();
  if(v->nrows()) row=v->nrows()-1;
  repos();
  }

void List::bol(int c)
  {
  echk();
  col=0;
  repos();
  }

void List::eol(int c)
  {
  echk();
  col=v->ncols()-1;
  repos();
  }

void List::pgup(int c)
  {
  echk();
  if(top>=(gtheight()-1)/2)
    {
    top-=(gtheight()-1)/2;
    row-=(gtheight()-1)/2;
    no_scroll=0;
    redraw();
    }
  else if(!top)
    {
    row=0;
    }
  else
    {
    row-=top;
    top=0;
    no_scroll=0;
    redraw();
    }
  repos();
  }

void List::pgdn(int c)
  {
  echk();
  if(top+gtheight()-1+(gtheight()-1)/2<=v->nrows())
    {
    top+=(gtheight()-1)/2;
    row+=(gtheight()-1)/2;
    no_scroll=0;
    redraw();
    }
  else if(top+gtheight()-1<v->nrows())
    {
    int x=v->nrows()-(top+gtheight()-1);
    top+=x;
    row+=x;
    no_scroll=0;
    redraw();
    }
  else if(row!=v->nrows())
    {
    row=v->nrows()-1;
    }
  repos();
  }

void List::dellin(int c)
  {
  echk();
  if(incflag) return; /* Could be better... */
  // if(row!=v->nrows())
  //  v->del(row);
  }

void List::backs(int c)
  {
  if(!emode) return;
  if(ebuf.size()) ebuf.erase(ebuf.size()-1,1);
  else emode=0;
  redraw_edit();
  }

void List::add(int c)
  {
  if(!emode)
    {
    if(incflag && !col) return;
    emode=1;
    ebuf="";
    eplusminus=1;
    redraw_edit();
    }
  else type('+');
  }

void List::date(int c)
  {
  if(!emode)
    {
    if(incflag && !col) return;
    emode=1;
    ebuf="";
    eplusminus=5;
    redraw_edit();
    }
  }

void List::sub(int c)
  {
  if(!emode)
    {
    if(incflag && !col) return;
    emode=1;
    ebuf="";
    eplusminus= -1;
    redraw_edit();
    }
  else type('-');
  }

void List::type(int c)
  {
  if(!emode)
    {
    if(incflag && !col) return;
    emode=1;
    ebuf="";
    eplusminus=0;
    }
  ebuf+=c;
  redraw_edit();
  }

void List::st(View *new_v)
  {
  int x;
  v=new_v;
  widths=(int *)malloc(sizeof(int)*v->ncols());
  start=(int *)malloc(sizeof(int)*v->ncols());
  for(x=0;x!=v->ncols();++x)
    widths[x]=v->colwidth(x);
  start[0]=0;
  for(x=1;x<v->ncols();++x)
    start[x]=start[x-1]+widths[x]+1;
  top=0;
  left=0;
  row=0;
  col=0;
  col_prev=0;
  row_prev=0;
  no_scroll=1;
  redraw();
  }

void List::ufocusnext(int c)
  {
  echk();
  Widget::ufocusnext(c);
  }

List::List()
  {
  if(!list_kmap)
    {
    list_kmap=new Kmap< void (Widget::*)(int) >(screen.scrn);
    list_kmap->add("sp TO ~",(void (Widget::*)(int))&List::type);
    list_kmap->add("^A",(void (Widget::*)(int))&List::bol);
    list_kmap->add(".kh",(void (Widget::*)(int))&List::bol);
    list_kmap->add("^E",(void (Widget::*)(int))&List::eol);
    list_kmap->add(".kH",(void (Widget::*)(int))&List::eol);
    list_kmap->add("^F",(void (Widget::*)(int))&List::rtarw);
    list_kmap->add(".kr",(void (Widget::*)(int))&List::rtarw);
    list_kmap->add("^B",(void (Widget::*)(int))&List::ltarw);
    list_kmap->add(".kl",(void (Widget::*)(int))&List::ltarw);
    list_kmap->add("^P",(void (Widget::*)(int))&List::uparw);
    list_kmap->add(".ku",(void (Widget::*)(int))&List::uparw);
    list_kmap->add("^N",(void (Widget::*)(int))&List::dnarw);
    list_kmap->add(".kd",(void (Widget::*)(int))&List::dnarw);
    list_kmap->add("^Y",(void (Widget::*)(int))&List::dellin);
    list_kmap->add("^H",(void (Widget::*)(int))&List::backs);
    list_kmap->add("^?",(void (Widget::*)(int))&List::backs);
    // list_kmap->add("^M",(void (Widget::*)(int))&List::rtn);
    list_kmap->add("+",(void (Widget::*)(int))&List::add);
    list_kmap->add("-",(void (Widget::*)(int))&List::sub);
    list_kmap->add("^P",(void (Widget::*)(int))&List::pgup);
    list_kmap->add("^N",(void (Widget::*)(int))&List::pgdn);
    list_kmap->add("^K U",(void (Widget::*)(int))&List::bof);
    list_kmap->add("^K u",(void (Widget::*)(int))&List::bof);
    list_kmap->add("^K ^U",(void (Widget::*)(int))&List::bof);
    list_kmap->add(".kp",(void (Widget::*)(int))&List::bof);
    list_kmap->add("^K V",(void (Widget::*)(int))&List::eof);
    list_kmap->add("^K v",(void (Widget::*)(int))&List::eof);
    list_kmap->add("^K ^V",(void (Widget::*)(int))&List::eof);
    list_kmap->add(".kn",(void (Widget::*)(int))&List::eof);
    list_kmap->add("^I",(void (Widget::*)(int))&List::ufocusnext);
    }

  stheight(txth(gtfgnd())*8);
  stwidth(txtw(gtfgnd(),"M")*32);
  stkmap(list_kmap);
  top=0;
  left=0;
  row=0;
  col=0;
  row_prev=0;
  col_prev=0;
  start=0;
  widths=0;
  emode=0;
  eplusminus=0;
  incflag=0;
  nextno=0;
  no_scroll=1;
  v=0;
  stbknd('_');
  focusable();
  }

List::~List()
  {
  if(start) free(start);
  if(widths) free(widths);
  }
