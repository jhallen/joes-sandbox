#ifndef _Ilist
#define _Ilist 1

#include "widget.h"
#include "view.h"

class List : public Widget
  {
  int top;		// First line of window
  int left;		// First column of windows
  int row;		// Cursor row
  int col;		// Cursor column
  int row_prev;		// Previous cursor position
  int col_prev;
  int *start;		// Starting column numbers
  int *widths;		// Column widths
  int emode;		// Set if we're editing
  int eplusminus;	// Type of edit
  int incflag;		// Column zero is autoincrementing
  int nextno;		// Next number to use
  bool no_scroll;
  String ebuf;		// Edit buffer
  View *v;		// View this window is on

  public:

  void repos();		// Scroll if necessary and move cursor
  void redraw_edit();	// Force redraw of cell at cursor

  virtual void draw(int new_x,int new_y,int new_w,int new_h);

  void type(int c);
  void sub(int c);
  void date(int c);
  void add(int c);
  void backs(int c);
  void dellin(int c);
  void pgdn(int c);
  void pgup(int c);
  void eol(int c);
  void bol(int c);
  void bof(int c);
  void eof(int c);
  void ltarw(int c);
  void rtarw(int c);
  void uparw(int c);
  void dnarw(int c);
  void ufocusnext(int c);

  void echk(void);

  void st(View *new_v);

  List();
  ~List();
  };

#endif
