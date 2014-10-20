#include <stdio.h>

unsigned short crcgen(unsigned int ch) 
{
	unsigned i, acc = 0;
	ch <<= 8;
	for (i = 8; i > 0; i--) {
		if ((ch ^ acc) & 0x8000)
			acc = (acc << 1) ^ 0x1021;
		else
			acc <<= 1;
		ch <<= 1;
        }
	return acc;
}

int main(void) 
{
	unsigned int x, y;
	for (y = 0; y < 32; y++) {
	        printf("	dw	");
		for (x = 0; x < 7; x++) {
			printf("$%4.4X, ", crcgen(x + y * 8));
                }
		printf("$%4.4X\n", crcgen(x + y * 8));
        }
	return 0;
}
