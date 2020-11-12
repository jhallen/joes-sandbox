/* Convert Intel hex (.mcs) files to binary */

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
        len = hextobin(buf + 1) + 4; // Add four bytes for byte count, address and type
        for (x = 0; x != len; ++x) {
                tot += hextobin(buf + 1 + x * 2);
        }
        cksum = hextobin(buf + 1 + x * 2);
        tot = (255 & -tot);
        if (tot != cksum) {
                fprintf(stderr, "Checksum mismatch on line %ld (got %x but calculated %x)\n", count, cksum, tot);
                return 1;
        } else {
                return 0;
        }
}

int mcsread(char *name)
{
        FILE *f;
	char buf[520];
	unsigned long count = 0;
	unsigned long line = 0;
	unsigned long seg = 0;
	unsigned long big = 0;
	int fail = 0;
	int gots9 = 0;
	printf("Loading %s...\n", name);
	f = fopen(name, "r");
	if (!f) {
		fprintf(stderr, "Couldn't open input file: %s\n", name);
		exit(1);
	}
	while (fgets(buf, sizeof(buf), f))
		if (buf[0] == ':') {
		        unsigned long type = hextobin(buf + 1 + 6);
		        unsigned long addr = (hextobin(buf + 1 + 2) << 8) + hextobin(buf + 1 + 4);
		        unsigned long len = hextobin(buf + 1 + 0);
		        ++line;
		        fail |= cksum(buf, line);
			switch (type) {
                                case 0: { // Data
                                        unsigned long x;
                                        for (x = 0; x != len; ++x) {
                                                unsigned long d = hextobin(buf + 1 + 8 + 2 * x);
                                                set(addr + seg + big + x, d);
                                        }
                                        break;
                                } case 1: { // End of file
                                        gots9 = 1;
                                        break;
                                } case 2: { // Segment
                                        seg = (((hextobin(buf + 1 + 8) << 8) + hextobin(buf + 1 + 10)) << 4);
                                        break;
                                } case 3: { // Start segment
                                        break;
                                } case 4: { // Extended
                                        big = (((hextobin(buf + 1 + 8) << 8) + hextobin(buf + 1 + 10)) << 16);
                                        break;
                                } case 5: { // Start lienar
                                        break;
                                } default: {
                                        fprintf(stderr, "%s %ld: Unknown line type\n", name, line);
                                        fail = 1;
                                        break;
                                }
                        }
                }
        if (!gots9) {
                fprintf(stderr, "%s %ld: Missing termination record\n", name, line);
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
                        printf("mcstobin [infile(s)] [[--start nnn] [--end nnn] -o outfile]...\n");
                        printf("  Convert Intel .MCS files into binary\n");
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
                        fail |= mcsread(argv[x]);
                }
	}

        if (fail) {
                fprintf(stderr, "Do not trust binary file due to previous errors\n");
        }
        return fail;
}
