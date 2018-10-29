#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <signal.h>
#include <stddef.h>
#include <stdlib.h>
#include "types.h"
#include "queue.h"
#include "xjunk.h"
#include "display.h"
#include "quiz.h"
#include "main.h"

int leave = 0;
int argc;
char **argv;

/* Trap for floating point errors.  'ferr' gets set whenever there
 * is a floating exception
 */

int ferr;

void seterr(int x)
{
	ferr = 1;
}

int main(int a, char *b[])
{
	XComposeStatus status;
	char kbuf[2];
	XEvent ev;
	DSPOBJ *dspobj;
	if (a != 2) {
		fprintf(stderr, "xquiz: error missing file name\n");
		exit(1);
	}
	argc = a;
	argv = b;
	signal(SIGFPE, seterr);
	xinit();
	dinit();
/* dspshw(); */
	do {
		XNextEvent(dsp, &ev);
		dspobj = dspfind(ev.xany.window);
		if (!dspobj)
			continue;
		if (ev.type == Expose) {
			if (!ev.xexpose.count)
				if (dspobj->show) {
					XWindowAttributes attr;
					XGetWindowAttributes(dsp, dspobj->win,
							     &attr);
					if (dspobj->width != attr.width)
						dspobj->resize =
						    1, dspobj->width =
						    attr.width;
					if (dspobj->height != attr.height)
						dspobj->resize =
						    1, dspobj->height =
						    attr.height;
					dspobj->show(dspobj); /* , &ev);*/ 
				}
		} else
			switch (ev.type) {
			case EnterNotify:
				if (dspobj->enter)
					dspobj->enter(dspobj, (XCrossingEvent *)&ev);
				break;
			case LeaveNotify:
				if (dspobj->leave)
					dspobj->leave(dspobj, (XCrossingEvent *)&ev);
				break;
			case ButtonPress:
				if (dspobj->top->trap)
					dspobj = dspobj->top->trap;
				if (dspobj->press)
					dspobj->press(dspobj, (XButtonEvent *)&ev);
				break;
			case ButtonRelease:
				if (dspobj->release)
					dspobj->release(dspobj, (XButtonEvent *)&ev);
				break;
			case KeyPress:
				if (XLookupString((XKeyEvent *)&ev, kbuf, 1, 0, &status)) {
					if (dspobj->top->trap) {
						dspobj = dspobj->top->trap;
						if (dspobj->type)
							dspobj->type(dspobj,
								     kbuf[0]);
					} else if (dspobj->top->keyobj)
						dspobj->top->keyobj->
						    type(dspobj->top->keyobj,
							 kbuf[0]);
				}
				break;
			case MotionNotify:
				if (dspobj->move)
					dspobj->move(dspobj, (XMotionEvent *)&ev);
				break;
			}
	}
	while (!leave);
	return 0;
}
