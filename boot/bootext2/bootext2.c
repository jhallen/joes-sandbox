/* Installer for tiny ext2fs boot loader
   Copyright (C) 1998 Joseph H. Allen

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 1, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
more details.

You should have received a copy of the GNU General Public License along with
this program; see the file COPYING.  If not, write to the Free Software
Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. */

#include <stdio.h>
#include <unistd.h>
#include <linux/hdreg.h>
#include <linux/fd.h>
#include <linux/ext2_fs.h>

int version=1;	/* Version no. of this program */

unsigned char boot[]=		/* Boot block */
{
234,113,124,  0,  0,  2, 55,241,  0,  0,  0,  0, 18,  0, 36,  0,
  2,  2,  1,  0,  0,  0,  0,  1, 32,  0,  8,  0,  7,  0,  0,  0,
  0,  0, 36,  0,  0,  0,  1,  0,  0,118,109,108,105,110,117,122,
  0,  0,  0,  0,  0,  0, 78,111, 32, 79, 83, 13, 10,  0, 69,114,
114,111,114, 13, 10,  0,255,255,255,255,255,255,255,255,255,255,
255,255,190, 54,124,232,  9,  0, 50,228,205, 22,205, 25,232,  6,
  0,172, 10,192,117,248,195, 83,180, 14,187,  7,  0,205, 16, 91,
195,186,  0,144,142,210,188,244, 63, 51,210,142,218,142,194,139,
234,252,161, 16,124,139, 14, 18,124,139, 22, 20,124,187,  0,124,
205, 19,115,  5,190, 62,124,235,188,190, 49,127,232,194,255,197,
 54,120,  0,191, 30,124,185, 11,  0,172, 38,128, 61,  0,116,  3,
 38,138,  5,170,226,243,  6, 31,199,  6,120,  0, 30,124,199,  6,
122,  0,  0,  0,184,  2,  0,186,  0,  0,232, 75,  0, 83,142,194,
187,  0,246,190, 41,124,139, 14, 28,124, 59, 79,  6,117, 41,139,
251,131,199,  8,243,166,117, 32,190, 41,124,232,115,255,139,  7,
139, 87,  2,232, 34,  0,191,  0,  0,190,  0,246, 22,  7,185,  0,
  5,243,165,234,  0,  0, 32,144,  3, 95,  4,232, 22,  2,139,236,
 59, 94,  0,117,190,233, 58,255, 45,  1,  0,131,218,  0,247, 54,
 22,124, 82, 51,210,247, 54, 24,124, 82, 51,210,  5,  2,  0,131,
210,  0,232, 13,  1, 88,177,  5,211,224,  3,216, 88, 51,210,247,
 54, 26,124, 82, 51,210,  3, 71,  8, 19, 87, 10,232,243,  0, 88,
177,  7,211,224,  3,216,139,243,187,  0,246,139, 68, 28, 64,209,
232,139,248,131,198, 40,185, 12,  0,232,160,  0,185,  1,  0,232,
176,  0,131,255,  0,116, 17,139,  4,139, 84,  2, 83,  6,232,213,
  0,139,243,  7, 91,232,151,  0, 51,192,139,208,232,  6,  0,198,
  6,  0,126,255,195,128, 62,  0,126,255,116, 53,131,  6,  5,126,
  1,131, 22,  7,126,  0,254,  6,  0,126,128, 62,  0,126, 63,116,
 12, 59,  6,  5,126,117,  6, 59, 22,  7,126,116, 39, 82, 80, 81,
139, 22,  3,126,161,  1,126,138, 14,  0,126,232,172,  0, 89, 88,
 90,163,  1,126,137, 22,  3,126,163,  5,126,137, 22,  7,126,198,
  6,  0,126,  0, 79,195,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 85,170,
255,  0,  0,  0,  0,  0,  0,  0,  0,185,  0,  1,131,255,  0,116,
 13,139,  4,139, 84,  2,232,124,255,131,198,  4,226,238,195,185,
  0,  1,131,255,  0,116, 26,139,  4,139, 84,  2, 86, 81, 83,  6,
232, 25,  0,139,243,  7, 91,232,207,255, 89, 94,131,198,  4,226,
225,195, 86,187,  0,132,190, 70,124,235, 18,144, 86,187,  0,136,
190, 74,124,235,  8,144, 86,187,  0,140,190, 78,124, 83, 81, 51,
201,142,193, 59,  4,117,  5, 59, 84,  2,116, 10,137,  4,137, 84,
  2,177,  1,232,  4,  0, 89, 91, 94,195, 87, 80, 82, 11,192,117,
 23, 11,210,117, 19,185,  0,  2,139,251, 51,192,243,171,129,195,
  0,  4,232,143,  0,233,136,  0,209,224,209,210,  3,  6,  8,124,
 19, 22, 10,124,208,225, 50,237,139,249, 82, 80,247, 54, 14,124,
139,194, 51,210,247, 54, 12,124,161, 12,124, 43,194, 59,199,114,
  2,139,199, 80,177,  9,211,224,  3,195,115, 13, 88,139,195,247,
216,211,232, 80,176, 46,232,142,253, 89, 88, 90, 87, 80, 82,181,
  2, 81,247, 54, 14,124,139,200,139,194,246, 54, 12,124,138,240,
254,196,138, 22, 20,124,134,233,208,201,208,201,  2,204, 88, 80,
205, 19,115,  3, 88,235,248, 89, 50,237,184,  0,  2,247,225,  3,
216,232, 16,  0, 90, 88, 95,  3,193,131,210,  0, 43,249,117,138,
 90, 88, 95,195,117, 10,140,195,129,195,  0, 16,142,195, 51,219,
195,101,120,116, 50,102,115, 32, 98,111,111,116, 32,108,111, 97,
100,101,114, 32,118, 46, 50, 13, 10,  0, 80, 82,139,236,138,198,
232, 40,  0,138,194,232, 35,  0,138, 70,  3,232, 29,  0,138, 70,
  2,232, 23,  0,138,197,232, 18,  0,138,193,232, 13,  0,176, 13,
232,244,252,176, 10,232,239,252, 90, 88,195, 80,208,232,208,232,
208,232,208,232,232,  3,  0, 88, 36, 15, 60, 10,114,  2,  4,  7,
  4, 48,233,210,252,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

/* boot loader header */

struct bootheader
 {
 unsigned char farjump[5];	/* Far jump to start of code */
 unsigned char version;		/* Version number */
 unsigned short magic;		/* Magic number */
 unsigned long start;		/* Starting sector of partition */
 unsigned short spt;		/* Sectors per track */
 unsigned short spcy;		/* Sectors per cylinder */

 unsigned char rsv;		/* AL No. sectors in boot record (2) */
 unsigned char two;		/* AH Set to 2 */
 unsigned char sectno;		/* CL Sector no. of boot record */
 unsigned char cylno;		/* CH Track no. of boot record */
 unsigned char drive;		/* DL Drive to boot from */
 unsigned char headno;		/* DH Head no. of boot record */

 unsigned short ipg;		/* Inodes per group */
 unsigned short gdpb;		/* Group descriptors per block */
 unsigned short ipb;		/* Inodes per block */
 unsigned short len;		/* Length of operating system name */
 unsigned char diskette_parms[11];	/* Diskette parm table */
 unsigned char os[13];		/* Name of operating system to boot */
 };

/* Set boot loader parameters */

setparms(hdr,sectors,heads,cylinders,start,drive,super)
struct bootheader *hdr;
struct ext2_super_block *super;
 {
 int startcyl=start/(sectors*heads);
 int starthead=(start-(startcyl*sectors*heads))/sectors;
 int startsect=(start-(startcyl*sectors*heads))%sectors;
 hdr->start=start;
 hdr->spt=sectors;
 hdr->spcy=sectors*heads;
 hdr->drive=drive;

 hdr->sectno=startsect+1+((startcyl>>2)&0xC0);
 hdr->headno=starthead;
 hdr->cylno=startcyl;
 printf("Boot record located at cylno=%d headno=%d sectno=%d\n",
        startcyl,starthead,startsect+1);
 if(startcyl>=1024) 
  {
  fprintf(stderr,"Can not install past cylinder 1024\n");
  return -1;
  }

 hdr->ipg=super->s_inodes_per_group;
 printf("ext2 inodes_per_group: %d\n",hdr->ipg);
 printf("ext2 block_size: %d\n",
        1<<(super->s_log_block_size+10));
 if(super->s_log_block_size!=0)
  {
  fprintf(stderr,"Sorry, block size must be 1024.\n");
  return -1;
  }
 return 0;
 }

main(argc,argv)
char *argv[];
 {
 struct hd_geometry geometry;	/* Drive geometry */
 struct floppy_struct floppy;	/* Floppy geometry */
 char *name=0;			/* Device name */
 int setdrive=-1;		/* Set drive no. */
 char *bootname=0;		/* Set boot name */
 int x;
 int err=0;
 int fd;			/* File descriptor for drive or partition */
 /* Drive geometry */
 int sectors;
 int heads;
 int cylinders;
 int start;
 int drive;	/* 0, 1, 2... for floppies; 128, 129, 130... for hard */
 unsigned char super[1024];	/* Ext2fs superblock */

 /* Parse options */
 for(x=1;argv[x];)
  if(!strcmp(argv[x],"-d"))
   if(!argv[++x]) err=1;
   else sscanf(argv[x++],"%d",&setdrive);
  else if(!strcmp(argv[x],"-n"))
   if(!argv[++x]) err=1;
   else
    if(bootname) err=1;
    else bootname=argv[x++];
  else
   if(name) err=1;
   else name=argv[x++];

 /* Check options */
 if(!name) err=1;
 if(bootname && strlen(bootname)>12) err=1;
 if(setdrive<-1 || setdrive>255) err=1;
 if(err)
  {
  fprintf(stderr,"bootext2 [options] path\n",name);
  fprintf(stderr,"Ext2fs boot record installer\n");
  fprintf(stderr,"Installer v.%d, Boot record v.%d\n",version,
          ((struct bootheader *)boot)->version);
  fprintf(stderr,"  -d drive   BIOS boot drive no.  0, 1, 2... for floppies (default is 0);\n");
  fprintf(stderr,"             128, 129... for hard drives (default is 128).\n");
  fprintf(stderr,"  -n name    Name of file to boot if not vmlinuz (12 chars max)\n");
  return -1;
  }

 /* Open drive or partition */
 fd=open(name,2);
 if(fd==-1)
  {
  fprintf(stderr,"Couldn't open %s\n",name);
  return -1;
  }

 /* Get geometry */
 if(ioctl(fd,HDIO_GETGEO,&geometry))
  {
  /* Maybe it's a floppy? */
  if(ioctl(fd,FDGETPRM,&floppy))
   {
   fprintf(stderr,"Couldn't get disk geometry\n");
   return -1;
   }
  else
   sectors=floppy.sect,
   heads=floppy.head,
   cylinders=floppy.track,
   start=0,
   drive=0;
  }
 else sectors=geometry.sectors,
      heads=geometry.heads,
      cylinders=geometry.cylinders,
      start=geometry.start,
      drive=128;
 printf("Geometry: sectors=%d, heads=%d, cylinders=%d, start=%d\n",sectors,
        heads,cylinders,start);

 /* Get ext2 superblock */
 if(lseek(fd,1024,SEEK_SET)==-1)
  {
  fprintf(stderr,"Seek error on %s\n",name);
  return -1;
  }
 if(1024!=read(fd,super,1024))
  {
  fprintf(stderr,"Read error on %s\n",name);
  return -1;
  }
 if(((struct ext2_super_block *)super)->s_magic!=EXT2_SUPER_MAGIC)
  {
  fprintf(stderr,"Not an ext2fs on %s\n",name);
  return -1;
  }

 /* Override with user given options */
 if(setdrive!=-1) drive=setdrive;
 printf("BIOS boot drive no. %d\n",drive);
 if(bootname)
  strcpy(((struct bootheader *)boot)->os,bootname),
  ((struct bootheader *)boot)->len=strlen(bootname);

 /* Set ext2 boot loader parameters */
 if(setparms(boot,sectors,heads,cylinders,start,drive,super)) return -1;

 /* Write superblock */
 if(lseek(fd,0,SEEK_SET)==-1)
  {
  fprintf(stderr,"Seek error on %s\n",name);
  return -1;
  }
 if(1024!=write(fd,boot,1024))
  {
  fprintf(stderr,"Write error on %s\n",name);
  return -1;
  }
 if(close(fd)==-1)
  {
  fprintf(stderr,"Close error on %s\n",name);
  return -1;
  }
 printf("Ext2_fs boot loader successfully installed.\n");
 return 0;
 }
