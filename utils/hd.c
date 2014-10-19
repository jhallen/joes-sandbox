/* Hex dump */

#include <stdio.h>

void dump(unsigned long addr, unsigned char *buf, int n)
{
	int q;
	printf("%8lx: ", addr);
	for (q = 0; q != n; ++q)
		printf("%2x ", buf[q]);
	while (q != 16)
		printf("   "), ++q;
	for (q = 0; q != n; ++q)
		if (buf[q] >= ' ' && buf[q] <= '~')
			printf("%c", buf[q]);
		else
			printf(".");
	printf("\n");
}

int main(int argc, char *argv[])
{
	unsigned char buf[16];
	FILE *f;
	int n;
	int lastn;
	int skip;
	unsigned long addr;
	if (argc > 2) {
		fprintf(stderr, "%s [file]\n", argv[0]);
		return 1;
	} else if (argc == 2) {
		f = fopen(argv[1], "r");
		if (!f) {
			fprintf(stderr, "Couldn't open file %s\n", argv[1]);
			return 1;
		}
	} else
		f = stdin;
	addr = 0;
	skip = 0;
	lastn = 0;
	while ((n = fread(buf, 1, 16, f)) != 0) {
		int x;
		lastn = n;
		for (x = 0; x != n && !buf[x]; ++x);
		if (x != 16 || n != 16) {
			if (skip)
				printf(". . . . . .\n"), skip = 0;
			dump(addr, buf, n);
		} else
			skip = 1;
		addr += n;
	}
	if (skip && lastn) {
		printf(". . . . . .\n");
		dump(addr - lastn, buf, lastn);
	}
	return 0;
}
