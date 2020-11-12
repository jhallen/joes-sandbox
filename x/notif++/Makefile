OBJS = border.o draw.o event.o help.o int.o kbd.o lith.o \
 parse.o string.o termcap.o text.o widget.o wind.o io.o \
 util.o icons.o icon.o button.o menu.o cascade.o \
 gsep.o slider.o bar.o toggle.o radio.o scroll.o edit.o \
 tedit.o index.o scope.o

CC = g++
CFLAGS = -g

all : test scope

test : notif.a main.o
	g++ -o test main.o notif.a -L/usr/X11R6/lib -lX11

tet : notif.a tet.o
	g++ -o tet tet.o notif.a -L/usr/X11R6/lib -lX11

scope : notif.a scopetest.o
	g++ -o scope scopetest.o notif.a -L/usr/X11R6/lib -lX11 -lm

notif.a : $(OBJS)
	rm -f notif.a
	ar r notif.a $(OBJS)
	ranlib notif.a

clean :
	rm *.o
