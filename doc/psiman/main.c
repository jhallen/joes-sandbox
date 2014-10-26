#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"
#include "attr.h"
#include "out.h"
#include "fmt.h"

int main(int argc, char *argv[])
{
	struct object *obj;
	int x;
	char *flg = 0;
	for (x = 1; argv[x]; ++x)
		if (!strcmp(argv[x], "-rows"))
			++x, sscanf(argv[x], "%d", &rows);
		else if (!strcmp(argv[x], "-cols"))
			++x, sscanf(argv[x], "%d", &cols);
		else if (!strcmp(argv[x], "-t"))
			++x, sscanf(argv[x], "%d", &top);
		else if (!strcmp(argv[x], "-b"))
			++x, sscanf(argv[x], "%d", &bottom);
		else if (!strcmp(argv[x], "-l"))
			++x, sscanf(argv[x], "%d", &left);
		else if (!strcmp(argv[x], "-r"))
			++x, sscanf(argv[x], "%d", &right);
		else if (!strcmp(argv[x], "-ansi"))
			device = 2, paging = 0, rows = 23;
		else if (!strcmp(argv[x], "-lp"))
			device = 1, paging = 1, rows = 66;
		else if (!strcmp(argv[x], "-color"))
			device = 3, paging = 0, rows = 23;
		else if (!strcmp(argv[x], "-ascii"))
			device = 4, paging = 0, rows = 66;
		else if (!strcmp(argv[x], "-hp"))
			device = 5, paging = 1, rows = 60;
		else if (!strcmp(argv[x], "-header"))
			header = argv[++x];
		else if (!strcmp(argv[x], "-paging"))
			paging = 1;
		else if (!strcmp(argv[x], "--paging"))
			paging = 0;
		else if (!strcmp(argv[x], "-fill"))
			fillflg = 1;
		else if (argv[x][0] != '-')
			if (flg) {
				fprintf(stderr, "Too many file names\n");
				exit(1);
			} else if (!freopen(argv[x], "r", stdin)) {
				fprintf(stderr, "Couldn\'t open file %s\n",
					argv[x]);
				exit(1);
			} else
				flg = argv[x];
		else {
			fprintf(stderr, "Command syntax error\n");
			fprintf(stderr, "psiman [-options] [file]\n");
			fprintf(stderr,
				"  (def) -ansi  (--paging, rows=23)   Ansi terminal output\n");
			fprintf(stderr,
				"        -ascii (--paging, rows=66)   Ascii output\n");
			fprintf(stderr,
				"        -color (--paging, rows=23)   Ansi color terminal output\n");
			fprintf(stderr,
				"        -hp    (-paging, rows=60)    HP laser output\n");
			fprintf(stderr,
				"        -lp    (-paging, rows=66)    Line printer output\n");
			fprintf(stderr,
				"        -paging          Enable paging\n");
			fprintf(stderr,
				"        --paging         Disable paging\n");
			fprintf(stderr,
				"        -cols nn (79)    Set page width\n");
			fprintf(stderr,
				"        -rows nn         Set page height\n");
			fprintf(stderr,
				"        -l nn (5)        Set left margin\n");
			fprintf(stderr,
				"        -r nn (4)        Set right margin\n");
			fprintf(stderr,
				"        -t nn (4)        Set top margin\n");
			fprintf(stderr,
				"        -b nn (4)        Set bottom margin\n");
			fprintf(stderr,
				"        -header 'text'   Header to print on each page\n");
			fprintf(stderr,
				"        -fill            Enable text filling\n");
			exit(1);
		}

	if (!header && flg)
		header = flg;

	while ((obj = parse()) != 0) {
		TRAY *tray;
		attr(obj);
		tray = fmt(obj);
		rmobj(obj);
		emit(tray);
		rmtray(tray);
	}
	finish();
	return 0;
}
