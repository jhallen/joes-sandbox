/* User application */

#include "scrn.h"
#include "blocks.h"
#include "cmd.h"
#include "kbd.h"
#include "rc.h"
#include "main.h"

/* Current cursor position */

int cursorx=0;
int cursory=0;
int *screen;
KBD *curkbd=0;

/* Initialize user application */

extern int beep;

void izuser(scrn)
SCRN *scrn;
 {
 screen=(int *)malloc(scrn->co*scrn->li*sizeof(int));
 msetI(screen,' ',scrn->li*scrn->co);
 curkbd=mkkbd(getcontext("test"));
 beep=1;
 }

/* Update the screen */

void update(scrn)
SCRN *scrn;
 {
 int y, x;
 for(y=0;y!=scrn->li;++y)
  for(x=0;x!=scrn->co-1;++x)
   if(screen[y*scrn->co+x]!=scrn->scrn[y*scrn->co+x])
    outatr(scrn,x,y,scrn->scrn[y*scrn->co+x]=screen[y*scrn->co+x]);
 cpos(scrn,cursorx,cursory);
 }

/* User commands */

int uuparw(scrn)
SCRN *scrn;
 {
 if(cursory)
  {
  --cursory;
  return 0;
  }
 else return -1;
 }

int udnarw(scrn)
SCRN *scrn;
 {
 if(cursory!=scrn->li-1)
  {
  ++cursory;
  return 0;
  }
 else return -1;
 }

int ultarw(scrn)
SCRN *scrn;
 {
 if(cursorx)
  {
  --cursorx;
  return 0;
  }
 else return -1;
 }

int urtarw(scrn)
SCRN *scrn;
 {
 if(cursorx!=scrn->co-2)
  {
  ++cursorx;
  return 0;
  }
 else return -1;
 }

int utype(scrn,c)
SCRN *scrn;
 {
 if(cursorx!=scrn->co-2)
  {
  screen[cursorx+cursory*scrn->co]=c;
  ++cursorx;
  return 0;
  }
 else return -1;
 }

/* Quit the application */

int uquit(scrn)
SCRN *scrn;
 {
 leave=1;
 exmsg="All finished.";
 return 0;
 }

/* Shell escape */

int ushell(scrn)
SCRN *scrn;
 {
 nescape(scrn);
 ttsusp();
 nreturn(scrn);
 return 0;
 }

/* Refresh the screen */

int urefresh(scrn)
SCRN *scrn;
 {
 nredraw(scrn);
 return 0;
 }

/* Table of user commands */

CMD cmds[]=
 {
   { "uparw",   0, uuparw,   0,	1, "dnarw" },
   { "dnarw",   0, udnarw,   0,	1, "uparw" },
   { "rtarw",   0, urtarw,   0,	1, "ltarw" },
   { "ltarw",   0, ultarw,   0,	1, "rtarw" },
   { "type",    0, utype,    0,	0, 0 },
   { "quit",    0, uquit,    0,	0, 0 },
   { "shell",   0, ushell,   0,	0, 0 },
   { "refresh", 0, urefresh, 0,	0, 0 },
   { 0, 0, 0, 0, 0 }
 };
