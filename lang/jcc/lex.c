// Scanner / pre-processor

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lex.h"

// Macros

struct macro_arg {
        struct macro_arg *next;
        char *name;	// Argument name, "__VA_ARGS__" for varargs
        char *subst;	// Substitution text: could be NULL for varargs
};

struct macro {
        struct macro *next;
        char  *name;
        struct macro_arg *args;
        char *body;
        char *file_name;
        int line;
} *macros;

// #if management

struct cond {
        struct cond *next;
        int t; // 1 to keep code, -1 remove code until true elif or else, 0 remove code until end
        int e; // set if we've seen else (for duplicate else detection)
} *conds;

// Value associated with most recent token

char *word_buffer;
int word_buffer_size;
int word_buffer_len;

struct num num;

void promote(struct num *a, struct num *b)
{
        if (a->is_long_long && !b->is_long_long) {
                b->is_long_long = 1;
                b->num = (long)b->num;
        } else if (b->is_long_long && !a->is_long_long) {
                a->is_long_long = 1;
                a->num = (long)a->num;
        }
        if (a->is_unsigned || b->is_unsigned) {
                a->is_unsigned = 1;
                b->is_unsigned = 1;
        }
}

double float_val;

// Token unget buffer

int unget_valid;
int unget_tok_val;

// List of include paths

struct include_path {
        struct include_path *next;
        char *path;
} *include_paths, *include_paths_last;

// Input: it could be from a file or from a macro

int white = 1; // Set if we've only seen whitespace since beginning of line
int line;
FILE *file;
char *file_name;
struct include_path *file_whence;
struct macro *curmac; // Set if we're reading from a macro
char *macptr;
int unget_char;
int unget;

void word_buffer_init()
{
        if (!word_buffer) {
                word_buffer_size = 16;
                word_buffer = (char *)malloc(word_buffer_size);
        }
        word_buffer_len = 0;
}

void word_buffer_next(int c)
{
        if (word_buffer_len == word_buffer_size) {
                word_buffer_size *= 2;
                word_buffer = (char *)realloc(word_buffer, word_buffer_size);
        }
        word_buffer[word_buffer_len++] = c;
}

void word_buffer_first(int c)
{
        word_buffer_init();
        word_buffer_next(c);
}

char *word_buffer_done()
{
        if (word_buffer_len == word_buffer_size) {
                word_buffer_size *= 2;
                word_buffer = (char *)realloc(word_buffer, word_buffer_size);
        }
        word_buffer[word_buffer_len] = 0;
        return word_buffer;
}

// Keywords

struct keyword {
	char *name;
	int token;
} keyword[] = {
	{ "auto", tAUTO },
	{ "break", tBREAK },
	{ "case", tCASE },
	{ "char", tCHAR },
	{ "const", tCONST },
	{ "continue", tCONTINUE },
	{ "default", tDEFAULT },
	{ "do", tDO },
	{ "double", tDOUBLE },
	{ "else", tELSE },
	{ "enum", tENUM },
	{ "extern", tEXTERN },
	{ "float", tFLOAT },
	{ "for", tFOR },
	{ "goto", tGOTO },
	{ "if", tIF },
	{ "int", tINT },
	{ "long", tLONG },
	{ "register", tREGISTER },
	{ "return", tRETURN },
	{ "short", tSHORT },
	{ "signed", tSIGNED },
	{ "sizeof", tSIZEOF },
	{ "static", tSTATIC },
	{ "struct", tSTRUCT },
	{ "switch", tSWITCH },
	{ "typedef", tTYPEDEF },
	{ "union", tUNION },
	{ "unsigned", tUNSIGNED },
	{ "void", tVOID },
	{ "volatile", tVOLATILE },
	{ "while", tWHILE },
	{ 0, 0 }
};

// Preprocessor keywords

struct keyword preproc_word[] = {
        { "define", pDEFINE },
        { "undef", pUNDEF },
        { "include", pINCLUDE },
        { "include_next", pINCLUDE_NEXT },
        { "ifdef", pIFDEF },
        { "ifndef", pIFNDEF },
        { "pragma", pPRAGMA },
        { "elif", pELIF },
        { "endif", pENDIF },
        { "else", pELSE },
        { "if", pIF },
        { "error", pERROR },
        { "warning", pWARNING },
        { "line", pLINE },
	{ 0, 0 }
};

int lookup(struct keyword table[], char *s)
{
	int x;
	for (x = 0; table[x].name; ++x)
		if (!strcmp(table[x].name, s))
			return table[x].token;
	return tWORD;
}

// Deal with continuation lines and MS-DOS line endings
// Switch files when we reach EOF
// This is where tri-graphs would go

struct file_stack {
        struct file_stack *next;
        char *file_name;
        int line;
        FILE *file;
        struct macro *macro;
        char *macptr;
        int unget;
        int unget_char;
        struct include_path *whence;
} *file_stack;

int push_file(char *name, struct include_path *whence)
{
        FILE *f = fopen(name, "r");
        struct file_stack *s;
        if (!f) {
                return -1;
        }
        // printf("%s %d: Opened %s\n", file_name, line, name);
        s = (struct file_stack *)calloc(1, sizeof(struct file_stack));
        s->file_name = file_name;
        s->line = line;
        s->file = file;
        s->macro = curmac;
        s->whence = file_whence;
        s->macptr = macptr;
        s->unget = unget;
        s->unget_char = unget_char;
        s->next = file_stack;
        file_stack = s;
        file_name = name;
        file_whence = whence;
        line = 1;
        file = f;
        curmac = 0;
        macptr = 0;
        unget = 0;
        return 0;
}

void push_macro(struct macro *m, char *text)
{
        struct file_stack *s;
        s = (struct file_stack *)calloc(1, sizeof(struct file_stack));
        s->file_name = file_name;
        s->line = line;
        s->file = file;
        s->macro = curmac;
        s->macptr = macptr;
        s->unget = unget;
        s->unget_char = unget_char;
        s->next = file_stack;
        file_stack = s;
        file = 0;
        unget = 0;
        curmac = m;
        macptr = text;
}

void tok_ungetc(int c)
{
        if (c == '\n')
                --line;
        unget = 1;
        unget_char = c;
}

int tok_getc()
{
        int c;
        loop:
        if (unget) {
                unget = 0;
                if (unget_char == '\n') {
                        white = 1;
                        ++line;
                } return unget_char;
        }
        if (curmac) {
                if (!*macptr) {
                        if (file_stack) {
                                file_name = file_stack->file_name;
                                line = file_stack->line;
                                file = file_stack->file;
                                curmac = file_stack->macro;
                                macptr = file_stack->macptr;
                                unget = file_stack->unget;
                                unget_char = file_stack->unget_char;
                                file_stack = file_stack->next;
                                goto loop;
                        } else {
                                return EOF;
                        }
                } else {
                        return *macptr++;
                }
        }
        c = fgetc(file);
        if (c == EOF) {
                if (file_stack) {
                        file_name = file_stack->file_name;
                        line = file_stack->line;
                        file = file_stack->file;
                        curmac = 0;
                        macptr = 0;
                        unget = file_stack->unget;
                        unget_char = file_stack->unget_char;
                        file_stack = file_stack->next;
                        goto loop;
                } else {
                        return c;
                }
        } else if (c == '\r') {
                c = fgetc(file);
                if (c == '\n') {
                        ++line;
                        white = 1;
                        return c;
                } else {
                        ungetc(c, file);
                        return '\r';
                }
        } else if (c == '\n') {
                ++line;
                white = 1;
                return c;
        } else if (c == '\\') {
                c = fgetc(file);
                if (c == '\n') {
                        ++line;
                        goto loop;
                } else if (c == '\r') {
                        c = fgetc(file);
                        if (c == '\n') {
                                ++line;
                                goto loop;
                        } else {
                               ungetc(c, file);
                               return '\r'; // \ will be missing.. but \^M is an error anyway
                        }
                } else {
                        ungetc(c, file);
                        return '\\';
                }
        } else {
                return c;
        }
}

// Skip whitespace and return next character

// If found character is '#" and this is the first character
// on a line, process following preprocessor directive.

// If cross_lines is false, returns EOF on newlines.

// If noskip is true, return whitespace instead of eating it, but still quash commments

void handle_preproc();

int tok_skipws_getc(int cross_lines, int noskip)
{
        int c;
        for (;;) switch (c = tok_getc()) {
                case '\n': {
                        if (!cross_lines) {
                                tok_ungetc(c);
                                return EOF;
                        }
                        break;
		} case ' ': case '\t': case '\v': case '\f': {
		        if (noskip)
		                goto norm;
			break;
		} case '/': {
			c = tok_getc();
			if (c == '/') {
				// Skip comments to end of line
                                do
                                        c = tok_getc();
                                while (c != EOF && c != '\n');
                                if (c == '\n' && !cross_lines) {
                                        tok_ungetc(c);
                                        return EOF;
                                }
			} else if (c == '*') {
			        int first = line;
				// Skip comments to end of comment
				do {
				        do
        				        c = tok_getc();
                                        while (c != '*' && c != EOF);
                                        while (c == '*')
                                                c = tok_getc();
				} while (c != '/' && c != EOF);
				if (c == EOF) {
        				printf("%s %d: Error: file ended before comment on line %d\n", file_name, line, first);
        				exit(-1);
                                }
			} else {
				// / operator
				tok_ungetc(c);
				c = '/';
				goto norm;
			}
			break;
                } case '#': {
                        if (white) {
                                white = 0;
                                handle_preproc();
                        } else
                                goto norm;
                        break;
                } default: {
                        norm:
                        white = 0;
                        if (!cross_lines || !conds || conds->t == 1)
                                return c;
                        break;
                }
        }
}

const char *unesc(int c)
{
        static char buf[10];
        if (c >= 32 && c <= 126) {
                buf[0] = c;
                buf[1] = 0;
                return buf;
        } else if (c == -1) {
                buf[0] = 'E';
                buf[1] = 'O';
                buf[2] = 'F';
                buf[3] = 0;
                return buf;
        } else if (c >= 0 && c <= 255) {
                buf[0] = '\\';
                buf[1] = 'x';
                buf[2] = "0123456789abcdef"[0xF & (c >> 4)];
                buf[3] = "0123456789abcdef"[0xF & c];
                buf[4] = 0;
                return buf;
        } else {
                sprintf(buf, "[%x?]", c);
                return buf;
        }
}

int escape(int c)
{
        if (c == '\\') {
                c = tok_getc();
                switch (c) {
                        case 'n': return '\n';
                        case 't': return '\t';
                        case 'v': return '\v';
                        case 'b': return '\b';
                        case 'r': return '\r';
                        case 'f': return '\f';
                        case 'a': return '\a';
                        case '\\': return '\\';
                        case '?': return '?';
                        case '\'': return '\'';
                        case '"': return '"';
                        case 'x': {
                                unsigned char n = 0;
                                c = tok_getc();
                                if (c >= '0' && c <= '9')
                                        n = c - '0';
                                else if (c >= 'a' && c <= 'f')
                                        n = c - 'a' + 10;
                                else if (c >= 'A' && c <= 'F')
                                        n = c - 'A' + 10;
                                else {
                                        tok_ungetc(c);
                                        printf("%s %d: Error: invalid hex escape sequence\n", file_name, line);
                                        return n;
                                }
                                c = tok_getc();
                                if (c >= '0' && c <= '9')
                                        n = n * 16 + c - '0';
                                else if (c >= 'a' && c <= 'f')
                                        n = n * 16 + c - 'a' + 10;
                                else if (c >= 'A' && c <= 'F')
                                        n = n * 16 + c - 'A' + 10;
                                else
                                        tok_ungetc(c);
                                return n;
                        } case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': {
                                unsigned char n = c - '0';
                                c = tok_getc();
                                if (c >= '0' && c <= '7') {
                                        n = n * 8 + c - '0';
                                        c = tok_getc();
                                        if (c >= '0' && c <= '7') {
                                                n = n * 8 + c - '0';
                                        } else {
                                                tok_ungetc(c);
                                        }
                                } else {
                                        tok_ungetc(c);
                                }
                                return n;
                        } default: {
                                printf("%s %d: Error: unknown escape sequence %s\n", file_name, line, unesc(c));
                                tok_ungetc(c); // In case it's EOF
                                return 0;
                        }
                }
        } else {
                return c;
        }
}

int only_spaces(char *s)
{
        while (*s == ' ')
                ++s;
        if (*s)
                return 0;
        else
                return 1;
}

int collect_args(struct macro_arg *args)
{
        int c;
        int toomany = 0;
        int count = 0;
        if (args && args->subst) {
                free(args->subst);
                args->subst = 0;
        }
        loop:
        word_buffer_init();
        c = tok_skipws_getc(1, 0);
        for (;;) {
                if (args && !strcmp(args->name, "__VA_ARGS__")) {
                        // All remaining args should be put into this argument
                        for (;;) {
                                if (c == ')' && count) {
                                        --count;
                                        word_buffer_next(c);
                                } else if (c == EOF) {
                                        printf("%s %d: Error: input ended while in middle of macro argument list\n", file_name, line);
                                        return -1;
                                } else if (c == ')') {
                                        // End of all arguments
                                        word_buffer_done();
                                        if (!only_spaces(word_buffer))
                                                args->subst = strdup(word_buffer);
                                        goto done;
                                } else if (c == '\'' || c == '"') {
                                        word_buffer_next(c);
                                        for (;;) {
                                                int d = tok_getc();
                                                if (d == c) {
                                                        word_buffer_next(d);
                                                        break;
                                                } else if (d == '\\') {
                                                        word_buffer_next(d);
                                                        d = tok_getc();
                                                        if (d == EOF) {
                                                                printf("%s %d: Error: Bad escape sequence\n", file_name, line);
                                                                word_buffer_next('\\');
                                                        } else {
                                                                word_buffer_next(d);
                                                        }
                                                } else if (d == EOF) {
                                                        word_buffer_next(c);
                                                        printf("%s %d: Error input ended before string\n", file_name, line);
                                                        break;
                                                } else {
                                                        word_buffer_next(d);
                                                }
                                        }
                                } else if (c == '/') {
                                        c = tok_getc();
                                        if (c == '/') {
                                                word_buffer_next(' ');
                                                do
                                                        c = tok_getc();
                                                while (c != EOF && c != '\n');
                                        } else if (c == '*') {
                                                int first = line;
                                                // Skip comments to end of comment
                                                do {
                                                        do
                                                                c = tok_getc();
                                                        while (c == '*');
                                                } while (c != '/' && c != EOF);
                                                if (c == EOF) {
                                                        printf("%s %d: Error: file ended before comment on line %d\n", file_name, line, first);
                                                        exit(-1);
                                                }
                                                word_buffer_next(' ');
                                        } else {
                                                tok_ungetc(c);
                                                word_buffer_next(c);
                                        }
                                } else if (c == '(') {
                                        ++count;
                                        word_buffer_next(c);
                                } else {
                                        word_buffer_next(c);
                                }
                                c = tok_getc();
                        }
                        break;
                } else if (c == ')' && count) {
                        --count;
                        word_buffer_next(c);
                } else if (c == ',' && count) {
                        word_buffer_next(c);
                } else if (c == ')' || c == ',' || c == EOF) {
                        // End of argument
                        word_buffer_done();
                        if (args) {
                                if (strcmp(args->name, "__VA_ARGS__") || !only_spaces(word_buffer))
                                        args->subst = strdup(word_buffer);
                                args = args->next;
                                if (args) {
                                        if (args->subst) {
                                                free(args->subst);
                                                args->subst = 0;
                                        }
                                }
                        } else {
                                if (!toomany) {
                                        toomany = 1;
                                        printf("%s %d: Error: too many macro arguments\n", file_name, line);
                                }
                        }
                        if (c == ',')
                                goto loop;
                        else if (c == ')')
                                goto done;
                        else {
                                printf("%s %d: Error: input ended while in middle of macro argument list\n", file_name, line);
                                goto done;
                        }
                } else if (c == '\'' || c == '"') {
                        word_buffer_next(c);
                        for (;;) {
                                int d = tok_getc();
                                if (d == c) {
                                        word_buffer_next(d);
                                        break;
                                } else if (d == '\\') {
                                        word_buffer_next(d);
                                        d = tok_getc();
                                        if (d == EOF) {
                                                printf("%s %d: Error: Bad escape sequence\n", file_name, line);
                                                word_buffer_next('\\');
                                        } else {
                                                word_buffer_next(d);
                                        }
                                } else if (d == EOF) {
                                        word_buffer_next(c);
                                        printf("%s %d: Error input ended before string\n", file_name, line);
                                        break;
                                } else {
                                        word_buffer_next(d);
                                }
                        }
                } else if (c == '/') {
                        c = tok_getc();
                        if (c == '/') {
                                word_buffer_next(' ');
                                do
                                        c = tok_getc();
                                while (c != EOF && c != '\n');
                        } else if (c == '*') {
                                int first = line;
                                // Skip comments to end of comment
                                do {
                                        do
                                                c = tok_getc();
                                        while (c == '*');
                                } while (c != '/' && c != EOF);
                                if (c == EOF) {
                                        printf("%s %d: Error: file ended before comment on line %d\n", file_name, line, first);
                                        exit(-1);
                                }
                                word_buffer_next(' ');
                        } else {
                                tok_ungetc(c);
                                word_buffer_next(c);
                        }
                } else if (c == '(') {
                        ++count;
                        word_buffer_next(c);
                } else {
                        word_buffer_next(c);
                }
                c = tok_getc();
        }
        done:
        if (args && strcmp(args->name, "__VA_ARGS__")) {
                printf("%s %d: Error: not enough macro arguments\n", file_name, line);
                return -1;
        } else {
                return 0;
        }
}

char *macro_subst(struct macro *m, char *s)
{
        int c;
        int first = 0;
        word_buffer_init();
        for (;;) {
                c = *s++;
                switch(c) {
                        case 0: {
                                goto done;
                        } case '\'': case '"': {
                                int d;
                                first = 1;
                                word_buffer_next(c);
                                for (;;) {
                                        d = *s++;
                                        if (d == c) {
                                                word_buffer_next(d);
                                                break;
                                        } else if (d == '\\') {
                                                word_buffer_next(d);
                                                word_buffer_next(*s++);
                                        } else {
                                                word_buffer_next(d);
                                        }
                                }
                                break;
                        } case '#': {
                                if (*s == '#') {
                                        ++s;
                                        /* Edit these out... */
                                        while (*s == ' ') ++s;
                                        if (!*s || !first) {
                                                printf("%s %d: Error: '##' can not appear at either end of a macro\n", m->file_name, m->line);
                                        }
                                        while (word_buffer_len && word_buffer[word_buffer_len-1] == ' ')
                                                --word_buffer_len;
                                } else {
                                        while (*s == ' ') ++s;
                                        c = *s;
                                        if (c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                                                int start = word_buffer_len;
                                                struct macro_arg *arg;
                                                ++s;
                                                do {
                                                        word_buffer_next(c);
                                                        c = *s++;
                                                } while ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_');
                                                --s;
                                                word_buffer_done();
                                                for (arg = m->args; arg; arg = arg->next)
                                                        if (!strcmp(arg->name, word_buffer + start))
                                                                break;
                                                if (arg) {
                                                        int x;
                                                        word_buffer_len = start;
                                                        word_buffer_next('"');
                                                        if (arg->subst) {
                                                                for (x = 0; arg->subst[x]; ++x) {
                                                                        if (arg->subst[x] == '"')
                                                                                word_buffer_next('\\');
                                                                        word_buffer_next(arg->subst[x]);
                                                                }
                                                        }
                                                        word_buffer_next('"');
                                                } else {
                                                        printf("%s %d: Error: '%s' is not a macro parameter\n", m->file_name, m->line, word_buffer + start);
                                                        word_buffer_len = start;
                                                }
                                        } else {
                                                printf("%s %d: Error: expected macro parameter after #\n", m->file_name, m->line);
                                        }
                                }
                                break;
                        } case '_':
                          case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h':
                          case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p':
                          case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x':
                          case 'y': case 'z':
                          case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H':
                          case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P':
                          case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
                          case 'Y': case 'Z': {
                                char *orgs;
                                int start = word_buffer_len;
                                struct macro_arg *arg;
                                first = 1;
                                do {
                                        word_buffer_next(c);
                                        c = *s++;
                                } while ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_');
                                --s;
                                word_buffer_done();
                                for (arg = m->args; arg; arg = arg->next)
                                        if (!strcmp(arg->name, word_buffer + start))
                                                break;
                                if (arg) {
                                        int x;
                                        word_buffer_len = start;
                                        if (arg->subst) {
                                                for (x = 0; arg->subst[x]; ++x)
                                                        word_buffer_next(arg->subst[x]);
                                        }
                                }
                                break;
                        } default: {
                                if (c != ' ')
                                        first = 1;
                                word_buffer_next(c);
                                break;
                        }
                }
        }
        done:
        word_buffer_done();
        return strdup(word_buffer);
}

int macro_not_active(struct macro *m)
{
        struct file_stack *s;
        if (!curmac)
                return 1;
        if (curmac == m)
                return 0;
        for (s = file_stack; s->macptr; s = s->next)
                if (s->macro == m)
                        return 0;
        return 1;
}

// preproc == 0: convert C keywords to tokens: user calls this
// preproc == 1: convert preprocessor directives to tokens (return others as tWORDs), don't cross lines
// preproc == 2: leave words as tWORDs, don't cross lines: used for parsing macro argument list
// preproc == 3: expand macros, don't cross lines, look for 'defined' used for #if expressions

int get_tok(int preproc)
{
	int c, d;
	if (unget_valid) {
		unget_valid = 0;
		return unget_tok_val;
	}

	again:

	for (;;) switch (c = tok_skipws_getc(!preproc, 0)) {
	        case EOF: {
	                return tEOF;
		} case '~': case '(': case ')': case '[': case ']': case '{': case '}':
		  case ',': case '?': case ';': case ':': {
			return c;
		} case '+': case '-': {
			d = tok_getc();
			if (d == c || d == '=') {
				return c + (d << 8);
			} else {
				tok_ungetc(d);
				return c;
			}
		} case '<': case '>': {
			d = tok_getc();
			if (d == '=') {
				return c + (d << 8);
			} else if (d == c) {
				d = tok_getc();
				if (d == '=')
					return c + (c << 8) + (d << 16);
				else {
					tok_ungetc(d);
					return c + (c << 8);
				}
			} else {
				tok_ungetc(d);
				return c;
			}
		} case '|': case '&': case '=': {
			d = tok_getc();
			if (c == d || d == '=') {
				return c + (d << 8);
			} else {
				tok_ungetc(d);
				return c;
			}
		} case '%': case '^': case '!': case '*': case '/': {
			d = tok_getc();
			if (d == '=') {
				return c + (d << 8);
			} else {
				tok_ungetc(d);
				return c;
			}
		} case '"': {
                        word_buffer_init();
                        for (;;) {
                                c = tok_getc();
                                if (c == '"')
                                        break;
                                else if (c == EOF) {
                                        printf("%s %d: Error: file ended before string constant\n", file_name, line);
                                        exit(-1);
                                } else {
                                        word_buffer_next(escape(c));
                                }
                        }
                        word_buffer_done();
                        return tSTRING;
		} case '\'': {
                        num.num = 0;
                        num.is_long_long = 0;
                        num.is_unsigned = 1;
                        for (;;) {
                                c = tok_getc();
                                if (c == '\'')
                                        break;
                                else if (c == EOF) {
                                        printf("%s %d: Error: file ended before character constant\n", file_name, line);
                                        exit(-1);
                                } else {
                                        num.num = num.num * 256 + escape(c);
                                }
                        }
                        return tNUM;
		} case '_':
		  case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h':
		  case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p':
		  case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x':
		  case 'y': case 'z':
		  case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H':
		  case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P':
		  case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
		  case 'Y': case 'Z': {
		        word_buffer_first(c);
		        for (;;) {
        		  	c = tok_getc();
        		  	if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') ||
        		  	    (c >= 'A' && c <= 'Z') || c == '_') {
        		  		word_buffer_next(c);
        		  	} else {
        		  		tok_ungetc(c);
        		  		break;
        		  	}
                        }
                        word_buffer_done();
                        if (preproc == 2) {
                                return tWORD;
                        } else if (preproc == 1)
                                return lookup(preproc_word, word_buffer);
                        else if (preproc == 3 && !strcmp("defined", word_buffer))
                                return tDEFINED;
                        else {
                                struct macro *m;
                                for (m = macros; m; m = m->next)
                                        if (!strcmp(m->name, word_buffer))
                                                break;
                                if (m && macro_not_active(m)) {
                                        if (m->args) {
                                                c = tok_skipws_getc(1, 0);
                                                if (c == '(') {
                                                        struct macro_arg *arg;
                                                        collect_args(m->args);
#if 0
                                                        printf("Args:");
                                                        for (arg = m->args; arg; arg = arg->next) {
                                                                printf(" %s='%s'", arg->name, arg->subst ? arg->subst : "none");
                                                        }
                                                        printf("\n");
#endif
                                                        push_macro(m, macro_subst(m, m->body));
                                                        goto again;
                                                } else {
                                                        tok_ungetc(c);
                                                }
                                        } else {
                                                push_macro(m, strdup(m->body));
                                                goto again;
                                        }
                                }
                                if (preproc != 3)
                		  	return lookup(keyword, word_buffer);
                                else
                                        return tWORD;
                        }
		} case '0': {
			d = tok_getc();
			num.is_unsigned = 0;
			num.is_long_long = 0;
			if (d == 'x') {
			        c = tok_getc();
			        if (c >= '0' && c <= '9')
			                num.num = c - '0';
                                else if (c >= 'a' && c <= 'f')
                                        num.num = c - 'a' + 10;
                                else if (c >= 'A' && c <= 'F')
                                        num.num = c - 'A' + 10;
                                else {
                                        printf("%s %d: Error: bad hex constant\n", file_name, line);
                                        num.num = 0;
                                        tok_ungetc(c);
                                        return tNUM;
                                }
                                for (;;) {
                                        c = tok_getc();
                                        if (c >= '0' && c <= '9')
                                                num.num = num.num * 16 + c - '0';
                                        else if (c >= 'a' && c <= 'f')
                                                num.num = num.num * 16 + c - 'a' + 10;
                                        else if (c >= 'A' && c <= 'F')
                                                num.num = num.num * 16 + c - 'A' + 10;
                                        else {
                                                tok_ungetc(c);
                                                break;
                                        }
                                }
                                goto num_qual;
			} else if (d == '.') {
			        word_buffer_first(c);
			        word_buffer_next(d);
			        goto fp_after_dp;
                        } else if (d == 'e' || d == 'E') {
                                word_buffer_first(c);
                                word_buffer_next(d);
			        goto fp_after_exp;
			} else {
			        word_buffer_first(c);
			        tok_ungetc(d);
			        num.num = 0;
                                for (;;) {
                                        c = tok_getc();
                                        if (c >= '0' && c <= '7') {
                                                num.num = num.num * 8 + c - '0';
                                                word_buffer_next(c);
                                        } else if (c == '.') {
                                                word_buffer_next(c);
                                                goto fp_after_dp;
                                        } else if (c == 'e' || c == 'E') {
                                                word_buffer_next(c);
                                                goto fp_after_exp;
                                        } else {
                                                tok_ungetc(c);
                                                break;
                                        }
                                }
                                goto num_qual;
			}
		} case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
		        word_buffer_first(c);
		  	num.num = c - '0';
		  	num.is_unsigned = 0;
		  	num.is_long_long = 0;
		  	for (;;) {
		  	        c = tok_getc();
		  	        if (c >= '0' && c <= '9') {
		  	                num.num = num.num * 10 + c - '0';
		  	                word_buffer_next(c);
		  	        } else if (c == '.') {
		  	                word_buffer_next(c);
		  	                goto fp_after_dp;
		  	        } else if (c == 'e' || c == 'E') {
		  	                word_buffer_next(c);
		  	                goto fp_after_exp;
		  	        } else {
		  	                tok_ungetc(c);
		  	                break;
		  	        }
		  	}
		  	num_qual: c = tok_getc();
		  	if (c == 'u' || c == 'U') {
		  	        num.is_unsigned = 1;
		  	        c = tok_getc();
                        }
                        if (c == 'l' || c == 'L') {
                                c = tok_getc();
                                if (c == 'l' || c == 'L') {
                                        num.is_long_long = 1;
                                } else {
                                        tok_ungetc(c);
                                }
                        } else {
                                tok_ungetc(c);
                        }
                        if (num.num >= 0x100000000ULL)
                                num.is_long_long = 1;
		  	return tNUM;
		} case '.': {
		        d = tok_getc();
		        if (d >= '0' && d <= '9') {
		                word_buffer_first(c);
		                word_buffer_next(d);
		                fp_after_dp:
		                for (;;) {
		                        c = tok_getc();
		                        if (c >= '0' && c <= '9')
		                                word_buffer_next(c);
                                        else if (c == 'e' || c == 'E') {
                                                word_buffer_next(c);
                                                goto fp_after_exp;
                                        } else {
                                                tok_ungetc(c);
                                                goto fp_done;
                                        }
		                }
		                fp_after_exp:
		                c = tok_getc();
		                if (c == '+' || c == '-')
		                        word_buffer_next(c);
                                else
                                        tok_ungetc(c);
                                c = tok_getc();
                                if (c >= '0' && c <= '9') {
                                        word_buffer_next(c);
                                        for (;;) {
                                                c = tok_getc();
                                                if (c >= '0' && c <= '9')
                                                        word_buffer_next(c);
                                                else {
                                                        tok_ungetc(c);
                                                        break;
                                                }
                                        }
                                } else {
                                        printf("%s %d: Error: exponent missing from floating point constant\n", file_name, line);
                                }
                                fp_done:
                                float_val = strtod(word_buffer_done(), NULL);
                                return tFP;
                        } else if (d == '.') {
                                d = tok_getc();
                                if (d == '.')
                                        return ('.' << 16) + ('.' << 8) + '.';
                                else {
                                        tok_ungetc(d);
                                        return ('.' << 8) + '.';
                                }
		        } else {
		                tok_ungetc(d);
		                return c;
		        }
		} default: {
			printf("%s %d: Error: unexpected character '%c'\n", file_name, line, c);
			break;
		}
		 
	}
}

void unget_tok(int token)
{
	if (unget_valid) {
		fprintf(stderr, "Internal error (near %s %d): Only one unget_tok allowed\n", file_name, line);
		exit(-1);
	} else {
		unget_tok_val = token;
		unget_valid = 1;
	}
}

void eat_eol(int c)
{
        while (c != EOF)
                c = tok_skipws_getc(0, 0);
}

void show_eol(int c)
{
        while (c != EOF) {
                c = tok_skipws_getc(0, 1);
                if (c != EOF)
                        putchar(c);
        }
        putchar('\n');
}

void check_eol()
{
        int c = tok_skipws_getc(0, 0);
        if (c != EOF) {
                printf("%s %d: Error: extra junk after preprocessor directive\n", file_name, line);
                eat_eol(c);
        }
}

void add_path(char *s)
{
        struct include_path *p = (struct include_path *)calloc(1, sizeof(struct include_path));
        if (include_paths_last) {
                include_paths_last->next = p;
                include_paths_last = p;
        } else {
                include_paths = include_paths_last = p;
        }
        p->path = s;
}

struct macro_arg *get_macro_args()
{
        struct macro_arg *first, *last, *m;
        int c, d = 0;
        first = 0;
        last = 0;
        for (;;) {
                c = get_tok(2);
                if (c == ')' && d != ',') {
                        break;
                } else if (c == ('.' << 16) + ('.' << 8) + '.') {
                        m = (struct macro_arg *)calloc(1, sizeof(struct macro_arg));
                        if (!last)
                                first = last = m;
                        else
                                last = last->next = m;
                        m->name = strdup("__VA_ARGS__");
                        d = get_tok(2);
                        if (d == ')')
                                break;
                        else {
                                printf("%s %d: Error: syntax error in argument list\n", file_name, line);
                                eat_eol(d);
                                break;
                        }
                } else if (c == tWORD) {
                        for (m = first; m; m = m->next)
                                if (!strcmp(m->name, word_buffer))
                                        printf("%s %d: Error: duplicate macro argument name '%s'\n", file_name, line, word_buffer);
                        m = (struct macro_arg *)calloc(1, sizeof(struct macro_arg));
                        if (!last)
                                first = last = m;
                        else
                                last = last->next = m;
                        m->name = strdup(word_buffer);
                        d = get_tok(2);
                        if (d == ',') ;
                        else if (d == ')')
                                break;
                        else
                                unget_tok(d);
                } else {
                        printf("%s %d: Error: syntax error in argument list\n", file_name, line);
                        eat_eol(c);
                        break;
                }
        }
        return first;
}

char *get_macro_body()
{
        int c;
        int added = 0;
        word_buffer_init();
        // Skip leading whitespace
        c = tok_skipws_getc(0, 0);
        for (;;) {
                switch (c) {
                        case EOF: case '\n': {
                                /* Always end with a space: during replacement this keeps us in the macro at the ends of words, allowing
                                   us to detect macro recursion */
                                if (!added) {
                                        added = 1;
                                        word_buffer_next(' ');
                                }
                                goto done;
                        } case ' ': case '\t': case '\v': case '\f': {
                                if (!added) {
                                        added = 1;
                                        word_buffer_next(' ');
                                }
                                break;
                        } case '/': {
                                c = tok_getc();
                                if (c == '/') {
                                        do
                                                c = tok_getc();
                                        while (c != EOF && c != '\n');
                                        goto done;
                                } else if (c == '*') {
                                        int first = line;
                                        // Skip comments to end of comment
                                        do {
                                                do
                                                        c = tok_getc();
                                                while (c == '*');
                                        } while (c != '/' && c != EOF);
                                        if (c == EOF) {
                                                printf("%s %d: Error: file ended before comment on line %d\n", file_name, line, first);
                                                exit(-1);
                                        }
                                        if (!added) {
                                                added = 1;
                                                word_buffer_next(' ');
                                        }
                                } else {
                                        tok_ungetc(c);
                                        added = 0;
                                        word_buffer_next(c);
                                }
                                break;
                        } case '\'': case '\"':{
                                int d;
                                word_buffer_next(c);
                                for (;;) {
                                        d = tok_getc();
                                        if (d == c) {
                                                word_buffer_next(d);
                                                break;
                                        } else if (d == '\n' || d == EOF) {
                                                printf("%s %d: Error: unterminated string\n", file_name, line);
                                                word_buffer_next(c);
                                                break;
                                        } else if (d == '\\') {
                                                word_buffer_next(d);
                                                d = tok_getc();
                                                if (d == EOF) {
                                                        printf("%s %d: Error: bad escape sequence\n", file_name, line);
                                                        word_buffer_next('\\');
                                                } else
                                                        word_buffer_next(d);
                                        } else {
                                                word_buffer_next(d);
                                        }
                                }
                                break;
                        } default: {
                                added = 0;
                                word_buffer_next(c);
                        }
                }
                c = tok_getc();
        }
        done:
        word_buffer_done();
        return strdup(word_buffer);
}

int args_differ(struct macro_arg *a, struct macro_arg *b)
{
        while (a && b) {
                if (strcmp(a->name, b->name))
                        return 1;
                if (a->subst && b->subst && strcmp(a->subst, b->subst))
                        return 1;
                if (!a->subst && b->subst)
                        return 1;
                if (a->subst && !b->subst)
                        return 1;
                a = a->next;
                b = b->next;
        }
        return a || b;
}

void set_macro(char *name, struct macro_arg *args, char *body, int name_line)
{
        struct macro *m;
        for (m = macros; m; m = m->next)
                if (!strcmp(m->name, name))
                        break;
        if (m) {
                if (strcmp(body, m->body) || args_differ(args, m->args))
                        printf("%s %d: Error: duplicate define '%s'\n", file_name, name_line, name);
                else
                        /* Ignore exact duplicate macro definition */ ;
        } else {
                m = (struct macro *)calloc(1, sizeof(struct macro));
                m->name = name;
                m->args = args;
                m->body = body;
                m->next = macros;
                m->file_name = file_name;
                m->line = name_line;
                macros = m;
        }
}

void clr_macro(char *name)
{
        struct macro **m;
        for (m = &macros; *m; m = &(*m)->next)
                if (!strcmp((*m)->name, name))
                        break;
        if (*m)
                *m = (*m)->next;
}

void push_cond(int val)
{
        struct cond *t = (struct cond *)calloc(1, sizeof(struct cond));
        t->next = conds;
        t->t = val;
        t->e = 0;
        conds = t;
}

/*
* / %		11
+ -		10
<< >>		9
< <= > >=	8
== !=		7
&		6
^		5
|		4
&&		3
||		2
? :		1
*/

int preproc_expr(int prec, struct num *rtn)
{
        int sta;
        struct num rhs[1];
        int c = get_tok(3);
        if (c == EOF) {
                sta = -1;
        } else if (c == '!') {
                sta = preproc_expr(12, rtn);
                rtn->num = !rtn->num;
                rtn->is_long_long = 0;
                rtn->is_unsigned = 0;
        } else if (c == '-') {
                sta = preproc_expr(12, rtn);
                rtn->num = -rtn->num;
                if (!rtn->is_long_long)
                        rtn->num = (unsigned long)rtn->num;
        } else if (c == '~') {
                sta = preproc_expr(12, rtn);
                rtn->num = ~rtn->num;
                if (!rtn->is_long_long)
                        rtn->num = (unsigned long)rtn->num;
        } else if (c == '(') {
                sta = preproc_expr(0, rtn);
                c = get_tok(3);
                if (c != ')') {
                        printf("%s %d: Error: missing ')'\n", file_name, line);
                        sta = -1;
                        tok_ungetc(c);
                }
        } else if (c == tNUM) {
                sta = 0;
                *rtn = num;
        } else if (c == tDEFINED) {
                int flg = 0;
                rtn->is_long_long = 0;
                rtn->is_unsigned = 0;
                rtn->num = 0;
                c = get_tok(2);
                if (c == '(') {
                        flg = 1;
                        c = get_tok(2);
                }
                if (c != tWORD) {
                        printf("%s %d: Error: missing name after define\n", file_name, line);
                        unget_tok(c);
                        sta = -1;
                } else {
                        struct macro *m;
                        for (m = macros; m; m = m->next)
                                if (!strcmp(m->name, word_buffer))
                                        break;
                        if (m)
                                rtn->num = 1;
                        sta = 0;
                }
                if (flg) {
                        c = get_tok(2);
                        if (c != ')') {
                                printf("%s %d: Error: missing )\n", file_name, line);
                                unget_tok(c);
                                sta = -1;
                        }
                }
        } else if (c == tWORD) {
                rtn->is_long_long = 0;
                rtn->is_unsigned = 0;
                rtn->num = 0;
                struct macro *m;
                for (m = macros; m; m = m->next)
                        if (!strcmp(m->name, word_buffer))
                                break;
                if (m)
                        rtn->num = 1;
                sta = 0;
        } else {
                printf("%s %d: Error: unexpected token %d\n", file_name, line, c);
                sta = -1;
        }
        while (!sta) {
                c = get_tok(3);

                if (c == '*' && prec < 11) {
                        sta = preproc_expr(11, rhs);
                        promote(rtn, rhs);
                        rtn->num *= rhs->num;
                        if (!rtn->is_long_long)
                                rtn->num = (unsigned long)rtn->num;
                } else if (c == '/' && prec < 11) {
                        sta = preproc_expr(11, rhs);
                        if (!sta) {
                                if (!rhs->num) {
                                        printf("%s %d: Error: divide by 0\n", file_name, line);
                                        sta = -1;
                                } else {
                                        promote(rtn, rhs);
                                        rtn->num /= rhs->num;
                                        if (!rtn->is_long_long)
                                                rtn->num = (unsigned long)rtn->num;
                                }
                                        
                        }
                } else if (c == '%' && prec < 11) {
                        sta = preproc_expr(11, rhs);
                        if (!sta) {
                                if (!rhs->num) {
                                        printf("%s %d: Error: modulus by 0\n", file_name, line);
                                        sta = -1;
                                } else {
                                        promote(rtn, rhs);
                                        rtn->num %= rhs->num;
                                        if (!rtn->is_long_long)
                                                rtn->num = (unsigned long)rtn->num;
                                }
                        }

                } else if (c == '+' && prec < 10) {
                        sta = preproc_expr(10, rhs);
                        promote(rtn, rhs);
                        rtn->num += rhs->num;
                        if (!rtn->is_long_long)
                                rtn->num = (unsigned long)rtn->num;
                } else if (c == '-' && prec < 10) {
                        sta = preproc_expr(10, rhs);
                        promote(rtn, rhs);
                        rtn->num -= rhs->num;
                        if (!rtn->is_long_long)
                                rtn->num = (unsigned long)rtn->num;

                } else if (c == '<' + ('<' << 8) && prec < 9) {
                        sta = preproc_expr(9, rhs);
                        rtn->num <<= rhs->num;
                        if (!rtn->is_long_long)
                                rtn->num = (unsigned long)rtn->num;
                } else if (c == '>' + ('>' << 8) && prec < 9) {
                        sta = preproc_expr(9, rhs);
                        rtn->num >>= rhs->num;

                } else if (c == '>' && prec < 8) {
                        sta = preproc_expr(8, rhs);
                        promote(rtn, rhs);
                        rtn->num = (rtn->num > rhs->num);
                        rtn->is_long_long = 0;
                        rtn->is_unsigned = 0;
                } else if (c == '>' + ('=' << 8) && prec < 8) {
                        sta = preproc_expr(8, rhs);
                        promote(rtn, rhs);
                        rtn->num = (rtn->num >= rhs->num);
                        rtn->is_long_long = 0;
                        rtn->is_unsigned = 0;
                } else if (c == '<' && prec < 8) {
                        sta = preproc_expr(8, rhs);
                        promote(rtn, rhs);
                        rtn->num = (rtn->num < rhs->num);
                        rtn->is_long_long = 0;
                        rtn->is_unsigned = 0;
                } else if (c == '<' + ('=' << 8) && prec < 8) {
                        sta = preproc_expr(8, rhs);
                        promote(rtn, rhs);
                        rtn->num = (rtn->num <= rhs->num);
                        rtn->is_long_long = 0;
                        rtn->is_unsigned = 0;

                } else if (c == '=' + ('=' << 8) && prec < 7) {
                        sta = preproc_expr(7, rhs);
                        promote(rtn, rhs);
                        rtn->num = (rtn->num == rhs->num);
                        rtn->is_long_long = 0;
                        rtn->is_unsigned = 0;
                } else if (c == '!' + ('=' << 8) && prec < 7) {
                        sta = preproc_expr(7, rhs);
                        promote(rtn, rhs);
                        rtn->num = (rtn->num != rhs->num);
                        rtn->is_long_long = 0;
                        rtn->is_unsigned = 0;

                } else if (c == '&' && prec < 6) {
                        sta = preproc_expr(6, rhs);
                        promote(rtn, rhs);
                        rtn->num &= rhs->num;

                } else if (c == '^' && prec < 5) {
                        sta = preproc_expr(5, rhs);
                        promote(rtn, rhs);
                        rtn->num ^= rhs->num;

                } else if (c == '|' && prec < 4) {
                        sta = preproc_expr(4, rhs);
                        promote(rtn, rhs);
                        rtn->num |= rhs->num;

                } else if (c == '&' + ('&' << 8) && prec < 3) {
                        sta = preproc_expr(3, rhs);
                        rtn->num = (rtn->num && rhs->num);
                        rtn->is_long_long = 0;
                        rtn->is_unsigned = 0;

                } else if (c == '|' + ('|' << 8) && prec < 2) {
                        sta = preproc_expr(2, rhs);
                        rtn->num = (rtn->num && rhs->num);
                        rtn->is_long_long = 0;
                        rtn->is_unsigned = 0;

                } else if (c == '?' && prec <= 1) {
                        sta = preproc_expr(1, rhs);
                        c = get_tok(3);
                        if (c != ':') {
                                printf("%s %d: Error: missing : after ?\n", file_name, line);
                                sta = -1;
                        } else {
                                struct num frhs[1];
                                sta |= preproc_expr(1, frhs);
                                promote(rhs, frhs);
                                if (rtn->num) {
                                        *rtn = *rhs;
                                } else {
                                        *rtn = *frhs;
                                }
                        }

                } else {
                        unget_tok(c);
                        break;
                }
        }
        return sta;
}

void handle_preproc()
{
        int c;
        c = get_tok(1);
        switch (c) {
                case pINCLUDE: case pINCLUDE_NEXT: {
                        int include_next = (c == pINCLUDE_NEXT);
                        if (!conds || conds->t == 1) {
                                c = tok_skipws_getc(0, 0);
                                if (c == '<' || c == '\"') {
                                        char *fi = file_name;
                                        int li = line;
                                        int d = c;
                                        word_buffer_init();
                                        for (;;) {
                                                c = tok_getc();
                                                if (c == '>' || c == '\"' || c == EOF || c == '\n')
                                                        break;
                                                else
                                                        word_buffer_next(c);
                                        }
                                        word_buffer_done();
                                        if ((d == '<' && c == '>') || (d == '\"' && c == '\"')) {
                                                fi = file_name;
                                                li = line;
                                                check_eol();
                                                if (include_next || d != '\"' || push_file(strdup(word_buffer), include_paths)) {
                                                        struct include_path *p;
                                                        for (p = (include_next ? file_whence : include_paths); p; p = p->next) {
                                                                char buf[1024];
                                                                sprintf(buf, "%s/%s", p->path, word_buffer);
                                                                if (!push_file(strdup(buf), p->next))
                                                                        break;
                                                        }
                                                        if (!p) {
                                                                printf("%s %d: Error: couldn't open file '%s'\n", fi, li, word_buffer);
                                                        }
                                                }
                                        } else {
                                                printf("%s %d: Error: bad file name for include\n", fi, li);
                                                eat_eol(c);
                                        }
                                } else {
                                        printf("%s %d: Error: missing file name for include\n", file_name, line);
                                        eat_eol(c);
                                }
                        } else {
                                eat_eol(c);
                        }
                        break;
                } case pDEFINE: {
                        if (!conds || conds->t == 1) {
                                char *name;
                                struct macro_arg *args = NULL;
                                int d;
                                int name_line;
                                c = get_tok(2);
                                if (c != tWORD) {
                                        printf("%s %d: Error: missing macro name for define\n", file_name, line);
                                        eat_eol(c);
                                        break;
                                }
                                name = strdup(word_buffer);
                                name_line = line;
                                d = tok_getc();
                                if (d == '(') {
                                        // It's a macro
                                        args = get_macro_args();
                                } else {
                                        // It's a simple define
                                        tok_ungetc(d);
                                }
                                set_macro(name, args, get_macro_body(), name_line);
                        } else {
                                eat_eol(c);
                        }
                        break;
                } case pUNDEF: {
                        if (!conds || conds->t == 1) {
                                c = get_tok(2);
                                if (c != tWORD) {
                                        printf("%s %d: Error: missing macro name for undef\n", file_name, line);
                                        eat_eol(c);
                                        break;
                                }
                                clr_macro(word_buffer);
                                check_eol();
                        } else {
                                eat_eol(c);
                        }
                        break;
                } case pIFDEF: case pIFNDEF: {
                        if (!conds || conds->t == 1) {
                                int d = get_tok(2);
                                if (d != tWORD) {
                                        printf("%s %d: Error: missing argument for #ifdef or #ifndef\n", file_name, line);
                                        eat_eol(d);
                                        push_cond(0);
                                } else {
                                        struct macro *m;
                                        for (m = macros; m; m = m->next)
                                                if (!strcmp(m->name, word_buffer))
                                                        break;
                                        if ((m && c == pIFDEF) || (!m && c == pIFNDEF)) {
                                                push_cond(1);
                                        } else {
                                                push_cond(-1);
                                        }
                                        check_eol();
                                }
                        } else {
                                eat_eol(c);
                                push_cond(0);
                        }
                        break;
                } case pELSE: {
                        if (conds) {
                                if (conds->e) {
                                        printf("%s %d: Error: multiple #else in #if\n", file_name, line);
                                } else {
                                        conds->e = 1;
                                        if (conds->t == 1)
                                                conds->t = 0;
                                        else if (conds->t == -1)
                                                conds->t = 1;
                                }
                        } else {
                                printf("%s %d: Error: #else with no matching #if\n", file_name, line);
                        }
                        check_eol();
                        break;
                } case pENDIF: {
                        if (conds) {
                                struct cond *t = conds;
                                conds = t->next;
                                free(t);
                        } else {
                                printf("%s %d: Error: #endif with no matching #if\n", file_name, line);
                        }
                        check_eol();
                        break;
                } case pIF: {
                        if (!conds || conds->t == 1) {
                                struct num n;
                                int sta = preproc_expr(0, &n);
                                // printf("%s %d: IF expression (%llu%s%s)\n", file_name, line, n.num, (n.is_unsigned ? "u" : ""), (n.is_long_long ? "ll" : ""));
                                // Check for extra tokens...
                                c = get_tok(1);
                                if (!sta && c != EOF) {
                                        printf("%s %d: Error: extra junk after #if expression\n", file_name, line);
                                        sta = -1;
                                        push_cond(0);
                                } else if (sta) { // Parse error
                                        printf("%s %d: Error: bad or missing expression for #if\n", file_name, line);
                                        push_cond(0);
                                } else {
                                        if (n.num)
                                                push_cond(1);
                                        else
                                                push_cond(-1);
                                }
                        } else {
                                eat_eol(c);
                                push_cond(0);
                        }
                        break;
                } case pELIF: {
                        if (conds && conds->e) {
                                printf("%s %d: Error: #elif after #else\n", file_name, line);
                                eat_eol(c);
                        } else if (conds && conds->t == 1) {
                                conds->t = 0;
                                eat_eol(c);
                        } else if (conds && conds->t == -1) {
                                struct num n;
                                int sta;
                                conds->t = 1; // Tell tok_skipws_getc not to skip
                                sta = preproc_expr(0, &n);
                                c = get_tok(1);
                                conds->t = -1; // Re-enable skipping
                                if (!sta && c != EOF) {
                                        printf("%s %d: Error: extra junk after #elif expression\n", file_name, line);
                                        sta = -1;
                                } else if (sta) { // Parse error
                                        printf("%s %d: Error: bad or missing expression for #elif\n", file_name, line);
                                } else {
                                        if (n.num)
                                                conds->t = 1;
                                }
                        } else if (conds && conds->t == 0) {
                                eat_eol(c);
                        } else {
                                printf("%s %d: Error: #elif with no matching #if\n", file_name, line);
                                eat_eol(c);
                        }
                        break;
                } case pPRAGMA: {
                        // Ignore them
                        eat_eol(c);
                        break;
                } case pERROR: {
                        if (!conds || conds->t == 1) {
                                printf("%s %d: Error:", file_name, line);
                                show_eol(c);
                        } else {
                                eat_eol(c);
                        }
                        break;
                } case pWARNING: {
                        if (!conds || conds->t == 1) {
                                printf("%s %d: Warning:", file_name, line);
                                show_eol(c);
                        } else {
                                eat_eol(c);
                        }
                        break;
                } case tEOF: {
                        // Blank preprocessor lines are allowed */
                        break;
                } default: {
                        if (!conds || conds->t == 1)
                                printf("%s %d: Error: missing or unknown preprocessor directive\n", file_name, line);
                        eat_eol(c);
                }
        }
}

void show_tok(int c)
{
        switch(c) {
                case tEOF: printf("%s %d: tEOF\n", file_name, line); break;
                case tNUM: printf("%s %d: tNUM (%llu%s%s)\n", file_name, line, num.num, (num.is_unsigned ? "u" : ""), (num.is_long_long ? "ll" : "")); break;
                case tSTRING: {
                        int x;
                        printf("%s %d: tSTRING \"", file_name, line);
                        for (x = 0; x != word_buffer_len; ++x)
                                printf("%s", unesc(((unsigned char *)word_buffer)[x]));
                        printf("\"\n");
                        break;
                }
                case tWORD: printf("%s %d: tWORD (%s)\n", file_name, line, word_buffer); break;
                case tFP: printf("%s %d: tFP (%lg)\n", file_name, line, float_val); break;
                case tAUTO: printf("%s %d: tAUTO\n", file_name, line); break;
                case tBREAK: printf("%s %d: tBREAK\n", file_name, line); break;
                case tCASE: printf("%s %d: tCASE\n", file_name, line); break;
                case tCHAR: printf("%s %d: tCHAR\n", file_name, line); break;
                case tCONST: printf("%s %d: tCONST\n", file_name, line); break;
                case tCONTINUE: printf("%s %d: tCONTINUE\n", file_name, line); break;
                case tDEFAULT: printf("%s %d: tDEFAULT\n", file_name, line); break;
                case tDO: printf("%s %d: tDO\n", file_name, line); break;
                case tDOUBLE: printf("%s %d: tDOUBLE\n", file_name, line); break;
                case tELSE: printf("%s %d: tELSE\n", file_name, line); break;
                case tENUM: printf("%s %d: tENUM\n", file_name, line); break;
                case tEXTERN: printf("%s %d: tEXTERN\n", file_name, line); break;
                case tFLOAT: printf("%s %d: tFLOAT\n", file_name, line); break;
                case tFOR: printf("%s %d: tFOR\n", file_name, line); break;
                case tGOTO: printf("%s %d: tGOTO\n", file_name, line); break;
                case tIF: printf("%s %d: tIF\n", file_name, line); break;
                case tINT: printf("%s %d: tINT\n", file_name, line); break;
                case tLONG: printf("%s %d: tLONG\n", file_name, line); break;
                case tREGISTER: printf("%s %d: tREGISTER\n", file_name, line); break;
                case tRETURN: printf("%s %d: tRETURN\n", file_name, line); break;
                case tSHORT: printf("%s %d: tSHORT\n", file_name, line); break;
                case tSIGNED: printf("%s %d: tSIGNED\n", file_name, line); break;
                case tSIZEOF: printf("%s %d: tSIZEOF\n", file_name, line); break;
                case tSTATIC: printf("%s %d: tSTATIC\n", file_name, line); break;
                case tSTRUCT: printf("%s %d: tSTRUCT\n", file_name, line); break;
                case tSWITCH: printf("%s %d: tSWITCH\n", file_name, line); break;
                case tTYPEDEF: printf("%s %d: tTYPEDEF\n", file_name, line); break;
                case tUNION: printf("%s %d: tUNION\n", file_name, line); break;
                case tUNSIGNED: printf("%s %d: tUNSIGNED\n", file_name, line); break;
                case tVOID: printf("%s %d: tVOID\n", file_name, line); break;
                case tVOLATILE: printf("%s %d: tVOLATILE\n", file_name, line); break;
                case tWHILE: printf("%s %d: tWHILE\n", file_name, line); break;
                default: {
                        printf("%s %d: ", file_name, line);
                        while (c) {
                                printf("%c", c & 255);
                                c >>= 8;
                        }
                        printf("\n");
                }
        }
}

void show_macros()
{
        struct macro *m;
        for (m = macros; m; m = m->next) {
                struct macro_arg *a;
                printf("Macro %s (", m->name);
                for (a = m->args; a; a = a->next) {
                        printf(" %s ", a->name);
                }
                printf(") %s\n", m->body);
        }
}

int main(int argc, char *argv[])
{
        int c;
        // Include path from: gcc -dM -E - < /dev/null
        add_path("/usr/lib/gcc/x86_64-pc-cygwin/7.3.0/include");
        add_path("/usr/include");
        add_path("/usr/include/w32api");
        file_whence = include_paths;
        file_name = argv[1];
        file = fopen(file_name, "r");
        line = 1;
        do {
                c = get_tok(0);
                show_tok(c);
        } while(c != tEOF);
        show_macros();
        return 0;
}
