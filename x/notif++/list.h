// List container

template<class T>
class List
  {
  public:

  struct Node
    {
    Node *next;
    T val;
    Node(T new_val)
      {
      val = new_val;
      next = 0;
      }
    };
  Node *first;
  Node *last;
  int nentries;

  struct Iterator
    {
    Node *p;
    };

  int no()
    {
    return nentries;
    }

  T nth(int n)
    {
    Node *l = first;
    while (l && n)
      {
      l = l->next;
      --n;
      }
    return l->val;
    }

  int find(T val)
    {
    int x = 0;
    Node *l = first;
    while (l && l->val != val)
      {
      ++x;
      l = l->next;
      }
    return x;
    }

  void next(Iterator& i)
    {
    if (i.p)
      i.p = i.p->next;
    }

  T indirect(Iterator& i)
    {
    return i.p->val;
    }

  int test(Iterator& i)  
    {
    return i.p != 0;
    }

  void begin(Iterator& i)
    {
    i.p = first;
    }

  List()
    {
    nentries = 0;
    first = 0;
    last = 0;
    }

  ~List()
    {
    Node *p, *n;
    for (p = first;p;p=n)
      {
      n=p->next;
      delete p;
      }
    }

  void insert(T val)
    {
    Node *e = new Node(val);
    if (last)
      {
      last->next = e;
      last = e;
      }
    else
      {
      first = last = e;
      }
    ++nentries;
    }
  };
