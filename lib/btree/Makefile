# Makefile

###############
# Configuration
###############

# Set where you want joe to go, where you
# want joe's initialization file (joerc)
# to go and where you want the man page
# to go:

WHEREJOE = /usr/local/bin
WHERERC = /usr/local/lib
WHEREMAN = /usr/man/man1

# If you want to use TERMINFO, you have to set
# the following variable to 1.  Also you have to
# include some additional libraries- see below.

TERMINFO = 0

# You may also have to add some additional
# defines to get the include files to work
# right on some systems.
#
# for some HPUX systems, you need to add:  -D_HPUX_SOURCE

# C compiler options: make's built-in rules use this variable

CFLAGS = -O

# C compiler to use: make's built-in rules use this variable

CC = cc

# You may have to include some extra libraries
# for some systems
#
# for Xenix, add (in this order!!): -ldir -lx
#
# For some systems you might have to add: -lbsd
# to get access to the timer system calls.
#
# If you wish to use terminfo, you have to
# add '-ltinfo', '-lcurses' or '-ltermlib',
# depending on the system.

EXTRALIBS =

# Object files

OBJS = blocks.o btree.o hash.o main.o path.o queue.o random.o va.o vfile.o \
 vs.o zstr.o termcap.o tty.o va.o scrn.o

# That's it!

# How to make joe from object files.  Object files are made from source
# files using make's built-in rules.

btree: $(OBJS)
	$(CC) $(CFLAGS) -o btree $(OBJS) $(EXTRALIBS)

# All object files depend on config.h

$(OBJS): config.h

# How to make config.h

config.h:
	$(CC) conf.c -o conf
	./conf $(WHERERC) $(TERMINFO)

# Cleanup proceedure

clean:
	rm -f $(OBJS) conf conf.o config.h
