#ifndef _Itty
#define _Itty 1

/* void ttopen(void);  Open the tty (attached to stdin) for use inside of JOE
 *
 * (0) Call sigjoe()
 *     There is also 'void ttopnn(void)' which does not do this step.
 *
 * (1) fflush(stdout)
 *
 * (2) Save the current state of the tty
 *
 * (3) Disable CR/LF/NL input translations,
 *     Disable all output processing,
 *     Disable echo and line editing, and
 *     Place tty in character at a time mode.
 *     (basically, disable all processing except for XON/XOFF if it's set)
 *
 * (4) Set this new tty state without loosing any typeahead (by using the
 *     proper ioctl).
 *
 * (5) Store the baud rate in the global variable 'baud'
 */
void ttopen();
void ttopnn();
extern unsigned baud;

/* void ttclose(void);  Restore the tty back to its original mode.
 *
 * (1) ttflsh()
 *
 * (2) Restore the original tty mode which ttopen() had saved.  Do this without
 *     loosing any typeahead.
 *
 * (3) Call signrm().  There is also 'void ttclsn(void)' which does not do
 *     the this step.
 */
void ttclose();
void ttclsn();

/* void ttshell(char *s);  Run a shell command or if 's' is zero, run a
 * sub-shell
 */
void ttshell(char *);

/* void ttsusp(void);  Suspend the process, or if the UNIX can't do it, call
 * ttshell(NULL)
 */
void ttsusp();

/* void ttsig(int n);  Signal handler you provide.  This is called if the
 * editor gets a hangup signal, termination signal or if the input closes.
 * It is called with 'n' set to the number of the caught signal or 0 if the
 * input closed.
 */
void ttsig(int);

/* void ttgtsz(int *x,int *y);  Get size of screen from ttsize/winsize
 * structure */
void ttgtsz(int *,int *);

/* You don't have to call these: ttopen/ttclose does it for you.  These
 * may be needed to make your own shell escape sequences.
 */

/* void sigjoe(void);  Set the signal handling for joe.  I.E., ignore all
 * signals the user can generate from the keyboard (SIGINT, SIGPIPE)
 * and trap the software terminate and hangup signals (SIGTERM, SIGHUP) so
 * that 'ttsig' gets called.
 */
void sigjoe();

/* void signrm(void);  Set above signals back to their default values.
 */
void signrm();

extern int noxon;
extern int Baud;

#endif
