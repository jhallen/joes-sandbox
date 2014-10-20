CC = gcc
CFLAGS = -O
OBJS = sc.o blocks.o heap.o main.o random.o queue.o zstr.o disp.o object.o \
       gedit.o

draft: $(OBJS) ttyxenix.o
	$(CC) $(CFLAGS) -o draft $(OBJS) ttyxenix.o -lm -lx
