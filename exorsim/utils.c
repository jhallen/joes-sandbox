/*	Utilities
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
#include <unistd.h>
#include <termios.h>
#include "utils.h"

/* Skip over whitespace */

int skipws(char **at_p)
{
	char *p = *at_p;
	while (*p == ' ' || *p == '\t')
		++p;
	*at_p = p;
	return 1;
}

/* Skip to whitespace */

int skiptows(char **at_p)
{
	char *p = *at_p;
	while (*p && *p != ' ' && *p != '\t')
		++p;
	*at_p = p;
	if (*p == ' ' || *p == '\t')
        	return 1;
        else
                return 0;
}

/* Skip over matching word */

int match_word(char **at_p, char *word)
{
        char *p = *at_p;
        int len = strlen(word);
        if (!strncmp(p, word, len) && (!p[len] || p[len] == ' ' || p[len] == '\t')) {
                p += len;
                skipws(&p);
                *at_p = p;
                return 1;
        } else
                return 0;
}

/* Extract word */

int parse_word(char **at_p, char *buf)
{
	char *p = *at_p;
	if (*p && *p != ' ' && *p != '\t' && *p != ',' && *p != '+' && *p != '-') {
		int x = 0;
		while (*p && *p != ' ' && *p != '\t' && *p != ',' && *p != '+' && *p != '-') {
			buf[x++] = *p++;
		}
		buf[x] = 0;
		*at_p = p;
		return 1;
	} else {
		return 0;
	}
}

/* Parse one hex digit */

int parse_hex1(char **at_p, int *hex)
{
	char *p = *at_p;
	if (*p >= '0' && *p <= '9' || *p >= 'a' && *p <= 'f' || *p >= 'A' && *p <= 'F') {
		unsigned short val = 0;
                if (*p >= '0' && *p <= '9')
                        val = val * 16 + *p++ - '0';
                else if (*p >= 'A' && *p <= 'F')
                        val = val * 16 + *p++ - 'A' + 10;
                else if (*p >= 'a' && *p <= 'f')
                        val = val * 16 + *p++ - 'a' + 10;
		*hex = val;
		*at_p = p;
		return 1;
	} else {
		return 0;
	}
}

/* Parse hex digits */

int parse_hex(char **at_p, int *hex)
{
	char *p = *at_p;
	if (*p >= '0' && *p <= '9' || *p >= 'a' && *p <= 'f' || *p >= 'A' && *p <= 'F') {
		unsigned short val = 0;
		int dig;
		while (parse_hex1(&p, &dig))
		        val = val * 16 + dig;
		*hex = val;
		*at_p = p;
		return 1;
	} else {
		return 0;
	}
}

/* Parse two hex digits */

int parse_hex2(char **at_p, int *hex)
{
	char *p = *at_p;
	int a = 0;
	int b = 0;
	if (parse_hex1(&p, &a) && parse_hex1(&p, &b)) {
		*hex = (a << 4) + b;
		*at_p = p;
		return 1;
	} else {
		return 0;
	}
}

/* Parse four hex digits */

int parse_hex4(char **at_p, int *hex)
{
	char *p = *at_p;
	int a = 0;
	int b = 0;
	if (parse_hex2(&p, &a) && parse_hex2(&p, &b)) {
		*hex = (a << 8) + b;
		*at_p = p;
		return 1;
	} else {
		return 0;
	}
}

/* Parse binary digits */

int parse_bin(char **at_p, unsigned short *hex)
{
	char *p = *at_p;
	if (*p >= '0' && *p <= '1') {
		unsigned short val = 0;
		while (*p >= '0' && *p <= '1')
			val = val * 2 + *p++ - '0';
		*hex = val;
		*at_p = p;
		return 1;
	} else {
		return 0;
	}
}

/* Parse ddd, $hhh, %bbbb */

int parse_dec(char **at_p, int *dec)
{
	char *p = *at_p;
	if (*p == '$') {
	        int rtn;
	        int hex;
	        ++p;
	        rtn = parse_hex(&p, &hex);
	        if (rtn) {
	                *at_p = p;
	                *dec = hex;
	                return 1;
	        } else
	                return 0;
	} else if (*p == '%') {
	        int rtn;
	        unsigned short bin;
	        ++p;
	        rtn = parse_bin(&p, &bin);
	        if (rtn) {
	                *at_p = p;
	                *dec = bin;
	                return 1;
	        } else
	                return 0;
        } else if (*p == '\'' && p[1]) {
                ++p;
                unsigned char c = *p++;
                *dec = c;
                if (*p == '\'')
                        ++p;
                *at_p = p;
                return 1;
	} else if (*p >= '0' && *p <= '9') {
		int val = 0;
		while (*p >= '0' && *p <= '9')
			val = val * 10 + *p++ - '0';
		*dec = val;
		*at_p = p;
		return 1;
	} else {
		return 0;
	}
}

/* Get input line with editing (for when cooked is off).
 * Returns -1 if Ctrl-C hit
 */

int jgetline(FILE *f, char *buf)
{
        int x = 0;

        if (f != stdin) {
                if (fgets(buf, 179, f)) {
                        if (strlen(buf))
                                buf[strlen(buf) - 1] = 0;
                        return 0;
                } else
                        return -1;
        }

        fflush(stdout);
        for (;;) {
                char c;
                if (read(fileno(stdin), &c, 1) < 0)
                        return -1;
                if (c == 8 || c == 127) {
                        if (x) {
                                fputs("\b \b", stdout);
                                fflush(stdout);
                                --x;
                        }
                } else if (c == 13) {
                        putchar('\n');
                        buf[x] = 0;
                        return 0;
                } else if (c == 3) {
                        return -1;
                } else {
                        putchar(c); fflush(stdout);
                        buf[x++] = c;
                }
        }
}

/* Hex to integer */

int hatoi(unsigned char *buf)
{
        int addr;
        sscanf(buf, "%x", &addr);
        return addr;
}

/* Hex dump */

void hd(FILE *out, unsigned char *mem, int start, int len)
{
        int y;
        int skip = (start & 0x0F);
        int skip1 = skip;
        start &= ~0x0F;

        len += skip;

        y = 0;
        while (len > 0) {
                int x;
                int len1 = len;
                fprintf(out, "%4.4X:", start + y);
                for (x = 0; x != 16; ++x) {
                        if (skip || len <= 0) {
                                --skip;
                                fprintf(out, "   ");
                        } else {
                                fprintf(out, " %2.2X", mem[start +y + x]);
                        }
                        if (x == 7)
                                fprintf(out, " ");
                        --len;
                }
                fprintf(out, " ");
                for (x = 0; x != 16; ++x) {
                        unsigned char c = mem[start + y + x];
                        if (c < 32 || c > 126) c = '.';
                        if (skip1 || len1 <= 0) {
                                --skip1;
                                fprintf(out, " ");
                        } else {
                                fprintf(out, "%c", c);
                        }
                        --len1;
                }
                fprintf(out, "\n");
                y += 16;
        }
}

/* Break up whitespace separated words into an array of string pointers */

int fields(char *buf, char *words[])
{
        int n = 0;
        while (skipws(&buf) && *buf) {
                words[n++] = buf;
                skiptows(&buf);
                if (*buf)
                        *buf++ = 0;
        }
        return n;
}

/* Copy a string, src and dest may overlap */

char *jstrcpy(char *d, char *s)
{
        char *org_d = d;
        while (*d++ = *s++);
        return org_d;
}

/* Convert to upper case */

int to_upper(int c)
{
        if (c >= 'a' && c <= 'z')
                c += 'A' - 'a';
        return c;
}

/* stricmp not univeral.. */

int jstricmp(char *d, char *s)
{
        while (*d && *s && to_upper(*d) == to_upper(*s)) {
                ++d;
                ++s;
        }
        if (!*d && !*s)
                return 0;
        else
                return 1;
}

/* Save current TERMIOS settings */

struct termios saved; // Original attributes of local system
int termios_good; // Set if 'saved' is valid.

void save_termios()
{
        if (!tcgetattr(fileno(stdin), &saved))
                termios_good = 1;
        else
                termios_good = 0;
}

/* Restore TERMIOS settings */

void restore_termios()   
{
        if (termios_good)
                tcsetattr(fileno(stdin),TCSADRAIN,&saved);
}
 
/* Set TERMIOS for simulation */

void sim_termios()
{
        struct termios attr;
        if (!tcgetattr(fileno(stdin), &attr)) {
                /* attr.c_oflag&=~ONLCR; */
                attr.c_iflag&=~ICRNL; /* Yes */
                attr.c_lflag&=~ICANON;
                attr.c_lflag&=~ECHO; /* Yes */
                /* attr.c_lflag&=~ISIG; */
                /* attr.c_iflag&=~IGNBRK; */
                /* attr.c_iflag|=BRKINT; */
                tcsetattr(fileno(stdin),TCSADRAIN,&attr);
        }
}

/* stty -isig */

void nosig_termios()
{
        struct termios attr;
        if (!tcgetattr(fileno(stdin), &attr)) {
                attr.c_lflag&=~ISIG;
                tcsetattr(fileno(stdin),TCSADRAIN,&attr);
        }
}

/* stty sig */

void sig_termios()
{
        struct termios attr;
        if (!tcgetattr(fileno(stdin), &attr)) {
                attr.c_lflag|=ISIG;
                tcsetattr(fileno(stdin),TCSADRAIN,&attr);
        }
}
