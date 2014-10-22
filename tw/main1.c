#include "scrn.h"
#include "kbd.h"
#include "tty.h"

void ttsig(int n)
  {
  }

main()
  {
  int x, y;
  Scrn scrn;
  ttopen();
  for(x=0;x!=20;++x)
    {
    scrn.outatr(x,x,'*');
    }
  ttclose();

  Kmap<int> k(scrn);
  Kbd<int> kbd(k);
  
  x=0;
  k.add("x to z",1);
  k.add("a b",2);
  k.add("a c",3);
  k.add(".ku",4);
  kbd.dokey(&x,'x'); cout << x << "\n";
  kbd.dokey(&x,'y'); cout << x << "\n";
  kbd.dokey(&x,'z'); cout << x << "\n";
  kbd.dokey(&x,'a'); kbd.dokey(&x,'b'); cout << x << "\n";
  kbd.dokey(&x,'a'); kbd.dokey(&x,'c'); cout << x << "\n";
  k.del("a c");
  kbd.dokey(&x,'a'); kbd.dokey(&x,'b'); cout << x << "\n";
  kbd.dokey(&x,'\033'), kbd.dokey(&x,'['), kbd.dokey(&x,'A'); cout << x << "\n";
  }
