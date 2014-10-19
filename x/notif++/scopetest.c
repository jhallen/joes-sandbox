#include "notif.h"
#include <math.h>

main()
  {
  unsigned char data[32768];
  Scroll *reg;
  Scope *s;
  int x;
  izws();
  reg = new Scroll();
    reg->sth(300);
    reg->stw(400);
    s = new Scope();
      for (x = 0; x != 32768; ++x)
    	data[x] = 128 + 127 * sin(x * M_PI * 256.0 / 32768.0);
      s->st(data);
    reg->st(s);
  root->add(reg);
  for (;;)
    {
    wsflsh();
    io();
    }
  }
