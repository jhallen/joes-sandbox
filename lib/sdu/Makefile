CFLAGS = -g -Wall

OBJS = test.o meta.o parse.o hash.o

sdu: $(OBJS)
	$(CC) $(CFLAGS) -o sdu $(OBJS)

clean:
	rm -f *~ *.o
