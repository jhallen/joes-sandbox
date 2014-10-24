// TERMCAP/TERMINFO database interface

// Fixup terminfo stuff

#include "util.h"
#include "parse.h"
#include "termcap.h"

#define JOERC "/usr/lib/joe"
char *joeterm=0;

#ifdef TERMINFO
extern char *tgoto();
extern char *tgetstr();
#endif

/* Default termcap entry */

static char defentry[]=
"\
:co#80:li#25:am:\
:ho=\\E[H:cm=\\E[%i%d;%dH:cV=\\E[%i%dH:\
:up=\\E[A:UP=\\E[%dA:DO=\\E[%dB:nd=\\E[C:RI=\\E[%dC:LE=\\E[%dD:\
:cd=\\E[J:ce=\\E[K:cl=\\E[H\\E[J:\
:so=\\E[7m:se=\\E[m:us=\\E[4m:ue=\\E[m:\
:mb=\\E[5m:md=\\E[1m:mh=\\E[2m:me=\\E[m:\
:ku=\\E[A:kd=\\E[B:kl=\\E[D:kr=\\E[C:\
:al=\\E[L:AL=\\E[%dL:dl=\\E[M:DL=\\E[%dM:\
:ic=\\E[@:IC=\\E[%d@:dc=\\E[P:DC=\\E[%dP:\
";

// Add contents of termcap entry to capability database

void Cap::add(map<String,String>& caps,String entry)
  {
  int x,y,z;
  for(x=0;x!=entry.size();x=y)
    {
    // Skip to first :
    x=entry.find(':',x);
    if(x==entry.npos) break;		// We're done
    ++x;
    // Skip to terminating : or newline
    y=entry.find_first_of(":\n",x);
    if(y==entry.npos) break;		// We're done
    if(entry[y]==':')
      { // entry[x..y] has the capability string
      if(y>x)
        { // Ignoring blank entries
        String sub=entry.substr(x,y-x);
        if((z=sub.find('=',0))!=sub.npos)
          { // String capability
          String name=sub.substr(0,z);
          String value=sub.substr(z+1,sub.size()-(z+1));
          caps[name]=value;
          }
        else if((z=sub.find('#',0))!=sub.npos)
          { // Numeric capability
          String name=sub.substr(0,z);
          String value=sub.substr(z+1,sub.size()-(z+1));
          caps[name]=value;
          }
        else if((z=sub.find('@',0))!=sub.npos)
          { // Delete a capability
          String name=sub.substr(0,z);
          caps[name]="DELETE";
          }
        else
          { // Flag
          caps[sub]="";
          }
        }
      }
    }
  }

/* Return true if termcap line matches name */

int Cap::match(String s,String name)
  {
  if(!s.size() || s[0]=='#' || s[0]==' ' || s[0]=='\t')
    return 0;
  for(int y=0;;)
    {
    int x;
    for(x=0;x+y!=s.size() && x!=name.size() && s[x+y]==name[x];++x);
    if(x==name.size() && x+y!=s.size() && (s[x+y]==':' || s[x+y]=='|'))
      return 1;
    while(x+y!=s.size() && s[x+y]!=':' && s[x+y]!='|')
      ++x;
    if(x+y==s.size() || s[x+y]==':')
      break;
    y+=x+1;
    }
  return 0;
  }

// Find termcap entry in a file and extract it

int Cap::find(map<String,String>& caps,fstream& f,String name)
  {
  String s;
  while(!f.eof())
    {
    getline(f,s);
    if(match(s,name))
      {
      add(caps,s);
      while(!f.eof())
        {
        getline(f,s);
        if(s.size() && s[0]!='#')
          {
          if(s[0]!=' ' && s[0]!='\t') break;
          add(caps,s);
          }
        }
      return 1;
      }
    }
  return 0;
  }

// Link

int Cap::link(map<String,String>& caps,list<String>& files,list<String>::iterator np)
  {
  map<String,String>::iterator p;
  if((p=caps.find("tc"))!=caps.end())
    {
    map<String,String> prev;
    if(listfind(prev,files,np,p->second))
      return -1;
    for(p=caps.begin();p!=caps.end();++p)
      prev[p->first]=p->second;
    caps=prev;
    }
  return 0;
  }

// Find termcap entry in name list

int Cap::listfind(map<String,String>& caps,list<String>& files,list<String>::iterator np,String name)
  {
  fstream f;
  while(np!=files.end())
    {
    f.open(np->c_str(),ios::in);
    if(f)
      {
      if(find(caps,f,name))
        {
        f.close();
        return link(caps,files,np);
        }
      else
        f.close();
      }
    ++np;
    }
  return -1;
  }

// Remove deleted entries

void Cap::fixup(map<String,String>& caps)
  {
  map<String,String>::iterator i, j;
  for(i=caps.begin();i!=caps.end();i=j)
    {
    j=++i;
    if(i->second=="DELETE")
      caps.erase(i);
    }
  }

/* Load termcap entry */

Cap::Cap(ostream& new_out,char *name,unsigned ibaud,int idopadding) :
  out(new_out)
  {
  String namebuf;		// List of termcap file names
  list<String> npbuf;		// Above list converted to individual strings
  String entry;
  char *s;
  int rtn;

  // Get terminal type name
  if(name || (name=joeterm) || (name=getenv("TERM")))
    {
    abuf=0;

#ifdef TERMINFO
    abuf=(char *)malloc(4096);
    abufp=cap->abuf;
    if(tgetent(cap->tbuf,name)==1)
      {
      stbaud(ibaud);
      stpadding(idopadding);
      return;
      }
    else
      {
      free(abuf);
      abuf=0;
      cerr << "Couldn't load terminfo entry\n";
      exit(1);
      }
#endif

    // Build list of file names to search for termcap entry

    // Handle the case where there is a termcap entry in the TERMCAP
    // environment variable

    s=getenv("TERMCAP");

    if(s && s[0]=='/') namebuf+=s;
    else
      {
      if(s) entry+=s;
      if((s=getenv("TERMPATH"))) namebuf+=s;
      else
        {
        if((s=getenv("HOME")))
          namebuf+=s,
          namebuf+='/';
        else
          namebuf="";
        namebuf+=".termcap ";
        namebuf+=JOERC;
        namebuf+="termcap /etc/termcap";
        }
      }

    // Convert name list into name array

    npbuf=parse_words(namebuf);

    // Scan list of files

    if(match(entry,name))
      rtn=link(caps,npbuf,npbuf.begin());
    else
      rtn=listfind(caps,npbuf,npbuf.begin(),name);
    }
  else
    {
    cerr << "Couldn't determine terminal name\n";
    rtn=0;
    }

  if(rtn)
    {
    // Use default
    cerr << "Couldn't load termcap entry\n";
    cerr << "Using default ANSI entry\n";
    add(caps,defentry);
    }
  caps["tc"]="DELETE";
  fixup(caps);

  if(getflag("pc"))
    pad=getstr("pc");
  else
    pad+='\0';
  stbaud(ibaud);
  stpadding(idopadding);
  }

void Cap::stbaud(unsigned ibaud)
  {
  baud=ibaud;
  div=100000/ibaud;
  }

void Cap::stpadding(int flg)
  {
  dopadding=flg;
  }

int Cap::getflag(String name)
  {
#ifdef TERMINFO
  return tgetflag(name.c_str());
#else
  map<String,String>::iterator p;
  p=caps.find(name);
  if(p!=caps.end())
    return 1;
  else
    return 0;
#endif
  }

String Cap::getstr(String name)
  {
  String x;
#ifdef TERMINFO
  return tgetstr(name.c_str(),&abufp);
#else
  map<String,String>::iterator p;
  p=caps.find(name);
  if(p!=caps.end())
    return p->second;
  else
    return x;
#endif
  }

int Cap::getnum(String name)
  {
#ifdef TERMINFO
  return tgetnum(name.c_str());
#else
  map<String,String>::iterator p;
  p=caps.find(name);
  if(p!=caps.end())
    return val(p->second);
  else
    return -1;
#endif
  }

Cap::~Cap()
  {
  if(abuf) free(abuf);
  }

char Cap::escape(String& s,int& idx)
  {
  char c=s[idx++];
  if(c=='^' && idx<s.size())
    if(s[idx]!='?')
      return 037&s[idx++];
    else
      return ++idx, 127;
  else if(c=='\\' && idx<s.size())
    switch(c=s[idx++])
      {
      case '0': case '1': case '2': case'3':
      case '4': case '5': case '6': case '7':
        c-='0';
        if(idx<s.size() && s[idx]>='0' && s[idx]<='7') c=(c<<3)+s[idx++]-'0';
        if(idx<s.size() && s[idx]>='0' && s[idx]<='7') c=(c<<3)+s[idx++]-'0';
        return c;
      case 'e':
      case 'E':
        return 27;
      case 'n':
      case 'l':
        return 10;
      case 'r':
        return 13;
      case 't':
        return 9;
      case 'b':
        return 8;
      case 'f':
        return 12;
      case 's':
        return 32;
      default:
        return c;
      }
  else
    return c;
  }

void Cap::texec(ostream& out,String s,int l,int a0,int a1,int a2,int a3)
  {
  int idx;
  int c, tenth=0, x;
  int args[4];
  int vars[128];
  int *a=args;

#ifdef TERMINFO
  if(cap->abuf)
    {
    char *a;
    outcap=cap;
    a=tgoto(s,a1,a0);
    tputs(a,l,outout);
    return;
    }
#endif

  /* Copy args into array (yuk) */
  args[0]=a0; args[1]=a1; args[2]=a2; args[3]=a3;

  idx=0;
  /* Get tenths of MS of padding needed */
  while(idx<s.size() && s[idx]>='0' && s[idx]<='9')
    tenth=tenth*10+s[idx++]-'0';
  tenth*=10;
  if(idx+1<s.size() && s[idx]=='.')
    {
    ++idx;
    tenth+= s[idx++]-'0';
    }

  /* Check if we have to multiply by number of lines */
  if(idx<s.size() && s[idx]=='*')
    ++idx, tenth*=l;

  /* Output string */
  while(idx<s.size())
    {
    c=s[idx];
    if(c=='%' && (idx+1)<s.size())
      {
      ++idx;
      switch(x=a[0], c= escape(s,idx))
        {
        case 'C':
          if(x>=96) out << (char)(x/96), x%=96;
        case '+':
          if(idx<s.size()) x+= escape(s,idx);
        case '.':
          out << (char)x; ++a; break;
        case 'd':
          if(x<10) goto one;
        case '2':
          if(x<100) goto two;
        case '3':
          c='0'; while(x>=100) ++c, x-=100; out << (char)c;
        two:
          c='0'; while(x>=10) ++c, x-=10; out << (char)c;
        one:
          out << (char)('0'+x); ++a; break;
        case 'r':
          a[0]=a[1]; a[1]=x; break;
        case 'i':
          ++a[0]; ++a[1]; break;
        case 'n':
          a[0]^=0140; a[1]^=0140; break;
        case 'm':
          a[0]^=0177; a[1]^=0177; break;
        case 'f':
          ++a; break;
        case 'b':
          --a; break;
        case 'a':
          c=escape(s,idx);
          if(escape(s,idx)=='p')
            x=a[escape(s,idx)-0100];
          else
            x=escape(s,idx);
          switch(c)
            {
            case '+':
              a[0]+=x; break;
            case '-':
              a[0]-=x; break;
            case '*':
              a[0]*=x; break;
            case '/':
              a[0]/=x; break;
            case '%':
              a[0]%=x; break;
            case 'l':
              a[0]=vars[x]; break;
            case 's':
              vars[x]=a[0]; break;
            default:
              a[0]=x;
            }
          break;
        case 'D':
          a[0]=a[0]-2*(a[0]&15); break;
        case 'B':
          a[0]=16*(a[0]/10)+a[0]%10; break;
        case '>':
          if(a[0]>escape(s,idx)) a[0]+=escape(s,idx); else escape(s,idx);
        default:
          out << '%';
          out << (char)c;
        }
      }
    else
      out << (char)escape(s,idx);
    }

  /* Output padding characters */
  if(dopadding)
    while(tenth>=div)
      {
      out << pad;
      tenth-=div;
      }
  }

int Cap::tcost(String s,int l,int a0,int a1,int a2,int a3)
  {
  if(s)
    {
    oStringstream z;
    texec(z,s,l,a0,a1,a2,a3);
    return z.str().size();
    }
  else
    return 10000;
  }

String Cap::tcompile(String s,int l,int a0,int a1,int a2,int a3)
  {
  if(s)
    {
    oStringstream z;
    texec(z,s,l,a0,a1,a2,a3);
    return z.str();
    }
  else
    return String();
  }
