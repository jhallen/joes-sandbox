/* Copyright 1998 John F. Reiser.  All rights reserved.
   This file is part of MBR (Master Boot Record)

   MBR is free software; you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software 
   Foundation; either version 2, or (at your option) any later version.  

   MBR is distributed in the hope that it will be useful, but WITHOUT ANY 
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
   FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
   details.  

   You should have received a copy of the GNU General Public License along with 
   MBR; see the file COPYING.  If not, write to the Free Software Foundation, 
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct part_blk {
	unsigned char random[446];
	unsigned char part[4][16];
	unsigned short magic;
};

static void
uread(int fd, void *buf, int len, char const *msg)
{
	int const bytes = read(fd, buf, len);
	if (bytes!=len) {
		fprintf(stderr,"%s: read only %d of %d bytes\n",
			msg, bytes, len);
		exit(1);
	}
}

static void
uwrite(int fd, void *buf, int len, char const *msg)
{
	int const bytes = write(fd, buf, len);
	if (bytes!=len) {
		fprintf(stderr,"%s: wrote only %d of %d bytes\n",
			msg, bytes, len);
		exit(1);
	}
}

static void
chk_magic(struct part_blk *blk, char const *msg)
{
	if (0xaa55!=blk->magic) {
		fprintf(stderr,"%s ends with %#x, should be %#x.\n",
			msg, blk->magic, 0xaa55);
		exit(1);
	}
}

static void
get_sector(int fd, struct part_blk *blk, long sector)
{
	char msg[20];
	if (0!=lseek(fd, sector<<9, SEEK_SET)) {
		perror("device:");
		exit(1);
	}
	sprintf(msg,"sector %d",sector);
	uread(fd, blk, sizeof(*blk), msg);
	chk_magic(blk, msg);
}

static void usage()
{
	fprintf(stderr,"tune_mbr device <.bin >.mbr options... labels...\n"
	"	-char0=c0\n"
	"	-default=c\n"
	"	-force=c\n"
	"	-wait=n\n"
	"	c0=label0 ...\n");
}

struct part_blk dev_mbr;
struct part_blk new_mbr;

void
main(int argc, char **argv)
{
	unsigned long countdown = 0;
	char char0 = 'a';
	char defltr = 0;
	char force = 0;

	char const *dev_name;
	int fd_dev;

	if (argc < 2) {
		usage();
		exit(1);
	}
	dev_name = argv[1];
	fd_dev = open(dev_name, 0);
	if (fd_dev < 0) {
		perror(dev_name);
		exit(1);
	}
	get_sector(fd_dev, &dev_mbr, 0);
	uread(0, &new_mbr, 5, ".bin hdr");  /* discard header */
	uread(0, &new_mbr, sizeof(new_mbr), "new mbr");  /* prototype mbr */
	chk_magic(&new_mbr, "new mbr");

	for (	(argv+=2), (argc-=2);
		argc > 0;
		(++argv), (--argc)
	) if ('-'==**argv) {
		char *value = strchr(*argv, '=');
		if (0!=value) {
			*value++ = 0;
		}
		if (0==strcmp("-wait", *argv)) {
			if (0!=value) {
				countdown = atoi(value);
				if (0 < countdown && countdown < 4000) {
					countdown *= 1000000;
				}
			}
		}
		else if (0==strcmp("-char0", *argv)) {
			if (0!=value) {
				char0 = *value;
			}
		}
		else if (0==strcmp("-default", *argv)) {
			if (0!=value) {
				defltr = *value;
			}
		}
		else if (0==strcmp("-force", *argv)) {
			if (0!=value) {
				force = *value;
			}
		}
		else {
			fprintf(stderr,"ignoring option %s\n", *argv);
		}
	}
	else {
		break;
	}

	/* Somebody please tell me how to read an .obj symbol table
	   so I can do these magic offsets symbolically!
	*/
#define COUNT  0x177
#define DEFLTR 0x165
#define DESLTR 0x03c
#define NAMES  0x18d
	memcpy(&new_mbr.random[COUNT -4], &countdown, sizeof(countdown));
	memcpy(&new_mbr.random[DEFLTR -1], &defltr, sizeof(defltr));
	memcpy(&new_mbr.random[DESLTR -2], &char0, sizeof(char0));
	memcpy(&new_mbr.random[DESLTR -1], &force, sizeof(force));
	if (0==countdown) {/* wait forever: no count at all */
		new_mbr.random[COUNT -6] = 0xeb;  /* JMP rel8 */
		new_mbr.random[COUNT -5] = 0x0d;  /* skip to getcWait */
	}

    if (0 < argc) {
	char *labels = &new_mbr.random[NAMES];
	int avail = (0x200 - 2 - 4*16 - 6) - NAMES;
		qsort(argv, argc, sizeof(*argv), (comparison_fn_t)strcmp);
	for (	;
		0!=argc;
		++argv, --argc
	) {
		int const len = 1+strlen(2+*argv);
		if ((avail -= len) < 0) {
			fprintf(stderr,"Use shorter labels; "
				"`%c=%s' won't fit.\n",
				**argv, 2+*argv);
		}
		else {
			strcpy(labels, 2+*argv);
			labels += len;
		}
		if (1 < argc  /* not last */
		&& (1 + *argv[0]) != *argv[1] ) {
			fprintf(stderr, "letters must be consecutive: %s %s\n",
				argv[0], argv[1] );
			exit(1);
		}
	}
    }

	/* WindowsNT disk administrator uses these 6 bytes */
	memcpy(&new_mbr.random[446-6], &dev_mbr.random[446-6], 6);

	memcpy(&new_mbr.part[0], &dev_mbr.part[0], 4*16);
	uwrite(1, &new_mbr, sizeof(new_mbr), "stdout");
	exit(0);
}
