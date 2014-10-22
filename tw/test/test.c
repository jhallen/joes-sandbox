#include <stdio.h>
#include "fn.h"

Fn_1<void,int> *foo;

void callback(int a,int b)
  {
  printf("%d %d %d\n",a,b,a+b);
  }

main()
  {
  Fn1_1<void,int,int> z(callback,7);
  foo = &z;
  foo->cont(foo,1);
  printf("%x\n",foo);
  }
