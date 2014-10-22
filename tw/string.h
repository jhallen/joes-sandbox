// Improved string library
// by: Joe Allen (jhallen@world.std.com).

// Todo:
// String(String&,int start=0,int size=npos);
// String(int size,char);
// length() (same as size)
// exception when position is past end
// String& operator=(char);
// String& assign(const String&);
// String& assign(const String&,int pos,int size);
// String& assign(const char *,int size);
// String& assign(const char *);
// String& assign(int size,char);
// int compare(int pos,int n,const String& s);
// int compare(int pos,int n,const String& s,int pos2,int n2);
// int compare(int pos,int n,const char *,int n2=npos);
// bool operator==(const char *,String&); (etc. for other compares).
// void push_back(char c) (same as s+=c).
// append, insert, replace, erase
// +
// find (for substrings).
// rfind
// find_last_of
// find_last_not_of
// explicit eol on getline
// swap.

// Stringstream adapted from stringstream written by:
// Written by Magnus Fromreide (magfr@lysator.liu.se).
// seekoff and ideas for overflow is largely borrowed from libstdc++-v3

#ifndef _Istring
#define _Istring 1

#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>

using namespace std;

struct Stringraw
  {
  int ref;		// Reference count
  int len;		// String length
  // String data starts here
  };

class String
  {
  Stringraw *raw;	// Pointer to possibly shared malloc block holding string
  char *s;		// Start of string
  int len;		// Length of string

  // Deference a string.  Free it if is completely dereferenced.
  inline void dereference(const Stringraw *orgraw)
    {
    Stringraw *raw=(Stringraw *)orgraw;
    if(!--raw->ref) free(raw);
    }

  // Allocate a Stringraw of specified length
  char *allocate_raw(Stringraw*& raw,int len)
    {
    raw=(Stringraw *)malloc(sizeof(Stringraw)+len+1);
    raw->ref=1;
    raw->len=len;
    ((char *)(raw+1))[len]=0;
    return (char *)(raw+1);
    }

  // Allocate a Stringraw of specified length and load it
  char *copy_on_write(Stringraw*& raw,const char *org,int len)
    {
    char *rtn=allocate_raw(raw,len);
    if(len) memcpy(rtn,org,len);
    return rtn;
    }

  int cmp(const char *a,int a_len,const char *b,int b_len) const;

  public:

  // Destructor
  ~String()
    {
    if(raw) dereference(raw);
    }

  // Constructors
  String()
    {
    raw=0;
    s=0;
    len=0;
    }

  String(const String& org)
    {
    if(org.s)
      {
      ++org.raw->ref;
      raw=org.raw;
      len=org.len;
      s=org.s;
      }
    else
      {
      raw=0;
      s=0;
      len=0;
      }
    }

  String(const char *org)
    {
    if(org)
      {
      len=strlen(org);
      s=copy_on_write(raw,org,len);
      }
    else
      {
      raw=0;
      s=0;
      len=0;
      }
    }

  String(const char *org_s,int org_len)
    {
    len=org_len;
    s=copy_on_write(raw,org_s,org_len);
    }

  String(int org_len)
    {
    len=org_len;
    s=allocate_raw(raw,org_len);
    }

  // Corresponding assignment operators
  inline String& operator=(const char *org)
    {
    if(raw) dereference(raw);
    if(org)
      {
      len=strlen(org);
      s=copy_on_write(raw,org,len);
      }
    else
      {
      raw=0;
      s=0;
      len=0;
      }
    return *this;
    }

  inline String& operator=(const String& org)
    {
    if(raw) dereference(raw);
    if(org.s)
      {
      raw=org.raw;
      ++raw->ref;
      len=org.len;
      s=org.s;
      }
    else
      {
      raw=0;
      s=0;
      len=0;
      }
    return *this;
    }

  // Array read
  inline char operator[](int x) const
    {
    return s[x];
    }

  // Array write
  inline char& operator[](int x)
    {
    if(raw->ref!=1)
      {
      Stringraw *org=raw;
      s=copy_on_write(raw,s,len);
      dereference(org);
      }
    return s[x];
    }

  // Insert

  String& insert(int pos,const char *new_s,int new_len);

  // Delete

  String& erase(int pos,int n);

  // Append

  inline String& append(const char *new_s,int new_len)
    {
    return insert(size(),new_s,new_len);
    }

  inline String& append(const String& s)
    {
    return insert(size(),s.data(),s.size());
    }

  inline String& append(const char *s)
    {
    return insert(size(),s,strlen(s));
    }

  inline String& append(char c)
    {
    return insert(size(),&c,1);
    }

  inline String& operator+=(const String& s)
    {
    return append(s);
    }

  inline String& operator+=(const char *s)
    {
    return append(s);
    }

  inline String& operator+=(char c)
    {
    return append(c);
    }

  // Copy to an array
  inline void copy(char *dest,int n) const
    {
    if(n) memcpy(dest,s,n);
    }

  // Compare
  int compare(const String& s) const
    {
    return cmp(data(),size(),s.data(),s.size());
    }

  int compare(const char *s) const
    {
    return cmp(data(),size(),s,strlen(s));
    }

  inline bool operator==(const String& s) const
    {
    return compare(s)==0;
    }

  inline bool operator==(const char *s) const
    {
    return compare(s)==0;
    }

  inline bool operator!=(const String& s) const
    {
    return compare(s)!=0;
    }

  inline bool operator!=(const char *s) const
    {
    return compare(s)!=0;
    }

  inline bool operator>(const String& s) const
    {
    return compare(s)>0;
    }

  inline bool operator>(const char *s) const
    {
    return compare(s)>0;
    }

  inline bool operator>=(const String& s) const
    {
    return compare(s)>=0;
    }

  inline bool operator>=(const char *s) const
    {
    return compare(s)>=0;
    }

  inline bool operator<(const String& s) const
    {
    return compare(s)<0;
    }

  inline bool operator<(const char *s) const
    {
    return compare(s)<0;
    }

  inline bool operator<=(const String& s) const
    {
    return compare(s)<=0;
    }

  inline bool operator<=(const char *s) const
    {
    return compare(s)<=0;
    }

  inline String operator+(int n) const
    {
    return substr(n,size()-n);
    }

  inline String operator-(int n) const
    {
    return substr(0,n);
    }

  // I/O
  friend ostream& operator<<(ostream&,const String&);

  // These provide test if assigned/not assigned
  inline operator void*() const
    {
    return (void *)s;
    }

  inline bool operator!() const
    {
    if(s) return 0;
    else return 1;
    }

  // Sub-stirng
  inline String substr(int start,int size) const
    {
    String x;
    x.s=s+start;
    x.len=size;
    x.raw=raw;
    ++raw->ref;
    return x;
    }

  // Return value of failed finds
  static const int npos= -1;

  // Finds
  int find(char c,int start=0) const;
  int find_first_of(String sep,int start=0) const;
  int find_first_not_of(String sep,int start=0) const;

  // Return equivalent C string
  inline const char *c_str() // const
    {
    if(raw)
      {
      Stringraw *org=raw;
      s=copy_on_write(raw,s,len);
      dereference(org);
      return s;
      }
    else
      return "";
    }

  // Info
  inline int size() const
    {
    return len;
    }

  inline const char *data() const
    {
    return s;
    }
  };

int val(String s);
String numstr(int n);
bool getline(istream&,String&);

class Stringbuf : public streambuf
  {
  // ios::open_mode mode;
  int mode;
  char *stream;
  int stream_len;

  public:

  void stringbuf_init()
    {
    if(mode & ios::ate)
      stringbuf_sync(0,stream_len);
    else
      stringbuf_sync(0,0);
    }

  explicit Stringbuf(int which=ios::in|ios::out) :
    streambuf(), mode(which), stream(NULL), stream_len(0)
    {
    stringbuf_init();
    }

  explicit Stringbuf(const String &str, int which=ios::in|ios::out) :
    streambuf(), mode(which), stream(NULL), stream_len(0)
    {
    if(mode & (ios::in|ios::out))
      {
      stream_len = str.size();
      stream = new char[stream_len];
      str.copy(stream,stream_len);
      }
    stringbuf_init();
    }

  virtual ~Stringbuf()
    {
    delete[] stream;
    }

  String str() const
    {
    if(pbase()!= 0)
      return String(stream, pptr()-pbase());
    else
      return String();
    }

  void str(const String& str)
    {
    delete[] stream;
    stream_len = str.size();
    stream = new char[stream_len];
    str.copy(stream,stream_len);
    stringbuf_init();
    }

  protected:

   // The buffer is already in gptr, so if it ends then it is out of data.
  virtual int underflow()
    {
    return -1;
    }

  virtual int overflow(int c = -1)
    {
    int res;
    if(mode & ios::out)
      {
      if(c != -1)
        {
        streamsize old_stream_len = stream_len;
        stream_len += 1;
        char *new_stream = new char[stream_len];
        if(old_stream_len) memcpy(new_stream, stream, old_stream_len);
        delete[] stream;
        stream = new_stream;
        stringbuf_sync(gptr()-eback(), pptr()-pbase());
        sputc(c);
        res = c;
	}
      else
        res = -1;
      }
    else
      res = 0;
    return res;
    }

  streambuf *setbuf(char *s, streamsize n)
    {
    if(n != 0)
      {
      delete[] stream;
      stream = new char[n];
      memcpy(stream, s, n);
      stream_len = n;
      stringbuf_sync(0, 0);
      }
    return this;
    }

  virtual int seekoff(int off, int way /* ios::seek_dir way */, int which = ios::in | ios::out)
    {
    int ret = -1;
    int testin = which & ios::in && mode & ios::in;
    int testout = which & ios::out && mode & ios::out;
    int testboth = testin && testout && way != ios::cur;

    if(stream_len && ((testin != testout) || testboth))
      {
      char *beg = stream;
      char *curi = 0;
      char *curo = 0;
      char *endi = 0;
      char *endo = 0;

      if(testin)
        {
        curi = gptr();
        endi = egptr();
        }
      if(testout)
        {
        curo = pptr();
        endo = epptr();
        }

      int newoffi = 0;
      int newoffo = 0;
      if(way == ios::beg)
        {
        newoffi = beg - curi;
        newoffo = beg - curo;
        }
      else if(way == ios::end)
        {
        newoffi = endi - curi;
        newoffo = endo - curo;
        }

      if(testin && newoffi + off + curi - beg >= 0 && endi - beg >= newoffi + off + curi - beg)
        {
        gbump(newoffi + off);
        ret = newoffi + off;
        }
      if(testout && newoffo + off + curo - beg >= 0 && endo - beg >= newoffo + off + curo - beg)
        {
        pbump(newoffo + off);
        ret = newoffo + off;
        }
      }
    return ret;
    }

  virtual int seekpos(int sp, int which = ios::in | ios::out)
    {
    int ret = seekoff(sp, ios::beg, which);
    return ret;
    }

  private:

  void stringbuf_sync(streamsize i, streamsize o)
    {
    if(mode & ios::in)
      setg(stream, stream + i, stream + stream_len);
    if(mode & ios::out)
      {
      setp(stream, stream + stream_len);
      pbump(o);
      }
    }

  };

class iStringstream : public istream
  {
  Stringbuf sb;

  public:

  explicit iStringstream(int which=ios::in) :
    istream(&sb), sb(which | ios::in)
    {
    }

  explicit iStringstream(const String& str, int which=ios::in) :
    istream(&sb), sb(str, which | ios::in)
    {
    }

  Stringbuf *rdbuf() const
    {
    return const_cast<Stringbuf*>(&sb);
    }

  String str() const
    {
    return rdbuf()->str();
    }

  void str(const String& s)
    {
    rdbuf()->str(s);
    }
  };

class oStringstream : public ostream
  {
  Stringbuf sb;

  public:

  typedef int int_type;

  explicit oStringstream(int which=ios::out) :
    ostream(&sb), sb(which | ios::out)
    {
    }

  explicit oStringstream(const String& str, int which=ios::out) :
    ostream(&sb), sb(str, which | ios::out)
    {
    }

  Stringbuf *rdbuf() const
    {
    return const_cast<Stringbuf*>(&sb);
    }

  String str() const
    {
    return rdbuf()->str();
    }

  void str(const String& s)
    {
    rdbuf()->str(s);
    }
  };

class Stringstream : public iostream
  {
  Stringbuf sb;

  public:

  explicit Stringstream(int which=ios::out|ios::in) :
    iostream(&sb), sb(which)
    {
    }

  explicit Stringstream(const String& str, int which=ios::out|ios::in) :
    iostream(&sb), sb(str, which)
    {
    }

  Stringbuf *rdbuf() const
    {
    return const_cast<Stringbuf*>(&sb);
    }

  String str() const
    {
    return rdbuf()->str();
    }

  void str(const String& s)
    {
    rdbuf()->str(s);
    }
  };

#endif
