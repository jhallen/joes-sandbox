CFLAGS = -pg -g -Wall
#CFLAGS = -O2 -Wall
CC = gcc

# LIBS = -lm -lreadline -lcurses
# For Cygwin...
# LIBS = -lm -lreadline -ltermcap
LIBS = -lm -lreadline

OBJS = ivy_parse.o ivy_tree.o main.o free_list.o hash.o atom.o \
  ivy_frag.o ivy_codegen.o ivy_run.o ivy_rtlib.o ivy_help.o

ivy: $(OBJS)
	$(CC) $(CFLAGS) -pg -o ivy $(OBJS) $(LIBS)

ivy_online.h: ../doc/manual.txt mkman
	./mkman <../doc/manual.txt >ivy_online.h

ivy_help.o: ivy_online.h ivy_help.c
	$(CC) $(CFLAGS) -c ivy_help.c

mkman: mkman.o
	$(CC) $(CFLAGS) -o mkman mkman.o

clean:
	/bin/rm -f ${OBJS} ${OBJS:.o=.d} *~ mkman mkman.d

# include dependancy files if they exist
-include $(OBJS:.o=.d) mkman.d

# compile and generate dependency info
%.o: %.c
	$(CC) -c $(CFLAGS) $*.c -o $*.o
	@$(CC) -MM $(CFLAGS) $*.c > $*.d
# Improve dependency file produced by gcc... allows files to be renamed and source files to exist
# in subdirectories.
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp
