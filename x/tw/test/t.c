#include <stdio.h>

#include <map>
using namespace std;

template<class T>
class Kmap
  {
  public:
  class Keynode
    {
    public:
    int x;
    };

  void foo(void)
    {
    typename map<int,Keynode>::iterator p;
    }
  };

Kmap<int> x;

int main()
  {
  x.foo();
  return 0;
  }
