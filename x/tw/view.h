#ifndef _IView
#define _IView 1

#include "string.h"

// Column types
#define tString 0	// Unformatted
#define tDate 1		// Date in format: Jan 14, 1980

class View
  {
  public:
  virtual int nrows() = 0;			// Number of rows in view
  virtual int ncols() = 0;			// Number of columns in view

  // How a column is displayed
  virtual int colwidth(int) = 0;		// Get width of a column
  virtual int coltype(int) = 0;			// 0=normal, 1=date

  virtual String colname(int) = 0;		// Get name of a column
  virtual String gt(int x,int y) = 0;		// Get field
  virtual void st(int x,int y,String s) = 0;	// Set field

  virtual ~View() {}
  };

#endif
