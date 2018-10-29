int xinit();
extern Display *dsp;
extern Font bigfont, smallfont;
extern XColor black, white, red, green, blue, yellow, magenta, cyan,
    hwhite, hred, hgreen, hblue, hyellow, hmagenta, hcyan;
extern XFontStruct *sfs, *bfs;
extern int bwidth, bheight, bdescent, bascent;
extern int swidth, sheight, sdescent, sascent;
void XPos(Window win, int *x, int *y);
