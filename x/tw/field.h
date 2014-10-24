// Text field

#ifndef _Ifield
#define _Ifield 1

#include "widget.h"

class Field : public Widget
  {
  String buf;				// Edit buffer
  String title;				// Title text
  Fn_0<String> *trigger;		// Called when user modifies value
  int ofst;				// Scroll offset
  int cursor;				// Cursor position

  public:

  void repos();

  void ltarw(int c);
  void rtarw(int c);
  void type(int c);
  void delch(int c);
  void backs(int c);
  void bol(int c);
  void eol(int c);
  void rtn(int c);

  void st(String val);			// Set field
  String gt();				// Get field
  void st_trigger(Fn_0<String> *);	// Set trigger function
  void sttitle(String val);		// Set title
  virtual void draw(int new_x,int new_y,int new_w,int new_h);
  virtual void focus();
  virtual void unfocus();
  Field();
  };

#endif
