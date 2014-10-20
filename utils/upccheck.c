#include <stdio.h>

char check(char *s)
{
	int n;
	n = (((s[0] & 15) + (s[2] & 15) + (s[4] & 15) + (s[6] & 15) +
	      (s[8] & 15) + (s[10] & 15)) * 3 + (s[1] & 15) + (s[3] & 15) +
	     (s[5] & 15) + (s[7] & 15) + (s[9] & 15));
	return '0' + ((10 - n % 10) % 10);
}

int main(int argc, char *argv[])
{
	char c = check(argv[1]);
	if (argv[1][11]) {
		if (c == argv[1][11]) {
			printf("The entered bar-code is correct\n");
			return 0;
		} else {
			argv[1][11] = c;
			printf
			    ("The entered bar-code is incorrect.  The correct code is: %s\n",
			     argv[1]);
			return -1;
		}
	} else {
		printf("The check digit is: %c\n", c);
		printf("The full bar code is: %s%c\n", argv[1], c);
		return 0;
	}
}
