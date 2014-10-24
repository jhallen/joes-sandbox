// Text editor window

#ifndef _Iedit
#define _Iedit 1

#include "widget.h"
#include "editbuf.h"

class Edit : public Widget
  {
  Buffer buf;				// Edit buffer
  String title;				// Title text

  Pointer *cursor;			// Cursor
  Pointer *top;				// Pointer to first line on screen
  long loffset;				// Left scrolling offset

  public:

  void repos();				// Called after edit functions to update cursor

  // User edit functions

  void ltarw(int c);
  void rtarw(int c);
  void uparw(int c);
  void dnarw(int c);
  void type(int c);
  void delch(int c);
  void dellin(int c);
  void backs(int c);
  void bol(int c);
  void eol(int c);
  void rtn(int c);

  void st(String val);			// Set entire edit buffer to val
  String gt();				// Get edit buffer as String
  void sttitle(String val);		// Set title
  virtual void draw(int new_x,int new_y,int new_w,int new_h);
  virtual void focus();
  virtual void unfocus();
  Edit();
  };

#endif
