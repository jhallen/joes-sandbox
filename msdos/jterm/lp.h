/* Printer interface */

/* Return true if printer can accept next character */

#define pcheck(port) (0x80&inportb((port)+1))

/* Print a character */
/* Printer must already be verfied as ready with pcheck() */

void pout(int port,unsigned char c);

/* Ports */

#define LP0 0x378
#define LP1 0x3BC
#define LP2 0x278
