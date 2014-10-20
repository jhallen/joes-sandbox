/* TTY interface header file
   Copyright (C) 1991 Joseph H. Allen

This file is part of JOE (Joe's Own Editor)

JOE is free software; you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software
Foundation; either version 1, or (at your option) any later version.  

JOE is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.  

You should have received a copy of the GNU General Public License
along with JOE; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#ifndef _Itty
#define _Itty 1

#include "config.h"

/* void ttopen(void);  Open the tty (attached to stdin) for use inside of JOE
 *
 * (0) Call sigjoe()
 *     There is also 'void ttopenn(void)' which does not do this step.
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
 * (4) Set this new tty state without loosing any typeahead
 *
 * (5) Store the baud rate in the global variable 'baud'
 *
 * (6) Divide the baud rate into the constant DIVIDEND and store the result
 *     in the global variable 'upc'.  This should come out to the number
 *     of microseconds needed to send each character.  The constant 'DIVIDEND'
 *     should be chosen so that 'upc' reflects the real throughput of the
 *     tty, not the theoretical best throughput.
 *
 * (7) Create an output buffer of a size which depends on 'upc' and the
 *     constant 'TIMES'.  'TIMES' is the number of times per second JOE
 *     should check for typeahead.  Since we only check for typehead after
 *     the output buffer is flushed, 'upc' and the size of the output buffer
 *     determine how often this occurs.  So for example if 'upc'==1000 (~9600
 *     baud) and 'TIMES'==3, the output buffer size is set to 333 characters.
 *     Each time this buffer is completely flushed, 1/3 of a second will go by.
 */
void ttopen();
void ttopnn();
extern unsigned long upc;
extern unsigned baud;

#define TIMES 3
#define DIVIDEND 10000000

/* void ttclose(void);  Restore the tty back to its original mode.
 *
 * (1) ttyflsh()
 *
 * (2) Restore the original tty mode which aopen() had saved.  Do this without
 *     loosing any typeahead.
 *
 * (3) Call signorm().  There is also 'void ttyclosen(void)' which does not do this
 *     step.
 */
void ttclose();
void ttclsn();

/* int ttgetc(void);  Flush the output and get the next character from the tty
 *
 * (1) aflush()
 *
 * (2) Read the next input character
 *     If the input closed, call 'tsignal' with 0 as its argument.
 *
 * (3) Clear 'have'
 */
int ttgetc();

/* void ttputc(char c);  Write a character to the output buffer.  If it becomes
 * full, call aflush()
 */
void ttputc();

/* void ttputs(char *s);  Write a string to the output buffer.  Any time the
 * output buffer gets full, call aflush()
 */
void ttputs();

/* void ttshell(char *s);  Run a shell command or if 's' is zero, run a sub-shell
 */
void ttshell();

/* void ttsusp(void);  Suspend the process, or if the UNIX can't do it, call
 * ttshell(NULL)
 */
void ttsusp();

/* void ttflsh(void);  Flush the output buffer and check for typeahead.
 *
 * (1) write() any character in the output buffer to the tty.  Sleep for the
 *     amount of time it should take for all of these characters to get
 *     to the tty.  This is so that any buffering between the editor and the
 *     tty is defeated.  If this is not done, the screen update will not be
 *     able to defer for typeahead.
 *
 *     The best way to do this (and it's currently only possible in BSD) is to
 *     set a timer for the necessary amount, write the characters to the tty,
 *     and then sleep until the timer expires.
 *
 *     If this can't be done, it's usually ok to 'write' and then to sleep for
 *     the necessary amount of time.  However, you will notice delays in the
 *     screen update if the 'write' actually takes any significant amount of
 *     time to execute (it usually takes none since all it usually does is
 *     write to an operating system buffer).
 *
 * (2) If the global variable 'leave' is not set and if the global variable
 *     'have' is not set, check for typeahead.  If there is any, set the global
 *     variable 'have'.  This absolutely must not read any characters from the
 *     'tty' if 'leave' is set or typeahead will be lost when the editor exists
 *     or does a shell escape.
 */
void ttflsh();

extern int have;
extern int leave;

/* void ttsig(int n);  Signal handler you provide.  This is called if the editor
 * gets a hangup signal, termination signal or if the input closes.  It is
 * called with 'n' set to the number of the caught signal or 0 if the input
 * closed.
 */
void ttsig();

/* void ttgtsz(int *x,int *y);  Get size of screen from ttsize/winsize
 * structure */
void ttgtsz();

/* You don't have to call these: ttopen/ttclose do it for you.  These
 * may be needed to make shell escape sequences.
 */

/* void sigjoe(void);  Set the signal handling for joe.  I.E., ignore all
 * signals the user can generate from the keyboard (SIGINT, SIGQUIT, SIGPIPE)
 * and trap the software terminate and hangup signals (SIGTERM, SIGHUP) so
 * that 'ttsig' gets called.
 */
void sigjoe();

/* void signrm(void);  Set above signals back to their default values.
 */
void signrm();

#endif
