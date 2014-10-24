#include "text.h"

void Text::st(String val)
  {
  buf=val;
  if(!ckw()) stwidth(buf.size());
  if(!ckh()) stheight(txth(gtfgnd()));
  }

String Text::gt()
  {
  return buf;
  }

void Text::draw(int new_x,int new_y,int new_w,int new_h)
  {
  drawclr();
  drawtxt(gtfgnd(),gtwidth()/2-txtw(gtfgnd(),buf)/2,gtheight()/2-txth(gtfgnd())/2,buf);
  }
