/*-----------------------------------------------------------------------*\
 |  main.c  --  main driver program for the z80 emulator  --  all I/O    |
 |  to the Unix world is done from this file  --  "z80.c" calls various  |
 |  functions within this file                                           |
 |                                                                       |
 |  Copyright 1986-1988 by Parag Patel.  All Rights Reserved.            |
 |  Copyright 1994-1995 by CodeGen, Inc.  All Rights Reserved.           |
\*-----------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "defs.h"
#include "vt.h"

#if defined macintosh
#	include <Types.h>
#	include <Events.h>
#	ifdef THINK_C
#		include <console.h>
#	endif
#elif defined DJGPP
#	include <pc.h>
#else	/* UNIX */
#	include <unistd.h>
#	include <sys/ioctl.h>
#	if defined POSIX_TTY
#		include <sys/termios.h>
#	elif defined BeBox
#		include <termios.h>
#	else
#		include <termio.h>
#	endif
#endif

#define INTR_CHAR	31	/* control-underscore */

extern int errno;


/* globally visible vars */
static FILE *logfile = NULL;


#if defined UNIX || defined BeBox
#ifdef POSIX_TTY
#	define termio termios
#endif
static struct termio rawterm, oldterm;	/* for raw terminal I/O */
#endif


static void dumptrace(z80info *z80);


char *jgets(char *s, int len, FILE *f)
{
	char *rtn = fgets(s, len, f);
	if (rtn)
	{
		int x;
		for (x = 0; s[x] && s[x] != '\r' && s[x] != '\n'; ++x);
		s[x] = 0;
	}
	return rtn;
}

/*-----------------------------------------------------------------------*\
 |  resetterm  --  reset terminal characteristics to original settings
\*-----------------------------------------------------------------------*/

void
resetterm(void)
{
	tcsetattr(fileno(stdin), TCSADRAIN, &oldterm);
}



/*-----------------------------------------------------------------------*\
 |  setterm  --  set terminal characteristics to raw mode
\*-----------------------------------------------------------------------*/

void
setterm(void)
{
	tcsetattr(fileno(stdin), TCSADRAIN, &rawterm);
}



/*-----------------------------------------------------------------------*\
 |  initterm  --  initialize terminal stuff  --  called once on startup
 |  and then after returning from a sub-shell
\*-----------------------------------------------------------------------*/

static void
initterm(void)
{
	if (tcgetattr(fileno(stdin), &oldterm))
	{
		fprintf(stderr, "Sorry, Must be using a terminal.\n");
		exit(1);
	}
	rawterm = oldterm;
	rawterm.c_iflag &= ~(ICRNL | IXON | IXOFF | INLCR | ICRNL);
	rawterm.c_lflag &= ~(ICANON | ECHO);
	rawterm.c_cc[VSUSP] = -1;
	rawterm.c_cc[VQUIT] = -1;
	rawterm.c_cc[VERASE] = -1;
	rawterm.c_cc[VKILL] = -1;

	// tcsetattr(fileno(stdin), TCSADRAIN, &rawterm);

#if 0
	/* rawterm.c_lflag &= ~(ISIG | ICANON | ECHO); */
	rawterm.c_lflag &= ~(ICANON | ECHO);
#ifdef IENQAK
	rawterm.c_iflag &= ~(IENQAK | IXON | IXOFF | INLCR | ICRNL);
#else
	rawterm.c_iflag &= ~(IXON | IXOFF | INLCR | ICRNL);
#endif
	rawterm.c_oflag &= ~OPOST;
	rawterm.c_cc[VINTR] = INTR_CHAR;
	rawterm.c_cc[VSUSP] = -1;
	rawterm.c_cc[VQUIT] = -1;
	rawterm.c_cc[VERASE] = -1;
	rawterm.c_cc[VKILL] = -1;
	rawterm.c_cc[VMIN] = 1;		/* MIN number of chars */
	rawterm.c_cc[VTIME] = 0;	/* TIME timeout value */
#endif
}




/*-----------------------------------------------------------------------*\
 |  command  --  called when user-level commands are needed by the z80
 |  for some reason or another
\*-----------------------------------------------------------------------*/

static void
command(z80info *z80)
{
	int i, j, t, e;
	char str[256], *s;
	FILE *fp;
	static word pe = 0;
	static word po = 0;

	resetterm();
	printf("\n");

loop:	/* "infinite" loop */

	/* prompt for a command from the user & then do it */
	printf("Cmd: ");
	fflush(stdout);
	*str = '\0';
	fgets(str, sizeof str - 1, stdin);

	for (s = str; *s == ' ' || *s == '\t'; s++)
		;

	switch (isupper(*(unsigned char *)s) ? tolower(*(unsigned char *)s) : *s)
	{
	case '?':					/* help */
		printf("   Q(uit)  T(race on/off)  S(tep trace)  D(ump regs)\n");
		printf("   E(xamine memory)  P(oke memory)  R(egister modify)\n");
		printf("   L(oad binary)  C(ontinue running - <CR> if Step)\n");
		printf("   G(o) B(oot CP/M)  Z(80 disassembled dump)\n");
		printf("   W(write memory to file)  X,Y(-set/clear breakpoint)\n");
		printf("   O(output to \"logfile\")\n\n");
		printf("   !(fork shell)  ?(command list)  V(ersion)\n\n");
		break;

	case 'o':
		if (logfile != NULL)
		{
			fclose(logfile);
			logfile = NULL;
			printf("    Logging off.\n");
		}
		else
		{
			printf("    Logfile name? ");
			jgets(str, sizeof(str), stdin);

			for (s = str; isspace(*(unsigned char *)s); s++)
				;

			if (*s == '\0')
				break;

			logfile = fopen(s, "w");

			if (logfile == NULL)
				printf("Cannot open logfile!\n");
			else
				printf("    Logging on.\n");
		}

		break;

	case '!':				/* fork a shell */
		system("exec ${SHELL:-/bin/sh}");
		initterm();
		printf("\n");
		break;

	case 'q':				/* quit */
		if (logfile != NULL)
			fclose(logfile);

		exit(0);
		break;

	case 'v':				/* version */
		printf("  Version %s\n", VERSION);
		break;

	case 'b':				/* boot cp/m */
		setterm();
		sysreset(z80);
		return;
		break;

	case 't':				/* toggle trace mode */
		z80->trace = !z80->trace;
		printf("    Trace %s\n", z80->trace ? "on" : "off");
		break;

	case 's':				/* toggle step-trace mode */
		z80->step = !z80->step;
		printf("    Step-trace %s\n", z80->step ? "on" : "off");
		printf("    Trace %s\n", z80->trace ? "on" : "off");
		break;

	case 'd':					/* dump registers */
		dumptrace(z80);
		break;

	case 'e':					/* examine memory */
		printf("    Starting at loc? (%.4X) : ", pe);
		jgets(str, sizeof(str), stdin);
		t = pe;
		sscanf(str, "%x", &t);
		pe = t;

		for (i = 0; i <= 8; i++)
		{
			printf("  %.4X:   ", pe);

			for (j = 0; j <= 0xF; j++)
				printf("%.2X  ", z80->mem[pe++]);

			printf("\n");
		}

		break;
	
	case 'w':			/* write memory to file */
		printf("    Starting at loc? ");
		jgets(str, sizeof(str), stdin);
		sscanf(str, "%x", &t);
		printf("    Ending at loc? ");
		jgets(str, sizeof(str), stdin);
		sscanf(str, "%x", &e);
		fp = fopen("mem", "w");

		if (fp == NULL)
			printf("Cannot open file 'mem' for writing!\n");
		else
		{
			j = 0;

			for (i = t; i < e; i++)
			{
				if (j++ > 9)
				{
					fprintf(fp, "\n");
					j = 0;
				}

				fprintf(fp, "0x%X, ", z80->mem[i]);
			}

			fprintf(fp, "\n");
			fclose(fp);
		}

		break;

	case 'x':			/* set breakpoint */
#ifdef MEM_BREAK
		printf("    Set breakpoint at loc? (A for abort): ");
		jgets(str, sizeof(str), stdin);

		if (tolower(*(unsigned char *)str) == 'a' || *str == '\0')
			break;

		sscanf(str, "%x", &t);

		if (t < 0 || t >= sizeof z80->mem)
		{
			printf("Cannot set breakpoint at addr 0x%X\n", t);
			break;
		}

		if (!(z80->membrk[t] & M_BREAKPOINT))
		{
			printf("    Breakpoint set at addr 0x%X\n", t);
			z80->membrk[t] |= M_BREAKPOINT;
			z80->numbrks++;
		}
#else
		printf("Sorry, Z80 has not been compiled with MEM_BREAK.\n");
#endif /* MEM_BREAK */
		break;

	case 'y':			/* clear breakpoints */
#ifdef MEM_BREAK
		printf("    Clear breakpoint at loc? (A for all) : ");
		jgets(str, sizeof(str), stdin);

		if (tolower(*(unsigned char *)str) == 'a')
		{
			for (i = 0; i < sizeof z80->membrk; i++)
				z80->membrk[i] &= ~M_BREAKPOINT;

			z80->numbrks = 0;
			printf("    All breakpoints cleared\n");
			break;
		}

		sscanf(str, "%x", &t);

		if (t < 0 || t >= sizeof z80->mem)
		{
			printf("    Cannot clear breakpoint at addr 0x%X\n", t);
			break;
		}

		if (z80->membrk[t] & M_BREAKPOINT)
		{
			printf("Breakpoint cleared at addr 0x%X\n", t);
			z80->membrk[t] &= ~M_BREAKPOINT;
			z80->numbrks--;
		}
#else
		printf("Sorry, Z80 has not been compiled with MEM_BREAK.\n");
#endif /* MEM_BREAK */
		break;

	case 'z':			/* z80 disassembled memory dump */
		printf("    Starting at loc? (%.4X) : ", pe);
		jgets(str, sizeof(str), stdin);
		t = pe;
		sscanf(str, "%x", &t);
		pe = t;

		for (i = 0; i < 0x10; i++)
		{
			printf("  %.4X:    ", pe);
			j = pe;
			pe += disassem(z80, pe, stdout);
			t = disassemlen(z80);

			while (t++ < 15)
				putchar(' ');

			while (j < pe)
				printf("  %.2X", z80->mem[j++]);

			printf("\n");
		}

		break;

	case 'p':				/* poke memory */
		printf("    Start at loc? (%.4X) : ", po);
		jgets(str, sizeof(str), stdin);
		sscanf(str, "%x", &i);
		po = i;

		for (;;)
		{
			printf("    Mem[%.4X] (%.2X) = ", po, z80->mem[po]);
			jgets(str, sizeof(str), stdin);

			for (s = str; *s == ' ' || *s == '\t'; s++)
				;

			if (*s == '~')			/* exit? */
			{
				po = i;
				break;
			}

			if (*s == '\0')		/* leave the value alone */
				continue;

			j = 0;
			sscanf(str, "%x", &j);
			z80->mem[po] = j;
			po++;
		}
		break;
			
	case 'r':				/* set a register */
		printf("    Value? = ");
		jgets(str, sizeof(str), stdin);
		i = 0;
		sscanf(str, "%x", &i);
		printf("    Reg? (A,F,B,C,D,E,H,L,IX,IY,SP,PC) : ");
		jgets(str, sizeof(str), stdin);

		for (s = str; *s == ' ' || *s == '\t'; s++)
			;

		switch (tolower(*(unsigned char *)s))
		{
		case 'a': A = i; break;
		case 'f': F = i; break;
		case 'b': B = i; break;
		case 'c': C = i; break;
		case 'd': D = i; break;
		case 'e': E = i; break;
		case 'h': H = i; break;
		case 'l': L = i; break;
		case 'i': 
			if (tolower(((unsigned char *)s)[1]) == 'x')
				IX = i;
			else if (tolower(((unsigned char *)s)[1]) == 'y')
				IY = i;

			break;

		case 'x': IX = i; break;
		case 'y': IY = i; break;
		case 's': SP = i; break;
		case 'p': PC = i; break;

		default:
			printf("No such register\n");
			break;
		}

		break;

	case 'l':			/* load a file into z80 memory */
		printf("    File-name: ");
		jgets(str, sizeof(str), stdin);

		if (!loadfile(z80, str))
			fprintf(stderr, "Cannot load file %s!\r\n", str);

		break;

	case '\0':			/* carriage-return */
	case '\r':
	case '\n':
		if (z80->trace && z80->step)
			goto cont;

		break;

	case 'c':			/* continue z80 execution */
	case 'g':
	cont:
		setterm();

		if (z80->trace)
		{
			z80->event = TRUE;
			z80->halt = TRUE;
		}

		return;

	default:
		/*putchar('\007');*/
		printf("\007Command \"%s\" not recognized\n", s);
		break;
	}

	goto loop;
}




/*-----------------------------------------------------------------------*\
 |  dumptrace  --  dump the z80 registers in an easy-to-trace format
 |  --  note that the dump takes exactly one line so that changes in
 |  register values are easier to spot  --  disassembles the z80 code
\*-----------------------------------------------------------------------*/

static void
dumptrace(z80info *z80)
{
	printf("a%.2X f%.2X bc%.4X de%.4X hl%.4X ",
			A, F, BC, DE, HL);
	printf("ix%.4X iy%.4X sp%.4X pc%.4X:%.2X  ",
			IX, IY, SP, PC, z80->mem[PC]);
	disassem(z80, PC, stdout);
	printf("\r\n");

	if (logfile)
	{
		fprintf(logfile, "a%.2X f%.2X bc%.4X de%.4X hl%.4X ",
				A, F, BC, DE, HL);
		fprintf(logfile, "ix%.4X iy%.4X sp%.4X pc%.4X:%.2X  ",
				IX, IY, SP, PC, z80->mem[PC]);
		disassem(z80, PC, logfile);
		fprintf(logfile, "\r\n");
	}
}



#define HEXVAL(c)	(('0' <= (c) && (c) <= '9') ? (c) - '0' :\
			(('a' <= (c) && (c) <= 'f') ? (c) - 'a' + 10 :\
			(('A' <= (c) && (c) <= 'F') ? (c) - 'A' + 10 :\
				-1 )))

static int
gethex(FILE *fp)
{
	int i, j;

	i = getc(fp);
	j = getc(fp);

	if (i < 0 || j < 0)
		return -1;

	i = HEXVAL(i);
	j = HEXVAL(j);

	if (i < 0 || j < 0)
		return -1;

	return (i << 4) | j;
}


static int
loadhex(z80info *z80, FILE *fp)
{
	int start = TRUE;
	int len, line, i;
	word addr, check, t;

	for (line = 1; getc(fp) >= 0; line++)		/* should be a ':' */
	{
		if ((len = gethex(fp)) <= 0)
			break;

		check = len;

		if ((i = gethex(fp)) < 0)
			break;

		addr = (word)i;
		check += addr;

		if ((i = gethex(fp)) < 0)
			break;

		t = (word)i;
		check += t;
		addr = (addr << 8) | t;

		if (start)
			PC = addr, start = FALSE;

		if ((i = gethex(fp)) < 0)		/* ??? */
			break;

		check += (word)i;

		while (len-- > 0)
		{
			if ((i = gethex(fp)) < 0)
				break;

			t = (word)i;
			check += t;
			z80->mem[addr] = t;
			addr++;
		}

		if ((i = gethex(fp)) < 0)		/* checksum */
			break;

		t = (word)i;

		if ((t + check) & 0xFF)
		{
			fprintf(stderr, "%d: Checksum error: %.2X != 0!\r\n",
					line, (t + check) & 0xFF);
			return FALSE;
		}

		if (getc(fp) < 0)		/* should be a '\n' */
			break;
	}

	return TRUE;
}



/*-----------------------------------------------------------------------*\
 |  getword  --  return a 16-bit word from the specified file
\*-----------------------------------------------------------------------*/

static int
getword(FILE *file)
{
	int w;

	w = getc(file) << 8;
	w |= getc(file);
	return w;
}



/*-----------------------------------------------------------------------*\
 |  loadpisces  --  load the specified file (assumed to be in Pisces+
 |  format) into the z80 memory for subsequent execution
\*-----------------------------------------------------------------------*/

static int
loadpisces(z80info *z80, FILE *file)
{
	int numbytes, i;
	unsigned short loadaddr;

	/* ignore the 1st 12 words in the file - the 13th word is the starting
	   PC value - the 14th is also ignored */
	for (i = 0; i < 12; i++)
		getword(file);

	PC = getword(file);
	getword(file);

	/* read in each block of words into the z80 memory - each block
	   specifies the number of bytes in the block and the address to load
	   the data into */
	while (getword(file) != EOF)
	{
		numbytes = getword(file);
		loadaddr = getword(file);
		getword(file);

		for (; numbytes > 0; numbytes -= 2)
		{
			z80->mem[loadaddr] = getc(file);
			loadaddr++;
			z80->mem[loadaddr] = getc(file);
			loadaddr++;
		}
	}

	return TRUE;
}


static void
suffix(char *str, const char *suff)
{
	while(*str != '\0' && *str != '.')
		str++;

	strcpy(str, suff);
}


boolean
loadfile(z80info *z80, const char *fname)
{
	char buf[200];
	FILE *fp;
	int ret;

	if ((fp = fopen(fname, "r")) != NULL)
	{
		ret = loadhex(z80, fp);
		fclose(fp);
		return ret;
	}

	strcpy(buf, fname);
	suffix(buf, ".hex");

	if ((fp = fopen(buf, "r")) != NULL)
	{
		ret = loadhex(z80, fp);
		fclose(fp);
		return ret;
	}

	strcpy(buf, fname);
	suffix(buf, ".X");

	if ((fp = fopen(buf, "r")) != NULL)
	{
		ret = loadpisces(z80, fp);
		fclose(fp);
		return ret;
	}

	return FALSE;
}



/* input  --  z80 input instruction  --  this function is called whenever
   an input ports is referenced from the z80 to handle the real I/O  --
   it returns a byte to the z80 just like the real I/O instruction  --
   the arguments represent the data on the bus as it would be for a real
   z80 - this routine is restarted later if there is no input pending,
   and we must wait for some to occur */

boolean
input(z80info *z80, byte haddr, byte laddr, byte *val)
{
	int data;

	/* just uses the lower 8-bits of the I/O address for now... */
	switch (laddr)
	{

	/* return a character from the keyboard - wait for it if necessary  --
	   return "last" if we have already read in something via 0x01 */
	case 0x00:
		if (1)
		{
#if defined macintosh
			EventRecord ev;
			
		again:
			fflush(stdout);

			while (!WaitNextEvent(keyDownMask | autoKeyMask,
					&ev, 20, nil))
				;

			data = ev.message & charCodeMask;

			if ((data == '.' && (ev.modifiers & cmdKey)) ||
					data == INTR_CHAR)
			{
				command(z80);
				goto again;
			}
			else if (data == 'q' && (ev.modifiers & cmdKey))
				exit(0);
#elif defined DJGPP
			fflush(stdout);
			data = getkey();

			while (data == INTR_CHAR)
			{
				command(z80);
				data = getkey();
			}
#else	/* TCGETA */
			fflush(stdout);
			data = kget(0);
//			data = getchar();

			while ((data < 0 && errno == EINTR) ||
					data == INTR_CHAR)
			{
				command(z80);
				data = kget(0);
//				data = getchar();
			}
#endif
		}

		*val = data & 0x7F;
		break;

	/* return 0xFF if we have a character waiting to be read - save the
	   character in "last" for 0x00 above */
	case 0x01: 
#if defined macintosh
		{
			EventRecord ev;
			*val = EventAvail(keyDownMask | autoKeyMask, &ev) ?
					0xFF : 0;
		}
#elif defined DJGPP
		*val = (kbhit()) ? 0xFF : 0;
#else	/* UNIX or BeBox */
		fflush(stdout);

		if (constat())
			*val = 0xFF;
		else
			*val = 0x00;

#endif
		break;

	/* default - prompt the user for an input byte */
	default:
		resetterm();
		printf("INPUT : addr = %X%X    DATA = ", haddr, laddr);
		fflush(stdout);
		scanf("%x", &data);
		setterm();
		*val = data;
		break;
	}

	return TRUE;
}


/*-----------------------------------------------------------------------*\
 |  output  --  output the data at the specified I/O address
\*-----------------------------------------------------------------------*/

void
output(z80info *z80, byte haddr, byte laddr, byte data)
{
	if (laddr == 0xFF) {
		/* BIOS call - interrupt the z80 before the next instruction 
		   since we may have to mess with the PC & other stuff - 
		   otherwise we would do it right here */
		z80->event = TRUE;
		z80->halt = TRUE;
		z80->syscall = TRUE;
		z80->biosfn = data;

		if (z80->trace)
		{
			printf("BIOS call %d\r\n", z80->biosfn);

			if (logfile)
				fprintf(logfile, "BIOS call %d\r\n",
					z80->biosfn);
		}
	} else if (laddr == 0) {
		/* output a character to the screen */
		// putchar(data);
		vt52(data);

		if (logfile != NULL)
			putc(data, logfile);
	} else {
		/* dump the data for our user */
		printf("OUTPUT: addr = %X%X  DATA = %X\r\n", haddr, laddr,data);
	}
}



/*-----------------------------------------------------------------------*\
 |  haltcpu  --  this is called after the z80 halts  --  it is used for
 |  tracing & such
\*-----------------------------------------------------------------------*/

void
haltcpu(z80info *z80)
{
	z80->halt = FALSE;
	
	/* we were interrupted by a Unix signal */
	if (z80->sig)
	{
		if (z80->sig != SIGINT)
			printf("\r\nCaught signal %d.\r\n", z80->sig);

		z80->sig = 0;
		command(z80);
		return;
	}

	/* we are tracing execution of the z80 */
	if (z80->trace)
	{
		/* re-enable tracing */
		z80->event = TRUE;
		z80->halt = TRUE;
		dumptrace(z80);

		if (z80->step)
			command(z80);
	}

	/* a CP/M syscall - done here so tracing still works */
	if (z80->syscall)
	{
		z80->syscall = FALSE;
		bios(z80, z80->biosfn);
	}
}

word
read_mem(z80info *z80, word addr)
{
#ifdef MEM_BREAK
	if (z80->membrk[addr] & M_BREAKPOINT)
	{
		fprintf(stderr, "\r\nBreak at 0x%X\r\n", addr);
	}
	else if (z80->membrk[addr] & M_READ_PROTECT)
	{
		fprintf(stderr,
			"\r\nAttempt to read protected memory at 0x%X\r\n",
			addr);
	}
	else if (z80->membrk[addr] & M_MEM_MAPPED_IO)
	{
		fprintf(stderr,
			"\r\nAttempt to perform mem-mapped input at 0x%X\r\n",
			addr);
		/* fake some sort of I/O here and return its value */
	}

	dumptrace(z80);
	command(z80);
#endif	/* MEM_BREAK */

	return z80->mem[addr];
}

word
write_mem(z80info *z80, word addr, byte val)
{
#ifdef MEM_BREAK
	if (z80->membrk[addr] & M_BREAKPOINT)
	{
		fprintf(stderr, "\r\nBreak at 0x%X\r\n", addr);
	}
	else if (z80->membrk[addr] & M_WRITE_PROTECT)
	{
		fprintf(stderr,
			"\r\nAttempt to write to protected memory at 0x%X\r\n",
			addr);
	}
	else if (z80->membrk[addr] & M_MEM_MAPPED_IO)
	{
		fprintf(stderr,
			"\r\nAttempt to perform mem-mapped output at 0x%X\r\n",
			addr);
		/* fake some sort of I/O here and set mem to its value, */
		/* then return */
	}

	dumptrace(z80);
	command(z80);
#endif	/* MEM_BREAK */

	return z80->mem[addr] = val;
}

void
undefinstr(z80info *z80, byte instr)
{
	printf("\r\nIllegal instruction 0x%.2X at PC=0x%.4X\r\n",
		instr, PC - 1);
	command(z80);
}



/*-----------------------------------------------------------------------*\
 |  quit -- terminate this program after cleaning up -- this it is       |
 |  intended to catch unused signals & not leave the terminal hosed      |
\*-----------------------------------------------------------------------*/

static void
quit(int sig)
{
	printf("\r\nCaught signal %d.\r\n", sig);
	resetterm();
	exit(2);
}


/* this is needed by both interrupt() and main() */
static z80info *z80 = NULL;


/*-----------------------------------------------------------------------*\
 |  interrupt  --  this is called when we get a usable signal from Unix
\*-----------------------------------------------------------------------*/

static void
interrupt(int s)
{
	/* we tell the z80 to stop when convenient, then reset & continue */
	if (z80 != NULL)
	{
	    z80->event = TRUE;
	    z80->halt = TRUE;
	    z80->sig = s;
	}

	signal(s, interrupt);
}


/*-----------------------------------------------------------------------*\
 |  main  --  set up the global vars & run the z80 
\*-----------------------------------------------------------------------*/

int
main(int argc, const char *argv[])
{
	int x;
	char cmd[256];
	int help = 0;

	cmd[0] = 0;

	for (x = 1; argv[x]; ++x) {
		if (argv[x][0] == '-' && argv[x][1] == '-') {
			if (!strcmp(argv[x], "--help")) {
				help = 1;
			} else if (!strcmp(argv[x], "--nobdos")) {
				nobdos = 1;
			} else if (!strcmp(argv[x], "--trace_bdos")) {
				trace_bdos = 1;
			} else if (!strcmp(argv[x], "--strace")) {
				strace = 1;
			} else {
				fprintf(stderr, "Unknown option %s\n", argv[x]);
				exit(1);
			}
		} else {
			if (!cmd[0]) {
				strcpy(cmd, argv[x]);
			} else {
				strcat(cmd, " ");
				strcat(cmd, argv[x]);
			}
		}
	}

	if (help) {
		fprintf(stderr, "\n%s [options] [CP/M command]\n", argv[0]);
		fprintf(stderr, "\n   Options:\n\n");
		fprintf(stderr, "    --help         Show this help\n");
		fprintf(stderr, "    --nobdos       Do not emulate BDOS: only emulate BIOS\n");
		fprintf(stderr, "                   Real disk images will be used.        \n");
		fprintf(stderr, "    --trace_bdos   Trace BDOS calls\n");
		fprintf(stderr, "\n");
		exit(0);
	}

	if (cmd[0]) {
		stuff_cmd = cmd;
	}

	z80 = new_z80info();

	if (z80 == NULL)
		return -1;

	initterm();

	/* set up the signals */
#ifdef SIGQUIT
	signal(SIGQUIT, quit);
#endif
#ifdef SIGHUP
	signal(SIGHUP, quit);
#endif
#ifdef SIGTERM
	signal(SIGTERM, quit);
#endif
#ifdef SIGINT
	signal(SIGINT, interrupt);
#endif

	setterm();

	sysreset(z80);

	while (1)
	{
#ifdef macintosh
		EventRecord ev;
		WaitNextEvent(0, &ev, 0, nil);
#endif
		z80_emulator(z80, 100000);
	}
}
