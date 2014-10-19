#include <stdio.h>

/* Decode stupid mime base 64 document */

/* Base 64 encoding.  Upper 6 bits of first byte are sent out first.
  0-25: ABCDEFGHIJKLMNOPQRSTUVWXYZ
 26-51: abcdefghijklmnopqrstuvwxyz
 52-61: 0-9
    62: +
    63: /
*/

int main()
{
	int accu = 0;
	int bits = 0;
	int c;
	while ((c = getchar()) != -1) {
		if (c >= 'A' && c <= 'Z') {
			accu <<= 6;
			bits += 6;
			accu |= c - 'A';
		} else if (c >= 'a' && c <= 'z') {
			accu <<= 6;
			bits += 6;
			accu |= c - 'a' + 26;
		} else if (c >= '0' && c <= '9') {
			accu <<= 6;
			bits += 6;
			accu |= c - '0' + 52;
		} else if (c == '+') {
			accu <<= 6;
			bits += 6;
			accu |= 62;
		} else if (c == '/') {
			accu <<= 6;
			bits += 6;
			accu |= 63;
		}
		if (bits >= 8) {
			putchar(accu >> (bits - 8));
			bits -= 8;
		}
	}
	return 0;
}
