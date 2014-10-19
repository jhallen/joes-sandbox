/* Convert tabs to spaces */

/* Provide list of field widths on command line.  If nothing given assume tab stops every 8 columns.
   When past last tab stop, assume more fields with same width as last provided.
*/

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int c, col;

	int nstops;
	int *widths;
	int *pos;
	int x;
	int dif;

	nstops = argc - 1;

	if (!nstops) {
		nstops = 1;
		widths = (int *)malloc(sizeof(int) * nstops);
		pos = (int *)malloc(sizeof(int) * (nstops + 1));
		widths[0] = 8;
		pos[0] = 0;
		pos[1] = 8;
	} else {
		widths = (int *)malloc(sizeof(int) * nstops);
		pos = (int *)malloc(sizeof(int) * (nstops + 1));
		pos[0] = 0;
		for (x = 0; x != nstops; ++x) {
			widths[x] = strtol(argv[x + 1], NULL, 0);
			if (widths[x] <= 0) {
				fprintf(stderr, "Zero width fields not allowed\n");
				exit(1);
			}
			pos[x + 1] = pos[x] + widths[x];
		}
	}

	dif = pos[nstops] - pos[nstops - 1];

	col = 0;
	while ((c = getchar()) != -1) {
		if (c == '\n') {
			putchar(c);
			col = 0;
			x = 0;
		} else if (c == '\t') {
			int stop;
			/* Find next tab stop based on current column */
			for (x = 0; x != nstops + 1; ++x) {
				stop = pos[x];
				if (stop > col)
					break;
			}
			while (stop <= col) {
				stop += dif;
			}

			/* Tab to next stop */
			do
				putchar(' '), ++col;
			while (col != stop);
		} else {
			putchar(c);
			++col;
		}
	}
}
