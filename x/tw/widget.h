// Text windowing system

#ifndef _Iwidget
#define _Iwidget 1

#include <list>
#include <vector>
using namespace std;
#include "link.h"
#include "scrn.h"
#include "pmgr.h"
#include "fn.h"
#include "kbd.h"

typedef int Gc;				// Graphics context is just an int for text windows

// A screen

class Screen
  {
  public:
  Scrn scrn;				// The screen
  int *scrn_buf;			// Screen buffer
  int cur_y, cur_x;			// Current cursor position (Window with focus has cursor)
  bool needs_redraw;			// Set if any windows need to be redrawn or reclipped

  Widget *root;				// Root widget representing entire screen

  void update_dirty(Widget *);		// Recursively send draw events to dirty widgets
  void update();			// Update the screen (call this before getting next char)

  Gc gtstdbknd() const { return ' '; }
  Gc gtstdfgnd() const { return 0; }

  Widget *focus;			// Widget with focus
  Widget *focus_rtn;			// Widget with return key focus

  void keypress(int c);			// Handle keypress
  void focuspos(Widget *w);		// Record focus path
  Widget *focusnext(Widget *w);		// Advance focus
  Widget *focusprev(Widget *w);
  Widget *gtfocus();			// Return Widget with focus
  void clfocus();			// Nobody has focus
  void stfocus(Widget *);		// Transfer focus to a specific widget
  void stfocus_rtn(Widget *);		// Set return key focus

  Kmap< void (Widget::*)(int) > *gtstdkmap();
  Kmap< void (Widget::*)(int) > stdkmap;	// Standard keymap

  Screen();
  ~Screen();
  };

extern Screen screen;			// Main screen

// A point

struct Point
  {
  int x;	// X position
  int y;	// Y position
  Point(int new_x,int new_y)
    {
    x=new_x;
    y=new_y;
    }
  };

// A rectangle

struct Rect
  {
  int x;	// X position
  int y;	// Y position
  int w;	// Width
  int h;	// Height
  Rect(int new_x,int new_y,int new_w,int new_h)
    {
    x=new_x;
    y=new_y;
    w=new_w;
    h=new_h;
    }
  Rect()
    {
    }
  };

// A widget/window

class Widget : public Pmgr
  {
  public:
  Screen& screen;			// Screen we're on
  Widget *in;				// Widget we're in
  Link<Widget> kids;			// Our kids
  int focus_path;			// Kid no. which has focused widget
  Link<Widget> link;			// Our siblings
  int x_pos, y_pos;			// Position in parent's coordinates
  int width, height;			// Size

  Gc bknd_gc;				// Background gc
  Gc fgnd_gc;				// Foreground gc

  // For drawing
  int x_scrn, y_scrn;			// Position in screen coordinates
  vector<Rect> rects;			// Clipping rectanges for this window
  bool needs_redraw;			// Set if area specified in redraw_rect need to be redrawn
  bool needs_full_redraw;		// Set if everything has to be redrawn
  bool needs_recalc;			// Set if our clipping list needs to be recalculated
  Rect redraw_rect;			// What needs to be redrawn

  // Focus management
  int x_cur, y_cur;			// Position of cursor if we have focus
  Widget *on_left,			// Focus after left arrow
    *on_right,				// Focus after right arrow
    *on_up,				// Focus after up arrow
    *on_down,				// Focus after down arrow
    *on_rtn,				// Focus after return
    *on_tab;				// Focus after tab

  Kmap< void (Widget::*)(int) > *kmap;	// Keymap to use for this widget
  Kbd< void (Widget::*)(int) > kbd;		// Keyboard handler

  // Flags
  bool on_flag;				// Set if widget is currently on
  bool enable_flag;			// Set if widget is enabled (allowed to be turned on)
  bool focusable_flag;			// Set if widget can have the focus

  // Not for users...

  void on(void);			// Turn on widget if it is enabled
  void force_recalc(void);		// Force self and kids to recalculate clipping rectangles
  void off(void);			// Turn off widget
  bool not_clipped(vector<Rect>& rects,int x,int y);
  vector<Rect> clip(int x,int y,int w,int h);
  void sibclip(vector<Rect>& rects,Widget *parent,Widget *i,int x,int y,int w,int h);

  void ufocusnext(int);
  void ufocusprev(int);

  // Event handlers
  // Events are only received by widgets which are on().

  virtual void draw(int new_x,int new_y,int new_w,int new_h);
  					// Draw this widget
  virtual void user(int c);		// Handle user input
  virtual void focus();			// We attained focus
  virtual void focusrtn();		// We attained return-key focus
  virtual void unfocus();		// We lost focus
  virtual void unfocusrtn();		// We lost return key focus
  virtual void resize(int new_w,int new_h);
  					// We were resized
  // Containment

  void add(Widget *);			// Add a widget to this one
  void rmv();				// Remove self from parent
  int no_kids();			// No. of kids we have
  int any_kids();			// True if we have any kids
  Widget *nth_kid(int n);		// Get nth kid
  int find(Widget *);			// Determine child no. of Widget *

  // Setup (before first draw)

  void stwidth(int new_w);		// Initial drawing width
  void stiw(int new_w);			// Inside width
  void stw(int new_w);			// Outside width
  void stheight(int new_h);		// Initial height
  void stih(int new_h);			// Inside height
  void sth(int new_h);			// Outside height
  void stx(int new_x);			// Initial x position
  void sty(int new_y);			// Initial y position

  // Mode

  void enable();			// Enable this widget
  void disable();			// Disable this widget

  void focusable();			// Allow widget to be focused
  void unfocusable();			// Prevent widget from being focused

  // Queries

  int gtwidth();			// Current drawing window width
  int gtiw();				// Current inside width
  int gtw();				// Current outside width

  int gtheight();			// Current drawing window height
  int gtih();				// Current inside height
  int gth();				// Current outside height

  int gtx();				// Current x position
  int gty();				// Current y position

  Widget *gtmom();			// Get parent
  int ckenabled();			// True if we're enabled
  int ckon();				// True if we're on
  int ckfocusable();			// True if we're focusable
  int ckx();				// True if st_x has been called
  int cky();				// True if st_y has been called
  int ckw();				// True if st_w has been called
  int ckh();				// True if st_h has been called

  // Graphics context

  Gc gtbknd();				// Get background Gc
  void stbknd(Gc);			// Set background Gc

  Gc gtfgnd();				// Get foreground Gc
  void stfgnd(Gc);			// Set foreground Gc

  // Keymap manegement
  void stkmap(Kmap< void (Widget::*)(int) > *new_kmap);
  					// Set keymap

  // Drawing commands and utilities

  void drawdot(Gc c,int x,int y);				// Draw a dot
  void drawtxt(Gc gc,int x,int y,String text);			// Draw text
  void drawfield(Gc gc,int w,int x,int y,String text);		// Draw a field
  void drawfilled(Gc gc,int x,int y,int w,int h);		// Draw filled rectangle
  void drawline(Gc gc,int x,int y,int w,int h);			// Draw a line
  void drawrect(Gc gc,int x,int y,int w,int h);			// Draw a rectangle
  void drawbox(Gc gc,int x,int y,int w,int h,int size);		// Draw a thick rectangle
  void drawclr();			// Clear to gtbknd()
  void drawclr(int x,int y,int w,int h);
  void redraw();			// Force full redraw
  void redraw(int new_x,int new_y,int new_w,int new_h);
  					// Force redraw of rectangle
  int txth(Gc gc);			// Text height
  int txtb(Gc gc);			// Text baseline
  int txtw(Gc gc,String text);		// Text width

  // Construct/Destruct
  Widget();
  ~Widget();
  };

#endif
