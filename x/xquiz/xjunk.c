#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stddef.h>
#include "types.h"
#include "xjunk.h"

Display *dsp;
XFontStruct *sfs, *bfs;
/* Font information */
int swidth, bwidth;
int sheight, bheight;
int sdescent, bdescent;
int sascent, bascent;

Font bigfont, smallfont;
Colormap cmap;
XColor black, white, red, green, blue, yellow, magenta, cyan,
    hwhite, hred, hgreen, hblue, hyellow, hmagenta, hcyan;

int xinit()
{
	if (!(dsp = XOpenDisplay(""))) {
		printf("Couldn't open display\n");
		exit(1);
	}
	if (!(bigfont = XLoadFont(dsp, "fixed"))) {
		printf("Couldn't load font 12x24\n");
		exit(1);
	}
	if (!(smallfont = XLoadFont(dsp, "fixed"))) {
		printf("Couldn't load font\n");
		exit(1);
	}
	bfs = XQueryFont(dsp, bigfont);
	bwidth = bfs->max_bounds.width;
	bheight = bfs->ascent + bfs->descent + bfs->descent / 2;
	bdescent = bfs->descent;
	bascent = bfs->ascent + bfs->descent / 2;

	sfs = XQueryFont(dsp, smallfont);
	swidth = sfs->max_bounds.width;
	sheight = sfs->ascent + sfs->descent + sfs->descent / 2;
	sdescent = sfs->descent;
	sascent = sfs->ascent + sfs->descent / 2;

	cmap = DefaultColormap(dsp, 0);
/*
black.red=65535; black.green=65535; black.blue=65535;
XAllocColor(dsp,cmap,&black);
white.red=0; white.green=0; white.blue=0;
XAllocColor(dsp,cmap,&white);
yellow.pixel=white.pixel;
blue.pixel=black.pixel;
red.pixel=black.pixel;
hred.pixel=black.pixel;
hblue.pixel=black.pixel;
*/
	black.red = 0;
	black.green = 0;
	black.blue = 0;
	XAllocColor(dsp, cmap, &black);

	white.red = 65535;
	white.green = 65535;
	white.blue = 65535;
	XAllocColor(dsp, cmap, &white);

	red.red = 65535;
	red.green = 0;
	red.blue = 0;
	XAllocColor(dsp, cmap, &red);

	green.red = 0;
	green.green = 65535;
	green.blue = 0;
	XAllocColor(dsp, cmap, &green);

	blue.red = 0;
	blue.green = 0;
	blue.blue = 65535;
	XAllocColor(dsp, cmap, &blue);

	yellow.red = 65535;
	yellow.green = 65535;
	yellow.blue = 0;
	XAllocColor(dsp, cmap, &yellow);

	magenta.red = 65535;
	magenta.green = 0;
	magenta.blue = 65535;
	XAllocColor(dsp, cmap, &magenta);

	cyan.red = 0;
	cyan.green = 65535;
	cyan.blue = 65535;
	XAllocColor(dsp, cmap, &cyan);

	hwhite.red = 45768;
	hwhite.green = 45768;
	hwhite.blue = 45768;
	XAllocColor(dsp, cmap, &hwhite);

	hred.red = 45768;
	hred.green = 0;
	hred.blue = 0;
	XAllocColor(dsp, cmap, &hred);

	hgreen.red = 0;
	hgreen.green = 45768;
	hgreen.blue = 0;
	XAllocColor(dsp, cmap, &hgreen);

	hblue.red = 0;
	hblue.green = 0;
	hblue.blue = 45768;
	XAllocColor(dsp, cmap, &hblue);

	hyellow.red = 45768;
	hyellow.green = 45768;
	hyellow.blue = 0;
	XAllocColor(dsp, cmap, &hyellow);

	hmagenta.red = 45768;
	hmagenta.green = 0;
	hmagenta.blue = 45768;
	XAllocColor(dsp, cmap, &hmagenta);

	hcyan.red = 0;
	hcyan.green = 45768;
	hcyan.blue = 45768;
	XAllocColor(dsp, cmap, &hcyan);
}

void XPos(Window win, int *x, int *y)
{
	XWindowAttributes attr;
	int xx = 0, yy = 0;
	unsigned int ntree;
	Window root;
	Window *tree;
	Window nwin = win;
	XGetWindowAttributes(dsp, win, &attr);
	xx = attr.x;
	yy = attr.y;
	do {
		win = nwin;
		XQueryTree(dsp, win, &root, &nwin, &tree, &ntree);
		XFree(tree);
		XGetWindowAttributes(dsp, nwin, &attr);
		xx += attr.x;
		yy += attr.y;
	}
	while (nwin != RootWindow(dsp, 0));
	*x = xx;
	*y = yy;
}
