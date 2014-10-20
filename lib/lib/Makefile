# Makefile for Joe's Own Editor

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

CFLAGS = -g

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

OBJS = ascii.o bipopen.o blocks.o callback.o cmd.o hash.o int.o path.o queue.o \
 random.o rc.o rmgr.o scrn.o serv.o termcap.o tim.o va.o vfile.o vs.o \
 zstr.o

# That's it!

# How to make joe from object files.  Object files are made from source
# files using make's built-in rules.

test: test.o joelib.a
	$(CC) $(CFLAGS) -o test test.o joelib.a

joelib.a: $(OBJS)
	rm -f joelib.a
	ar r joelib.a $(OBJS)
	ranlib joelib.a

# All object files depend on config.h

$(OBJS): config.h

# How to make config.h

config.h:
	$(CC) conf.c -o conf
	./conf $(WHERERC) $(TERMINFO)

# How to make termidx

termidx: termidx.o
	$(CC) $(CFLAGS) -o termidx termidx.o

# Install proceedure

install: user termidx
	strip user
	strip termidx
	if [ ! -d $(WHEREJOE) ]; then mkdir $(WHEREJOE); chmod a+rx $(WHEREJOE); fi
	rm -f $(WHEREJOE)/user $(WHEREJOE)/termidx
	mv user $(WHEREJOE)
	mv termidx $(WHEREJOE)
	if [ ! -d $(WHERERC) ]; then mkdir $(WHERERC); chmod a+rx $(WHERERC); fi
	rm -f $(WHERERC)/userrc $(WHEREMAN)/user.1
	cp userrc $(WHERERC)
	cp user.1 $(WHEREMAN)
	chmod a+x $(WHEREJOE)/user
	chmod a+r $(WHERERC)/userrc
	chmod a+r $(WHEREMAN)/user.1
	chmod a+x $(WHEREJOE)/termidx
	rm -f $(WHERERC)/termcap
	cp termcap $(WHERERC)/termcap
	chmod a+r $(WHERERC)/termcap
	rm -f $(WHERERC)/terminfo
	cp terminfo $(WHERERC)/terminfo
	chmod a+r $(WHERERC)/terminfo

# Cleanup proceedure

clean:
	rm -f $(OBJS) termidx.o conf conf.o config.h joelib.a
