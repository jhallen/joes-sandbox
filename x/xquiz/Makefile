# apt-get install xorg-dev

CFLAGS = -g

OBJS = box.o builtin.o button.o display.o graph.o main.o \
 msg.o parse.o prompt.o pulldown.o queue.o quiz.o scan.o \
 simplify.o sym.o unparse.o work.o xjunk.o

quiz: $(OBJS)
	cc -o quiz $(OBJS) -L/usr/lib/x86-linux-gnu -lX11 -lm

clean:
	rm -f $(OBJS) *~
