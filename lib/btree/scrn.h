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
#include "tty.h"

extern int skiptop;

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

/* SCRN *nopen(void);
 *
 * Open the screen (sets TTY mode so that screen may be used immediatly after
 * the 'nopen').
 */
SCRN *nopen();

/* void nresize(SCRN *t,int w,int h);
 *
 * Change size of screen.  For example, call this when you find out that
 * the Xterm changed size.
 */
void nresize();

/* void nredraw(SCRN *t);
 *
 * Invalidate all state variables for the terminal.  This way, everything gets
 * redrawn.
 */
void nredraw();

void npartial();
void nescape();
void nreturn();

/* void nclose(SCRN *t);
 *
 * Close the screen and restore TTY to initial state.
 *
 * if 'flg' is set, tclose doesn't mess with the signals.
 */
void nclose();

/* int cpos(SCRN *t,int x,int y);
 *
 * Set cursor position
 */
int cpos();

/* int attr(SCRN *t,int a);
 *
 * Set attributes
 */
int attr();

/* void outatr(SCRN *t,int *scrn,int x,int y,int c,int a);
 *
 * Output a character at the given screen cooridinate.  The cursor position
 * after this function is executed is indeterminate.
 */

/* Character attribute bits */

#ifdef __MSDOS__

#define INVERSE 1
#define UNDERLINE 2
#define BOLD 4
#define BLINK 8
#define DIM 16
extern unsigned atab[];

#define outatr(t,scrn,x,y,c,a) \
  ( \
    (t), (x), (y), *(scrn)=((unsigned)(c)|atab[a]) \
  )

#else

#define INVERSE 256
#define UNDERLINE 512
#define BOLD 1024
#define BLINK 2048
#define DIM 4096

#define outatr(t,scrn,xx,yy,c,a) \
  ( \
    (*(scrn)!=((c)|(a))) ? \
      ( \
      *(scrn)=((c)|(a)), \
      ((t)->ins?clrins(t):0), \
      ((t)->x!=(xx) || (t)->y!=(yy)?cpos((t),(xx),(yy)):0), \
      ((t)->attrib!=(a)?attr((t),(a)):0), \
      ttputc(c), ++(t)->x \
      ) \
    : \
      0 \
  )

#endif

extern unsigned xlata[256];
extern unsigned char xlatc[256];
extern int dspasis;

#define xlat(a,c) \
  ( \
  (dspasis && ((unsigned)(c)>=128)) ? \
      ((a)=0) \
    : \
      (((a)=xlata[(unsigned)(c)]), ((c)=xlatc[(unsigned)(c)])) \
  )

/* int eraeol(SCRN *t,int x,int y);
 *
 * Erase from screen coordinate to end of line.
 */
int eraeol();

/* void nscrlup(SCRN *t,int top,int bot,int amnt);
 *
 * Buffered scroll request.  Request that some lines up.  'top' and 'bot'
 * indicate which lines to scroll.  'bot' is the last line to scroll + 1.
 * 'amnt' is distance in lines to scroll.
 */
void nscrlup();

/* void nscrldn(SCRN *t,int top,int bot,int amnt);
 *
 * Buffered scroll request.  Scroll some lines down.  'top' and 'bot'
 * indicate which lines to scroll.  'bot' is the last line to scroll + 1.
 * 'amnt' is distance in lines to scroll.
 */
void nscrldn();

/* void nscroll(SCRN *t);
 *
 * Execute buffered scroll requests
 */
void nscroll();

/* void magic(SCRN *t,int y,int *cur,int *new);
 *
 * Figure out and execute line shifting
 */
void magic();

int clrins();

#endif
