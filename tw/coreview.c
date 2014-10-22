#include "coreview.h"

Coreview::Coreview()
  {
  the_coltypes=0;
  the_colwidths=0;
  the_colnames=0;
  the_dirty=0;
  the_fields=0;
  }

Coreview::~Coreview()
  {
  if(meta)
    {
    int y;
    delete meta;
    delete[] the_coltypes;
    delete[] the_colwidths;
    delete[] the_colnames;
    delete[] the_dirty;
    for(y=0;y!=no_rows;++y) delete[] the_fields[y];
    delete[] the_fields;
    }
  }

void Coreview::setup(char *new_meta)
  {
  no_rows=0;
  no_size=10;
  no_cols=nfields(new_meta);
  the_coltypes=new int[no_cols];
  the_colwidths=new int[no_cols];
  the_colnames=new String[no_cols];
  the_dirty=new int[no_cols];
  the_fields=new String *[no_cols];
  meta=String(new_meta);
  for(x=0;x!=no_rows;++x)
    {
    
    }
  }

virtual int Coreview::nrows()
  {
  return no_rows;
  }

virtual int Coreview::ncols()
  {
  return no_cols;
  }

virtual int Coreview::colwidth(int x)
  {
  return the_colwidths[x];
  }

virtual int Coreview::coltype(int x)
  {
  return the_coltypes[x];
  }

virtual String Coreview::colname(int x)
  {
  return the_colnames[x];
  }

virtual String Coreview::gt(int x,int y)
  {
  return the_fields[y][x];
  }

virtual void Coreview::st(int x,int y,String s)
  {
  the_fields[y][x]=s;
  }

// Create a new row

int Coreview::add()
  {
  }

// Delete a row

void Coreview::del(int y)
  {
  }
