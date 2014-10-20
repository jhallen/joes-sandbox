/* Key-map handler
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

#include "config.h"
#include "termcap.h"
#include "kbd.h"

/* Create a KBD */

KBD *mkkbd(kmap)
KMAP *kmap;
 {
 KBD *kbd=(KBD *)malloc(sizeof(KBD));
 kbd->topmap=kmap;
 kbd->curmap=kmap;
 kbd->x=0;
 return kbd;
 }

/* Eliminate a KBD */

void rmkbd(k)
KBD *k;
 {
 free(k);
 }

/* Find entry position of key code */

static int kfind(map,code)
KMAP *map;
 {
 int a, b, c;
 a=0; b=c=map->len;
 while(b!=(a+c)/2)
  {
  b=(a+c)/2;
  if(map->key[b].k>=code) c=b;
  else if(map->key[b].k<code) a=b;
  }
 return b;
 }

/* Process next key for KBD */

void *dokey(kbd,key)
KBD *kbd;
 {
 int n;
 void *bind;

 /* If we're starting from scratch, clear the keymap sequence buffer */
 if(kbd->curmap==kbd->topmap) kbd->x=0;

 /* Search for key code */
 n=kfind(kbd->curmap,key<<1);

 if(n!=kbd->curmap->len && (kbd->curmap->keys[n].k&~1)==(key<<1))
  if(kbd->curmap->keys[n].k&1)
   { /* A prefix key was found */
   kbd->seq[kbd->x++]=key;
   kbd->curmap=kbd->curmap->keys[n].value.submap;
   return 0;
   }
  else
   /* A complete key sequence was entered */
   bind=kbd->curmap->keys[n].value.bind;
 else
  bind=0;
 kbd->x=0;
 kbd->curmap=kbd->topmap;
 return bind;
 }

/* X Windows key name to value table */
/* This table must be sorted */

struct xkey
 {
 char *name;
 int value;
 } xkeys[]=
 {
  { "BackSpace",	0xFF08 },
  { "Begin",		0xFF58 },
  { "Break",		0xFF6B },
  { "Cancel",		0xFF69 },
  { "Clear",		0xFF0B },
  { "Delete",		0xFFFF },
  { "Down",		0xFF54 },
  { "End",		0xFF57 },
  { "Escape",		0xFF1B },
  { "Execute",		0xFF62 },
  { "F1",		0xFFBE },
  { "F10",		0xFFC7 },
  { "F11",		0xFFC8 },
  { "F12",		0xFFC9 },
  { "F13",		0xFFCA },
  { "F14",		0xFFCB },
  { "F15",		0xFFCC },
  { "F16",		0xFFCD },
  { "F17",		0xFFCE },
  { "F18",		0xFFCF },
  { "F19",		0xFFD0 },
  { "F2",		0xFFBF },
  { "F20",		0xFFD1 },
  { "F21",		0xFFD2 },
  { "F22",		0xFFD3 },
  { "F23",		0xFFD4 },
  { "F24",		0xFFD5 },
  { "F25",		0xFFD6 },
  { "F26",		0xFFD7 },
  { "F27",		0xFFD8 },
  { "F28",		0xFFD9 },
  { "F29",		0xFFDA },
  { "F3",		0xFFC0 },
  { "F30",		0xFFDB },
  { "F31",		0xFFDC },
  { "F32",		0xFFDD },
  { "F33",		0xFFDE },
  { "F34",		0xFFDF },
  { "F35",		0xFFE0 },
  { "F4",		0xFFC1 },
  { "F5",		0xFFC2 },
  { "F6",		0xFFC3 },
  { "F7",		0xFFC4 },
  { "F8",		0xFFC5 },
  { "F9",		0xFFC6 },
  { "Find",		0xFF68 },
  { "Help",		0xFF6A },
  { "Home",		0xFF50 },
  { "Insert",		0xFF63 },
  { "KP_0",		0xFFB0 },
  { "KP_1",		0xFFB1 },
  { "KP_2",		0xFFB2 },
  { "KP_3",		0xFFB3 },
  { "KP_4",		0xFFB4 },
  { "KP_5",		0xFFB5 },
  { "KP_6",		0xFFB6 },
  { "KP_7",		0xFFB7 },
  { "KP_8",		0xFFB8 },
  { "KP_9",		0xFFB9 },
  { "KP_Add",		0xFFAB },
  { "KP_Begin",		0xFF9D },
  { "KP_Decimal",	0xFFAE },
  { "KP_Delete",	0xFF9F },
  { "KP_Divide",	0xFFAF },
  { "KP_Down",		0xFF99 },
  { "KP_End",		0xFF9C },
  { "KP_Enter",		0xFF8D },
  { "KP_Equal",		0xFFBD },
  { "KP_F1",		0xFF91 },
  { "KP_F2",		0xFF92 },
  { "KP_F3",		0xFF93 },
  { "KP_F4",		0xFF94 },
  { "KP_Home",		0xFF95 },
  { "KP_Insert",	0xFF9E },
  { "KP_Left",		0xFF96 },
  { "KP_Multiply",	0xFFAA },
  { "KP_Next",		0xFF9B },
  { "KP_Page_Down",	0xFF9B },
  { "KP_Page_Up",	0xFF9A },
  { "KP_Prior",		0xFF9A },
  { "KP_Right",		0xFF98 },
  { "KP_Separator",	0xFFAC },
  { "KP_Space",		0xFF80 },
  { "KP_Subtract",	0xFFAD },
  { "KP_Tab",		0xFF89 },
  { "KP_Up",		0xFF97 },
  { "L1",		0xFFC8 },
  { "L10",		0xFFD1 },
  { "L2",		0xFFC9 },
  { "L3",		0xFFCA },
  { "L4",		0xFFCB },
  { "L5",		0xFFCC },
  { "L6",		0xFFCD },
  { "L7",		0xFFCE },
  { "L8",		0xFFCF },
  { "L9",		0xFFD0 },
  { "Left",		0xFF51 },
  { "Linefeed",		0xFF0A },
  { "Menu",		0xFF67 },
  { "Mode_switch",	0xFF7E },
  { "Next",		0xFF56 },
  { "Num_Lock",		0xFF7F },
  { "Page_Down",	0xFF56 },
  { "Page_Up",		0xFF55 },
  { "Pause",		0xFF13 },
  { "Print",		0xFF61 },
  { "Prior",		0xFF55 },
  { "R1",		0xFFD2 },
  { "R10",		0xFFDB },
  { "R11",		0xFFDC },
  { "R12",		0xFFDD },
  { "R13",		0xFFDE },
  { "R14",		0xFFDF },
  { "R15",		0xFFE0 },
  { "R2",		0xFFD3 },
  { "R3",		0xFFD4 },
  { "R4",		0xFFD5 },
  { "R5",		0xFFD6 },
  { "R6",		0xFFD7 },
  { "R7",		0xFFD8 },
  { "R8",		0xFFD9 },
  { "R9",		0xFFDA },
  { "Redo",		0xFF66 },
  { "Return",		0xFF0D },
  { "Right",		0xFF53 },
  { "SP",		0x20   },
  { "Scroll_Lock",	0xFF14 },
  { "Select",		0xFF60 },
  { "Sp",		0x20   },
  { "Sys_Req",		0xFF15 },
  { "Tab",		0xFF09 },
  { "Undo",		0xFF65 },
  { "Up",		0xFF52 },
  { "script_switch",	0xFF7E },
  { "sp",		0x20   }
 };

/* Search for X key value */

static int xkfind(s)
char *s;
 {
 int a, b, c;
 a=0; b=c=sizeof(xkeys)/sizeof(xkey);
 while(b!=(a+c)/2)
  {
  b=(a+c)/2;
  switch(zcmp(xkeys[b].name,s))
   {
   case  1: c=b; break;
   case  0: return xkeys[b].value;
   case -1: a=b; break;
   }
  }
 return -1;
 }

/* Return key code for key name or -1 for syntax error */

static int keyval(s)
unsigned char *s;
 {
 int n;
 if(bicmp(s,"A-") && (n=keyval(s+2)!=-1)) return n+Alt;
 else if(bicmp(s,"C-") && (n=keyval(s+2)!=-1)) return n+Ctrl;
 else if(bicmp(s,"S-") && (n=keyval(s+2)!=-1)) return n+Shift;
 else if(s[0]=='^' && s[1] && !s[2])
  if(s[1]=='?') return 127;
  else return s[1]&0x1F;
 else if((n=xkfind(s))!=-1) return n;
 else if(s[1] || !s[0]) return -1;
 else return s[0];
 }

/* Create an empty keymap */

KMAP *mkkmap(dup,rm,obj)
void *(*dup)();
void (*rm)();
void *obj;
 {
 KMAP *kmap=(KMAP *)malloc(sizeof(KMAP));
 kmap->dup=dup;
 kmap->rm=rm;
 kmap->obj=obj;
 kmap->len=0;
 kmap->siz=1;
 kmap->keys=(KEY *)malloc(sizeof(KEY)*kmap->siz);
 return kmap;
 }

/* Eliminate a keymap */

void rmkmap(kmap)
KMAP *kmap;
 {
 int x;
 if(!kmap) return;
 for(x=0;x!=kmap->len;++x)
  if(kmap->keys[x].k&1)
   rmkmap(kmap->keys[x].value.submap);
  else if(kmap->rm)
   kmap->rm(kmap->obj,kmap->keys[x].value.bind);
 free(kmap->keys);
 free(kmap);
 }

/* Parse a range */

static char *range(seq,vv,ww)
char *seq;
int *vv, *ww;
 {
 char c;
 int x, v, w;
 for(x=0;seq[x] && seq[x]!=' ';++x);	/* Skip to a space */
 c=seq[x]; seq[x]=0;			/* Zero terminate the string */
 w=v=keyval(seq);			/* Get key */
 if(w<0) return 0;
 seq[x]=c;				/* Restore the space or 0 */
 for(seq+=x;*seq==' ';++seq);		/* Skip over spaces */
 /* Check for 'TO ' */
 if(!bicmp(seq,"TO") && seq[2]==' ')
  {
  for(seq+=3;*seq==' ';++seq);		/* Skip over spaces */
  for(x=0;seq[x] && seq[x]!=' ';++x);	/* Skip to space */
  c=seq[x]; seq[x]=0;			/* Zero terminate the string */
  w=keyval(seq);			/* Get key */
  if(w<0) return 0;
  seq[x]=c;				/* Restore the space or 0 */
  for(seq+=x;*seq==' ';++seq);		/* Skip over spaces */
  }
 if(v>w) return 0;
 *vv=v; *ww=w;
 return seq;
 }

/* Delete binding at a specific entry */

static void kzap(map,n)
KMAP *map;
 {
 if(map->keys[n].k&1) rmkmap(map->keys[n].value.submap);
 }

/* Delete a range of bindings */

static void krm(map,from,to)
KMAP *map;
 {
 int x;
 for(x=from;x!=to;++x) kzap(map,x);
 mcpy(map->keys+from,map->keys+to,(map->len-to)*sizeof(KEY));
 map->len-=to-from;
 }

/* Add a single binding */

static void kbind(map,k,val)
KMAP *map;
void *val;
 {
 int n=kfind(kmap,val&~1);
 if(n<map->len && (map->key[n].k&~1)==(k&~1))
  kzap(map,n);
 else
  {
  if(map->len==map->siz)
   map->key=realloc(map->key,(map->siz*=2)*sizeof(KEY));
  mcpy(map->key+n+1,map->key+n,(map->len-n)*sizeof(KEY));
  }
 map->key[n].k=k;
 map->key[n].v.bind=val;
 }

/* Add a binding to a keymap */

static CAP *cap;		/* Termcap entry */
static unsigned char *seq;	/* Zero terminated key sequence */
static unsigned char *capseq;	/* Termcap key sequence */
static int seql;		/* Termcap sequency length */
static int err;			/* Error return */
static void *bind;		/* Value to bind to sequence */

static void getcapseq()
 {
 int x, c;
 char *s;
 /* Skip to end of termcap entry name */
 for(x=0;seq[x] && seq[x]!=' ';++x);
 /* Zero terminate it */
 c=seq[x]; seq[x]=0;
#ifdef __MSDOS__
 /* Check for built-in MS-DOS key sequences */
 /* Should be a better way to do this: may have special MS-DOS termcap
    entry? */
 if(!zcmp(seq+1,"ku")) capseq="\0H", seql=2;
 else if(!zcmp(seq+1,"kd")) capseq="\0P", seql=2;
 else if(!zcmp(seq+1,"kl")) capseq="\0K", seql=2;
 else if(!zcmp(seq+1,"kr")) capseq="\0M", seql=2;
 else if(!zcmp(seq+1,"kI")) capseq="\0R", seql=2;
 else if(!zcmp(seq+1,"kD")) capseq="\0S", seql=2;
 else if(!zcmp(seq+1,"kh")) capseq="\0G", seql=2;
 else if(!zcmp(seq+1,"kH")) capseq="\0O", seql=2;
 else if(!zcmp(seq+1,"kP")) capseq="\0I", seql=2;
 else if(!zcmp(seq+1,"kN")) capseq="\0Q", seql=2;
 else if(!zcmp(seq+1,"k1")) capseq="\0;", seql=2;
 else if(!zcmp(seq+1,"k2")) capseq="\0<", seql=2;
 else if(!zcmp(seq+1,"k3")) capseq="\0=", seql=2;
 else if(!zcmp(seq+1,"k4")) capseq="\0>", seql=2;
 else if(!zcmp(seq+1,"k5")) capseq="\0?", seql=2;
 else if(!zcmp(seq+1,"k6")) capseq="\0@", seql=2;
 else if(!zcmp(seq+1,"k7")) capseq="\0A", seql=2;
 else if(!zcmp(seq+1,"k8")) capseq="\0B", seql=2;
 else if(!zcmp(seq+1,"k9")) capseq="\0C", seql=2;
 else if(!zcmp(seq+1,"k0")) capseq="\0D", seql=2;
 /* Restore original string */
 seq[x]=c;
 /* Skip over termcap key sequence and spaces which follow it */
 if(seql) for(seq+=x;*seq==' ';++seq);
#else
 /* Look up termcap key sequency */
 s=jgetstr(cap,seq+1);
 /* Restore original string */
 seq[x]=c;
 /* Retrieve key sequence from termcap entry */
 if(s && (s=tcompile(cap,s)) && (sLEN(s)>1 || s[0]<0))
  {
  capseq=s;
  seql=sLEN(s);
  /* Skip over .xxx and spaces which follow it */
  for(seq+=x;*seq==' ';++seq);
  }
#endif
 else
  err= -2;
 }

static KMAP *dobuild(kmap)
KMAP *kmap;
 {
 int v, w;

 /* If we're not processing a termcap sequence and there's a termcap entry name
    in the key sequence */ 
 if(!seql && seq[0]=='.' && seq[1]) getcapseq();

 if(err) return kmap;

 if(seql)
  { /* If we're doing a termcap sequence, get next character from it */
  v=w= (unsigned char)*capseq++;
  --seql;
  }
 else
  { /* Otherwise parse next range specified at input */
  seq=range(seq,&v,&w);
  if(!seq)
   {
   err= -1;
   return kmap;
   }
  }

 /* Make bindings between v and w */
 while(v<=w)
  {
  int n=kfind(kmap,v);
  if(*seq || seql)
   {
   if((kmap->keys[n].k>>1)==v)
    {
    if(kmap->keys[v].value.bind && kmap->rm)
     kmap->rm(kmap->obj,kmap->keys[v].value.bind);
    kmap->keys[v].value.submap=mkkmap(kmap->dup,kmap->rm,kmap->obj);
    }
   kmap->keys[n].k=((v<<1)|1);
   kmap->keys[v].value.submap=dobuild(kmap->keys[v].value.bind);
   }
  else
   {
   if(kmap->keys[v].k==0 && kmap->keys[v].value.bind && kmap->rm)
    kmap->rm(kmap->obj,kmap->keys[v].value.bind);
   if(kmap->keys[v].k==1) rmkmap(kmap->keys[v].value.submap);
   kmap->keys[v].k=0;
   kmap->keys[v].value.bind=
    /* This bit of code sticks the key value in the macro */
    ((v==w||!kmap->dup)?bind:kmap->dup(kmap->obj,bind));
   }
  ++v;
  }
 return kmap;
 }

int kadd(kmap,termcap,keyseq,value)
KMAP *kmap;
CAP *termcap;
char *keyseq;
void *value;
 {
 err=0;
 capseq=0;
 seql=0;
 cap=termcap;
 bind=value;
 seq=keyseq;
 dobuild(kmap);
 return err;
 }

void kcpy(dest,src)
KMAP *dest, *src;
 {
 int x;
 for(x=0;x!=KEYS;++x)
  if(src->keys[x].k==1)
   {
   if(dest->keys[x].k!=1)
    {
    if(dest->keys[x].value.bind && dest->rm)
     dest->rm(dest->obj,dest->keys[x].value.bind);
    dest->keys[x].k=1;
    dest->keys[x].value.submap=mkkmap(dest->dup,dest->rm,dest->obj);
    }
   kcpy(dest->keys[x].value.submap,src->keys[x].value.submap);
   }
  else if(src->keys[x].k==0 && src->keys[x].value.bind)
   {
   if(dest->keys[x].k==0 && dest->keys[x].value.bind &&
      dest->rm) dest->rm(dest->obj,dest->keys[x].value.bind);
   if(dest->keys[x].k==1) rmkmap(dest->keys[x].value.submap);
   if(src->dup)
    dest->keys[x].value.bind=src->dup(src->obj,src->keys[x].value.bind);
   else
    dest->keys[x].value.bind=src->keys[x].value.bind;
   dest->keys[x].k=0;
   }
 }

/* Remove a binding from a keymap */

int kdel(kmap,seq)
KMAP *kmap;
char *seq;
 {
 int err=1;
 int v, w;

 seq=range(seq,&v,&w);
 if(!seq) return -1;

 /* Clear bindings between v and w */
 while(v<=w)
  {
  if(*seq)
   {
   if(kmap->keys[v].k==1)
    {
    int r=kdel(kmap->keys[v].value.submap,seq);
    if(err!= -1) err=r;
    }
   }
  else
   {
   if(kmap->keys[v].k==0 && kmap->rm && kmap->keys[v].value.bind)
    kmap->rm(kmap->obj,kmap->keys[v].value.bind);
   if(kmap->keys[v].k==1) rmkmap(kmap->keys[v].value.submap);
   kmap->keys[v].k=0;
   kmap->keys[v].value.bind=0;
   if(err!= -1) err=0;
   }
  ++v;
  }

 return err;
 }
