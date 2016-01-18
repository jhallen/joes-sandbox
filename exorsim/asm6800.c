/*	6800 Assembler
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
#include "asm6800.h"

/* Fixup types */

enum {
        FIXUP_EXT,	/* An extended address */
        FIXUP_DIR,	/* An 8-bit address */
        FIXUP_REL,	/* A branch offset */
};

/* A pending fixup */

struct fixup {
        struct fixup *next;
        unsigned short fixup;	/* Address of data which needs to be fixed up */
        int type;		/* Type of fixup */
        int ofst;
};

/* Symbol table */

struct symbol {
        struct symbol *next;
        char *name;		/* Name of symbol */
        int valid;		/* Set if symbol's value is known */
        unsigned short val;	/* Value of symbol */
        struct fixup *fixups;	/* Fixups waiting for this symbol */
} *symbols;

struct symbol *find_symbol(char *name)
{
        struct symbol *sy;
        for (sy = symbols; sy; sy = sy->next)
                if (!strcmp(sy->name, name))
                        return sy;
        sy = (struct symbol *)malloc(sizeof(struct symbol));
        sy->next = symbols;
        symbols = sy;
        sy->name = strdup(name);
        sy->valid = 0;
        sy->val = 0;
        sy->fixups = 0;
        return sy;
}

/* Get symbol name by address */

char *find_label(unsigned short val)
{
        struct symbol *sy;
        for (sy = symbols; sy; sy = sy->next)
                if (sy->val == val)
                        return sy->name;
        return 0;
}

/* Add a fixup */

void add_fixup(struct symbol *sy, unsigned short addr, int type, int ofst)
{
        struct fixup *fix = (struct fixup *)malloc(sizeof(struct fixup));
        fix->type = type;
        fix->fixup = addr;
        fix->next = sy->fixups;
        fix->ofst = ofst;
        sy->fixups = fix;
}

/* Show symbol table */

void show_syms(FILE *f)
{
        struct symbol *sy;
        for (sy = symbols; sy; sy = sy->next) {
                struct fixup *fix;
                if (sy->valid) {
                        fprintf(f,"%4.4X %s\n", sy->val, sy->name);
                } else {
                        fprintf(f,"???? %s\n", sy->name);
                }
                for (fix = sy->fixups; fix; fix = fix->next) {
                        if (fix->type == FIXUP_EXT)
                                fprintf(f,"    16-bit fixup at %4.4X\n", fix->fixup);
                        else if (fix->type == FIXUP_DIR)
                                fprintf(f,"    8-bit fixup at  %4.4X\n", fix->fixup);
                        else if (fix->type == FIXUP_REL)
                                fprintf(f,"    8-bit rel fixup %4.4X\n", fix->fixup);
                }
        }
}

/* Clear symbol table */

void clr_syms(void)
{
        struct symbol *sy;
        while (sy = symbols) {
                struct fixup *fix;
                symbols = sy->next;
                while (fix = sy->fixups) {
                        sy->fixups = fix->next;
                        free(fix);
                }
                free(sy->name);
                free(sy);
        }
}

/* Set symbol's value, process pending fixups */

void set_symbol(unsigned char *mem, struct symbol *sy, unsigned short val)
{
        struct fixup *fix;
        if (!sy)
                return;
        if (sy->valid) {
                printf("Symbol '%s' already defined to %4.4x\n", sy->name, sy->val);
                return;
        }
        sy->valid = 1;
        sy->val = val;
        while (fix = sy->fixups) {
                sy->fixups = fix->next;
                if (fix->type == FIXUP_EXT) {
                        mem[fix->fixup] = ((val + fix->ofst) >> 8);
                        mem[fix->fixup + 1]  = (val + fix->ofst);
                        printf("Address at %4.4X set to %4.4X\n", fix->fixup, val + fix->ofst);
                } else if (fix->type == FIXUP_DIR) {
                        mem[fix->fixup]  = (val + fix->ofst);
                        printf("Byte at %4.4X set to %2.2X\n", fix->fixup, ((val + fix->ofst) & 255));
                } else if (fix->type == FIXUP_REL) {
                        mem[fix->fixup] = val + fix->ofst - (fix->fixup + 1);
                        printf("Offset at %4.4X set to %2.2X\n", fix->fixup, val + fix->ofst - (fix->fixup + 1));
                }
                free(fix);
        }
}

/* Instruction type codes
 *
 *   RM   #imm8  -> 0x00
 *        dir8 -> 0x10
 *        nn,x -> 0x20
 *        ext16 -> 0x30
 *
 *   IDX  #imm16 -> 0x00
 *        dir8 -> 0x10
 *        nn,x -> 0x20
 *        ext16 -> 0x30
 *
 *   REL  rel8
 *
 *   RMW  nn,x  -> 0x20
 *        ext16 -> 0x30
 */

enum {
        RM,	/* Register-Memory */
        RMW,	/* Read-modify-write */
        REL,	/* Relative branch */
        IDX,	/* Index register-Memory */
        FCB,	/* FCB pseudo-op */
        FDB,	/* FDB pseudo-op */
        EQU,	/* EQU pseudo-op */
        RMB,	/* RMB pseudo-op */
        ORG,	/* ORG pseudo-op */
        IGN,	/* Ignore these pseudo-ops */
        ACC,	/* Accumulator needed */
        ACC1,	/* Accumulator optional */
        ACCB,	/* we need an A or a B: add 0x01 for B */
        NONE	/* No operand */
};

struct { char *insn; int opcode; int type; } table[] =
{
        { "lda", 0x86, ACC1 },
        { "sta", 0x87, ACC1 },
        { "ora", 0x8a, ACC1 },

        { "sub", 0x80, ACC },
        { "cmp", 0x81, ACC },
        { "sbc", 0x82, ACC },
        { "and", 0x84, ACC },
        { "bit", 0x85, ACC },
        { "eor", 0x88, ACC },
        { "adc", 0x89, ACC },
        { "add", 0x8b, ACC },

        { "suba", 0x80, RM },
        { "cmpa", 0x81, RM },
        { "sbca", 0x82, RM },
        { "anda", 0x84, RM },
        { "bita", 0x85, RM },
        { "ldaa", 0x86, RM },
        { "staa", 0x87, RM },
        { "eora", 0x88, RM },
        { "adca", 0x89, RM },
        { "oraa", 0x8a, RM },
        { "adda", 0x8b, RM },

        { "subb", 0xc0, RM },
        { "cmpb", 0xc1, RM },
        { "sbcb", 0xc2, RM },
        { "andb", 0xc4, RM },
        { "bitb", 0xc5, RM },
        { "ldb", 0xc6, RM },
        { "stb", 0xc7, RM },
        { "ldab", 0xc6, RM },
        { "stab", 0xc7, RM },
        { "eorb", 0xc8, RM },
        { "adcb", 0xc9, RM },
        { "orb", 0xca, RM },
        { "orab", 0xca, RM },
        { "addb", 0xcb, RM },

        { "cpx", 0x8c, IDX },
        { "bsr", 0x8d, REL },
        { "lds", 0x8e, IDX },
        { "sts", 0x8f, IDX },
        { "jsr", 0x8d, RMW },
        { "ldx", 0xce, IDX },
        { "stx", 0xcf, IDX },

        { "neg", 0x40, RMW },
        { "com", 0x43, RMW },
        { "lsr", 0x44, RMW },
        { "ror", 0x46, RMW },
        { "asr", 0x47, RMW },
        { "asl", 0x48, RMW },
        { "rol", 0x49, RMW },
        { "dec", 0x4a, RMW },
        { "inc", 0x4c, RMW },
        { "tst", 0x4d, RMW },
        { "jmp", 0x4e, RMW },
        { "clr", 0x4f, RMW },

        { "nega", 0x40, NONE },
        { "coma", 0x43, NONE },
        { "lsra", 0x44, NONE },
        { "rora", 0x46, NONE },
        { "asra", 0x47, NONE },
        { "asla", 0x48, NONE },
        { "rola", 0x49, NONE },
        { "deca", 0x4a, NONE },
        { "inca", 0x4c, NONE },
        { "tsta", 0x4d, NONE },
        { "clra", 0x4f, NONE },

        { "negb", 0x50, NONE },
        { "comb", 0x53, NONE },
        { "lsrb", 0x54, NONE },
        { "rorb", 0x56, NONE },
        { "asrb", 0x57, NONE },
        { "aslb", 0x58, NONE },
        { "rolb", 0x59, NONE },
        { "decb", 0x5a, NONE },
        { "incb", 0x5c, NONE },
        { "tstb", 0x5d, NONE },
        { "clrb", 0x5f, NONE },

        { "nop", 0x01, NONE },
        { "tap", 0x06, NONE },
        { "tpa", 0x07, NONE },
        { "inx", 0x08, NONE },
        { "dex", 0x09, NONE },
        { "clv", 0x0a, NONE },
        { "sev", 0x0b, NONE },
        { "clc", 0x0c, NONE },
        { "sec", 0x0d, NONE },
        { "cli", 0x0e, NONE },
        { "sei", 0x0f, NONE },
        { "sba", 0x10, NONE },
        { "cba", 0x11, NONE },
        { "tab", 0x16, NONE },
        { "tba", 0x17, NONE },
        { "daa", 0x19, NONE },
        { "aba", 0x1b, NONE },

        { "bra", 0x20, REL },
        { "bhi", 0x22, REL },
        { "bls", 0x23, REL },
        { "bcc", 0x24, REL },
        { "bcs", 0x25, REL },
        { "bne", 0x26, REL },
        { "beq", 0x27, REL },
        { "bvc", 0x28, REL },
        { "bvs", 0x29, REL },
        { "bpl", 0x2a, REL },
        { "bmi", 0x2b, REL },
        { "bge", 0x2c, REL },
        { "blt", 0x2d, REL },
        { "bgt", 0x2e, REL },
        { "ble", 0x2f, REL },

        { "tsx", 0x30, NONE },
        { "ins", 0x31, NONE },
        { "pul", 0x32, ACCB },
        { "pula", 0x32, NONE },
        { "pulb", 0x33, NONE },
        { "des", 0x34, NONE },
        { "txs", 0x35, NONE },
        { "psh", 0x36, ACCB },
        { "psha", 0x36, NONE },
        { "pshb", 0x37, NONE },
        { "rts", 0x39, NONE },
        { "rti", 0x3b, NONE },
        { "wai", 0x3e, NONE },
        { "swi", 0x3f, NONE },

        { "fcb", 0, FCB },
        { "fcc", 0, FCB },
        { "fdb", 0, FDB },
        { "equ", 0, EQU },
        { "rmb", 0, RMB },
        { "org", 0, ORG },

        { "end", 0, IGN },
        { "mon", 0, IGN },
        { "opt", 0, IGN },
        { "nam", 0, IGN },
        { "ttl", 0, IGN },
        { "spc", 0, IGN },
        { "page", 0, IGN },

        { 0, 0, 0 }
};

/* Parse a value (this should really be an expression parser) */

int parse_val(char **buf, int *operand, struct symbol **sy, unsigned short addr)
{
        char str[80];
        *sy = 0;
        if (!parse_dec(buf, operand)) {
                if (parse_word(buf, str)) {
                        char *p;
                        if (!strcmp(str, "*")) {
                                *operand = addr;
                        } else {
                                *sy = find_symbol(str);
                                *operand = (*sy)->val;
                                if ((*sy)->valid)
                                        *sy = 0;
                        }
                        p = *buf;
                        if (*p == '+' || *p == '-') {
                                char c = *p++;
                                int ofst;
                                *buf = p;
                                if (parse_dec(buf, &ofst)) {
                                        if (c == '+')
                                                *operand += ofst;
                                        else
                                                *operand -= ofst;
                                } else {
                                        printf("Missing value after + or -\n");
                                }
                        }
                } else {
                        return 0;
                }
        }
        return 1;
}

unsigned assemble(unsigned char *mem, unsigned addr, char *buf)
{
        char str[80];
        unsigned short label_addr = addr;
        struct symbol *label_sy;
        struct symbol *sy;
        int opcode = -1;
        int operand;
        int type;
        char *p;
        int c;
        int x;

        label_sy = 0;
        sy = 0;

        if (buf[0] == '*' || !buf[0]) {
                /* Comment line, ignore */
                return addr;
        }

        if (!(buf[0] == ' ' || buf[0] == '\t') && parse_word(&buf, str)) {
                /* A label */
                label_sy = find_symbol(str);

                skipws(&buf);

                if (!parse_word(&buf, str)) {
                        goto done;
                }
        } else {
                skipws(&buf);
                if (!parse_word(&buf, str)) {
                        printf("Huh?\n");
                        return addr;
                }
        }

        /* Lookup instruction */
        for (x = 0; table[x].insn; ++x)
                if (!jstricmp(table[x].insn, str)) {
                        opcode = table[x].opcode;
                        type = table[x].type;
                        break;
                }

        if (!table[x].insn) {
                printf("Huh?\n");
                return addr;
        }

        skipws(&buf);

        if (type == IGN) {
                goto done;
        } else if (type == NONE) {
                mem[addr++] = opcode;
                goto done;
        } else if (type == ACC) {
                type = RM;
                if ((buf[0] == 'a' || buf[0] == 'A') && (!buf[1] || buf[1] == ' ' || buf[1] == '\t')) {
                        buf++;
                } else if ((buf[0] == 'b' || buf[0] == 'B') && (!buf[1] || buf[1] == ' ' || buf[1] == '\t')) {
                        opcode += 0x40;
                        buf++;
                } else {
                        printf("Accu missing after intrustion\n");
                        return addr;
                }
                skipws(&buf);
                goto norm;
        } else if (type == ACC1) {
                type = RM;
                if ((buf[0] == 'a' || buf[0] == 'A') && (!buf[1] || buf[1] == ' ' || buf[1] == '\t')) {
                        buf++;
                } else if ((buf[0] == 'b' || buf[0] == 'B') && (!buf[1] || buf[1] == ' ' || buf[1] == '\t')) {
                        opcode += 0x40;
                        buf++;
                }
                skipws(&buf);
                goto norm;
        } else if (type == RMW) {
                /* More syntax of old asm: psh a, inc a, etc. */
                if ((buf[0] == 'a' || buf[0] == 'A') && (!buf[1] || buf[1] == ' ' || buf[1] == '\t')) {
                        mem[addr++] = opcode;
                        goto done;
                } else if ((buf[0] == 'b' || buf[0] == 'B') && (!buf[1] || buf[1] == ' ' || buf[1] == '\t')) {
                        mem[addr++] = opcode + 0x10;
                        goto done;
                } else {
                        goto normal;
                }
        } else if (type == ACCB) {
                /* More syntax of old asm: psh a, inc a, etc. */
                if ((buf[0] == 'a' || buf[0] == 'A') && (!buf[1] || buf[1] == ' ' || buf[1] == '\t')) {
                        mem[addr++] = opcode;
                        goto done;
                } else if ((buf[0] == 'b' || buf[0] == 'B') && (!buf[1] || buf[1] == ' ' || buf[1] == '\t')) {
                        mem[addr++] = opcode + 0x01;
                        goto done;
                } else {
                        printf("Missing accumulator\n");
                        return addr;
                }
        } else if (type == FCB) {
                char c;
                if (!*buf) {
                        printf("Missing value\n");
                        return addr;
                }
                fcb_loop:
                /* Special case for FCB/FCC */
                if (*buf == '"' || *buf == '/') {
                        c = *buf++;
                        while (*buf && *buf != c) {
                                mem[addr++] = *buf++;
                        }
                        if (*buf)
                                ++buf;
                } else if (parse_val(&buf, &operand, &sy, addr)) {
                        if (sy) {
                                add_fixup(sy, addr, FIXUP_DIR, operand);
                                mem[addr++] = 0;
                        } else {
                                mem[addr++] = operand;
                        }
                } else {
                        mem[addr++] = 0;
                }
                skipws(&buf);
                if (buf[0] == ',') {
                        ++buf;
                        skipws(&buf);
                        goto fcb_loop;
                }
                goto done;
        } else {
                norm:
                if (*buf == '#') {
                        ++buf;
                        if (!parse_val(&buf, &operand, &sy, addr)) {
                                printf("Missing number or label after #\n");
                                return addr;
                        }
                        if (type == RM) {
                                /* 8-bit immediate */
                                mem[addr++] = opcode;
                                if (sy)
                                        add_fixup(sy, addr, FIXUP_DIR, operand);
                                mem[addr++] = operand;
                                goto done;
                        } else if (type == IDX) {
                                /* 16-bit immediate */
                                mem[addr++] = opcode;
                                if (sy)
                                        add_fixup(sy, addr, FIXUP_EXT, operand);
                                mem[addr++] = (operand >> 8);
                                mem[addr++] = operand;
                                goto done;
                        } else {
                                printf("Invalid operand\n");
                                return addr;
                        }
                } else {
                        normal:
                        /* Check for stupid syntax of old asm: lda x is same as lda 0,x */
                        if ((buf[0] == 'x' || buf[0] == 'X') && (!buf[1] || buf[1] == ' ' || buf[1] == '\t')) {
                                operand = 0;
                                goto ndx;
                        }
                        if (parse_val(&buf, &operand, &sy, addr)) {
                                if (buf[0] == ',' && (buf[1] == 'x' || buf[1] == 'X')) {
                                        ndx:
                                        /* We have an indexed operand */
                                        if (type == RM || type == IDX || type == RMW) {
                                                mem[addr++] = opcode + 0x20;;
                                                if (sy)
                                                        add_fixup(sy, addr, FIXUP_DIR, operand);
                                                mem[addr++] = operand;
                                                goto done;
                                        } else {
                                                printf("Invalid operand for this instruction\n");
                                                return addr;
                                        }
                                } else {
                                        /* We have a direct address operand */
                                        if ((type == RM || type == IDX) && !sy && operand < 256) {
                                                mem[addr++] = opcode + 0x10;
                                                mem[addr++] = operand;
                                                goto done;
                                        } else if (type == RM || type == IDX || type == RMW) {
                                                mem[addr++] = opcode + 0x30;
                                                if (sy)
                                                        add_fixup(sy, addr, FIXUP_EXT, operand);
                                                mem[addr++] = (operand >> 8);
                                                mem[addr++] = operand;
                                                goto done;
                                        } else if (type == REL) {
                                                mem[addr++] = opcode;
                                                if (sy) {
                                                        mem[addr] = 0;
                                                        add_fixup(sy, addr, FIXUP_REL, operand);
                                                } else {
                                                        mem[addr] = operand - (addr + 1);
                                                }
                                                addr++;
                                                goto done;
                                        } else if (type == FDB) {
                                                if (!*buf) {
                                                        printf("Value missing\n");
                                                        return addr;
                                                }
                                                more_fdb:
                                                if (sy) {
                                                        add_fixup(sy, addr, FIXUP_EXT, operand);
                                                        mem[addr++] = 0;
                                                        mem[addr++] = 0;
                                                } else {
                                                        mem[addr++] = (operand >> 8);
                                                        mem[addr++] = operand;
                                                }
                                                skipws(&buf);
                                                if (buf[0] == ',') {
                                                        ++buf;
                                                        skipws(&buf);
                                                        if (parse_val(&buf, &operand, &sy, addr)) {
                                                                goto more_fdb;
                                                        } else {
                                                                operand = 0;
                                                                sy = 0;
                                                                goto more_fdb;
                                                        }
                                                }
                                                goto done;
                                        } else if (type == RMB) {
                                                if (sy) {
                                                        printf("Resolved symbol required for rmb\n");
                                                } else {
                                                        addr += operand;
                                                }
                                                goto done;
                                        } else if (type == EQU) {
                                                if (sy) {
                                                        printf("Resolved symbol required for equ\n");
                                                } else if (!label_sy) {
                                                        printf("Label required for equ\n");
                                                } else {
                                                        label_addr = operand;
                                                }
                                                goto done;
                                        } else if (type == ORG) {
                                                if (sy) {
                                                        printf("Resolved symbol required for org\n");
                                                }
                                                label_addr = addr = operand;
                                                goto done;
                                        } else {
                                                printf("Invalid operand for this instruction\n");
                                                return addr;
                                        }
                                }
                        }
                        /* We have no operand */
                        if (type == NONE) {
                                mem[addr++] = opcode;
                                goto done;
                        } else {
                                printf("Operand required\n");
                                return addr;
                        }
                }
        }
        done:
        set_symbol(mem, label_sy, label_addr);
        return addr;
}
