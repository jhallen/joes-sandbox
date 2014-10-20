OBJS = _tmp/nicetex.o

_tmp/nicetex: $(OBJS)
	cc -g -o _tmp/nicetex $(OBJS)

# Rule to convert *.c file to _tmp/*.o file.
#   $< is dependancy (what matched right hand side)
#   $@ is target (what matched left hand side)

_tmp/%.o:%.c
	cc -g -o $@ -c $<

clean:
	rm -f *~
	rm -f *.bak
	rm -f _tmp/*.o
