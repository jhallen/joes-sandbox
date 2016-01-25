/*-----------------------------------------------------------------------*\
 |  defs.h  --  main definitions for z80 emulator                        |
 |                                                                       |
 |  Copyright 1986-1988 by Parag Patel.  All Rights Reserved.            |
 |  Copyright 1994-1995 by CodeGen, Inc.  All Rights Reserved.           |
\*-----------------------------------------------------------------------*/

#ifndef __DEFS_H_
#define __DEFS_H_

#include <stdio.h>

/* the current version of the z80 emulator */
#define VERSION "3.1"


/* system definitions */
#if defined THINK_C || defined applec || defined macintosh
#	ifndef macintosh
#		define macintosh
#	endif
#elif defined __MWERKS__
#   define BeBox
#elif defined MSDOS && defined GO32
#	define DJGPP
#	ifndef LITTLE_ENDIAN
#		define LITTLE_ENDIAN
#	endif
#else
#	define UNIX	/* cannot use "unix" since DJGPP defines it as well */
#endif


/* some headers define macros this way */
#ifdef BYTE_ORDER
#   if BYTE_ORDER == LITTLE_ENDIAN
#      undef BIG_ENDIAN
#   else
#      undef LITTLE_ENDIAN
#   endif
#endif


/* misc. handy defs */

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

typedef int boolean;


#define CNTL(c) ((c) & 037)	/* convert a char to its control equivalent */


/* handy typedefs for an 8-bit byte, 16-bit word, & 32-bit longword */

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long longword;


/* handy bit definitions - bit fields are not used as they are generally
   much slower than the equivalent logical masking operations */

#define BIT16  0x10000L
#define BIT15   0x8000
#define BIT14   0x4000
#define BIT13   0x2000
#define BIT12   0x1000
#define BIT11   0x0800
#define BIT10   0x0400
#define BIT9    0x0200
#define BIT8    0x0100
#define BIT7    0x0080
#define BIT6    0x0040
#define BIT5    0x0020
#define BIT4    0x0010
#define BIT3    0x0008
#define BIT2    0x0004
#define BIT1    0x0002
#define BIT0    0x0001

/* handy masks to get a particular number of bits out */

#define MASK1   0x01
#define MASK2   0x03
#define MASK3   0x07
#define MASK4   0x0F
#define MASK5   0x1F
#define MASK6   0x3F
#define MASK7   0x7F
#define MASK8   0xFF

#define MASKU4  0xF0
#define MASK16  0xFFFF


/* z80 flag register definitions */

#define SIGN      0x80
#define ZERO      0x40
#define HALF      0x10
#define PARITY    0x04
#define OVERFLOW  PARITY
#define NEGATIVE  0x02
#define CARRY     0x01


/* z80 interrupt types - used to set the intr struct var */

#define INTRMASK	0xF00
#define INT_FLAG	0x100
#define NM_FLAG		0x200
#define RESET_FLAG	0x400


/* max number of the BIOS drive tables */
#define MAXDISCS	16


typedef struct z80info
{
    boolean event;
//    byte regaf[2], regbc[2], regde[2], reghl[2];
    word regaf, regbc, regde, reghl;
    word regaf2, regbc2, regde2, reghl2;
    word regsp, regpc, regix, regiy;
    byte regi, regr;
    byte iff, iff2, imode;
    byte reset, nmi, intr, halt;

    /* these point to the addresses of the above registers */
    byte *reg[8];
    word *regpairaf[4];
    word *regpairsp[4];
    word *regpairxy[4];
    word *regixy[2];
    byte *regir[2];

    /* these are for the I/O, CP/M, and outside needs */
    boolean trace;		/* trace mode off/on */
    boolean step;		/* step-trace mode off/on */
    int sig;		/* caught a signal */
    int syscall;	/* CP/M syscall to be done */
    int biosfn;		/* BIOS function be done */

    /* these are for the CP/M BIOS */
    int	drive;
    word dma;
    word track;
    word sector;
    FILE *drives[MAXDISCS];
    long drivelen[MAXDISCS];

    /* 64k bytes - may be allocated separately if desired */
    byte mem[0x10000L];

#ifdef MEM_BREAK
    /* one for each byte of memory for breaks, memory-mapped I/O, etc */
    byte membrk[0x10000L];
    long numbrks;
#endif
} z80info;


/* All the following macros assume that a variable named "z80" is
   available to access the above info.  This is to allow multiple
   z80s to run without stepping on each other.
*/


/* These macros allow memory-mapped I/O if MEM_BREAK is defined.
   Because of this, these macros must be very carefully used, and
   there must not be ANY side-effects, such as increment/decerement
   in any of the macro args.  Customizations go into read_mem() and
   write_mem().
*/

#ifdef MEM_BREAK
#    define MEM(addr)	\
		(z80->membrk[(word)(addr)] ?	\
		read_mem(z80, addr) :	\
		z80->mem[(word)(addr)])
#    define SETMEM(addr, val)	\
		(z80->membrk[(word)(addr)] ?	\
		write_mem(z80, addr, val) :	\
		(z80->mem[(word)(addr)] = (byte)(val)))

	/* various flags for "membrk" - others may be added */
#	define M_BREAKPOINT	0x01		/* breakpoint */
#	define M_READ_PROTECT	0x02		/* read-protected memory */
#	define M_WRITE_PROTECT	0x04		/* write-protected memory */
#	define M_MEM_MAPPED_IO	0x08		/* memory-mapped I/O addr */

#else
#    define MEM(addr) z80->mem[(word)(addr)]
#    define SETMEM(addr, val) (z80->mem[(word)(addr)] = (byte)(val))
#endif


/* how to access the z80 registers & register pairs */

#ifdef LITTLE_ENDIAN
#  define A	((unsigned char *)&z80->regaf)[1]
#  define F	((unsigned char *)&z80->regaf)[0]
#  define B	((unsigned char *)&z80->regbc)[1]
#  define C	((unsigned char *)&z80->regbc)[0]
#  define D	((unsigned char *)&z80->regde)[1]
#  define E	((unsigned char *)&z80->regde)[0]
#  define H	((unsigned char *)&z80->reghl)[1]
#  define L	((unsigned char *)&z80->reghl)[0]
#else
#  define A	((unsigned char *)&z80->regaf)[0]
#  define F	((unsigned char *)&z80->regaf)[1]
#  define B	((unsigned char *)&z80->regbc)[0]
#  define C	((unsigned char *)&z80->regbc)[1]
#  define D	((unsigned char *)&z80->regde)[0]
#  define E	((unsigned char *)&z80->regde)[1]
#  define H	((unsigned char *)&z80->reghl)[0]
#  define L	((unsigned char *)&z80->reghl)[1]
#endif

#define I	z80->regi
#define R	z80->regr
#define AF	z80->regaf
#define BC	z80->regbc
#define DE	z80->regde
#define HL	z80->reghl
#define AF2	z80->regaf2
#define BC2	z80->regbc2
#define DE2	z80->regde2
#define HL2	z80->reghl2
#define SP	z80->regsp
#define PC	z80->regpc
#define IX	z80->regix
#define IY	z80->regiy
#define IFF	z80->iff
#define IFF2	z80->iff2
#define IMODE	z80->imode
#define RESET	z80->reset
#define NMI	z80->nmi
#define INTR	z80->intr
#define HALT	z80->halt

#define EVENT	z80->event


/* function externs: */

/* z80.c */
extern z80info *new_z80info(void);
extern z80info *init_z80info(z80info *z80);
extern z80info *destroy_z80info(z80info *z80);
extern void delete_z80info(z80info *z80);

extern boolean z80_emulator(z80info *z80, int count);

extern int nobdos;

/* main.c */
extern void resetterm(void);
extern void setterm(void);
extern boolean input(z80info *z80, byte haddr, byte laddr, byte *val);
extern void output(z80info *z80, byte haddr, byte laddr, byte data);
extern void haltcpu(z80info *z80);
extern word read_mem(z80info *z80, word addr);
extern word write_mem(z80info *z80, word addr, byte val);
extern void undefinstr(z80info *z80, byte instr);
extern boolean loadfile(z80info *z80, const char *fname);

/* bios.c */
extern void bios(z80info *z80, int fn);
extern void sysreset(z80info *z80);
extern void warmboot(z80info *z80);
extern void finish(z80info *z80);

/* disassem.c */
extern int disassemlen(z80info *z80);
extern int disassem(z80info *z80, word start, FILE *fp);

/* bdos */
#define BDOS_HOOK 0xDC06
void check_BDOS_hook(z80info *z80);
extern int silent_exit;
extern char *stuff_cmd;
extern int trace_bdos;
extern int strace;
char *bdos_decode(int n);
int bdos_fcb(int n);
void bdos_fcb_dump(z80info *z80);

#endif /* __DEFS_H_ */
