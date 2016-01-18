/*	EXORcister simulator
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
#include <fcntl.h>
#include <sys/poll.h>
#include <signal.h>

#include "utils.h"
#include "asm6800.h"
#include "sim6800.h"
#include "exor.h"

FILE *mon_out;
FILE *mon_in;

int last;
unsigned short last_u;
int step;

struct cmd { char *name; int (*func)(char *); char *help; } cmds[];

void huh()
{
        printf("Huh?\n");
}

int quit_cmd(char *p)
{
        printf("\nBye\n");
        restore_termios();
        exit(-1);
        return 0;
}

int help_cmd(char *p)
{
        int x;
        for (x = 0; cmds[x].name; ++x)
                fprintf(mon_out, "%s%s\n", cmds[x].name, cmds[x].help);
        printf("Append >file to redirect output to file\n");
        printf("Append >>file to append output to file\n");
        printf("Append <file to redirect input from file\n");
        return 0;
}

int caps_cmd(char *p)
{
        if (match_word(&p, "on"))
                lower = 0;
        else if (match_word(&p, "off"))
                lower = 1;
        else
                huh();
        return 0;
}

int m_cmd(char *p)
{
        char buf[180];
        int addr;
        if (parse_hex(&p, &addr)) {
                for (;;) {
                        printf("%4.4x %2.2x ", addr, mem[addr]);
                        if (!jgetline(stdin, buf)) {
                               if (buf[0]) {
                                       mem[addr] = hatoi(buf);
                               }
                               ++addr;
                        } else {
                                printf("\n");
                                break;
                        }
                }
        } else
                huh();
        return 0;
}

int regs_cmd(char *p)
{
        int val;
        if (!*p) {
                fprintf(mon_out, "PC=%4.4X A=%2.2X B=%2.2X X=%4.4X SP=%2.2X CC=%2.2X\n", pc, acca, accb, ix, sp, read_flags());
        } else if (match_word(&p, "pc") && parse_hex(&p, &val)) {
                pc = val;
        } else if (match_word(&p, "sp") && parse_hex(&p, &val)) {
                sp = val;
        } else if (match_word(&p, "x") && parse_hex(&p, &val)) {
                ix = val;
        } else if (match_word(&p, "a") && parse_hex(&p, &val)) {
                acca = val;
        } else if (match_word(&p, "b") && parse_hex(&p, &val)) {
                accb = val;
        } else if (match_word(&p, "cc") && parse_hex(&p, &val)) {
                write_flags(val);
        } else
                huh();
        return 0;
}

int call_cmd(char *p)
{
        int val;
        if (parse_hex(&p, &val)) {
                pc = val;
                sp_stop = sp;
                stop = 0;
        } else
                huh();
        return 1;
}

int c_cmd(char *p)
{
        int val;
        if (!*p) {
                stop = 0;
                return 1;
        } else if (parse_hex(&p, &val)) {
                stop = 0;
                pc = val;
                return 1;
        } else
                huh();
        return 0;
}

int s_cmd(char *p)
{
        int val;
        if (!*p) {
                stop = 1;
                step = 1;
                return 1;
        } else if (parse_hex(&p, &val)) {
                stop = 1;
                step = 1;
                pc = val;
                return 1;
        } else
                huh();
        return 0;
}

int clr_cmd(char *p)
{
        clr_syms();
        return 0;
}

int sy_cmd(char *p)
{
        show_syms(mon_out);
        return 0;
}

int b_cmd(char *p)
{
        int val;
        if (!*p) {
                brk = 0;
                printf("Breakpoint cleared\n");
        } else if (parse_hex(&p, &val)) {
                brk_addr = val;
                brk = 1;
                printf("Breakpoint set at %4.4X\n", brk_addr);
        } else
                huh();
        return 0;
}

int d_cmd(char *p)
{
        int val;
        int len;
        if (!*p) {
                hd(mon_out, mem, last, 0x80);
                last += 0x80;
        } else if (parse_hex(&p, &val) && skipws(&p)) {
                len = 0x80;
                parse_hex(&p, &len);
                hd(mon_out, mem, last = val, len);
                last += len;
        } else
                huh();
        return 0;
}

int p_cmd(char *p)
{
        int start;
        int len;
        int addr = 0;
        if (parse_hex(&p, &start) && skipws(&p) && parse_hex(&p, &len)) {
                int l;
                int cksum;
                while (len) {
                        int x;
                        cksum = 0;
                        if (len >= 16)
                                l = 16;
                        else
                                l = len;
                        cksum = l + 3;
                        cksum += (start >> 8);
                        cksum += (start & 0xFF);
                        fprintf(mon_out, "S1%2.2X%4.4X", l + 3, start);
                        for (x = 0; x != l; ++x) {
                                fprintf(mon_out,"%2.2X", mem[start + x]);
                                cksum += mem[start + x];
                        }
                        fprintf(mon_out, "%2.2X\n", (~cksum & 0xFF));
                        len -= l;
                        start += l;
                }
                /* End mark, starting address */
                skipws(&p);
                parse_hex(&p, &addr);
                l = 0;
                cksum = l + 3;
                cksum += (addr >> 8);
                cksum += (addr & 0xFF);
                fprintf(mon_out, "S9%2.2X%4.4X%2.2X\n", l + 3, addr, (~cksum & 0xFF));
        }
        return 0;
}

int l_cmd(char *p)
{
        char buf[180];
        int line = 0;
        unsigned short addr;
        unsigned short start = 0;
        int cksum;
        int l;
        while (!jgetline(mon_in, buf)) {
                ++line;
                if (buf[0] == 'S' && buf[1] == '1') {
                        int l;
                        int addr;
                        int x;
                        int data;
                        int cksum;
                        int chk;
                        p = buf + 2;
                        parse_hex2(&p, &l);
                        chk = l;
                        parse_hex4(&p, &addr);
                        chk += (addr >> 8);
                        chk += (addr & 0xFF);
                        for (x = 0; x < l - 3; ++x) {
                                parse_hex2(&p, &data);
                                chk += data;
                                mem[addr + x] = data;
                        }
                        parse_hex2(&p, &cksum);
                        if (cksum != (~chk & 0xFF)) {
                                printf("Checksum mismatch on line %d\n", line);
                        }
                } else if (buf[0] == 'S' && buf[1] == '9') {
                        int l;
                        int addr;
                        int cksum;
                        int chk;
                        p = buf + 2;
                        parse_hex2(&p, &l);
                        chk = l;
                        parse_hex4(&p, &addr);
                        chk += (addr >> 8);
                        chk += (addr & 0xFF);
                        parse_hex2(&p, &cksum);
                        if (cksum != (~chk & 0xFF)) {
                                printf("Checksum mismatch on line %d\n", line);
                        }
                        pc = addr;
                        printf("PC set to %4.4x\n", addr);
                } else {
                        printf("Unknown record on line %d\n", line);
                }
        }
        return 0;
}

int a_cmd(char *p)
{
        int addr;
        char buf[180];
        if (parse_hex(&p, &addr)) {
                for (;;) {
                        fprintf(mon_out, "%4.4x: ", addr);
                        if (!jgetline(mon_in, buf)) {
                                if (mon_in != stdin) {
                                        fprintf(mon_out, "%s\n", buf);
                                }
                                if (buf[0]) {
                                        addr = assemble(mem, addr, buf);
                                } else {
                                        break;
                                }
                        } else {
                                fprintf(mon_out, "\n");
                                break;
                        }
                }
        } else
                huh();
        return 0;
}

int u_cmd(char *p)
{
        char buf[180];
        int addr = last_u;
        if (parse_hex(&p, &addr) || !*p) {
                int target;
                int x;
                for (x = 0; x != 22; ++x) {
                        unasm_line(mem, &addr, buf, &target, 1);
                        fprintf(mon_out, "%s\n", buf);
                }
                last_u = addr;
        } else
                huh();
        return 0;
}

int abort_cmd(char *p)
{
        abrt = 1;
        stop = 0;
        return 1;
}

int reset_cmd(char *p)
{
        reset = 1;
        stop = 0;
        return 1;
}

int dump_cmd(char *p)
{
        char name[180];
        int start = 0;
        int size = 65536;
        int fd;
        strcpy(name, "dump");

        parse_word(&p, name) && skipws(&p) && parse_hex(&p, &start) && skipws(&p) && parse_hex(&p, &size);

        if (!size || start + size > 65536) {
                printf("start + size > 64K or size == 0\n");
                return 0;
        }

        fd = creat(name, 0666);
        if (fd != -1) {
                write(fd, mem + start, size);
                close(fd);
                printf("Wrote %d bytes starting at %d to file '%s'\n", size, start, name);
        } else {
                printf("Couldn't open file '%s'\n", name);
        }
        return 0;
}

int read_cmd(char *p)
{
        char name[180];
        int start = 0;
        int size;
        FILE *f;
        strcpy(name, "dump");

        parse_word(&p, name) && skipws(&p) && parse_hex(&p, &start);
        f = fopen(name, "r");
        if (!f) {
                printf("Couldn't open '%s'\n", name);
                return 0;
        }
        fseek(f, 0, SEEK_END);
        size = ftell(f);
        if (!size || start + size > 65536) {
                printf("start + size > 64K or size == 0\n");
                fclose(f);
                return 0;
        }
        rewind(f);
        fread(mem + start, size, 1, f);
        fclose(f);
        printf("Loaded %d bytes starting at %d\n", size, start);
        return 0;
}

int t_cmd(char *p)
{
        if (match_word(&p, "on"))
                trace = 1;
        else if (match_word(&p, "off"))
                trace = 0;
        else
                huh();
        return 0;
}

int poll_cmd(char *p)
{
        if (match_word(&p, "on"))
                polling = 1;
        else if (match_word(&p, "off"))
                polling = 0;
        else
                huh();
}

/* Command table */

struct cmd cmds[]=
{
        { "help", help_cmd,	"			Show this help text" },
        { "h", help_cmd,	"			Show this help text" },
        { "q", quit_cmd,	"			Exit simulator" },
        { "quit", quit_cmd,	"			Exit simulator" },
        { "t", t_cmd,		" [on|off]		Turn tracing on / off" },
        { "poll", poll_cmd,	" [on|off]		Turn ACIA polling on / off" },
        { "c", c_cmd,		" [hhhh]		Continue simulating [jump to address]" },
        { "s", s_cmd,		" [hhhh]		Step one instruction [jump to address]" },
        { "b", b_cmd,		" [hhhh]		Set/Clear breakpoint" },
        { "r", regs_cmd,	" [reg hhhh]		Show regs, set reg" },
        { "x", call_cmd,	" hhhh			Call subroutine, return to monitor when done" },
        { "reset", reset_cmd,	"			Hit reset button" },
        { "abort", abort_cmd,	"			Hit abort button" },
        { "caps", caps_cmd,	" [on|off]		Force uppercase on / off" },
        { "d", d_cmd,		" hhhh [nnnn]		Hex dump" },
        { "m", m_cmd,		" hhhh			Modify" },
        { "a", a_cmd,		" hhhh			Assemble" },
        { "clr", clr_cmd,	"			Clear symbol table" },
        { "sy", sy_cmd,		"			Show symbol table" },
        { "u", u_cmd,		" hhhh			Unassemble" },
        { "p", p_cmd,		" hhhh nnnn [ssss]	Punch S19" },
        { "l", l_cmd,		"			Load S19" },
        { "save", dump_cmd,	" [file [hhhh [nnnn]]]	Save memory to file in binary\n" },
        { "read", read_cmd,	" [file [hhhh]]		Read binary file into memory\n" },
        { 0, 0, 0 }
};

void monitor()
{
        if (mon_out != stdout) {
                fclose(mon_out);
                mon_out = stdout;
        }
        if (mon_in != stdin) {
                fclose(mon_in);
                mon_in = stdin;
        }

        /* system("stty -isig"); */
        nosig_termios();

        if (step) {
                if (trace)
                        show_traces(1);
                else
                        show_traces(2);
                step = 0;
        } else {
                if (trace)
                        show_traces(1);
                else
                        show_traces(128);
        }

        printf("\nType 'help'\n");

        for (;;) {
                char buf[180];
                char word[180];
                char *p = buf;
                char *q;
                fputs("% ", stdout);
                if (jgetline(stdin, buf)) {
                        quit_cmd(p);
                }
                if ((p = strstr(buf, " >>")) && parse_word(((q = p + 3), &q), word)) {
                        /* Append output */
                        jstrcpy(p, q);
                        mon_out = fopen(word, "a");
                        if (!mon_out) {
                                printf("Couldn't open '%s' for append\n", word);
                                continue;
                        }
                } else if ((p = strstr(buf, " >")) && parse_word(((q = p + 2), &q), word)) {
                        /* Redirect output */
                        jstrcpy(p, q);
                        mon_out = fopen(word, "w");
                        if (!mon_out) {
                                printf("Couldn't open '%s' for write\n", word);
                                continue;
                        }
                }
                if ((p = strstr(buf, " <")) && parse_word(((q = p + 2), &q), word)) {
                        /* Redirect input */
                        jstrcpy(p, q);
                        mon_in = fopen(word, "r");
                        if (!mon_in) {
                                printf("Couldn't open '%s' for reading\n", word);
                                if (mon_out != stdout) {
                                        fclose(mon_out);
                                        mon_out = stdout;
                                }
                                continue;
                        }
                }
                p = buf;
                if (skipws(&p) && *p) {
                        int x;
                        for (x = 0; cmds[x].name; ++x)
                                if (match_word(&p, cmds[x].name))
                                        break;
                        if (cmds[x].name) {
                                skipws(&p);
                                if (cmds[x].func(p))
                                        break;
                        } else
                                printf("Huh?\n");
                }
                if (mon_out != stdout) {
                        fclose(mon_out);
                        mon_out = stdout;
                }
                if (mon_in != stdin) {
                        fclose(mon_in);
                        mon_in = stdin;
                }
        }
        /* system("stty isig"); */
        sig_termios();
}
