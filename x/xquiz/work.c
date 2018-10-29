#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <math.h>
#include <stddef.h>
#include "types.h"
#include "queue.h"
#include "xjunk.h"
#include "display.h"
#include "msg.h"
#include "box.h"
#include "scan.h"
#include "parse.h"
#include "unparse.h"
#include "builtin.h"
#include "sym.h"
#include "prompt.h"
#include "main.h"
#include "quiz.h"
#include "button.h"
#include "pulldown.h"
#include "graph.h"
#include "simplify.h"
#include "work.h"

LST **sel = 0;
WORK *selwork = 0;

void rmwork(DSPOBJ *gd)
{
	WORK *work = gd->extend;
	struct undo *redo;
	discard(work->eqn);
	if (work->undo)
		redo = work->undo->next;
	else
		redo = 0;
	while (work->undo) {
		struct undo *u = work->undo;
		work->undo = u->prev;
		discard(u->eqn);
		free(u);
	}
	while (redo) {
		struct undo *u = redo;
		redo = u->next;
		discard(u->eqn);
		free(u);
	}
	free(work->sname);
	free(work->vars);
	free(work->vnames);
	free(work);
}

void wbye(DSPOBJ *dspobj, XButtonEvent *ev)
{
	dspclose(dspobj->top);
}

void wdone(DSPOBJ *dspobj, XButtonEvent *ev)
{
	WORK *work = dspobj->top->extend;
	if (prompt) {
		char buf[1024];
		char *s = buf;
		unparsetext(&s, work->eqn, 0);
		*s = 0;
		promptforce(prompt, buf);
		{
			PROMPT *p = prompt->extend;
			LST *ll = parseeqn(p->answer);
			ans = ll;
			XClearArea(dsp, top->win, 0, cury + 1, top->width,
				   top->height - (bheight + 4) - sheight -
				   (cury + 1), 0);
			if (ans)
				unparse(top, 20,
					top->height - unbase(ans,
							     0) - (bheight +
								   4) - sheight,
					&ans, 0, 0);
		}
		dspclose(dspobj->top);
	}
}

void showwork(DSPOBJ *dspobj)
{
	WORK *work = dspobj->extend;
	work->cury = bheight + 2 + sheight;
	XClearWindow(dsp, dspobj->win);
	unparse(dspobj, 30,
		work->cury + unheight(work->eqn, 0) - unbase(work->eqn,
							     0) - sheight,
		&work->eqn, sel, 0);
	work->cury += unheight(work->eqn, 0);
	work->cury -= sheight - 1;
	XDrawLine(dsp, dspobj->win, dspobj->gc, 0, work->cury,
		  dspobj->width - 1, work->cury);
}

/* Undo system */

void wundo(DSPOBJ *dspobj, XButtonEvent *ev)
{
	WORK *work = (dspobj = dspobj->top)->extend;
	if (work->undo)
		if (work->undo->prev) {
			sel = 0;
			work->undo = work->undo->prev;
			discard(work->eqn);
			work->eqn = dup(work->undo->eqn);
			showwork(dspobj);
		}
}

void wredo(DSPOBJ *dspobj, XButtonEvent *ev)
{
	WORK *work = (dspobj = dspobj->top)->extend;
	if (work->undo)
		if (work->undo->next) {
			sel = 0;
			discard(work->eqn);
			work->undo = work->undo->next;
			work->eqn = dup(work->undo->eqn);
			showwork(dspobj);
		}
}

void undomark(WORK *work, LST *eqn)
{
	struct undo *u = calloc(sizeof(struct undo), 1);
	u->eqn = dup(eqn);
	u->prev = work->undo;
	if (work->undo) {
		struct undo *v, *t;
		for (v = work->undo->next; v; v = t) {
			discard(v->eqn);
			t = v->next;
			free(v);
		}
		work->undo->next = u;
	}
	work->undo = u;
}

void wenter(DSPOBJ *prmt)
{
	PROMPT *prompt = prmt->extend;
	DSPOBJ *dspobj = prmt->in;
	WORK *work = dspobj->extend;
	LST *n = parseeqn(prompt->answer);
	if (err == 1) {
		prompt->answer[--prompt->cursor] = 0;
		XBell(dsp, 100);
	} else {
		work->ent = n;
		XClearArea(dsp, dspobj->win, 0, work->cury + 1, dspobj->width,
			   dspobj->height - (bheight + 4) - sheight -
			   (work->cury + 1), 0);
		if (n)
			unparse(dspobj, 20,
				dspobj->height - unbase(n,
							0) - (bheight + 4) -
				sheight, &n, 0, 0);
	}
}
void wnew(DSPOBJ *dspobj);

void wnew1(DSPOBJ *prmt)
{
	PROMPT *prompt = prmt->extend;
	DSPOBJ *dspobj = prmt->top;
	WORK *work = dspobj->extend;
	if (work->ent) {
		LST *tmp;
		tmp = subst(work->ent, ydot, work->eqn);
		if (sel && work == selwork) {
			discard(*sel);
			*sel = tmp;
		} else {
			discard(work->eqn);
			work->eqn = tmp;
		}
		undomark(work, work->eqn);
	}
	dspclose(prmt);
	wnew(dspobj);
	showwork(dspobj);
}

void wnew(DSPOBJ *dspobj)
{
	WORK *work = (dspobj = dspobj->top)->extend;
	DSPOBJ *prompt;
	prompt =
	    mkprompt(dspobj, dspobj->height - (bheight + 3), "Replace>", wenter,
		     wnew1, dspobj);
}

void wsimp(DSPOBJ *dspobj, XButtonEvent *ev)
{
	WORK *work = (dspobj = dspobj->in)->extend;
	if (sel && work == selwork)
		*sel = simplify(*sel, 1, 0);
	else
		work->eqn = simplify(work->eqn, 1, 0);
	undomark(work, work->eqn);
	showwork(dspobj);
}

void wdist(DSPOBJ *dspobj, XButtonEvent *ev)
{
	WORK *work = (dspobj = dspobj->in)->extend;
	if (sel && work == selwork)
		*sel = distribute(*sel);
	else {
		work->eqn = distribute(work->eqn);
		show(work->eqn);
		printf("\n");
	}
	undomark(work, work->eqn);
	showwork(dspobj);
}

void wfactor(DSPOBJ *dspobj, XButtonEvent *ev)
{
	WORK *work = (dspobj = dspobj->in)->extend;
	if (sel && work == selwork)
		*sel = factor(*sel);
	else
		work->eqn = factor(work->eqn);
	undomark(work, work->eqn);
	showwork(dspobj);
}

void wsubst1(DSPOBJ *prmt)
{
	PROMPT *prompt = prmt->extend;
	DSPOBJ *dspobj = prmt->in;
	WORK *work = dspobj->extend;
	if (work->ent) {
		if (sel && work == selwork)
			*sel = subst(*sel, work->vars[work->which], work->ent);
		else
			work->eqn =
			    subst(work->eqn, work->vars[work->which],
				  work->ent);
		undomark(work, work->eqn);
	}
	dspclose(prmt);
	showwork(dspobj);
}

void wsubst(DSPOBJ *dspobj, int n)
{
	WORK *work = (dspobj = dspobj->in)->extend;
	DSPOBJ *prompt;
	strcpy(work->sname, work->vnames[n]);
	strcat(work->sname, "=");
	prompt =
	    mkprompt(dspobj, dspobj->height - (bheight + 3), work->sname,
		     wenter, wsubst1, dspobj);
	work->which = n;
}

void wgtvars(DSPOBJ *dspobj, char ***items, int *nitems)
{
	int z;
	WORK *work = dspobj->in->extend;
	work->nvars = 0;
	if (sel && work == selwork)
		genlist(*sel, work->vars, &work->nvars);
	else
		genlist(work->eqn, work->vars, &work->nvars);
	for (z = 0; z != work->nvars; z++)
		work->vnames[z] = work->vars[z]->s;
	*items = work->vnames;
	*nitems = work->nvars;
}

void wbutton(DSPOBJ *dspobj, XButtonPressedEvent *ev)
{
	WORK *work = dspobj->extend;
	mousex = ev->x;
	mousey = ev->y;
	sel =
	    findmouse(30,
		      bheight + 2 + unheight(work->eqn, 0) - unbase(work->eqn,
								    0),
		      &work->eqn, 0);
	selwork = work;
	showwork(dspobj);
}

void wgraph(DSPOBJ *dspobj, XButtonEvent *ev)
{
	WORK *work = dspobj->in->extend;
	if (sel && work == selwork)
		mkgraph(0, 25, 25, XDisplayWidth(dsp, 0) - 50,
			XDisplayHeight(dsp, 0) - 50, dup(*sel));
	else
		mkgraph(0, 25, 25, XDisplayWidth(dsp, 0) - 50,
			XDisplayHeight(dsp, 0) - 50, dup(work->eqn));
}

char whelptext[] = "\
Help for equation editor (click on help window to eliminate it)\n\
\n\
   * Click on a part of the equation to have the edit functions only\n\
     affect that part of it\n\
\n\
   * Click in the quiz window to transfer the selected part of\n\
     the equation to the prompt-line of the quiz window\n\
\n\
   * Type in a new equation to replace the existing one\n\
\n\
   * Click on Simplify, Distribute or Factor to simplify, distribute\n\
     (multiply through), or factor the equation\n\
\n\
   * Click on Substitute to replace a variable with typed in text\n\
\n\
   * Click on Graph to graph the equation\n\
\n\
   * Click on Undo and Redo to go through the change history\n\
\n\
   * Click on DONE to place the equation in the prompt line of the\n\
     quiz window\n\
\n\
";

void whelp(DSPOBJ *dspobj, XButtonEvent *ev)
{
	int x, y;
	dspobj = dspobj->in;
	XPos(dspobj->win, &x, &y);
	mkhelp(0, x + 50, y + 50, 640, 400, whelptext);
}

DSPOBJ *mkwork(DSPOBJ *in, int xx, int y, int width, int height, LST *eqn)
{
	int x = 0;
	int z;
	DSPOBJ *t;
	DSPOBJ *dspobj;
	WORK *work;
	if (!eqn)
		return 0;
	dspobj = dspopen(in, xx, y, width, height);
	XStoreName(dsp, dspobj->win, "Equation Editor");
	XSetIconName(dsp, dspobj->win, "EqEdit");
	dspobj->show = showwork;
	dspobj->extend = work = calloc(sizeof(WORK), 1);
	dspobj->press = wbutton;
	dspobj->close = rmwork;
	work->eqn = eqn;
	undomark(work, eqn);
	t = mkbutton(dspobj, x, 0, "QUIT", wbye);
	x += t->width;
	t = mkbutton(dspobj, x, 0, "HELP", whelp);
	x += t->width;
	t = mkbutton(dspobj, x, 0, "DONE", wdone);
	x += t->width;
/*
t=mkbutton(dspobj,x,0,"Replace",wnew);
x+=t->width;
*/

	work->sname = malloc(100);
	work->vars = malloc(sizeof(SYM *) * 20);
	work->vnames = malloc(20 * sizeof(char *));

	t = mkpulldown(dspobj, x, 0, "Substitute", wgtvars, wsubst);
	x += t->width;

	t = mkbutton(dspobj, x, 0, "Simplify", wsimp);
	x += t->width;

	t = mkbutton(dspobj, x, 0, "Factor", wfactor);
	x += t->width;
	t = mkbutton(dspobj, x, 0, "Distribute", wdist);
	x += t->width;

	t = mkbutton(dspobj, x, 0, "Undo", wundo);
	x += t->width;
	t = mkbutton(dspobj, x, 0, "Redo", wredo);
	x += t->width;

	t = mkbutton(dspobj, x, 0, "GRAPH", wgraph);

	wnew(dspobj);
	return dspobj;
}
