#include <stdio.h>

int main(int argc, char *argv[])
{
	int c;
	while ((c = getchar()) != -1)
		if (c == '\r') {
			getchar();
			putchar('\n');
		} else if (c == 26);
		else if (c == '\n') {
			putchar('\r');
			putchar('\n');
		} else
			putchar(c);
	return 0;
}
