#include <stdio.h>

int top = 5;
int bot = 5;
int len = 66;
int left = 1000;

int main(int argc, char *argv[])
{
	char in[1024];
	char out[1024];
	int x, y;
	int lin = 0;
	int flg = 0;
	int first = 0;
	int blankf = 0;
	int ind = 0;
	if (argc != 2) {
		fprintf(stderr, "cvt file\n");
		return 1;
	}
	if (!freopen(argv[1], "r", stdin)) {
		fprintf(stderr, "Couldn't open %s\n", argv[1]);
		return 1;
	}

	/* First pass - find left margin */
	while (gets(in)) {
		int x;
		for (x = 0; in[x] == ' '; ++x);
		if (in[x] && x < left)
			left = x;
	}
	fprintf(stderr, "Left margin is %d\n", left);

	rewind(stdin);
	while (gets(in)) {
		/* Kill backspaces */
		for (x = y = 0; in[x]; ++x)
			if (in[x] == 8)
				--y;
			else
				out[y++] = in[x];
		out[y] = 0;

		/* Guess page length */
		if (lin >= 0 && lin < top && y) {
			if (!flg)
				first = lin, flg = 1;
			else if (flg == 1) {
				len = lin + len - first, flg = 2;
				fprintf(stderr,
					"Page length appears to be %d\n",
					len);
			}
		}

		/* Output line if it isn't in margins */
		if (lin >= top && lin < len - bot) {
			if (!y)
				++blankf;
			else {
				for (x = 0; out[x] == ' '; ++x);
				if (blankf) {
					if (ind != x)
						putchar('\n');
					else if (!
						 (out[x] >= 'a'
						  && out[x] <= 'z'))
						putchar('\n');
					else if (blankf == 1)
						putchar('\n');
				}
				ind = x;
				for (x = 0; out[x]; ++x)
					if (x >= left)
						putchar(out[x]);
				blankf = 0;
				putchar('\n');
			}
		}

		/* Increment page number */
		if (++lin == len)
			lin = 0;
	}
	return 0;
}
