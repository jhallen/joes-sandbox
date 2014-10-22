// Field widget

#include "field.h"

Kmap< void (Widget::*)(int) > *field_kmap;

void Field::st(String val)
  {
  buf=val;
  if(!ckw()) stwidth(buf.size()+1);
  if(!ckh()) stheight(txth(gtfgnd()));
  cursor=buf.size();
  repos();
  }

String Field::gt()
  {
  return buf;
  }

void Field::draw(int new_x,int new_y,int new_w,int new_h)
  {
  drawclr();
  repos();
  if(!buf.size())
    { // Display title
    if(title.size()>=gtwidth()) drawtxt(gtfgnd(),0,gtheight()/2-txth(gtfgnd())/2,title.substr(0,gtwidth()));
    else drawtxt(gtfgnd(),0,gtheight()/2-txth(gtfgnd())/2,title);
    }
  else
    { // Display edif buffer
    if(buf.size()-ofst>=gtwidth()) drawtxt(gtfgnd(),0,gtheight()/2-txth(gtfgnd())/2,buf.substr(ofst,gtwidth()));
    else drawtxt(gtfgnd(),0,gtheight()/2-txth(gtfgnd())/2,buf.substr(ofst,buf.size()-ofst));
    }
  }

void Field::st_trigger(Fn_0<String> *fn)
  {
  trigger=fn;
  }

void Field::focus()
  {
  stbknd(' '+INVERSE);
  stfgnd(INVERSE);
  }

void Field::unfocus()
  {
  stbknd('_');
  stfgnd(0);
  }

void Field::repos()
  {
  if(cursor<ofst)
    {
    ofst=cursor;
    redraw();
    }
  if(cursor>=ofst+gtwidth())
    {
    ofst=cursor-gtwidth()+1;
    redraw();
    }
  y_cur=gtheight()/2-txth(gtfgnd())/2;
  x_cur=cursor-ofst;
  }

void Field::type(int c)
  {
  char ch=c;
  buf.insert(cursor,&ch,1);
  ++cursor;
  redraw();
  }

void Field::delch(int c)
  {
  if(cursor!=buf.size())
    {
    buf.erase(cursor,1);
    redraw();
    }
  }

void Field::backs(int c)
  {
  if(cursor)
    {
    --cursor;
    delch(0);
    redraw();
    }
  }

void Field::rtn(int c)
  {
  }

void Field::bol(int c)
  {
  cursor=0;
  repos();
  }

void Field::eol(int c)
  {
  cursor=buf.size();
  repos();
  }

void Field::ltarw(int c)
  {
  if(cursor) --cursor;
  repos();
  }

void Field::rtarw(int c)
  {
  if(cursor!=buf.size()) ++cursor;
  repos();
  }

void Field::sttitle(String val)
  {
  title=val;
  if(!buf.size()) redraw();
  }

Field::Field()
  {
  if(!field_kmap)
    {
    field_kmap=new Kmap< void (Widget::*)(int) >(screen.scrn);
    field_kmap->merge(*screen.gtstdkmap());
    field_kmap->add("^A",(void (Widget::*)(int))&Field::bol);
    field_kmap->add(".kh",(void (Widget::*)(int))&Field::bol);
    field_kmap->add("^E",(void (Widget::*)(int))&Field::eol);
    field_kmap->add(".kH",(void (Widget::*)(int))&Field::eol);
    field_kmap->add("^F",(void (Widget::*)(int))&Field::rtarw);
    field_kmap->add(".kr",(void (Widget::*)(int))&Field::rtarw);
    field_kmap->add("^B",(void (Widget::*)(int))&Field::ltarw);
    field_kmap->add(".kl",(void (Widget::*)(int))&Field::ltarw);
    field_kmap->add("^D",(void (Widget::*)(int))&Field::delch);
    field_kmap->add(".kD",(void (Widget::*)(int))&Field::delch);
    field_kmap->add("^H",(void (Widget::*)(int))&Field::backs);
    field_kmap->add("^?",(void (Widget::*)(int))&Field::backs);
    field_kmap->add("^M",(void (Widget::*)(int))&Field::rtn);
    field_kmap->add("sp TO ~",(void (Widget::*)(int))&Field::type);
    }

  stheight(txth(gtfgnd()));
  stkmap(field_kmap);
  trigger=0;
  cursor=0;
  ofst=0;
  stbknd('_');
  focusable();
  }
