CC = cc
CFLAGS = -O
OBJS = parse.o main.o attr.o obj.o out.o fmt.o

all: psiman psicvt

psiman: $(OBJS)
	$(CC) $(CFLAGS) -o psiman $(OBJS)
	strip psiman

psicvt: psicvt.o
	$(CC) $(CFLAGS) -o psicvt psicvt.o
	strip psicvt

clean:
	/bin/rm -f $(OBJS) psicvt.o

