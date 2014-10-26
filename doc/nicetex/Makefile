CFLAGS = -Wall

all: nicetex tocinsert npass nlink

nicetex: nicetex.o
	$(CC) -g -o nicetex nicetex.o

tocinsert: tocinsert.o
	$(CC) -g -o tocinsert tocinsert.o

npass: npass.o
	$(CC) -g -o npass npass.o

nlink: nlink.o
	$(CC) -g -o nlink nlink.o

install: nicetex tocinsert npass nlink fig figps nice.tex README
	install nicetex ${HOME}/bin
	install tocinsert ${HOME}/bin
	install npass ${HOME}/bin
	install nlink ${HOME}/bin
	install fig ${HOME}/bin
	install figps ${HOME}/bin
	install nice.tex ${HOME}/etc
	install README ${HOME}/etc/nicetex.doc

clean:
	rm -f *~
	rm -f *.bak
	rm -f *.o
