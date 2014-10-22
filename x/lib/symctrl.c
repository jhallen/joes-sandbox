/* Symbolic port control */

#include <termios.h>
#include "zstr.h"
#include "tim.h"
#include "symctrl.h"

char *strattr(stuff,name)
char *stuff, *name;
 {
 static char buf[128];
 while(stuff!=(char *)1)
  if(!bcmp(stuff,name) && stuff[zlen(name)]=='=')
   {
   int x;
   stuff+=zlen(name)+1;
   for(x=0;stuff[x] && stuff[x]!=',';++x)
    buf[x]=stuff[x];
   buf[x]=0;
   return buf;
   }
  else stuff=zchr(stuff,',')+1;
 return 0;
 }

int numattr(stuff,name)
char *stuff, *name;
 {
 while(stuff!=(char *)1)
  if(!bcmp(stuff,name) && stuff[zlen(name)]=='=')
   return ston(stuff+zlen(name)+1);
  else stuff=zchr(stuff,',')+1;
 return -1;
 }

int symctrl(fd,name)
char *name;
 {
 char *s;
 struct termios attr;
 if(s=zrchr(name,':')) name=s+1;
 if(!tcgetattr(fd,&attr))
  { /* We have a tty */
  int numval;
  char *strval;

  /* Force modes we always want */
  attr.c_iflag|=(IGNBRK|IGNPAR);
  attr.c_iflag&=~(BRKINT|PARMRK|INPCK|ISTRIP|INLCR|IGNCR|IUCLC|IXANY);
  attr.c_oflag|=(OPOST|OFILL);
  attr.c_oflag&=~(OLCUC|OCRNL|ONOCR|ONLRET|OFDEL|NLDLY|CRDLY|TABDLY|BSDLY|VTDLY|FFDLY);
  attr.c_cflag|=(CREAD);
  attr.c_lflag&=~(XCASE|ECHONL|ECHOCTL|ECHOPRT|ECHOK|NOFLSH);
  attr.c_lflag|=(ECHOE|ECHOKE);

  numval=numattr(name,"stop");
  if(numval==1) attr.c_cflag&=~CSTOPB;
  else if(numval==2) attr.c_cflag|=CSTOPB;

  numval=numattr(name,"bits");
  if(numval==5) attr.c_cflag=(attr.c_cflag&~CSIZE)|CS5;
  else if(numval==6) attr.c_cflag=(attr.c_cflag&~CSIZE)|CS6;
  else if(numval==7) attr.c_cflag=(attr.c_cflag&~CSIZE)|CS7;
  else if(numval==8) attr.c_cflag=(attr.c_cflag&~CSIZE)|CS8;

  strval=strattr(name,"echo");
  if(strval)
   if(!zcmp(strval,"on"))
    attr.c_lflag|=ECHO;
   else if(!zcmp(strval,"off"))
    attr.c_lflag&=~ECHO;

  strval=strattr(name,"flow");
  if(strval)
   if(!zcmp(strval,"none"))
    attr.c_iflag&=~(IXON|IXOFF),
    attr.c_cflag&=~CRTSCTS;
   else if(!zcmp(strval,"rtscts"))
    attr.c_iflag&=~(IXON|IXOFF),
    attr.c_cflag|=CRTSCTS;
   else if(!zcmp(strval,"xonxoff"))
    attr.c_iflag|=(IXON|IXOFF),
    attr.c_cflag&=~CRTSCTS;

  strval=strattr(name,"parity");
  if(strval)
   if(!zcmp(strval,"none"))
    attr.c_cflag&=~PARENB;
   else if(!zcmp(strval,"even"))
    attr.c_cflag|=PARENB,
    attr.c_cflag&=~PARODD;
   else if(!zcmp(strval,"odd"))
    attr.c_cflag|=(PARENB|PARODD);

  strval=strattr(name,"cd");
  if(strval)
   if(!zcmp(strval,"on"))
    attr.c_cflag&=~CLOCAL;
   else if(!zcmp(strval,"off"))
    attr.c_cflag|=CLOCAL;

  strval=strattr(name,"hup");
  if(strval)
   if(!zcmp(strval,"on"))
    attr.c_cflag|=HUPCL;
   else if(!zcmp(strval,"off"))
    attr.c_cflag&=~HUPCL;

  strval=strattr(name,"trans");
  if(strval)
   if(!zcmp(strval,"none"))
    attr.c_oflag&=~ONLCR,
    attr.c_iflag&=~ICRNL;
   else if(!zcmp(strval,"unix"))
    attr.c_oflag|=ONLCR,
    attr.c_iflag|=ICRNL;

 strval=strattr(name,"sig");
 if(strval)
  if(!zcmp(strval,"on"))
   attr.c_lflag|=ISIG;
  else if(!zcmp(strval,"off"))
   attr.c_lflag&=~ISIG;

 strval=strattr(name,"edit");
 if(strval)
  if(!zcmp(strval,"on"))
   attr.c_lflag|=(ICANON|ECHO|ISIG);
  else if(!zcmp(strval,"off"))
   attr.c_lflag&=~(ICANON|ECHO|ISIG);

  numval=numattr(name,"baud");
  if(numval>=0)
   {
   int baud=B9600;
   switch(numval)
    {
    case 300: baud=B300; break;
    case 1200: baud=B1200; break;
    case 2400: baud=B2400; break;
    case 4800: baud=B4800; break;
    case 9600: baud=B9600; break;
    case 19200: baud=B19200; break;
    case 38400: baud=B38400; break;
    case 57600: baud=B57600; break;
    case 115200: baud=B115200; break;
    }
   cfsetospeed(&attr,baud);
   cfsetispeed(&attr,baud);
   }
  tcsetattr(fd,TCSADRAIN,&attr);
  }
 }

int symopen(name,mode)
char *name;
 {
 int fd;
 char buf[1024];
 char *ptr;
 zcpy(buf,name);
 if(ptr=zrchr(buf,':')) *ptr=0;
 fd=open(buf,mode);
 if(fd>=0)
  {
  symctrl(fd,"baud=9600,bits=8,stop=1,parity=none,flow=rtscts,cd=off,hup=on,trans=none,sig=off,echo=off,edit=off");
  symctrl(fd,name);
  }
 return fd;
 }
