/* Cool little branch-free bit-manipulatoin functions */

#include "hack.h"

/* Reverse bits */

unsigned rev(unsigned x)
{
	x = ((x & 0x55555555) << 1) | ((x >> 1) & 0x55555555);
	x = ((x & 0x33333333) << 2) | ((x >> 2) & 0x33333333);
	x = ((x & 0x0F0F0F0F) << 4) | ((x >> 4) & 0x0F0F0F0F);
	return (x << 24) | ((x & 0xFF00) << 8) | ((x >> 8) & 0xFF00) | (x >> 24);
}

/* Outer perfect shuffle: abcdABCD -> aAbBcCdD */

unsigned outer_shuffle(unsigned x)
{
	x = ((x & 0x0000FF00) << 8) | (x >> 8) & 0x0000FF00 | x & 0xFF0000FF;
	x = ((x & 0x00F000F0) << 4) | (x >> 4) & 0x00F000F0 | x & 0xF00FF00F;
	x = ((x & 0x0C0C0C0C) << 2) | (x >> 2) & 0x0C0C0C0C | x & 0xC3C3C3C3;
	x = ((x & 0x22222222) << 1) | (x >> 1) & 0x22222222 | x & 0x99999999;
	return x;
}

/* Output perfect unshuffle aAbBcCdD -> abcdABCD */

unsigned outer_unshuffle(unsigned x)
{
	x = ((x & 0x22222222) << 1) | (x >> 1) & 0x22222222 | x & 0x99999999;
	x = ((x & 0x0C0C0C0C) << 2) | (x >> 2) & 0x0C0C0C0C | x & 0xC3C3C3C3;
	x = ((x & 0x00F000F0) << 4) | (x >> 4) & 0x00F000F0 | x & 0xF00FF00F;
	x = ((x & 0x0000FF00) << 8) | (x >> 8) & 0x0000FF00 | x & 0xFF0000FF;
	return x;
}

/* Inner perfect shuffle: abcdABCD -> AaBbCcDd */

unsigned inner_shuffle(unsigned x)
{
	x = (x >> 16) | (x << 16);
	x = ((x & 0x0000FF00) << 8) | (x >> 8) & 0x0000FF00 | x & 0xFF0000FF;
	x = ((x & 0x00F000F0) << 4) | (x >> 4) & 0x00F000F0 | x & 0xF00FF00F;
	x = ((x & 0x0C0C0C0C) << 2) | (x >> 2) & 0x0C0C0C0C | x & 0xC3C3C3C3;
	x = ((x & 0x22222222) << 1) | (x >> 1) & 0x22222222 | x & 0x99999999;
	return x;
}

/* Inner perfect unshuffle AaBbCcDd -> abcdABCD */

unsigned inner_unshuffle(unsigned x)
{
	x = ((x & 0x22222222) << 1) | (x >> 1) & 0x22222222 | x & 0x99999999;
	x = ((x & 0x0C0C0C0C) << 2) | (x >> 2) & 0x0C0C0C0C | x & 0xC3C3C3C3;
	x = ((x & 0x00F000F0) << 4) | (x >> 4) & 0x00F000F0 | x & 0xF00FF00F;
	x = ((x & 0x0000FF00) << 8) | (x >> 8) & 0x0000FF00 | x & 0xFF0000FF;
	x = (x >> 16) | (x << 16);
	return x;
}

/* Convert binary to gray */

unsigned bin_to_gray(unsigned x)
{
	return x ^ (x >> 1);
}

/* Convert gray to binary */

unsigned gray_to_bin(unsigned x)
{
	x ^= (x >> 1);
	x ^= (x >> 2);
	x ^= (x >> 4);
	x ^= (x >> 8);
	x ^= (x >> 16);
	return x;
}

/* Compute parity (1 if no. 1s is odd) */

int parity(unsigned x)
{
	/* gray_to_bin(x) */
	x ^= (x >> 1);
	x ^= (x >> 2);
	x ^= (x >> 4);
	x ^= (x >> 8);
	x ^= (x >> 16);
	/* LSB */
	return (x & 1);
}

/* sel is a bit mask which chooses which bits of val we want.
 * The selected bits are compressed together in the right most N
 * bits of the result.
 */

unsigned gather(unsigned val,unsigned sel)
{
	unsigned long mk, mp, mv, t;
	int i;
	val &= sel;
	mk = ~sel << 1;
	for (i = 0; i != 5; ++i) {
		mp = mk ^ (mk << 1);
		mp = mp ^ (mp << 2);
		mp = mp ^ (mp << 4);
		mp = mp ^ (mp << 8);
		mp = mp ^ (mp << 16);
		mv = mp & sel;
		sel = sel ^ mv | (mv >> (1 << i));
		t = val & mv;
		val = val ^ t | (t >> (1 << i));
		mk = mk & ~mp;
	}
	return val;
}

unsigned scatter(unsigned val,unsigned sel)
{
	unsigned long mk, mp, mv, t;
	int i;
	unsigned array[5];
	mk = ~sel << 1;
	for (i = 0; i != 5; ++i) {
		mp = mk ^ (mk << 1);
		mp = mp ^ (mp << 2);
		mp = mp ^ (mp << 4);
		mp = mp ^ (mp << 8);
		mp = mp ^ (mp << 16);
		mv = mp & sel;
		array[i] = mv;
		sel = sel ^ mv | (mv >> (1 << i));
		mk = mk & ~mp;
	}

	for (i = 4; i >= 0; --i) {
		unsigned m = (array[i] >> (1 << i));
		val = (val & ~m) | ((val & m) << (1 << i));
	}

	return val;
}

/* Scatter - this is not good */

unsigned scatter_slow(unsigned val,unsigned sel)
{
	unsigned long q = 0;
	unsigned long b = 1;
	while (sel) {
		unsigned long pick = find_lsob(sel);
		sel &= ~pick;
		if (b & val)
			q |= pick;
		b <<= 1;
	}
	return q;
}

/* Count no. 1s in x.
 * This works by calculating adjacent sets of 2 bits in parallel, then
 * adjacent sets of 4 bits, etc. */

int pop(unsigned x)
{
	x = x - ((x >> 1) & 0x55555555);
	x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
	x = (x + (x >> 4)) & 0x0F0F0F0F;
	x = x + (x >> 8);
	x = x + (x >> 16);
	return x & 0x3F;
}

/* Count no. trailing zeros */

int ntz(unsigned x)
{
	x = ~x & (x - 1);
	/* pop(x) */
	x = x - ((x >> 1) & 0x55555555);
	x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
	x = (x + (x >> 4)) & 0x0F0F0F0F;
	x = x + (x >> 8);
	x = x + (x >> 16);
	return x & 0x3F;
}

/* Count leading zeros */
/* log2(x) is 31-nlz(x) */

int nlz(unsigned x)
{
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	x = ~x;
	/* pop(x) */
	x = x - ((x >> 1) & 0x55555555);
	x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
	x = (x + (x >> 4)) & 0x0F0F0F0F;
	x = x + (x >> 8);
	x = x + (x >> 16);
	return x & 0x3F;
}

/* Greatest power of 2 less than or equal to x */

unsigned gp2(unsigned x)
{
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	return x - (x >> 1);
}

/* Least power of 2 greater than or equal to x */

unsigned lp2(unsigned x)
{
	--x;
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	return x + 1;
}

/*
min for x86:
	sub ax,bx
	sbb dx,dx ; all 0 or all 1's
	and ax,dx ; 0 or a-b
	add ax,bx ; min

max for x86:
	sub ax,bx
	sbb dx,dx ; 0 or all 1's
	not dx
	and ax,dx ; either 0 or a-b
	add ax,bx ; max
*/

unsigned doz(unsigned a, unsigned b)
{
	unsigned q = a - b;
	q &= ((~q) >> 16);
	return q;
}

unsigned qmax(unsigned a, unsigned b)
{
	unsigned q = a - b;
	q &= ((~q) >> 16);
	return q + b;
}

unsigned qmin(unsigned a, unsigned b)
{
	unsigned q = a - b;
	q &= ((~q) >> 16);
	return a - q;
}

/*
int main(int argc,char *argv[])
{
	printf("%d\n", doz(atoi(argv[1]),atoi(argv[2])));
	printf("min %d\n", qmin(atoi(argv[1]),atoi(argv[2])));
	printf("max %d\n", qmax(atoi(argv[1]),atoi(argv[2])));
}
*/

int only_one(unsigned long n)
{
	return n == (n & -n);
}
