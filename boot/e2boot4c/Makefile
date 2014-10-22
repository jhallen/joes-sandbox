# Device name of root (such as /dev/hda3, /dev/sda2, etc.)
EXT2ROOT = /dev/hda14

# Filename of kernel
KERNEL = /boot/vmlinuz

# Modify the parameters to suit.
e2boot.br: e2boot.bin tune_e2boot Makefile
	./tune_e2boot < e2boot.bin  > e2boot.br \
		-wait=20 -boot="$(KERNEL)|initrd root=$(EXT2ROOT) reboot=warm" \
	|| rm e2boot.br
# If tune_e2boot fails then don't leave e2boot.br lying around.

e2boot.bin: e2boot.a86
	as86 -b e2boot.bin -l e2boot.lst -o e2boot.obj e2boot.a86

# The real thing.  Make a boot floppy, too, just in case.
# SEE THE "Bootdisk HOWTO" IF YOU REQUIRE AN INITIAL RAM DISK JUST TO BOOT.
install: e2boot.br
	dd if=/$(KERNEL) of=/dev/fd0; /usr/sbin/rdev /dev/fd0 $(EXT2ROOT)
	dd if=$(EXT2ROOT) of=old_br.$$$$ bs=1024 count=1
	dd if=e2boot.br of=$(EXT2ROOT) bs=1024 count=1

# UNFORTUNATELY, tune_e2boot.c DEPENDS ON USE_MBR, USE_PTAB, USE_LABEL.
# EDITING IS REQUIRED!
tune_e2boot: tune_e2boot.c e2boot.a86
	cc -g -o tune_e2boot tune_e2boot.c

FILES = \
	COPYING \
	Makefile \
	README \
	e2boot.a86 \
	e2boot.bin \
	tune_e2boot.c

tar: $(FILES)
	tar -cvzf e2boot4c.tgz $(FILES)
