/* Generate CRC32 table a bit at a time */
/* Big endian, MSB first version */

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

unsigned int crcgen(unsigned char b)
{
	unsigned char m;
	crc = 0;
	for (m = 0x80; m; m >>= 1)
		docrc(b & m);
	return crc;
}

int main(void) 
{
	unsigned int x, y;
	for (y = 0; y < 64; y++) {
	        printf("	");
		for (x = 0; x < 3; x++) {
			printf("0x%8.8x, ", crcgen(x + y * 4));
                }
		printf("0x%8.8x\n", crcgen(x + y * 4));
        }
	return 0;
}
