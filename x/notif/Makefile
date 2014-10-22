CC = cc
CFLAGS = -O2 -g

OBJS = bar.o border.o button.o cascade.o dragdrop.o draw.o edit.o event.o \
 gsep.o help.o icon.o icons.o image.o index.o list.o lith.o menu.o place.o \
 radio.o scope.o scroll.o slider.o tedit.o text.o toggle.o widget.o wind.o

notif.a: $(OBJS)
	rm -f notif.a
	ar r notif.a $(OBJS)
	ranlib notif.a

clean:
	rm -f *~
	rm -f *.o
