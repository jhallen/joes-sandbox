
/* Return true if input character available */
int constat();

/* Get input character:
    w = 0: wait until we have a character
    w = 1: return -1 if we don' have one
    */
int kget(int w);

/* Write character to terminal */
void vt52(int c);

