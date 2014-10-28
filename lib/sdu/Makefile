CFLAGS = -g -Wall

OBJS = test.o sdu.o hash.o

sdu: $(OBJS)
	$(CC) $(CFLAGS) -o sdu $(OBJS)

clean:
	rm -f *~ *.o
