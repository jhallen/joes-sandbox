#ifndef _ICoreview
#define _ICoreview 1

#include "view.h"
#include "string.h"
#include "util.h"

class Coreview : public View
  {
  // Meta data
  String meta;

  int no_cols;
  int no_rows;
  int no_size;

  // One for each column
  int *the_coltypes;
  int *the_colwidths;
  String *the_colnames;

  // One for each row
  int *the_dirty;		// Set for each row which needs updating: 0=no change, 1=add, 2=delete, 3=replace
  String **the_fields;		// Fields

  public:

  void setup(char *meta);	// Set up View: meta contains column names, types in tab sep format

  int add();			// Add a record to the end of the view.  New row number is returned
  void del(int n);		// Delete a row

  virtual int nrows();
  virtual int ncols();
  virtual int colwidth(int x);
  virtual int coltype(int x);
  virtual String colname(int x);
  virtual String gt(int x,int y);
  virtual void st(int x,int y,String s);

  Coreview();			// Create empty Coreview
  virtual ~Coreview();		// Delete Coreview
  };

#endif
