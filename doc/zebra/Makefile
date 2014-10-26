
all: form genpsion direct fnt

form: form.o
	$(CC) -o form form.o

genpsion: genpsion.o zstr.o
	$(CC) -o genpsion genpsion.o zstr.o

direct: direct.o zstr.o
	$(CC) -o direct direct.o zstr.o

fnt: fnt.o
	$(CC) -o fnt fnt.o
