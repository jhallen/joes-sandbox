CFLAGS = -Wall

all: calc quick crcgen crct

calc: calc.o
	$(CC) -o calc calc.o -lm

quick: quick.o
	$(CC) -o quick quick.o -lm

crcgen: crcgen.o
	$(CC) -o crcgen crcgen.o

crct: crct.o
	$(CC) -o crct crct.o

clean:
	rm -f *~ *.o
