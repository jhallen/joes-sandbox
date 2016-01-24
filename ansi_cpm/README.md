
# ANSI CP/M Emulator

This emulator allows you to execute CP/M commands on UNIX (Linux or Cygwin). 

If a CP/M command is provided on the command line, it is executed
immediately.  For example, this will start WordStar:

	cpm ws

Otherwise, you will get the CP/M command prompt:

	cpm
	A>

By default, BDOS is emulated so that the current directory in UNIX appears
as the __A>__ drive from CP/M.  The BDOS emulation can be optionally
disabled.  In this case, P2DOS (a BDOS clone) is used to access real disk
images.

This is a modified version of Parag Patel's [CP/M](https://en.wikipedia.org/wiki/CP/M) emulator.  This
version includes a filter/emulator for [VT-52](https://en.wikipedia.org/wiki/VT52) and
[ADM-3A](https://en.wikipedia.org/wiki/ADM-3A) terminals. 
Basically it converts VT-52 codes into ANSI codes on output and converts
arrow keys and PgUp / PgDn into WordStar motion keys on input.

This allows screen oriented programs written for Kaypro-II such as WordStar,
Turbo PASCAL and dBASE-II to operate directly with ANSI terminal emulators,
such as Xterm or RXVT for Linux and Cygwin.

The output side of the VT52/ADM-3A and the BDOS emulation comes from
Benjamin C.  Sittler bsittler@iname.com from another emulator:
cpm-0.2.1-mod2.  CPM-0.2.1 is i686 only- it's partially written in assembly
language.  Hence I prefer Parag's all C emulator.

Type 'make' to build the program.

Type './cpm __command__' to execute a CP/M .COM file located in the current
directory.

Type './cpm' to get the __A>__ prompt.  Type __bye__ to exit back to UNIX.

Type './cpm --nobdos' to start it without BDOS emulation and instead use
disk images called A-Hdrive and B-Hdrive.  In this case:

At the __A>__ prompt:

Type __bye__ to exit back to Linux

Type __putunix cpm-file unixfile__ to copy a file out of CP/M and to UNIX/Linux.

Type __getunix unixfile cpm-file__ to copy a file from UNIX/Linux into CP/M.

The A-Hdrive and B-Hdrive disk images include some programs.  Many more are
available from here.

[http://www.retroarchive/org/cpm/](http://www.retroarchive.org/cpm/)

Here is CP/M WordStar running on Ubuntu in an xterm:

![CP/M WordStar](cpmws.png)

			-- Joe Allen

# Original README

This is a Z80 instruction-set simulator written entirely ANSI C.  It can
boot and run CP/M or CP/M clones.  It's also got a builtin debugger
which supports tracing, single-stepping, breakpoints, read or write
protected memory, memory-mapped I/O, logging, and disassembly.  This is
not the fastest emulator around, but it's very portable.

So far z80 has been tested under Linux, FreeBSD, DEC's UNIX on Alpha,
and SunOS, the Macintosh using either Think C or CodeWarrior (PPC and
68k), MS-DOS using DJGPP, and the BeOS.  It should be quite easy to port
to other UNIX-line systems.

The file "Makefile" will need tweaking for your system.  The two key
compilation flags are -DLITTLE_ENDIAN and -DPOSIX_TTY as described
further in the Makefile.  You'll almost certainly have to tweak the
other make variables, such as CFLAGS and CC.  Then just type "make".

For the Macintosh, the file "MacProj.hqx" is a BinHex SIT archive with
three project files for Think C, CodeWarrior 68k, and CW PowerPC.

The file "main.c" contains UNIX routines for character-mode I/O, if
these need to be ported to your flavor of UNIX.  They're pretty generic
and ought to work under just about anything.

Once z80 is built, run it, and enter 'b' at the prompt to boot CP/M.
Enter '?'  to see a list of debugging commands it understands.  The
interrupt character, ^C, is wanted by CP/M for its own uses, so to force
the z80 into its debugger, use ^_ (control-underscore).  (You may have
to type <return> after the ^_ to wake it up.)

If z80 is linked or renamed to "cpm", it will directly boot into CP/M
without displaying the debugger prompt.  If an "A-Hdrive" doesn't exist
in the current working directory for CP/M, it will be created.

The file "z80.c" contains the emulator proper.  Most variables are
accessed via macros defined in "defs.h".  The file "bios.c" contains the
mock-BIOS code that allows the emulator to boot and run CP/M or its
clones.  The file "cpm.c" contains an image of P2D0S 2.3 and ZCPR1, so
it should be safe to distribute freely.  (This allows the z80 to
directly boot CP/M.)

The z80 program will automatically create virtual CP/M "drives" as they
are accessed, and will only allocate disk space for them when it is
needed.  The obsolete "makedisc.c" program is included to allocate all
the space for a floppy at once and also allows placing a single Unix
file within it.

The "bios.c" code is built for two 5Mb virtual hard-drives (A-Hdrive,
and B-Hdrive), and a bunch of floppy drives (C-drive, D-drive, and so
on).  It is possible to rebuild it for different numbers of hard-drives
by changing the macro NUMHDISCS at the top.  The hard disks emulated are
the venerable ST-506 5Mb 5" 5Mb drives.  The floppies are the
traditional 8" 256k drives.  The code names the virtual hard-drives as
"?-Hdrive" and floppies as "?-drive" to help avoid accidentlally
confusing one for the other.  Their contents should be identical to the
real thing, even down to the reserved tracks, assuming I did it right.

There are several "*.mac" files which are Z80 macro-assembly sources for
getting files from UNIX into CP/M (GETUNIX.MAC), putting files out from
CP/M to UNIX (PUTUNIX.MAC), and quitting the emulator (BYE.MAC).  All
these files are also in "A-Hdrive", all assembled and ready to run.
They do little useful error checking, but it should be pretty obvious
from the code how they work.  Any text files transfered into CP/M must
have CR-LF as the line separator or it gets awfully confused.

Usage:
	PUTUNIX <CP/M-file> <unixfile>
	GETUNIX <unixfile> <CP/M-file>
	BYE

Additional utilities that come with the P2DOS23 distribution are also
included to support the date/time functions of P2DOS.  These are located
within the "A-Hdrive" and within the P2DOS archive.  These are date.com,
ddir.com, initdir.com, public.com, set.com, and touch.com.

I had picked up a Z80 assembler named "zmac" from the Internet quite
some time ago.  I've hacked it heavily to add a lot of support for
different assembler formats but it's still kind of finicky.  You'll need
yacc of some flavor to compile it up.  Sorry, but I don't have a
Makefile for it any longer.  The "*.z" files there are sample assembly
sources.  There's also a separate Z80 disassembler that came with zmac.
Unfortunately the sources did not come with any copyright, but the file
"zmac.y" does contain a somewhat cryptic list of folks who hacked it.  I
don't know if "zdis" is by the same authors or not - there are no
comments in it.

zmac assembled P2DOS 2.3 (off of the CP/M archives on oak.oakland.edu)
without too much trouble.  ZCPR1 (from the archives) needed to be
converted from the .ASM format to .MAC, which I did using XLATE5 (also
from the Oak archives).  Then the resulting output had to be hacked
somewhat so that zmac could assemble it.  Z80DOS23 and Z80CCP from the
archives also assembled up without much trouble.

To play with different BDOS/CCP replacements, just rename the assembler
output HEX file from wherever you built it into the files "bdos.hex" and
"ccp.hex".  If z80 sees these in the current directory, it will load
them to boot CP/M instead of using its builtin versions of P2DOS/ZCPR1.

There's a lot of stuff to do in this code, but it's functional enough to
leave the rest as an exercise for the student.  The Mac version needs a
really cool interface, with separate windows for the out, setting
registers, tracing, a virtual terminal, etc.  The debug-level prompting
code pretty much sucks and needs to be thrown out.

This code has been designed so that the z80 emulator proper should be
thread-safe and runs independently of CP/M.  It should be possible to
run multiple z80s within one program if desired.

The CP/M layer may be built so that the fake BIOS is turned into real
Z80 code with all I/O occurring through fake devices using either
INPUT/OUTPUT or the memory-mapped I/O hooks.  Conversely, it's also
possible to turn then entire BDOS/CCP into C code much as the BIOS is
currently coded with magic hooks that trigger the appropriate actions.

Use this code as you wish, so long as it is properly attributed.  That
is, display our copyrights where you would display yours (manuals,
boot-up, etc) and do not claim that this code is yours.  If you do use
it, please drop me a note.  This code is "as-is" without any warrantee.
Be warned that P2DOS23 and ZCPR1 only allow free redistribution for
non-commercial use.  (See their documentation for more details.)

Copyright 1986-1988 by Parag Patel.  All Rights Reserved.
Copyright 1994-1995 by CodeGen, Inc.  All Rights Reserved.


		-- Parag Patel <parag@cgt.com>


Version history:

3.1	Added support for virtual 5Mb ST-506 drives in bios.c.
	Fixed a few bugs in emulator, fake BIOS, and console I/O.
	String search for "cpm" in argv[0] now uses strrchr() in main.c.
	Change all "(char)" casts to "(signed char)" for AIX and other
		systems that have "char" default to "unsigned".
	Making a start at adding support for printing.

3.0	First public release 1995.
	Use publicly available P2DOS23 and ZCPR1 instead of
		copyrighted CP/M 2.2.
	Added date/time support for P2DOS and its utilities.

## Readme from cpm-0.2.1-mod2

The Kaypro 2x supports several extensions to the ADM 3A terminal
escapes, and I added translations to vt52() for some of these
extensions. vt52() should probably be replaced by a proper curses
applications so we don't depend on having an ANSI terminal, but I'm
lazy...

Here are the added escape sequences:

	ESC E (insert line)
	ESC R (delete line)
	ESC B 0 (start reverse video)
	ESC C 0 (stop reverse video)
	ESC B 1 (start half intensity)
	ESC C 1 (stop half intensity)
	ESC B 2 (start blinking)
	ESC C 2 (stop blinking)
	ESC B 3 (start underlining)
	ESC C 3 (stop underlining)
	ESC B 4 (cursor on)
	ESC C 4 (cursor off)
	ESC B 6 (remember cursor position)
	ESC C 6 (restore cursor position)

The following sequences are recognized by vt52() but don't cause any
ANSI output:

	ESC B 5 (enter video mode)
	ESC C 5 (leave video mode)
	ESC B 7 (preserve status line)
	ESC C 7 (don't preserve status line)
	ESC * row+32 col+32 (set pixel)
	ESC SPC row+32 col+32 (clear pixel)
	ESC L row+32 col+32 row+32 col+32 (set line)
	ESC D row+32 col+32 row+32 col+32 (delete line)

I also added a VBELL #define in io.c which causes those annoying BELs
to be replaced by temporary flashing, providing your terminal supports
the ESC [ ? 5 h and ESC ? 5 l sequences (invert and revert screen.)
Both XFree86 3.3.1 xterm and the Linux console support them, so I'm
happy.

 -- "Benjamin C. W. Sittler" <bsittler@iname.com>
