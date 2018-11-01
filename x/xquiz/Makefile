# apt-get install xorg-dev

CFLAGS = -g

OBJS = box.o builtin.o button.o display.o graph.o main.o \
 msg.o parse.o prompt.o pulldown.o queue.o quiz.o scan.o \
 simplify.o sym.o unparse.o work.o xjunk.o

quiz: $(OBJS)
	cc -o quiz $(OBJS) -L/usr/lib/x86-linux-gnu -lX11 -lm

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
