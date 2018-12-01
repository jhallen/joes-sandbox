all: build ivy

CFLAGS = -pg -g -Wall
#CFLAGS = -O3 -Wall
CC = gcc

# LIBS = -lm -lreadline -lcurses
# For Cygwin...
# LIBS = -lm -lreadline -ltermcap
LIBS = -lm -lreadline


OBJS = ivy_parse.o ivy_tree.o main.o ivy_free_list.o ivy_hash.o ivy_symbols.o \
  ivy_frag.o ivy_codegen.o ivy_run.o ivy_rtlib.o ivy_help.o ivy_gc.o \
  ivy_str.o ivy_obj.o

SUBDIR_OBJS = $(addprefix build/, $(OBJS))

ivy: $(SUBDIR_OBJS)
	$(CC) $(CFLAGS) -pg -o ivy $(SUBDIR_OBJS) $(LIBS)

ivy_online.h: ../doc/manual.md build/mkman
	build/mkman <../doc/manual.md >ivy_online.h

build/ivy_help.o: ivy_online.h ivy_help.c
	$(CC) $(CFLAGS) -c ivy_help.c -o build/ivy_help.o

build/mkman: build/mkman.o
	$(CC) $(CFLAGS) -o build/mkman build/mkman.o

build:
	mkdir build

clean:
	/bin/rm -f build/* *~ ivy_online.h gmon.out

# include dependancy files if they exist
-include $(SUBDIR_OBJS:.o=.d) build/mkman.d

# compile and generate dependency info
build/%.o: %.c
	$(CC) -c $(CFLAGS) $*.c -o build/$*.o
	@$(CC) -MM $(CFLAGS) $*.c > build/$*.d
# Improve dependency file produced by gcc... allows files to be renamed and source files to exist
# in subdirectories.
	@mv -f build/$*.d build/$*.d.tmp
	@sed -e 's|.*:|build/$*.o:|' < build/$*.d.tmp > build/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < build/$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> build/$*.d
	@rm -f build/$*.d.tmp
