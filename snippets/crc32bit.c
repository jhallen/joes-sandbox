/* Calculate CRC32 a bit at a time */
/* Big endian, MSB first version */

/* WHen you append the calculated CRC to a file, the CRC of the result will be 0 */

#include <stdio.h>

unsigned int crc;

void docrc(int bit)
{
    int b = !!(crc & 0x80000000) ^ !!bit;
    crc <<= 1;
    if (b) {
        crc ^= 0x04c11db7;
    }
}

unsigned int crcb(unsigned char b)
{
	unsigned char m;
	for (m = 0x80; m; m >>= 1)
		docrc(b & m);
	return crc;
}

int main(int argc, char *argv[])
{
	FILE *f = fopen(argv[1], "r");
	if (f) {
		int c;
		while ((c = fgetc(f)) != -1) {
			crcb(c);
		}
		fclose(f);
		printf("CRC is %8.8x\n", crc);
	}
}
