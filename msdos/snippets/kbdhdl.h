/* Keyboard interrupt handler .h file */

void kopen(void);	/* Install and enable the keyboard interrupt handler */
void kclose(void);	/* Restore original keyboard interrupt handler */
int kcheck(void);	/* Return true if there are any keys */
void kdump(void);	/* Dump any keys in the input buffer */
extern int brkflg;	/* This is set when the break key is pressed */

/* The following function is called when a new key arrives, but only when
   we're not in a dos function and when we're not in the 'key' function
   for any previous key.

   This function should return true if no more keys are to be accepted (I.E.,
   to prevent lossage when the program is about to exit).
*/

int key(unsigned char ascii,unsigned char scan);

/* Simulate terminal input.  This function can be called to give keys to the
   background program.  The background program will receive the keys when
   the console I/O routines are called.
*/

void sti(unsigned char ascii,unsigned char scan);

/* Steal terminal output.  This function is called when the background program
   writes to the terminal.  This function is only called when no DOS function
   is running, no 'key' function is running and no 'sto' function is running.
*/

void sto(unsigned char);
