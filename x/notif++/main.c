#include "notif.h"

Button *b1;
Button *b2;

void fred1()
  {
  printf("Hi!\n");
  b1->stfn(new Fn0_0<void>(fred1));
  }

void fred2()
  {
  printf("There!\n");
  b2->stfn(new Fn0_0<void>(fred2));
  }

main()
  {
  Index *idx;
  Scroll *reg;
  Widget *w;
  Widget *x;
  Text *t;
  Icon *i;
  Button *b;
  Mbutton *a;
  Mbcascade *c;
  Mcascade *d;
  Menu *men;
  Gsep *sep;
  Hbar *hbar;
  Vbar *vbar;
  Toggle *to;
  izws();
  Radio *r;
  Edit *e;
  Tedit *te;
  Menu *submenu;

  submenu = new Menu();
    a=new Mbutton();
      a->sttext("Sub-first");
    submenu->add(a);
    a=new Mbutton();
      a->sttext("Sub-second");
    submenu->add(a);
    a=new Mbutton();
      a->sttext("Sub-third");
    submenu->add(a);
  root->add(submenu);

  men = new Menu();
    a=new Mbutton();
      a->sttext("First");
    men->add(a);
    d=new Mcascade();
      d->sttext("Second on is long");
      d->st(submenu);
    men->add(d);
    a=new Mbutton();
      a->sttext("Third");
    men->add(a);
  root->add(men);

  idx = new Index();

  reg=new Scroll();
    reg->sth(350);
    reg->stvstep(16);
    reg->stw(350);

  w = new Widget();
    w->ltor(NULL,0);
    w->tedge(0);

    x = new Widget();
      x->ltor(NULL,0);
      x->tedge(0);
      c=new Mbcascade();
        sthelptext(c,"File menu");
        c->sttext("File");
        c->st(men);
      x->add(c);

      c=new Mbcascade();
        sthelptext(c,"Edit menu");
        c->sttext("Edit");
        c->st(men);
      x->add(c);

    w->add(x);

    w->ledge(0);
    w->ttob(x,0);

    e = new Edit();
    w->add(e);
    te = new Tedit();
    te->st("{d2 1 12}:{d2 0 59}:{d2 0 59}");
    w->add(te);


    t = new Text();
      t->st("Hello, world!");
      t->stborder(new Shadow());
      sthelptext(t,"Foo!");
    w->add(t);
    sep = new Gsep();
      sep->stwidth(100);
    w->add(sep);
    t = new Text();
      t->st("Hello, world!");
      t->stborder(new Shadow());
      sthelptext(t,"Bar!");
    w->add(t);
    t = new Text();
      t->st("Hello, world!");
      t->stborder(new Shadow());
      sthelptext(t,"Bar!");
    w->add(t);
    i = new Icon();
      i->st(piccheck);
    w->add(i);
    i = new Icon();
      i->st(picarc);
    w->add(i);
    i = new Icon();
      i->st(picarc);
      i->stwidth(100);
      i->stheight(100);
    w->add(i);
    b1 = new Button();
      b1->sttext("Press");
      b1->stfn(new Fn0_0<void>(fred1));
    w->add(b1);
    b2 = new Button();
      b2->sttext("Me");
      b2->stfn(new Fn0_0<void>(fred2));
    w->add(b2);
    hbar = new Hbar();
      hbar->stwidth(100);
    w->add(hbar);
    vbar = new Vbar();
      vbar->stheight(100);
    w->add(vbar);
    to = new Toggle();
    w->add(to);
    r = new Radio();
      to = new Toggle();
      r->groupadd(to);
      to = new Toggle();
      r->groupadd(to);
      to = new Toggle();
      r->groupadd(to);
    w->add(r);

  reg->st(w);

  b = new Button();
    b->sttext("First");

  idx->addpair(b, reg);

  b = new Button();
    b->sttext("Second");
  t = new Text();
    t->st("Foo!");
  
  idx->addpair(b, t);

  root->add(idx);


  for(;;)
    {
    wsflsh();
    io();
    }
  }
