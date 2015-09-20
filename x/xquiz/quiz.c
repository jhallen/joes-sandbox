/* Load problems */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <math.h>
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
#include "graph.h"
#include "work.h"
#include "main.h"

DSPOBJ *top;

void mkhelp()
{
}

char buf[128];

int level = -1;			/* Level number */
int correct = 0;		/* Number of correct answers */
int incorrect = 0;		/* Number of incorrect answers */
int hints = 0;			/* Number of hints given */

DSPOBJ *prompt = 0;

LST *parseeqn(char *s)
{
	LST *n;
	TOKEN *t;
	setscan(s);
	err = 0;
	n = parse(0);
	t = gettok();
	if (t->type != tEOF)
		err = 1;
	rmtok(t);
	return n;
}

#if 0
char *strdup(char *s)
{
	return strcpy(malloc(strlen(s) + 1), s);
}
#endif

struct hint {
	struct hint *next;
	char *text;
	int bksize;
	int tp;
};

struct rndm {
	struct rndm *next;
	SYM *sy;
	I from;
	I to;
	I steps;
};

struct problem {
	struct problem *next;
	char *text;
	char *prompt;
	int bksize;
	int tp;
	int nvars;
	SYM **vars;
	LST *equation;
	struct hint *hints;
	struct rndm *rndms;
};

struct section {
	struct section *next;
	char *title;
	int nproblems;
	struct problem *problems;
};

struct section *sections = 0;
int nsections = 0;

void sload()
{
	struct section *section = 0;
	struct problem *problem;
	struct hint *hint;
	int x, c;
	int mode = 0;		/* 0=find section, 1=find problem, 2=loading problem, 3=loading hint */
	FILE *f = fopen(argv[1], "r");
	char ibuf[256];
	if (!f) {
		fprintf(stderr, "Error opening equation file\n");
		exit(1);
	}
	while (fgets(ibuf, 256, f))
		if (ibuf[0] == ':') {
			for (x = 1; ibuf[x]; ++x)
				if (ibuf[x] == ' ' || ibuf[x] == '\t'
				    || ibuf[x] == '\n')
					break;
			c = ibuf[x];
			ibuf[x] = 0;
			if (!strcmp(ibuf + 1, "section")) {
				int z;
				mode = 1;
				problem = 0;
				if (!section)
					sections = section =
					    (struct section *)
					    malloc(sizeof(struct section));
				else
					section = section->next =
					    (struct section *)
					    malloc(sizeof(struct section));
				ibuf[x] = c;
				++nsections;
				z = strlen(ibuf + x);
				if (ibuf[x + z - 1] == '\n')
					ibuf[x + z - 1] = 0;
				section->title = strdup(ibuf + x);
				section->problems = 0;
				section->nproblems = 0;
				section->next = 0;
			} else if (!strcmp(ibuf + 1, "problem")) {
				if (!section)
					printf("No section\n");
				else {
					mode = 2;
					if (!problem)
						section->problems = problem =
						    (struct problem *)
						    malloc(sizeof
							   (struct problem));
					else
						problem = problem->next =
						    (struct problem *)
						    malloc(sizeof
							   (struct problem));
					ibuf[x] = c;
					++section->nproblems;
					problem->next = 0;
					problem->text = 0;
					problem->rndms = 0;
					problem->equation = 0;
					problem->vars =
					    malloc(sizeof(SYM *) * 20);
					problem->nvars = 0;
					problem->hints = 0;
					problem->text =
					    (char *)malloc(problem->bksize =
							   256);
					problem->text[problem->tp = 0] = 0;
					hint = 0;
				}
			} else if (!strcmp(ibuf + 1, "random")) {
				if (!section || !problem)
					printf("No problem\n");
				else {
					int y;
					struct rndm *rndm =
					    calloc(1, sizeof(struct rndm));
					rndm->next = problem->rndms;
					problem->rndms = rndm;
					ibuf[x] = c;
					while (ibuf[x] == ' ')
						++x;
					if (ibuf[x]) {
						y = x;
						while (ibuf[y]
						       && ibuf[y] != ' ')
							++y;
						c = ibuf[y];
						ibuf[y] = 0;
						if (!
						    (rndm->sy =
						     lookup(ibuf + x)))
							rndm->sy =
							    add(ibuf + x);
						ibuf[x = y] = c;

						while (ibuf[x]
						       && ibuf[x] == ' ')
							++x;
						if (ibuf[x]) {
							y = x;
							while (ibuf[y]
							       && ibuf[y] !=
							       ' ')
								++y;
							c = ibuf[y];
							ibuf[y] = 0;
							sscanf(ibuf + x, "%d",
							       &rndm->from);
							ibuf[x = y] = c;
							while (ibuf[x]
							       && ibuf[x] ==
							       ' ')
								++x;
							if (ibuf[x]) {
								y = x;
								while (ibuf[y]
								       &&
								       ibuf[y]
								       != ' ')
									++y;
								c = ibuf[y];
								ibuf[y] = 0;
								sscanf(ibuf + x,
								       "%d",
								       &rndm->
								       to);
								ibuf[x = y] = c;
								while (ibuf[x]
								       &&
								       ibuf[x]
								       == ' ')
									++x;
								if (ibuf[x]) {
									y = x;
									while
									    (ibuf
									     [y]
									     &&
									     ibuf
									     [y]
									     !=
									     ' ')
										++y;
									c = ibuf
									    [y];
									ibuf[y]
									    = 0;
									sscanf
									    (ibuf
									     +
									     x,
									     "%d",
									     &rndm->
									     steps);
									ibuf[x =
									     y]
									    = c;
								}
							}
						}
					}
				}
			} else if (!strcmp(ibuf + 1, "prompt")) {
				int z;
				if (!problem)
					printf("No problem\n");
				ibuf[x] = c;
				z = strlen(ibuf);
				if (ibuf[z - 1] == '\n')
					ibuf[z - 1] = 0;
				problem->prompt =
				    strcpy(malloc(strlen(ibuf + x) + 1),
					   ibuf + x);
			} else if (!strcmp(ibuf + 1, "solution")) {
				if (!section || !problem)
					printf("No problem\n");
				else if (problem->equation)
					printf("Already have equation\n");
				else {
					ibuf[x] = c;
					problem->equation = parseeqn(ibuf + x);
				}
			} else if (!strcmp(ibuf + 1, "vars")) {
				if (!section || !problem)
					printf("No problem\n");
				else {
					SYM *s;
					int z;
					ibuf[x] = c;
 lll:
					while (ibuf[x] == ' ' || ibuf[x] == '\t'
					       || ibuf[x] == '\n')
						++x;
					z = x;
					while (ibuf[z] != ' ' && ibuf[z] != '\t'
					       && ibuf[z] != '\n'
					       && ibuf[z] != 0)
						++z;
					if (ibuf[z])
						ibuf[z++] = 0;
					if (x != z) {
						s = lookup(ibuf + x);
						if (!s)
							s = add(ibuf + x);
						problem->vars[problem->
							      nvars++] = s;
						x = z;
						goto lll;
					}
				}
			} else if (!strcmp(ibuf + 1, "hint")) {
				if (!section || !problem)
					printf("No problem\n");
				else {
					if (!hint)
						problem->hints = hint =
						    (struct hint *)
						    malloc(sizeof(struct hint));
					else
						hint = hint->next =
						    (struct hint *)
						    malloc(sizeof(struct hint));
					hint->next = 0;
					mode = 3;
					hint->text =
					    (char *)malloc(hint->bksize = 256);
					hint->text[hint->tp = 0] = 0;
				}
			} else
				printf("Unknown command '%s'\n", ibuf + 1);
		} else if (mode == 2) {
			int len = strlen(ibuf);
			if (len + problem->tp >= problem->bksize)
				problem->text =
				    (char *)realloc(problem->text,
						    problem->bksize =
						    len + problem->tp + 256);
			strcat(problem->text, ibuf);
			problem->tp = strlen(problem->text);
		} else if (mode == 3) {
			int len = strlen(ibuf);
			if (len + hint->tp >= hint->bksize)
				hint->text =
				    (char *)realloc(hint->text, hint->bksize =
						    len + hint->tp + 256);
			strcat(hint->text, ibuf);
			hint->tp = strlen(hint->text);
		} else {
			for (x = 0; ibuf[x]; ++x)
				if (ibuf[x] != ' ' && ibuf[x] != '\n'
				    && ibuf[x] != '\t') {
					printf("Syntax error \'%s\'\n", ibuf);
					break;
				}
		}
	fclose(f);
}

int nlevels;
char **levels;

struct section *cursection = 0;
struct problem *curproblem = 0;
struct hint *curhint = 0;
LST *ans = 0, *prob = 0;
int cury = 0;
char *bmsg = 0;

void showit(char *text)
{
	int x = 0;
	LST *n, *nn;
	do {
		int xx = x;
		while (text[x] && text[x] != '\n')
			++x;
		if (text[xx])
			if (text[xx] != '|') {
				XDrawString(dsp, top->win, top->gc, 0,
					    cury - sdescent, text + xx, x - xx);
				cury += sheight;
			} else {
				n = parseeqn(text + xx + 1);
				n = simplify(nn = n, 0);
				if (!prob)
					prob = n;
				unparse(top, 30,
					cury + unheight(n, 0) - unbase(n,
								       0) -
					sheight, &n, 0, 0);
				cury += unheight(n, 0);
				/*  discard(n); discard(nn); */
			}
	}
	while (text[x++]);
}

char str[100];

char *helptext = "\
Help for Xquiz (click anywhere on the help screen to eliminate it)\n\
\n\
     Click on DRILL and select a subject to be quized on\n\
\n\
     If you are having trouble with a given problem, click on\n\
     HINT to get a helpfull hint\n\
\n\
     Whenever there is an equation in the lower half of the screen,\n\
     you can click on GRAPH to generate a graph of it\n\
\n\
     Click on EDIT to get an equation editor window\n\
\n\
";

void help()
{
	int x, y;
	XPos(top->win, &x, &y);
	mkhelp(0, x + 50, y + 50, 640, 400, helptext);
}

#define sc(x) x,sizeof(x)-1

void showscrn(DSPOBJ *dspobj)
{
	struct hint *hint;
	cury = bheight + 2 + sheight;
	XClearWindow(dsp, dspobj->win);
	if (dspobj->resize) {
		if (prompt)
			prompt = mvprompt(prompt, top->height - (bheight + 3));
		dspobj->resize = 0;
	}
	if (level != -1) {
		int w;
		sprintf(str, "Hints=%d Correct=%d Incorrect=%d", hints, correct,
			incorrect);
		w = XTextWidth(sfs, str, strlen(str));
		XDrawString(dsp, top->win, top->gc, top->width - w - 10,
			    bascent, str, strlen(str));
	}
	XDrawLine(dsp, dspobj->win, dspobj->gc, 0, cury - sheight,
		  top->width - 1, cury - sheight);
	if (level != -1) {
		int w = XTextWidth(sfs, levels[level], strlen(levels[level]));
		XDrawString(dsp, top->win, top->gc, top->width / 2 - w / 2,
			    cury - sdescent, levels[level],
			    strlen(levels[level]));
		cury += sheight;
		XDrawLine(dsp, dspobj->win, dspobj->gc, 0, cury - sheight,
			  top->width - 1, cury - sheight);
	}
	if (curproblem) {
		showit(curproblem->text);
		for (hint = curproblem->hints; hint != curhint;
		     hint = hint->next)
			showit(hint->text);
	}
	if (cury != bheight + sheight + 2) {
		cury -= sheight - 1;
		XDrawLine(dsp, dspobj->win, dspobj->gc, 0, cury, top->width - 1,
			  cury);
	}
	if (ans)
		unparse(top, 20,
			dspobj->height - unbase(ans,
						0) - (bheight + 4) - sheight,
			&ans, 0, 0);
	if (bmsg) {
		int w;
		w = XTextWidth(sfs, bmsg, strlen(bmsg));
		XDrawString(dsp, top->win, top->gc, top->width / 2 - w / 2,
			    top->height - (bheight + 4) - sdescent, bmsg,
			    strlen(bmsg));
	}
}

void bm(char *s)
{
	int w;
	if (bmsg)
		XClearArea(dsp, top->win, 0,
			   top->height - (bheight + 4) - sheight, top->width,
			   sheight, 0);
	bmsg = s;
	w = XTextWidth(sfs, bmsg, strlen(bmsg));
	XDrawString(dsp, top->win, top->gc, top->width / 2 - w / 2,
		    top->height - (bheight + 4) - sdescent, bmsg, strlen(bmsg));
}

void showanswer(DSPOBJ *dspobj)
{
	PROMPT *prompt = dspobj->extend;
	LST *n = parseeqn(prompt->answer);
	if (err == 1) {
		prompt->answer[--prompt->cursor] = 0;
		XBell(dsp, 100);
	} else {
		ans = n;
		XClearArea(dsp, top->win, 0, cury + 1, top->width,
			   top->height - (bheight + 4) - sheight - (cury + 1),
			   0);
		if (ans)
			unparse(top, 20,
				top->height - unbase(ans,
						     0) - (bheight + 4) -
				sheight, &ans, 0, 0);
	}
}

int rcheck(double a, LST *b, double *rndms, int n, int *map, int m, SYM **vb)
{
	int x, y, z;
	if (!m)
		return fabs(a - ev(b)) < fabs(a / 100.0);
	for (x = 0; x != m; x++) {
		for (z = 0; map[z]; z++) ;
		for (y = 0; y != x; y++)
			while (map[++z]) ;
		map[z] = 1;
		vb[n - m]->bind = newnum();
		vb[n - m]->bind->n = rndms[z];
		if (rcheck(a, b, rndms, n, map, m - 1, vb)) {
			discard(vb[n - m]->bind);
			vb[n - m]->bind = 0;
			map[z] = 0;
			return 1;
		} else {
			discard(vb[n - m]->bind);
			vb[n - m]->bind = 0;
			map[z] = 0;
		}
	}
	return 0;
}

int check(LST *a, LST *b, SYM **sy, int sys)
{
	int x, y;
	int trys = 0;
	int na = 0;
	SYM **va = calloc(sizeof(SYM *), 20);
	int nb = 0;
	SYM **vb = calloc(sizeof(SYM *), 20);
	double *rndms = malloc(20 * sizeof(double)), eva;
	int *map = calloc(20, sizeof(int));

 loop:
/* Assign random numbers to true variables */
	for (x = 0; x != sys; x++) {
		if (sy[x]->bind)
			discard(sy[x]->bind);
		sy[x]->bind = newnum();
		sy[x]->bind->n = (double)(random() % 100) / 20.0 + .1;
	}

/* Determine what the remaining constants are */
	genlist(a, va, &na);
	genlist(b, vb, &nb);
	if (na != nb)
		goto nope;
	if (!na) {
		eva = ev(a);
		if (fabs(eva - ev(b)) > eva / 100.0) {
 nope:
			for (x = 0; x != sys; x++)
				if (sy[x]->bind)
					discard(sy[x]->bind), sy[x]->bind = 0;
			free(rndms);
			free(va);
			free(vb);
			free(map);
			return 0;
		} else {
 yep:
			if (++trys != 4)
				goto loop;
			for (x = 0; x != sys; x++)
				if (sy[x]->bind)
					discard(sy[x]->bind), sy[x]->bind = 0;
			free(rndms);
			free(va);
			free(vb);
			free(map);
			return 1;
		}
	}

	for (x = 0; x != na; x++)
		rndms[x] = (double)(random() % 100) / 20.0 + .1;
/* Bind numbers */
	for (x = 0; x != na; x++)
		va[x]->bind = newnum(), va[x]->bind->n = rndms[x];
/* Get a */
	eva = ev(a);
	for (x = 0; x != na; x++)
		discard(va[x]->bind), va[x]->bind = 0;

/* Go through combinations with b */
	if (!rcheck(eva, b, rndms, na, map, na, vb))
		goto nope;
	goto yep;
}

void ckey()
{
	++correct;
	bm("");
	newproblem();
}

void ikey(int b, char c)
{
	++incorrect;
	if (c == 't' || c == 'T') {
		bm("");
		top->keyobj = prompt;
		showscrn(top);
	} else if (c == 'n' || c == 'N') {
		newproblem();
		bm("");
	} else if (c == 's' || c == 'S') {
		ans = dup(curproblem->equation);
		ans = simplify(ans, 0);
		bm("Hit any key to continue");
		top->keyobj = top;
		top->type = ckey;
		showscrn(top);
	}
}

void checkanswer()
{
	if (ans && curproblem) {
		if (err)
			goto in;
		if (!check
		    (ans, curproblem->equation, curproblem->vars,
		     curproblem->nvars)) {
 in:
			XBell(dsp, 100);
			bm("Incorrect!  (T)ry again, (S)how answer (N)ew problem");
			top->keyobj = top;
			top->type = ikey;
		} else {
			bm("Correct!  Hit any key to continue");
			top->keyobj = top;
			top->type = ckey;
		}
	}
}

void newproblem()
{
	NUM *nn;
	SYM *sy;
	LST *n;
	int y;
	struct rndm *rndm;
	if (prob)
		discard(prob), prob = 0;
	y = random() % cursection->nproblems;
	clrbinds();
	for (curproblem = cursection->problems; y;
	     --y, curproblem = curproblem->next) ;
	curhint = curproblem->hints;
	if (prompt) {
		dspclose(prompt);
		prompt = 0;
		top->keyobj = 0;
	}
	prompt = mkprompt(top, top->height - (bheight + 3),
			  curproblem->prompt ? curproblem->prompt : "",
			  showanswer, checkanswer, top);
	for (rndm = curproblem->rndms; rndm; rndm = rndm->next) {
		I r = random() % ((rndm->to - rndm->from) * rndm->steps);
		double rr =
		    (double)r / (double)rndm->steps + (double)rndm->from;
		rndm->sy->bind = newnum();
		rndm->sy->bind->n = rr;
	}
	ans = 0;
	showscrn(top);
}

void dolevel(DSPOBJ *b, int a)
{
	level = a;
	correct = 0;
	incorrect = 0;
	hints = 0;
	for (cursection = sections; a--; cursection = cursection->next) ;
	newproblem();
}

void nexthint()
{
	if (curhint) {
		++hints;
		curhint = curhint->next;
	}
	showscrn(top);
}

char *yesno[] = { "YES", "NO" };

void byebye(int n)
{
	if (!n)
		leave = 1;
}

void bye()
{
	mkmsg(top, "Really quit?", yesno, 2, byebye);
}

/* Initial screen set-up */

XSizeHints hnts = { 0 };

void work()
{
	int x, y;
	XPos(top->win, &x, &y);
	if (ans)
		mkwork(0, x + 50, y + 50, 640, 200, dup(ans));
	else if (prob)
		mkwork(0, x + 50, y + 50, 640, 200, dup(prob));
}

int gtlevels(DSPOBJ *dspobj, char **items, int *nitems)
{
	*items = levels;
	*nitems = nlevels;
}

void graph()
{
	mkgraph(0, 25, 25, XDisplayWidth(dsp, 0) - 50,
		XDisplayHeight(dsp, 0) - 50, dup(ans));
}

void dopress()
{
	if (sel && prompt) {
		LST *ll;
		PROMPT *p = prompt->extend;
		char buf[1024];
		char *s;
		strcpy(buf, p->answer);
		s = buf + strlen(buf);
		unparsetext(&s, *sel, 0);
		*s = 0;
		ll = parseeqn(buf);
		if (err == 1)
			XBell(dsp, 100);
		else {
			promptforce(prompt, buf);
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
	}
}

int dinit()
{
	DSPOBJ *t, *u, *v, *w, *x, *y;
	{
		struct section *sect;
		int x;
		srandom(time(0));
		builtin();
		izsimp();
		sload();
		for (sect = sections, nlevels = 0; sect; sect = sect->next)
			++nlevels;
		levels = malloc(nlevels * (sizeof(char *)));
		for (sect = sections, x = 0; sect; sect = sect->next)
			levels[x++] = sect->title;
	}
	top = dspopen(0, 100, 100, 640, 400);
	top->press = dopress;
	XStoreName(dsp, top->win, "Xquiz");
	XSetIconName(dsp, top->win, "Xquiz");
	t = mkbutton(top, 0, 0, "QUIT", bye);
	u = mkbutton(top, t->width, 0, "HINT", nexthint);
	v = mkpulldown(top, t->width + u->width, 0, "DRILL", gtlevels, dolevel);
	w = mkbutton(top, t->width + u->width + v->width, 0, "GRAPH", graph);
	x = mkbutton(top, t->width + u->width + v->width + w->width, 0, "HELP",
		     help);
	y = mkbutton(top, t->width + u->width + v->width + w->width + x->width,
		     0, "EDIT", work);
	top->show = showscrn;
}
