// Placement manager
// Should be inherited by Widget

// Uses gtwidth(), gtheight(), stwidth(), stheight(),
//      gtw(), gth(), stw(), sth(),
//      ckx(), cky(), ckw(), ckh().

// Whenever a widget is added to a widget, pmgr_add should
// be called.

// calcw() and calch() should be called by gtwidth and gtheight if
// user hasn't set size manually.  calcx() and calcy() should be
// called by stwidth() and stheight() to position the children.

#ifndef _Ipmgr
#define _Ipmgr 1

class Widget;

class Pmgr
  {
  public:

  // Object which controls  placement needs these
  // These could be freed after placement is complete.
  int lrel, rrel, wid, hmode, hofst;		// Horz. positioning mode
  int alrel, arrel, awid, ahmode, ahofst;	// Aux horz. positioning mode
  int trel, brel, hgt, vmode, vofst;		// Vert. positioning mode
  int atrel, abrel, ahgt, avmode, avofst;	// Aux Vert. positioning mode

  // Object being placed needs these
  // These should stay around in case of resize.
  int lofst, rofst, tofst, bofst;		// Relative position

  void dox(Widget *w);
  void doy(Widget *w);

  void calcx(Widget *self);		// Calculate X positions of kids
  void calcy(Widget *self);		// Calculate Y positions of kids
  void calcw(Widget *self);		// Calculate our width
  void calch(Widget *self);		// Calculate our height
  void pmgr_add(Widget *self,Widget *w);// Tell Pmgr that a widget was added
  Pmgr();
  ~Pmgr();

  // Set positioning mode

  // Horz modes
  void ltor(Widget *rel,int ofst);	// Left to right
  void ledge(int ofst);			// Left edge
  void lsame(Widget *rel,int ofst);	// Left same as another
  void rtol(Widget *rel,int ofst);	// Right to left
  void redge(int ofst);			// Right edge
  void rsame(Widget *rel,int ofst);	// Right same as another
  void hcenter();			// Centered

  // Aux horz modes
  void auxltor(Widget *rel,int ofst);
  void auxledge(int ofst);
  void auxlsame(Widget *rel,int ofst);
  void auxrtol(Widget *rel,int ofst);
  void auxredge(int ofst);
  void auxrsame(Widget *rel,int ofst);

  // Vert modes
  void ttob(Widget *rel,int ofst);	// Top to bottom
  void tedge(int ofst);			// Top edge
  void tsame(Widget *rel,int ofst);	// Top same as another
  void btot(Widget *rel,int ofst);	// Bottom to top
  void bedge(int ofst);			// Bottom edge
  void bsame(Widget *rel,int ofst);	// Bottom same as another
  void vcenter();			// Centered

  // Aux vert modes
  void auxttob(Widget *rel,int ofst);
  void auxtedge(int ofst);
  void auxtsame(Widget *rel,int ofst);
  void auxbtot(Widget *rel,int ofst);
  void auxbedge(int ofst);
  void auxbsame(Widget *rel,int ofst);
  };

#endif
