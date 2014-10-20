#ifndef _Ifmt
#define _Ifmt 1

typedef struct tray TRAY;
struct tray
 {
 int len;	/* No. lines in tray */
 int siz;	/* Malloc size of tray array */
 int **tray;	/* A tray of lines */
 int *traylen;	/* Length of each line */
 int *traysiz;	/* Malloc size of each line */
 };

extern int fillflg;
void rmtray();
TRAY *fmt();

#endif
