/* Key-map handler */

#ifndef _Ikbd
#define _Ikbd 1

#include "parse.h"
#include "termcap.h"

String parse_range(String,int&,int&);
String getcapseq(Cap&,String);
String parse_dotword(String,String&);

// A keymap

template<class Bind>
class Kmap
  {
  public:

  Cap& cap;			// Termcap entry for this keyboard

  class Keynode
    {
    public:
    bool leaf;			// Set for leaf node
    int key;			// Key code.
    map<int,Keynode> submap;	// If we've got a submap
    Bind bind;			// Binding if this is a leaf node
    Keynode(int new_key,Bind new_bind)
      {
      leaf=1;
      key=new_key;
      bind=new_bind;
      }
    Keynode(int new_key)
      {
      leaf=0;
      }
    Keynode()
      {
      }
    };

  map<int,Keynode> keys;	// Top-level map

  Kmap(Cap& new_cap) :
    cap(new_cap)
    {
    }

  /*
   * Add a key sequence binding to a key map
   *
   * Returns 0 for success
   *         1 for for invalid key sequence
   *
   * A valid key sequence is one or more keys seperated with spaces.  A key
   * is a single character or one of the following strings:
   *
   *    ^?	                   127 (DEL)
   *
   *    ^@   -   ^_                  Control characters
   *
   *    sp, Sp or SP                 32 (space character)
   *
   *    ! - ~			   Key for printable ASCII character
   *
   *    .xx			   Termcap string sequence
   *
   *    xxxxx			   X key sequence name
   *
   * In addition, the last key of a key sequence may be replaced with
   * a range-fill of the form: <KEY> TO <KEY>
   *
   * So for example, if the sequence: ^K A TO Z
   * is speicified, then the key sequences
   * ^K A, ^K B, ^K C, ... ^K Z are all bound.
   */
  bool dobuild(map<int,Keynode>& keys,String seq,String capseq,Bind bind)
    {
    int v, w;
    String rest;
    if(!capseq)
      {
      String s;
      String rest;
      rest=parse_dotword(seq,s);
      if(rest)
        {
        capseq=getcapseq(cap,s);
        if(!capseq) return 1;
        else seq=rest;
        }
      }

    if(capseq.size())
      {
      v=w=capseq[0];
      capseq=capseq+1;
      }
    else if(!(seq=parse_range(seq,v,w)))
      return 1;

    while(v<=w)
      {
      if(capseq.size() || seq.size())
        { // Add an internal node
        typename map<int,Keynode>::iterator p=keys.find(v);
        if(p!=keys.end() && !p->second.leaf)
          {
          if(dobuild(p->second.submap,seq,capseq,bind))
            return 1;
          }
        else
          {
          Keynode& node=(keys[v]=Keynode(v));
          if(dobuild(node.submap,seq,capseq,bind))
            return 1;
          }
        }
      else
        { // Add a leaf node
        keys[v]=Keynode(v,bind);
        }
      ++v;
      }
    return 0;
    }

  bool add(String seq,Bind bind)
    {
    return dobuild(keys,seq,String(),bind);
    }

  void domerge(map<int,Keynode>& dest,map<int,Keynode>& src)
    {
    typename map<int,Keynode>::iterator p,q;
    for(p=src.begin();p!=src.end();++p)
      {
      q=dest.find(p->first);
      if(q!=dest.end() && !p->second.leaf && !q->second.leaf)
        domerge(q->second.submap,p->second.submap);
      else
        dest[p->first]=p->second;
      }
    }

  // Merge all of the entries in the 'src' keymap into this keymap
  void merge(Kmap<Bind>& src)
    {
    domerge(keys,src.keys);
    }

  bool dodel(map<int,Keynode>& keys,String seq,String capseq)
    {
    int v, w;
    String rest;

    if(!capseq)
      {
      String s;
      String rest;
      rest=parse_dotword(seq,s);
      if(rest)
        {
        capseq=getcapseq(cap,s);
        if(!capseq) return 1;
        else seq=rest;
        }
      }

    if(capseq.size())
      {
      v=w=capseq[0];
      capseq=capseq+1;
      }
    else if(!(seq=parse_range(seq,v,w)))
      return 1;

    while(v<=w)
      {
      typename map<int,Keynode>::iterator p=keys.find(v);
      if(p!=keys.end())
        {
        Keynode& n=p->second;
        if(!n.leaf && (seq.size() || capseq.size()))
          {
          if(dodel(p->second.submap,seq,capseq))
            return 1;
          }
        else
          keys.erase(p);
        }
      ++v;
      }
    return 0;
    }

  /* Delete a binding from a keymap
   *
   * Returns 0 for success
   *         1 if the given key sequence was invalid
   */
  bool del(String seq)
    {
    return dodel(keys,seq,String());
    }
  };

// A keyboard event handler

template<class Bind>
class Kbd
  {
  public:
  Kmap<Bind>* kmap;
  map<int,typename Kmap<Bind>::Keynode> *curmap;
  int seq[16];			// Current sequence of keys
  int x;			// No. keys we have so far in this sequence

  Kbd()
    {
    kmap=0;
    curmap=0;
    x=0;
    }

  void stkmap(Kmap<Bind> *new_kmap)
    {
    kmap=new_kmap;
    curmap= &kmap->keys;
    x=0;
    }

  /*
   Handle a key for a KBD:

     Returns 0 for unknown key
     Returns 1 for prefix key accepted
     Returns 2 for a completed key sequence (Binding return in bind).
  */
  int dokey(Bind *bind,int key)	// Handle a key
    {
    int n;
    typename map<int,typename Kmap<Bind>::Keynode>::iterator p;

    /* If we're starting from scratch, clear the keymap sequence buffer */
    if(curmap== &kmap->keys) x=0;

    /* Search for key code */
    p=curmap->find(key);

    if(p!=curmap->end())
      if(!p->second.leaf)
        { /* A prefix key was found */
        seq[x++]=key;
        curmap= &p->second.submap;
        return 1;
        }
      else
        { /* A complete key sequence was entered */
        x=0;
        curmap= &kmap->keys;
        *bind= p->second.bind;
        return 2;
        }
    else
      {
      x=0;
      curmap= &kmap->keys;
      return 0;
      }
    }

  int seqlen(void)		// Return the no. of keys processed so far
    {
    return x;
    }

  int nthkey(int n)		// Return value of nth key in sequence
    {
    return seq[n];
    }

  void clrkbd(void)		// Clear keysequence
    {
    curmap= &kmap->keys;
    }
  };

/* Key code modifiers */

#define Shift  0x00010000
#define Ctrl   0x00040000
#define Alt    0x00080000

#endif
