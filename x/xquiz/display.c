/* Display object management */

#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stddef.h>
#include "types.h"
#include "queue.h"
#include "xjunk.h"
#include "display.h"

/* The top-level display objects */
LINK dspobjs = { &dspobjs, &dspobjs };

DSPOBJ *dofind(LINK *lst, Window win)
{
	DSPOBJ *dspobj, *t;
	for (dspobj = lst->next; dspobj != lst; dspobj = dspobj->link.next)
		if (dspobj->win == win)
			return dspobj;
		else if (t = dofind(&dspobj->children, win))
			return t;
	return 0;
}

DSPOBJ *dspfind(Window win)
{
	return dofind(&dspobjs, win);
}

void doshw(LINK *lst)
{
	DSPOBJ *dspobj;
	for (dspobj = lst->next; dspobj != lst; dspobj = dspobj->link.next) {
		if (dspobj->show)
			dspobj->show(dspobj);
		doshw(&dspobj->children);
	}
}

void dspshw()
{
	doshw(&dspobjs);
}

DSPOBJ *dspopen(DSPOBJ *in, int x, int y, int width, int height)
{
	DSPOBJ *dspobj = calloc(1, sizeof(DSPOBJ));
	XSizeHints hints;
	XWMHints wmhints;
	XSetWindowAttributes attributes;
	attributes.background_pixel = hblue.pixel;
	attributes.override_redirect = 0;
	attributes.border_pixel = white.pixel;
	if (in) {
		dspobj->top = in->top;
		dspobj->win =
		    XCreateSimpleWindow(dsp, in->win, x, y, width, height, 1,
					white.pixel, hblue.pixel);
	} else {
		dspobj->top = dspobj;
		dspobj->win =
		    XCreateWindow(dsp, RootWindow(dsp, 0), x, y, width, height,
				  1, CopyFromParent, InputOutput,
				  CopyFromParent,
				  CWBorderPixel | CWBackPixel |
				  CWOverrideRedirect, &attributes);
		/*
		   dspobj->win=XCreateSimpleWindow(dsp,RootWindow(dsp,0),x,y,width,height,0,0,
		   hblue.pixel);
		 */
	}
	XSelectInput(dsp, dspobj->win, ButtonMotionMask | ButtonPressMask |
		     ButtonReleaseMask | EnterWindowMask | LeaveWindowMask |
		     ExposureMask | KeyPressMask | PointerMotionMask);
	dspobj->in = in;
	izqueue(&dspobj->children);

	dspobj->x = x;
	dspobj->y = y;
	dspobj->width = width;
	dspobj->height = height;

	wmhints.initial_state = NormalState;
	wmhints.flags = StateHint;
	XSetWMHints(dsp, dspobj->win, &wmhints);

	hints.x = x;
	hints.y = y;
	hints.width = width;
	hints.height = height;
	hints.flags = USPosition | USSize;
	XSetNormalHints(dsp, dspobj->win, &hints);

	XMapRaised(dsp, dspobj->win);
	dspobj->gc = XCreateGC(dsp, dspobj->win, 0, 0);
	XSetForeground(dsp, dspobj->gc, yellow.pixel);
	XSetFont(dsp, dspobj->gc, smallfont);
	if (in)
		return enquefront(&in->children, dspobj);
	else
		return enquefront(&dspobjs, dspobj);
}

void dspclose(DSPOBJ *dspobj)
{
	while (dspobj->children.next != &dspobj->children)
		dspclose(dspobj->children.next);
	if (dspobj->close)
		dspobj->close(dspobj);
	XDestroyWindow(dsp, dspobj->win);
	free(deque(dspobj));
}
