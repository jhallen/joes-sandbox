#include <stdio.h>

class foo
  {
  public:
  virtual void fred() { printf("Foo!\n"); }
  };

class bar : public foo
  {
  public:
  virtual void fred() { printf("Bar!\n"); }
  };

foo x;
bar y;
foo *z;

main()
{
x=y;
(&x)->fred();
z= &y;
z->fred();
}
