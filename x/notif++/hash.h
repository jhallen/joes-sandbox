// Hash table template 

#include <string>
using namespace std;

template<class T>
class Hash
  {
  public:
  struct Entry
    {
    Entry *next;
    string name;
    T val;
    int hv;
    Entry()
      {
      next=0;
      }
    };
  Entry *table;
  int nentries;
  int last;

  struct Iterator
    {
    Hash<T> *hash;
    int idx;
    Entry *e;
    };

  void next(Iterator& i)
    {
    if(i.e) i.e=i.e->next;
    while(!i.e && i.idx!=last+1)
      {
      i.e=table[++i.idx].next;
      }
    }

  T indirect(Iterator& i)
    {
    return i.e->val;
    }

  int test(Iterator& i)  
    {
    return i.idx!=last+1;
    }

  void begin(Iterator& i)
    {
    i.hash=this;
    i.idx=-1;
    i.e=0;
    next(i);
    }

  Hash()
    {
    nentries=0;
    last=15;
    table=new Entry[last+1];
    }

  ~Hash()
    {
    int x;
    Entry *e, *n;
    for(x=0;x<=last;++x)
      for(e=table[x].next;e;e=n)
        {
        n=e->next;
        delete e;
        }
    delete[] table;
    }

  void enlarge(void)
    {
    int x;
    Entry *e, *n;
    int newlast=(last+1)*2-1;
    Entry *newtable=new Entry[newlast+1];
    for(x=0;x<=last;++x)
      for(e=table[x].next;e;e=n)
        {
        n=e->next;
        e->next=newtable[e->hv&newlast].next;
        newtable[e->hv&newlast].next=e;
        }
    delete[] table;
    table=newtable;
    last=newlast;
    }

  int hval(string s)
    {
    int accu, x;
    for(accu=x=0;x!=s.length();++x) accu=(accu<<4)^s[x]^(accu>>28);
    return accu;
    }

  void insert(string name,T val)
    {
    int idx=hval(name);
    Entry *e=new Entry;
    if(++nentries==last+1) enlarge();
    e->hv=idx;
    e->next=table[idx&last].next;
    e->name=name;
    e->val=val;
    table[idx&last].next=e;
    }

  T find(string name)
    {
    int idx=hval(name);
    Entry *e;
    for(e=table[idx&last].next;e;e=e->next)
      if(e->name==name) return e->val;
    return 0;
    }

  T remove(string name)
    {
    int idx=hval(name);
    Entry *e;
    for(e=table+(idx&last);e->next;e=e->next)
      if(e->next->name==name)
        {
        Entry *x=e->next;
        T r=x->val;
        e->next=x->next;
        delete x;
        --nentries;
        return r;
        }
    return 0;
    }

  };
