## x86 Boot Loaders

Here are boot loaders for Linux on x86, including replacement Master Boot
Record and single block linux loaders which understand the EXT2 filesystem.

John F. Reiser (who ported UNIX to VAX in 1978!) extended these to use
larger disks.

[mbr - Original MBR in MASM](https://github.com/jhallen/joes-sandbox/tree/master/boot/mbr)

[bootext2 - Original EXT2 loader in MASM](https://github.com/jhallen/joes-sandbox/tree/master/boot/bootext2)

[bootfat - Original FAT loader in MASM](https://github.com/jhallen/joes-sandbox/tree/master/boot/bootfat)

[mbr03 - John Reiser's updated version of MBR in a86](https://github.com/jhallen/joes-sandbox/tree/master/boot/mbr03)

[e2boot4c - John Reiser's updated version of bootext2 in a86](https://github.com/jhallen/joes-sandbox/tree/master/boot/e2boot4c)
