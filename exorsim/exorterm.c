/*	EXORterm simulator
 *	Copyright
 *		(C) 2016 Joseph H. Allen
 *
 * This is free software; you can redistribute it and/or modify it under the 
 * terms of the GNU General Public License as published by the Free Software 
 * Foundation; either version 1, or (at your option) any later version.  
 *
 * It is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
 * details.  
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this software; see the file COPYING.  If not, write to the Free Software Foundation, 
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/poll.h>
#include "exorterm.h"

/* State of real terminal */

#define WIDTH 80
#define HEIGHT 24

int scrn[WIDTH*HEIGHT]; /* Screen contents */
int scrn_x = 0; /* Screen cursor position */
int scrn_y = 0;

/* Set real cursor position */

void cpos(int y, int x)
{
	if (y > scrn_y) { /* Need to go down */
		/* if (y - scrn_y <= 4) {
			while (y > scrn_y) {
				putchar(10);
				++scrn_y;
			}
		} else */
		if (y - scrn_y == 1) {
			printf("\033[B");
			++scrn_y;
		} else {
			printf("\033[%dB", y - scrn_y);
			scrn_y += y - scrn_y;
		}
	}

	if (y < scrn_y) { /* Need to go up */
		if (scrn_y - y == 1) {
			printf("\033[A");
			--scrn_y;
		} else {
			printf("\033[%dA", scrn_y - y);
			scrn_y -= scrn_y - y;
		}
	}

	if (x > scrn_x) { /* Need to go right */
		/* if (x - scrn_x <= 4) {
			while (x > scrn_x) {
				putchar(scrn[scrn_y * WIDTH + scrn_x]);
				++scrn_x;
			}
		} */
		if (x - scrn_x == 1) {
			printf("\033[C");
			scrn_x += 1;
		} else {
			printf("\033[%dC", x - scrn_x);
			scrn_x += x - scrn_x;
		}
	}

	if (x < scrn_x) { /* Need to go left */
		if (x == 0) {
			putchar(13);
			scrn_x = 0;
		} else if (scrn_x - x <= 4) {
			while (x < scrn_x) {
				putchar(8);
				--scrn_x;
			}
		} else { 
			printf("\033[%dD", scrn_x - x);
			scrn_x -= scrn_x - x;
		}
	}
}

/* Output character at a position */

void out(int y, int x, int c)
{
	cpos(y, x);
	putchar(c);
	scrn[scrn_y * WIDTH + scrn_x] = c;
	++scrn_x;
	if (scrn_x == WIDTH) {
		putchar(13);
		scrn_x = 0;
	}
}

/* Initialize screen */

void izscrn()
{
	int y;
	putchar('\r');
	for (y = 0; y != HEIGHT; ++y) {
		int x;
		for (x = 0; x != WIDTH; ++x) {
			scrn[y*WIDTH + x] = ' ';
		}
		putchar('\n');
	}
	scrn_x = 0;
	scrn_y = HEIGHT - 1;
//	printf("\033[H\033[J");
//	scrn_x = 0;
//	scrn_y = 0;
}

/* Modes */

enum {
	SCROLL,
	PAGE,
	PROTECT
};

/* Current mode */

int mode;

/* Current virtual view area */

int xleft = 0;
int xright = WIDTH;
int ytop = 0;
int ybottom = HEIGHT;

/* Current cursor position */

int xpos = 0;
int ypos = 0;

/* Buffer */

int screen[WIDTH*HEIGHT];

/* Magic cookie bits */
#define BLINK 0x01
#define INVERSE 0x02
#define DIM 0x04
#define UNDERLINE 0x08
#define BLANK 0x10
#define PROTECT 0x20

/* Terminal input FIFO */

#define FIFOSIZE 8192

unsigned char fifo[FIFOSIZE];
int fifo_old = 0;
int fifo_new = 0;

void term_put(int c)
{
	fifo[fifo_new++] = c;
	if (fifo_new == FIFOSIZE)
		fifo_new = 0;
}

void term_reset()
{
	int y;
	for (y = 0; y != HEIGHT; ++y) {
		int x;
		for (x = 0; x != WIDTH; ++x) {
			screen[y*WIDTH + x] = ' ';
		}
	}
	xleft = 0;
	xright = WIDTH;
	ytop = 0;
	ybottom = HEIGHT;
	mode = SCROLL;
	xpos = 0;
	ypos = 0;
}

/* Make real screen look like buffer */

void update()
{
	int y;
	for (y = 0; y != HEIGHT; ++y) {
		int x;
		for (x = 0; x != WIDTH; ++x) {
			if (scrn[y*WIDTH + x] != screen[y*WIDTH + x]) {
				out(y, x, screen[y*WIDTH + x]);
			}
		}
	}
	cpos(ypos, xpos);
	fflush(stdout);
}

/* Initialize emulator */

void izexorterm()
{
	izscrn();
	term_reset();
	update();
}

/* Delete line in virtual area */

void del(int line)
{
	int y;
	int x;
	for (y = ytop + line + 1; y != ybottom; ++y) {
		memcpy(screen + (y - 1) * WIDTH + xleft,
		       screen + (y    ) * WIDTH + xleft,
		       (xright - xleft) * sizeof(screen[0]));
	}
	for (x = xleft; x != xright; ++x) {
		screen[(y - 1) * WIDTH + x] = ' ';
	}
}

/* Insert list in virtual area */

void ins(int line)
{
	int y;
	int x;
	for (y = ybottom - 1; y > ytop + line; --y) {
		memcpy(screen + (y    ) * WIDTH + xleft,
		       screen + (y - 1) * WIDTH + xleft,
		       (xright - xleft) * sizeof(screen[0]));
	}
	for (x = xleft; x != xright; ++x) {
		screen[y * WIDTH + x] = ' ';
	}
}

/* Scroll virtual area up */

void scrup()
{
	/* Scroll the real terminal if we can */
	if (xleft == 0 && xright == WIDTH && ytop == 0 && ybottom == HEIGHT) {
		int y, x;
		update();
		cpos(HEIGHT - 1, 0);
		putchar(10);
		for (y = 1; y != HEIGHT; ++y) {
			memcpy(scrn + (y - 1) * WIDTH,
			       scrn + (y    ) * WIDTH,
			       WIDTH * sizeof(screen[0]));
		}
		for (x = 0; x != WIDTH; ++x) {
			scrn[(y - 1) * WIDTH + x] = ' ';
		}
	}
	del(0);
}

/* Type a character */

void term_type(int c)
{
	switch (mode) {
		case SCROLL: { /* Scroll */
			screen[ypos * WIDTH + xpos] = c;
			if (++xpos == xright)
				xpos = xright - 1;
			break;
		} case PAGE: { /* Page */
			screen[ypos * WIDTH + xpos] = c;
			if (++xpos == xright) {
				xpos = xleft;
				if (++ypos == ybottom) {
					xpos = ytop;
				}
			}
			break;
		} case PROTECT: { /* Protect */
			break;
		}
	}
}

void term_up()
{
	switch (mode) {
		case SCROLL: {
			if (ypos != ytop)
				--ypos;
			break;
		} case PAGE: {
			if (ypos != ytop) {
				--ypos;
			} else {
				ypos = ybottom - 1;
			}
			break;
		} case PROTECT: {
			break;
		}
	}
}

void term_down()
{
	switch (mode) {
		case SCROLL: {
			if (ypos + 1 == ybottom) {
				scrup();
			} else {
				++ypos;
			}
			break;
		} case PAGE: {
			if (ypos + 1 == ybottom) {
				ypos = ytop;
			} else {
				++ypos;
			}
			break;
		} case PROTECT: {
			break;
		}
	}
}

void term_left()
{
	switch (mode) {
		case SCROLL: {
			if (xpos != xleft) {
				--xpos;
			}
			break;
		} case PAGE: {
			if (xpos != xleft) {
				--xpos;
			} else {
				xpos = xright - 1;
				term_up();
			}
			break;
		} case PROTECT: {
			break;
		}
	}
}

void term_right()
{
	switch (mode) {
		case SCROLL: {
			if (xpos + 1 != xright) {
				++xpos;
			} else {
				
			}
			break;
		} case PAGE: {
			if (xpos + 1 != xright) {
				++xpos;
			} else {
				xpos = 0;
				term_down();
			}
			break;
		} case PROTECT: {
			break;
		}
	}
}

void term_goto(int row, int col)
{
	if (row >= ybottom || row < ytop)
		return;
	if (col >= xright || col < xleft)
		return;
	xpos = col;
	ypos = row;
/*
	if (row >= ybottom - ytop)
		return;
	if (col >= xright - xleft)
		return;
	ypos = ytop + row;
	xpos = xleft + col;
*/
}

void term_home()
{
	xpos = xleft;
	ypos = ytop;
}

void term_clear()
{
	int y, x;
	for (y = ytop; y != ybottom; ++y) {
		for (x = xleft; x != xright; ++x)
			screen[y * WIDTH + x] = ' ';
	}
	term_home();
}

void term_set_left(int col)
{
	if (col >= 0 && col < xright) {
		xleft = col;
		if (xpos < xleft) {
			xpos = xleft;
		}
	}
}

void term_set_right(int col)
{
	if (col > xleft && col < WIDTH) {
		xright = col + 1;
		if (xpos >= xright) {
			xpos = xright - 1;
		}
	}
}

void term_set_top(int row)
{
	if (row >= 0 && row < ybottom) {
		ytop = row;
		if (ypos < ytop) {
			ypos = ytop;
		}
	}
}

void term_set_bottom(int row)
{
	if (row > ytop && row < HEIGHT) {
		ybottom = row + 1;
		if (ypos >= ybottom) {
			ypos = ybottom - 1;
		}
	}
}

void term_reset_virtual()
{
	xleft = 0;
	xright = WIDTH;
	ytop = 0;
	ybottom = HEIGHT;
}

void term_return()
{
	xpos = xleft;
}

void term_read_cursor()
{
	term_put(0xDD);
	term_put(0x20 + ypos);
	term_put(0x20 + xpos);
	term_put(0xDE);
}

void term_set_page_mode()
{
	if (mode != PAGE) {
		mode = PAGE;
		term_reset_virtual();
		term_home();
	}
}

void term_set_scroll_mode()
{
	if (mode != SCROLL) {
		mode = SCROLL;
		term_reset_virtual();
		term_home();
	}
}

void term_set_protect_mode()
{
	mode = PROTECT;
	term_reset_virtual();
	term_home();
}

void term_delete_char()
{
}

void term_enable_keyboard()
{
}

void term_disable_keyboard()
{
}

void term_page_erase()
{
}

void term_line_erase()
{
}

void term_line_insert()
{
	if (mode == PAGE) {
		ins(ypos - ytop);
		term_return();
	}
}

void term_line_delete()
{
	if (mode == PAGE) {
		del(ypos - ytop);
		term_return();
	}
}

void term_send_page()
{
}

void term_send_line()
{
}

void term_forward_tab()
{
}

void term_backward_tab()
{
}

void term_fac_set(int fac)
{
}

void term_fac_reset(int fac)
{
}

void term_set_transparent()
{
}

void term_reset_transparent()
{
}

void term_set_inverse()
{
}

void term_reset_inverse()
{
}

void term_set_status()
{
}

void term_reset_status()
{
}

void term_autolinefeed()
{
}

void term_set_special()
{
}

void term_reset_special()
{
}

/* Parser states */

enum {
	IDLE,
	LOAD_CURSOR_1,
	LOAD_CURSOR_2,
	SET_TOP,
	SET_BOTTOM,
	SET_LEFT,
	SET_RIGHT,
	WRITE_ABSOLUTE_1,
	WRITE_ABSOLUTE_2,
	WRITE_ABSOLUTE_3,
	WRITE_ABSOLUTE_4,
	READ_ABSOLUTE_1,
	INSERT_CHAR,
	SET_TABS,
};

/* Current state */

int state = IDLE;
int inbuf[3];

FILE *logfile;
int reveal;

char *decode(int c)
{
	switch (c) {
		case 0x08: {
			return "cursor left";
			break;
		} case 0x0A: {
			return "cursor down";
			break;
		} case 0x0B: {
			return "cursor up";
			break;
		} case 0x0C: {
			return "cursor right";
			break;
		} case 0x0D: {
			return "return";
			break;
		} case 0xC0: {
			return "home";
			break;
		} case 0xC1: {
			return "cursor up";
			break;
		} case 0xC2: {
			return "cursor down";
			break;
		} case 0xC3: {
			return "cursor left";
			break;
		} case 0xC4: {
			return "cursor right";
			break;
		} case 0xC5: {
			return "load cursor";
			break;
		} case 0xC6: {
			return "read cursor";
			break;
		} case 0xC7: {
			return "set page mode";
			break;
		} case 0xC8: {
			return "set scroll mode";
			break;
		} case 0xC9: {
			return "set top";
			break;
		} case 0xCA: {
			return "set bottom";
			break;
		} case 0xCB: {
			return "set left";
			break;
		} case 0xCC: {
			return "set right";
			break;
		} case 0xCD: {
			return "set protect mode";
			break;
		} case 0xCE: {
			return "write absolute";
			break;
		} case 0xCF: {
			return "read absolute";
			break;
		} case 0xD0: {
			return "insert char";
			break;
		} case 0xD1: {
			return "delete char";
			break;
		} case 0xD2: {
			return "enable keyboard";
			break;
		} case 0xD3: {
			return "disable keyboard";
			break;
		} case 0xD4: {
			return "page erase";
			break;
		} case 0xD5: {
			return "line erase";
			break;
		} case 0xD6: {
			return "line insert";
			break;
		} case 0xD7: {
			return "line delete";
			break;
		} case 0xD8: {
			return "clear";
			break;
		} case 0xD9: {
			return "send page";
			break;
		} case 0xDA: {
			return "forward tab";
			break;
		} case 0xDB: {
			return "backware tab";
			break;
		} case 0xDC: {
			return "set tabs";
			break;
		} case 0xDF: {
			return "send line";
			break;
		} case 0xE0: {
			return "term_fac_set(BLINK)";
			break;
		} case 0xE1: {
			return "term_fac_reset(BLINK);";
			break;
		} case 0xE2: {
			return "term_fac_set(INVERSE);";
			break;
		} case 0xE3: {
			return "term_fac_reset(INVERSE);";
			break;
		} case 0xE4: {
			return "term_fac_set(DIM);";
			break;
		} case 0xE5: {
			return "term_fac_reset(DIM);";
			break;
		} case 0xE6: {
			return "term_fac_set(UNDERLINE);";
			break;
		} case 0xE7: {
			return "term_fac_reset(UNDERLINE);";
			break;
		} case 0xE8: {
			return "term_fac_set(BLANK);";
			break;
		} case 0xE9: {
			return "term_fac_reset(BLANK);";
			break;
		} case 0xEA: {
			return "term_fac_set(PROTECT);";
			break;
		} case 0xEB: {
			return "term_fac_reset(PROTECT);";
			break;
		} case 0xEC: {
			return "term_set_transparent();";
			break;
		} case 0xED: {
			return "term_reset_transparent();";
			break;
		} case 0xEE: {
			return "term_set_inverse();";
			break;
		} case 0xEF: {
			return "term_reset_inverse();";
			break;
		} case 0xF1: {
			return "term_reset();";
			break;
		} case 0xF2: {
			return "term_set_status();";
			break;
		} case 0xF3: {
			return "term_reset_status();";
			break;
		} case 0xF7: {
			return "term_autolinefeed();";
			break;
		} case 0xFC: {
			return "term_set_special();";
			break;
		} case 0xFD: {
			return "term_reset_special();";
			break;
		} default: {
			if (c >= 0x20 && c <= 0x7E) {
				return "term_type(c);";
			} else {
				return "huh?";
			}
			break;
		}
	}
}

void term_out(int c)
{
	if (reveal) {
		if (!logfile) {
			logfile = fopen("logfile", "w");
		}
		fprintf(logfile, "%2.2x '%c' %s\n", c, c, decode(c));
		fflush(logfile);
	}

	// putchar(c); return;
	switch (state) {
		case IDLE: {
			switch (c) {
				case 0x08: {
					term_left();
					break;
				} case 0x0A: {
					term_down();
					break;
				} case 0x0B: {
					term_up();
					break;
				} case 0x0C: {
					term_right();
					break;
				} case 0x0D: {
					term_return();
					break;
				} case 0xC0: {
					term_home();
					break;
				} case 0xC1: {
					term_up();
					break;
				} case 0xC2: {
					term_down();
					break;
				} case 0xC3: {
					term_left();
					break;
				} case 0xC4: {
					term_right();
					break;
				} case 0xC5: {
					state = LOAD_CURSOR_1;
					break;
				} case 0xC6: {
					term_read_cursor();
					break;
				} case 0xC7: {
					term_set_page_mode();
					break;
				} case 0xC8: {
					term_set_scroll_mode();
					break;
				} case 0xC9: {
					state = SET_TOP;
					break;
				} case 0xCA: {
					state = SET_BOTTOM;
					break;
				} case 0xCB: {
					state = SET_LEFT;
					break;
				} case 0xCC: {
					state = SET_RIGHT;
					break;
				} case 0xCD: {
					term_set_protect_mode();
					break;
				} case 0xCE: {
					state = WRITE_ABSOLUTE_1;
					break;
				} case 0xCF: {
					state = READ_ABSOLUTE_1;
					break;
				} case 0xD0: {
					state = INSERT_CHAR;
					break;
				} case 0xD1: {
					term_delete_char();
					break;
				} case 0xD2: {
					term_enable_keyboard();
					break;
				} case 0xD3: {
					term_disable_keyboard();
					break;
				} case 0xD4: {
					term_page_erase();
					break;
				} case 0xD5: {
					term_line_erase();
					break;
				} case 0xD6: {
					term_line_insert();
					break;
				} case 0xD7: {
					term_line_delete();
					break;
				} case 0xD8: {
					term_clear();
					break;
				} case 0xD9: {
					term_send_page();
					break;
				} case 0xDA: {
					term_forward_tab();
					break;
				} case 0xDB: {
					term_backward_tab();
					break;
				} case 0xDC: {
					state = SET_TABS;
					break;
				} case 0xDF: {
					term_send_line();
					break;
				} case 0xE0: {
					term_fac_set(BLINK);
					break;
				} case 0xE1: {
					term_fac_reset(BLINK);
					break;
				} case 0xE2: {
					term_fac_set(INVERSE);
					break;
				} case 0xE3: {
					term_fac_reset(INVERSE);
					break;
				} case 0xE4: {
					term_fac_set(DIM);
					break;
				} case 0xE5: {
					term_fac_reset(DIM);
					break;
				} case 0xE6: {
					term_fac_set(UNDERLINE);
					break;
				} case 0xE7: {
					term_fac_reset(UNDERLINE);
					break;
				} case 0xE8: {
					term_fac_set(BLANK);
					break;
				} case 0xE9: {
					term_fac_reset(BLANK);
					break;
				} case 0xEA: {
					term_fac_set(PROTECT);
					break;
				} case 0xEB: {
					term_fac_reset(PROTECT);
					break;
				} case 0xEC: {
					term_set_transparent();
					break;
				} case 0xED: {
					term_reset_transparent();
					break;
				} case 0xEE: {
					term_set_inverse();
					break;
				} case 0xEF: {
					term_reset_inverse();
					break;
				} case 0xF1: {
					term_reset();
					break;
				} case 0xF2: {
					term_set_status();
					break;
				} case 0xF3: {
					term_reset_status();
					break;
				} case 0xF7: {
					term_autolinefeed();
					break;
				} case 0xFC: {
					term_set_special();
					break;
				} case 0xFD: {
					term_reset_special();
					break;
				} default: {
					if (c >= 0x20 && c <= 0x7E) {
						term_type(c);
					}
					break;
				}
			}
			break;
		} case LOAD_CURSOR_1: {
			state = IDLE;
			if (c >= 0x20 && c < 0x70) {
				inbuf[0] = c - 0x20;
				state = LOAD_CURSOR_2;
			}
			break;
		} case LOAD_CURSOR_2: {
			state = IDLE;
			if (c >= 0x20 && c < 0x70) {
				term_goto(inbuf[0], c - 0x20);
				
			}
			break;
		} case SET_TOP: {
			state = IDLE;
			if (c >= 0x20 && c < 0x70)
				term_set_top(c - 0x20);
			break;
		} case SET_BOTTOM: {
			state = IDLE;
			if (c >= 0x20 && c < 0x70)
				term_set_bottom(c - 0x20);
			break;
		} case SET_LEFT: {
			state = IDLE;
			if (c >= 0x20 && c < 0x70)
				term_set_left(c - 0x20);
			break;
		} case SET_RIGHT: {
			state = IDLE;
			if (c >= 0x20 && c < 0x70)
				term_set_right(c - 0x20);
			break;
		} case WRITE_ABSOLUTE_1: {
			state = IDLE;
			if (c == 0xDD)
				state = WRITE_ABSOLUTE_2;
			break;
		} case WRITE_ABSOLUTE_2: {
			state = IDLE;
			if (c >= 0x20 && c < 0x70) {
				inbuf[0] = c - 0x20;
				state = WRITE_ABSOLUTE_3;
			}
			break;
		} case WRITE_ABSOLUTE_3: {
			state = IDLE;
			if (c >= 0x20 && c < 0x70) {
				inbuf[1] = c - 0x20;
				state = WRITE_ABSOLUTE_4;
			}
			break;
		} case WRITE_ABSOLUTE_4: {
			state = IDLE;
			if (c >= 0x20 && c < 0x7F && inbuf[0] >= 0 && inbuf[0] < HEIGHT && inbuf[1] >= 0 && inbuf[1] < WIDTH) {
				screen[inbuf[0] * WIDTH + inbuf[1]] = c;
				++inbuf[1];
				state = WRITE_ABSOLUTE_4;
			}
			break;
		} case READ_ABSOLUTE_1: {
			state = IDLE;
			break;
		} case INSERT_CHAR: {
			state = IDLE;
			break;
		} case SET_TABS: {
			state = IDLE;
			break;
		}
	}
}

/* Check for input */

extern int stop;
extern int lower;

enum {
	INIDLE,
	INESC,
	INBRACK,
	INBRACKBRACK,
	INNUM,
	INO
};

int instate;
int innum;

int term_poll()
{
	int flags;
	int rtn;
	unsigned char c;
	if (fifo_old != fifo_new)
		return 1;

	update();
	// fflush(stdout);

	again:

	flags = fcntl(fileno(stdin), F_GETFL);
	if (flags == -1) {
		perror("fcntl error");
		exit(-1);
	}
	fcntl(fileno(stdin), F_SETFL, flags | O_NONBLOCK);
	rtn = read(fileno(stdin), &c, 1);
	fcntl(fileno(stdin), F_SETFL, flags);
	if (rtn < 1 && !stop) {
		poll(NULL, 0, 8); /* Don't hog CPU time */
	}
	if (rtn == 1) {
		switch (instate) {
			case INIDLE: {
				if (c == 27) {
					instate = INESC;
				} else {
					if (!lower && c >= 'a' && c <= 'z')
						c += 'A' - 'a';
					term_put(c);
					return 1;
				}
				break;
			} case INESC: {
				instate = INIDLE;
				if (c == '[') {
					instate = INBRACK;
				} else if (c == 'O') {
					instate = INO;
				}
				break;
			} case INO: {
				instate = INIDLE;
				if (c == 'P') { /* F1 */
					term_put(0xA0);
					return 1;
				} else if (c == 'Q') { /* F2 */
					term_put(0xA1);
					return 1;
				} else if (c == 'R') { /* F3 */
					term_put(0xA2);
					return 1;
				} else if (c == 'S') { /* F4 */
					term_put(0xA3);
					return 1;
				} else if (c == 'A') { /* Up arrow */
					if (mode == SCROLL)
						term_put(0x0B);
					else
						term_put(0xC1);
					return 1;
				} else if (c == 'B') { /* Down arrow */
					if (mode == SCROLL)
						term_put(0x0A);
					else
						term_put(0xC2);
					return 1;
				} else if (c == 'C') { /* Right arrow */
					if (mode == SCROLL)
						term_put(0x0C);
					else
						term_put(0xC4);
					return 1;
				} else if (c == 'D') { /* Left arrow */
					if (mode == SCROLL)
						term_put(0x08);
					else
						term_put(0xC3);
					return 1;
				}
				break;
			} case INBRACK: {
				instate = INIDLE;
				if (c == '[') {
					instate = INBRACKBRACK;
				} else if (c == 'A') {
					if (mode == SCROLL)
						term_put(0x0B);
					else
						term_put(0xC1);
					return 1;
				} else if (c == 'B') {
					if (mode == SCROLL)
						term_put(0x0A);
					else
						term_put(0xC2);
					return 1;
				} else if (c == 'C') {
					if (mode == SCROLL)
						term_put(0x0C);
					else
						term_put(0xC4);
					return 1;
				} else if (c == 'D') {
					if (mode == SCROLL)
						term_put(0x08);
					else
						term_put(0xC3);
					return 1;
				} else if (c >= '1' && c <= '9') {
					innum = c - '0';
					instate = INNUM;
				}
				break;
			} case INBRACKBRACK: {
				instate = INIDLE;
				if (c == 'A') { /* F1 */
					term_put(0xA0);
					return 1;
				} else if (c == 'B') { /* F2 */
					term_put(0xA1);
					return 1;
				} else if (c == 'C') { /* F3 */
					term_put(0xA2);
					return 1;
				} else if (c == 'D') { /* F4 */
					term_put(0xA3);
					return 1;
				} else if (c == 'E') { /* F5 */
					term_put(0xA4);
					return 1;
				}
				break;
			} case INNUM: {
				instate = INIDLE;
				if (c >= '0' && c <= '9') {
					innum = innum * 10 + c - '0';
					instate = INNUM;
				} else if (c == '~') {
					switch (innum) {
						case 11: { /* F1 */
							term_put(0xA0);
							return 1;
						} case 12: { /* F2 */
							term_put(0xA1);
							return 1;
						} case 13: { /* F3 */
							term_put(0xA2);
							return 1;
						} case 14: { /* F4 */
							term_put(0xA3);
							return 1;
						} case 15: { /* F5 */
							term_put(0xA4);
							return 1;
						} case 17: { /* F6 */
							term_put(0xA5);
							return 1;
						} case 18: { /* F7 */
							term_put(0xA6);
							return 1;
						} case 19: { /* F8 */
							term_put(0xA7);
							return 1;
						} case 20: { /* F9 */
							term_put(0xA8);
							return 1;
						} case 21: { /* F10 */
							term_put(0xA9);
							return 1;
						} case 23: { /* F11 */
							reveal = 1;
							term_put(0xAA);
							return 1;
						} case 24: { /* F12 */
							reveal = 0;
							term_put(0xAB);
							return 1;
						}
					}
				}
			}
		}
		goto again;
	}
	return 0;
}

/* Read from terminal */

int term_in()
{
	int c;

	/* Wait for a character */
	while (!stop && !term_poll());

	/* Return it */
	c = fifo[fifo_old++];
	if (fifo_old == FIFOSIZE)
		fifo_old = 0;
	return c;
}
