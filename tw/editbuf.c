#include "util.h"
#include "editbuf.h"

// Header

Header::Header()
  {
  hole=0;
  ehole=PGSIZE;
  nlines=0;
  }

int Header::space()
  {
  return ehole-hole;
  }

int Header::size()
  {
  return PGSIZE-(ehole-hole);
  }

void Header::stgap(int ofst)
  {
  if(ofst>hole)
    memmove(buf+hole,buf+ehole,ofst-hole);
  else if(ofst<hole)
    memmove(buf+ehole-(hole-ofst),buf+ofst,hole-ofst);
  ehole=ofst+ehole-hole;
  hole=ofst;
  }

void Header::insert(int ofst,const char *s,int len)
  {
  if(ofst!=hole) stgap(ofst);
  memcpy(buf+hole,s,len);
  hole+=len;
  }

void Header::del(int ofst,int len)
  {
  if(ofst!=hole) stgap(ofst);
  ehole+=len;
  }

void Header::copy(int ofst,char *s,int len)
  {
  if(ofst<hole)
    if(len>hole-ofst)
      {
      memcpy(s,buf+ofst,hole-ofst);
      memcpy(s+hole-ofst,buf+ehole,len-(hole-ofst));
      }
    else
      memcpy(s,buf+ofst,len);
  else
    memcpy(s,buf+ofst+ehole-hole,len);
  }

int Header::rc(int ofst)
  {
  if(ofst==size()) return -1;
  else if(ofst>=hole) return ((unsigned char *)buf)[ofst+ehole-hole];
  else return ((unsigned char *)buf)[ofst];
  }

// Pointer

void Pointer::bof()
  {
  set(b->bof);
  }

void Pointer::eof()
  {
  set(b->eof);
  }

bool Pointer::isbof()
  {
  return hdr==b->bof->hdr && !ofst;
  }

bool Pointer::iseof()
  {
  return ofst==hdr->size();
  }

bool Pointer::iseol()
  {
  int c;
  if(iseof()) return 1;
  c=rc();
  if(c=='\n') return 1;
  return 0;
  }

bool Pointer::isbol()
  {
  int c;
  if(isbof()) return 1;
  c=rgetc(); getc();
  if(c=='\n') return 1;
  else return 0;
  }

bool Pointer::isbow()
  {
  Pointer *q=clone();
  int c=rc();
  int d=q->rgetc();
  q->rm();
  if(crest(c) && !crest(d)) return 1;
  else return 0;
  }

bool Pointer::iseow()
  {
  Pointer *q=clone();
  int d=q->rc();
  int c=q->rgetc();
  q->rm();
  if(crest(c) && !crest(d)) return 1;
  else return 0;
  }

bool Pointer::isblank()
  {
  Pointer *q=clone();
  q->bol();
  while(cwhite(q->rc())) q->getc();
  if(q->iseol())
    {
    q->rm();
    return 1;
    }
  else
    {
    q->rm();
    return 0;
    }
  }

long Pointer::gtindent()
  {
  Pointer *q=clone();
  long col;
  q->bol();
  while(cwhite(q->rc())) q->getc();
  col=q->col;
  q->rm();
  return col;
  }

bool Pointer::next()
  {
  if(hdr==b->eof->hdr)
    {
    ofst=hdr->size();
    return 0;
    }
  hdr=hdr->link.next;
  ofst=0;
  return 1;
  }

bool Pointer::prev()
  {
  if(hdr==b->bof->hdr)
    {
    ofst=0;
    return 0;
    }
  hdr=hdr->link.prev;
  ofst=hdr->size();
  return 1;
  }

int Pointer::getc()
  {
  int c=rc();
  if(c==-1) return -1;
  if(++ofst==hdr->size()) next();
  ++byte;
  if(c=='\n') 
    {
    ++line;
    col=0;
    valcol=1;
    }
  else
    {
    if(c=='\t') col+=b->tabwidth-col%b->tabwidth;
    else ++col;
    }
  return c;
  }

bool Pointer::fwrd(long n)
  {
  if(!n) return 1;
  valcol=0;
  do
    {
    if(ofst==hdr->size())
      do
        {
        if(!ofst)
          {
          byte+=hdr->size();
          n-=hdr->size();
          line+=hdr->nlines;
          }
        if(!next()) return 0;
        }
        while(n>hdr->size());
    if(rc()=='\n') ++line;
    ++byte;
    ++ofst;
    }
    while(--n);
  if(ofst==hdr->size()) next();
  return 1;
  }

int Pointer::rgetc()
  {
  int c;
  if(!ofst && !prev()) return -1;
  --ofst;
  c=rc();
  --byte;
  if(c=='\n')
    {
    --line;
    valcol=0;
    }
  else
    {
    if(c=='\t') valcol=0;
    --col;
    }
  return c;
  }

bool Pointer::bkwd(long n)
  {
  if(!n) return 1;
  valcol=0;
  do
    {
    if(!ofst)
      do
        {
        if(ofst)
          {
          byte-=ofst;
          n-=ofst;
          line-=hdr->nlines;
          }
        if(!prev()) return 0;
        }
        while(n>hdr->size());
    --ofst;
    --byte;
    if(rc()=='\n') --line;
    }
    while(--n);
  return 1;
  }

void Pointer::goto_byte(long loc)
  {
  if(loc>byte) fwrd(loc-byte);
  else if(loc<byte) bkwd(byte-loc);
  }

void Pointer::fixcol()
  {
  Header *h=hdr;
  int o=ofst;
  bol();
  while(ofst!=o || hdr!=h) getc();
  }

void Pointer::bol()
  {
  if(prevl()) getc();
  col=0;
  valcol=1;
  }

void Pointer::eol()
  {
  while(ofst!=hdr->size())
    {
    int c=rc();
    if(c=='\n') break;
    else
      {
      ++byte;
      ++ofst;
      if(c=='\t') col+=b->tabwidth-col%b->tabwidth;
      else ++col;
      if(ofst==hdr->size()) next();
      }
    }
  }

bool Pointer::nextl()
  {
  int c;
  do
    {
    if(ofst==hdr->size())
      do
        {
        byte+=hdr->size()-ofst;
        if(!next()) return 0;
        }
        while(!hdr->nlines);
    c=rc();
    ++byte;
    ++ofst;
    }
    while(c!='\n');
  ++line;
  col=0;
  valcol=1;
  if(ofst==hdr->size()) next();
  return 1;
  }

bool Pointer::prevl()
  {
  int c;
  valcol=0;
  do
    {
    if(!ofst)
      do
       {
       byte-=ofst;
       if(!prev()) return 0;
       }
       while(!hdr->nlines);
    --ofst;
    --byte;
    c=rc();
    }
    while(c!='\n');
  --line;
  return 1;
  }

bool Pointer::goto_line(long toline)
  {
  if(toline>b->eof->line)
    {
    set(b->eof);
    return 0;
    }
  // Pick a good starting point
  if(toline<Labs(line-toline))
    set(b->bof);
  if(Labs(b->eof->line-toline)<Labs(line-toline))
    set(b->eof);

  if(line==toline)
    {
    bol();
    return 1;
    }
  while(toline>line) nextl();
  if(toline<line)
    {
    while(toline<line) prevl();
    bol();
    }
  return 1;
  }

void Pointer::goto_col(long goalcol)
  {
  bol();
  do
    {
    int c;
    int wid;
    c=rc();
    if(c=='\n') break;
    if(c==-1) break;
    if(c=='\t') wid=b->tabwidth-col%b->tabwidth;
    else wid=1;
    if(col+wid>goalcol) break;
    if(++ofst==hdr->size()) next(); 
    ++byte;
    col+=wid;
    }
    while(col!=goalcol);
  }

void Pointer::colwse(long goalcol)
  {
  int c;
  goto_col(goalcol);
  do
    c=rgetc();
    while(c==' ' || c=='\t');
  if(c!=-1) getc();
  }

void Pointer::coli(long goalcol)
  {
  bol();
  while(col<goalcol)
    {
    int c=rc();
    if(c==-1) break;
    if(c=='\n') break;
    if(c=='\t') col+=b->tabwidth-col%b->tabwidth;
    else ++col;
    if(++ofst==hdr->size()) next(); 
    ++byte;
    }
  }

// Read

int Pointer::rc()
  {
  return hdr->rc(ofst);
  }

void Pointer::copy(char *blk,int len)
  {
  char *bk=blk;
  Pointer *np=clone();
  int amnt;
  while(len>(amnt=hdr->size()-np->ofst))
   {
   np->hdr->copy(np->ofst,bk,amnt);
   bk+=amnt;
   len-=amnt;
   np->next();
   }
  if(len) np->hdr->copy(np->ofst,bk,len);
  np->rm();
  }

String Pointer::rs(int len)
  {
  String rtn(len);
  char *s=(char *)rtn.data();
  copy(s,len);
  return rtn;
  }

Buffer *Pointer::rb(Pointer *to)
  {
  Link<Header> headers;
  if(byte<to->byte)
    {
    Header *l;
    Pointer *q=clone();
    if(q->hdr==to->hdr)
      { // Single
      l=new Header();
      if(q->ofst!=q->hdr->hole) q->hdr->stgap(q->ofst);
      l->nlines=mcnt(q->hdr->buf+q->hdr->ehole,'\n',l->hole=to->ofst-q->ofst);
      if(l->hole) memcpy(l->buf,q->hdr->buf+q->hdr->ehole,l->hole);
      headers.push_back(l);
      }
    else
      {
      // First
      l=new Header();
      if(q->ofst!=q->hdr->hole) q->hdr->stgap(q->ofst);
      l->nlines=mcnt(q->hdr->buf+q->hdr->ehole,'\n',l->hole=PGSIZE-q->hdr->ehole);
      memcpy(l->buf,q->hdr->buf+q->hdr->ehole,l->hole);
      headers.push_back(l);
      q->next();

      // Middle
      while(q->hdr!=to->hdr)
        {
        l=new Header();
        l->nlines=q->hdr->nlines;
        memcpy(l->buf,q->hdr->buf,q->hdr->hole);
        memcpy(l->buf+q->hdr->hole,q->hdr->buf+q->hdr->ehole,PGSIZE-q->hdr->ehole);
        l->hole=q->hdr->size();
        headers.push_back(l);
        q->next();
        }

      // Last
      if(to->ofst)
        {
        l=new Header();
        if(to->ofst!=to->hdr->hole) to->hdr->stgap(to->ofst);
        l->nlines=mcnt(to->hdr->buf,'\n',to->ofst);
        memcpy(l->buf,to->hdr->buf,l->hole=to->ofst);
        headers.push_back(l);
        }
      }
    q->rm();
    }
  else
    {
    headers.push_back(new Header());
    }
  return new Buffer(headers,to->byte-byte,to->line-line);
  }

// Search

// Move pointer to q, which has an invalid line number

void Pointer::getto(Pointer *q)
  {
  while(hdr!=q->hdr || ofst!=q->ofst)
    {
    if(rc()=='\n') ++line;
    ++byte;
    ++ofst;
    if(ofst==hdr->size())
      next();
    while(!ofst && hdr!=q->hdr)
      {
      byte+=hdr->size();
      line+=hdr->nlines;
      next();
      }
    }
  }

void Pointer::rgetto(Pointer *q)
  {
  while(hdr!=q->hdr || ofst!=q->ofst)
    {
    if(!ofst)
      do
        {
        if(ofst)
          {
          byte-=ofst;
          line-=hdr->nlines;
          }
        prev();
        }
        while(hdr!=q->hdr);
    --ofst;
    --byte;
    if(rc()=='\n') --line;
    }
  }

void Pointer::ffwrd(int n)
  {
  while(n>hdr->size()-ofst)
    {
    n-=hdr->size()-ofst;
    if(!next()) return;
    }
  ofst+=n;
  if(ofst==hdr->size())
    next();
  }

int Pointer::frgetc()
  {
  if(ofst || prev())
    {
    --ofst;
    return rc();
    }
  else
    return -1;
  }

bool Pointer::ffind(char *s,int len)
  {
  long amnt=b->eof->byte-byte;
  int x;
  int table[256];
  int c;
  if(len>amnt) return 0;
  if(!len) return 1;
  valcol=0;
  for(x=0;x!=256;++x) table[x]= -1;
  for(x=0;x!=len-1;++x) table[((unsigned char *)s)[x]]=x;
  ffwrd(len); amnt-=len; x=len; do
    if((c=frgetc())!=((unsigned char *)s)[--x])
      {
      if(table[c]==-1) ffwrd(len+1), amnt-=x+1;
      else if(x<=table[c]) ffwrd(len-x+1), --amnt;
      else ffwrd(len-table[c]), amnt-=x-table[c];
      if(amnt<0) return 0;
      else x=len;
      }
    while(x);
  return 1;
  }

bool Pointer::find(char *s,int len)
  {
  Pointer *q=clone();
  if(q->ffind(s,len))
    {
    getto(q);
    q->rm();
    return 1;
    }
  else
    {
    q->rm();
    return 0;
    }
  }

bool Pointer::fifind(char *s,int len)
  {
  long amnt=b->eof->byte-byte;
  int x;
  int table[256];
  int c;
  if(len>amnt) return 0;
  if(!len) return 1;
  valcol=0;
  for(x=0;x!=256;++x) table[x]= -1;
  for(x=0;x!=len-1;++x) table[((unsigned char *)s)[x]]=x;
  ffwrd(len); amnt-=len; x=len; do
    if((c=toup(frgetc()))!=((unsigned char *)s)[--x])
      {
      if(table[c]==-1) ffwrd(len+1), amnt-=x+1;
      else if(x<=table[c]) ffwrd(len-x+1), --amnt;
      else ffwrd(len-table[c]), amnt-=x-table[c];
      if(amnt<0) return 0;
      else x=len;
      }
    while(x);
  return 1;
  }

bool Pointer::ifind(char *s,int len)
  {
  Pointer *q=clone();
  if(q->fifind(s,len))
    {
    getto(q);
    q->rm();
    return 1;
    }
  else
    {
    q->rm();
    return 0;
    }
  }

void Pointer::fbkwd(int n)
  {
  while(n>ofst)
    {
    n-=ofst;
    if(!prev()) return;
    }
  if(ofst>=n) ofst-=n;
  else ofst=0;
  }

int Pointer::fpgetc()
  {
  int c=rc();
  if(c!=-1)
    if(++ofst==hdr->size())
      next();
  return c;
  }

bool Pointer::frfind(char *s,int len)
  {
  long amnt=byte;
  int x;
  int c;
  int table[256];
  if(len>b->eof->byte-byte)
    {
    x=len-(b->eof->byte-byte);
    if(amnt<x) return 0;
    amnt-=x;
    fbkwd(x);
    }
  if(!len) return 1;
  valcol=0;
  for(x=0;x!=256;++x) table[x]=255;
  for(x=len;--x;table[((unsigned char *)s)[x]]=len-x-1);
  x=0; do
    if((c=fpgetc())!=((unsigned char *)s)[x++])
      {
      if(table[c]==255) fbkwd(len+1), amnt-=len-x+1;
      else if(len-table[c]<=x) fbkwd(x+1), --amnt;
      else fbkwd(len-table[c]), amnt-=len-table[c]-x;
      if(amnt<0) return 0;
      else x=0;
      }
    while(x!=len);
  fbkwd(len);
  return 1;
  }

bool Pointer::rfind(char *s,int len)
  {
  Pointer *q=clone();
  if(q->frfind(s,len))
    {
    rgetto(q);
    q->rm();
    return 1;
    }
  else
    {
    q->rm();
    return 0;
    }
  }

bool Pointer::frifind(char *s,int len)
  {
  long amnt=byte;
  int x;
  int c;
  int table[256];
  if(len>b->eof->byte-byte)
    {
    x=len-(b->eof->byte-byte);
    if(amnt<x) return 0;
    amnt-=x;
    fbkwd(x);
    }
  if(!len) return 1;
  valcol=0;
  for(x=0;x!=256;++x) table[x]=255;
  for(x=len;--x;table[((unsigned char *)s)[x]]=len-x-1);
  x=0; do
    if((c=toup(fpgetc()))!=((unsigned char *)s)[x++])
      {
      if(table[c]==255) fbkwd(len+1), amnt-=len-x+1;
      else if(len-table[c]<=x) fbkwd(x+1), --amnt;
      else fbkwd(len-table[c]), amnt-=len-table[c]-x;
      if(amnt<0) return 0;
      else x=0;
      }
    while(x!=len);
  fbkwd(len);
  return 1;
  }

bool Pointer::rifind(char *s,int len)
  {
  Pointer *q=clone();
  if(q->frifind(s,len))
    {
    rgetto(q);
    q->rm();
    return 1;
    }
  else
    {
    q->rm();
    return 0;
    }
  }

// Insert

// Build a chain from a memory block

Link<Header> bldchn(const char *blk,int size,long& nlines)
  {
  Link<Header> anchor;
  Header *l;
  nlines=0;
  do
    {
    char *ptr;
    int amnt;
    l=new Header();
    if(size>PGSIZE) amnt=PGSIZE;
    else amnt=size;
    if(amnt) memcpy(l->buf,blk,amnt);
    l->hole=amnt;
    l->nlines=mcnt(l->buf,'\n',amnt);
    nlines+=l->nlines;
    anchor.push_back(l);
    blk+=amnt;
    size-=amnt;
    }
    while(size);
  return anchor;
  }

// Split a block at p's ofst
// p is placed in the new block such that it points to the same text but with
// p->ofst==0

void Pointer::split()
  {
  if(ofst)
    {
    Header *l;
    Pointer *pp;

    l=new Header();

    if(ofst!=hdr->hole) hdr->stgap(ofst);

    memcpy(l->buf,hdr->buf+hdr->ehole,PGSIZE-hdr->ehole);
    l->hole=PGSIZE-hdr->ehole;
    l->nlines=mcnt(l->buf,'\n',hdr->hole);
    hdr->nlines-=l->nlines;
    hdr->ehole=PGSIZE;

    b->headers.insert_after(hdr,l);

    for(pp=b->pointers.next;pp;pp=pp->link.next)
      if(pp!=this && pp->hdr==hdr && pp->ofst>=ofst)
        {
        pp->hdr=hdr;
        pp->ofst-=ofst;
        }

    hdr=l;
    ofst=0;
    }
 }

// Insert a chain

void Pointer::inschn(Link<Header> a)
  {
  if(!b->eof->byte)
    { // Buffer is empty: replace the empty segment in p with a
    hdr=a.next;
    b->headers=a;
    b->bof->set(this);
    b->eof->hdr=b->headers.prev;
    b->eof->ofst=b->headers.prev->size();
    }
  else if(iseof())
    { // We're at the end of the file: append a to the file
    hdr=a.next;
    ofst=0;
    b->headers.push_back(a);
    b->eof->hdr=b->headers.prev;
    b->eof->ofst=b->headers.prev->size();
    }
  else if(isbof())
    { // We're at the beginning of the file: insert chain and set bof pointer
    hdr=b->headers.insert_before(hdr,a);
    b->bof->set(this);
    }
  else
    { // We're in the middle of the file: split and insert
    split();
    hdr=b->headers.insert_before(hdr,a);
    }
  }

void Pointer::fixupins(long amnt,long nlines,Header *hdr,int hdramnt)
  {
  Pointer *pp;

  // We inserted line(s) so scroll
  // if(nlines && !isbol()) scrins(b,line,nlines,1);
  // else scrins(b,line,nlines,0);

  // What is this for?
  // inserr(b->name,line,nlines);

  // Invalidate column numbers
  for(pp=b->pointers.next;pp;pp=pp->link.next)
    if(pp!=this && pp->line==line && (pp->byte>byte || pp->end && pp->byte==byte)) pp->valcol=0;

  // Update line numbers of byte counts
  for(pp=b->pointers.next;pp;pp=pp->link.next)
    if(pp!=this)
      if(pp->byte==byte && !pp->end)
        pp->set(this);
      else if(pp->byte>byte || pp->end && pp->byte==byte)
        {
        pp->byte+=amnt;
        pp->line+=nlines;
        if(pp->hdr==hdr) pp->ofst+=hdramnt;
        }

  // Undo mark
  // if(b->undo) b->undo->undoins(this,amnt);

  // Buffer has been changed
  b->changed=1;
  }

void Pointer::insb(Buffer *b)
  {
  if(b->eof->byte)
    {
    Pointer *q=clone();
    q->inschn(b->headers);
    q->fixupins(b->eof->byte,b->eof->line,NULL,0);
    q->coalesce();
    q->rm();
    }
  delete b;
  }

void Pointer::insm(const char *blk,int amnt)
  {
  long nlines;
  Header *h=0;
  int hdramnt;
  Pointer *q;
  if(!amnt) return;
  q=clone();
  if(amnt<=q->hdr->space())
    {
    h=q->hdr;
    hdramnt=amnt;
    h->insert(q->ofst,blk,amnt);
    nlines=mcnt(blk,'\n',amnt);
    q->hdr->nlines+=nlines;
    }
  else if(!q->ofst && q->hdr!=q->b->bof->hdr && amnt<=q->hdr->link.prev->space())
    {
    q->prev();
    q->hdr->insert(q->ofst,blk,amnt);
    nlines=mcnt(blk,'\n',amnt);
    q->hdr->nlines+=nlines;
    }
  else
    {
    Link<Header> x = bldchn(blk,amnt,nlines);
    q->inschn(x);
    }
  q->fixupins((long)amnt,nlines,h,hdramnt);
  q->coalesce();
  q->rm();
  }

void Pointer::inss(String s)
  {
  insm(s.data(),s.size());
  }

void Pointer::insc(char c)
  {
  insm(&c,1);
  }

void Pointer::fill(long to,int usetabs)
  {
  gtcol();
  if(usetabs)
    while(col<to)
      if(col+b->tabwidth-col%b->tabwidth<=to)
        insc('\t'), getc();
      else
        insc(' '), getc();
  else
    while(col<to)
      insc(' '), getc();
  }

// Delete

/* Delete the text between two pointers from a buffer and return it in a new
 * buffer.
 *
 * This routine calls these functions:
 *  gstgap	- to position gaps
 *  halloc	- to allocate new header/segment pairs
 *  vlock	- virtual memory routines
 *  vunlock
 *  vchanged
 *  vupcount
 *  mcpy	- to copy deleted text
 *  mcnt	- to count NLs
 *  snip	- queue routines
 *  enqueb
 *  splicef
 *  scrdel	- to tell screen update to scroll when NLs are deleted
 *  bmkchn	- to make a buffer out of a chain
 */

// This is only to be used for Pointer::del()

Buffer *Pointer::cut(Pointer *to)
  {
  Link<Header> text;		// Deleted text
  Header *i;
  Pointer *p;
  long nlines;			// No. EOLs to delete
  long amnt;			// No. bytes to delete
  int toamnt;			// Amount to delete from segment in 'to'
  int bofmove=0;		// Set if bof got deleted
 
  if(!(amnt=to->byte-byte))
    return 0;			// ...nothing to delete
 
  nlines=to->line-line;
 
  if(hdr==to->hdr)
    { // Delete is within a single segment
    // Move gap to deletion point
    if(ofst!=hdr->hole) hdr->stgap(ofst);
 
    // Store the deleted text
    i=new Header();
    memcpy(i->buf,hdr->buf+hdr->ehole,amnt);
    i->hole=amnt;
    i->nlines=nlines;

    text.push_back(i);
 
    // Delete
    hdr->ehole+=amnt;
    hdr->nlines-=nlines;
 
    toamnt=amnt;
    }
  else
    { // Delete crosses segments
    Header *a;
    Header *h; // Text deleted from beginning
    Header *i;
    if(toamnt=to->ofst)
      {
      Header *i;
      // Delete beginning of to
      // Move gap to deletion point
      // To could be deleted if it's at the end of the file
      if(to->ofst!=to->hdr->hole) to->hdr->stgap(to->ofst);
  
      // Save deleted text
      i=new Header();
      memcpy(i->buf,to->hdr->buf,to->hdr->hole);
      i->hole=to->hdr->hole;
      i->nlines=mcnt(to->hdr->buf,'\n',to->hdr->hole);
 
      // Delete
      to->hdr->nlines-=i->nlines;
      to->hdr->hole=0;
      }
    else
      i=0;
 
    // Delete end of from
    if(!ofst)
      {
      // ..unless from needs to be deleted too
      a=hdr;
      if(!a->link.prev) bofmove=1;
      h=0;
      }
    else
      {
      a=hdr->link.next;

      // Move gap to deletion point
      if(ofst!=hdr->hole) hdr->stgap(ofst);
 
      // Save deleted text
      h=new Header();
      memcpy(h->buf,hdr->buf+hdr->ehole,PGSIZE-hdr->ehole);
      h->hole=PGSIZE-hdr->ehole;
      h->nlines=mcnt(hdr->buf,'\n',h->hole);
 
      // Delete
      hdr->nlines-=h->nlines;
      hdr->ehole=PGSIZE;
      }
 
    // Make from point to header/segment of to
    hdr=to->hdr;
    ofst=0;
 
    // Chain deleted text together
    if(h) text.push_back(h);
    
    // Delete headers/segments between a and to->hdr (if there are any)
    if(a!=to->hdr)
      {
      Link<Header> tmp;
      b->headers.snip(tmp,a,to->hdr->link.prev);
      text.push_back(tmp);
      }

    if(i) text.push_back(i);
    }

  // If to is empty, then it must have been at the end of the file.  If
  // the file did not become empty, delete to
  if(!to->hdr->size() && byte)
    {
    Header *ph=hdr->link.prev;
    delete b->headers.deque(hdr);
    hdr=ph;
    ofst=hdr->size();
    b->eof->hdr=hdr;
    b->eof->ofst=ofst;
    }
 
  if(bofmove)
    b->bof->set(this);

  // The deletion is now done
 
  // Scroll if necessary

  if(nlines && !isbol())
    {
    // scrdel(b,line,nlines,1);
    // delerr(b->name,line,nlines,0);
    }
  else
    {
    // scrdel(b,line,nlines,0);
    // delerr(b->name,line,nlines,1);
    }
 
  // Fix pointers

  // Invalidate column numbers
  for(p=b->pointers.next;p;p=p->link.next)
    if(p->line==line && p->byte>byte)
      p->valcol=0; 

  // Fix line numbers, byte numbers and offsets
  for(p=b->pointers.next;p;p=p->link.next)
    if(p!=this)
      if(p->byte>=byte)
        if(p->byte<=byte+amnt)
          p->set(this);
        else
          {
          if(p->hdr==to->hdr) p->ofst-=toamnt;
          p->byte-=amnt;
          p->line-=nlines;
          }

  coalesce();

  // Make buffer out of deleted text and return it
 
  return new Buffer(text,amnt,nlines);
  }

void Pointer::del(Pointer *to)
  {
  if(to->byte-byte)
    {
    Buffer *bb=cut(to);
    // if(b->undo) b->undo->undodel(byte,bb);
    // else
    delete bb;
    b->changed=1;
    }
  }

void Pointer::backws()
  {
  int c;
  Pointer *q=clone();
  do
    c=q->rgetc();
    while(c==' ' || c=='\t');
  if(c!=-1) q->getc();
  q->del(this);
  q->rm();
  }

// Cleanup

// Coalesce small blocks into a single larger one

void Pointer::coalesce()
  {
  if(hdr!=b->eof->hdr && hdr->size()+hdr->link.next->size()<=PGSIZE-PGSIZE/4)
    {
    Header *l=hdr->link.next; // We're going to delete this one
    int osize=hdr->size();
    int size=l->size();
    Pointer *q;
    l->stgap(size);
    hdr->insert(osize,l->buf,size);
    hdr->nlines+=l->nlines;
    delete b->headers.deque(l);

    for(q=b->pointers.next;q;q=link.next)
      if(q->hdr==l)
        {
        q->hdr=hdr;
        q->ofst+=osize;
        }
    }
  if(hdr!=b->bof->hdr && hdr->size()+hdr->link.prev->size()<=PGSIZE-PGSIZE/4)
    {
    Header *l=hdr->link.prev;
    int size=l->size();
    Pointer *q;
    l->stgap(size);
    hdr->insert(0,l->buf,size);
    hdr->nlines+=l->nlines;
    delete b->headers.deque(l);
    for(q=b->pointers.next;q;q=q->link.next)
      if(q->hdr==l)
        q->hdr=hdr;
      else if(q->hdr==hdr)
        q->ofst+=size;
    }
  }

// Create/Delete/Copy

// Pointers had better both belong to same buffer

void Pointer::set(Pointer* to)
  {
  b=to->b;
  ofst=to->ofst;
  hdr=to->hdr;
  byte=to->byte;
  line=to->line;
  col=to->col;
  xcol=to->xcol;
  valcol=to->valcol;
  }

Pointer *Pointer::clone()
  {
  Pointer *p=new Pointer();
  p->set(this);
  if(end) b->pointers.insert_before(this,p);
  else b->pointers.insert_after(this,p);
  return p;
  }

void Pointer::rm()
  {
  if(b) delete b->pointers.deque(this);
  }

Pointer::Pointer()
  {
  b=0;
  ofst=0;
  hdr=0;
  byte=0;
  line=0;
  col=0;
  xcol=0;
  valcol=1;
  end=0;
  }

// Buffer

void Buffer::iz(Link<Header> text,long amnt,long nlines)
  {
  headers=text;

  tabwidth=8;
  changed=0;

  eof=new Pointer();
  bof=new Pointer();

  pointers.push_back(eof);
  pointers.push_front(bof);

  bof->b=this;
  bof->hdr=headers.next;

  eof->b=this;
  eof->hdr=headers.prev;
  eof->ofst=eof->hdr->size();
  eof->byte=amnt;
  eof->line=nlines;
  eof->valcol=0;
  eof->end=1;

  bof->coalesce();
  eof->coalesce();
  }

Buffer::Buffer(Link<Header> text,long amnt,long nlines)
  {
  iz(text,amnt,nlines);
  }

Buffer::Buffer()
  {
  Link<Header> text;
  text.push_back(new Header());
  iz(text,0,0);
  }
