
all: tabify tab base64 crlf hd exotobin bintoc csv csvcmp lfsr lpf reindent \
 rev subtotal subtotal1 upc upccheck chk mcs hex exo bomcmp

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

bintoc: bintoc.o
	$(CC) -o bintoc bintoc.o

exotobin: exotobin.o
	$(CC) -o exotobin exotobin.o

csv: csv.o
	$(CC) -o csv csv.o

csvcmp: csvcmp.o
	$(CC) -o csvcmp csvcmp.o

bomcmp: bomcmp.o
	$(CC) -o bomcmp bomcmp.o

lfsr: lfsr.o
	$(CC) -o lfsr lfsr.o

lpf: lpf.o
	$(CC) -o lpf lpf.o

reindent: reindent.o
	$(CC) -o reindent reindent.o

rev: rev.o
	$(CC) -o rev rev.o

subtotal: subtotal.o
	$(CC) -o subtotal subtotal.o

subtotal1: subtotal1.o
	$(CC) -o subtotal1 subtotal1.o

upc: upc.o
	$(CC) -o upc upc.o

upccheck: upccheck.o
	$(CC) -o upccheck upccheck.o

mcs: mcs.o
	$(CC) -o mcs mcs.o

chk: chk.o
	$(CC) -o chk chk.o

hex: hex.o
	$(CC) -o hex hex.o

exo: exo.o
	$(CC) -o exo exo.o

install:
	install tabify $(HOME)/bin
	install tab $(HOME)/bin
	install base64 $(HOME)/bin
	install crlf $(HOME)/bin
	install hd $(HOME)/bin
	install exotobin $(HOME)/bin
	install bintoc $(HOME)/bin
	install csv $(HOME)/bin
	install csvcmp $(HOME)/bin
	install bomcmp $(HOME)/bin
	install lfsr $(HOME)/bin
	install lpf $(HOME)/bin
	install reindent $(HOME)/bin
	install rev $(HOME)/bin
	install subtotal $(HOME)/bin
	install subtotal1 $(HOME)/bin
	install upc $(HOME)/bin
	install upccheck $(HOME)/bin
	install mcs $(HOME)/bin
	install chk $(HOME)/bin
	install hex $(HOME)/bin
	install exo $(HOME)/bin

clean:
	rm -f *.o *~
