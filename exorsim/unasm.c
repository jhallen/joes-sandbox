/*	Disassembler
 *	Copyright
 *		(C) 2011 Joseph H. Allen
 *
 * This is free software; you can redistribute it and/or modify it under the 
 * terms of the GNU General Public License as published by the Free Software 
 * Foundation; either version 1, or (at your option) any later version.  
 *
 * It is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
 * details.  
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this software; see the file COPYING.  If not, write to the Free Software Foundation, 
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* Unassembler */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "unasm6800.h"

int main(int argc, char *argv[])
{
	int label_no = 1;
	unsigned char mem[65536+5];
	char buf[256];
	int target;
	unsigned short pc;
	FILE *f;
	int x;

	int addr = 0;
	char *facts_name = 0;
	char *dump_name = 0;

	memset(mem, 0, sizeof(mem));

        for (x = 1; x != argc; ++x) {
                if (argv[x][0] == '-') {
                        if (!strcmp(argv[x], "--facts") && x + 1 != argc) {
                                ++x;
                                facts_name = argv[x];
                        } else if (!strcmp(argv[x], "--addr") && x + 1 != argc) {
                                ++x;
                                char *p = argv[x];
                                parse_hex(&p, &addr);
                        } else {
                                err:
                                printf("M6800 unassembler\n");
                                printf("\n");
                                printf("unasm [options] dumpfile\n");
                                printf("\n");
                                printf("  --facts file	Annotate with known facts\n");
                                printf("  --addr hhhh	Starting address of dumpfile\n");
                                exit(-1);
                        }
                } else {
                        if (dump_name) {
                                printf("Syntax error\n");
                                return -1;
                        } else
                                dump_name = argv[x];
                }
        }

        if (!dump_name) {
                printf("Syntax error\n");
                return -1;
        }

        if (facts_name) {
                printf("Load facts\n");
                f = fopen(facts_name, "r");
                parse_facts(f);
                fclose(f);
        }

        printf("Load dumpfile\n");
	f = fopen(dump_name, "r");
	fread(mem, 1, 65536, f);
	fclose(f);

	/* Pass 1: find targets */
	for (pc = 0;;) {
		target = -1;
		if (pc >= 0xFFF8)
			fdb_line(mem, &pc, buf, 0);
		else	
			unasm_line(mem, &pc, buf, &target, 0);
		/* printf("%s\n", buf); */
		if (target > 0 && target < 0x10000)
			if (!targets[target])
				targets[target] = label_no++;
		if (pc == 0)
			break;
	}

	/* Pass 2: print with targets */
	for (pc = 0;;) {
		target = -1;
		if (pc >= 0xFFF8)
			fdb_line(mem, &pc, buf, 1);
		else	
			unasm_line(mem, &pc, buf, &target, 1);
		printf("%s\n", buf);
		if (pc == 0)
			break;
	}
}
