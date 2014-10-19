/* Convert tabs to spaces */

/* Provide list of field widths on command line.  If nothing given assume tab stops every 8 columns.
   When past last tab stop, assume more fields with same width as last provided.
*/

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int c, col; // Character, column

	int nstops; // No. tab stops
	int *pos; // Array of tab stop positions
	int x;
	int width; // Last field width

	nstops = argc - 1;

	if (!nstops) {
		nstops = 1;
		pos = (int *)malloc(sizeof(int) * (nstops + 1));
		pos[0] = 0;
		pos[1] = 8;
		width = 8;
	} else {
		pos = (int *)malloc(sizeof(int) * (nstops + 1));
		pos[0] = 0;
		for (x = 0; x != nstops; ++x) {
			width = strtol(argv[x + 1], NULL, 0);
			if (width <= 0) {
				fprintf(stderr, "Zero width fields not allowed\n");
				exit(1);
			}
			pos[x + 1] = pos[x] + width;
		}
	}

	col = 0;
	while ((c = getchar()) != -1) {
		if (c == '\n') {
			putchar(c);
			col = 0;
		} else if (c == '\t') {
			int stop;
			/* Find next tab stop based on current column */
			for (x = 0; x != nstops + 1; ++x) {
				stop = pos[x];
				if (stop > col)
					break;
			}
			while (stop <= col) {
				stop += width;
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
	return 0;
}
