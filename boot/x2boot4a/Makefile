# Device name of root (such as /dev/hda3, /dev/sda2, etc.)
EXT2ROOT = /dev/hda11

# Filename of kernel (<= 0.5MB compressed)
KERNEL = /boot/vmlinuz

# Modify the parameters to suit.
bootext2.br: bootext2.bin tune_ext2 Makefile
	./tune_ext2 < bootext2.bin  > bootext2.br \
		-wait=20 -boot="$(KERNEL) root=$(EXT2ROOT) reboot=warm mem=64M" \
	|| rm bootext2.br
# If tune_ext2 fails then don't leave bootext2.br lying around.

bootext2.bin: bootext2.a86
	as86 -b bootext2.bin -l bootext2.lst -o bootext2.obj bootext2.a86

# Test using an ext2 floppy;
# build bootext2.br with USE_LABEL = 1 , USE_FLOPPY = 1 .
# (Some BIOS refuse to boot such a floppy because
# it doesn't look like an MS-DOS system disk.
# Those BIOS are broken.)
install_test: bootext2.br
	/sbin/mkfs -t ext2 /dev/fd0
	mount /dev/fd0
	mkdir /mnt/floppy/boot
	cp $(KERNEL) /mnt/floppy/boot
	umount /dev/fd0
	dd if=bootext2.br of=/dev/fd0 bs=1024 count=1

# The real thing.  Make a boot floppy, too, just in case.
install: bootext2.br
	dd if=$(KERNEL) of=/dev/fd0; /usr/sbin/rdev /dev/fd0 $(EXT2ROOT)
	dd if=$(EXT2ROOT) of=old_br.$$$$ bs=1024 count=1
	dd if=bootext2.br of=$(EXT2ROOT) bs=1024 count=1

# UNFORTUNATELY, tune_ext2.c DEPENDS ON USE_MBR, USE_PTAB, USE_LABEL.
# EDITING IS REQUIRED!
tune_ext2: tune_ext2.c bootext2.a86
	cc -g -o tune_ext2 tune_ext2.c

FILES = \
	COPYING \
	Makefile \
	README \
	bootext2.a86 \
	bootext2.bin \
	tune_ext2.c

tar: $(FILES)
	tar -cvzf x2boot4a.tgz $(FILES)
