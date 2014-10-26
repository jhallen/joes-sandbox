#include <stdio.h>
#include <string.h>
#include "obj.h"
#include "fmt.h"
#include "out.h"

/* Page size */
int rows = 66;
int cols = 79;

/* Margins */
int top = 4;
int bottom = 4;
int left = 5;
int right = 4;

/* Current position */
int row = 0;
int page = 0;
int paging = 0;			/* Set for paging */

char *header = 0;		/* Header at top of page */

/* Output a line with proper attributes */

void outleft()
{
	int x;
	for (x = 0; x != left; ++x)
		putchar(' ');
}

/* Version for simple line printer */
int device = 0;

void outlin(int *text)
{
	if (!device) {		/* Set default device */
		device = 2;
	}
	if (device == 2) {
		int attr = 0;
		while (*text) {
			if ((*text & (BOLD + ITAL)) != attr)
				printf("\033[m"), attr = 0;
			if ((*text & BOLD) && !(attr & BOLD))
				printf("\033[1m"), attr |= BOLD;
			if ((*text & ITAL) && !(attr & ITAL))
				printf("\033[4m"), attr |= ITAL;
			putchar(*text);
			++text;
		}
		if (attr)
			printf("\033[m");
		putchar('\n');
	} else if (device == 5) {
		int attr = 0;
		while (*text) {
			if ((*text & BOLD) && !(attr & BOLD))
				printf("\033(s7B"), attr |= BOLD;
			if ((*text & ITAL) && !(attr & ITAL))	/* printf("\033(s1S") */
				printf("\033(s7B"), attr |= ITAL;
			if (!(*text & BOLD) && (attr & BOLD))
				printf("\033(s0B"), attr &= ~BOLD;
			if (!(*text & ITAL) && (attr & ITAL))	/* printf("\033(s0S") */
				printf("\033(s0B"), attr &= ~ITAL;
			putchar(*text);
			++text;
		}
		if (attr)
			printf("\033(s0S\033(s0B");
		putchar('\r'), putchar('\n');
	} else if (device == 3) {
		int attr = 0;
		while (*text) {
			if ((*text & (BOLD + ITAL)) != attr)
				printf("\033[37m"), attr = 0;
			if ((*text & BOLD) && !(attr & BOLD))
				printf("\033[36m"), attr |= BOLD;
			if ((*text & ITAL) && !(attr & ITAL))
				printf("\033[35m"), attr |= ITAL;
			putchar(*text);
			++text;
		}
		if (attr)
			printf("\033[37m");
		putchar('\n');
	} else if (device == 4) {
		while (*text) {
			putchar(*text);
			++text;
		}
		putchar('\n');
	} else {
		char ital[1024];
		char bold[1024];
		char italbold[1024];
		char norm[1024];
		int iif = -1;
		int bf = -1;
		int ibf = -1;
		int x;
		for (x = 0; text[x]; ++x) {
			norm[x] = text[x];

			if (text[x] & ITAL)
				ital[iif = x] = '_';
			else
				ital[x] = ' ';

			if ((text[x] & ITAL) && (text[x] & BOLD))
				italbold[ibf = x] = '_';
			else
				italbold[x] = ' ';

			if (text[x] & BOLD)
				bold[bf = x] = text[x];
			else
				bold[x] = ' ';
		}
		++iif;
		++bf;
		++ibf;
		ital[iif] = 0;
		bold[bf] = 0;
		italbold[ibf] = 0;
		norm[x] = 0;
		if (iif)
			outleft(), fputs(ital, stdout), putchar('\r');
		if (ibf)
			outleft(), fputs(italbold, stdout), putchar('\r');
		if (ibf)
			outleft(), fputs(italbold, stdout), putchar('\r');
		if (bf)
			outleft(), fputs(bold, stdout), putchar('\r');
		if (bf)
			outleft(), fputs(bold, stdout), putchar('\r');
		if (x)
			outleft(), fputs(norm, stdout);
		putchar('\r');
		putchar('\n');
	}
	if (++row == rows)
		row = 0, ++page;
}

/* Line output routine */

int blank[1] = { 0 };

void eachlin(int *text)
{
	if (paging) {
		while (row < top) {
			if (row == 1 && header) {
				int hdr[1024];
				int x;
				int len = strlen(header);
				for (x = 0; x != cols - right; ++x)
					hdr[x] = ' ';
				hdr[x] = 0;
				for (x = 0; header[x]; ++x)
					hdr[x] =
					    header[x],
					    hdr[cols - right - len + x] =
					    header[x];
				outlin(hdr);
			} else
				outlin(blank);
		}
	}
	outlin(text);
	if (paging) {
		while (row >= rows - bottom) {
			if (row == rows - 2) {
				char buf[20];
				int hdr[1024];
				int len;
				int x;
				sprintf(buf, "Page %d", page + 1);
				len = strlen(buf);
				for (x = 0; x != cols - right; ++x)
					hdr[x] = ' ';
				hdr[x] = 0;
				for (x = 0; buf[x]; ++x)
					hdr[cols - right - len + x] =
					    buf[x];
				outlin(hdr);
			} else
				outlin(blank);
		}
	}
}

/* Output to end of page */

void finish()
{
	if (paging) {
		if (row)
			while (row)
				eachlin(blank);
	}
}

/* No. lines remaining on page */

int remain()
{
	int lins = rows - top - bottom;
	if (row > top && row < rows - bottom)
		lins -= row - top;
	return lins;
}

/* Emit an object */

void emit(TRAY * tray)
{
	int left;
	int x;
	int orph = 0;
	if (!tray->len) {
		if (!paging || (row > top && row < rows - bottom))
			eachlin(blank);
		return;
	}
	if (tray->len == 1) {
		/* Try only has one line */
		eachlin(tray->tray[0]);
		return;
	}
	if (paging) {
		left = remain();
		if (left == 1)
			/* A widow */
			eachlin(blank);
		else if (tray->len - left == 1) {	/* An orphan */
			if (bottom)
				--bottom, orph = 1;
			else
				eachlin(blank);
		}
	}

	/* Output tray */
	for (x = 0; x != tray->len; ++x)
		eachlin(tray->tray[x]);
	if (orph)
		++bottom;
}
