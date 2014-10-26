#include <stdio.h>
#include <stdlib.h>

char *fonts[256];
char buf[4096];
int widths[256];
int heights[256];


int main(int argc, char *argv[])
{
	FILE *f;
	int nfonts = 0;
	int maxwid = 0;
	int maxhei = 0;
	int c;
	while (gets(buf)) {
		int chr, wid, hei;
		int y, x, z;
		sscanf(buf, "%d,%d,%d", &chr, &wid, &hei);
		fonts[chr] = (char *) calloc(wid * hei + 4, 1);
		for (y = 0, z = 0; y != hei; ++y, z += wid) {
			gets(buf);
			for (x = 0; x != wid && buf[x]; ++x)
				if (buf[x] != 32)
					fonts[chr][z + x] = 1;
				else
					fonts[chr][z + x] = 0;
			while (x != wid)
				fonts[chr][z + x++] = 0;
		}
		++nfonts;
		if (wid > maxwid)
			maxwid = wid;
		if (hei > maxhei)
			maxhei = hei;
		widths[chr] = wid;
		heights[chr] = hei;
	}
	printf("^XA^IDIVYFONT.FNT^XZ\n~DBIVYFONT.FNT,N,%d,%d,%d,%d,%d,,",
	       maxhei, maxwid, maxhei, maxwid, nfonts);
	for (c = 0; c != 256; ++c)
		if (fonts[c]) {
			int x, y;
			printf("#%4.4X.%d.%d.0.%d.%d.\n", c, heights[c],
			       widths[c], heights[c] - 2, widths[c]);
			for (y = 0; y != heights[c]; ++y) {
				for (x = 0; x < widths[c]; x += 8) {
					int q =
					    (fonts[c]
					     [x + 0 +
					      y * widths[c]] << 3) +
					    (fonts[c]
					     [x + 1 +
					      y * widths[c]] << 2) +
					    (fonts[c]
					     [x + 2 +
					      y * widths[c]] << 1) +
					    (fonts[c]
					     [x + 3 + y * widths[c]] << 0);
					int r =
					    (fonts[c]
					     [x + 4 +
					      y * widths[c]] << 3) +
					    (fonts[c]
					     [x + 5 +
					      y * widths[c]] << 2) +
					    (fonts[c]
					     [x + 6 +
					      y * widths[c]] << 1) +
					    (fonts[c]
					     [x + 7 + y * widths[c]] << 0);
					printf("%c%c",
					       "0123456789ABCDEF"[q],
					       "0123456789ABCDEF"[r]);
				}
				printf("\n");
			}
		}
	printf("^XA^CWX,IVYFONT.FNT^FS^PRA^XZ\n");
	f = fopen("fnt.parms", "w");
	fprintf(f, "%d,%d\n", maxwid, maxhei);
	fclose(f);
}
