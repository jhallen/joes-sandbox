OBJS = _tmp/main2.o _tmp/string.o _tmp/scrn.o _tmp/termcap.o _tmp/tty.o _tmp/util.o _tmp/parse.o _tmp/kbd.o \
  _tmp/widget.o _tmp/pmgr.o _tmp/text.o _tmp/field.o _tmp/editbuf.o _tmp/edit.o _tmp/list.o _tmp/date.o

_tmp/tw: $(OBJS)
	g++ -g -o _tmp/tw $(OBJS)

# Rule to convert *.c file to _tmp/*.o file.
#   $< is dependancy (what matched right hand side)
#   $@ is target (what matched left hand side)

_tmp/%.o:%.c
	g++ -g -o $@ -c $<

clean:
	rm -f *~
	rm -f *.bak
	rm -f _tmp/*.o
