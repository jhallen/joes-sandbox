/* Fix indentation to use tabs */

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	int col = 0;
	int first = 0;
	int c;
	int width = 8;
	if (argv[1]) {
		width = strtol(argv[1], NULL, 0);
		if (width <= 0) {
			fprintf(stderr, "tab stop must be > 0\n");
			return -1;
		}
	}
	while ((c = getchar()) != -1) {
		if (!first) {
			if (c == ' ')
				++col;
			else if (c == '\t')
				do ++col; while (col & 7);
			else {
				first = 1;
				while (col >= 8) {
					putchar('\t');
					col -= 8;
				}
				while (col) {
					putchar (' ');
					--col;
				}
				putchar(c);
			}
		} else {
			putchar (c);
			if (c == '\n') {
				first = 0;
				col = 0;
			}
		}
	}
	return 0;
}
