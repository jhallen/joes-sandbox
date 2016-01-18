/*	M6800 unassembler
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unasm6800.h"

struct fact *swi_facts;
int targets[65536];
struct fact *facts[65536];

struct fact *mkfact(unsigned short addr, int type, int len, char *label, char *comment)
{
	struct fact *f = (struct fact *)malloc(sizeof(struct fact));
        f->next = 0;
	f->addr = addr;
	f->type = type;
	f->len = len;
	f->label = strdup(label);
	f->comment = strdup(comment);
	/* printf("Create fact addr=%x type=%d len=%d label=%s comment=%s\n",
               addr, type, len, label, comment); */
	return f;
}

/* Parse known facts */

void parse_facts(FILE *f)
{
	char buf[100];
	char *p;
	int line = 0;
	while (fgets(buf, sizeof(buf)-1, f)) {
		if (buf[0] && buf[strlen(buf)-1]=='\n')
			buf[strlen(buf)-1] = 0;
		if (buf[0] && buf[strlen(buf)-1]=='\r')
			buf[strlen(buf)-1] = 0;
		p = buf;
		unsigned short addr;
		char keyword[100];
		int len;
		char label[100];
		char *comment = 0;
		addr = 0;
		keyword[0] = 0;
		len = 0;
		label[0] = 0;
		++line;
		if (parse_hex(&p, &addr) && skipws(&p)) {
			if (parse_word(&p, keyword) && skipws(&p)) {
				if (!strcmp(keyword, "code")) {
					if (parse_word(&p, label) && skipws(&p)) {
						comment = p;
						facts[addr] = mkfact(addr, 0, 0, label, comment);
					}
                                } else if (!strcmp(keyword, "subr")) {
					if (parse_word(&p, label) && skipws(&p)) {
						comment = p;
						facts[addr] = mkfact(addr, 5, 0, label, comment);
					}
				} else {
					if (parse_dec(&p, &len) && skipws(&p) &&
					    parse_word(&p, label) && skipws(&p)) {
					    	comment = p;
					    	if (!strcmp(keyword, "swi")) {
					    	        struct fact *f = mkfact(addr, 6, len, label, comment);
					    	        f->next = swi_facts;
					    	        swi_facts = f;
					    	} else if (!strcmp(keyword, "fcb")) {
							facts[addr] = mkfact(addr, 1, len, label, comment);
					    	} else if (!strcmp(keyword, "fdb")) {
							facts[addr] = mkfact(addr, 2, len, label, comment);
					    	} else if (!strcmp(keyword, "fcc")) {
							facts[addr] = mkfact(addr, 3, len, label, comment);
					    	} else if (!strcmp(keyword, "rmb")) {
							facts[addr] = mkfact(addr, 4, len, label, comment);
					    	} else {
					    		printf("%d: Unknown keyword '%s'\n", line, keyword);
					    	}
					} else {
						printf("%d: Syntax error\n", line);
					}
				}
			}
		}
	}
}

#define IDX() (sprintf(buf + strlen(buf), "%2.2X ", mem[pc++]), -1)
#define IMM() (sprintf(buf + strlen(buf), "%2.2X ", mem[pc++]), (pc - 1))
#define IMM2() (sprintf(buf + strlen(buf), "%2.2X ", mem[pc++]), sprintf(buf + strlen(buf), "%2.2X ", mem[pc++]), (pc - 2))
#define DIR() (sprintf(buf + strlen(buf), "%2.2X ", mem[pc++]), mem[pc - 1])
#define EXT() (sprintf(buf + strlen(buf), "%2.2X ", mem[pc++]), sprintf(buf + strlen(buf), "%2.2X ", mem[pc++]), (mem[pc - 2] << 8) + mem[pc - 1])

int fcb_line(unsigned char *mem, unsigned short *at_pc, char *outbuf, int flag)
{
	char buf[256];
	char buf1[256];
	unsigned short pc = *at_pc;
	struct fact *fact;
	int flg = targets[pc];
	sprintf(buf, "%4.4X: %2.2X ", pc, mem[pc]);
	fact = facts[pc];
	pc++;
	if (fact) {
		sprintf(buf1, "%-10s FCB $%2.2X", fact->label, mem[pc - 1]);
	} else if (flg) {
		sprintf(buf1, "L%4.4d      FCB $%2.2X", flg, mem[pc - 1]);
	} else {
		sprintf(buf1, "           FCB $%2.2X", mem[pc - 1]);
	}
	sprintf(outbuf, "%-15s%-9s",buf, buf1);
        if (fact)
                sprintf(outbuf + strlen(outbuf),"			* %s", fact->comment);
	*at_pc = pc;
	return flg;
}

int rmb_line(unsigned char *mem, unsigned short *at_pc, char *outbuf, int flag, int len)
{
	char buf[256];
	char buf1[256];
	unsigned short pc = *at_pc;
	struct fact *fact;
	int flg = targets[pc];
	sprintf(buf, "%4.4X: %2.2X ", pc, mem[pc]);
	fact = facts[pc];
	pc += len;
	if (fact) {
		sprintf(buf1, "%-10s RMB %d", fact->label, len);
	} else if (flg) {
		sprintf(buf1, "L%4.4d      RMB %d", flg, len);
	} else {
		sprintf(buf1, "           RMB %d", len);
	}
	sprintf(outbuf, "%-15s%-9s",buf, buf1);
        if (fact)
                sprintf(outbuf + strlen(outbuf),"			* %s", fact->comment);
	*at_pc = pc;
	return flg;
}

int fcc_line(unsigned char *mem, unsigned short *at_pc, char *outbuf, int flag, int len)
{
	char buf[256];
	char buf1[256];
	unsigned short pc = *at_pc;
	struct fact *fact;
	int flg = targets[pc];
	sprintf(buf, "%4.4X: %2.2X ", pc, mem[pc]);
	fact = facts[pc];
	pc += len;
	if (fact) {
		sprintf(buf1, "%-10s FCC %d", fact->label, len);
	} else if (flg) {
		sprintf(buf1, "L%4.4d      FCC %d", flg, len);
	} else {
		sprintf(buf1, "           FCC %d", len);
	}
	sprintf(outbuf, "%-15s%-9s",buf, buf1);
        if (fact)
                sprintf(outbuf + strlen(outbuf),"			* %s", fact->comment);
	*at_pc = pc;
	return flg;
}

int fdb_line(unsigned char *mem, unsigned short *at_pc, char *outbuf, int flag)
{
	char buf[256];
	char buf1[256];
	int pc = *at_pc;
	struct fact *fact;
	int flg = targets[pc];
	sprintf(buf, "%4.4X: %2.2X %2.X", pc, mem[pc], mem[pc+1]);
	fact = facts[pc];
	pc += 2;
	if (fact) {
		sprintf(buf1, "%-10s FDB $%4.4X", fact->label, (((int)mem[pc - 2] << 8) + mem[pc -1]));
	} else if (flg) {
		sprintf(buf1, "L%4.4d      FDB $%4.4X", flg, (((int)mem[pc - 2] << 8) + mem[pc -1]));
	} else {
		sprintf(buf1, "           FDB $%4.4X", (((int)mem[pc - 2] << 8) + mem[pc -1]));
	}
	sprintf(outbuf, "%-15s%-9s",buf, buf1);
        if (fact)
                sprintf(outbuf + strlen(outbuf),"			* %s", fact->comment);
	*at_pc = pc;
	return flg;
}

void unasm_line(unsigned char *mem, unsigned short *at_pc, char *outbuf, int *at_target, int flag)
{
	unsigned short pc;
	struct fact *fact;
	char *insn;
	char buf[150];
	char buf1[150];
	char operand[150];
        unsigned char opcode;
        int ea_fact = -1;
        int is_jsr = 0;
        int is_swi = -1;

        int branch_target = -1;

	int ea;
	int flg;


        buf[0] = 0;
        operand[0] = 0;
	char offset;

	/* Fetch */

	pc = *at_pc;
	flg = targets[pc];
	fact = facts[pc];

	insn = "???";
	sprintf(buf,"%4.4X: ", pc);

	operand[0] = 0;

	if (fact) switch(fact->type) {
	        case 1: /* FCB */ {
	                int x;
	                for (x = 0; x != fact->len; ++x) {
	                        fcb_line(mem, at_pc, outbuf, flag);
	                }
	                return;
	        } case 2: /* FDB */ {
	                int x;
	                for (x = 0; x != fact->len; ++x) {
	                        fdb_line(mem, at_pc, outbuf, flag);
	                }
	                return;
	        } case 3: /* FCC */ {
	                fcc_line(mem, at_pc, outbuf, flag, fact->len);
	                return;
	        } case 4: /* RMB */ {
	                rmb_line(mem, at_pc, outbuf, flag, fact->len);
	                return;
	        }
	}

	sprintf(buf + strlen(buf), "%2.2X ", opcode = mem[pc++]);

	if (opcode & 0x80) {
		if ((opcode & 0x0F) < 0x0C) {
			/* Get operand A */
			if (opcode & 0x40) {
				sprintf(operand, "B");
			} else {
				sprintf(operand, "A");
			}
			switch (opcode & 0x30) {
				case 0x00: {
					ea = IMM();
					break;
				} case 0x10: {
					ea = DIR();
					break;
				} case 0x20: {
					ea = IDX();
					break;
				} case 0x30: {
					ea = EXT();
					break;
				}
			}
			switch (opcode & 0x30) {
				case 0x00: {
					sprintf(operand + strlen(operand), " #$%2.2X", mem[ea]);
					break;
				} case 0x10: {
				        ea_fact = ea;
					sprintf(operand + strlen(operand), " $%2.2X", ea);
					break;
				} case 0x20: {
					sprintf(operand + strlen(operand), " $%2.2X,X", mem[pc - 1]);
					break;
				} case 0x30: {
				        ea_fact = ea;
					sprintf(operand + strlen(operand), " $%4.4X", ea);
					break;
				}
			}
			/* Operate F = A op B */
			switch (opcode & 0x0F) {
				case 0x00: /* SUB N,Z,V,C (no H?) */ {
					insn = "SUB";
					break;
				} case 0x01: /* CMP N,Z,V,C (no H?) */ {
					insn = "CMP";
					break;
				} case 0x02: /* SBC N,Z,V,C (no H?) */ {
					insn = "SBC";
					break;
				} case 0x03: /* ??? */ {
					insn = "???";
					break;
				} case 0x04: /* AND N,Z,V=0 */ {
					insn = "AND";
					break;
				} case 0x05: /* BIT N,Z,V=0*/ {
					insn = "BIT";
					break;
				} case 0x06: /* LDA N,Z,V=0 */ {
					insn = "LD";
					break;
				} case 0x07: /* STA N,Z,V=0 */ {
					insn = "ST";
					break;
				} case 0x08: /* EOR N,Z,V=0*/ {
					insn = "EOR";
					break;
				} case 0x09: /* ADC H,N,Z,V,C */ {
					insn = "ADC";
					break;
				} case 0x0A: /* ORA N,Z,V=0 */ {
					insn = "OR";
					break;
				} case 0x0B: /* ADD H,N,Z,V,C */ {
					insn = "ADD";
					break;
				}
			}
		} else {
			/* Compute EA */
			switch (opcode & 0x30) {
				case 0x00: {
					if (opcode == 0x8D) {
						ea = IMM();
						offset = mem[ea];
						sprintf(operand, " $%4.4X", pc + offset);
					} else {
						ea = IMM2();
						sprintf(operand, " #$%4.4X", ea_fact = ((mem[pc - 2] << 8) + mem[pc -1]));
					}
					break;
				} case 0x10: {
					ea = DIR();
					ea_fact = ea;
					sprintf(operand, " $%4.4X", ea);
					break;
				} case 0x20: {
					ea = IDX();
					sprintf(operand, " $%2.2X,X", mem[pc - 1]);
					break;
				} case 0x30: {
					ea = EXT();
					ea_fact = ea;
					sprintf(operand, " $%4.4X", ea);
					break;
				}
			}
			switch (opcode) {
				case 0x8C: case 0x9C: case 0xAC: case 0xBC: /* CPX N,Z,V */ {
					insn = "CPX";
					break;
				} case 0x8D: /* BSR REL */ {
				        offset = mem[ea];
					branch_target = (pc + offset);
					is_jsr = 1;
					insn = "BSR";
					break;
				} case 0x8E: case 0x9E: case 0xAE: case 0xBE: /* LDS N,Z,V */ {
					insn = "LDS";
					break;
				} case 0x8F: case 0x9F: case 0xAF: case 0xBF: /* STS N,Z,V */ {
					insn = "STS";
					break;
				} case 0xAD: case 0xBD: /* JSR */ {
					branch_target = (mem[pc - 2] << 8) + mem[pc - 1];
					is_jsr = 1;
					insn = "JSR";
					break;
				} case 0xCE: case 0xDE: case 0xEE: case 0xFE: /* LDX N,Z,V */ {
					insn = "LDX";
					break;
				} case 0xCF: case 0xDF: case 0xEF: case 0xFF: /* STX N,Z,V */ {
					insn = "STX";
					break;
				} default: /* ??? */ {
					insn = "???";
					break;
				}
			}
		}
	} else if (opcode & 0x40) {
		switch (opcode & 0x30) {
			case 0x00: {
				sprintf(operand, "A");
				break;
			} case 0x10: {
				sprintf(operand, "B");
				break;
			} case 0x20: {
				ea = IDX();
				sprintf(operand, " $%2.2X,X", mem[pc - 1]);
				break;
			} case 0x30: {
				ea = EXT();
				ea_fact = ea;
				sprintf(operand, " $%4.4X", ea);
				break;
			}
		}
		switch (opcode & 0x0F) {
			case 0x00: /* NEG N, Z, V= ((f==0x80)?1:0), C= ((f==0)?1:0) */ {
				insn = "NEG";
				break;
			} case 0x01: /* ??? */ {
				insn = "???";
				break;
			} case 0x02: /* ??? */ {
				insn = "???";
				break;
			} case 0x03: /* COM N, Z, V=0, C=1 */ {
				insn = "COM";
				break;
			} case 0x04: /* LSR N=0,Z,C,V=N^C */ {
				insn = "LSR";
				break;
			} case 0x05: /* ??? */ {
				insn = "???";
				break;
			} case 0x06: /* ROR N,Z,C,V=N^C */ {
				insn = "ROR";
				break;
			} case 0x07: /* ASR N,Z,C,V=N^C */ {
				insn = "ASR";
				break;
			} case 0x08: /* ASL N,Z,C,V=N^C */ {
				insn = "ASL";
				break;
			} case 0x09: /* ROL N,Z,C,V=N^C */ {
				insn = "ROL";
				break;
			} case 0x0A: /* DEC N,Z,V = (a=0x80?1:0) */ {
				insn = "DEC";
				break;
			} case 0x0B: /* ??? */ {
				insn = "???";
				break;
			} case 0x0C: /* INC N,Z,V = (a=0x7F?1:0) */ {
				insn = "INC";
				break;
			} case 0x0D: /* TST N,Z,V=0,C=0 */ {
				insn = "TST";
				break;
			} case 0x0E: /* JMP */ {
				branch_target = ea;
				insn = "JMP";
				break;
			} case 0x0F: /* CLR N=0, Z=1, V=0, C=0 */ {
				insn = "CLR";
				break;
			}
		}
	} else {
		switch(opcode) {
			case 0x01: /* NOP */ { /* Do nothing */
				insn = "NOP";
				break;
			} case 0x06: /* TAP (all flags) */ {
				insn = "TAP";
				break;
			} case 0x07: /* TPA */ {
				insn = "TPA";
				break;
			} case 0x08: /* INX Z */ {
				insn = "INX";
				break;
			} case 0x09: /* DEX Z */ {
				insn = "DEC";
				break;
			} case 0x0A: /* CLV */ {
				insn = "CLV";
				break;
			} case 0x0B: /* SEV */ {
				insn = "SEV";
				break;
			} case 0x0C: /* CLC */ {
				insn = "CLC";
				break;
			} case 0x0D: /* SEC */ {
				insn = "SEC";
				break;
			} case 0x0E: /* CLI */ {
				insn = "CLI";
				break;
			} case 0x0F: /* SEI */ {
				insn = "SEI";
				break;
			} case 0x10: /* SBA N,Z,V,C */ {
				insn = "SBA";
				break;
			} case 0x11: /* CBA N,Z,V,C */ {
				insn = "CBA";
				break;
			} case 0x16: /* TAB N,Z,V=0 */ {
				insn = "TAB";
				break;
			} case 0x17: /* TBA N,Z,V=0 */ {
				insn = "TBA";
				break;
			} case 0x19: /* DAA N,Z,V,C */ {
				insn = "DAA";
				break;
			} case 0x1B: /* ABA H,N,Z,V,C */ {
				insn = "ABA";
				break;
			} case 0x20: /* BRA */ {
				offset = mem[pc++];
				sprintf(buf + strlen(buf), "%2.2X ", mem[pc - 1]);
				branch_target = pc + offset;
				sprintf(operand + strlen(operand), " $%4.4X", branch_target);
				insn = "BRA";
				break;
			} case 0x22: /* BHI */ {
				offset = mem[pc++];
				sprintf(buf + strlen(buf), "%2.2X ", mem[pc - 1]);
				branch_target = pc + offset;
				sprintf(operand + strlen(operand), " $%4.4X", branch_target);
				insn = "BHI";
				break;
			} case 0x23: /* BLS */ {
				offset = mem[pc++];
				sprintf(buf + strlen(buf), "%2.2X ", mem[pc - 1]);
				branch_target = pc + offset;
				sprintf(operand + strlen(operand), " $%4.4X", branch_target);
				insn = "BLS";
				break;
			} case 0x24: /* BCC */ {
				offset = mem[pc++];
				sprintf(buf + strlen(buf), "%2.2X ", mem[pc - 1]);
				branch_target = pc + offset;
				sprintf(operand + strlen(operand), " $%4.4X", branch_target);
				insn = "BCC";
				break;
			} case 0x25: /* BCS */ {
				offset = mem[pc++];
				sprintf(buf + strlen(buf), "%2.2X ", mem[pc - 1]);
				branch_target = pc + offset;
				sprintf(operand + strlen(operand), " $%4.4X", branch_target);
				insn = "BCS";
				break;
			} case 0x26: /* BNE */ {
				offset = mem[pc++];
				sprintf(buf + strlen(buf), "%2.2X ", mem[pc - 1]);
				branch_target = pc + offset;
				sprintf(operand + strlen(operand), " $%4.4X", branch_target);
				insn = "BNE";
				break;
			} case 0x27: /* BEQ */ {
				offset = mem[pc++];
				sprintf(buf + strlen(buf), "%2.2X ", mem[pc - 1]);
				branch_target = pc + offset;
				sprintf(operand + strlen(operand), " $%4.4X", branch_target);
				insn = "BEQ";
				break;
			} case 0x28: /* BVC */ {
				offset = mem[pc++];
				sprintf(buf + strlen(buf), "%2.2X ", mem[pc - 1]);
				branch_target = pc + offset;
				sprintf(operand + strlen(operand), " $%4.4X", branch_target);
				insn = "BVC";
				break;
			} case 0x29: /* BVS */ {
				offset = mem[pc++];
				sprintf(buf + strlen(buf), "%2.2X ", mem[pc - 1]);
				branch_target = pc + offset;
				sprintf(operand + strlen(operand), " $%4.4X", branch_target);
				insn = "BVS";
				break;
			} case 0x2A: /* BPL */ {
				offset = mem[pc++];
				sprintf(buf + strlen(buf), "%2.2X ", mem[pc - 1]);
				branch_target = pc + offset;
				sprintf(operand + strlen(operand), " $%4.4X", branch_target);
				insn = "BPL";
				break;
			} case 0x2B: /* BMI */ {
				offset = mem[pc++];
				sprintf(buf + strlen(buf), "%2.2X ", mem[pc - 1]);
				branch_target = pc + offset;
				sprintf(operand + strlen(operand), " $%4.4X", branch_target);
				insn = "BMI";
				break;
			} case 0x2C: /* BGE */ {
				offset = mem[pc++];
				sprintf(buf + strlen(buf), "%2.2X ", mem[pc - 1]);
				branch_target = pc + offset;
				sprintf(operand + strlen(operand), " $%4.4X", branch_target);
				insn = "BGE";
				break;
			} case 0x2D: /* BLT */ {
				offset = mem[pc++];
				sprintf(buf + strlen(buf), "%2.2X ", mem[pc - 1]);
				branch_target = pc + offset;
				sprintf(operand + strlen(operand), " $%4.4X", branch_target);
				insn = "BLT";
				break;
			} case 0x2E: /* BGT */ {
				offset = mem[pc++];
				sprintf(buf + strlen(buf), "%2.2X ", mem[pc - 1]);
				branch_target = pc + offset;
				sprintf(operand + strlen(operand), " $%4.4X", branch_target);
				insn = "BGT";
				break;
			} case 0x2F: /* BLE */ {
				offset = mem[pc++];
				sprintf(buf + strlen(buf), "%2.2X ", mem[pc - 1]);
				branch_target = pc + offset;
				sprintf(operand + strlen(operand), " $%4.4X", branch_target);
				insn = "BLE";
				break;
			} case 0x30: /* TSX */ {
				insn = "TSX";
				break;
			} case 0x31: /* INS */ {
				insn = "INS";
				break;
			} case 0x32: /* PULA */ {
				insn = "PULA";
				break;
			} case 0x33: /* PULB */ {
				insn = "PULB";
				break;
			} case 0x34: /* DES */ {
				insn = "DES";
				break;
			} case 0x35: /* TXS */ {
				insn = "TXS";
				break;
			} case 0x36: /* PSHA */ {
				insn = "PSHA";
				break;
			} case 0x37: /* PSHB */ {
				insn = "PSHB";
				break;
			} case 0x39: /* RTS */ {
				insn = "RTS";
				break;
			} case 0x3B: /* RTI */ {
				insn = "RTI";
				break;
			} case 0x3E: /* WAI */ {
				insn = "WAI";
				break;
			} case 0x3F: /* SWI */ {
				insn = "SWI";
				ea = IMM();
				sprintf(operand + strlen(operand), " $%2.2X", mem[ea]);
				is_swi = mem[ea];
				break;
			} default: /* ??? */ {
				insn = "???";
				break;
			}
		}
	}
	if (fact)
	        sprintf(buf1, "%-10s ", fact->label);
	else if (flg)
		sprintf(buf1, "L%4.4d      ", flg);
	else
		sprintf(buf1, "           ");
	strcat(buf1, insn);
	strcat(buf1, operand);
	if (ea_fact == -1 && (branch_target >= 0 && branch_target <= 0xFFFF))
	        ea_fact = branch_target;
	if (ea_fact != -1) {
	        struct fact *f = facts[ea_fact];
	        if (f) {
	                sprintf(buf1 + strlen(buf1)," [%s %s]", f->label,f->comment);
	        }
	}
	sprintf(outbuf, "%-15s%-9s",buf, buf1);
	if (is_swi != -1) {
	        struct fact *f;
	        for (f = swi_facts;f;f = f->next)
	                if (f->len == is_swi)
	                        break;
                if (f) {
                        sprintf(outbuf + strlen(outbuf), " (%s    %s)",f->label,f->comment);
                }
	} else if (branch_target > 0 && branch_target < 0x10000 && facts[branch_target]) {
	        /* sprintf(outbuf + strlen(outbuf), " (to %s [%s])", facts[branch_target]->label, facts[branch_target]->comment); */
	} else if (branch_target > 0 && branch_target < 0x10000 && targets[branch_target])
		sprintf(outbuf + strlen(outbuf), " (to L%4.4d)", targets[branch_target]);

        if (fact && flag && fact->type == 5) {
                printf("\n");
                printf("* %s\n", fact->comment);
                printf("\n");
        } else if (fact)
                sprintf(outbuf + strlen(outbuf),"			* %s", fact->comment);
	*at_pc = pc;
	if (is_jsr)
		*at_target = branch_target;
}
