
all: tabify tab base64 crlf hd exotobin

CFLAGS = -Wall

tabify: tabify.o
	$(CC) -o tabify tabify.o

tab: tab.o
	$(CC) -o tab tab.o

base64: base64.o
	$(CC) -o base64 base64.o

crlf: crlf.o
	$(CC) -o crlf crlf.o

hd: hd.o
	$(CC) -o hd hd.o

exotobin: exotobin.o
	$(CC) -o exotobin exotobin.o

install:
	install tabify $(HOME)/bin
	install tab $(HOME)/bin
	install base64 $(HOME)/bin
	install crlf $(HOME)/bin
	install hd $(HOME)/bin
	install exotobin $(HOME)/bin

clear:
	rm -f *.o
