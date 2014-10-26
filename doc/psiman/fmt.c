#include <stdio.h>
#include <stdlib.h>

#include "obj.h"
#include "out.h"
#include "fmt.h"

void rmtray(TRAY * tray)
{
	int x;
	for (x = 0; x != tray->len; ++x)
		free(tray->tray[x]);
	free(tray->tray);
	free(tray->traylen);
	free(tray->traysiz);
	free(tray);
}

TRAY *mktray()
{
	TRAY *tray = (TRAY *) malloc(sizeof(TRAY));
	tray->len = 0;
	tray->siz = 10;
	tray->traysiz = (int *) malloc(sizeof(int) * tray->siz);
	tray->traylen = (int *) malloc(sizeof(int) * tray->siz);
	tray->tray = (int **) malloc(sizeof(int *) * tray->siz);
	return tray;
}

int fillflg = 0;

void put(TRAY * tray, int y, int x, int c)
{
	/* Add a line if needed */
	if (y > tray->len) {
		fprintf(stderr, "OOOps\n");
		exit(1);
	}
	if (y == tray->len) {
		if (y == tray->siz) {
			tray->siz = y + 10;
			tray->traysiz =
			    (int *) realloc(tray->traysiz,
					    sizeof(int) * tray->siz);
			tray->traylen =
			    (int *) realloc(tray->traylen,
					    sizeof(int) * tray->siz);
			tray->tray =
			    (int **) realloc(tray->tray,
					     sizeof(int) * tray->siz);
		}
		tray->traysiz[y] = x + 40;
		tray->traylen[y] = 0;
		tray->tray[y] =
		    (int *) malloc(sizeof(int) * (tray->traysiz[y]));
		++tray->len;
	}

	/* Extend line if needed */
	if (x + 1 >= tray->traysiz[y]) {
		tray->traysiz[y] = x + 40;
		tray->tray[y] =
		    (int *) realloc(tray->tray[y],
				    sizeof(int) * (tray->traysiz[y]));
	}

	/* Space-fill line if needed */
	while (x > tray->traylen[y])
		tray->tray[y][tray->traylen[y]++] = ' ';

	tray->tray[y][x] = c;
	if (x == tray->traylen[y]) {
		tray->tray[y][x + 1] = 0;
		++tray->traylen[y];
	}
}

int nxt()
{
	static unsigned long cur = 142857;
	cur = (cur << 4) + (cur >> 28) + 3579545;
	return cur & 0x7FFF;
}

int *fill(int *text, int start, int *len, int *siz, int width)
{
	int x;
	int flg = 1;
	int holes[256];
	int touched[256];
	int nholes = 0;
	int nn = 0;
	while (*len && (text[*len - 1] & 255) == ' ')
		text[--*len] = 0;
	if (*siz < start + width + 1) {
		*siz = start + width + 1 + 40;
		text = (int *) realloc(text, sizeof(int) ** siz);
	}
	for (x = start; text[x]; ++x)
		if ((text[x] & 255) == ' ' && flg)
			holes[nholes] = x, touched[nholes] =
			    0, ++nholes, flg = 0;
		else if ((text[x] & 255) != ' ')
			flg = 1;
	if (nholes < 3)
		return text;
	while (*len < start + width) {
		int n = nxt() % nholes;
		if (touched[n] > nn) {
			int org = n;
			do
				n = (n + 1) % nholes;
			while (n != org && touched[n] > nn);
			if (n == org)
				++nn;
		}
		for (x = *len + 1; x >= holes[n]; --x)
			text[x + 1] = text[x];
		++touched[n];
		while (++n < nholes)
			++holes[n];
		++*len;
	}
	return text;
}

void fmtto(TRAY * tray, int *text, int start, int width)
{
	int line = 0;		/* Line number */
	int rest = width;	/* Amount of space left */
	int ed;
	int bg;
	int x;

	while (text[0]) {
		/* Get to end of next word */
		for (ed = 0; (text[ed] & 255) == ' '; ++ed);	/* Skip preceeding spaces */
		bg = ed;
		while (text[ed] && (text[ed] & 255) != ' ')
			++ed;	/* Skip to end of word */

		if (rest == width) {
			text += bg;
			ed -= bg;
			bg = 0;
		}

		if (ed - bg <= width && ed > rest) {
			/* Bump to next line */
			text += bg;
			goto bump;
		}

		for (x = 0; x != ed; ++x)
			put(tray, line, start + width - rest + x, text[x]);
		rest -= ed;
		text += ed;

		if (rest <= 0) {
			int c;
		      bump:
			c = (tray->tray[line][start] & 255);
			if (fillflg)
				tray->tray[line] =
				    fill(tray->tray[line], start,
					 tray->traylen + line,
					 tray->traysiz + line, width);
			if ((width > 4 && c == '-')
			    || (c == '*'
				&& (tray->tray[line][start + 1] & 255) ==
				' '))
				start += 2, width -= 2;
			++line;
			rest = width;
		}
	}
}

TRAY *fmt(OBJ * obj)
{
	TRAY *tray = mktray();
	int x;
	for (x = 0; x != obj->n; ++x) {
		int width;
		if (x + 1 == obj->n)
			width = cols - left - right - obj->start[x];
		else
			width = obj->start[x + 1] - obj->start[x];
		fmtto(tray, obj->glob[x], obj->start[x], width);
	}
	return tray;
}
