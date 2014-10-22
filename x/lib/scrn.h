/* Device independant tty interface for JOE
   Copyright (C) 1992 Joseph H. Allen

This file is part of JOE (Joe's Own Editor)

JOE is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation; either version 1, or (at your option) any later version.  

JOE is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.  

You should have received a copy of the GNU General Public License along with 
JOE; see the file COPYING.  If not, write to the Free Software Foundation, 
675 Mass Ave, Cambridge, MA 02139, USA.  */ 

#ifndef _Iscrn
#define _Iscrn 1

#include "config.h"
#include "termcap.h"

typedef struct scrn SCRN;

struct hentry
 {
 int next;
 int loc;
 };

/* Each terminal has one of these */

#ifdef __MSDOS__

struct scrn
 {
 int li;
 int co;
 short *scrn;
 int scroll;
 int insdel;
 int *updtab;
 int *compose;
 int *sary;
 };

#else

struct scrn
 {
 CAP *cap;		/* Termcap/Terminfo data */

 int li;			/* Screen height */
 int co;			/* Screen width */

 char *ti;			/* Initialization string */
 char *cl;			/* Home and clear screen... really an
 				   init. string */
 char *cd;			/* Clear to end of screen */
 char *te;			/* Restoration string */

 int haz;			/* Terminal can't print ~s */
 int os;			/* Terminal overstrikes */
 int eo;			/* Can use blank to erase even if os */
 int ul;			/* _ overstrikes */
 int am;			/* Terminal has autowrap, but not magicwrap */
 int xn;			/* Terminal has magicwrap */

 char *so;			/* Enter standout (inverse) mode */
 char *se;			/* Exit standout mode */

 char *us;			/* Enter underline mode */
 char *ue;			/* Exit underline mode */
 char *uc;			/* Single time underline character */

 int ms;			/* Ok to move when in standout/underline mode */

 char *mb;			/* Enter blinking mode */
 char *md;			/* Enter bold mode */
 char *mh;			/* Enter dim mode */
 char *mr;			/* Enter inverse mode */
 char *me;			/* Exit above modes */

 char *Sb;			/* Set background color */
 char *Sf;			/* Set foreground color */
 int ut;			/* Screen erases with background color */

 int da, db;			/* Extra lines exist above, below */
 char *al, *dl, *AL, *DL;	/* Insert/delete lines */
 char *cs;			/* Set scrolling region */
 int rr;			/* Set for scrolling region relative addressing */
 char *sf, *SF, *sr, *SR;	/* Scroll */

 char *dm, *dc, *DC, *ed;	/* Delete characters */
 char *im, *ic, *IC, *ip, *ei;	/* Insert characters */
 int mi;			/* Set if ok to move while in insert mode */

 char *bs;			/* Move cursor left 1 */
 int cbs;
 char *lf;			/* Move cursor down 1 */
 int clf;
 char *up;			/* Move cursor up 1 */
 int cup;
 char *nd;			/* Move cursor right 1 */

 char *ta;			/* Move cursor to next tab stop */
 int cta;
 char *bt;			/* Move cursor to previous tab stop */
 int cbt;
 int tw;			/* Tab width */

 char *ho;			/* Home cursor to upper left */
 int cho;
 char *ll;			/* Home cursor to lower left */
 int cll;
 char *cr;			/* Move cursor to left edge */
 int ccr;
 char *RI;			/* Move cursor right n */
 int cRI;
 char *LE;			/* Move cursor left n */
 int cLE;
 char *UP;			/* Move cursor up n */
 int cUP;
 char *DO;			/* Move cursor down n */
 int cDO;
 char *ch;			/* Set cursor column */
 int cch;
 char *cv;			/* Set cursor row */
 int ccv;
 char *cV;			/* Goto beginning of specified line */
 int ccV;
 char *cm;			/* Set cursor row and column */
 int ccm;

 char *ce;			/* Clear to end of line */
 int cce;

 /* Basic abilities */
 int scroll;			/* Set to use scrolling */
 int insdel;			/* Set to use insert/delete within line */

 /* Current state of terminal */
 int *scrn;			/* Current contents of screen */
 int x,y;			/* Current cursor position (-1 for unknown) */
 int top,bot;			/* Current scrolling region */
 int attrib;			/* Current character attributes */
 int ins;			/* Set if we're in insert mode */

 int *updtab;			/* Dirty lines table */
 int avattr;			/* Bits set for available attributes */
 int *sary;			/* Scroll buffer array */

 int *compose;			/* Line compose buffer */
 int *ofst;			/* stuff for magic */
 struct hentry *htab;
 struct hentry *ary;
 };

#endif

/* SCRN *nopen(CAP *cap);
 *
 * Create a screen output object
 *  'cap' is the termcap entry for the terminal.  If 'cap' is NULL,
 *  NULL is returned by 'nopen'.
 */
extern SCRN *nopen();

/* void nresize(SCRN *t,int w,int h);
 *
 * Change size of screen.  For example, call this when you find out that
 * the Xterm changed size.
 */
extern void nresize();

/* void nredraw(SCRN *t);
 *
 * Invalidate all state variables for the terminal.  This way, everything gets
 * redrawn.
 */
extern void nredraw();

/* void nclose(SCRN *t);
 *
 * Close the screen and restore TTY to initial state.
 */
extern void nclose();

/* int cpos(SCRN *t,int x,int y);
 *
 * Set cursor position
 */
extern int cpos();

/* void outatr(SCRN *t,int x,int y,int c);
 *
 * Output a character at the given screen cooridinate.  The cursor position
 * after this function is executed is indeterminate.
 */
extern void outatr();

/* Character attribute bits */

#define INVERSE 256
#define UNDERLINE 512
#define BOLD 1024
#define BLINK 2048
#define DIM 4096
#define AT_MASK (INVERSE+UNDERLINE+BOLD+BLINK+DIM)

/* Standard background colors */

#define BG_SHIFT 13
#define BG_MASK (7<<BG_SHIFT)
#define BG_BLACK (0<<BG_SHIFT)		/* Default */
#define BG_RED (1<<BG_SHIFT)
#define BG_GREEN (2<<BG_SHIFT)
#define BG_YELLOW (3<<BG_SHIFT)
#define BG_BLUE (4<<BG_SHIFT)
#define BG_MAGENTA (5<<BG_SHIFT)
#define BG_CYAN (6<<BG_SHIFT)
#define BG_WHITE (7<<BG_SHIFT)

/* Standard foreground colors */

#define FG_SHIFT 16
#define FG_MASK (7<<FG_SHIFT)
#define FG_WHITE (0<<FG_SHIFT)		/* Default */
#define FG_CYAN (1<<FG_SHIFT)
#define FG_MAGENTA (2<<FG_SHIFT)
#define FG_BLUE (3<<FG_SHIFT)
#define FG_YELLOW (4<<FG_SHIFT)
#define FG_GREEN (5<<FG_SHIFT)
#define FG_RED (6<<FG_SHIFT)
#define FG_BLACK (7<<FG_SHIFT)

/* void eraeol(SCRN *t,int x,int y);
 *
 * Erase from screen coordinate to end of line.
 */
extern void eraeol();

/* void nscrlup(SCRN *t,int top,int bot,int amnt);
 *
 * Buffered scroll request.  Request that some lines up.  'top' and 'bot'
 * indicate which lines to scroll.  'bot' is the last line to scroll + 1.
 * 'amnt' is distance in lines to scroll.
 */
extern void nscrlup();

/* void nscrldn(SCRN *t,int top,int bot,int amnt);
 *
 * Buffered scroll request.  Scroll some lines down.  'top' and 'bot'
 * indicate which lines to scroll.  'bot' is the last line to scroll + 1.
 * 'amnt' is distance in lines to scroll.
 */
extern void nscrldn();

/* void nscroll(SCRN *t);
 *
 * Execute buffered scroll requests
 */
extern void nscroll();

/* void magic(SCRN *t,int y,int *cur,int *new);
 *
 * Figure out and execute line shifting
 */
extern void magic();

#endif
