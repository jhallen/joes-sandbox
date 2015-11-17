/* Main

	    Copyright (C) 1993 Joseph H. Allen

This file is part of IVY

IVY is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 1, or (at your option) any later version.

IVY is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
details.

You should have received a copy of the GNU General Public License along with
IVY; see the file COPYING.  If not, write to the Free Software Foundation,
675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "ivy_tree.h"
#include "ivy.h"

void error_print(void *obj, char *s)
{
	fputs(s, stderr);
	fputc('\n', stderr);
}

int main(int argc,char *argv[])
{
	char *filename = 0;
	int unasm = 0;
	int ptree = 0;
	int ptop = 0;
	int norun = 0;
	int trace = 0;
	int x;
	Ivy ivy[1];

	mk_ivy(ivy, error_print, NULL, stdin, stdout);
	set_globals(ivy, mk_globals(ivy));

	/* Parse options */
	for(x = 1; x != argc; ++x) {
		if (!strcmp(argv[x], "-u")) {
			/* Unassemble */
			unasm = 1;
		} else if (!strcmp(argv[x], "-t")) {
			/* Print tree */
			ptree = 1;
		} else if (!strcmp(argv[x], "-r")) {
			/* tRace while running */
			trace = 1;
		} else if (!strcmp(argv[x], "-c")) {
			/* Print result of each command */
			ptop = 1;
		} else if (!strcmp(argv[x], "-n")) {
			/* Compile only, don't run */
			norun = 1;
		} else if (!strcmp(argv[x], "-h")) {
			printf("ivy [options] [file]\n");
			printf("  -n   Compile, don't run\n");
			printf("  -u   Unassemble\n");
			printf("  -t   Print parse tree\n");
			printf("  -r   Trace\n");
			printf("  -c   Print result of each command\n");
			return 0;
		} else if (argv[x][0] == '-') {
			fprintf(stderr, "Unknown option %s\n", argv[x]);
			return -1;
		} else {
			if (filename) {
				fprintf(stderr, "Only one file may be given\n");
				return -1;
			} else {
				filename = argv[x];
			}
		}
	}

	if (filename) { /* Run a file */
		FILE *f = fopen(filename, "r");
		if (!f) {
			fprintf(stderr,"Couldn't open file \'%s\'\n", filename);
			return -1;
		} else {
			char buf[1024];
			Parser *parser = mkparser(ivy, filename);
			while (fgets(buf, sizeof(buf) - 1, f)) {
				parse(ivy, parser, buf, unasm, ptree, ptop, norun, trace);
			}
			parse_done(ivy, parser, unasm, ptree, ptop, norun, trace);
			rmparser(parser);
			fclose(f);
		}
	} else { /* Interactive */
		Parser *parser = mkparser(ivy, "Command Line");
		char *s;
		char prompt[100];
		printf("\nIvy\n\n");
		printf("type 'help' if you need it\n\n");
		for(;;) {
			if (parser->need_more)
				sprintf(prompt, "%d#", parser->paren_level);
			else
				sprintf(prompt, "->");
			s = readline(prompt);
			if (!s)
				break;
			add_history(s);
			parse(ivy, parser, s, unasm, ptree, ptop, norun, trace);
			if (!parser->need_more)
				parse_done(ivy, parser, unasm, ptree, ptop, norun, trace);
			free(s);
		}
		rmparser(parser);
	}
	return 0;
}
