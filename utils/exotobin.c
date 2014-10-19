/* Convert Motorola S19 file into binary */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct buffer {
        struct buffer *next;
        unsigned long addr;
        unsigned char buf[4096];
} *buffers;

unsigned long prev = 0;
unsigned long last = 0;

/* Set a byte of memory to 'val' */

void set(unsigned long addr, unsigned char val)
{
        struct buffer *b;
        unsigned ofst;
        if (addr != last) {
                if (prev != last)
                        printf("  Load from %lx - %lx\n", prev, last);
                prev = addr;
        }
        last = addr + 1;
        ofst = addr % sizeof(buffers->buf);
        addr -= ofst;
        for (b = buffers; b; b = b->next) {
                if (b->addr == addr)
                        break;
        }
        if (!b) {
                b = calloc(sizeof(struct buffer), 1);
                if (!b) {
                        fprintf(stderr, "Ran out of memory\n");
                        exit(1);
                }
                b->next = buffers;
                buffers = b;
                b->addr = addr;
        }
        b->buf[ofst] = val;
}

void fin()
{
        if (prev != last) {
                printf("  Load from %lx - %lx\n", prev, last);
        }
}

/* Emit zeros */

unsigned char zeros[128];

void fzero(int len, FILE *f)
{
        int l = sizeof(zeros);
        while (len) {
                if (len < l)
                        l = len;
                fwrite(zeros, 1, l, f);
                len -= l;
        }
}

/* Emit a range of memory */

void emit(FILE *f, unsigned long start, unsigned long end)
{
        int len = sizeof(buffers->buf);
        while (start < end) {
                struct buffer *b;
                if (end - start < len)
                        len = end - start;
                for (b = buffers; b; b = b->next)
                        if (b->addr == start)
                                break;
                if (b) {
                        fwrite(b->buf, 1, len, f);
                } else {
                        fzero(len, f);
                }
                start += len;
        }
}

/* Convert ASCII hex to binary
 * Note that range of output is 0 - 255 no matter what junk might be on the input */

unsigned long hextobin(char *ptr)
{
	unsigned char c = 0;
	if (*ptr >= '0' && *ptr <= '9')
		c = 16 * (*ptr & 0xf);
	
	else
		c = 16 * ((*ptr & 0xf) + 9);
	++ptr;
	if (*ptr >= '0' && *ptr <= '9')
		c += (*ptr & 0xf);
	
	else
		c += ((*ptr & 0xf) + 9);
	return c;
}

/* Verify checksum.. */

int cksum(char *buf, unsigned long count)
{
        unsigned len;
        unsigned char tot;
        unsigned char cksum;
        unsigned x;
        tot = 0;
        len = hextobin(buf + 2);
        for (x = 0; x != len; ++x) {
                tot += hextobin(buf + 2 + x * 2);
        }
        cksum = hextobin(buf + 2 + x * 2);
        tot = (255 & ~tot);
        if (tot != cksum) {
                fprintf(stderr, "Checksum mismatch on line %ld (got %x but calculated %x)\n", count, cksum, tot);
                return 1;
        } else {
                return 0;
        }
}

int exoread(char *name)
{
        FILE *f;
	char buf[520];
	unsigned long count = 0;
	unsigned long line = 0;
	int fail = 0;
	int gots9 = 0;
	printf("Loading %s...\n", name);
	f = fopen(name, "r");
	if (!f) {
		fprintf(stderr, "Couldn't open input file: %s\n", name);
		exit(1);
	}
	while (fgets(buf, sizeof(buf), f))
		if (buf[0] == 'S') {
		        ++line;
		        fail |= cksum(buf, line);
			switch (buf[1]) {
			        case '0': { /* Header */
			                unsigned len = hextobin(buf + 2) - 3;
			                unsigned long ofst = hextobin(buf + 4) * 256UL +
			                                     hextobin(buf + 6);
                                        if (ofst != 0) {
                                                fprintf(stderr, "%s %ld: Expected address == 0 in header\n", name, line);
                                                fail = 1;
                                        }
                                        if (len < 24) {
                                                fprintf(stderr, "%s %ld: Short header\n", name, line);
                                                fail = 1;
                                        } else {
                                                unsigned x;
                                                printf("  Module name = ");
                                                for (x = 0; x != 20; ++x)
                                                        putchar(hextobin(buf + 8 + x * 2));
                                                printf("\n");
                                                printf("  Version = %d\n", (int)hextobin(buf + 48));
                                                printf("  Revision = %d\n", (int)hextobin(buf + 50));
                                                printf("  Comment = ");
                                                for (x = 0; x != len - 24; ++x)
                                                        putchar(hextobin(buf + 52 + x * 2));
                                                printf("\n");
                                        }
                                        break;
			        } case '1': { /* Data < 64K */
					unsigned len = hextobin(buf + 2) - 3;
					unsigned long ofst = hextobin(buf + 4) * 256UL +
                                                             hextobin(buf + 6);
					unsigned x;
					for (x = 0; x != len; ++x)
					        set(ofst + x, hextobin(buf + 8 + x * 2));
                                        ++count;
        				break;
                                } case '2': { /* Data < 16M */
					unsigned len = hextobin(buf + 2) - 4;
					unsigned long ofst = hextobin(buf + 4) * 65536UL + 
					                     hextobin(buf + 6) * 256UL +
					                     hextobin(buf + 8);
					unsigned x;
					for (x = 0; x != len; ++x)
					        set(ofst + x, hextobin(buf + 10 + x * 2));
                                        ++count;
        				break;
                                } case '3': { /* Data < 4G */
					unsigned len = hextobin(buf + 2) - 5;
					unsigned long ofst = hextobin(buf + 4) * 16777216UL + 
					                     hextobin(buf + 6) * 65536UL + 
					                     hextobin(buf + 8) * 256UL +
					                     hextobin(buf + 10);
					unsigned x;
					for (x = 0; x != len; ++x)
					        set(ofst + x, hextobin(buf + 12 + x * 2));
                                        ++count;
        				break;
                                } case '5': { /* Record count */
                                        unsigned len = hextobin(buf + 2);
                                        if (len != 3) {
                                                fprintf(stderr, "%s %ld: S5 record has unexpected length\n", name, line);
                                                fail = 1;
                                        } else {
                                                unsigned long cnt = hextobin(buf + 4) * 256UL +
                                                                    hextobin(buf + 6);
                                                if (cnt != count) {
                                                        fprintf(stderr, "%s %ld: Record count does not match S5 record!\n", name, line);
                                                        fail = 1;
                                                }
                                        }
                                        break;
                                } case '6': { /* Record count */
                                        unsigned len = hextobin(buf + 2);
                                        if (len != 4) {
                                                fprintf(stderr, "%s %ld: S6 record has unexpected length\n", name, line);
                                                fail = 1;
                                        } else {
                                                unsigned long cnt = hextobin(buf + 3) * 65536UL +
                                                                    hextobin(buf + 6) * 256UL +
                                                                    hextobin(buf + 8);
                                                if (cnt != count) {
                                                        fprintf(stderr, "%s %ld: Record count does not match S6 record\n", name, line);
                                                        fail = 1;
                                                }
                                        }
                                        break;
                                } case '7': {
                                        unsigned len = hextobin(buf + 2);
                                        unsigned long addr;
                                        if (len != 5) {
                                                fprintf(stderr, "%s %ld: S7 record has unexpected length\n", name, line);
                                                fail = 1;
                                        }
                                        if (gots9) {
                                                fprintf(stderr, "%s %ld: Duplicate termination record\n", name, line);
                                                fail = 1;
                                        }
                                        gots9 = 1;
                                        addr =  hextobin(buf + 4) * 16777216UL +
                                                hextobin(buf + 6) * 65536UL +
                                                hextobin(buf + 8) * 256UL +
                                                hextobin(buf + 10);
                                        printf("  Run address = %lx\n", addr);
                                        break;
                                } case '8': {
                                        unsigned len = hextobin(buf + 2);
                                        unsigned long addr;
                                        if (len != 4) {
                                                fprintf(stderr, "%s %ld: S8 record has unexpected length\n", name, line);
                                                fail = 1;
                                        }
                                        if (gots9) {
                                                fprintf(stderr, "%s %ld: Duplicate termination record\n", name, line);
                                                fail = 1;
                                        }
                                        gots9 = 1;
                                        addr = hextobin(buf + 4) * 65536UL +
                                               hextobin(buf + 6) * 256UL +
                                               hextobin(buf + 8);
                                        printf("  Run address = %lx\n", addr);
                                        break;
                                } case '9': { /* End */
                                        unsigned len = hextobin(buf + 2);
                                        unsigned long addr;
                                        if (len != 3) {
                                                fprintf(stderr, "%s %ld: S9 record has unexpected length\n", name, line);
                                                fail = 1;
                                        }
                                        if (gots9) {
                                                fprintf(stderr, "%s %ld: Duplicate termination record\n", name, line);
                                                fail = 1;
                                        }
                                        gots9 = 1;
                                        addr = hextobin(buf + 4) * 256UL +
                                               hextobin(buf + 6);
                                        printf("  Run address = %lx\n", addr);
                                        break;
                                }
                        }
                }
        if (!gots9) {
                fprintf(stderr, "%s %ld: Missing termination (S7, S8 or S9) record\n", name, line);
                fail = 1;
        }
        fclose(f);
        fin();
        return fail;
}

int main(int argc, char *argv[])
{
	int x;
	unsigned long start = 0;
	unsigned long end = 0;
	char *outfile = 0;
	int fail = 0;

	for (x = 1; argv[x]; ++x) {
	        if (!strcmp(argv[x], "--start") && argv[x + 1]) {
                        ++x;
                        start = strtol(argv[x], NULL, 0);
                } else if (!strcmp(argv[x], "--end") && argv[x + 1]) {
                        ++x;
                        end = strtol(argv[x], NULL, 0);
                } else if (!strcmp(argv[x], "--help") || !strcmp(argv[x], "-h")) {
                        printf("exotobin [infile(s)] [[--start nnn] [--end nnn] -o outfile]...\n");
                        printf("  Convert Motorola .S19 files into binary\n");
                        printf("  Files are loaded or written in the order they appear on the command line\n");
                        printf("  If --start is not specified, the default is 0\n");
                        printf("  If --end is no specified, the default is just past highest load address\n");
                        return 0;
                } else if (!strcmp(argv[x], "-o") && argv[x + 1]) {
                        ++x;
                        outfile = argv[x];
                        FILE *f = fopen(outfile, "wb");
                        if (!f) {
                                fprintf(stderr, "Couldn't open output file: %s\n", outfile);
                                exit(1);
                        }
                        printf("Saving image to %s\n", outfile);
                        printf("  Writing %lx - %lx\n", start, end ? end : last);
                        emit(f, start, end ? end : last);
                        start = 0;
                        end = 0;
                        fclose(f);
                } else if (argv[x][0] == '-') {
                        printf("Unknown option %s\n", argv[x]);
                        return -1;
                } else {
                        fail |= exoread(argv[x]);
                }
	}

        if (fail) {
                fprintf(stderr, "Do not trust binary file due to previous errors\n");
        }
        return fail;
}
