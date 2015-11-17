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
#include "graph.h"

void rmgraph(DSPOBJ *dspobj)
{
	DSPOBJ *gd = dspobj->top;
	GRAPH *graph = gd->extend;
	discard(graph->eqn);
	free(graph->vals);
	free(graph->vnames);
	free(graph->vars);
	free(graph);
}

void gbye(DSPOBJ *dspobj)
{
	DSPOBJ *gd = dspobj->top;
	dspclose(gd);
}

void goleft(DSPOBJ *dspobj)
{
	double amnt;
	GRAPH *graph;
	dspobj = dspobj->in;
	graph = dspobj->extend;
	amnt = (graph->right - graph->left) / 4.0;
	graph->left -= amnt;
	graph->right -= amnt;
	showgraph(dspobj);
}

void goright(DSPOBJ *dspobj)
{
	double amnt;
	GRAPH *graph;
	dspobj = dspobj->in;
	graph = dspobj->extend;
	amnt = (graph->right - graph->left) / 4.0;
	graph->left += amnt;
	graph->right += amnt;
	showgraph(dspobj);
}

void goup(DSPOBJ *dspobj)
{
	double amnt;
	GRAPH *graph;
	dspobj = dspobj->in;
	graph = dspobj->extend;
	amnt = (graph->top - graph->bottom) / 4.0;
	graph->top += amnt;
	graph->bottom += amnt;
	showgraph(dspobj);
}

void godown(DSPOBJ *dspobj)
{
	double amnt;
	GRAPH *graph;
	dspobj = dspobj->in;
	graph = dspobj->extend;
	amnt = (graph->top - graph->bottom) / 4.0;
	graph->top -= amnt;
	graph->bottom -= amnt;
	showgraph(dspobj);
}

void setpoints1(DSPOBJ *dspobj)
{
	int amnt;
	PROMPT *prompt = dspobj->extend;
	DSPOBJ *gwin = dspobj->in;
	GRAPH *graph = gwin->extend;
	sscanf(prompt->answer, "%d", &amnt);
	if (amnt < 1)
		return;
	graph->npoints = amnt;
	dspclose(dspobj);
	showgraph(gwin);
}

void setpoints(DSPOBJ *dspobj)
{
	GRAPH *graph;
	DSPOBJ *prompt;
	dspobj = dspobj->in;
	graph = dspobj->extend;
	prompt =
	    mkprompt(dspobj, dspobj->height - (bheight + 3),
		     "Number of points to graph: ", 0, setpoints1, dspobj);
}

void setxvar(DSPOBJ *dspobj, int n)
{
	GRAPH *graph;
	dspobj = dspobj->in;
	graph = dspobj->extend;
	graph->xvar = graph->vars[n];
	showgraph(dspobj);
}

char bff[128];

void setconst1(DSPOBJ *dspobj)
{
	double amnt;
	PROMPT *prompt = dspobj->extend;
	DSPOBJ *gwin = dspobj->in;
	GRAPH *graph = gwin->extend;
	sscanf(prompt->answer, "%lg", &amnt);
	graph->vals[graph->nn] = amnt;
	dspclose(dspobj);
	showgraph(gwin);
}

void setconst(DSPOBJ *dspobj, int n)
{
	GRAPH *graph;
	dspobj = dspobj->in;
	graph = dspobj->extend;
	sprintf(bff, "Value for constant %s:", graph->vnames[n]);
	graph->nn = n;
	mkprompt(dspobj, dspobj->height - (bheight + 3), bff, 0, setconst1,
		 dspobj);
}

void goreset(DSPOBJ *dspobj)
{
	double amnt;
	GRAPH *graph;
	dspobj = dspobj->in;
	graph = dspobj->extend;
	amnt = (graph->top - graph->bottom);
	graph->top = amnt / 2.0;
	graph->bottom = -amnt / 2.0;
	amnt = (graph->right - graph->left);
	graph->left = -amnt / 2.0;
	graph->right = amnt / 2.0;
	showgraph(dspobj);
}

char buf[128];

char *gtext = "\
Help for graphing  (click on the help screen to eliminate it)\n\
\n\
    *  Click on the UP LEFT DOWN and RIGHT buttons to pan to\n\
       different parts of the graph\n\
\n\
    *  Click on CENTER to position the window back on the origen\n\
\n\
    *  Click with the left button to zoom in\n\
\n\
    *  Click on the right button to zoom out\n\
\n\
    *  Click on LINE to have points connected\n\
\n\
    *  Click on POINTS to select the number of points to graph\n\
\n\
    *  Click on X-VAR to select which variable of the equation should\n\
       be the X-AXIS\n\
\n\
    *  Click on CONST to set the value of other variables in the\n\
       equation\n\
";

void showgraph(DSPOBJ *dspobj)
{
	int xaxis;
	int yaxis;
	int ox, oy, flg = 0;
	int x, w;
	GRAPH *graph = dspobj->extend;
	XClearWindow(dsp, dspobj->win);
/* Bind variables */
	for (x = 0; x != graph->nvars; x++) {
		if (graph->vars[x]->bind)
			discard(graph->vars[x]->bind);
		graph->vars[x]->bind = newnum();
		graph->vars[x]->bind->n = graph->vals[x];
	}

/* Generate graph */

	if (graph->xvar)
		for (graph->xvar->bind->n = graph->left;
		     graph->xvar->bind->n < graph->right;
		     graph->xvar->bind->n +=
		     (graph->right - graph->left) / graph->npoints) {
			double y;
			ferr = 0;
			y = ev(graph->eqn);
			if (!ferr) {
				if (graph->line) {
					int xx, yy;
					xx = (int)((double)dspobj->width *
						   (graph->xvar->bind->n -
						    graph->left) /
						   (graph->right -
						    graph->left));
					yy = (int)((double)dspobj->height *
						   (graph->top -
						    y) / (graph->top -
							  graph->bottom));
					if (!flg) {
						if (xx < dspobj->width
						    && xx >= 0
						    && yy < dspobj->height
						    && yy >= 0) {
							XDrawPoint(dsp,
								   dspobj->win,
								   dspobj->gc,
								   ox = xx, oy =
								   yy);
							flg = 1;
						}
					} else {
						if (xx < dspobj->width
						    && xx >= 0
						    && yy < dspobj->height
						    && yy >= 0) {
							XDrawLine(dsp,
								  dspobj->win,
								  dspobj->gc,
								  ox, oy, xx,
								  yy);
							ox = xx;
							oy = yy;
						} else
							flg = 0;
					}
				} else
					XDrawPoint(dsp, dspobj->win, dspobj->gc,
						   (int)((double)dspobj->width *
							 (graph->xvar->bind->n -
							  graph->left) /
							 (graph->right -
							  graph->left)),
						   (int)((double)dspobj->
							 height * (graph->top -
								   y) /
							 (graph->top -
							  graph->bottom)));
			} else
				flg = 0;
	} else {
		double y;
		int oy;
		ferr = 0;
		y = ev(graph->eqn);
		if (!ferr) {
			oy = (int)((double)dspobj->height * (graph->top - y) /
				   (graph->top - graph->bottom));
			for (x = 0; x != dspobj->width; x++)
				XDrawPoint(dsp, dspobj->win, dspobj->gc, x, oy);
		}
	}

/* Unbind variables */
	for (x = 0; x != graph->nvars; x++)
		if (graph->vars[x]->bind)
			discard(graph->vars[x]->bind), graph->vars[x]->bind = 0;

	xaxis =
	    graph->top / (graph->top - graph->bottom) * (double)dspobj->height;
	yaxis =
	    graph->left / (graph->left - graph->right) * (double)dspobj->width;
	if (xaxis >= 0 && xaxis < dspobj->height) {
		XDrawLine(dsp, dspobj->win, dspobj->gc, 0, xaxis,
			  dspobj->width - 1, xaxis);
		sprintf(buf, "%g", graph->left);
		XDrawString(dsp, dspobj->win, dspobj->gc, 5, xaxis + sascent,
			    buf, strlen(buf));
		sprintf(buf, "%g", graph->right);
		w = XTextWidth(sfs, buf, strlen(buf));
		XDrawString(dsp, dspobj->win, dspobj->gc, dspobj->width - w - 5,
			    xaxis + sascent, buf, strlen(buf));
		if (graph->xvar) {
			w = XTextWidth(sfs, graph->xvar->s,
				       strlen(graph->xvar->s));
			XDrawString(dsp, dspobj->win, dspobj->gc,
				    dspobj->width - w - 5, xaxis - sdescent,
				    graph->xvar->s, strlen(graph->xvar->s));
		}
	} else {
		sprintf(buf, "(%g)", graph->left);
		XDrawString(dsp, dspobj->win, dspobj->gc, 5,
			    dspobj->height / 2 + sascent, buf, strlen(buf));
		sprintf(buf, "(%g)", graph->right);
		w = XTextWidth(sfs, buf, strlen(buf));
		XDrawString(dsp, dspobj->win, dspobj->gc, dspobj->width - w - 5,
			    dspobj->height / 2 + sascent, buf, strlen(buf));
		if (graph->xvar) {
			w = XTextWidth(sfs, graph->xvar->s,
				       strlen(graph->xvar->s));
			XDrawString(dsp, dspobj->win, dspobj->gc,
				    dspobj->width - w - 5,
				    dspobj->height / 2 - sdescent,
				    graph->xvar->s, strlen(graph->xvar->s));
		}
	}
	if (yaxis >= 0 && yaxis < dspobj->width) {
		XDrawLine(dsp, dspobj->win, dspobj->gc, yaxis, 0, yaxis,
			  dspobj->height - 1);
		sprintf(buf, "%g", graph->top);
		w = XTextWidth(sfs, buf, strlen(buf));
		XDrawString(dsp, dspobj->win, dspobj->gc, yaxis - w - 5,
			    sascent, buf, strlen(buf));
		sprintf(buf, "%g", graph->bottom);
		w = XTextWidth(sfs, buf, strlen(buf));
		XDrawString(dsp, dspobj->win, dspobj->gc, yaxis - w - 5,
			    dspobj->height - sdescent, buf, strlen(buf));
		XDrawString(dsp, dspobj->win, dspobj->gc, yaxis + 5, sascent,
			    "y", 1);
/*
 unparse(dspobj,yaxis+5,unheight(graph->eqn,0)-unbase(graph->eqn,0),graph->eqn,0);
*/
	} else {
		sprintf(buf, "(%g)", graph->top);
		w = XTextWidth(sfs, buf, strlen(buf));
		XDrawString(dsp, dspobj->win, dspobj->gc,
			    dspobj->width / 2 - w - 5, sascent, buf,
			    strlen(buf));
		sprintf(buf, "(%g)", graph->bottom);
		w = XTextWidth(sfs, buf, strlen(buf));
		XDrawString(dsp, dspobj->win, dspobj->gc,
			    dspobj->width / 2 - w - 5,
			    dspobj->height - sdescent, buf, strlen(buf));
		XDrawString(dsp, dspobj->win, dspobj->gc, dspobj->width / 2 + 5,
			    sascent, "y", 1);
/*
 unparse(dspobj,dspobj->width/2+5,unheight(graph->eqn,0)-unbase(graph->eqn,0),graph->eqn,0);
*/
	}
}

void gthelp(DSPOBJ *dspobj)
{
	int x, y;
	dspobj = dspobj->in;
	XPos(dspobj->win, &x, &y);
	mkhelp(0, x + 50, y + 50, 640, 400, gtext);
}

void setline(DSPOBJ *dspobj)
{
	GRAPH *graph;
	dspobj = dspobj->in;
	graph = dspobj->extend;
	graph->line = !graph->line;
	showgraph(dspobj);
}

void zoom(DSPOBJ *dspobj, XButtonEvent *ev)
{
	GRAPH *graph = dspobj->extend;
	if (ev->button == Button1) {
		double amnt = graph->top - graph->bottom, center =
		    graph->bottom + (graph->top - graph->bottom) / 2.0;
		graph->top = center + amnt / 4.0;
		graph->bottom = center - amnt / 4.0;
		amnt = graph->right - graph->left, center =
		    graph->left + (graph->right - graph->left) / 2.0;
		graph->left = center - amnt / 4.0;
		graph->right = center + amnt / 4.0;
		showgraph(dspobj);
	} else if (ev->button == Button2) {
		double amnt = graph->top - graph->bottom, center =
		    graph->bottom + (graph->top - graph->bottom) / 2.0;
		graph->top = center + amnt;
		graph->bottom = center - amnt;
		amnt = graph->right - graph->left, center =
		    graph->left + (graph->right - graph->left) / 2.0;
		graph->left = center - amnt;
		graph->right = center + amnt;
		showgraph(dspobj);
	}
}

void gtvars(DSPOBJ *dspobj, char **items, int *nitems)
{
	GRAPH *graph = dspobj->in->extend;
	*items = graph->vnames;
	*nitems = graph->nvars;
}

DSPOBJ *mkgraph(DSPOBJ *in, int xx, int y, int width, int height, LST *eqn)
{
	int x = 0;
	int z;
	DSPOBJ *t;
	DSPOBJ *dspobj;
	GRAPH *graph;
	if (!eqn)
		return;
	dspobj = dspopen(in, xx, y, width, height);
	XStoreName(dsp, dspobj->win, "Graph");
	XSetIconName(dsp, dspobj->win, "Graph");
	graph = calloc(sizeof(GRAPH), 1);
	graph->eqn = eqn;
	graph->nvars = 0;
	dspobj->press = zoom;
	dspobj->close = rmgraph;
	graph->vars = malloc(sizeof(SYM *) * 20);
	graph->vals = malloc(sizeof(double) * 20);
	for (z = 0; z != 20; z++)
		graph->vals[z] = 0.0;
	genlist(graph->eqn, graph->vars, &graph->nvars);
	graph->vnames = malloc(20 * sizeof(char *));
	for (z = 0; z != graph->nvars; z++)
		graph->vnames[z] = graph->vars[z]->s;
	graph->left = -1.0;
	graph->right = 1.0;
	graph->top = 1.0;
	graph->bottom = -1.0;
	graph->xvar = 0;
	if (graph->nvars)
		graph->xvar = graph->vars[0];
	graph->npoints = 320.0;
	dspobj->show = showgraph;
	dspobj->extend = graph;
	t = mkbutton(dspobj, x, 0, "QUIT", gbye);
	x += t->width;
	t = mkpulldown(dspobj, x, 0, "X-VAR", gtvars, setxvar);
	x += t->width;
	t = mkpulldown(dspobj, x, 0, "CONST", gtvars, setconst);
	x += t->width;
	t = mkbutton(dspobj, x, 0, "POINTS", setpoints);
	x += t->width;
	t = mkbutton(dspobj, x, 0, "LINE", setline);
	x += t->width;
	t = mkbutton(dspobj, x, 0, "HELP", gthelp);
	x += t->width;

	x = 0;
	t = mkbutton(dspobj, x, (bheight + 2) * 2, "LEFT", goleft);
	x += t->width;
	t = mkbutton(dspobj, x, (bheight + 2) * 2, "CENTER", goreset);
	x += t->width;
	t = mkbutton(dspobj, x, (bheight + 2) * 2, "RIGHT", goright);
	x += t->width;

	mkbutton(dspobj, x / 2 - (XTextWidth(bfs, "UP", 2) + bwidth + 2) / 2,
		 (bheight + 2), "UP", goup);
	mkbutton(dspobj, x / 2 - (XTextWidth(bfs, "DOWN", 4) + bwidth + 2) / 2,
		 (bheight + 2) * 3, "DOWN", godown);

	return dspobj;
}
