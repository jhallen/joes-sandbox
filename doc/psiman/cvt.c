#include <stdio.h>
#include <string.h>

int indent = 0;
int curcol = 0;
int attr = 0;

void toindent(int indent)
{
	if (curcol >= indent) {
		curcol = 0;
		return;
	}
	while (curcol + 8 - (curcol & 7) <= indent)
		putchar('\t'), curcol += 8 - (curcol & 7);
	while (curcol < indent)
		putchar(' '), ++curcol;
	curcol = 0;
}

void fix(char *s)
{
	int x, y;
	for (y = x = 0; s[x]; ++x)
		if (s[x] == '\\')
			switch (s[++x]) {
			case 'f':
				switch (s[++x]) {
				case 'B':
					s[y++] = '_';
					attr = 1;
					break;
				case 'R':
					s[y++] = '_';
					attr = 0;
					break;
				}
				break;

			case 's':
				while (s[x + 1] == '-' || s[x + 1] >= '0'
				       && s[x + 1] <= '9')
					++x;
				break;

			case 'e':
				s[y++] = '\\';
				break;

			case '^':
			case '(':
			case '&':
			case '!':
			case '%':
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'h':
			case 'l':
			case 'L':
			case 'o':
			case 'p':
			case 'r':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'z':
			case '{':
			case '}':
				break;

			case 'k':
				++x;
				break;

			case '0':
				s[y++] = ' ';
				break;

			default:
				s[y++] = s[x];
		} else
			s[y++] = s[x];
	while (y && s[y - 1] == ' ')
		--y;
	if (attr)
		s[y++] = '_', attr = 0;
	s[y] = 0;
}

void out(int indent, char *s)
{
	int x;
	while (*s == ' ' || *s == '\t')
		++s;
	toindent(indent);
	puts(s);
}

char buf[1024];

int dot(char *s)
{
	int x;
	for (x = 0; buf[x] && s[x]; ++x)
		if (s[x] != buf[x])
			break;
	if (s[x] == 0 && (buf[x] == ' ' || !buf[x]))
		return 1;
	return 0;
}

char ary[65536];
int idx = 0;

void hold(char *s)
{
	while (*s == ' ')
		++s;
	if (!*s)
		return;
	if (idx)
		ary[idx++] = ' ';
	strcpy(ary + idx, s);
	idx += strlen(ary + idx);
}

void nhold(char *s)
{
	int flg = 0;
	while (*s == ' ')
		++s;
	if (!*s)
		return;
	if (idx)
		ary[idx++] = ' ';
	while (*s) {
		if (*s == '"')
			flg = !flg;
		else if (flg)
			ary[idx++] = *s;
		else if (*s != ' ')
			ary[idx++] = *s;
		++s;
	}
}

void ihold(char *s)
{
	int flg = 0;
	while (*s == ' ')
		++s;
	if (!*s)
		return;
	if (idx)
		ary[idx++] = ' ';
	while (*s) {
		if (*s == '"')
			flg = !flg;
		else
			ary[idx++] = *s;
		++s;
	}
}

void flsh()
{
	int x;
	int y;
	int col = indent;
	if (!idx) {
		if (curcol)
			putchar('\n'), curcol = 0;
		return;
	}
	ary[idx] = 0;
	fix(ary);
	idx = strlen(ary);
	toindent(indent);
	for (y = x = 0; x != idx; ++x)
		if (col > 77) {
			while (x != y && ary[x - 1] != ' ')
				--x;
			while (x != y && ary[x - 1] == ' ')
				--x;
			ary[x] = 0;
			out(0, ary + y);
			ary[x] = ' ';
			while (ary[x] == ' ' && x != idx)
				++x;
			y = x;
			col = 0;
		} else
			++col;
	if (y != x) {
		ary[x] = 0;
		out(0, ary + y);
	}
	idx = 0;
}

int main(int argc, char *argv[])
{
	int blankf = 0;
	while (gets(buf))
		if (buf[0] == '.' && buf[1] == '\\' && buf[2] == '"');
		else if (dot(".ne"));
		else if (dot(".ns"));
		else if (dot(".HP"));
		else if (dot(".sp") || dot(".br")) {
			flsh();
			if (!blankf)
				putchar('\n'), blankf = 1;
		} else if (dot(".SH")) {
			flsh();
			indent = 8;
			fix(buf + 3);
			out(0, buf + 3);
		} else if (dot(".DA"));
		else if (dot(".I") || dot(".R")) {
			blankf = 0;
			hold(buf + 2);
		} else if (dot(".IR")) {
			blankf = 0;
			nhold(buf + 3);
		} else if (dot(".BI")) {
			blankf = 0;
			ihold(buf + 3);
		} else if (dot(".RB") || dot(".SM") || dot(".BR")
			   || dot(".RS") || dot(".RE")) {
			blankf = 0;
			hold(buf + 3);
		} else if (dot(".SS")) {
			flsh();
			indent = 8;
			fix(buf + 3);
			out(4, buf + 3);
		} else if (dot(".PP") || dot(".LP")) {
			flsh();
			indent = 8;
			if (!blankf)
				putchar('\n'), blankf = 1;
		} else if (dot(".B")) {
			blankf = 0;
			hold(buf + 2);
		} else if (dot(".IP") || dot(".TP")) {
			flsh();
			indent = 8;
			if (!blankf)
				putchar('\n'), blankf = 1;
			fix(buf + 3);
			if (strlen(buf + 3) > 8) {
				out(indent, buf + 3);
				indent = 16;
			} else {
				int x;
				for (x = 0; x != indent; ++x)
					putchar(' ');
				if (buf[3]) {
					for (x = 0; buf[x + 4]; ++x)
						putchar(buf[x + 4]);
					curcol = x + indent;
					indent = 16;
				}
			}
		} else if (!buf[0] && !blankf) {
			flsh();
			++blankf;
			putchar('\n');
		} else if (dot(".TH"))
			flsh();
		else if (buf[0] == '.')
			fprintf(stderr, "Unknown sequence '.%c%c'\n",
				buf[1], buf[2]);
		else if (buf[0]) {
			hold(buf);
			blankf = 0;
		}
	flsh();
	return 0;
}
