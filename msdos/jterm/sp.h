/* Interrupt driven serial port */

/* Set the baud rate of a port */
void spbaud(int port,long baud);

/* Open with port address, interrupt vector number and interrupt mask */
void spopen(int port,int vect,int mask);

/* Close serial port */
void spclose(void);

/* Send and receive character */
void spputc(unsigned char ch);
unsigned char spgetc(void);

/* Return true if we can get without blocking */
int spcangetc(void);

/* Return true if we can send without blocking */
int spcanputc(void);

/* Return true if we can close without blocking */
int spcanclose(void);

/* Send a break */
void spbreak(void);

