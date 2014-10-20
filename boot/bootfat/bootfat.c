/* Installer for tiny FAT filesystem boot loader
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

int version=1;	/* Version no. of this program */

/* FAT12 boot program */
unsigned char fat12[]=
{
235, 60,144, 87,105,110, 98,108,111,119,115,  0,  2,  1,  1,  0,
  2,224,  0, 64, 11,240,  9,  0, 18,  0,  2,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0, 41,  0,  0,  0,  0, 32, 32, 32, 32, 32,
 32, 32, 32, 32, 32, 32, 70,111,114, 32, 82,101,110,116,187,  0,
144,142,211,188,244, 63, 51,210,142,218,142,194,139,234,252,197,
 54,120,  0,191,242,125,185, 11,  0,172, 38, 56, 21,116,  3, 38,
138,  5,170,226,244,  6, 31,199,  6,120,  0,242,125,137, 22,122,
  0,184, 18,  0,186,224,  0,131,195, 32,121,  4, 64,232,168,  0,
139,251,190,228,125,185, 11,  0,243,166,116, 15, 74,117,232,190,
220,125,232, 55,  1, 50,228,205, 22,205, 25,190,228,125,232, 43,
  1,139,127, 26,187,  0,246, 51,201, 87,128,193,  1,139,199,209,
231,  3,248,209,239,232, 71,  0,169,  1,  0,116,  8,209,239,209,
239,209,239,209,239,129,231,255, 15,129,255,240, 15,114,  2, 51,
255,128,249,124,115,  5, 64, 59,199,116,207, 88, 87,186,  1,  0,
247,226,  5, 31,  0,131,210,  0,232, 88,  0, 95, 11,255,117,183,
190,  0,246, 22,  7,185,  0,  5,243,165,234,  0,  0, 32,144,  6,
 80, 83, 81, 82, 87,232, 17,  0,138, 17, 95, 71,232, 10,  0,138,
 49, 82, 95, 90, 89, 91, 88,  7,195, 87, 88,177,  9,211,232,  5,
  1,  0,144,144,129,231,255,  1, 80, 81, 82, 51,210,142,194,187,
  0,126, 59,197,116,  9,139,232,177,  1, 83,232,  5,  0, 91, 90,
 89, 88,195,  5,  0,  0,129,210,  0, 16, 50,237,139,241, 82, 80,
247, 54,240,125,139,194, 51,210,247, 54, 24,124,184, 18,  0, 43,
194, 59,198,114,  2,139,198, 80,177,  9,211,224,  3,195,115, 13,
 88,139,195,247,216,211,232, 80,176, 46,232, 85,  0, 89, 88, 90,
 86, 80, 82,181,  2, 81,247, 54,240,125,139,200,139,194,246, 54,
 24,124,138,240,254,196,178,  0,134,233,208,201,208,201,  2,204,
 88, 80,205, 19,115,  3, 88,235,248, 89, 50,237,184,  0,  2,247,
225,  3,216,117,  7,140,192,  5,  0, 16,142,192, 90, 88, 94,  3,
193,131,210,  0, 43,241,117,134,195,232,  6,  0,172, 10,192,117,
248,195, 83,180, 14,187,  7,  0,205, 16, 91,195, 78,111, 32, 79,
 83, 13, 10,  0, 86, 77, 76, 73, 78, 85, 90, 32, 32, 32, 32,  0,
 36,  0,  0,  0,  0,  0, 36,  0,  0,  0,  1,  0,  0,  1, 85,170
};

/* FAT 16 boot program */
unsigned char fat16[]=
{
235, 60,144, 87,105,110, 98,108,111,119,115,  0,  2,  1,  1,  0,
  2,224,  0, 64, 11,240,  9,  0, 18,  0,  2,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0, 41,  0,  0,  0,  0, 32, 32, 32, 32, 32,
 32, 32, 32, 32, 32, 32, 70,111,114, 32, 82,101,110,116,187,  0,
144,142,211,188,244, 63, 51,210,142,218,142,194,139,234,252,197,
 54,120,  0,191,242,125,185, 11,  0,172, 38, 56, 21,116,  3, 38,
138,  5,170,226,244,  6, 31,199,  6,120,  0,242,125,137, 22,122,
  0,184, 18,  0,186,224,  0,131,195, 32,121,  4, 64,232,168,  0,
139,251,190,228,125,185, 11,  0,243,166,116, 15, 74,117,232,190,
220,125,232, 55,  1, 50,228,205, 22,205, 25,190,228,125,232, 43,
  1,139,127, 26,187,  0,246, 51,201, 87,128,193,  1,139,199,232,
 77,  0,131,255,240,144,144,144,144,144,144,144,144,144,144,144,
144,144,144,144,144,144,144,144,144,144,144,144,144,114,  2, 51,
255,128,249,124,115,  5, 64, 59,199,116,207, 88, 87,186,  1,  0,
247,226,  5, 31,  0,131,210,  0,232, 88,  0, 95, 11,255,117,183,
190,  0,246, 22,  7,185,  0,  5,243,165,234,  0,  0, 32,144,  6,
 80, 83, 81, 82, 87,232, 17,  0,138, 17, 95,232, 11,  0, 71,138,
 49, 82, 95, 90, 89, 91, 88,  7,195, 87, 88,177,  8,211,232,  5,
  1,  0,209,231,129,231,255,  1, 80, 81, 82, 51,210,142,194,187,
  0,126, 59,197,116,  9,139,232,177,  1, 83,232,  5,  0, 91, 90,
 89, 88,195,  5,  0,  0,129,210,  0, 16, 50,237,139,241, 82, 80,
247, 54,240,125,139,194, 51,210,247, 54, 24,124,184, 18,  0, 43,
194, 59,198,114,  2,139,198, 80,177,  9,211,224,  3,195,115, 13,
 88,139,195,247,216,211,232, 80,176, 46,232, 85,  0, 89, 88, 90,
 86, 80, 82,181,  2, 81,247, 54,240,125,139,200,139,194,246, 54,
 24,124,138,240,254,196,178,  0,134,233,208,201,208,201,  2,204,
 88, 80,205, 19,115,  3, 88,235,248, 89, 50,237,184,  0,  2,247,
225,  3,216,117,  7,140,192,  5,  0, 16,142,192, 90, 88, 94,  3,
193,131,210,  0, 43,241,117,134,195,232,  6,  0,172, 10,192,117,
248,195, 83,180, 14,187,  7,  0,205, 16, 91,195, 78,111, 32, 79,
 83, 13, 10,  0, 86, 77, 76, 73, 78, 85, 90, 32, 32, 32, 32,  0,
 36,  0,  0,  0,  0,  0, 36,  0,  0,  0,  1,  0,  0,  1, 85,170
};

/* FAT superblock */

struct fatsuper
 {
 unsigned char jump[3];		/* Jump to start of code */

 /* Header which must be preserved */
 unsigned char installer[8];	/* Name of installing OS */
 unsigned char bps[2];		/* Bytes per sector */
 unsigned char spc;		/* Sectors per cluster */
 unsigned char rsv[2];		/* Sectors in boot record */
 unsigned char fat;		/* No. fats */
 unsigned char dir[2];		/* No. root directory entries */
 unsigned char nls[2];		/* No. sectors if < 65536 */
 unsigned char typ;		/* Media type */
 unsigned char nfs[2];		/* Number of fat sectors */
 unsigned char spt[2];		/* Number of sectors per track */
 unsigned char hds[2];		/* Number of heads */
 unsigned char nhs[4];		/* Number of hidden sectors */
 unsigned char nlsb[4];		/* Number of sectors if >=65536 */
 unsigned char drive;		/* BIOS drive no. */
 unsigned char waste;		/* Reserved byte */
 unsigned char vmagic;		/* Volume name magic no. */
 unsigned char id[4];		/* Volume ID */
 unsigned char label[11];	/* Volume label */
 unsigned char wasted[8];	/* 8 more wasted bytes */

 unsigned char pgm[422];	/* Boot loader program */

 unsigned char os[12];		/* Operating system name */
 unsigned char spcy[2];		/* Sectors per cylinder */
 unsigned char parm[11];	/* Replacement diskette parm table */
 unsigned char ver;		/* Boot loader version no. */
 unsigned char magic[2];	/* Valid boot sector magic no. */
 };

/* Set boot loader parameters */

setparms(boot,super,name,drive)
struct fatsuper *boot, *super;
char *name;
 {
 char nambuf[12];
 int spt;	/* Sectors per track */
 int spcy;	/* Sectors per cylinder */
 int firstc;	/* Sector no. corresponding to cluster 0 (first cluster is 2) */
 int dirs;	/* Sector no. of root directory (-1) */
 int x;
 int secs;

 if(super->bps[0]+super->bps[1]*256!=512)
  {
  printf("Must have 512 byte sectors.  Non MS-DOS partition?\n");
  return -1;
  }

 if(super->rsv[0]+256*super->rsv[1]!=1)
  {
  printf("Must have 1 reserved sector.  Non MS-DOS partition?\n");
  return -1;
  }

 if(((super->dir[0]+256*super->dir[1])*32)%512!=0)
  {
  printf("Incorrect no. of directory entries.  Non MS-DOS partition?\n");
  return -1;
  }
 
 /* Calculate derived constants */
 spt=super->spt[0]+256*super->spt[1];
 spcy=spt*(super->hds[0]+256*super->hds[1]);
 firstc=(super->rsv[0]+256*super->rsv[1])+
        super->fat*(super->nfs[0]+256*super->nfs[1])+
        (super->dir[0]+256*super->dir[1])*32/512-2*super->spc;
 dirs=super->rsv[0]+256*super->rsv[1]+
      super->fat*(super->nfs[0]+256*super->nfs[1])-1;

 if(super->nlsb[0] || super->nlsb[1] || super->nlsb[2] || super->nlsb[3])
  secs=super->nlsb[0]+(super->nlsb[1]<<8)+(super->nlsb[2]<<16)+(super->nlsb[3]<<24);
 else
  secs=super->nls[0]+(super->nls[1]<<8);

 secs-=firstc;
 secs/=super->spc;

 printf("Total clusters=%d\n",secs-2);

 if(secs>65520)
  {
  printf("Looks like a 32-bit fat?  Sorry, we only handle 12 and 16-bit fats\n");
  return -1;
  }

 if(secs<=4080 && ((secs*3)/2+511)/512==super->nfs[0]+256*super->nfs[1])
  {
  printf("Looks like a 12-bit fat\n");
  memcpy((char *)boot,fat12,512);
  }
 else if(secs>=4080 && (secs*2+511)/512==super->nfs[0]+256*super->nfs[1])
  {
  printf("Looks like a 16-bit fat\n");
  memcpy((char *)boot,fat16,512);
  }
 else
  {
  printf("Can't figure fat.  Non-msdos partition?\n");
  return -1;
  }

 /* Copy header part */
 memcpy(((char *)boot)+3,((char *)super)+3,59);

 /* Set name */
 if(name)
  {
  int x, y;

  for(x=0,y=0;x!=8;++x)
   if(name[y] && name[y]!='.')
    {
    if(name[y]>='a' && name[y]<='z') nambuf[x]=name[y]+'A'-'a';
    else nambuf[x]=name[y];
    ++y;
    }
   else
    nambuf[x]=' ';

  /* Name too long */
  if(name[y] && name[y]!='.') return -1;
  /* Skip . */
  if(name[y]=='.') ++y;

  for(x=8;x!=11;++x)
   if(name[y])
    {
    if(name[y]=='.') return -1;
    if(name[y]>='a' && name[y]<='z') nambuf[x]=name[y]+'A'-'a';
    else nambuf[x]=name[y];
    ++y;
    }
   else nambuf[x]=' ';

  nambuf[x]=0;

  strcpy(boot->os,nambuf);
  }

 /* Set drive */
 if(drive!=-1) boot->drive=drive;

 printf("Bytes per sector=%d\n",boot->bps[0]+boot->bps[1]*256);
 printf("Sectors per cluster=%d\n",boot->spc);
 printf("Sectors in boot record=%d\n",boot->rsv[0]+boot->rsv[1]*256);
 printf("No. fats=%d\n",boot->fat);
 printf("Root directory entries=%d\n",boot->dir[0]+boot->dir[1]*256);
 printf("No. sectors (small)=%d\n",boot->nls[0]+256*boot->nls[1]);
 printf("Media type=%x\n",boot->typ);
 printf("No. fat sectors per fat=%d\n",boot->nfs[0]+boot->nfs[1]*256);
 printf("Sectors per track=%d\n",boot->spt[0]+boot->spt[1]*256);
 printf("No. heads=%d\n",boot->hds[0]+256*boot->hds[1]);
 printf("Starting sector of parition=%d\n",
        boot->nhs[0]+(boot->nhs[1]<<8)+(boot->nhs[2]<<16)+(boot->nhs[3]<<24));
 printf("No. sectors (big)=%d\n",
        boot->nlsb[0]+(boot->nlsb[1]<<8)+(boot->nlsb[2]<<16)+(boot->nlsb[3]<<24));
 printf("BIOS boot drive=%d\n",boot->drive);
 printf("Sector no. of cluster 0=%d\n",firstc);
 printf("Sector no. of root directory=%d\n",dirs+1);
 printf("Booting %s\n",boot->os);

 boot->spcy[0]=spcy; boot->spcy[1]=(spcy>>8);
 /* Patch immediate constants */
 /* Immediate constants are in place of direct addressing
  * to save space */
 ((char *)boot)[0x197]=boot->drive;
 ((char *)boot)[0x15d]=boot->spt[0];
 ((char *)boot)[0x15e]=boot->spt[1];
 ((char *)boot)[0x144]=boot->nhs[0];
 ((char *)boot)[0x145]=boot->nhs[1];
 ((char *)boot)[0x148]=boot->nhs[2];
 ((char *)boot)[0x149]=boot->nhs[3];
 ((char *)boot)[0x120]=boot->rsv[0];
 ((char *)boot)[0x121]=boot->rsv[1];
 ((char *)boot)[0xe3]=firstc;
 ((char *)boot)[0xe4]=(firstc>>8);
 ((char *)boot)[0xde]=boot->spc;
 ((char *)boot)[0xac]=boot->spc;
 ((char *)boot)[0x75]=boot->dir[0];
 ((char *)boot)[0x76]=boot->dir[1];
 ((char *)boot)[0x72]=dirs;
 ((char *)boot)[0x73]=(dirs>>8);

 return 0;
 }

main(argc,argv)
char *argv[];
 {
 char *name=0;			/* Device name */
 int setdrive=-1;		/* Set drive no. */
 char *bootname=0;		/* Set boot name */
 int x;
 int err=0;
 int fd;			/* File descriptor for drive or partition */
 unsigned char super[512];	/* Fat superblock */
 unsigned char boot[512];	/* New superblock */

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
 if(bootname && strlen(bootname)>8) err=1;
 if(setdrive<-1 || setdrive>255) err=1;
 if(err)
  {
  fprintf(stderr,"bootfat [options] path\n",name);
  fprintf(stderr,"FAT boot record installer\n");
  fprintf(stderr,"Installer v.%d, Boot record v.%d\n",version,
          ((struct fatsuper *)fat16)->ver);
  fprintf(stderr,"  -d drive   BIOS boot drive no.  0, 1, 2... for floppies (default is 0);\n");
  fprintf(stderr,"             128, 129... for hard drives (default is 128).\n");
  fprintf(stderr,"  -n name    Name of file to boot if not vmlinuz (in 8.3 format)\n");
  return -1;
  }
 /* Open drive or partition */
 fd=open(name,2);
 if(fd==-1)
  {
  fprintf(stderr,"Couldn't open %s\n",name);
  return -1;
  }
 /* Get fat superblock */
 if(512!=read(fd,super,512))
  {
  fprintf(stderr,"Read error on %s\n",name);
  return -1;
  }
 /* Set fat boot loader parameters */
 if(setparms(boot,super,bootname,setdrive)) return -1;
 /* Write superblock */
 if(lseek(fd,0,SEEK_SET)==-1)
  {
  fprintf(stderr,"Seek error on %s\n",name);
  return -1;
  }
 if(512!=write(fd,boot,512))
  {
  fprintf(stderr,"Write error on %s\n",name);
  return -1;
  }
 if(close(fd)==-1)
  {
  fprintf(stderr,"Close error on %s\n",name);
  return -1;
  }
 printf("FAT boot loader successfully installed.\n");
 return 0;
 }
