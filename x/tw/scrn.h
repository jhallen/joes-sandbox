#ifndef _Iscrn
#define _Iscrn 1

#include <iostream>
using namespace std;
#include "string.h"
#include "termcap.h"

struct hentry
  {
  int next;
  int loc;
  };

class Scrn : public Cap
  {
  public:
  // Capabilities extracted from 'Cap'
  // when prefixed with c, means cost of
  int skiptop;

  int li;			// Screen height
  int co;			// Screen width

  String ti;			// Initialization string
  String cl;			// Home and clear screen... really an
  				// init. string
  String cd;			// Clear to end of screen
  String te;			// Restoration string

  bool haz;			// Terminal can't print ~s
  bool os;			// Terminal overstrikes
  bool eo;			// Can use blank to erase even if os
  bool ul;			// _ overstrikes
  bool am;			// Terminal has autowrap, but not magicwrap
  bool xn;			// Terminal has magicwrap

  String so;			// Enter standout (inverse) mode
  String se;			// Exit standout mode

  String us;			// Enter underline mode
  String ue;			// Exit underline mode
  String uc;			// Single time underline character

  bool ms;			// Ok to move when in standout/underline mode

  String mb;			// Enter blinking mode
  String md;			// Enter bold mode
  String mh;			// Enter dim mode
  String mr;			// Enter inverse mode
  String me;			// Exit above modes

  String Sb;			// Set background color
  String Sf;			// Set foreground color
  bool ut;			// Screen erases with background color

  bool da, db;			// Extra lines exist above, below
  String al, dl, AL, DL;	// Insert/delete lines
  String cs;			// Set scrolling region
  bool rr;			// Set for scrolling region relative addressing
  String sf, SF, sr, SR;	// Scroll

  String dm, dc, DC, ed;	// Delete characters
  String im, ic, IC, ip, ei;	// Insert characters
  bool mi;			// Set if ok to move while in insert mode

  String bs;			// Move cursor left 1
  int cbs;
  String lf;			// Move cursor down 1
  int clf;
  String up;			// Move cursor up 1
  int cup;
  String nd;			// Move cursor right 1

  String ta;			// Move cursor to next tab stop
  int cta;
  String bt;			// Move cursor to previous tab stop
  int cbt;
  int tw;			// Tab width

  String ho;			// Home cursor to upper left
  int cho;
  String ll;			// Home cursor to lower left
  int cll;
  String cr;			// Move cursor to left edge
  int ccr;
  String RI;			// Move cursor right n
  int cRI;
  String LE;			// Move cursor left n
  int cLE;
  String UP;			// Move cursor up n
  int cUP;
  String DO;			// Move cursor down n
  int cDO;
  String ch;			// Set cursor column
  int cch;
  String cv;			// Set cursor row
  int ccv;
  String cV;			// Goto beginning of specified line
  int ccV;
  String cm;			// Set cursor row and column
  int ccm;

  String ce;			// Clear to end of line
  int cce;

  // Basic modes
  bool scroll_flg;		// Set to use scrolling
  bool insdel_flg;		// Set to use insert/delete within line

  // Current state of terminal
  int *scrn;			// Current contents of screen
  int cur_x,cur_y;		// Current cursor position (-1 for unknown)
  int cur_top,cur_bot;		// Current scrolling region
  int cur_attr;			// Current character attributes
  int cur_ins;			// Set if we're in insert mode

  int *updtab;			// Dirty lines table
  int avattr;			// Bits set for available attributes
  int *sary;			// Scroll buffer array

  int *compose;			// Line compose buffer
  int *magic_ofst;		// stuff for magic
  struct hentry *magic_htab;
  struct hentry *magic_ary;

  void doupscrl(int top,int bot,int amnt);
  void dodnscrl(int top,int bot,int amnt);
  void dodelchr(int x,int y,int n);
  void doinschr(int x,int y,int *s,int n);
  void cposs(int x,int y);
  int relcost(int x,int y,int ox,int oy);
  void outatri(int x,int y,int c);
  void clrins();
  void setins(int x);
  void setregn(int top,int bot);
  void attr(int c);

  inline int width() { return co; }	// Get width
  inline int height() { return li; }	// Get height

  int cpos(int x,int y);		// Set cursor position
  void outatr(int x,int y,int c);	// Output character at screen coord
  void eraeol(int x,int y);		// Erase to end of line


  void scrlup(int top,int bot,int amnt);
  					// Scroll up request
  void scrldn(int top,int bot,int amnt);
  					// Scroll down request
  void scroll();			// Execute scroll requests

  void magic(int y,int *cur,int *newline,int placex);
  					// Line shifting

  void partial();
  void suspend();			// Prepare for shell escape
  void close();				// Prepare for close
  void resume();			// Resume editing after suspend()
  void redraw();			// Invalidate everything
  void resize(int w,int h);		// Resize screen
  void attach();			// Process capabilities
  Scrn(ostream& new_out=cout,char *name=0,unsigned ibaud=38400,int idopadding=0);	// Create
  ~Scrn();
  };

// Character attribute bits

#define INVERSE 256
#define UNDERLINE 512
#define BOLD 1024
#define BLINK 2048
#define DIM 4096
#define AT_MASK (INVERSE+UNDERLINE+BOLD+BLINK+DIM)

// Standard background colors

#define BG_SHIFT 13
#define BG_MASK (7<<BG_SHIFT)
#define BG_BLACK (0<<BG_SHIFT)		// Default
#define BG_RED (1<<BG_SHIFT)
#define BG_GREEN (2<<BG_SHIFT)
#define BG_YELLOW (3<<BG_SHIFT)
#define BG_BLUE (4<<BG_SHIFT)
#define BG_MAGENTA (5<<BG_SHIFT)
#define BG_CYAN (6<<BG_SHIFT)
#define BG_WHITE (7<<BG_SHIFT)

// Standard foreground colors

#define FG_SHIFT 16
#define FG_MASK (7<<FG_SHIFT)
#define FG_WHITE (0<<FG_SHIFT)		// Default
#define FG_CYAN (1<<FG_SHIFT)
#define FG_MAGENTA (2<<FG_SHIFT)
#define FG_BLUE (3<<FG_SHIFT)
#define FG_YELLOW (4<<FG_SHIFT)
#define FG_GREEN (5<<FG_SHIFT)
#define FG_RED (6<<FG_SHIFT)
#define FG_BLACK (7<<FG_SHIFT)

#endif
