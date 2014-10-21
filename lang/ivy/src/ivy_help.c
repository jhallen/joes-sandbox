/* On-line help

   Copyright (C) 1993 Joseph H. Allen

This file is part of IVY

IVY is free software; you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation; either version 1, or (at your option) any later version.  

IVY is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
details.  


IVY; see the file COPYING.  If not, write to the Free Software Foundation, 
675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <setjmp.h>
#include <sys/stat.h>

#include "ivy_tree.h"
#include "ivy.h"

#include "ivy_online.h"

char *choose_pager()
{
	char *s = getenv("PAGER");
	struct stat stbuf;
	if (s && !stat(s, &stbuf)) return s;
	else if (!stat(s = "/usr/bin/more", &stbuf)) return s;
	else if (!stat(s = "/usr/bin/less", &stbuf)) return s;
	else return 0;
}

void rthelp(Ivy *ivy)
{
	char buf[32];
	Var *a;
	int x;
	a = getv(ivy, "a");
	if (a && a->val.type == tSTR) {
		strncpy(buf, a->val.u.str->s, sizeof(buf) - 1);
		buf[sizeof(buf) - 1]=0;
		goto find;
	}
	fprintf(ivy->out, "Choose topic:\n");
	for (x=0; sections[x].name; ++x)
		fprintf(ivy->out, "  %s\n", sections[x].name);
	while (fprintf(ivy->out, "Help>"), fgets(buf, sizeof(buf) - 1, ivy->in)) {
		if (strlen(buf)) buf[strlen(buf) - 1] = 0;
		find: if (buf[0]) {
			for (x = 0; sections[x].name; ++x)
				if (!strcmp(sections[x].name, buf)) {
					FILE *f;
					char *s;
					s = choose_pager();
					f = 0;
					if (s)
						f = popen(s, "w");
					if (!f)
						f = ivy->out;
					fprintf(f, "%s", sections[x].text);
					if (s)
						pclose(f);
					goto bye;
				}
			fprintf(ivy->out, "No such section\n");
		}
	}
	bye:
	mkval(psh(ivy), tVOID);
}		
