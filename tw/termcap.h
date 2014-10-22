#ifndef _Itermcap
#define _Itermcap 1

#include <iostream>
#include <fstream>
#include <map>
#include <list>
using namespace std;
#include "string.h"

class Cap
  {
  public:
  ostream& out;			// Output stream

  map<String,String> caps;	// Capabilities

  char *abuf;			// For terminfo compatible version
  char *abufp;

  int div;			// tenths of MS per char
  int baud;			// Baud rate
  String pad;			// Padding string
  int dopadding;		// Set if pad characters should be used

  list<String> words(String,String);
  int match(String s,String name);
  void fixup(map<String,String>& caps);
  void add(map<String,String>& caps,String entry);
  int find(map<String,String>& caps,fstream& f,String name);
  int link(map<String,String>& caps,list<String>& files,list<String>::iterator np);
  int listfind(map<String,String>& caps,list<String>& files,list<String>::iterator np,String name);
  char escape(String&,int&);

  // Get CAP entry for terminal named in 'name'.  If 'name' is zero, the name in
  // the environment variable 'TERM' is used instead.
  //
  // 'ibaud'      is the baud rate used for 'texec' to calculate number of pad chars
  // 'iout'       is the ostream to use for 
  // 'ioutptr'    is the passed as the first arg to 'out'
  //              the second arg contains the char to output
  // 'idopadding' should be set if pad characters are to be used.
  //
  // This is how 'Cap' finds the entry:  First a list of file names is
  // built.  If the environment variable 'TERMCAP' begins with a '/', it is
  // used as the list of file names.  Otherwise, if the environment variable
  // 'TERMPATH' is set, it is used as the list of file names.  If that isn't
  // set, then the string '/.termcap /usr/lib/joe/termcap' is appended to
  // value of the 'HOME' environment variable, and that is used as the list
  // of names.  If HOME isn't set, then '.termcap /usr/lib/joe/termcap'
  // is used as the list of file names.
  //
  // Now the contents of the environment variable 'TERMCAP' (if it's defined and
  // if it doesn't begin with a '/') and the files from the above list are
  // scanned for the terminal name.  The contents of the environment variable
  // are scanned first, then the files are scanned in the order they appear in
  // the named list.
  //
  // If the last part of a matching termcap entry is a 'tc=filename', then
  // the current file is rewound and rescanned for the matching entry (and if
  // it's not found, the next entry in the file name list is searched).  If
  // a matching termcap entry in the TERMCAP environment variable ends with
  // a 'tc=filename', then all of the files in the name list are searched.
  //
  // There is no limit on the size of the termcap entries.  No checking is
  // done for self-refering 'tc=filename' links (so all of core will be
  // allocated if there are any).

  Cap(ostream& new_out=cout,char *name=0,unsigned ibaud=38400,int idopadding=0);

  ~Cap();

  // Set padding flag.  If 'flg' is true, padding will be used.  If 'flg' is
  // false, no padding will be sent.

  void stpadding(int flg);

  // Set baud rate to assume for padding

  void stbaud(unsigned ibaud);

  // Get value of string capability.

  String getstr(String name);

  // Return true if the named capability is found.

  int getflag(String name);

  // Return value of numeric capability or return -1 if it's not found.

  int getnum(String name);

  // Execute and output a termcap string capability.
  //
  // 'cap' is the CAP returned by getcap which contains the baud rate and output
  // function.
  //
  // 'str' is the string to execute.  If 'str'==NULL, nothing happens.
  //
  // 'l' is the number of lines effected by this string.  For example, if you
  // use the clear to end of screen capability, the number of lines between
  // the current cursor position and the end of the screen should be
  // given here.
  //
  // 'a0' - 'a1' are the arguments for the string

  void texec(ostream& out,String str,int l=1,int a0=0,int a1=0,int a2=0,int a3=0);
  inline void exec(String str,int l=1,int a0=0,int a1=0,int a2=0,int a3=0) { texec(out,str,l,a0,a1,a2,a3); }

  // Return cost in number of characters which need to be sent
  // to execute a termcap string capability.
  //
  // 'cap' is the CAP returned by getcap which contains the baud rate and output
  // functions.
  //
  // 'str' is the string to execute.  If 'str'==NULL, tcost return 10000.
  //
  // 'l' is the number of lines effected by this string.  Ex: if you
  // use the clear to end of screen capability, the number of lines between
  // the current cursor position and the end of the screen should be
  // given here.
  //
  // 'a0' - 'a3' are arguements passed to the string

  int tcost(String str,int l=1,int a0=0,int a1=0,int a2=0,int a3=0);

  // Compile a string capability.  Pad characters are not placed in the string.

  String tcompile(String str,int l=1,int a0=0,int a1=0,int a2=0,int a3=0);
  };

#endif
