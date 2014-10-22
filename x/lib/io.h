/* Event driven I/O */

#ifndef _Iio
#define _Iio 1

typedef struct connection PORT;

#include "queue.h"
#include "task.h"

#include "mutex.h"

/* Output command */

struct output
 {
 QITEM(struct output);		/* Doubly-linked list of output commands */
 char *buf;			/* Data */
 int len;			/* Length */
 TASK *write;			/* TASK to call after data has been sent */
 };

/* A connected port */

struct connection
 {
 QITEM(struct connection);
 int fd;			/* fd of socket attached to this port */
 int count;			/* Count of people who have this connection */

 /* Input */
 int mode;			/* 0=raw mode, 1=line mode, 2=block mode, 3=
 				 * listen mode */
 int bksize;			/* Block mode block size */
 int iflush;			/* 1=flushing the input */
 TASK *read;			/* TASK to call for read data */
 char *buf;			/* Input buffer */
 int siz;				/* Buffer siz */
 int new;				/* Write point */
 int old;				/* Read point */
 int taken;			/* Amount of data passed to previous read */
 thread_t reader;		/* Thread selecting on read */
 int has_reader;		/* Set if there is a reader */

 /* Output */
 char *obuf;			/* Output buffer */
 int osiz;				/* Size */
 int onew;				/* Write point */
 int oold;				/* Read point */
 struct output out[1];		/* Output queue */
 thread_t writer;		/* Thread selecting on write */
 int has_writer;		/* Set if there is a writer */
 };

/* int io(void);
 * Handle I/O.  Select()s and Read()s available data.  Enques tasks to
 * handle the data.
 */
int io();

/* PORT *portopen(int fd);
 * Create a PORT for the file descriptor.  close is called when the port
 * becomes close due to EPIPE.
 */
PORT *portopen();

/* PORT *portlisten(int listen_port);
 * Listen for and accept connections on the specified TCP/IP port no.
 * When a new connection is accepted, a PORT is created for it and the
 * callback function is called with its address.
 */
PORT *portlisten();

/* PORT *portconnect(char *name,int port);
 * Connect to a port.  'name' contains the machine name or IP address of the
 * machine.  'port' is the numeric port no.  close is called when the port
 * closes.
 */
PORT *portconnect();

/* void stportread(PORT *,TASK *read,int len);
 *
 * Set read callback function for a port.  When data is received, further
 * reads are disabled and the callback function is called with the the
 * address of the data buffer and the size of the data.  portack() must be
 * called to allow the next datum to be read and the resulting next
 * invokation of the read callback function to be made.
 *
 * As stated above, portack() must be called to allow the next datum to be
 * read.  When and where to call portack() depends on the details of your
 * protocol.  Generally: if a particular read callback function does not
 * cause a write to any port, portack() should be called during that read
 * callback function.  If a particular read callback function does actually
 * write to any port, then portack() should be called only after all of the
 * writes have been completed- I.E., during the final write callback
 * function for those writes.  This will avoid the infinite memory
 * allocation which would otherwise occur when your data sources are faster
 * than your data sinks.
 *
 * It is safe to call other port communications functions while in a read
 * callback function.
 *
 * Note that the buffer pointer passed to the callback function points to an
 * internal buffer.  The buffer is guarenteed to retain its contents until
 * portack() is called.  Memory allocation is automatically handled,
 * therefore the buffer pointer should never be passed to free().
 *
 * If no read callback function is set, any received data will be discarded.
 * No data will be missed as long as stportread() is called either during the
 * new client callback function (see portlisten()) or before the next
 * invokation of or return to serv() if the PORT is created with portopen() or
 * portconnect().
 *
 * The call to stportread itself will not execute any callback functions.
 */
void stportread();

/* void stlinemode(PORT *);
 * Set line mode for reading on a port.  This is the default setting for new
 * PORTs.
 *
 * When the port is in line mode, the read callback function is called with
 * each complete '\n' terminated line received from the port.  The '\n' is
 * replaced with NUL before the callback function is executed.  The size of
 * the line passed to the read callback function will not include the
 * terminating NUL (but it is in the buffer, and can be read- the buffer
 * pointer can be considered a zero-terminated string pointer).
 *
 * Received lines could be of any length.  The buffer is automatically
 * expanded to handle longer lines.
 *
 * The call to stlinemode itself will not execute any callback functions.
 */
void stlinemode();

/* void strawmode(PORT *);
 * Set raw mode for reading on a port.
 *
 * When the port is in raw mode, the read callback function is called
 * whenever data is received.  The callback function is called with however
 * much data was actually received.  The callback function should call
 * portack() with the amount of the data which was actually taken from the
 * buffer.  If the amount taken is less than the amount passed to the
 * callback function, the next read callback function will get the balance
 * of the data.  If stlinemode() or stblockmode() is called before the next
 * return to the server after portack(), the balance of the data will be the
 * start of the next line or block.
 *
 * strawmode itself will not execute any callback functions.
 */
void strawmode();

/* void stblockmode(PORT *,int size);
 * Set block mode with specified block size.
 *
 * When the port is in block mode, the read callback function is called
 * only when an entire block of the specified size is received.  The
 * read callback function may call stblockmode to set the block size for
 * the next block.
 *
 * stblockmode itself will not execute any callback functions.
 */
void stblockmode();

/* void stnomode(PORT *);
 *
 * Set no mode.  The read callback function is called when there is pending
 * input, but it must read the data itself.
 */
void stnomode();

/* Amount of data passed to last read callback function */
#define portlen(p) ((p)->taken)

/* int portwrite(PORT *,char *buf,int len,TASK *fn);
 *
 * Write data to port's output buffer (if it's full, portwrite() attempts to
 * empty it by calling write()).  If we were successful, return 1. 
 * Otherwise return 0: the data is added to the output queue and the
 * callback function will be executed after the port has accepted the data to
 * at least its output buffer.
 *
 * portwrite itself will not execute any callback functions.
 */
int portwrite();

/* int finalwrite(PORT *,char *buf,int len,TASK *fn);
 *
 * Write any data ahead of us to the port (I.E., with an actual write()
 * call), then write the specified data to the port.  If we were successful,
 * return 1.  Otherwise return 0: the data is added to the output queue and
 * the callback function will be executed after the data has actually been
 * written with write().
 *
 * finalwrite iteself will not execute any callback functions.
 */
void finalwrite();

/* void portclose(PORT *);
 * Close a port.  All fns associated with the port will be fnzapped.
 *
 * The call to portclose() itself will not execute any callback functions.
 */
void portclose();

/* void portfree(PORT *);
 * Decrement the reference count on a PORT.  If it reached zero, free
 * the PORT.  The initial reference count after portopen() is 1.
 */
void portfree();

/* PORT *portdup(PORT *);
 * Up the reference count on the port.  The initial reference count after
 * portopen() is 1.
 */
PORT *portdup();

#endif
