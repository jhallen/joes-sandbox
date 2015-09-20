/* Message/query window */

#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "types.h"
#include "queue.h"
#include "display.h"
#include "xjunk.h"
#include "msg.h"
#include "main.h"

void msgshow(DSPOBJ *dspobj)
{
	MSG *msg = dspobj->extend;
	int y;
	int x;
	XClearWindow(dsp, dspobj->win);
	XDrawString(dsp, dspobj->win, dspobj->gc, msg->x1, msg->h + bascent,
		    msg->text, strlen(msg->text));
	for (y = 0, x = msg->x2; y != msg->nitems; ++y) {
		int ww = XTextWidth(bfs, msg->items[y], strlen(msg->items[y]));
		XDrawString(dsp, dspobj->win, dspobj->gc,
			    x + msg->w2 / 2 - ww / 2, bascent + msg->h * 3 + 1,
			    msg->items[y], strlen(msg->items[y]));
		XDrawLine(dsp, dspobj->win, dspobj->gc, x + 1, msg->h * 3,
			  x + msg->w2 - 2, msg->h * 3);
		XDrawLine(dsp, dspobj->win, dspobj->gc, x + msg->w2 - 2,
			  msg->h * 3, x + msg->w2 - 2, msg->h * 4 + 2);
		XDrawLine(dsp, dspobj->win, dspobj->gc, x + msg->w2 - 2,
			  msg->h * 4 + 2, x + 1, msg->h * 4 + 2);
		XDrawLine(dsp, dspobj->win, dspobj->gc, x + 1, msg->h * 4 + 2,
			  x + 1, msg->h * 3);
		x += msg->w2 * 2;
	}
}

void msgremove(DSPOBJ *dspobj)
{
	MSG *msg = dspobj->extend;
	free(msg);
}

void msgclick(DSPOBJ *dspobj, XButtonPressedEvent *ev)
{
	MSG *msg = dspobj->extend;
	int x;
	int y;
	if (ev->y < msg->h * 3 || ev->y >= msg->h * 4)
		return;
	for (y = 0, x = msg->x2; y != msg->nitems; y++, x += 2 * msg->w2)
		if (ev->x >= x && ev->x < x + msg->w2) {
			msg->func(y);
			dspobj->top->trap = 0;
			dspclose(dspobj);
		}
}

/* Create a message window */

DSPOBJ *mkmsg(DSPOBJ *in, char *text, char **items, int nitems, void (*func)(int y))
{
	int w1, w2, yy, width, w;
	int hght = bheight * 5;
	DSPOBJ *dspobj;
	MSG *msg;
	w1 = XTextWidth(bfs, text, strlen(text)) + bwidth;
	for (yy = 0, w2 = 0; yy != nitems; ++yy) {
		w = XTextWidth(bfs, items[yy], strlen(items[yy])) + bwidth;
		if (w > w2)
			w2 = w;
	}
	w = yy * w2 * 2 + w2;
	if (w1 > w)
		width = w1;
	else
		width = w;
	dspobj =
	    dspopen(in, in->width / 2 - width / 2, in->height / 2 - hght / 2,
		    width, hght);
	dspobj->top->trap = dspobj;
	msg = calloc(1, sizeof(MSG));
	XSetFont(dsp, dspobj->gc, bigfont);
	XSetWindowBackground(dsp, dspobj->win, hred.pixel);
	dspobj->show = msgshow;
	dspobj->extend = msg;
	dspobj->press = msgclick;
	dspobj->close = msgremove;
	msg->items = items;
	msg->w2 = w2;
	msg->h = bheight;
	msg->nitems = nitems;
	msg->func = func;
	msg->text = text;
	msg->x1 = width / 2 - w1 / 2;
	msg->x2 = width / 2 - (w - w2 * 2) / 2;
	return dspobj;
}
