// Generic microcode assembler
// by Joe Allen

// Todo:
//   Use bignum library for Value
//   Check for number overflow
//   Add expression parser
//   Cross reference listing
//   More output format options (cut output into multiple N-bit width tables).
//   Enums
//   Errors should appear in listing

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <string>
#include <fstream>
#include <map>
using namespace std;

typedef long long Value;	// Value (limits width of microinstructions)

Value *image;			// Output image

// Source file

struct Source
  {
  Source *next;
  fstream f;			// File
  int line;			// Line number
  string name;			// File name
  Source(string);		// Open file
  void rewind();		// Rewind
  };

Source *src;			// Current source

Source::Source(string new_name)
  {
  line=0;
  next=src;
  name=new_name;
  f.open(name.c_str(),ios::in);
  if(!f)
    {
    if(src)
      cerr << src->name << " " << src->line << ": Couldn't open file '" << name << "'\n";
    else
      cerr << "Couldn't open file '" << name << "'\n";
    exit(-1);
    }
  }

void Source::rewind()
  {
  f.close();
  f.open(name.c_str(),ios::in);
  if(!f)
    {
    cerr << "Couldn't reopen " << name << "\n";
    exit(-1);
    }
  line=0;
  }

// Get next source line

int next_line(string& s)
  {
  while(!getline(src->f,s))
    if(src->next)
      src=src->next;
    else
      return -1;
  ++src->line;
  return 0;
  }

// A field definition

struct Field
  {
  string name;			// Field name
  int pos;			// Field position
  int width;			// Field width
  map<string,Value> enums;	// Enums for this field
  Value bind_v;			// Bound value
  int bind_flag;		// Set if we bound
  };

// An instruction definition

struct Insn
  {
  string name;			// Instruction name
  Value code;			// Code for instruction
  Value bind_v;
  map<string,Field *> fields;	// Bit-fields of instruction
  void bind(map<string,Value>& args);
  				// Bind arguments to fields
  };

map<string,Insn *> insns;	// Instructions

// Global variables

int pass=0;			// Pass no.
int pc=0;			// Current location

int width=32;			// Width of microinstructions
int width_flag=0;

int size=256;			// Size of microprogram
int size_flag=0;

Value filler=0;			// Fill to use for balance
int filler_flag=0;

char *src_name;			// Source file name
char *list_name;		// List file name
char *out_name;			// Output file name
fstream list_file;		// Listing file
fstream out_file;		// Output file
map<string,Value> symbol_table;	// Symbol table

// Convert a value to hex

string vtohex(Value v)
  {
  string rtn;
  int ndigs;
  int x;
  // Trim to width
  x=(1<<(width-1));
  x<<=1;
  x-=1;
  v&=x;
  ndigs=(width+3)/4;
  for(x=ndigs-1;x>=0;--x)
    rtn+="0123456789ABCDEF"[(v>>(x*4))&15];
  return rtn;
  }

// Parsing functions

// Advance pos to first non-whitespace char

void skipws(string& s,int& pos)
  {
  while(pos!=s.size() && (s[pos]==' ' || s[pos]=='\t')) ++pos;
  }

// Parse single character and any following whitespace.  Returns
// 0 if the character was found or -1 if not.

int parse_char(char c,string& s,int& pos)
  {
  if(pos!=s.size() && s[pos]==c)
    {
    ++pos;
    skipws(s,pos);
    return 0;
    }
  else
    return -1;
  }

// Extract word beginning at i and then advance i past any whitespace
// following the word.  Returns 0 for success or -1 if there is no word at
// s,i.

int parse_word(string& s,int& i,string& rtn)
  {
  if(i!=s.size() &&
     (s[i]>='A' && s[i]<='Z' || s[i]>='a' && s[i]<='z' || s[i]=='_'))
    {
    int y;
    for(y=i;y!=s.size() &&
            (s[y]>='a' && s[y]<='z' ||
             s[y]>='A' && s[y]<='Z' ||
             s[y]>='0' && s[y]<='9' ||
             s[y]=='_');++y);
    rtn=s.substr(i,y-i);
    skipws(s,y);
    i=y;
    return 0;
    }
  else
    return -1;
  }

// Same as above but allow '.'

int parse_dword(string& s,int& i,string& rtn)
  {
  if(i!=s.size() &&
     (s[i]>='A' && s[i]<='Z' || s[i]>='a' && s[i]<='z' || s[i]=='_' || s[i]=='.'))
    {
    int y;
    for(y=i;y!=s.size() &&
            (s[y]>='a' && s[y]<='z' ||
             s[y]>='A' && s[y]<='Z' ||
             s[y]>='0' && s[y]<='9' ||
             s[y]=='_' || s[y]=='.');++y);
    rtn=s.substr(i,y-i);
    skipws(s,y);
    i=y;
    return 0;
    }
  else
    return -1;
  }

// Parse a string

int parse_string(string& s,int& i,string& rtn)
  {
  if(i!=s.size() && s[i]=='"')
    {
    ++i;
    int y;
    for(y=i;y!=s.size() && s[y]!='"';++y);
    rtn=s.substr(i,y-i);
    if(y!=s.size() && s[y]=='"')
      {
      ++y;
      skipws(s,y);
      }
    else
      cerr << src->name << " " << src->line << ": missing \" after string\n";
    i=y;
    return 0;
    }
  else
    return -1;
  }

// Parse a value
// (this should be an expression parser)

int parse_value(string& s,int& i,Value& value)
  {
  if(i==s.size()) return -1;
  else if(s[i]>='1' && s[i]<='9')
    { // Decimal
    value=0;
    while(i!=s.size() && s[i]>='0' && s[i]<='9')
      value=value*10+s[i++]-'0';
    skipws(s,i);
    return 0;
    }
  else if(s[i]=='0' && i+2<s.size() && (s[i+1]=='x' || s[i+1]=='X') &&
          (s[i+2]>='0' && s[i+2]<='9' || s[i+2]>='a' && s[i+2]<='f' || s[i+2]>='A' && s[i+2]<='F'))
    { // Hexadecimal
    value=0;
    i+=2;
    while(i!=s.size() && (s[i]>='0' && s[i]<='9' || s[i]>='A' && s[i]<='F' || s[i]>='a' && s[i]<='f'))
      if(s[i]>='0' && s[i]<='9')
        value=value*16+s[i++]-'0';
      else if(s[i]>='A' && s[i]<='F')
        value=value*16+s[i++]-'A'+10;
      else
        value=value*16+s[i++]-'a'+10;
    skipws(s,i);
    return 0;
    }
  else if(s[i]=='0' && i+2<s.size() && (s[i+1]=='b' || s[i+1]=='B') &&
          (s[i+2]>='0' && s[i+2]<='1'))
    { // Binary
    value=0;
    i+=2;
    while(i!=s.size() && s[i]>='0' && s[i]<='1')
      value=value*2+s[i++]-'0';
    skipws(s,i);
    return 0;
    }
  else if(s[i]=='0' && i+1<s.size() && (s[i+1]>='0' && s[i+1]<='7'))
    { // Octal
    value=0;
    ++i;
    while(i!=s.size() && s[i]>='0' && s[i]<='7')
      value=value*8+s[i++]-'0';
    skipws(s,i);
    return 0;
    }
  else if(s[i]=='0')
    { // Zero
    ++i;
    skipws(s,i);
    value=0;
    return 0;
    }
  else if(s[i]>='A' && s[i]<='Z' || s[i]>='a' && s[i]<='z' || s[i]=='_')
    { // A label
    string w;
    parse_word(s,i,w);
    map<string,Value>::iterator p=symbol_table.find(w);
    if(p==symbol_table.end())
      {
      if(pass)
        cerr << src->name << " " << src->line << ": undefined symbol: " << w << "\n";
      }
    else
      value=p->second;
    return 0;
    }
  else
    return -1;
  }

// Parse name=value pair at i and then advance i past any following whitespace

int parse_namevalue(string& s,int& i,string& name,Value& value)
  {
  if(parse_word(s,i,name)) return -1;
  if(parse_char('=',s,i)) return -1;
  if(parse_value(s,i,value)) return -1;
  return 0;
  }

// Parse argument list

void parse_args(string& s,int& i,map<string,Value>& args)
  {
  string name;
  Value value;
  int flag=-1;
  while(!parse_namevalue(s,i,name,value))
    {
    map<string,Value>::iterator p;
    p=args.find(name);
    if(p==args.end())
      {
      args[name]=value;
      }
    else
      {
      cerr << src->name << " " << src->line << ": duplicate arg '" << name << "'\n";
      }
    if(flag=parse_char(',',s,i)) break;
    if(i==s.size() || s[i]==';')
      {
      if(pass && list_file)
        {
        char buf[1024];
        sprintf(buf,"%3d      %-22s ",src->line,"");
        list_file << buf << s << "\n";
        }
      i=0;
      if(next_line(s)) break;
      skipws(s,i);
      }
    }
  if(!flag)
    {
    cerr << src->name << " " << src->line << ": argument missing after ,\n";
    }
  }

// Parse a field definition name[value:value]

int parse_field(string& s,int& i,Field *& field)
  {
  Field *f;
  string name;
  Value up;
  Value down;
  if(parse_word(s,i,name)) return -1;
  if(parse_char('[',s,i)) return -1;
  if(parse_value(s,i,up)) return -1;
  if(parse_char(':',s,i)) return -1;
  if(parse_value(s,i,down)) return -1;
  if(parse_char(']',s,i)) return -1;
  f=new Field;
  f->name=name;
  f->pos=down;
  f->width=up-down+1;
  field=f;
  return 0;
  }

// Parse field definition list

void parse_fields(string& s,int& i,map<string,Field *>& fields)
  {
  Field *field;
  int flag=-1;
  while(!parse_field(s,i,field))
    {
    map<string,Field *>::iterator p;
    p=fields.find(field->name);
    if(p==fields.end())
      {
      fields[field->name]=field;
      }
    else
      {
      cerr << src->name << " " << src->line << ": duplicate field '" << field->name << "'\n";
      }
    if(flag=parse_char(',',s,i)) break;
    if(i==s.size() || s[i]==';')
      {
      if(pass && list_file)
        {
        char buf[1024];
        sprintf(buf,"%3d      %-22s ",src->line,"");
        list_file << buf << s << "\n";
        }
      i=0;
      if(next_line(s)) break;
      skipws(s,i);
      }
    }
  if(!flag)
    {
    cerr << src->name << " " << src->line << ": field missing after ,\n";
    }
  }

// Bind args

void Insn::bind(map<string,Value>& args)
  {
  map<string,Field *>::iterator p;
  map<string,Value>::iterator n;
  bind_v=code;
  for(p=fields.begin();p!=fields.end();++p)
    p->second->bind_flag=0;
  for(n=args.begin();n!=args.end();++n)
    {
    p=fields.find(n->first);
    if(p==fields.end())
      {
      cerr << src->name << " " << src->line << ": unknown field '" << n->first << "'\n";
      }
    else if(p->second->bind_flag)
      {
      cerr << src->name << " " << src->line << ": duplicate binding '" << n->first << "'\n";
      }
    else
      {
      int x=(1<<(p->second->width-1));
      x<<=1;
      x-=1;
      p->second->bind_flag=1;
      p->second->bind_v=n->second;
      bind_v|=((n->second&x)<<p->second->pos);
      }
    }
  for(p=fields.begin();p!=fields.end();++p)
    if(!p->second->bind_flag)
      {
      cerr << src->name << " " << src->line << ": missing field '" << p->first << "'\n";
      }
  }

// Assemble a line

void assemble(string s)
  {
  int y=0;
  string label;
  string inst;
  int lflag=0;
  int orgpc=pc;
  int eflag=0;
  if(!parse_word(s,y,label))
    { // We parsed a label
    // Skip over optional :
    parse_char(':',s,y);
    lflag=1;
    }
  else
    skipws(s,y);

  if(!parse_dword(s,y,inst))
    { // We parse a mnemonic
    if(inst==".insn")
      { // Instruction definition
      string new_insn;
      Value value;
      map<string,Field *> fields;
      map<string,Insn *>::iterator p;
      if(parse_word(s,y,new_insn))
        {
        cerr << src->name << " " << src->line << ": instruction name missing after .insn\n";
        }
      else if(parse_value(s,y,value))
        {
        cerr << src->name << " " << src->line << ": value missing after .insn\n";
        }
      else
        {
        parse_fields(s,y,fields);
        if(y!=s.size() && s[y]!=';')
          cerr << src->name << " " << src->line << ": extra junk at end of line\n";
        if(!pass)
          {
          p=insns.find(new_insn);
          if(p==insns.end())
            {
            Insn *n=new Insn;
            n->name=new_insn;
            n->code=value;
            n->fields=fields;
            insns[new_insn]=n;
            }
          else
            {
            cerr << src->name << " " << src->line << ": duplicate instruction definition '" << new_insn << "'\n";
            }
          }
        }
      }
    else if(inst==".equ")
      { // Set symbol to value
      Value v;
      if(parse_value(s,y,v))
        {
        cerr << src->name << " " << src->line << ": expression missing after .equ\n";
        }
      else
        {
        if(!lflag)
          cerr << src->name << " " << src->line << ": label missing before .equ\n";
        else
          {
          // Add label to symbol table
          map<string,Value>::iterator p=symbol_table.find(label);
          if(p!=symbol_table.end())
            {
            if(p->second!=v)
              cerr << src->name << " " << src->line << ": value of symbol changed: " << label << "\n";
            }
          else
            symbol_table[label]=v;
          }
        if(y!=s.size() && s[y]!=';')
          cerr << src->name << " " << src->line << ": extra junk at end of line\n";
        }
      lflag=0;
      }
    else if(inst==".width")
      { // Set microinstruction width
      Value v;
      if(parse_value(s,y,v))
        {
        cerr << src->name << " " << src->line << ": expression missing after .width\n";
        }
      else
        {
        if(!width_flag)
          {
          width=v;
          width_flag=1;
          }
        else if(width!=v)
          cerr << src->name << " " << src->line << ": value of 'width' changed\n";
        if(y!=s.size() && s[y]!=';')
          cerr << src->name << " " << src->line << ": extra junk at end of line\n";
        }
      }
    else if(inst==".org")
      { // Set origin
      Value v;
      if(parse_value(s,y,v))
        {
        cerr << src->name << " " << src->line << ": expression missing after .org\n";
        }
      else
        {
        pc=v;
        orgpc=v;
        if(y!=s.size() && s[y]!=';')
          cerr << src->name << " " << src->line << ": extra junk at end of line\n";
        }
      }
    else if(inst==".include")
      { // Include a file
      string n;
      if(parse_string(s,y,n))
        {
        cerr << src->name << " " << src->line << ": string missing after .include\n";
        }
      else
        {
        if(y!=s.size() && s[y]!=';')
          cerr << src->name << " " << src->line << ": extra junk at end of line\n";
        src=new Source(n);
        }
      }
    else if(inst==".fill")
      { // Fill balance of microgram with this
      Value v;
      if(parse_value(s,y,v))
        {
        cerr << src->name << " " << src->line << ": expression missing after .fill\n";
        }
      else
        {
        if(!filler_flag)
          {
          filler=v;
          filler_flag=1;
          }
        else if(filler!=v)
          cerr << src->name << " " << src->line << ": value of 'fill' changed\n";
        if(y!=s.size() && s[y]!=';')
          cerr << src->name << " " << src->line << ": extra junk at end of line\n";
        }
      }
    else if(inst==".size")
      { // Set microprogram image size
      Value v;
      if(parse_value(s,y,v))
        {
        cerr << src->name << " " << src->line << ": expression missing after .size\n";
        }
      else
        {
        if(!size_flag)
          {
          size=v;
          size_flag=1;
          }
        else if(size!=v)
          cerr << src->name << " " << src->line << ": value of 'size' changed\n";
        if(y!=s.size() && s[y]!=';')
          cerr << src->name << " " << src->line << ": extra junk at end of line\n";
        }
      }
    else
      { // Parse a normal instruction
      map<string,Insn *>::iterator p;
      p=insns.find(inst);
      if(p==insns.end())
        {
        cerr << src->name << " " << src->line << ": undefined instruction: " << inst << "\n";
        }
      else
        {
        Insn *insn=p->second;
        // Parse args
        map<string,Value> args;
        parse_args(s,y,args);
        if(y!=s.size() && s[y]!=';')
          cerr << src->name << " " << src->line << ": extra junk at end of line\n";
        // match up args and emit instruction here.
        if(pass)
          {
          insn->bind(args);
          if(pc>=size)
            {
            cerr << src->name << " " << src->line << ": exceeded microprogram size\n";
            }
          else
            {
            eflag=1;
            image[pc]=insn->bind_v;
            }
          }
        ++pc;
        }
      }
    }
  else if(y!=s.size() && s[y]!=';')
    {
    cerr << src->name << " " << src->line << ": syntax error\n";
    }
  if(lflag)
    {
    // Add label to symbol table
    map<string,Value>::iterator p=symbol_table.find(label);
    if(p!=symbol_table.end())
      {
      if(p->second!=orgpc)
        cerr << src->name << " " << src->line << ": value of symbol changed: " << label << "\n";
      }
    else
      symbol_table[label]=orgpc;
    }
  if(pass && list_file)
    {
    char buf[1024];
    string v;
    const char *vs;
    if(eflag)
      {
      v=vtohex(image[orgpc]);
      vs=v.c_str();
      }
    if(eflag)
      sprintf(buf,"%3d %4.4X %-22s ",src->line,orgpc,vs);
    else
      sprintf(buf,"%3d      %-22s ",src->line,"");
    list_file << buf << s << "\n";
    }
  }

// Main

int main(int argc,char *argv[])
  {
  string s;
  int x;
  int flg=0;

  // Parse argument list

  for(x=1;x!=argc;++x)
    if(!strcmp(argv[x],"-o"))
      if(++x==argc || out_name)
        {
        flg=1;
        break;
        }
      else
        out_name=argv[x];
    else if(!strcmp(argv[x],"-l"))
      if(++x==argc || list_name)
        {
        flg=1;
        break;
        }
      else
        list_name=argv[x];
    else if(src_name)
      {
      flg=1;
      break;
      }
    else
      src_name=argv[x];

  if(flg || !src_name)
    {
    cerr << "Syntax error\n";
    cerr << "uasm [ -o output_file_name ] [ -l listing_file_name] source_file_name\n";
    return -1;
    }

  src=new Source(src_name);

  if(list_name)
    {
    list_file.open(list_name,ios::out);
    if(!list_file)
      {
      cerr << "Couldn't open " << list_name << "\n";
      return -1;
      }
    }

  if(out_name)
    {
    out_file.open(out_name,ios::out);
    if(!out_file)
      {
      cerr << "Couldn't open " << out_name << "\n";
      return -1;
      }
    }

  // First pass
  cerr << "Pass 1...\n";
  pc=0;
  pass=0;
  while(!next_line(s))
    assemble(s);

  cerr << "Width is " << width << "\n";
  cerr << "Size is " << size << "\n";
  cerr << "Fill is " << filler << "\n";
  cerr << "Pass 2...\n";

  // Malloc image
  image=new Value[size];
  for(x=0;x!=size;++x)
    image[x]=filler;

  // Second pass
  src->rewind();
  pc=0;
  pass=1;
  while(!next_line(s))
    assemble(s);

  // Dump symbol table
  if(list_file)
    {
    list_file << "\nSymbol table:\n\n";
    map<string,Value>::iterator p;
    for(p=symbol_table.begin();p!=symbol_table.end();++p)
      {
      char buf[1024];
      const char *n=p->first.c_str();
      string v=vtohex(p->second);
      const char *vs=v.c_str();
      sprintf(buf,"%+40s %s",n,vs);
      list_file << buf << "\n";
      }
    }

  cerr << "Emitting image...\n";

  // Emit image
  if(out_file)
    {
    for(x=0;x!=size;++x)
      out_file << vtohex(image[x]) << "\n";
    }

  cerr << "Done.\n";

  return 0;
  }
