/* Prompt widget */

#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stddef.h>
#include "types.h"
#include "queue.h"
#include "display.h"
#include "xjunk.h"
#include "prompt.h"

void promptshow(DSPOBJ *dspobj)
{
	PROMPT *prompt = dspobj->extend;
	char buf[512];
	strcpy(buf, prompt->prompt);
	strcat(buf, prompt->answer);
	strcat(buf, "_");
	XClearWindow(dsp, dspobj->win);
	XDrawString(dsp, dspobj->win, dspobj->gc, 0, prompt->y, buf,
		    strlen(buf));
}

void promptclr(DSPOBJ *dspobj)
{
	PROMPT *prompt = dspobj->extend;
	prompt->cursor = 0;
	prompt->answer[0] = 0;
	promptshow(dspobj);
}

void promptforce(DSPOBJ *dspobj, char *s)
{
	PROMPT *prompt = dspobj->extend;
	prompt->cursor = strlen(s);
	strcpy(prompt->answer, s);
	promptshow(dspobj);
}

void rmprompt(DSPOBJ *dspobj)
{
	PROMPT *prompt = dspobj->extend;
	prompt->key->keyobj = prompt->keyobj;
	free(prompt->answer);
	free(prompt);
}

void prompttype(DSPOBJ *dspobj, char c)
{
	PROMPT *prompt = dspobj->extend;
	if ((c == 8 || c == 127) && prompt->cursor) {
		prompt->answer[--prompt->cursor] = 0;
		if (prompt->eachkey)
			prompt->eachkey(dspobj);
		promptshow(dspobj);
	} else if (c == 13) {
		if (prompt->lastkey)
			prompt->lastkey(dspobj);
	} else if (c >= 32 && c < 127) {
		prompt->answer[prompt->cursor] = c;
		prompt->answer[++prompt->cursor] = 0;
		if (prompt->eachkey)
			prompt->eachkey(dspobj);
		promptshow(dspobj);
	}
}

DSPOBJ *mkprompt(DSPOBJ *in, int y, char *prmpt, void (*eachkey)(DSPOBJ *dspobj), void (*lastkey)(DSPOBJ *dspobj), DSPOBJ *key)
{
	DSPOBJ *dspobj = dspopen(in, 0, y, in->width - 2, sheight + 1);
	PROMPT *prompt = calloc(1, sizeof(PROMPT));
	XSetWindowBackground(dsp, dspobj->win, black.pixel);
	dspobj->extend = prompt;
	dspobj->show = promptshow;
	dspobj->type = prompttype;
	dspobj->close = rmprompt;
	prompt->prompt = prmpt;
	prompt->keyobj = key->keyobj;
	prompt->key = key;
	key->keyobj = dspobj;
	prompt->eachkey = eachkey;
	prompt->lastkey = lastkey;
	prompt->y = sascent;
	prompt->cursor = 0;
	prompt->answer = malloc(256);
	prompt->answer[0] = 0;
	return dspobj;
}

DSPOBJ *mvprompt(DSPOBJ *dspobj, int y)
{
	DSPOBJ *in = dspobj->in;
	PROMPT *prompt = dspobj->extend;
	dspclose(deque(dspobj));
	dspobj = dspopen(in, 0, y, in->width - 2, sheight + 1);
	XSetWindowBackground(dsp, dspobj->win, black.pixel);
	dspobj->extend = prompt;
	dspobj->show = promptshow;
	dspobj->type = prompttype;
	return dspobj;
}
