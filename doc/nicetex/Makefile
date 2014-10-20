OBJS = nicetex.o

nicetex: $(OBJS)
	$(CC) -g -o nicetex $(OBJS)

install:
	install nicetex ${HOME}/bin
	install fig ${HOME}/bin
	install figps ${HOME}/bin
	install nice.tex ${HOME}/etc

clean:
	rm -f *~
	rm -f *.bak
	rm -f _tmp/*.o
