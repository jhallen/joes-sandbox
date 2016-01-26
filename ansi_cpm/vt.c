#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "vt.h"

int last = -1;

int kpoll(int w)
{
	int c;
	unsigned char d;
	int tries;
	if (last != -1) {
		c = last;
		last = -1;
		return c;
	}
	for (tries = 0; tries != 1; ++tries) {
		int flags;
		if (w) {
			flags = fcntl(fileno(stdin), F_GETFL);
			fcntl(fileno(stdin), F_SETFL, flags | O_NONBLOCK);
		}
		c = read(fileno(stdin), &d, 1);
		if (w) {
			fcntl(fileno(stdin), F_SETFL, flags);
		}
		if (c == 1) {
/*			printf("\r\nkpoll got %d \r\n", d); */
			return d;
		}
/*		usleep(1); */
	}
/*	printf("\r\n--- no char after esc? ---\r\n"); fflush(stdout); */
	return -1;
}

int constat()
{
	if (last != -1)
		return 1;
	last = kpoll(1);
	if (last != -1)
		return 1;
	else
		return 0;
}

/* Input FIFO */

#define FIFO_SIZE 4
int stuff[FIFO_SIZE];
int stuff_ptr;

void kpush(int c)
{
	if (c != -1 && stuff_ptr != FIFO_SIZE) {
		stuff[stuff_ptr++] = c;
	}
}

int kget(int w)
{
        int c;
        if (stuff_ptr) {
        	int x;
        	c = stuff[0];
        	for (x = 0; x != stuff_ptr - 1; ++x) {
        		stuff[x] = stuff[x + 1];
        	}
        	stuff[x] = 0;
        	return c;
        }

        c = kpoll(w);
        if (c != 27) {
                return c;
        }
        /* We got ESC.. see if any chars follow */
        c = kpoll(1);

        if (c == -1) { /* Just ESC */
        	return 27;
        } else if (c == '[') {
                c = kpoll(0);
                if (c == 'A') { /* Up arrow */
                        return 'E' - '@';
                } else if (c == 'B') { /* Down arrow */
                        return 'X' - '@';
                } else if (c == 'C') { /* Right arrow */
                        return 'D' - '@';
                } else if (c == 'D') { /* Left arrow */
                        return 'S' - '@';
                } else if (c == '3') { /* Delete key */
                        c = kpoll(0);
                        return 'G' - '@';
                } else if (c == '2') { /* Insert key */
                        c = kpoll(0);
                        return 'V' - '@';
                } else if (c == '5') { /* PgUp */
                        c = kpoll(0);
                        return 'R' - '@';
                } else if (c == '6') { /* PgDn */
                        c = kpoll(0);
                        return 'C' - '@';
                } else if (c == '1' || c == '7') { /* Home */
                	kpush('s');
                        c = kpoll(0);
                        return 'Q' - '@';
                } else if (c == '4' || c == '8') { /* End */
                        kpush('d');
                        c = kpoll(0);
                        return 'Q' - '@';
                } else if (c == 'H') { /* Home */
                        kpush('s');
                        return 'Q' - '@';
                } else if (c == 'F') { /* End */
                        kpush('d');
                        return 'Q' - '@';
                } else {
                	kpush('[');
                	kpush(c);
                        return 27;
		}
        } else if (c == 'O') {
                c = kpoll(0);
                if (c == 'A') { /* Up arrow */
                        return 'E' - '@';
                } else if (c == 'B') { /* Down arrow */
                        return 'X' - '@';
                } else if (c == 'C') { /* Right arrow */
                        return 'D' - '@';
                } else if (c == 'D') { /* Left arrow */
                        return 'S' - '@';
                } else if (c == 'd') { /* Ctrl left arrow (rxvt) */
                        return 'A' - '@';
                } else if (c == 'c') { /* Ctrl right arrow (rxvt) */
                        return 'F' - '@';
                } else if (c == 'H') { /* Home */
                        kpush('s');
                        return 'Q' - '@';
                } else if (c == 'F') { /* End */
                        kpush('d');
                        return 'Q' - '@';
                } else {
                	kpush('O');
                	kpush(c);
                	return 27;
		}
        } else {
        	kpush(c);
        	return 27;
        }
}

/*
[5~ PgUp
[6~ PgDn
[7~ Home
[8~ End
Od  Ctrl-Ltarw
Oc  Ctrl-Rtarw
*/

void putch(int c) {	/* output character without postprocessing */
    write(fileno(stdout), &c, 1);
}

void putmes(const char *s) {
    write(fileno(stdout), s, strlen(s));
}

void vt52(int c) {	/* simple vt52,adm3a => ANSI conversion */
    static int state = 0, x, y;
    char buff[32];
#ifdef DEBUGLOG
    static FILE *log = NULL;
    if (!log)
	log = fopen("cpm.out", "w");
    fputc(c, log);
#endif
    switch (state) {
    case 0:
	switch (c) {
#ifdef VBELL
        case 0x07:              /* BEL: flash screen */
            putmes("\033[?5h\033[?5l");
	    break;
#endif
	case 0x7f:		/* DEL: echo BS, space, BS */
	    putmes("\b \b");
	    break;
	case 0x1a:		/* adm3a clear screen */
	case 0x0c:		/* vt52 clear screen */
	    putmes("\033[H\033[2J");
	    break;
	case 0x1e:		/* adm3a cursor home */
	    putmes("\033[H");
	    break;
	case 0x1b:
	    state = 1;	/* esc-prefix */
	    break;
	case 1:
	    state = 2;	/* cursor motion prefix */
	    break;
	case 2:		/* insert line */
	    putmes("\033[L");
	    break;
	case 3:		/* delete line */
	    putmes("\033[M");
	    break;
	case 0x18: case 5:	/* clear to eol */
	    putmes("\033[K");
	    break;
	case 0x12: case 0x13:
	    break;
	default:
	    putch(c);
	}
	break;
    case 1:	/* esc was sent */
	switch (c) {
        case 0x1b:
	    putch(c);
	    break;
	case '=':
	case 'Y':
	    state = 2;
	    break;
	case 'E':	/* insert line */
	    putmes("\033[L");
	    break;
	case 'R':	/* delete line */
	    putmes("\033[M");
	    break;
	case 'B':	/* enable attribute */
	    state = 4;
	    break;
	case 'C':	/* disable attribute */
	    state = 5;
	    break;
        case 'L':       /* set line */
        case 'D':       /* delete line */
            state = 6;
            break;
	case '*':       /* set pixel */
	case ' ':       /* clear pixel */
	    state = 8;
	    break;
	default:		/* some true ANSI sequence? */
	    state = 0;
	    putch(0x1b);
	    putch(c);
	}
	break;
    case 2:
	y = c - ' '+1;
	state = 3;
	break;
    case 3:
	x = c - ' '+1;
	state = 0;
	sprintf(buff, "\033[%d;%dH", y, x);
	putmes(buff);
	break;
    case 4:	/* <ESC>+B prefix */
        state = 0;
        switch (c) {
	case '0': /* start reverse video */
	    putmes("\033[7m");
	    break;
	case '1': /* start half intensity */
	    putmes("\033[1m");
	    break;
	case '2': /* start blinking */
	    putmes("\033[5m");
	    break;
	case '3': /* start underlining */
	    putmes("\033[4m");
	    break;
	case '4': /* cursor on */
	    putmes("\033[?25h");
	    break;
	case '6': /* remember cursor position */
	    putmes("\033[s");
	    break;
	case '5': /* video mode on */
	case '7': /* preserve status line */
	    break;
	default:
	    putch(0x1b);
	    putch('B');
	    putch(c);
        }
	break;
    case 5:	/* <ESC>+C prefix */
        state = 0;
        switch (c) {
	case '0': /* stop reverse video */
	    putmes("\033[27m");
	    break;
	case '1': /* stop half intensity */
	    putmes("\033[m");
	    break;
	case '2': /* stop blinking */
	    putmes("\033[25m");
	    break;
	case '3': /* stop underlining */
	    putmes("\033[24m");
	    break;
	case '4': /* cursor off */
	    putmes("\033[?25l");
	    break;
	case '6': /* restore cursor position */
	    putmes("\033[u");
	    break;
	case '5': /* video mode off */
	case '7': /* don't preserve status line */
	    break;
	default:
	    putch(0x1b);
	    putch('C');
	    putch(c);
        }
	break;
/* set/clear line/point */
    case 6:
    case 7:
    case 8:
        state ++;
	break;
    case 9:
	state = 0;
    } 
}
