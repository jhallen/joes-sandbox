// Key-map handler

#include <stdlib.h>
#include "kbd.h"

/* X Windows key name to value table */
/* This table must be sorted */

struct xkey
  {
  char *name;
  int value;
  };

static struct xkey xkeys[]=
  {
    { "BackSpace",	0xFF08 },
    { "Begin",		0xFF58 },
    { "Break",		0xFF6B },
    { "Cancel",		0xFF69 },
    { "Clear",		0xFF0B },
    { "Delete",		0xFFFF },
    { "Down",		0xFF54 },
    { "End",		0xFF57 },
    { "Escape",		0xFF1B },
    { "Execute",	0xFF62 },
    { "F1",		0xFFBE },
    { "F10",		0xFFC7 },
    { "F11",		0xFFC8 },
    { "F12",		0xFFC9 },
    { "F13",		0xFFCA },
    { "F14",		0xFFCB },
    { "F15",		0xFFCC },
    { "F16",		0xFFCD },
    { "F17",		0xFFCE },
    { "F18",		0xFFCF },
    { "F19",		0xFFD0 },
    { "F2",		0xFFBF },
    { "F20",		0xFFD1 },
    { "F21",		0xFFD2 },
    { "F22",		0xFFD3 },
    { "F23",		0xFFD4 },
    { "F24",		0xFFD5 },
    { "F25",		0xFFD6 },
    { "F26",		0xFFD7 },
    { "F27",		0xFFD8 },
    { "F28",		0xFFD9 },
    { "F29",		0xFFDA },
    { "F3",		0xFFC0 },
    { "F30",		0xFFDB },
    { "F31",		0xFFDC },
    { "F32",		0xFFDD },
    { "F33",		0xFFDE },
    { "F34",		0xFFDF },
    { "F35",		0xFFE0 },
    { "F4",		0xFFC1 },
    { "F5",		0xFFC2 },
    { "F6",		0xFFC3 },
    { "F7",		0xFFC4 },
    { "F8",		0xFFC5 },
    { "F9",		0xFFC6 },
    { "Find",		0xFF68 },
    { "Help",		0xFF6A },
    { "Home",		0xFF50 },
    { "Insert",		0xFF63 },
    { "KP_0",		0xFFB0 },
    { "KP_1",		0xFFB1 },
    { "KP_2",		0xFFB2 },
    { "KP_3",		0xFFB3 },
    { "KP_4",		0xFFB4 },
    { "KP_5",		0xFFB5 },
    { "KP_6",		0xFFB6 },
    { "KP_7",		0xFFB7 },
    { "KP_8",		0xFFB8 },
    { "KP_9",		0xFFB9 },
    { "KP_Add",		0xFFAB },
    { "KP_Begin",	0xFF9D },
    { "KP_Decimal",	0xFFAE },
    { "KP_Delete",	0xFF9F },
    { "KP_Divide",	0xFFAF },
    { "KP_Down",	0xFF99 },
    { "KP_End",		0xFF9C },
    { "KP_Enter",	0xFF8D },
    { "KP_Equal",	0xFFBD },
    { "KP_F1",		0xFF91 },
    { "KP_F2",		0xFF92 },
    { "KP_F3",		0xFF93 },
    { "KP_F4",		0xFF94 },
    { "KP_Home",	0xFF95 },
    { "KP_Insert",	0xFF9E },
    { "KP_Left",	0xFF96 },
    { "KP_Multiply",	0xFFAA },
    { "KP_Next",	0xFF9B },
    { "KP_Page_Down",	0xFF9B },
    { "KP_Page_Up",	0xFF9A },
    { "KP_Prior",	0xFF9A },
    { "KP_Right",	0xFF98 },
    { "KP_Separator",	0xFFAC },
    { "KP_Space",	0xFF80 },
    { "KP_Subtract",	0xFFAD },
    { "KP_Tab",		0xFF89 },
    { "KP_Up",		0xFF97 },
    { "L1",		0xFFC8 },
    { "L10",		0xFFD1 },
    { "L2",		0xFFC9 },
    { "L3",		0xFFCA },
    { "L4",		0xFFCB },
    { "L5",		0xFFCC },
    { "L6",		0xFFCD },
    { "L7",		0xFFCE },
    { "L8",		0xFFCF },
    { "L9",		0xFFD0 },
    { "Left",		0xFF51 },
    { "Linefeed",	0xFF0A },
    { "Menu",		0xFF67 },
    { "Mode_switch",	0xFF7E },
    { "Motion",		0x8006 },
    { "Next",		0xFF56 },
    { "Num_Lock",	0xFF7F },
    { "Page_Down",	0xFF56 },
    { "Page_Up",	0xFF55 },
    { "Pause",		0xFF13 },
    { "Press1",		0x8000 },
    { "Press2",		0x8001 },
    { "Press3",		0x8002 },
    { "Print",		0xFF61 },
    { "Prior",		0xFF55 },
    { "R1",		0xFFD2 },
    { "R10",		0xFFDB },
    { "R11",		0xFFDC },
    { "R12",		0xFFDD },
    { "R13",		0xFFDE },
    { "R14",		0xFFDF },
    { "R15",		0xFFE0 },
    { "R2",		0xFFD3 },
    { "R3",		0xFFD4 },
    { "R4",		0xFFD5 },
    { "R5",		0xFFD6 },
    { "R6",		0xFFD7 },
    { "R7",		0xFFD8 },
    { "R8",		0xFFD9 },
    { "R9",		0xFFDA },
    { "Redo",		0xFF66 },
    { "Release1",	0x8003 },
    { "Release2",	0x8004 },
    { "Release3",	0x8005 },
    { "Return",		0xFF0D },
    { "Right",		0xFF53 },
    { "SP",		0x20   },
    { "Scroll_Lock",	0xFF14 },
    { "Select",		0xFF60 },
    { "Sp",		0x20   },
    { "Sys_Req",	0xFF15 },
    { "Tab",		0xFF09 },
    { "Undo",		0xFF65 },
    { "Up",		0xFF52 },
    { "script_switch",	0xFF7E },
    { "sp",		0x20   }
  };

/* Search for X key value */

int xkfind(const String& s)
  {
  int a, b, c, q;
  a=0; b=c=sizeof(xkeys)/sizeof(struct xkey);
  while(b!=(a+c)/2)
    {
    b=(a+c)/2;
    q=s.compare(xkeys[b].name);
    if(q<0) c=b;
    else if(q>0) a=b;
    else return xkeys[b].value;
    }
  return -1;
  }

/* Return key code for key name or -1 for syntax error */

int parse_keyval(String s)
  {
  int n;
  if(!s) return -1;
  else if((n=parse_keyval(parse_prefix(s,"A-")))!=-1) return n|Alt;
  else if((n=parse_keyval(parse_prefix(s,"C-")))!=-1) return n|Ctrl;
  else if((n=parse_keyval(parse_prefix(s,"S-")))!=-1) return n|Shift;
  else if(s.size()==2 && s[0]=='^')
    if(s[1]=='?') return 127;
    else return s[1]&0x1F;
  else if((n=xkfind(s))!=-1) return n;
  else if(s.size()==1) return (unsigned char)s[0];
  else return -1;
  }

// Parse a range

String parse_range(String seq,int& vv,int& ww)
  {
  int v, w;
  String word;
  String rest;
  if(!(seq=parse_word(seq,word), seq) || (w=v=parse_keyval(word))==-1)
    {
    return String();
    }
  if(seq.size() && seq[0]==' ') seq=seq+1;
  if(rest=parse_keyword(seq,"TO"))
    {
    if(!(rest=parse_word(rest,word), rest) || (w=parse_keyval(word))==-1) return String();
    if(v>w) return String();
    if(rest.size() && rest[0]==' ') rest=rest+1;
    seq=rest;
    }
  vv=v;
  ww=w;
  return seq;
  }

// Get termcap key sequence

String getcapseq(Cap& cap,String name)
  {
  return cap.tcompile(cap.getstr(name));
  }

// Parse termcap key sequence name (.xxx)

String parse_dotword(String s,String& rtn)
  {
  if(s.size() && s[0]=='.') return parse_word(s+1,rtn);
  else return String();
  }
