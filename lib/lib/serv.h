/* TCP/IP to Daemon interface (I/O multiplexer) */

#ifndef _Iserv
#define _Iserv 1

typedef struct connection PORT;

/* void serv();
 * Start the server.  This function never returns.
 */
void serv();

/* PORT *portopen(int fd);
 * Associate a file-descriptor with a port
 */
PORT *portopen();

/* PORT *portlisten(int listen_port,
 *                  void (*func)(void *arg,PORT *new),void *arg);
 * Listen for connections on the specified port no.
 * When a new connection is accepted, a port is created for it and the
 * callback function is called with its address.
 */
PORT *portlisten();

/* PORT *portconnect(char *name,int port);
 * Connect to a port.
 */
PORT *portconnect();

/* void stportread(PORT *,int (*func)(void *arg,char *buf,int len),void *arg);
 * Set read callback function for a port.  When data is received, the call-
 * back function is called with argument, data and length.  It is safe to
 * call other port communications functions while in a read callback
 * function.
 *
 * Note that the buffer pointer passed to the callback function points to
 * an internal buffer.  The buffer is guarenteed to retain its contents until
 * the return to the event loop.  Memory allocation is automatically handled,
 * therefore the buffer pointer should never be passed to free().
 *
 * The return value of the callback function is ignored, except in raw mode
 * (see strawmode() below).
 *
 * If no read callback function is set, any received data will be discarded.
 * No data will be missed as long as stportread is called before the newclient
 * callback function returns (see serv() above).
 *
 * stportread itself will not execute any callback functions.
 */
void stportread();

/* void stlinemode(PORT *);
 * Set line mode for reading on a port.  This is the default initial setting
 * when a connection is first established.
 *
 * When the port is in line mode, the read callback function is called with
 * each complete '\n' terminated line received from the port.  The '\n' is
 * replaced with NUL before the callback function is executed.  The size of
 * the line passed to the read callback function will not include the
 * terminating NUL (but it is in the buffer, and can be read- the buffer
 * pointer can be considered a zero-terminated string pointer).
 *
 * Received lines could be any length.  The buffer is automatically expanded
 * to handle longer lines.
 *
 * stlinemode itself will not execute any callback functions.
 */
void stlinemode();

/* void strawmode(PORT *);
 * Set raw mode for reading on a port.
 *
 * When the port is in raw mode, the read callback function is called
 * whenever data is received.  The callback function is called with however
 * much data was actually received.  The callback function should return the
 * amount of the data which was actually taken.  If the amount returned is
 * less than the amount passed to the callback function, the next read
 * callback function will get the balance of the data.  If stlinemode is
 * called before the callback function returns, the balance of the data
 * will be the start of the next line.
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

/* void portiflush(PORT *);
 * Flush (discard) pending input data.
 *
 * portiflush itself will not execute any callback functions.  The actual
 * flushing process occurs during the next pass through the event loop.  The
 * flushing process may detect that the port closed, in which case the close
 * callback function will be executed.
 */
void portiflush();

/* void portwrite(PORT *,char *buf,int len,int (*func)(void *arg),void *arg);
 * Write data to a port.  The callback function is executed after
 * this data has been written.  It is safe to call other port
 * communications functions while in a portwrite() callback function.
 *
 * Note that the data is not coppied by this function.  Port write simply
 * notes the pointer to the output buffer which you provide.  Each buffer
 * must be maintained until the data has been written, the output is
 * flushed, or the port closes.
 *
 * portwrite itself will not execute any callback functions.
 */
void portwrite();

/* void portoflush(PORT *);
 * Flush (discard) pending output data
 *
 * portoflush itself will not execute any callback functions.
 */
void portoflush();

/* void stportclose(PORT *,int (*func)(void *arg),void *arg);
 * Set close callback function for a port.  When the port closes (due to the
 * the connection being lost), the callback function is called.  It is safe
 * to call other port communications functions while in a close callback
 * function.
 *
 * stportclose itself will not execute any callback functions.
 */
void stportclose();

/* void portclose(PORT *);
 * Close a port.  The close callback function does not get called when
 * the port is closed with this function.
 *
 * portclose itself will not execute any callback functions.
 */
void portclose();

#endif
