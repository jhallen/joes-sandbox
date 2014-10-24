/* Install master boot record
   Copyright (C) 1998 Joseph H. Allen

This file is part of MBR (Master Boot Record)

MBR is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation; either version 1, or (at your option) any later version.  

MBR is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.  

You should have received a copy of the GNU General Public License along with 
MBR; see the file COPYING.  If not, write to the Free Software Foundation, 
675 Mass Ave, Cambridge, MA 02139, USA.  */ 

#include <stdio.h>

#if defined(MSDOS) || defined(__MSDOS__)

#include <dos.h>

/* Read boot sector into buffer
 * returns with 0 for success */

sectin(buf)
char *buf;
 {
 union REGS in, out;
 struct SREGS segs;
 segread(&segs);
 segs.es=segs.ds;
 in.x.ax=0x0201;
 in.x.bx=(unsigned)buf;
 in.x.cx=0x0001;
 in.x.dx=0x0080;
 return int86x(0x13,&in,&out,&segs);
 }

/* Write buffer to boot sector
 * returns with 0 for success
 */

sectout(buf)
char *buf;
 {
 union REGS in, out;
 struct SREGS segs;
 segread(&segs);
 segs.es=segs.ds;
 in.x.ax=0x0301;
 in.x.bx=(unsigned)buf;
 in.x.cx=0x0001;
 in.x.dx=0x0080;
 return int86x(0x13,&in,&out,&segs);
 }

#else

#include <unistd.h>

int fd;

sectin(buf)
char *buf;
 {
 lseek(fd,0,SEEK_SET);
 return read(fd,buf,512)!=512;
 }

sectout(buf)
char *buf;
 {
 lseek(fd,0,SEEK_SET);
 return write(fd,buf,512)!=512;
 }

#endif

/* Prototype master boot record */

unsigned char mbr[]=
 {
 51,192,142,208,188,  0,124,142,192,142,216,139,244,191,  0,  6,
185,  0,  1,252,243,165,234, 27,  6,  0,  0,184,115,  7,232, 27,
  1,190, 82,  7,232,181,  0,232,204,  0,190,138,  7,173, 60,255,
116,115,139,232,191,  0,124,128, 61,  0,116,  8,172,174,117,126,
 60,  0,117,248, 51,192,128,126,  0,128,190,190,  7,136,  4,136,
 68, 16,136, 68, 32,136, 68, 48,198, 70,  0,128,116, 39,128, 62,
137,  7,  0,116, 32,191,  5,  0,186,128,  0,185,  1,  0,184,  1,
  3,187,  0,  6,205, 19,115, 13, 51,192,205, 19, 79,117,239,190,
102,  7,232, 87,  0,191,  5,  0,139, 86,  0,139, 78,  2,184,  1,
  2,187,  0,124,205, 19,115, 48, 51,192,205, 19, 79,117,239,190,
102,  7,232, 55,  0,190, 89,  7,232, 49,  0,190,138,  7,173, 60,
255,117,  3,233,107,255,232, 35,  0,232, 38,  0,235,240, 60,  0,
117,  3,233,104,255,172,235,246,196,  6,129,  7,232,109,  0,139,
245,234,  0,124,  0,  0,190, 79,  7,232, 13,  0,172, 60,  0,117,
248,195,176, 13,232,  2,  0,176, 10,180, 14,187,  7,  0,205, 16,
195, 50,228,205, 22,195,191,  0,124,161,135,  7,131,248,  0,116,
 26,163,133,  7,131, 62,133,  7,  0,117, 10,191,  0,124,232,209,
255,198,  5,  0,195,180,  1,205, 22,116,233,232,211,255, 60, 13,
116,236, 60,  8,117, 12,129,255,  0,124,116,205, 79,232,166,255,
235,199, 60, 32,114,195,170,232,175,255,235,189,250,135,  6,112,
  0,163,129,  7,140,192,135,  6,114,  0,163,131,  7,251,195, 32,
  8,  0, 10,109, 98,114, 58, 32,  0, 10, 99,104,111,111,115,101,
 58, 32, 13, 10, 10,  0,100,105,115,107, 32,101,114,114,111,114,
 13, 10,  0, 46,131, 62,133,  7,  0,116,  5, 46,255, 14,133,  7,
234,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 85,170,
 };

/* Master boot record offsets */
#define mbrPART 0x1BE			/* Partition table 4 16-byte entries */
#define mbrNAMES 0x18A			/* Partition names table */
#define mbrNAMESZ (mbrPART-mbrNAMES)	/* Space available for names */
#define mbrDELAY 0x187			/* word: Timeout delay in ticks */
#define mbrWRITE 0x189			/* byte: Update active flag if set */
#define mbrCODE 0x181			/* Size of static code in MBR */
/* Names are stored in the boot record as follows:
 *  <partition address> <name string> \0
 *  <partition address> <name string> \0
 *  ...
 *  \xFF (terminator byte)
 *
 * <parition address> is the absolute address of the partition table
 * entry for this name.  The boot sector copies itself to 0x600, so
 * the partition table begins at 0x7BE.  This is just offset part of
 * the address, the segment part is assumed to be zero.
 *
 * The first name in the table is the default name for timeouts and
 * when you hit enter at a blank prompt.
 */

/* Partition table entry offsets */
#define partACTIVE 0		/* Active flag */
#define partBEGHEAD 1		/* Starting head */
#define partBEGSECT 2		/* Beginning sect in lower 6 bits
				   Upper two bits of beginning cylinder
				   in upper 2 bits */
#define partBEGCYL 3		/* Beginning cylinder */
#define partID 4		/* Part ID code */
#define partENDHEAD 5		/* Ending head */
#define partENDSECT 6		/* Ending sector */
#define partENDCYL 7		/* Ending cylinder */
#define partSTART 8		/* Long: no. sectors before partition */
#define partSIZE 12		/* Size of partition */

/* Operating system ID codes */
struct
 {
 char *name;
 int code;
 } ids[]=
 {
  { "DOS 12-bit FAT", 1 }, { "Xenix root", 2 }, { "Xenix usr", 3 },
  { "DOS 16-bit <32MB", 4 }, { "Extended", 5 }, { "DOS 16-bit >=32MB", 6 },
  { "OS/2 HPFS", 7 }, { "AIX", 8 }, { "AIX bootable", 9 },
  { "OS/2 boot mangr", 0x0A }, { "Venix 80286", 0x40 }, { "Novell?", 0x51 },
  { "Microport", 0x52 }, { "GNU HURD", 0x63 }, { "Novell netware", 0x64 },
  { "Novell netware", 0x65 }, { "PC/IX", 0x70 }, { "Old MINIX", 0x80 },
  { "Linux/MINIX", 0x81 }, { "Linux swap",0x82 }, { "Linux",0x83 },
  { "Amoeba", 0x93 }, { "Amoeba BBT", 0x94 }, { "BSD/386", 0xa5 },
  { "BSDI", 0xb7 }, { "BSDI swap", 0xb8 }, { "Syrinx", 0xc7 },
  { "CP/M", 0xdb }, { "DOS access", 0xe1 }, { "DOS r/o", 0xe3 },
  { "DOS secondary", 0xf2 }, { "BBT", 0xFF }, { 0 }
 };

/* Lookup ID code */

char *lookup(code)
 {
 int x;
 for(x=0;ids[x].name;++x) if(ids[x].code==code) return ids[x].name;
 return "unknown";
 }

/* Structure to hold partition table data */

struct partentry
 {
 int active;		/* Set if active */
 int beghead;		/* Starting head */
 int begsect;		/* Starting sector */
 int begcyl;		/* Starting cylinder */
 int id;		/* ID code */
 char *str;		/* ID string */
 int endhead;		/* Ending head */
 int endsect;		/* Ending sector */
 int endcyl;		/* Ending cylinder */
 unsigned long start;	/* Sectors before partition */
 unsigned long size;	/* Size of partition */
 char name[64];		/* Our name for the partition */
 int def;		/* Set if this partition should boot by default */
 } entries[4];

/* Read partition table from buffer */

void getpart(sect)
unsigned char *sect;
 {
 int x;
 int flg=0;
 unsigned char *buf=sect+mbrPART;
 for(x=0;x!=4;++x,buf+=16)
  {
  entries[x].active=buf[partACTIVE];
  entries[x].beghead=buf[partBEGHEAD];
  entries[x].begsect=buf[partBEGSECT]&63;
  entries[x].begcyl=buf[partBEGCYL]+((buf[partBEGSECT]&0xC0)<<2);
  entries[x].id=buf[partID];
  entries[x].endhead=buf[partENDHEAD];
  entries[x].endsect=buf[partENDSECT]&63;
  entries[x].endcyl=buf[partENDCYL]+((buf[partENDSECT]&0xC0)<<2);
  entries[x].start=(unsigned long)buf[partSTART]+
                   ((unsigned long)buf[partSTART+1]<<8)+
                   ((unsigned long)buf[partSTART+2]<<16)+
                   ((unsigned long)buf[partSTART+3]<<24);
  entries[x].size=(unsigned long)buf[partSIZE]+
                  ((unsigned long)buf[partSIZE+1]<<8)+
                  ((unsigned long)buf[partSIZE+2]<<16)+
                  ((unsigned long)buf[partSIZE+3]<<24);
  entries[x].str=lookup(entries[x].id);
  entries[x].name[0]=0;
  entries[x].def=0;
  }
 x=mbrNAMES;
 while(sect[x]!=0xFF)
  {
  int y=sect[x]+(sect[x+1]<<8);
  x+=2;
  y-=0x7BE;
  y/=16;
  strcpy(entries[y].name,sect+x);
  x+=strlen(sect+x)+1;
  if(!flg) flg=entries[y].def=1;
  }
 }

/* Stick names from entries into mbr buffer */

void inst(sect)
unsigned char *sect;
 {
 int x, n;
 int at=mbrNAMES;			/* Name table begins here */
 for(x=0;!entries[x].def;++x);		/* Find default partition */
 for(n=0;n!=4;++n)			/* Copy the names */
  {
  if(entries[x].name[0])
   {
   int ad=16*x+0x7BE;
   sect[at++]=ad; sect[at++]=ad>>8;
   strcpy(sect+at,entries[x].name);
   at+=strlen(entries[x].name)+1;
   }
  if(++x==4) x=0;
  }
 sect[at]=0xFF;
 }

/* Show partition table */

void showpart(sect)
unsigned char *sect;
 {
 int x;
 printf("\n");
 printf("No.    Size   ID Type                 Default Name\n");
 printf("--- --------- -- -------------------- ------- ----\n");
 for(x=0;x!=4;++x) if(entries[x].id)
  printf(" %d  %6lu MB %2x %-20s    %c    %s\n",x+1,
         entries[x].size/2048L,
         entries[x].id,entries[x].str,(entries[x].def?'*':' '),entries[x].name);
 printf("\n");
 x=sect[mbrDELAY]+sect[mbrDELAY+1]*256;
 if(x==1) printf("- Boot prompt times out instantly\n");
 else if(x) printf("- Boot prompt times out in %d secs\n",x/18);
 else printf("- Boot prompt never times out\n");
 if(sect[mbrWRITE]) printf("- Partition table write-back enabled\n");
 else printf("- Partition table write-back disabled\n");
 printf("\n");
 }

unsigned char org[512];	/* Original master boot record */
unsigned char new[512];	/* New master boot record */

main(argc,argv)
char *argv[];
 {
 char buf[80];
 int x;

#if !defined(MSDOS) && !defined(__MSDOS__)
 if(argc>2 || argc==2 && (argv[1][0]=='-' || argv[1][0]=='?'))
  {
  printf("mbr [drive]\n");
  return 0;
  }
 else if(argc==2) fd=open(argv[1],2);
 else
  {
  printf("Using /dev/hda by default\n");
  fd=open("/dev/hda",2);
  }
 if(fd==-1)
  {
  printf("Could not open drive\n");
  return 1;
  }
#endif

 /* Read current master boot record */
 if(sectin(org))
  {
  printf("Error reading partition table from hard drive\n");
  return 1;
  }

 /* Copy partition table into master boot record */
 do
  {
  /* Get prototype */
  for(x=0;x!=512;++x) new[x]=mbr[x];
  /* Check if we've already been installed */
  for(x=0;x!=mbrCODE;++x) if(new[x]!=org[x]) break;
  if(x==mbrCODE)
   /* Copy name table too */
   for(x=mbrCODE;x!=510;++x) new[x]=org[x];
  else
   /* Copy only partition table */
   for(x=0;x!=64;++x) new[x+mbrPART]=org[x+mbrPART];

  getpart(new);
  printf("\nParition table:\n");
  showpart(new);

  do
   {
   int len;
   do
    {
    len=0;
    for(x=0;x!=4;++x)
     if(entries[x].id)
      {
      printf("Enter name for partition %d: ",x+1);
      fgets(entries[x].name,63,stdin);
      if(entries[x].name[0] &&
         entries[x].name[strlen(entries[x].name)-1]=='\n')
       entries[x].name[strlen(entries[x].name)-1]=0;
      if(entries[x].name[0]) len+=strlen(entries[x].name)+3;
      entries[x].def=0;
      }
     else entries[x].name[0]=entries[x].def=0;
    ++len;
    if(len>mbrNAMESZ)
     printf("\n\aNames too long (maximum of %d chars total).  Pick shorter ones.\n\n",mbrNAMESZ-13);
    else if(len==1)
     printf("\n\aYou can not boot unless you name a partition\n\n");
    } while(len>mbrNAMESZ || len==1);
   showpart(new);
   printf("Names look ok (y,n)? ");
   fgets(buf,79,stdin);
   }
   while(buf[0]!='y' && buf[0]!='Y');

  do
   {
   printf("\nEnter name of partition which should boot by default: ");
   fgets(buf,63,stdin);
   if(buf[0] && buf[strlen(buf)-1]=='\n') buf[strlen(buf)-1]=0;
   for(x=0;x!=4;++x) if(!strcmp(entries[x].name,buf)) break;
   if(x==4) printf("\n\aNo such partition\n"), showpart(new);
   } while(x==4);

  entries[x].def=1;

  printf("\nSeconds to wait at prompt before booting default (0=no delay, -=forever): ");
  fgets(buf,63,stdin);
  if(buf[0]=='-') x=0;
  else
   {
   x=atoi(buf)*18;
   if(!x) x=1;
   }
  new[mbrDELAY]=x;
  new[mbrDELAY+1]=(x>>8);

  printf("\nSome operating systems require the active flag to be properly\n");
  printf("set so that it matches the partition you're actually booting.\n");
  printf("For example, you need this if you are using SCO Xenix or if you\n");
  printf("have more than one bootable DOS partition.  Would you like the\n");
  printf("master boot record to write the partition table with the correctly\n");
  printf("set active flag back to the drive whenever a different partition\n");
  printf("is selected for booting (y,n)? ");

  fgets(buf,63,stdin);
  if(buf[0]=='y' || buf[0]=='Y') new[mbrWRITE]=1;
  else new[mbrWRITE]=0;

  printf("\nOk, please review everything one more time...\n");
  inst(new);
  getpart(new);
  showpart(new);
  printf("\nWrite master boot block (y,n)? ");
  fgets(buf,63,stdin);
  } while(buf[0]!='y' && buf[0]!='Y');

 /* Write new master boot record */
 if(sectout(new))
  {
  printf("Error writing new master boot record\n");
  return 1;
  }

 printf("Master boot record installed\n");
 return 0;
 }
