#ifndef _Iparse
#define _Iparse 1

#include <list>
using namespace std;
#include "string.h"

String parse_keyword(String s,const char *keyword);
String parse_prefix(String s,const char *prefix);
String parse_ws(String s);
String parse_word(String s,String& rtn);
list<String> parse_words(String s);

#endif
