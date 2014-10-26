/* Program to generate PSION index and data file */

#include <stdio.h>
#include <string.h>
#include "zstr.h"

#define SIZE 1024

char jindex[SIZE][255];

unsigned short hsh(char *s)
{
	unsigned short hval = 0;
	while (*s)
		hval = (hval * 31 + *s++) % SIZE;
	return hval;
}

int main(int argc, char *argv[])
{
	char buf[1024];
	char gen[10240];
	char *field[10];
	int x, y, z;
	int pos = 1;
	FILE *f = fopen("data", "w");
	for (x = 0; x != SIZE; ++x)
		strcpy(jindex[x], "X");
	while (gets(buf)) {
		int h;
		int opos;
		fields(buf, field, ':');
		h = hsh(field[0]);
		while (strcmp(jindex[h], "X"))
			h = (h + 1) % SIZE;
		gets(gen);
		opos = pos;
		for (y = 0; gen[y];) {
			for (z = 0;
			     gen[z + y] && !(gen[z + y] == '^'
					     && gen[z + y + 1] == 'F'
					     && gen[z + y + 2] == 'S');
			     ++z);
			if (gen[z + y]) {
				gen[z + y] = 0;
				fprintf(f, "%s^FS\n", gen + y);
				z += 3;
			} else
				fprintf(f, "%s\n", gen + y);
			++pos;
			y += z;
		}
		sprintf(jindex[h], "%s\t%s\t%s\t%d\t%d", field[0],
			field[1], field[2], opos, pos - opos);
	}
	fclose(f);
	for (x = 0; x != SIZE; ++x)
		puts(jindex[x]);
	return 0;
}
