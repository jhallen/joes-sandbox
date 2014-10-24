#include "scrn.h"
#include "kbd.h"
#include "tty.h"
#include "widget.h"
#include "text.h"
#include "field.h"
#include "edit.h"

void ttsig(int n)
  {
  }

main()
  {
  int c;
  Widget *bord;
  Widget *b;
  Text *a;
  Field *x, *y;
  Edit *z;

  ttopen();
  screen.root->hcenter();
  screen.root->ttob(0,0);

  a=new Text();
    a->stfgnd(INVERSE);
    a->st("This is a test");
  screen.root->add(a);

  b=new Widget();
    b->stbknd('*');
    b->stwidth(6);
    b->stheight(6);
  screen.root->add(b);

  bord=new Widget();
    bord->stheight(3);
    bord->stwidth(32);
    bord->stbknd(' '|BG_YELLOW);
    x=new Field();
      x->sttitle("(name)");
      x->stwidth(30);
      x->stx(1);
      x->sty(1);
    bord->add(x);
  screen.root->add(bord);

  y=new Field();
    y->st("Second");
  screen.root->add(y);

  z=new Edit();
    z->stwidth(30);
    z->stheight(10);
  screen.root->add(z);

  // cout << "no. kids " << screen.root->nwidgets() << "|\n";

  screen.stfocus(x);
  
  for(;;)
    {
    screen.update();
    c=cin.get();
    if(c=='X')
      {
      ttclose();
      return 0;
      }
    screen.keypress(c);
    }
  }
