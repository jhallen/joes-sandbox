CC = cc
CFLAGS = -O2 -g -I../notif -I../lib

OBJS = main.o keypad.o nkpfield.o selset.o clock.o vctrl.o jctrl.o lctrl.o pctrl.o \
 sctrl.o ldialog.o pdialog.o sdialog.o timefield.o jdialog.o caltouch.o

valve: $(OBJS)
	$(CC) -o valve $(OBJS) ../notif/notif.a ../lib/joelib.a -L /usr/X11R6/lib -lX11

clean:
	rm -f *~
	rm -f *.o
