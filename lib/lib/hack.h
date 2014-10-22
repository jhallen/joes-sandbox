/* Quick branch-free instructions */

/* Reverse bits in a word */
unsigned rev(unsigned x);

/* Bit shuffling (interleaving) */
unsigned outer_shuffle(unsigned x);
unsigned outer_unshuffle(unsigned x);
unsigned inner_shuffle(unsigned x);
unsigned inner_unshuffle(unsigned x);

/* Compute parity of a word */
unsigned bin_to_gray(unsigned x);
unsigned gray_to_bin(unsigned x);
int parity(unsigned x);

/* Compress bits from val with corresponding set bits in sel to the right */
unsigned gather(unsigned val, unsigned sel);

/* Count no. 1 bits in x */
int pop(unsigned x);

/* Count no. trailing zeros */
int ntz(unsigned x);

/* Count no. leading zeros */
int nlz(unsigned x);

/* Greatest power of 2 less than or equal to x */
unsigned gp2(unsigned x);

/* Least power of 2 greater than or equal to x */
unsigned lp2(unsigned x);

/* True if number is an exact power of 2 */
#define isp2(x) !((x) & ((x) - 1))

/* Clear least significant one bit */
#define clear_lsob(x) ((x) & ((x) - 1))

/* Isolate least significant one bit */
#define find_lsob(x) ((x) & -(x))

/* Form mask from least significant zero bits */
#define mask_lszb(x) (~(x) & ((x) - 1))

/* Form mask from least significant zero bits including least significant one bit */
#define mask_lsozb(x) ((x) ^ ((x) - 1))

/* Right-propogate least significant 1 bit */
#define prop_lsob(x) ((x) | ((x) - 1))

/* Clear rightmost contiguous string of 1 bits */
#define clear_ones(x) ((prop_lsob(x) + 1) & (x))

/* Set least significant zero bit */
#define set_lszb(x) ((x) | ((x) + 1))

/* Find least significant zero bit */
#define find_lszb(x) (~(x) & ((x) + 1))

/* Absolute value */
#define quick_abs(x) (((x) ^ ((x) >> (sizeof(int) - 1))) - ((x) >> (sizeof(int) - 1)))

/* Negative absolute value (can't overflow) */
#define quick_nabs(x) (((x) >> (sizeof(int) - 1)) - ((x) ^ ((x) >> (sizeof(int) - 1))))

/* 1 if >0, 0 if 0, -1 if <0 */
#define sign(x) (((x) >> (sizeof(int) - 1)) | ((unsigned)(-(x)) >> (sizeof(int) - 1)))

/* Decode zero means 2^n (if value is 0, replace it with 2^n) */
#define zdec(x,n) ((((x) - 1) & ((1 << (n)) - 1)) + 1)

unsigned qmin(unsigned x, unsigned y);

unsigned qmax(unsigned x, unsigned y);
