/* Reverse bits of each byte */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int c;
	while ((c = getchar()) != -1) {
		int d = (128 & (c << 7)) + (64 & (c << 5)) + (32 & (c << 3)) + (16 & (c << 1)) + (8 & (c >> 1)) + (4 & (c >> 3)) + (2 & (c >> 5)) + (1 & (c >> 7));
		putchar(d);
	}
	return 0;
}
