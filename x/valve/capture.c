/* Capture board device driver */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/mman.h>

extern int errno;

/* Read long from I/O port */

inline unsigned inl(unsigned short port)
 {
 unsigned v;
 __asm__ __volatile__ ("inl %%dx,%%eax":"=a" (v):"d" (port));
 return v;
 }

/* Write long to I/O port */

inline void outl(unsigned short port,unsigned v)
 {
 __asm__ __volatile__ ("outl %%eax,%%dx"::"a" (v),"d" (port));
 }

/* Read long from configuration space at slot #, address */

unsigned gtconfig(slot,reg)
 {
 outl(0xCF8,(slot<<11)|0x80000000|reg);
 return inl(0xCFC);
 }

/* Write long to configuration space at slot #, address */

void stconfig(slot,reg,val)
 {
 outl(0xCF8,(slot<<11)|0x80000000|reg);
 outl(0xCFC,val);
 }

/* ms delay */

void dly(n)
 {
 struct timeval tv;
 tv.tv_sec=0;
 tv.tv_usec=n*1000;
 select(0,NULL,NULL,NULL,&tv);
 }

fred() { }

int slot;			/* Slot no. capture card is in */
unsigned addr=0xD0000000;	/* Physical address of card */
int *mem;			/* Virtual address of card */
int fd;				/* file descriptor for /dev/mem */

/* Register values */
int reg6=0x40;			/* Field detect offset */
int reg12=0xA1;			/* Video source, oscillator freq. */
int reg13=0x08;			/* DAC serial bits, oscillator enable bit */
int reg14=0x00;			/* Clocking enable, clocking ram bank */
int reg7=0;
int cursync=0;			/* Current sync source */

#define cardid 0x5123		/* Device ID for our card */

/* Find capture card and load configuration data if needed.  If 'reload' is
 * set, reload is forced.  Records values in slot#, addr, and mem above. */

void setup(reload)
 {
 unsigned n;
 if(mem) return;
 fd=open("/dev/mem",O_RDWR);
 if(fd== -1)
  fprintf(stderr,"Couldn't open /dev/mem\n"), exit(1);
 if(iopl(3))
  fprintf(stderr,"Couldn't get permissions (need to be root)\n"), exit(1);
 if(!reload)
  {
  for(slot=0;slot!=32;++slot)
   if(gtconfig(slot,0)==cardid)
    {
    fprintf(stderr,"board found in slot %d\n",slot);
    addr=gtconfig(slot,0x10);
    mem=mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_FILE|MAP_SHARED,fd,addr);
    if(mem==(int *)0xFFFFFFFF || mem==0)
     {
     fprintf(stderr,"mmap error\n");
     exit(1);
     }
    mem[6]=reg6;
    mem[12]=reg12;
    mem[13]=reg13;
    mem[14]=reg14;
    return;
    }
  }
 for(slot=0;slot!=32;++slot)
  if((n=gtconfig(slot,0)), n==0xFFFFFFFF || n==cardid)
   {
   char buf[65536];
   FILE *f;
   int len;
   int x,y,z;
   int code=14;
   f=fopen("fpga.bin","r");
   if(!f) fprintf(stderr,"Couldn't open fpga file\n"), exit(1);
   len=fread(buf,1,65536,f);
   fprintf(stderr,"Writing %d bytes to slot %d\n",len,slot);

   /* Reset fpga */
   code&=~4;
   for(y=0;y!=512;++y) stconfig(slot,64+code*4,0);
   code|=4;
   for(y=0;y!=512;++y) stconfig(slot,64+code*4,0);

   dly(40);

   for(x=0;x!=len;++x)
    for(z=1;z!=256;z<<=1)
     {
     /* Data bit */
     if(buf[x]&z) code|=1;
     else code&=~1;
     stconfig(slot,64+code*4,0);

     /* Pulse clock */
     code&=~2;
     stconfig(slot,64+code*4,0);
     code|=2;
     stconfig(slot,64+code*4,0);
     }

   dly(40);

   if(gtconfig(slot,0)==cardid)
    {
    fprintf(stderr,"Board found in slot %d\n",slot);
    stconfig(slot,0x04,0x20000002);
    stconfig(slot,0x10,addr);
    mem=mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_FILE|MAP_SHARED,fd,addr);
    if(mem==(int *)0xFFFFFFFF || mem==0)
     {
     fprintf(stderr,"mmap error\n");
     exit(1);
     }
    mem[6]=reg6;
    mem[12]=reg12;
    mem[13]=reg13;
    mem[14]=reg14;
    return;
    }
   }
 fprintf(stderr,"Board not found\n");
 exit(-1);
 }

#define oscfreq 600000000

setupclock(freq,clks,ofst,phase)
 {
 unsigned char clk[2048*3];
 int y;
 int accu;
 int flg=0;

 for(y=0;y!=2048*3;++y) clk[y]=0;

 accu=oscfreq-phase*(oscfreq/360);
 printf("starting accu=%d\n",accu);
 for(y=0;y!=2048*3*8;++y)
  {
  if((accu-=freq)<0)
   {
   accu+=oscfreq;
   if(ofst) --ofst;
   else if(clks)
    {
    flg=2;
    --clks;
    }
   }
  if(flg)
   {
   clk[y>>3]|=(1<<(y&7));
   --flg;
   }
  }

 /* Clear counter; disable clocking; sync=0 */
 mem[14]=0x98;
 fred(mem[8]); fred(mem[8]);
 mem[14]=0x18;
 fred(mem[8]); fred(mem[8]); /* Delay to allow finishing clearing */

 /* Program clocking scheme */

 for(y=0;y!=2047;++y)
  {
  mem[8]=clk[y*3];
  mem[9]=clk[y*3+1];
  mem[10]=clk[y*3+2];
  }

 fred(mem[8]); fred(mem[8]); /* Delay to allow write to finish */

 /* Enable clocking; enable sync */
 /* Causes an immediate clear */
 reg14=(0x40|(cursync<<3));
 mem[14]=reg14;

 /* Make sure clock is stable before initializing SDRAMs */
 dly(10);

 /* Initialize SDRAMs */
 fred(mem[4]);
 }

/* Continuous clock */

contclock(freq)
 {
 unsigned char clk[2048*3];
 int y;
 int accu;
 int flg=0;

 for(y=0;y!=2048*3;++y) clk[y]=0;

 accu=oscfreq;
 for(y=0;y!=2047*3*8;++y)
  {
  if((accu-=freq)<0)
   {
   accu+=oscfreq;
   if(y!=2047*3*8-1) flg=2;
   }
  if(flg)
   {
   clk[y>>3]|=(1<<(y&7));
   --flg;
   }
  }

 /* Clear counter; disable clocking */
 mem[14]=0x98;
 mem[14]=0x18;
 fred(mem[8]);

 /* Program clocking scheme */

 for(y=0;y!=2047;++y)
  {
  mem[8]=clk[y*3];
  mem[9]=clk[y*3+1];
  mem[10]=clk[y*3+2];
  }

 mem[8]=clk[0];
 mem[9]=clk[1];
 mem[10]=clk[2];

 mem[14]=0x98;
 mem[14]=0x18;
 fred(mem[8]);

 /* Enable clocking- no resync */
 reg14=0x58;
 mem[14]=reg14;

 dly(10);

 /* Initialize SDRAMs */
 fred(mem[4]);
 }

izd()
 {
 mem[14]=reg14=0;
 reg13&=~2;
 mem[13]=reg13;
 fred(mem[4]);
 }

/* Set DAC values */

void dac(gain,level)
 {
 int a;
 unsigned x;
 int y;
 a=(gain&0xFFFF)|(level<<16);
 mem[13]=reg13;
 for(y=0,x=0x80000000;y!=32;++y,(x>>=1))
  {
  if(a&x) reg13|=1;
  else reg13&=~1;
  mem[13]=reg13|4;
  mem[13]=reg13;
  }
 mem[13]=(reg13&~8);
 mem[13]=(reg13|8);
 }

/* Capture an image into a buffer */

int rrr=0;

cap(buf,w,h,skip)
int *buf;
 {
 int n;
 int x;
 unsigned long input;
 fprintf(stderr,"Capturing...");
 mem[4]=0;
 mem[5]=0;

 dly(60);
 fprintf(stderr,"done.  Reading...");

/* contclock(50000000); */
 fred(mem[4]);
 mem[4]=0;

 dly(10);

 n=0;
 for(x=0;x!=4;x+=4)
  {
  input=mem[n++];
  if(n==4) n=0;
  }
 for(x=0;x<w*skip;x+=4)
  {
  input=mem[n++];
  if(n==4) n=0;
  }
 for(x=0;x<w*h;x+=4)
  {
  *buf++=mem[n++];
  if(n==4) n=0;
  }
 fprintf(stderr,"done.\n");
 }

void videosource(n)
 {
 reg12&=~0x38;
 switch(n)
  {
  case 0: reg12|=0x20; break;
  case 1: reg12|=0x28; break;
  case 2: reg12|=0x10; break;
  case 3: reg12|=0x18; break;
  }
 mem[12]=reg12;
 }

void hsyncsource(n)
 {
 cursync=n;
 }

void vsyncsource(n)
 {
 reg7=n;
 mem[7]=reg7;
 }

void vfieldoffset(n)
 {
 reg6=4095-n;
 mem[6]=reg6;
 }

#if 0
main(argc,argv)
char *argv[];
 {
 char *buf;
 int freq=atoi(argv[1]);
 int x;

 int w=640;

 setup(0);			/* Find card */

 videosource(0);		/* Set video source */

 syncsource(0);			/* Set sync source */

 mem[7]=1;			/* Use 60Hz signal */

/* contclock(freq); */


 setupclock(freq*1,w,80,270);	/* Set clocking scheme */

 dac(300,325);			/* Set up video levels */

 buf=malloc(w*480);

 cap(buf,w,480,46);		/* Capture an image */
 
 printf("%d\n480\n",w);
 for(x=0;x!=w*480;++x) putchar(buf[x]);

 return 0;
 }
#endif
