/* Button widget */

#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stddef.h>
#include "types.h"
#include "queue.h"
#include "display.h"
#include "xjunk.h"
#include "button.h"

void buttonshow(DSPOBJ *dspobj)
{
	BUTTON *button = dspobj->extend;
	XClearWindow(dsp, dspobj->win);
	XDrawString(dsp, dspobj->win, dspobj->gc, button->x, button->y, button->name, strlen(button->name));
}

void rmbutton(DSPOBJ *dspobj)
{
	BUTTON *button = dspobj->extend;
	free(button);
}

/* Create a button widget
     in: window widget is in
      x: x position of widget
      y: y position of widget
   name: name to display in button
   func: function to call when button is pressed
*/

DSPOBJ *mkbutton(DSPOBJ *in, int x, int y, char *name, void (*func)(DSPOBJ *obj, XButtonEvent *ev))
{
	int width = XTextWidth(bfs, name, strlen(name)) + bwidth;
	DSPOBJ *dspobj = dspopen(in, x, y, width, bheight + 1);
	BUTTON *button = calloc(1, sizeof(BUTTON));
	XSetWindowBackground(dsp, dspobj->win, hgreen.pixel);
	XSetFont(dsp, dspobj->gc, bigfont);
	dspobj->extend = button;
	dspobj->show = buttonshow;
	dspobj->press = func;
	dspobj->close = rmbutton;
	button->name = name;
	button->y = bascent;
	button->x = bwidth / 2;
	return dspobj;
}
