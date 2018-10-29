/* Pull-down menu thingy */

#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stddef.h>
#include "types.h"
#include "queue.h"
#include "display.h"
#include "xjunk.h"
#include "pulldown.h"
#include "main.h"

void subshow(DSPOBJ *dspobj)
{
	PULLDOWN *pulldown = dspobj->extend;
	int y;
	XClearWindow(dsp, dspobj->win);
	for (y = 0; y != pulldown->nitems; y++) {
		XDrawString(dsp, dspobj->win, dspobj->gc, pulldown->x,
			    y * pulldown->h + pulldown->y, pulldown->items[y],
			    strlen(pulldown->items[y]));
		if (y == pulldown->which) {
			XDrawLine(dsp, dspobj->win, dspobj->gc, 1,
				  y * pulldown->h, pulldown->w - 2,
				  y * pulldown->h);
			XDrawLine(dsp, dspobj->win, dspobj->gc, pulldown->w - 2,
				  y * pulldown->h, pulldown->w - 2,
				  (y + 1) * pulldown->h);
			XDrawLine(dsp, dspobj->win, dspobj->gc, pulldown->w - 2,
				  (y + 1) * pulldown->h, 1,
				  (y + 1) * pulldown->h);
			XDrawLine(dsp, dspobj->win, dspobj->gc, 1,
				  (y + 1) * pulldown->h, 1, y * pulldown->h);
		}
	}
}

void subremove(DSPOBJ *dspobj, XCrossingEvent *ev)
{
	PULLDOWN *pulldown = dspobj->extend;
	pulldown->sub = 0;
	dspclose(dspobj);
}

void pullshow(DSPOBJ *dspobj)
{
	PULLDOWN *pulldown = dspobj->extend;
	XClearWindow(dsp, dspobj->win);
	XDrawString(dsp, dspobj->win, dspobj->gc, pulldown->x, pulldown->y,
		    pulldown->name, strlen(pulldown->name));
}

void pullmove(DSPOBJ *dspobj, XMotionEvent *ev)
{
	PULLDOWN *pulldown = dspobj->extend;
	int which = -1;
	if (!(dspobj = pulldown->sub))
		return;
	if (ev->y >= pulldown->h + 1 && ev->y < dspobj->height + pulldown->h + 1
	    && ev->x >= 0 && ev->x < pulldown->w)
		which = (ev->y - pulldown->h - 1) / pulldown->h;
	if (which != pulldown->which) {
		int y = pulldown->which;
		/* Remove old */
		if (y != -1) {
			XSetForeground(dsp, dspobj->gc, hred.pixel);
			XDrawLine(dsp, dspobj->win, dspobj->gc, 1,
				  y * pulldown->h, pulldown->w - 2,
				  y * pulldown->h);
			XDrawLine(dsp, dspobj->win, dspobj->gc, pulldown->w - 2,
				  y * pulldown->h, pulldown->w - 2,
				  (y + 1) * pulldown->h);
			XDrawLine(dsp, dspobj->win, dspobj->gc, pulldown->w - 2,
				  (y + 1) * pulldown->h, 1,
				  (y + 1) * pulldown->h);
			XDrawLine(dsp, dspobj->win, dspobj->gc, 1,
				  (y + 1) * pulldown->h, 1, y * pulldown->h);
		}
		XSetForeground(dsp, dspobj->gc, yellow.pixel);
		y = pulldown->which = which;
		/* Put in new */
		if (y != -1) {
			XDrawLine(dsp, dspobj->win, dspobj->gc, 1,
				  y * pulldown->h, pulldown->w - 2,
				  y * pulldown->h);
			XDrawLine(dsp, dspobj->win, dspobj->gc, pulldown->w - 2,
				  y * pulldown->h, pulldown->w - 2,
				  (y + 1) * pulldown->h);
			XDrawLine(dsp, dspobj->win, dspobj->gc, pulldown->w - 2,
				  (y + 1) * pulldown->h, 1,
				  (y + 1) * pulldown->h);
			XDrawLine(dsp, dspobj->win, dspobj->gc, 1,
				  (y + 1) * pulldown->h, 1, y * pulldown->h);
		}
	}
}

void pullclick(DSPOBJ *dspobj, XButtonEvent *ev)
{
	PULLDOWN *pulldown = dspobj->extend;
	if (!pulldown->sub) {
		DSPOBJ *sub;
		int ww;
		pulldown->gtitems(dspobj, &pulldown->items, &pulldown->nitems);
		pulldown->w = 0;
		for (ww = 0; ww != pulldown->nitems; ww++) {
			int w =
			    XTextWidth(bfs, pulldown->items[ww],
				       strlen(pulldown->items[ww])) + bwidth;
			if (w > pulldown->w)
				pulldown->w = w;
		}
		sub =
		    dspopen(dspobj->in, dspobj->x, dspobj->y + pulldown->h + 2,
			    pulldown->w, pulldown->h * pulldown->nitems + 1);
		pulldown->sub = sub;
		XSetWindowBackground(dsp, sub->win, hred.pixel);
		sub->extend = pulldown;
		sub->show = subshow;
		sub->leave = subremove;
		pulldown->which = -1;
		XSetFont(dsp, sub->gc, bigfont);
		subshow(sub);
	}
}

void pullunclick(DSPOBJ *dspobj, XButtonPressedEvent *ev)
{
	PULLDOWN *pulldown = dspobj->extend;
	if (pulldown->sub) {
		if (ev->x >= 0 &&
		    ev->x < pulldown->sub->width &&
		    ev->y + dspobj->y - pulldown->sub->y >= 0 &&
		    ev->y + dspobj->y - pulldown->sub->y <
		    pulldown->sub->height)
			pulldown->select(dspobj,
					 (ev->y + dspobj->y -
					  pulldown->sub->y) / pulldown->h);
		subremove(pulldown->sub, NULL);
	}
}

void rmpulldown(DSPOBJ *dspobj)
{
	PULLDOWN *pulldown = dspobj->extend;
	free(pulldown);
}

DSPOBJ *mkpulldown(
    DSPOBJ *in,
    int x, int y,
    char *name,
    void (*gtitems)(DSPOBJ *dspobj, char ***items, int *nitems),
    void (*select)(DSPOBJ *dspobj, int n)
) {
	int width = XTextWidth(bfs, name, strlen(name)) + bwidth;
	int ww;
	DSPOBJ *dspobj = dspopen(in, x, y, width, bheight + 1);
	PULLDOWN *pulldown = calloc(1, sizeof(PULLDOWN));
	XSetFont(dsp, dspobj->gc, bigfont);
	XSetWindowBackground(dsp, dspobj->win, hred.pixel);
	dspobj->extend = pulldown;
	dspobj->show = pullshow;
	dspobj->press = pullclick;
	dspobj->release = pullunclick;
	dspobj->move = pullmove;
	dspobj->close = rmpulldown;
	pulldown->name = name;
	pulldown->y = bascent;
	pulldown->x = bwidth / 2;
	pulldown->h = bheight;
	pulldown->w = 0;
	pulldown->sub = 0;
	pulldown->select = select;
	pulldown->gtitems = gtitems;
	return dspobj;
}
