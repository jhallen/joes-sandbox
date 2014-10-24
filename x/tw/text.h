// Text widget

#ifndef _Itext
#define _Itext 1

#include "widget.h"

class Text : public Widget
  {
  public:
  String buf;
  void st(String val);		// Set field
  String gt();			// Get field
  virtual void draw(int new_x,int new_y,int new_w,int new_h);		// Draw it
  };

#endif
