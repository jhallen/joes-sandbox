#include <iostream>

#include "termcap.h"

main()
  {
  Cap cp;
  map<String,String>::iterator i;
  for(i=cp.caps.begin();i!=cp.caps.end();++i)
    cout << i->first << "\t" << i->second << "\n";
  cp.exec(cp.getstr("cl"),24,0,0,0,0);
  cout << cp.tcost(cp.getstr("cl"),24,0,0,0,0) << "\n";
  cout << cp.tcompile(cp.getstr("cm"),1,5,5,0,0) << "\n";
  }
