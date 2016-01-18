
OBJS = unasm.o utils.o exor.o sim6800.o asm6800.o mdos.o unasm6800.o exorterm.o

DIST = unasm.c utils.c exor.c sim6800.c asm6800.c mdos.c unasm6800.c mon.c exorterm.c Makefile \
  exbug.bin swtbug.bin utils.h sim6800.h asm6800.h unasm6800.h exor.h facts README COPYING \
  doc/doc.man doc/mdos-hello.txt doc/mdos-tech.txt doc/mdos.txt doc/mpl_upd.txt doc/mplnotes \
  doc/notes doc/fort.man

DISKS = \
  mdos.dsk \
  flex.dsk \
  disks/32479-1.dsk \
  disks/32479-10.dsk \
  disks/32479-3.dsk \
  disks/32479-8.dsk \
  disks/32479-9.dsk \
  disks/b220mast.dsk \
  disks/b300mast.dsk \
  disks/bascomp.dsk \
  disks/blank.dsk \
  disks/doc.dsk \
  disks/games.dsk \
  disks/gamesetc.dsk \
  disks/hll.dsk \
  disks/joe.dsk \
  disks/mace3814.dsk \
  disks/md201bc.dsk \
  disks/md22.dsk \
  disks/mdb301.dsk \
  disks/ug01-25.dsk \
  disks/ug26-50.dsk \
  disks/ug51-75.dsk \
  disks/ug76-90.dsk \
  disks/unlab02.dsk \
  disks/unlab03.dsk \
  disks/unlab04.dsk \
  disks/unlab06.dsk \
  disks/unlab07.dsk \
  disks/unlab08.dsk \
  disks/unlab09.dsk \
  disks/unlab10.dsk \
  disks/unlab11.dsk \
  disks/unlab12.dsk \
  disks/unlab13.dsk \
  disks/unlab14.dsk \
  disks/unlab16.dsk \
  disks/unlab17.dsk \
  disks/unlab18.dsk \
  disks/unlab21.dsk \
  disks/upd3_2.dsk \
  disks/upd_5.dsk

PDFS = \
  pdfs/M6800_Microprocessor_Applications_Manual_1975.pdf \
  pdfs/M6809PM.rev0_May83.pdf \
  pdfs/M68MDOS3_MDOS3um_Jun79.pdf \
  pdfs/Motorola_M6800_Programming_Reference_Manual_M68PRMD_Nov76.pdf \
  pdfs/m6800.pdf


NAME = exor-1.1

CFLAGS = -g

CC = gcc

all : mdos exor unasm

unasm : unasm.o utils.o unasm6800.o
	$(CC) -o unasm unasm.o utils.o unasm6800.o

exor : exor.o utils.o sim6800.o asm6800.o unasm6800.o mon.o exorterm.o
	$(CC) -o exor exor.o utils.o sim6800.o asm6800.o unasm6800.o mon.o exorterm.o

mdos : mdos.o
	$(CC) -o mdos mdos.o

clean :
	rm -f ${OBJS} ${OBJS:.o=.d}

dist :
	rm -rf $(NAME)
	mkdir $(NAME) && \
	(tar cf - $(DIST) | (cd $(NAME); tar -xf -)) && \
	tar cf - $(NAME) | gzip >$(NAME).tar.gz

joedist :
	rm -rf $(NAME)
	mkdir $(NAME) && \
	(tar cf - $(DIST) $(DISKS) | (cd $(NAME); tar -xf -)) && \
	tar cf - $(NAME) | gzip >$(NAME).tar.gz

# include dependancy files if they exist
-include $(OBJS:.o=.d)

# compile and generate dependency info
%.o: %.c
	$(CC) -c $(CFLAGS) $*.c -o $*.o
	@$(CC) -MM $(CFLAGS) $*.c > $*.d
# Improve dependency file produced by gcc... allows files to be renamed and source files to exist
# in subdirectories.
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp
