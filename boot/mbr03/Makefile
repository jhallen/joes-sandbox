# Device filenames; modify to match your configuration.
HARD = /dev/hda
FLOP = /dev/fd0

# Suggested procedure:
#
#	"make install_flop" then boot the floppy.
#	If it doesn't boot, then please send a bug report
#	(and do not proceed any farther).
#
#	Modify the command-line parameters to the invocation of
#	tune_mbr under target 'install_test' to match your
#	partition layout.  "make install_test" then boot the floppy.
#	Some BIOS will refuse to boot this floppy because it doesn't
#	have a FAT volume label, but if your BIOS boots this floppy
#	then you have some assurance that the final step (below)
#	will work, and you have not yet modified the Master Boot Record
#	(MBR) on the harddisk.
#
#	Make a boot/rescue floppy.  Once recipe for this is
#		dd if=/boot/vmlinuz of=/dev/fd0
#		ROOT=` mount  |  grep ' / '  |  sed 's/ .*//' `
#		/usr/sbin/rdev /dev/fd0 $ROOT
#	If you use LILO, then [also] make a LILO boot floppy:
#	Set "boot=/dev/fd0" in /etc/lilo.conf, then run /sbin/lilo.
#	Test boot the boot/rescue floppy (and the LILO boot floppy)
#	to be sure that it works.  Do not proceed to the final step
#	until you can boot from the floppy.
#
#	Check the command-line parameters to tune_mbr again.
#	Finally, "make install_hard" to save the old MBR and write the new MBR.
#

# Encourage testing by placing this target first in Makefile.
install_flop: flp2hard.mbr
	dd if=flp2hard.mbr of=$(FLOP) bs=512 count=1

# flp2hard.mbr should be bootable from any BIOS.
# Use as a test, and as a machine-independent "universal" harddrive booter.
# flp2hard.mbr lists type, size, and letter; but no user label strings.
# flp2hard.mbr uses hardwired values for timeout and defaults.
flp2hard.mbr: mbr.a86 flop_on.a86
	cat flop_on.a86 mbr.a86  > flp2hard.a86
	as86 -l flp2hard.lst -b flp2hard.bin -o flp2hard.obj flp2hard.a86
	rm flp2hard.a86
	dd if=flp2hard.bin of=flp2hard.mbr bs=1 skip=5 count=512

install_test: testhard.mbr
	dd if=testhard.mbr of=$(FLOP) bs=512 count=1

# testhard.mbr is the customized, candidate boot block.
# Some "plug and play" BIOSes reject it if booted from a floppy,
# because testhard.mbr does not look like the superblock of a FAT filesystem.
# Modify the command-line arguments to tune_mbr to suit your configuration.
testhard.mbr: testhard.bin tune_mbr Makefile
	./tune_mbr $(HARD)  < testhard.bin  > testhard.mbr \
	    -wait=20 -char0=a -default=b \
	    a=dos b=IB c=NT d= e= f=os2 g= h= i= j=RH4 k=u4 l= m=RH5 n=u5 \
	|| rm testhard.mbr
# If tune_mbr fails then don't leave testhard.mbr lying around.

testhard.bin: mbr.a86 flop_off.a86
	rm -f testhard.a86
	cat flop_off.a86 mbr.a86  > testhard.a86
	as86 -l testhard.lst -b testhard.bin -o testhard.obj testhard.a86
	rm -f testhard.a86

# _AFTER_ successful testing using a floppy,
# then install on the harddrive.
# The first 'dd' creates a backup of the existing mbr.
install_hard: testhard.mbr
	dd if=$(HARD)      of=old_mbr.$$$$ bs=512 count=1
	dd if=testhard.mbr of=$(HARD)      bs=512 count=1

tune_mbr: tune_mbr.c testhard.bin
	cc -g -o tune_mbr tune_mbr.c

files = \
	COPYING  \
	Makefile  \
	README  \
	flop_on.a86  \
	flop_off.a86  \
	flp2hard.mbr \
	mbr.a86  \
	testhard.bin \
	tune_mbr.c

tar: $(files)
	touch flop_off.a86 flop_on.a86	# Encourage use of tune_mbr
	tar -cvzf mbr03.tgz $(files)
