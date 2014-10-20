/* ANSI terminal simulator */

/* Primary functions */
void ttyinit(void);             /* Initialize */
void ttyout(unsigned char c);   /* Accept character */

/* Function you provide for pass-through printing */
void pp(unsigned char c);

/* Other functions */
void bioscpos(int x,int y);     /* Set cursor position using bios */
void sparms(void);              /* Determine screen address and size */

/* Scroll functions */
void scrollup(unsigned first,unsigned last,unsigned count);
void scrolldown(unsigned first,unsigned last,unsigned count);

/* Assembly language helper functions */

/* Copy in forwards direction */
void fbfwrdw(unsigned short far *dest,unsigned short far *src,unsigned size);

/* Copy in backwards direction */
void fbbkwdw(unsigned short far *dest,unsigned short far *src,unsigned size);

/* Set block of memory to word */
void fbsetw(unsigned short far *dest,unsigned size,unsigned val);

#define BLANK 0x0720            /* Blank character for eraseing */
#define MAXPARMS 4              /* Max no. numeric parameters to allow */

extern unsigned short far *screen;      /* Address of screen buffer */
extern int a6845;                       /* Port number of 6845 chip */
extern unsigned width, height;          /* Size of screen */
extern int mapplication;                /* Set for application keypad */
extern int mautowrap;                   /* Set if autowrap is on */
extern int minmargins;                  /* Set for margin relative cursor */
extern int minsert;                     /* Set for insert mode */
extern int mnl;                         /* Set if LF is NL */
extern int minvisable;                  /* Set for invisable cursor */
extern unsigned x,y;                    /* Current cursor position */
extern unsigned stop, sbot;             /* Top/Bottom margins */
extern char tabs[];                     /* Current tab stops */

/* Attributes bits (not related to video card) */
#define VTABOLD 1
#define VTAINVERSE 2
#define VTABLINK 4
#define VTAUNDERLINE 8

extern unsigned atable[];               /* Attribute bit to video OR value */
                                        /* translation table */
extern unsigned attrib;                 /* Current video OR value */
extern int vtattrib;                    /* Current attribute bits */

/* Set the cursor position quickly */

#define cpos(x,y) \
 ( \
 outportb(a6845,15), outportb(a6845+1,(y)*width+(x)), \
 outportb(a6845,14), outportb(a6845+1,((y)*width+(x))>>8) \
 )

