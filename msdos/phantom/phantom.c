/* Joe's Partition Archiver */

#include <stdio.h>
#include <dos.h>
#include "lz78.h"

extern void *malloc(int);

/* Transfer buffer */

#define sectsz 512			/* Sector size in bytes */
#define sectshft 9			/* log2(sectsz) */
#define bigbufsects 31			/* No. sectors in transfer buffer */
#define bigbufsz (sectsz*bigbufsects)	/* Transfer buffer size in bytes */

unsigned char *bigbuf;
unsigned char *bigbuf1;

/* Read sectors.  Returns 0 for success.  This function can not read across
 * track boundaries.  'drive' is numbered as follows: 0=floppy A, 1=floppy B,
 * 128=first hard drive, 129=second hard drive, etc. */

int getsects(char *buf,
             int nsects,
             unsigned cyl,
             unsigned head,
             unsigned sect,
             unsigned drive)
 {
 union REGS in, out;
 struct SREGS segs;
 segread(&segs);
 segs.es=FP_SEG(buf);
 in.x.ax=0x0200+nsects;
 in.x.bx=FP_OFF(buf);
 in.x.cx=((cyl&0xFF)<<8)+((cyl>>8)<<6)+sect+1;
 in.x.dx=drive+(head<<8);
 return int86x(0x13,&in,&out,&segs);
 }

/* Write sectors.  Returns 0 for success.  This function can not write across
 * track boundaries.  'drive' is numbered as follows: 0=floppy A, 1=floppy B,
 * 128=first hard drive, 129=second hard drive, etc. */

int putsects(char *buf,int nsects,unsigned cyl,unsigned head,unsigned sect,unsigned drive)
 {
 union REGS in, out;
 struct SREGS segs;
 segread(&segs);
 segs.es=FP_SEG(buf);
 in.x.ax=0x0300+nsects;
 in.x.bx=FP_OFF(buf);
 in.x.cx=((cyl&0xFF)<<8)+((cyl>>8)<<6)+sect+1;
 in.x.dx=drive+(head<<8);
 return int86x(0x13,&in,&out,&segs);
 }

/* Master boot record offsets */
#define mbrPART 0x1BE			/* Partition table 4 16-byte entries */

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
  { "DOS 12-bit FAT", 1 },
  { "Xenix root", 2 },
  { "Xenix usr", 3 },
  { "DOS 16-bit <32MB", 4 },
  { "Extended", 5 },
  { "DOS 16-bit >=32MB", 6 },
  { "HPFS / NTFS", 7 },
  { "AIX boot", 8 },
  { "AIX data", 9 },
  { "OS/2 boot mangr", 0x0A },
  { "Win95 FAT32",0x0b},
  { "Win95 FAT32 (LBA)",0x0c},
  { "Win95 FAT16 (LBA)",0x0e},
  { "Win95 Extended (LBA)",0x0f},
  { "OPUS",0x10},
  { "Hidden DOS FAT12",0x11},
  { "Compaq diagnostics",0x12},
  { "Hidden DOS FAT16",0x14},
  { "Hidden DOS FAT16 (big)",0x16},
  { "Hidden HPFS/NTFS",0x17},
  { "AST Windows swapfile",0x18},
  { "NEC DOS",0x24},
  { "PartitionMagic recovery",0x3c},
  { "Venix 80286", 0x40 },
  { "Linux/MINIX (sharing disk with DRDOS)",0x41},
  { "SFS or Linux swap (sharing disk with DRDOS)",0x42},
  { "Linux native (sharing disk with DRDOS)",0x43},
  { "DM (disk manager)",0x50},
  { "DM6 Aux1 (or Novell)",0x51},
  { "CP/M or Microport SysV/AT",0x52},
  { "DM6 Aux3",0x53},
  { "DM6",0x54},
  { "EZ-Drive (disk manager)",0x55},
  { "Golden Bow (disk manager)",0x56},
  { "Priam Edisk (disk manager)",0x5c},
  { "SpeedStor",0x61},
  { "GNU HURD or Mach or Sys V/386 (such as ISC UNIX)",0x63},
  { "Novell netware 286", 0x64 },
  { "Novell netware 386", 0x65 },
  { "DiskSecure Multi-Boot",0x70},
  { "PC/IX",0x75},
  { "QNX4.x",0x77},
  { "QNX4.x 2nd part",0x78},
  { "QNX4.x 3rd part",0x79},
  { "Old MINIX", 0x80 },
  { "Linux/MINIX", 0x81 },
  { "Linux swap",0x82 },
  { "Linux",0x83 },
  { "OS/2 hidden C: drive",0x84},
  { "Linux extended",0x85},
  { "NTFS volume set",0x86},
  { "NTFS volume set",0x87},
  { "Amoeba", 0x93 }, { "Amoeba BBT", 0x94 },
  { "IBM Thinkpad hibernation",0xa0}, /* according to dan@fch.wimsey.bc.ca */
  { "BSD/386", 0xa5 },
  { "NeXTSTEP 486",0xa7},
  { "BSDI", 0xb7 }, { "BSDI swap", 0xb8 },
  { "DRDOS/sec (FAT-12)",0xc1},
  { "DRDOS/sec (FAT-16, < 32M)",0xc4},
  { "DRDOS/sec (FAT-16, >= 32M)",0xc6},
  { "Syrinx", 0xc7 },
  { "DOS access or SpeedStor 12-bit FAT extended partition",0xe1},
  { "DOS R/O or SpeedStor",0xe3},
  { "SpeedStor 16-bit FAT extended partition < 1024 cyl.",0xe4},
  { "SpeedStor",0xf1},
  { "DOS 3.3+ secondary",0xf2},
  { "SpeedStor large partition",0xf4},
  { "SpeedStor >1024 cyl. or LANstep",0xfe},
  { "Xenix Bad Block Table",0xff},
  {0, 0}
 };

/* Lookup ID code */

char *lookupid(int code)
 {
 int x;
 for(x=0;ids[x].name;++x) if(ids[x].code==code) return ids[x].name;
 return "unknown";
 }

/* Structure to hold partition table */

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
 } entries[64];
int nentries;

/* Current drive geometry. */
unsigned heads;
unsigned sects;
unsigned headsects;

/* Load partition table */

unsigned char sect[512];	/* Partition sector buffer */

void getpart(void)
 {
 int x;
 int ofst;
 unsigned long pofst=0;		/* Sector no. of current extended partition */
 unsigned pcyl, phead, psect;	/* cyl/head/sect of extended partition */
 unsigned char *buf;
 ofst=0;
 pcyl=0; phead=0; psect=0;
 next:
 /* Get partition table */
 if(getsects(sect,1,pcyl,phead,psect,128))
  {
  fprintf(stderr,"Couldn't read partition table\n");
  exit(1);
  }
 /* Get partition table data into structure */
 buf=sect+mbrPART;
 for(x=0;x!=4;++x,buf+=16)
  {
  entries[x+ofst].active=buf[partACTIVE];
  entries[x+ofst].beghead=buf[partBEGHEAD];
  entries[x+ofst].begsect=buf[partBEGSECT]&63;
  entries[x+ofst].begcyl=buf[partBEGCYL]+((buf[partBEGSECT]&0xC0)<<2);
  entries[x+ofst].id=buf[partID];
  entries[x+ofst].endhead=buf[partENDHEAD];
  if(entries[x+ofst].id) heads=entries[x+ofst].endhead+1;
  entries[x+ofst].endsect=buf[partENDSECT]&63;
  if(entries[x+ofst].id) sects=entries[x+ofst].endsect;
  headsects=heads*sects;
  entries[x+ofst].endcyl=buf[partENDCYL]+((buf[partENDSECT]&0xC0)<<2);
  entries[x+ofst].start=(unsigned long)buf[partSTART]+
                   ((unsigned long)buf[partSTART+1]<<8)+
                   ((unsigned long)buf[partSTART+2]<<16)+
                   ((unsigned long)buf[partSTART+3]<<24)+pofst;
  entries[x+ofst].size=(unsigned long)buf[partSIZE]+
                  ((unsigned long)buf[partSIZE+1]<<8)+
                  ((unsigned long)buf[partSIZE+2]<<16)+
                  ((unsigned long)buf[partSIZE+3]<<24);
  entries[x+ofst].str=lookupid(entries[x+ofst].id);
  }
 /* Search for extended partition */
 for(x=0;x!=4;++x)
  if(entries[x+ofst].id==5)
   {
   pcyl=entries[x+ofst].begcyl;
   phead=entries[x+ofst].beghead;
   psect=entries[x+ofst].begsect-1;
   pofst=entries[x+ofst].start;
   ofst+=4;
   goto next;
   }
 nentries=ofst+4;
 }

/* Read sectors.  Sectors can cross track boundaries, but 'heads' and 'sects'
 * must be properly set for this to work.
 */

int readsects(unsigned char *buf,
              int drive,
              unsigned long start,
              unsigned long size)
 {
 unsigned cyl=start/headsects;
 unsigned head=start%headsects;
 unsigned sect=head%sects;
 head/=sects;
 while(size)
  if(sects-sect<size)
   {
   if(getsects(buf,sects-sect,cyl,head,sect,drive)) return -1;
   buf+=((sects-sect)<<sectshft);
   size-=sects-sect;
   sect=0;
   if(++head==heads) head=0, ++cyl;
   }
  else
   {
   if(getsects(buf,size,cyl,head,sect,drive)) return -1;
   size=0;
   }
 return 0;
 }

/* Write sectors.  Sectors can cross track boundaries, but 'heads' and 'sects'
 * must be properly set for this to work.
 */

int writesects(unsigned char *buf,int drive,unsigned long start,unsigned long size)
 {
 unsigned cyl=start/headsects;
 unsigned head=start%headsects;
 unsigned sect=head%sects;
 head/=sects;
 while(size)
  if(sects-sect<size)
   {
   if(putsects(buf,sects-sect,cyl,head,sect,drive)) return -1;
   buf+=((sects-sect)<<sectshft);
   size-=sects-sect;
   sect=0;
   if(++head==heads) head=0, ++cyl;
   }
  else
   {
   if(putsects(buf,size,cyl,head,sect,drive)) return -1;
   size=0;
   }
 return 0;
 }

/* Show partition table */

void showpart(void)
 {
 int x;
 printf("\n");
 printf("No.    Size   ID Type                 Active\n");
 printf("--- --------- -- -------------------- ------\n");
 for(x=0;x!=nentries;++x) if(entries[x].id && entries[x].id!=5)
  printf(" %d  %6lu MB %2x %-20s    %c\n",x+1,
         entries[x].size/2048L,
         entries[x].id,entries[x].str,(entries[x].active?'*':' '));
 printf("\n");
 printf("Drive has %d heads and %d sectors per track\n\n",heads,sects);
 }

main(argc,argv)
char *argv[];
 {
 char buf[80];
 int x;
 FILE *f=0;
 int flg=0;
 int lout;
 char *name=0;

 if(izcmp() || !(bigbuf=malloc(bigbufsz+5)) || !(bigbuf1=malloc(bigbufsz+5)))
  {
  fprintf(stderr,"Not enough memory\n");
  return 1;
  }

 if(argc==1)
  {
  printf("Partition Archiver\n");
  printf(" phantom -c archive-file-name     Create archive from partition\n");
  printf(" phantom -x archive-file-name     Extract archive to partition\n\n");
  printf("\n- Archive file should be on a partition different from the one\n");
  printf("  being archived.\n\n");
  printf("- You should do a 'smartdrv /c' before running phantom if you\n");
  printf("  have smartdrv running\n\n");
  printf("- This program can not move or resize partitions - it can only\n");
  printf("  save and restore them\n");
  return 1;
  }

 for(x=1;x!=argc;++x)
  if(!strcmp(argv[x],"-x"))
   {
   if(flg) fprintf(stderr,"Can only have one of -x or -c\n"), exit(1);
   flg=1;
   }
  else if(!strcmp(argv[x],"-c"))
   {
   if(flg) fprintf(stderr,"Can only have one of -x or -c\n"), exit(1);
   flg=2;
   }
  else
   {
   if(name) fprintf(stderr,"Only one file name can be specified\n"), exit(1);
   name=argv[x];
   }

 if(!flg)
  {
  fprintf(stderr,"Must specify -c or -x\n");
  exit(1);
  }

 /* Get parition table */
 getpart();
 printf("\nParition table for drive 0\n");
 showpart();

 if(flg==1)
  { /* Extract from archive */
  int partno;
  unsigned long mb= -1;
  unsigned long cnt=0;
  unsigned long amnt;
  unsigned long a, b, c, d;
  unsigned long start;
  unsigned long size;

  /* Open archive */
  f=fopen(name,"rb");
  if(!f)
   {
   fprintf(stderr,"Couldn't open file %s\n",name);
   exit(1);
   }

  /* Get start sector and size of partition */
  a=fgetc(f); b=fgetc(f); c=fgetc(f); d=fgetc(f);
  start=a+(b<<8)+(c<<16)+(d<<24);
  a=fgetc(f); b=fgetc(f); c=fgetc(f); d=fgetc(f);
  size=a+(b<<8)+(c<<16)+(d<<24);
  printf("Archived partition starting sector=%lu size=%lu\n",
         start,size);
  printf("(cylinder=%lu head=%lu sector=%lu)\n\n",
         start/headsects,(start%headsects)/sects,(start%headsects)%sects);

  /* Search for partition */
  for(partno=0;partno!=nentries;++partno)
   if(entries[partno].id && entries[partno].id!=5 &&
      entries[partno].start==start && entries[partno].size==size) break;
  if(partno==nentries)
   {
   fprintf(stderr,"Could not find partition for this archive\n");
   fprintf(stderr,"Can only extract back to the original partition\n");
   exit(1);
   }

  printf("This corresponds to partition no. %d\n",partno+1);
  printf("Data currently in this partition will be lost.  Is this correct (y,n)? ");
  gets(buf);
  if(buf[0]!='y' && buf[0]!='Y') return 1;

  /* Extract archive */
  while((lout=fgetc(f))!=-1)
   {
   lout+=(fgetc(f)<<8);
   if(lout!=fread(bigbuf1,1,lout,f))
    {
    fprintf(stderr,"File read error?\nAborting\n");
    exit(1);
    }
   amnt=ucmp(bigbuf,bigbuf1,lout);
   if(writesects(bigbuf,128,entries[partno].start+cnt,amnt>>sectshft))
    {
    fprintf(stderr,"\nDisk write error\nAborting\n");
    return -1;
    }
   cnt+=(amnt>>sectshft);
   if(mb!=cnt/2048)
    {
    mb=cnt/2048;
    printf("\rRestored %lu MB...",mb); fflush(stdout);
    }
   }
  printf("\rRestored %lu MB.        \n",mb);

  printf("All done!  You should reboot the computer now.\n");
  }
 else if(flg==2)
  { /* Create archive */
  int partno;
  unsigned amnt;
  unsigned long cnt;
  unsigned long mb= -1;
  loop:
  printf("Enter number of partition to archive: ");
  gets(buf);
  partno=atoi(buf);
  if(partno<1 || partno>nentries || !entries[--partno].id)
   {
   printf("Invalid partition no.\n");
   goto loop;
   }
  printf("Archiving partition %d into file %s.  Is this correct (y,n)? ",partno+1,name);
  gets(buf);
  if(buf[0]!='y' && buf[0]!='Y') exit(1);

  /* Create archive */
  f=fopen(name,"wb");
  if(!f) fprintf(stderr,"Could not open file %s\n",name), exit(1);

  /* Save starting sector of partition */
  fputc(entries[partno].start,f);
  fputc(entries[partno].start>>8,f);
  fputc(entries[partno].start>>16,f);
  fputc(entries[partno].start>>24,f);

  /* Save size of partition */
  fputc(entries[partno].size,f);
  fputc(entries[partno].size>>8,f);
  fputc(entries[partno].size>>16,f);
  fputc(entries[partno].size>>24,f);

  /* Save 63 sectors (32K-512) at a time */
  for(cnt=0;cnt+bigbufsects<=entries[partno].size;cnt+=bigbufsects)
   {
   if(readsects(bigbuf,128,entries[partno].start+cnt,bigbufsects))
    {
    fprintf(stderr,"\nDisk read error\nAborting\n");
    return -1;
    }
   lout=cmp(bigbuf1,bigbuf,bigbufsz);
   fputc(lout,f);
   fputc(lout>>8,f);
   if(lout!=fwrite(bigbuf1,1,lout,f))
    {
    fprintf(stderr,"File write error - out of space?\nAborting\n");
    exit(1);
    }
   if(mb!=cnt/2048)
    {
    mb=cnt/2048;
    printf("\rArchived %lu MB...",mb);
    }
   fflush(stdout);
   }
  /* Save balance of data */
  if(cnt<entries[partno].size)
   {
   if(readsects(bigbuf,128,entries[partno].start+cnt,entries[partno].size-cnt))
    {
    fprintf(stderr,"\nDisk read error 2\nAborting\n");
    return -1;
    }
   lout=cmp(bigbuf1,bigbuf,(entries[partno].size-cnt)<<sectshft);
   fputc(lout,f);
   fputc(lout>>8,f);
   if(lout!=fwrite(bigbuf1,1,lout,f))
    {
    fprintf(stderr,"\nFile write error - out of space?\nAborting\n");
    return -1;
    }
   }
  printf("\rArchived %lu MB.      \n",entries[partno].size/2048);
  printf("All done!\n");
  }

 fclose(f);

 return 0;
 }
