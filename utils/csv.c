/* Clean up csv files */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 65536
#define MAXCOLS 80

char *delim = ","; /* Field delimiter on output */
char *line_delim = "; "; /* Replace quoted newlines with this */

int noquote = 0; /* Suppress quotes on output */
int had_to_quote; /* Set if we had to quote something because delimiter was in the text */

int deglitch = 0; /* Fix """", glitch */

int cleanup_en = 0; /* Clean up fields: suppress leading / trailing spaces, convert to uppercase */

int line = 0;

/* Headers */

struct field {
	struct field *next;
	char *name;
	int col;
} *fields;

/* Find column number given name */

int find(char *name)
{
	struct field *f;
	for (f = fields; f; f = f->next) {
		if (!strcmp(f->name, name))
			return f->col;
	}
	return -1;
}

/* Add a header column */

void add(char *name, int col)
{
	struct field *f;
	for (f = fields; f; f = f->next) {
		if (!strcmp(f->name, name)) {
			fprintf(stderr,"Duplicate header column\n");
			exit(-1);
		}
	}
	f = (struct field *)malloc(sizeof(struct field));
	f->next = fields;
	fields = f;
	f->col = col;
	f->name = strdup(name);
}

void killm(char *s)
{
	int n = strlen(s);
	if (n && s[n-1] == 13)
		s[n-1] = 0;
}

/* Convert csv line into fields */

int parse_fields(char **ptr, char *s)
{
	char buf[8192];
	int ncols = 0;
	int x;
	int more = 1;

	while (more) {
		more = 0;
		if (*s == '"') {
			/* It's quoted: de-quote it. */
			char *start = s;
			x = 0;
			++s;
			for (;;) {
				while (!*s) {
					/*
					fprintf(stderr,"%d %d: End of string before end of quoted field?\n",line,ncols);
					exit(1);
					*/
					/* Get more input */
					s+=3;
					if(!gets(s)) {
						fprintf(stderr,"%d: Need more input, but eof reached\n",line);
						exit(-1);
					}
					killm(s);
					++line;
					strcpy(buf + x, line_delim);
					x += strlen(line_delim);
				}
				if (*s != '"') {
					/* Regular character */
					buf[x++] = *s++;
				} else {
					/* End or quote */
					++s;
					if (deglitch && s[0] == '"' && s[1] == '"' && s[2] == '"' && s[3] == ',') {
					        /* Skip one quote */
					} else if (*s=='"') {
						/* Quote */
						buf[x++] = *s++;
					} else if (!*s) {
						/* End */
						break;
					} else if (*s==',') {
						/* End */
						++s;
						more = 1;
						break;
					} else {
						fprintf(stderr,"%d %d: quote with unknown character following it\n",line,ncols);
						exit(1);
					}
				}
			}
			buf[x] = 0;
			strcpy(start, buf);
			ptr[ncols] = start;
		} else {
			/* Not quoted */
			ptr[ncols] = s;
			while (*s && *s != ',')
				++s;
			/* Zero terminate the field */
			if (*s) {
				*s++ = 0;
				more = 1;
			}
		}

		++ncols;
	}
	return ncols;
}

char *clean_up(char *s)
{
	static char buf[MAXLINE];
	int bufx = 0;
	int x;
	int y;

	if (!cleanup_en) {
	        strcpy(buf, s);
	        return buf;
	}

	/* Kill trailing */
	y = strlen(s);
	while (y && s[--y]==' ')
		s[y] = 0;
	/* Skip leading */
	x = 0;
	while (s[x] && s[x]==' ')
		++x;
	/* Copy, convert to upper along the way */
	while (s[x]) {
		if (s[x]>='a' && s[x]<='z')
			buf[bufx++] = s[x++]+'A'-'a';
		else
			buf[bufx++] = s[x++];
	}
	buf[bufx] = 0;
	return buf;
}

void emit(char **s, int *map, int mapx)
{
	int x;
	for (x = 0; x != mapx; ++x) {
		char *q;
		int no = noquote;
		q = clean_up(s[map[x]]);
		if (noquote && strstr(q, delim)) {
		        no = 0;
		        had_to_quote = 1;
		}
		if (!no)
        		putchar('"');
		while (*q) {
		        if (!no) {
        			if (*q=='"')
        				putchar('"');
                        }
			putchar(*q);
			++q;
		}
		if (!no)
        		putchar('"');
		if (x + 1 != mapx)
		        fputs(delim, stdout);
	}
	putchar('\n');
}

char empty[]="";

int is_blank(char *s)
{
	while (*s==' ') ++s;
	if (!*s)
		return 1;
	else
		return 0;
}

int main(int argc, char *argv[])
{
	char header[MAXLINE];
	char *colname[MAXCOLS];
	char buf[MAXLINE];
	char *ptrs[MAXCOLS];
	int map[MAXCOLS];
	int del[MAXCOLS];
	int ncols;
	int x;
	int delx;
	int mapx;
	if (gets(header)) {
		int x;
		++line;
		killm(header);
		ncols = parse_fields(colname,header);
		if (!ncols) {
			fprintf(stderr,"No columns\n");
			return -1;
		}
		fprintf(stderr,"Columns:\n");
		for (x = 0; x != ncols; ++x) {
			fprintf(stderr,"  '%s'\n",colname[x]);
			add(colname[x], x);
		}
	} else {
		fprintf(stderr,"No headers column\n");
		exit(-1);
	}

	/* Create map */
	delx = 0;
	mapx = 0;
	for (x = 1; argv[x]; ++x) {
                if (!strcmp(argv[x], "--help")) {
                        printf("csvtool [options] [-]names\n");
                        printf("  Only include named columns.  All columns included if no names given, except\n");
                        printf("  -name deletes a column in this case.\n");
                        printf("\n");
                        printf("  --delim ','        Set output field delimiter\n");
                        printf("  --noquote          Don't quote fields unless they contain field delimiter\n");
                        printf("  --deglitch         Fix """", glitch\n");
                        printf("  --cleanup          Delete leading and trailing spaces and convert to uppercase\n");
                        printf("  --line_delim '; '  Quoted newlines are replaced with this string\n");
                        return 0;
                } else if (!strcmp(argv[x], "--delim")) {
                        ++x;
                        delim = argv[x];
                } else if (!strcmp(argv[x], "--noquote")) {
                        noquote = 1;
                } else if (!strcmp(argv[x], "--deglitch")) {
                        deglitch = 1;
                } else if (!strcmp(argv[x], "--cleanup")) {
                        cleanup_en = 1;
                } else if (!strcmp(argv[x], "--line_delim")) {
                        ++x;
                        line_delim = argv[x];
                } else if (argv[x][0] == '-' && argv[x][1] == '-') {
                        fprintf(stderr, "Unknown option, try --help\n");
                        return -1;
		} else if (argv[x][0] == '-') {
			/* Delete this column */
			del[delx] = find(&argv[x][1]);
			if (del[delx] == -1) {
				fprintf(stderr,"Unknown column\n");
				exit(-1);
			} else {
				++delx;
			}
		} else {
			map[mapx] = find(&argv[x][0]);
			if (map[mapx] == -1) {
				fprintf(stderr,"Unknown column\n");
				exit(-1);
			} else {
				++mapx;
			}
		}
	}
	if (!mapx) {
		/* No columns were given, so take all of them except those in del list */
		for (x = 0; x != ncols; ++x) {
			int y;
			for (y = 0; y != delx; ++y)
				if (x == del[y])
					break;
			if (y == delx) {
				map[mapx++] = x;
			}
		}
	}
	if (!mapx) {
		fprintf(stderr,"No columns to output\n");
		exit(-1);
	}

	/* Emit header row */
	emit(colname, map, mapx);

	/* OK, process database */
	while (gets(buf)) {
		int n;
		++line;
		killm(buf);
		n = parse_fields(ptrs, buf);
		if (n > ncols) {
			fprintf(stderr,"%d: Record found with more columns than header\n",line);
			exit(-1);
		} else if (n < ncols) {
			fprintf(stderr,"%d: Warning - record found with less columns than header.  Filling in.\n",line);
			while(n < ncols) {
				ptrs[n++] = empty;
			}
		}

		/* Emit fields */
		emit(ptrs, map, mapx);
	}
	if (had_to_quote) {
	        fprintf(stderr, "Warning: had to quote some fields because delimiter appeared in them\n");
	}
	return 0;
}
