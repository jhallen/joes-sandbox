// Edit buffer using doubly-linked list of gap buffers

#ifndef _Ieditbuf
#define _Ieditbuf 1

#include "link.h"
#include "string.h"

#define PGSIZE 4096		// Bytes per gap buffer

class Header;
class Pointer;

// A buffer

class Buffer
  {
  public:
  Link<Header> headers;		// List of gap buffers
  Link<Pointer> pointers;	// List of pointers on this buffer
  Pointer *bof;			// Beginning of file pointer
  Pointer *eof;			// End of file pointer
  bool changed;			// Set if buffer has been modified
  int tabwidth;			// Tab width
  Buffer();			// Create an empty buffer
  Buffer(Link<Header> text,long amnt,long nlines);
  void iz(Link<Header> text,long amnt,long nlines);
  				// Create a buffer from link list of gap buffers
  };

// A gap buffer

struct Header
  {
  public:
  Link<Header> link;		// Doubly linked list of headers
  int hole;			// Offset to hole
  int ehole;			// Offset to after hole
  int nlines;			// No. lines in this gap buffer
  char buf[PGSIZE];		// Gap buffer
  Header();			// Create an empty header
  int size();			// Return no. bytes in header
  int space();			// Return no. bytes of free space
  void stgap(int ofst);		// Set gap position
  void insert(int ofst,const char *s,int len);
  				// Insert
  void del(int ofst,int len);
  				// Delete
  void copy(int ofst,char *s,int len);
  				// Copy to array
  int rc(int ofst);		// Get character at offset or -1 if ofst==size()
  };

// A pointer on a buffer

struct Pointer
  {
  public:
  Link<Pointer> link;		// Doubly linked list of pointers

  Buffer *b;			// Buffer this pointer points in
  int ofst;			// Offset into current page
  Header *hdr;			// Page

  long byte;			// Byte offset
  long line;			// Line number
  long col;			// Column number
  long xcol;			// Set if we're past end of line
  bool valcol;			// Set if 'col' is valid
  bool end;			// Set if this is the eof Pointer

  // Utilities
  void coalesce();		// Merge small blocks together
  bool next();			// Move pointer to beginning of next buffer.  Return 1 for success, 0 if we
  				// were in last one.
  bool prev();			// Move pointer to after end of previous buffer (an illegal position).  Return 1 for
  				// success, or 0 if were in first one.
  void fixcol();		// Fix column number (used by gtcol())

  // Utilities for insert
  void split();
  void inschn(Link<Header>);
  void fixupins(long amnt,long nlines,Header *hdr,int hdramnt);

  // Utilities for delete
  Buffer *cut(Pointer *to);

  // Utilities for fast search functions
  void ffwrd(int n);
  void fbkwd(int n);
  int fpgetc();
  int frgetc();
  void getto(Pointer *);
  void rgetto(Pointer *);
  bool ffind(char *,int);
  bool fifind(char *,int);
  bool frfind(char *,int);
  bool frifind(char *,int);

  // Queries
  bool isbof();			// Set if we're at beginning of file
  bool iseof();			// Set if we're at end of file
  bool isbol();			// Set if we're at beginning of line
  bool iseol();			// Set if we're at end of line
  bool isbow();			// Set if we're at beginning of word
  bool iseow();			// Set if we're at end of word
  bool isblank();		// True if line only containes whitespace

  long gtindent();		// Get indendation level (column no. of first non-white char) of this line

  inline long gtcol()		// Get current column no.
    {
    if(!valcol) fixcol();
    return col;
    }

  // Goto
  void bof();			// Set to beginning of file
  void eof();			// Set to end of file
  void eol();			// Go to end of line
  void bol();			// Go to beginning of line
  bool nextl();			// Go to beginning of next line
  bool prevl();			// Go to end of previous line
  bool goto_line(long n);	// Go to beginning of line no.
  void goto_col(long goalcol);	// Move pointer so that it's column is <=goalcol
  void coli(long goalcol);	// Move pointer so that it's column is >=goalcol (or <goalcol for short line)
  void colwse(long goalcol);	// Move to column number, and then skip whitespace backwards
  int getc();			// Get character and advance.  Returns -1 if we're at eof.
  int rgetc();			// Get previous character.  Returns -1 if we're at bof.
  bool fwrd(long n);		// Move forward n bytes.  Returns true for success.
  bool bkwd(long n);		// Move backward n bytes
  void goto_byte(long n);	// Go to byte

  // Read
  void copy(char *blk,int n);	// Read array
  int rc();			// Read character
  String rs(int n);		// Read String
  Buffer *rb(Pointer *to);	// Read range into a new buffer

  // Insert
  void insb(Buffer *b);		// Insert a buffer
  void insm(const char *blk,int n);	// Insert an array
  void insc(char c);		// Insert a character
  void inss(String s);		// Insert a String
  void fill(long to,int usetabs);
  				// Generate whitespace to extend line to column 'to'

  // Delete
  void del(Pointer *to);	// Delete between here and to
  void backws();		// Delete whitespace backwards

  // Search
  bool find(char *s,int n);	// Search for string
  bool ifind(char *s,int n);	// Search case independent
  bool rfind(char *s,int n);	// Reverse search for string
  bool rifind(char *s,int n);	// Reverse search, case independent

  // Creation/deletion
  void set(Pointer *);		// Set pointer
  Pointer *clone();		// Duplicate pointer
  void rm();			// Delete a pointer

  Pointer();			// Create en empty pointer
  };

#endif
