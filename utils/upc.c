/* UPC (bar code) label sheet printer for HP-Laser jet
   Copyright (C) 1992 Joseph H. Allen

This is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation; either version 1, or (at your option) any later version.  

This is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.  

You should have received a copy of the GNU General Public License along with
this program; see the file COPYING.  If not, write to the Free Software
Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Goto absolute position (x,y) 1/300ths of an inch */

void go(int x, int y)
{
	printf("\033*p%dX\033*p%dY", x, y);
}

/* Add (x,y) to current position 1/300ths of an inch */

void gorel(int x, int y)
{
	if (x)
		printf("\033*p%+dX", x);
	if (y)
		printf("\033*p%+dY", y);
}

/* Select graphics resolution 75, 100, 150 or 300 */

void res(int r)
{
	printf("\033*t%dR", r);
}

/* UPC coding table.  A UPC code begins with 'Start' then 6 digits, then
 * 'middle' then 6 inverse-video digits, then 'end'.
 */

char *codes[] = {
	"0001101",		/* 0 */
	"0011001",		/* 1 */
	"0010011",		/* 2 */
	"0111101",		/* 3 */
	"0100011",		/* 4 */
	"0110001",		/* 5 */
	"0101111",		/* 6 */
	"0111011",		/* 7 */
	"0110111",		/* 8 */
	"0001011",		/* 9 */
	"01010",		/* Middle */
	"101",			/* Start/End */
	"0000000"		/* Blank */
};

/* Generate digit 'z' (index into codes table) into buffer beginning at 'x'.
 * The lines generated are 'w' units (of the selected resolution) wide
 */

int bargen(char *buf, int x, int z, int w)
{
	int q, r;
	z &= 15;
	for (q = 0; codes[z][q]; ++q) {
		for (r = 0; r != w; ++r) {
			if (codes[z][q] & 1)
				buf[x / 8] |= (128 >> (x & 7));
			++x;
		}
	}
	return x;
}

/* Like above, but generate inverse-video digit */

int bargeni(char *buf, int x, int z, int w)
{
	int q, r;
	z &= 15;
	for (q = 0; codes[z][q]; ++q) {
		for (r = 0; r != w; ++r) {
			if (!(codes[z][q] & 1))
				buf[x / 8] |= (128 >> (x & 7));
			++x;
		}
	}
	return x;
}

/* Output a full UPC code line.  Uses the given width. */

void bcline(int width, char *code)
{
	int bytes = (95 * width + 7) / 8;
	char *buf = (char *) calloc(bytes, 1);
	int x;
	int z;
	printf("\033*r1A");
	printf("\033*b%dW", bytes);	/* Start graphics */
	x = 0;

	x = bargen(buf, x, 11, width);	/* Start */
	for (z = 0; z != 6; ++z)
		x = bargen(buf, x, code[z], width);
	x = bargen(buf, x, 10, width);	/* Middle */
	for (; z != 12; ++z)
		x = bargeni(buf, x, code[z], width);
	x = bargen(buf, x, 11, width);	/* End */

	for (x = 0; x != bytes; ++x)
		putchar(buf[x]);	/* Output buffer */

	printf("\033*rB");	/* Done graphics */
	free(buf);
}

/* Output an empty UPC code line (where the digits go) */

void bcempty(int width, char *code)
{
	int bytes = (95 * width + 7) / 8;
	char *buf = (char *) calloc(bytes, 1);
	int x;
	int z;
	printf("\033*r1A");
	printf("\033*b%dW", bytes);
	x = 0;

	x = bargen(buf, x, 11, width);	/* Start */
	for (z = 0; z != 6; ++z)
		x = bargen(buf, x, 12, width);	/* Blank */
	x = bargen(buf, x, 10, width);	/* Middle */
	for (; z != 12; ++z)
		x = bargen(buf, x, 12, width);	/* Blank */
	x = bargen(buf, x, 11, width);	/* End */

	for (x = 0; x != bytes; ++x)
		putchar(buf[x]);
	printf("\033*rB");
	free(buf);
}

/* Print a UPC code of height 'h', width multiple 'w' */

void bc(int w, int h, char *code)
{
	int n;
	for (n = 0; n != h - 12; ++n)
		bcline(w, code);
	for (; n != h; ++n)
		bcempty(w, code);
}

/* Print page of labels */

void page(char *title, char *code)
{
	int y, x;

	res(150);

	/* Begin macro recording */
	printf("\033&f5Y");
	printf("\033&f0X");

	/* Title */
	y = 26 / 2 - strlen(title) / 2;
	for (x = 0; x != y; ++x)
		putchar(' ');
	printf("%s", title);
	gorel(-(strlen(title) + y) * 30 + 165, 50 + 75);
	printf("%c             %c", code[0], code[11]);

	/* Digits */
	gorel(-15 * 30, 75 + 50);
	printf("  %c%c%c%c%c %c%c%c%c%c", code[1], code[2], code[3],
	       code[4], code[5], code[6], code[7], code[8], code[9],
	       code[10]);
	gorel(35 - 13 * 30, -287 + 50);

	/* Barcode */
	bc(2, 112, code);

	/* Done macro */
	printf("\033&f1X");

	/* Execute macro 30 times on page */
	for (y = 0; y != 10; ++y)
		for (x = 0; x != 3; ++x) {
			go(x * 825 + 17, 303 * y + 42);
			printf("\033&f2X");
		}
}

/* Determine UPC check digit */

char check(char *s)
{
	int n;
	n = ((s[0] & 15) + (s[2] & 15) + (s[4] & 15) + (s[6] & 15) +
	     (s[8] & 15) + (s[10] & 15)) * 3 + (s[1] & 15) + (s[3] & 15) +
	    (s[5] & 15) + (s[7] & 15) + (s[9] & 15);
	return 1 + '9' - n % 10;
}


int main(int argc, char *argv[])
{
	char buf[13], c;
	if (argc != 3)
		fprintf(stderr, "bar \'title\' code | lpr\n"), exit(1);
	if (strlen(argv[2]) < 11 || strlen(argv[2]) > 12)
		fprintf(stderr, "product code must be 11 or 12 digits\n"),
		    exit(1);

	strcpy(buf, argv[2]);
	c = check(buf);
	if (buf[11]) {
		if (buf[11] != c)
			fprintf(stderr, "UPC check digit is incorrect\n"),
			    exit(1);
	} else {
		buf[11] = c;
		buf[12] = 0;
		fprintf(stderr, "UPC check digit is %c\nFull UPC is %s\n",
			c, buf);
	}

	page(argv[1], buf);	/* Print labels */

	putchar('\f');		/* Form-feed */

	return 0;
}
