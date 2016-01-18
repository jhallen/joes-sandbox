/*	M6800 Simulator
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

#include "exor.h"
#include "unasm6800.h"
#include "asm6800.h"
#include "sim6800.h"

int skip = 0; /* Skip first nn instructions in trace */
int trace = 0; /* Enable instruction trace */
int stop; /* Stop flag */
int reset; /* User hit reset */
int abrt; /* User hit abort */
int sp_stop;

/* CPU registers */
unsigned char acca;
unsigned char accb;
unsigned short ix;
unsigned short pc;
unsigned short sp;
unsigned char c_flag;
unsigned char v_flag;
unsigned char z_flag;
unsigned char n_flag;
unsigned char i_flag; /* 1=masked, 0=enabled */
unsigned char h_flag;

/* Breakpoint */
int brk;
unsigned short brk_addr;

/* Trace buffer */

int trace_idx;

#define TRACESIZE 128

struct trace_entry
{
	unsigned short ix;
	unsigned short pc;
	unsigned short sp;
	unsigned short ea;
	unsigned short data;
	unsigned char acca;
	unsigned char accb;
	unsigned char cc;
	unsigned char insn[3];
} trace_buf[TRACESIZE];

unsigned char read_flags()
{
	return (0xC0 + c_flag + (v_flag << 1) + (z_flag << 2) + (n_flag << 3) + (i_flag << 4) + (h_flag << 5));
}

void write_flags(unsigned char f)
{
	c_flag = (f & 1);
	v_flag = ((f >> 1) & 1);
	z_flag = ((f >> 2) & 1);
	n_flag = ((f >> 3) & 1);
	i_flag = ((f >> 4) & 1);
	h_flag = ((f >> 5) & 1);
}

unsigned short mread2(unsigned short addr)
{
	return (mread(addr) << 8) + mread(addr + 1);
}

unsigned char fetch()
{
        unsigned char c = mread(pc++);
	return c;
}

unsigned short fetch2()
{
        unsigned short v;
        v = fetch();
        v = (v << 8) + fetch();
        return v;
}

void mwrite2(unsigned short addr, unsigned short data)
{
	mwrite(addr, (data >> 8));
	mwrite(addr + 1, (data & 0xFF));
}

void push(unsigned char data)
{
//        printf("push %x\n", data);
	mwrite(sp--, data);
}

unsigned char pull()
{
	return mread(++sp);
}

void push2(unsigned short data)
{
	push(data & 0xFF);
	push(data >> 8);
}

unsigned short pull2()
{
	unsigned short w;
	w = pull();
	w = (w << 8) + pull();
	return w;
}

#define IDX() (ix + fetch())
#define IMM() ((pc += 1), (pc - 1))
#define IMM2() ((pc += 2), (pc - 2))
#define DIR() (fetch())
#define EXT() (fetch2())

/* Macros which update flags following an arithmetic or logical operation */

#define Z_16(n) ((~((n) | -(n)) >> 15) & 1)
#define Z(n) ((~((n) | -(n)) >> 7) & 1)
#define N_16(n) (n >> 15)
#define N(n) (n >> 7)
#define V_16(a,b,f) ((((f) ^ (a)) & ((f) ^ (b))) >> 15)
#define V(a,b,f) ((((f) ^ (a)) & ((f) ^ (b))) >> 7)
#define C(a,b,f) ((((a) & (b)) | (((a) | (b)) & ~(f))) >> 7)
#define V_SUB(a,b,f) ((((a) ^ (b)) & ((f) ^ (a))) >> 7)
#define C_SUB(a,b,f) (((~(a) & (b)) | ((~(a) | (b)) & (f))) >> 7)
#define H(a,b,f) ((((f) ^ (a) ^ (b)) >> 4) & 1)

/* Print one trace line */

void show_trace(int insn_no, struct trace_entry *t)
{
        char *fact_label;
        char *fact_comment;
        char *ea_label;

        char buf_ea[80];
	char buf1[80];
	char buf[80]; /* Address and fetched data */
	char operand[20]; /* Address mode / operand */
	char buf3[80]; /* Effective address and data */
	char *insn = "Huh?";
	int subr = 0;
	operand[0] = 0;
	buf3[0] = 0;
	buf[0] = 0;

	/* Any facts about this address? */
	fact_label = find_label(t->pc);
	fact_comment = "";
	if (!fact_label) {
                if (facts[t->pc]) {
                        fact_label = facts[t->pc]->label;
                        fact_comment = facts[t->pc]->comment;
                } else {
                        fact_label = "";
                }
        }

        /* About the effective address? */

        ea_label = find_label(t->ea);
        if (!ea_label && facts[t->ea])
                ea_label = facts[t->ea]->label;
	if (ea_label)
	        sprintf(buf_ea, "(%s)", ea_label);
        else
                buf_ea[0] = 0;

	if (!(t->cc & 0x40)) {
                if (insn_no >= skip) {
                        fprintf(mon_out, " %10d ---- Subroutine at %4.4X processed by simulator ---- RTS executed ---\n\n",
                               insn_no, t->pc);
                        return;
                }
	}

	sprintf(buf,"%4.4X: ", t->pc);

        sprintf(buf + strlen(buf), "%2.2X ", t->insn[0]);

	if (t->insn[0] & 0x80) {
		if ((t->insn[0] & 0x0F) < 0x0C) {
			if (t->insn[0] & 0x40) {
				sprintf(operand, "B");
			} else {
				sprintf(operand, "A");
			}
			if (t->cc & 0x80)
        			sprintf(buf3, "EA=%4.4X%s D=%2.2X", t->ea, buf_ea, t->data);
			switch (t->insn[0] & 0x30) {
				case 0x00: {
					sprintf(buf + strlen(buf), "%2.2X ", t->insn[1]);
					sprintf(operand + strlen(operand), " #%2.2X", t->insn[1]);
					break;
				} case 0x10: {
					sprintf(buf + strlen(buf), "%2.2X ", t->insn[1]);
					sprintf(operand + strlen(operand), " %2.2X", t->insn[1]);
					break;
				} case 0x20: {
					sprintf(buf + strlen(buf), "%2.2X ", t->insn[1]);
					sprintf(operand + strlen(operand), " %2.2X,X", t->insn[1]);
					break;
				} case 0x30: {
					sprintf(buf + strlen(buf), "%2.2X ", t->insn[1]);
					sprintf(buf + strlen(buf), "%2.2X ", t->insn[2]);
					sprintf(operand + strlen(operand), " %4.4X", (t->insn[1] << 8) + t->insn[2]);
					break;
				}
			}
			/* Operate F = A op B */
			switch (t->insn[0] & 0x0F) {
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
					goto invalid;
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
			switch (t->insn[0] & 0x30) {
				case 0x00: {
				        if (t->insn[0] == 0x8D) {
        					sprintf(buf + strlen(buf), "%2.2X ", t->insn[1]);
        					sprintf(operand, " $%4.4X", t->pc + (char)t->insn[1]);
                                        } else {
        					sprintf(buf + strlen(buf), "%2.2X ", t->insn[1]);
        					sprintf(buf + strlen(buf), "%2.2X ", t->insn[2]);
        					sprintf(operand, " #$%4.4X", (t->insn[1] << 8) + t->insn[2]);
                                        }
					break;
				} case 0x10: {
					sprintf(buf + strlen(buf), "%2.2X ", t->insn[1]);
					sprintf(operand, " %2.2X", t->insn[1]);
					break;
				} case 0x20: {
					sprintf(buf + strlen(buf), "%2.2X ", t->insn[1]);
					sprintf(operand, " %2.2X,X", t->insn[1]);
					break;
				} case 0x30: {
					sprintf(buf + strlen(buf), "%2.2X ", t->insn[1]);
					sprintf(buf + strlen(buf), "%2.2X ", t->insn[2]);
					sprintf(operand, " %4.4X", (t->insn[1] << 8) + t->insn[2]);
					break;
				}
			}
			switch (t->insn[0]) {
				case 0x8C: case 0x9C: case 0xAC: case 0xBC: /* CPX N,Z,V */ {
				        if (t->cc & 0x80)
        					sprintf(buf3, "EA=%4.4X%s D=%4.4X", t->ea, buf_ea, t->data);
					insn = "CPX";
					break;
				} case 0x8D: /* BSR REL */ {
					sprintf(operand, " %4.4X", t->pc + 2 + (char)t->insn[1]);
				        if (t->cc & 0x80)
                                                sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
					subr = 1;
					insn = "BSR";
					break;
				} case 0x8E: case 0x9E: case 0xAE: case 0xBE: /* LDS N,Z,V */ {
				        if (t->cc & 0x80)
        					sprintf(buf3, "EA=%4.4X%s D=%4.4X", t->ea, buf_ea, t->data);
					insn = "LDS";
					break;
				} case 0x8F: case 0x9F: case 0xAF: case 0xBF: /* STS N,Z,V */ {
				        if (t->cc & 0x80)
        					sprintf(buf3, "EA=%4.4X%s D=%4.4X", t->ea, buf_ea, t->data);
					insn = "STS";
					break;
				} case 0xAD: case 0xBD: /* JSR */ {
				        if (t->cc & 0x80)
        					sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
					insn = "JSR";
					subr = 1;
					break;
				} case 0xCE: case 0xDE: case 0xEE: case 0xFE: /* LDX N,Z,V */ {
				        if (t->cc & 0x80)
        					sprintf(buf3, "EA=%4.4X%s D=%4.4X", t->ea, buf_ea, t->data);
					insn = "LDX";
					break;
				} case 0xCF: case 0xDF: case 0xEF: case 0xFF: /* STX N,Z,V */ {
				        if (t->cc & 0x80)
        					sprintf(buf3, "EA=%4.4X%s D=%4.4X", t->ea, buf_ea, t->ix);
					insn = "STX";
					break;
				} default: /* ??? */ {
					goto invalid;
					break;
				}
			}
		}
	} else if (t->insn[0] & 0x40) {
		switch (t->insn[0] & 0x30) {
			case 0x00: {
				sprintf(operand, "A");
				break;
			} case 0x10: {
				sprintf(operand, "B");
				break;
			} case 0x20: {
                                if (t->cc & 0x80)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				sprintf(buf + strlen(buf), "%2.2X ", t->insn[1]);
				sprintf(operand, " %2.2X,X", t->insn[1]);
				break;
			} case 0x30: {
                                if (t->cc & 0x80)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				sprintf(buf + strlen(buf), "%2.2X ", t->insn[1]);
				sprintf(buf + strlen(buf), "%2.2X ", t->insn[2]);
				sprintf(operand, " %4.4X", (t->insn[1] << 8) + t->insn[2]);
				break;
			}
		}
		switch (t->insn[0] & 0x0F) {
			case 0x00: /* NEG N, Z, V= ((f==0x80)?1:0), C= ((f==0)?1:0) */ {
				insn = "NEG";
				break;
			} case 0x01: /* ??? */ {
				goto invalid;
				break;
			} case 0x02: /* ??? */ {
				goto invalid;
				break;
			} case 0x03: /* COM N, Z, V=0, C=1 */ {
				insn = "COM";
				break;
			} case 0x04: /* LSR N=0,Z,C,V=N^C */ {
				insn = "LSR";
				break;
			} case 0x05: /* ??? */ {
				goto invalid;
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
				goto invalid;
				break;
			} case 0x0C: /* INC N,Z,V = (a=0x7F?1:0) */ {
				insn = "INC";
				break;
			} case 0x0D: /* TST N,Z,V=0,C=0 */ {
				insn = "TST";
				break;
			} case 0x0E: /* JMP */ {
				insn = "JMP";
				break;
			} case 0x0F: /* CLR N=0, Z=1, V=0, C=0 */ {
				insn = "CLR";
				break;
			}
		}
	} else {
		switch(t->insn[0]) {
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
				sprintf(buf + strlen(buf), "%2.2X ", t->insn[1]);
				sprintf(operand, " %4.4X", t->pc + 2 + (char)t->insn[1]);
                                if (t->cc & 0x80)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				insn = "BRA";
				break;
			} case 0x22: /* BHI */ {
				sprintf(buf + strlen(buf), "%2.2X ", t->insn[1]);
				sprintf(operand, " %4.4X", t->pc + 2 + (char)t->insn[1]);
                                if (t->cc & 0x80)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				insn = "BHI";
				break;
			} case 0x23: /* BLS */ {
				sprintf(buf + strlen(buf), "%2.2X ", t->insn[1]);
				sprintf(operand, " %4.4X", t->pc + 2 + (char)t->insn[1]);
                                if (t->cc & 0x80)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				insn = "BLS";
				break;
			} case 0x24: /* BCC */ {
				sprintf(buf + strlen(buf), "%2.2X ", t->insn[1]);
				sprintf(operand, " %4.4X", t->pc + 2 + (char)t->insn[1]);
                                if (t->cc & 0x80)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				insn = "BCC";
				break;
			} case 0x25: /* BCS */ {
				sprintf(buf + strlen(buf), "%2.2X ", t->insn[1]);
				sprintf(operand, " %4.4X", t->pc + 2 + (char)t->insn[1]);
                                if (t->cc & 0x80)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				insn = "BCS";
				break;
			} case 0x26: /* BNE */ {
				sprintf(buf + strlen(buf), "%2.2X ", t->insn[1]);
				sprintf(operand, " %4.4X", t->pc + 2 + (char)t->insn[1]);
                                if (t->cc & 0x80)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				insn = "BNE";
				break;
			} case 0x27: /* BEQ */ {
				sprintf(buf + strlen(buf), "%2.2X ", t->insn[1]);
				sprintf(operand, " %4.4X", t->pc + 2 + (char)t->insn[1]);
                                if (t->cc & 0x80)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				insn = "BEQ";
				break;
			} case 0x28: /* BVC */ {
				sprintf(buf + strlen(buf), "%2.2X ", t->insn[1]);
				sprintf(operand, " %4.4X", t->pc + 2 + (char)t->insn[1]);
                                if (t->cc & 0x80)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				insn = "BVC";
				break;
			} case 0x29: /* BVS */ {
				sprintf(buf + strlen(buf), "%2.2X ", t->insn[1]);
				sprintf(operand, " %4.4X", t->pc + 2 + (char)t->insn[1]);
                                if (t->cc & 0x80)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				insn = "BVS";
				break;
			} case 0x2A: /* BPL */ {
				sprintf(buf + strlen(buf), "%2.2X ", t->insn[1]);
				sprintf(operand, " %4.4X", t->pc + 2 + (char)t->insn[1]);
                                if (t->cc & 0x80)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				insn = "BPL";
				break;
			} case 0x2B: /* BMI */ {
				sprintf(buf + strlen(buf), "%2.2X ", t->insn[1]);
				sprintf(operand, " %4.4X", t->pc + 2 + (char)t->insn[1]);
                                if (t->cc & 0x80)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				insn = "BMI";
				break;
			} case 0x2C: /* BGE */ {
				sprintf(buf + strlen(buf), "%2.2X ", t->insn[1]);
				sprintf(operand, " %4.4X", t->pc + 2 + (char)t->insn[1]);
                                if (t->cc & 0x80)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				insn = "BGE";
				break;
			} case 0x2D: /* BLT */ {
				sprintf(buf + strlen(buf), "%2.2X ", t->insn[1]);
				sprintf(operand, " %4.4X", t->pc + 2 + (char)t->insn[1]);
                                if (t->cc & 0x80)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				insn = "BLT";
				break;
			} case 0x2E: /* BGT */ {
				sprintf(buf + strlen(buf), "%2.2X ", t->insn[1]);
				sprintf(operand, " %4.4X", t->pc + 2 + (char)t->insn[1]);
                                if (t->cc & 0x80)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				insn = "BGT";
				break;
			} case 0x2F: /* BLE */ {
				sprintf(buf + strlen(buf), "%2.2X ", t->insn[1]);
				sprintf(operand, " %4.4X", t->pc + 2 + (char)t->insn[1]);
                                if (t->cc & 0x80)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
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
				subr = 1;
				break;
			} case 0x3B: /* RTI */ {
				insn = "RTI";
				subr = 1;
				break;
			} case 0x3E: /* WAI */ {
				insn = "WAI";
				break;
			} case 0x3F: /* SWI */ {
				insn = "SWI";
				break;
			} default: /* ??? */ {
				goto invalid;
				break;
			}
		}
	}
	invalid:

	strcpy(buf1, insn);
	strcat(buf1, operand);
	if (insn_no >= skip) {
	        if (pc == t->pc)
	                fprintf(mon_out, ">");
                else
                        fprintf(mon_out, " ");
		fprintf(mon_out, "%10d A=%2.2X B=%2.2X X=%4.4X SP=%4.4X %c%c%c%c%c%c %-8s %-15s%-9s %-14s %s\n",
		       insn_no, t->acca, t->accb, t->ix, t->sp,
		       ((t->cc & 32) ? 'H' : '-'), ((t->cc & 16) ? 'I' : '-'),
		       ((t->cc & 8) ? 'N' :'-'), ((t->cc & 4) ? 'Z' : '-'),
		       ((t->cc & 2) ? 'V' : '-'), ((t->cc & 1) ? 'C' : '-'), fact_label, buf, buf1, buf3, fact_comment);
		if (subr)
		        fprintf(mon_out, "\n");
        }
}

void simulated(unsigned short addr)
{
        struct trace_entry *t = trace_buf + (trace_idx++ & (TRACESIZE - 1));
        t->cc &= ~0x40;
        t->pc = addr;
}

/* Show trace buffer */

void show_traces(int n)
{
	int x;
	for (x = 0; x != n; ++x) {
		show_trace(trace_idx + x - n, trace_buf + ((trace_idx + x - n) & (TRACESIZE - 1)));
	}
}

/* This is the simulator */

void sim(void)
{
        unsigned char opcode;
	for (;;) {
	        int org_trace_idx = trace_idx;
		struct trace_entry *t = (trace_buf + (trace_idx++ & (TRACESIZE - 1)));
		int ti = 0;
		char offset;
		unsigned char imm;
		unsigned short ea;
		unsigned char a;
		unsigned char b;
		unsigned char f;
		unsigned char data;
		unsigned short w;
		unsigned short fw;
		int wb;

		wb = 1;

		t->pc = pc;
                t->acca = acca;
                t->accb = accb;
                t->ix = ix;
                t->sp = sp;
                t->cc = (read_flags() & 0x7F);
		t->insn[0] = mread(pc);
		t->insn[1] = mread(pc + 1);
		t->insn[2] = mread(pc + 2);
		t->ea = 0;
		t->data = 0;

		if (brk && brk_addr == pc || stop) {
		        if (brk && brk_addr == pc)
        		        printf("\r\nBreakpoint!\n");
		        monitor();
                        t->pc = pc;
                        t->acca = acca;
                        t->accb = accb;
                        t->ix = ix;
                        t->sp = sp;
                        t->cc = (read_flags() & 0x7F);
                        t->insn[0] = mread(pc);
                        t->insn[1] = mread(pc + 1);
                        t->insn[2] = mread(pc + 2);
                        t->ea = 0;
                        t->data = 0;
		}

                if (reset) {
                        reset = 0;
                        abrt = 0;
                        pc = 0xF000;
                        if (trace)
                                printf("           RESET!\n");
                        t->pc = pc;
                        t->acca = acca;
                        t->accb = accb;
                        t->ix = ix;
                        t->sp = sp;
                        t->cc = (read_flags() & 0x7F);
                        t->insn[0] = mread(pc);
                        t->insn[1] = mread(pc + 1);
                        t->insn[2] = mread(pc + 2);
                        t->ea = 0;
                        t->data = 0;
                }

                if (abrt) {
                        abrt = 0;
                        push2(pc);
                        push2(ix);
                        push(acca);
                        push(accb);
                        push(read_flags());
                        jump(mread2(0xFFFC));
                        printf("           NMI! to PC=%4.4X\n", pc);
                        t->pc = pc;
                        t->acca = acca;
                        t->accb = accb;
                        t->ix = ix;
                        t->sp = sp;
                        t->cc = (read_flags() & 0x7F);
                        t->insn[0] = mread(pc);
                        t->insn[1] = mread(pc + 1);
                        t->insn[2] = mread(pc + 2);
                        t->ea = 0;
                        t->data = 0;
                }

		opcode = fetch();

		if (opcode & 0x80) {
			if ((opcode & 0x0F) < 0x0C) {
				/* Get operand A */
				if (opcode & 0x40) {
					a = accb;
				} else {
					a = acca;
				}
				/* Get operand B, record effective address */
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
				if ((opcode & 0x0F) != 0x07) {
        				b = mread(ea);
        				t->ea = ea;
        				t->data = b;
                                } else {
                                        b = 0;
                                        t->ea = ea;
                                        t->data = a;
                                }
				/* Operate F = A op B */
				switch (opcode & 0x0F) {
					case 0x00: /* SUB N,Z,V,C (no H?) */ {
						f = a - b;
						c_flag = C_SUB(a,b,f);
						v_flag = V_SUB(a,b,f);
						n_flag = N(f);
						z_flag = Z(f);
						break;
					} case 0x01: /* CMP N,Z,V,C (no H?) */ {
						f = a - b;
						c_flag = C_SUB(a,b,f);
						v_flag = V_SUB(a,b,f);
						n_flag = N(f);
						z_flag = Z(f);
						f = a;
						break;
					} case 0x02: /* SBC N,Z,V,C (no H?) */ {
						f = a - b - c_flag;
						c_flag = C_SUB(a,b,f);
						v_flag = V_SUB(a,b,f);
						n_flag = N(f);
						z_flag = Z(f);
						break;
					} case 0x03: /* ??? */ {
					        goto invalid;
					        break;
					} case 0x04: /* AND N,Z,V=0 */ {
						f = (a & b);
						n_flag = N(f);
						z_flag = Z(f);
						v_flag = 0;
						break;
					} case 0x05: /* BIT N,Z,V=0*/ {
						f = (a & b);
						n_flag = N(f);
						z_flag = Z(f);
						v_flag = 0;
						f = a;
						break;
					} case 0x06: /* LDA N,Z,V=0 */ {
						f = b;
						n_flag = N(f);
						z_flag = Z(f);
						v_flag = 0;
						break;
					} case 0x07: /* STA N,Z,V=0 */ {
					        f = a;
					        n_flag = N(f);
					        z_flag = Z(f);
					        v_flag = 0;
					        mwrite(ea, f);
					        break;
					} case 0x08: /* EOR N,Z,V=0*/ {
						f = a ^ b;
						n_flag = N(f);
						z_flag = Z(f);
						v_flag = 0;
						break;
					} case 0x09: /* ADC H,N,Z,V,C */ {
						f = a + b + c_flag;
						v_flag = V(a,b,f);
						c_flag = C(a,b,f);
						n_flag = N(f);
						z_flag = Z(f);
						h_flag = H(a,b,f);
						break;
					} case 0x0A: /* ORA N,Z,V=0 */ {
						f = a | b;
						n_flag = N(f);
						z_flag = Z(f);
						v_flag = 0;
						break;
					} case 0x0B: /* ADD H,N,Z,V,C */ {
						f = a + b;
						v_flag = V(a,b,f);
						c_flag = C(a,b,f);
						n_flag = N(f);
						z_flag = Z(f);
						h_flag = H(a,b,f);
						break;
					}
				}
				/* Write back */
				if (opcode & 0x40) {
					accb = f;
				} else {
					acca = f;
				}
			} else {
				/* Compute EA */
				switch (opcode & 0x30) {
					case 0x00: {
						ea = IMM2();
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
				switch (opcode) {
					case 0x8C: case 0x9C: case 0xAC: case 0xBC: /* CPX N,Z,V */ {
						w = mread2(ea);
                                                t->ea = ea; t->data = w;
						fw = ix - w;
						z_flag = Z_16(fw);
						n_flag = N_16(fw);
						v_flag = V_16(ix, w, fw);
						break;
					} case 0x8D: /* BSR REL */ {
						push2(pc - 1);
						jump(t->ea = (pc - 1 + (char)mread(ea)));
						break;
					} case 0x8E: case 0x9E: case 0xAE: case 0xBE: /* LDS N,Z,V */ {
						sp = mread2(ea);
                                                t->ea = ea; t->data = sp;
						z_flag = Z_16(sp);
						n_flag = N_16(sp);
						v_flag = 0;
						break;
					} case 0x8F: case 0x9F: case 0xAF: case 0xBF: /* STS N,Z,V */ {
						mwrite2(ea, sp);
                                                t->ea = ea; t->data = sp;
						z_flag = Z_16(sp);
						n_flag = N_16(sp);
						v_flag = 0;
						break;
					} case 0xAD: case 0xBD: /* JSR */ {
						push2(pc);
						jump(ea);
                                                t->ea = ea;
						break;
					} case 0xCE: case 0xDE: case 0xEE: case 0xFE: /* LDX N,Z,V */ {
						ix = mread2(ea);
                                                t->ea = ea; t->data = ix;
						z_flag = Z_16(ix);
						n_flag = N_16(ix);
						v_flag = 0;
						break;
					} case 0xCF: case 0xDF: case 0xEF: case 0xFF: /* STX N,Z,V */ {
						mwrite2(ea, ix);
                                                t->ea = ea; t->data = ix;
						z_flag = Z_16(ix);
						n_flag = N_16(ix);
						v_flag = 0;
						break;
					} default: /* ??? */ {
					        goto invalid;
						break;
					}
				}
			}
                } else if (opcode & 0x40) {
                	switch (opcode & 0x30) {
                		case 0x00: {
                		        b = acca;
                                        t->data = b;
                		        break;
                                } case 0x10: {
                                        b = accb;
                                        t->data = b;
                                        break;
                                } case 0x20: {
                                        ea = IDX();
                                        b = mread(ea);
                                        t->ea = ea; t->data = b;
                                        break;
                                } case 0x30: {
                                        ea = EXT();
                                        b = mread(ea);
                                        t->ea = ea; t->data = b;
                                        break;
                                }
                	}
			switch (opcode & 0x0F) {
				case 0x00: /* NEG N, Z, V= ((f==0x80)?1:0), C= ((f==0)?1:0) */ {
					f = -b;
					v_flag = ((b & f) >> 7);
					n_flag = N(f);
					z_flag = Z(f);
					c_flag = z_flag;
					break;
				} case 0x01: /* ??? */ {
				        goto invalid;
					break;
				} case 0x02: /* ??? */ {
				        goto invalid;
					break;
				} case 0x03: /* COM N, Z, V=0, C=1 */ {
					f = ~b;
					c_flag = 1;
					n_flag = N(f);
					z_flag = Z(f);
					v_flag = 0;
					break;
				} case 0x04: /* LSR N=0,Z,C,V=N^C */ {
					f = (b >> 1);
					c_flag = (b & 1);
					n_flag = 0;
					z_flag = Z(f);
					v_flag = c_flag;
					break;
				} case 0x05: /* ??? */ {
				        goto invalid;
					break;
				} case 0x06: /* ROR N,Z,C,V=N^C */ {
					f = (b >> 1) + (c_flag << 7);
					c_flag = (b & 1);
					z_flag = Z(f);
					n_flag = N(f);
					v_flag = (n_flag ^ c_flag);
					break;
				} case 0x07: /* ASR N,Z,C,V=N^C */ {
					f = (b >> 1) + (b & 0x80);
					c_flag = (b & 1);
					z_flag = Z(f);
					n_flag = N(f);
					v_flag = (n_flag ^ c_flag);
					break;
				} case 0x08: /* ASL N,Z,C,V=N^C */ {
					f = (b << 1);
					c_flag = (b >> 7);
					z_flag = Z(f);
					n_flag = N(f);
					v_flag = (n_flag ^ c_flag);
					break;
				} case 0x09: /* ROL N,Z,C,V=N^C */ {
					f = (b << 1) + c_flag;
					c_flag = (b >> 7);
					z_flag = Z(f);
					n_flag = N(f);
					v_flag = (n_flag ^ c_flag);
					break;
				} case 0x0A: /* DEC N,Z,V = (a=0x80?1:0) */ {
					f = b - 1;
					z_flag = Z(f);
					n_flag = N(f);
					v_flag = (((~f & b) >> 7) & 1);
					break;
				} case 0x0B: /* ??? */ {
				        goto invalid;
					break;
				} case 0x0C: /* INC N,Z,V = (a=0x7F?1:0) */ {
					f = b + 1;
					z_flag = Z(f);
					n_flag = N(f);
					v_flag = (((f & ~b) >> 7) & 1);
					break;
				} case 0x0D: /* TST N,Z,V=0,C=0 */ {
				        f = b;
					z_flag = Z(f);
					n_flag = N(f);
					v_flag = 0;
					c_flag = 0;
					wb = 0;
					break;
				} case 0x0E: /* JMP */ {
				        wb = 0;
				        jump(ea);
					break;
				} case 0x0F: /* CLR N=0, Z=1, V=0, C=0 */ {
					f = 0;
					n_flag = 0;
					z_flag = 1;
					v_flag = 0;
					c_flag = 0;
					break;
				}
			}
			if (wb) switch (opcode & 0x30) {
                		case 0x00: {
                		        acca = f;
                		        break;
                                } case 0x10: {
                                        accb = f;
                                        break;
                                } case 0x20: {
                                        mwrite(ea, f);
                                        break;
                                } case 0x30: {
                                        mwrite(ea, f);
                                        break;
                                }
                	}
                } else {
                	switch(opcode) {
				case 0x01: /* NOP */ { /* Do nothing */
					break;
				} case 0x06: /* TAP (all flags) */ {
					write_flags(acca);
					break;
				} case 0x07: /* TPA */ {
					acca = read_flags();
					break;
				} case 0x08: /* INX Z */ {
					ix = ix + 1;
					z_flag = Z_16(ix);
					break;
				} case 0x09: /* DEX Z */ {
					ix = ix - 1;
					z_flag = Z_16(ix);
					break;
				} case 0x0A: /* CLV */ {
					v_flag = 0;
					break;
				} case 0x0B: /* SEV */ {
					v_flag = 1;
					break;
				} case 0x0C: /* CLC */ {
					c_flag = 0;
					break;
				} case 0x0D: /* SEC */ {
					c_flag = 1;
					break;
				} case 0x0E: /* CLI */ {
					i_flag = 0;
					break;
				} case 0x0F: /* SEI */ {
					i_flag = 1;
					break;
				} case 0x10: /* SBA N,Z,V,C */ {
					f = acca - accb;
					c_flag = C_SUB(acca,accb,f);
					v_flag = V_SUB(acca,accb,f);
					n_flag = N(f);
					z_flag = Z(f);
					acca = f;
					break;
				} case 0x11: /* CBA N,Z,V,C */ {
					f = acca - accb;
					c_flag = C_SUB(acca,accb,f);
					v_flag = V_SUB(acca,accb,f);
					n_flag = N(f);
					z_flag = Z(f);
					break;
				} case 0x16: /* TAB N,Z,V=0 */ {
					accb = acca;
					z_flag = Z(accb);
					n_flag = N(accb);
					v_flag = 0;
					break;
				} case 0x17: /* TBA N,Z,V=0 */ {
					acca = accb;
					z_flag = Z(acca);
					n_flag = N(acca);
					v_flag = 0;
					break;
				} case 0x19: /* DAA N,Z,V,C */ {
				        /* Only set C, don't clear it */
				        /* Do not change H */
				        if (h_flag || (acca & 0x0F) >= 0x0A) {
				                acca += 0x06;
				        }
				        if (c_flag || (acca & 0xF0) >= 0xA0) {
				                acca += 0x60;
				                c_flag = 1;
				        }
				        n_flag = N(acca);
				        z_flag = Z(acca);
				        /* ??? What is V supposed to be? */
				        break;
				} case 0x1B: /* ABA H,N,Z,V,C */ {
					f = acca + accb;
					v_flag = V(acca,accb,f);
					c_flag = C(acca,accb,f);
					n_flag = N(f);
					z_flag = Z(f);
					h_flag = H(a,b,f);
					acca = f;
					break;
				} case 0x20: /* BRA */ {
					offset = fetch();
					jump(t->ea = (pc + offset));
					break;
				} case 0x22: /* BHI */ {
					offset = fetch();
					t->ea = pc + offset;
					if (!(c_flag | z_flag))
					        jump(pc + offset);
					break;
				} case 0x23: /* BLS */ {
					offset = fetch();
					t->ea = pc + offset;
					if (c_flag | z_flag)
					        jump(pc + offset);
					break;
				} case 0x24: /* BCC */ {
					offset = fetch();
					t->ea = pc + offset;
					if (!c_flag)
					        jump(pc + offset);
					break;
				} case 0x25: /* BCS */ {
					offset = fetch();
					t->ea = pc + offset;
					if (c_flag)
					        jump(pc + offset);
					break;
				} case 0x26: /* BNE */ {
					offset = fetch();
					t->ea = pc + offset;
					if (!z_flag)
					        jump(pc + offset);
					break;
				} case 0x27: /* BEQ */ {
					offset = fetch();
					t->ea = pc + offset;
					if (z_flag)
					        jump(pc + offset);
					break;
				} case 0x28: /* BVC */ {
					offset = fetch();
					t->ea = pc + offset;
					if (!v_flag)
					        jump(pc + offset);
					break;
				} case 0x29: /* BVS */ {
					offset = fetch();
					t->ea = pc + offset;
					if (v_flag)
					        jump(pc + offset);
					break;
				} case 0x2A: /* BPL */ {
					offset = fetch();
					t->ea = pc + offset;
					if (!n_flag)
					        jump(pc + offset);
					break;
				} case 0x2B: /* BMI */ {
					offset = fetch();
					t->ea = pc + offset;
					if (n_flag)
					        jump(pc + offset);
					break;
				} case 0x2C: /* BGE */ {
					offset = fetch();
					t->ea = pc + offset;
					if (!(n_flag ^ v_flag))
					        jump(pc + offset);
					break;
				} case 0x2D: /* BLT */ {
					offset = fetch();
					t->ea = pc + offset;
					if (n_flag ^ v_flag)
					        jump(pc + offset);
					break;
				} case 0x2E: /* BGT */ {
					offset = fetch();
					t->ea = pc + offset;
					if (!(z_flag | (n_flag ^ v_flag)))
					        jump(pc + offset);
					break;
				} case 0x2F: /* BLE */ {
					offset = fetch();
					t->ea = pc + offset;
					if (z_flag | (n_flag ^ v_flag))
					        jump(pc + offset);
					break;
				} case 0x30: /* TSX */ {
					ix = sp + 1;
					break;
				} case 0x31: /* INS */ {
					sp = sp + 1;
					break;
				} case 0x32: /* PULA */ {
					acca = pull();
					break;
				} case 0x33: /* PULB */ {
					accb = pull();
					break;
				} case 0x34: /* DES */ {
					sp = sp - 1;
					break;
				} case 0x35: /* TXS */ {
					sp = ix - 1;
					break;
				} case 0x36: /* PSHA */ {
					push(acca);
					break;
				} case 0x37: /* PSHB */ {
					push(accb);
					break;
				} case 0x39: /* RTS */ {
				        if (sp == sp_stop) {
				                stop = 1;
				                sp_stop = -1;
				        } else
        					jump(pull2());
					break;
				} case 0x3B: /* RTI */ {
					write_flags(pull());
					accb = pull();
					acca = pull();
					ix = pull2();
					jump(pull2());
					break;
				} case 0x3E: /* WAI */ {
				        printf("WAI encountered...\n");
				        return;
					break;
				} case 0x3F: /* SWI */ {
					push2(pc);
					push2(ix);
					push(acca);
					push(accb);
					push(read_flags());
					jump(mread2(0xFFFA));
					break;
				} default: /* ??? */ {
				        goto invalid;
					break;
				}
                	}
                }
                goto normal;
                invalid:
                printf("\nInvalid opcode=$%2.2X at $%4.4X\n", opcode, pc - 1);
                stop = 1;
                normal:
                t->cc |= 0x80;
                if (trace) {
                        while (org_trace_idx != trace_idx) {
                                show_trace(org_trace_idx, trace_buf + (org_trace_idx & (TRACESIZE - 1)));
                                org_trace_idx++;
                        }
                }
	}
}
