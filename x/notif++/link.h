#ifndef _Ilink
#define _Ilink 1

// Doubly-linked list node and base

// If you want a doubly-linked list of objects, instantiate one
// of these in the object, and one as the base somewhere else:
//
// For example, a doubly linked list of integers:
//
// class Number
//   {
//   Link<Number> link; // This _must_ be called 'link'.
//   int value;
//   };
//
// Link<Number> items; // This is the base.
//
// items.push_back(new Number()); // Insert a new node

// To iterate through the doubly-linked list, use pointers:
//
// for(Number *l=items.next;l;l=l->link.next) do_something(l);

// Base can be copied cheaply.  When this happens, any nodes in the
// destination are deleted (typically there are none), and all of the nodes
// from the source are moved to the destination. The source ends up empty
// after the copy.

template<class T>
struct Link
  {
  // Doubly-linked list pointers
  mutable T *next;
  mutable T *prev;

  int empty()
    {
    return next==0;
    }

  // Constructor
  Link()
    {
    next=0;
    prev=0;
    }

  // Copy constructor
  Link(const Link& from)
    {
    next=from.next;
    prev=from.prev;
    from.next=0;
    from.prev=0;
    }

  // Assignment
  Link& operator=(Link& from)
    {
    T *p, *n;
    for(p=next;p;p=n)
      {
      n=p->link.next;
      p->link.next=0;
      p->link.prev=0;
      delete p;
      }
    next=from.next;
    prev=from.prev;
    from.next=0;
    from.prev=0;
    return *this;
    }

  // Destructor
  ~Link()
    {
    T *p, *n;
    for(p=next;p;p=n)
      {
      n=p->link.next;
      p->link.next=0;
      p->link.prev=0;
      delete p;
      }
    }

  // Get nth node

  T *nth(int n)
    {
    T *p = next;
    while(n)
      {
      p = p->link.next;
      --n;
      }
    return p;
    }

  // Count the nodes

  int no()
    {
    int n = 0;
    T *p;
    for(p=next;p;p=p->link.next)
      ++n;
    return n;
    }

  // Find a node's position

  int findn(T *l)
    {
    int n = 0;
    T *p;
    for (p=next; p; ++n, p=p->link.next)
      if(p==l)
        break;
    if (p) return n;
    else return -1;
    }

  // Remove a node

  T *deque(T *node)
    {
    if(node->link.next)
      node->link.next->link.prev=node->link.prev;
    else
      prev=node->link.prev;

    if(node->link.prev)
      node->link.prev->link.next=node->link.next;
    else
      next=node->link.next;

    node->link.next=0;
    node->link.prev=0;

    return node;
    }

  // Remove a chain

  void snip(Link& l,T *first,T *last)
    {
    l.next=first;
    l.prev=last;

    if(first->link.prev) first->link.prev->link.next=last->link.next;
    else next=last->link.next;

    if(last->link.next) last->link.next->link.prev=first->link.prev;
    else prev=first->link.prev;

    // return l;
    }

  // Insert a node before base

  T *push_back(T *node)
    {
    node->link.next=0;
    node->link.prev=prev;
    if(prev)
      prev->link.next=node;
    else
      next=node;
    prev=node;
    return node;
    }

  // Insert chain at end of list

  T *push_back(Link& chain)
    {
    if(chain.next)
      {
      T *rtn=chain.next;

      chain.next->link.prev=prev;

      if(prev) prev->link.next=chain.next;
      else next=chain.next;

      prev=chain.prev;

      chain.next=0;
      chain.prev=0;
      return rtn;
      }
    else
      return 0;
    }

  // Insert node before another node

  T *insert_before(T *x,T *node)
    {
    node->link.next=x;
    node->link.prev=x->link.prev;
    if(x->link.prev)
      x->link.prev->link.next=node;
    else
      next=node;
    x->link.prev=node;
    return node;
    }

  // Insert chain before another node

  T *insert_before(T *x,Link& chain)
    {
    if(chain.next)
      {
      T *rtn=chain.next;

      chain.prev->link.next=x;
      chain.next->link.prev=x->link.prev;

      if(x->link.prev)
        x->link.prev->link.next=chain.next;
      else
        next=chain.next;

      x->link.prev=chain.prev;

      chain.next=0;
      chain.prev=0;
      return rtn;
      }
    else
      return x;
    }

  // Insert node after another node

  T *insert_after(T *x,T *node)
    {
    node->link.next=x->link.next;
    node->link.prev=x;
    if(x->link.next)
      x->link.next->link.prev=node;
    else
      prev=node;
    x->link.next=node;
    return node;
    }

  // Insert chain after another node

  T *insert_after(T *x,Link& chain)
    {
    if(chain.next)
      {
      T *rtn=chain.next;

      chain.next->link.prev=x;
      chain.prev->link.next=x->link.next;

      if(x->link.next)
        x->link.next->link.prev=chain.prev;
      else
        prev=chain.prev;

      x->link.next=chain.next;

      chain.next=0;
      chain.prev=0;
      return rtn;
      }
    else
      return x->link.next;
    }

  // Insert node after base

  T *push_front(T *node)
    {
    node->link.prev=0;
    node->link.next=next;
    if(next)
      next->link.prev=node;
    else
      prev=node;
    next=node;
    return node;
    }

  // Insert chain at beginning of list

  T *push_front(Link& chain)
    {
    if(chain.next)
      {
      T *rtn=chain.next;

      chain.prev->link.next=next;
      if(next)
        next->link.prev=chain.prev;
      else
        prev=chain.prev;
      next=chain.next;

      chain.next=0;
      chain.prev=0;
      return rtn;
      }
    else
      return 0;
    }

  // Promote to front

  T *promote(T *node)
    {
    return push_front(deque(node));
    }

  // Demote to back

  T *demote(T *node)
    {
    return push_back(deque(node));
    }
  };

#endif
