/* Fix indentation to use tabs */

main()
{
	int col = 0;
	int first = 0;
	int c;
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
}
