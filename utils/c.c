/* Connnect to a serial port
 - Joe Allen
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <termios.h>
#include <sys/fcntl.h>
#include <signal.h>
#include <errno.h>
#include <pwd.h>
#include <sys/ioctl.h>
#include <unistd.h>

/* TODO:
  allow escape characters in send_string (command line use is easier)
  delete lock if user hits ^C
  check option values when parsing file / command line
  echo option needs to go with local tty
  simplify symctrl
*/

// Uncomment to prompt for name for lock owner
#define ASKNAME 1

char *killnl(char *p)
{
	if (p && *p) {
		unsigned len = strlen(p);
		if (p[len - 1] == '\n')
			p[len - 1] = 0;
	}
	return p;
}

char *jgets(char *buf, int len)
{
	return killnl(fgets(buf, len - 1, stdin));
}

/* Syntax checking for options */

const char *syntax[]=
{

	"baud", "Baud rate", "9600", "921600", "460800", "230400", "115200",
	"57600", "38400", "19200", "4800", "2400", "1800", "1200", "600",
	"300", "200", "150", "134", "110", "75", "50", 0,

	"flow", "Flow control", "none", "rtscts", "xonxoff", 0,

	"stop", "No. stop bits", "1", "2", 0,

	"bits", "No. data bits", "8", "7", "6", "5", 0,

	"parity", "Parity", "none", "even", "odd", 0,

	"hup", "Hangup: lower DTR on exit", "on", "off", 0,

	"cd", "Carrier detect: exit if DCD goes low", "off", "on", 0,

	"on", "Command to turn power on", "", 0,

	"off", "Command to turn power off", "", 0,

	"help", "Show help", 0,

	"send", "Send string to serial port and exit", "", 0,

	"noansi", "Suppress ANSI sequences", 0,

	"hexdump", "Display received data in hexadecimal", 0,

	"h", "Give remote host name (from telnetd)", "", 0,

	"p", "Mystery option from telnetd", 0,

	0
};

/* True if value is OK */

int check_val(char *name, char *val)
{
	const char **p;

	for (p = syntax; *p;) {
		if (!strcmp(*p, name)) {
			++p;
			while (*++p) {
				if (!strcmp(*p, "") || !strcmp(*p, val))
					return 1;
			}
		} else {
			while (*p++);
		}
	}
	return 0;
}

/* Does option exist? */

int check_opt(char *name)
{
	const char **p;

	for (p = syntax; *p;) {
		if (!strcmp(*p, name)) {
			return 1;
		} else {
			while (*p++);
		}
	}
	return 0;
}

/* Does option need an argument? */

int needs_arg(char *name)
{
	const char **p;

	for (p = syntax; *p;) {
		if (!strcmp(*p, name)) {
			if (p[2])
				return 1;
			else
				return 0;
		} else {
			while (*p++);
		}
	}
	return 1; /* Unknown option.. assume yes */
}

/* Show allowed values for option */

int show_valid_vals(const char *name)
{
	const char **p;

	for (p = syntax; *p;) {
		if (!strcmp(*p, name)) {
			++p;
			while (*++p)
				printf(" %s", *p);
			printf("\n");
			return 1;
		} else {
			while (*p++);
		}
	}
	return 0;
}

/* Print list of possible options with help */

void show_valid_opts()
{
	const char **p;
	for (p = syntax; *p;) {
		if (p[2]) // Don't show options with no args since user can't set them
			printf("%-8s  %s\n", p[0], p[1]);
		while (*p++);
	}
}

void show_valid_opts_cmd()
{
	const char **p;
	for (p = syntax; *p;) {
		if (p[2]) {
			char buf[80];
			sprintf(buf, "--%s <val>", p[0]);
			printf("  %-18s %s\n", buf, p[1]);
			if (p[2][0]) {
				printf("    one of:"); show_valid_vals(p[0]);
			}
				printf("\n");
		} else {
			char buf[80];
			sprintf(buf, "--%s", p[0]);
			printf("  %-18s %s\n\n", buf, p[1]);
		}
		while (*p++);
	}
}

/* Functions for processing options stored in files or on the command line */

/* Linked list of options */

struct opt {
	struct opt *next;
	char *name;
	char *value;
};

/* Option database */

struct opt *opts;

/* Find named option, NULL if none found */

struct opt *find_opt(struct opt *list, const char *name)
{
	while (list && strcmp(list->name, name))
		list = list->next;
	return list;
}

/* Get option value, NULL if none found */

char *get_opt(struct opt *list, const char *name)
{
	struct opt *o = find_opt(list, name);
	if (o)
		return o->value;
	else
		return 0;
}

/* Set option value */

struct opt *set_opt(struct opt *list, const char *name, const char *value)
{
	struct opt *o = find_opt(list, name);
	if (o) {
		free(o->value);
		o->value = strdup(value);
		return list;
	} else {
		o = (struct opt *)malloc(sizeof(struct opt));
		o->name = strdup(name);
		o->value = strdup(value);
		o->next = list;
		return o;
	}
}

/* Quote a single character */

char *quote(int c)
{
	static char buf[8];
	if (c == '\n')
		sprintf(buf, "\\n");
	else if (c == '\r')
		sprintf(buf, "\\r");
	else if (c == 27)
		sprintf(buf, "\\e");
	else if (c == '\b')
		sprintf(buf, "\\b");
	else if (c == '\t')
		sprintf(buf, "\\t");
	else if (c == '\f')
		sprintf(buf, "\\f");
	else if (c == '\a')
		sprintf(buf, "\\a");
	else if (c == '\v')
		sprintf(buf, "\\v");
	else if (c == '\\')
		sprintf(buf, "\\\\");
	else if (c < 32 || c > 126)
		sprintf(buf, "\\x%2.2x", c);
	else
		sprintf(buf, "%c", c);
	return buf;
}

/* Quote string */

void quotes(FILE *f, char *s)
{
	while (*s) {
		fprintf(f, "%s", quote(*(unsigned char *)s));
		++s;
	}
}

/* Print options to a FILE */

void show_opts(FILE *f, struct opt *list)
{
	while (list) {
		fprintf(f, "%s = \"", list->name);
		quotes(f, list->value);
		fprintf(f, "\"\n");
		list = list->next;
	}
}

/* Skip whitespace */

int skipws(char **t)
{
	char *s = *t;
	while (*s == ' ' || *s == '\t')
		++s;
	*t = s;
	return 1;
}

/* True if blank or comment line */

int blankline(char **t)
{
	char *s = *t;
	if (!*s || *s == '#' || *s == '\r' || *s == '\n')
		return 1;
	return 0;
}

/* Parse field */

int parse_field(char *val, char **t)
{
	char *org = val;
	char *s = *t;
	skipws(&s);
	while (*s != ' ' && *s != '\t' && *s && *s != '=' && *s != '\r' && *s != '\n' && *s != '#')
		*val++ = *s++;
	*t = s;
	*val = 0;
	return org != val;
}

/* Parse character */

int parse_char(char c, char **t)
{
	char *s = *t;
	if (*s == c) {
		++s;
		*t = s;
		return 1;
	}
	return 0;
}

/* Convert ASCII hex digit to binary */

int hexbin(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	else if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	else if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	else
		return -1;
}

/* Parse one character from a string */

int parse_string_char(char *val, char **t)
{
	char *s = *t;
	if (*s == '\\') {
		++s;
		if (*s == '\\')
			*val = '\\';
		else if (*s == '\r')
			*val = '\r';
		else if (*s == '\n')
			*val = '\n';
		else if (*s == '\b')
			*val = '\b';
		else if (*s == '\f')
			*val = '\f';
		else if (*s == '\a')
			*val = '\a';
		else if (*s == '\t')
			*val = '\t';
		else if (*s == '\v')
			*val = '\v';
		else if (*s == '\"')
			*val = '"';
		else if (*s == 'e')
			*val = '\x1b';
		else if (*s == 'x') {
			*val = 0;
			if (hexbin(s[1]) >= 0) {
				*val = hexbin(s[1]);
				++s;
				if (hexbin(s[1]) >= 0) {
					*val = *val * 16 + hexbin(s[1]);
				}
			}
		}
		++s;
	} else
		*val = *s++;
	*t = s;
	return 1;
}

/* Parse a string */

int parse_string(char *val, char **t)
{
	char *s = *t;
	if (*s == '"') {
		++s;
		while (*s && *s != '\"') {
			parse_string_char(val++, &s);
		}
		*val = 0;
		if (*s == '"') {
			++s;
			*t = s;
			return 1;
		}
	}
	return 0;
}

/* Read options from rc file */

struct opt *proc_rc(char *name)
{
	char buf[1024];
	struct opt *opt = 0;
	FILE *f = fopen(name, "r");
	int line = 0;
	if (!f)
		return NULL;
	printf("Reading options from %s...\n", name);
	while (fgets(buf, sizeof(buf) - 1, f)) {
		char *s;
		char nam[1024];
		char value[1024];
		++line;
		s = buf;
		skipws(&s);
		if (skipws(&s) && blankline(&s)) {
			/* Do nothing: comment or blank line */
		} else if (parse_field(nam, &s) && skipws(&s) && parse_char('=', &s) && skipws(&s)) {
			if (parse_string(value, &s)) {
				opt = set_opt(opt, nam, value);
			} else if (parse_field(value, &s)) {
				opt = set_opt(opt, nam, value);
			} else {
				fprintf(stderr,"%s %d: Syntax error: value missing\n", nam, line);
				exit(1);
			}
		} else {
			fprintf(stderr,"%s %d: Syntax error\n", nam, line);
		}
			
	}
	fclose(f);
	return opt;
}

/* Read options from command line */

struct opt *proc_args(struct opt *op, char **argv)
{
	while (*argv) {
		if (argv[0][0] == '-') { /* Found some kind of option */
			char *p = argv[0] + 1;
			char *q;
			if (*p == '-')
				++p;
			/* Check for ---baud= format */
			for (q = p; *q && *q != '='; ++q);
			if (*q == '=')
				*q++ = 0;
			if (!needs_arg(p)) {
				op = set_opt(op, p, "1");
			} else if (*q || argv[1]) {
				if (*q) { /* We have argument after = */
					op = set_opt(op, p, q);
				} else { /* We don't, so take next arg from commnad line */
					++argv;
					op = set_opt(op, p, argv[0]);
				}
			} else {
				fprintf(stderr, "Syntax error\n");
				return 0;
			}
		} else { /* Otherwise it's the port we wish to connect to */
			op = set_opt(op, "port", argv[0]);
		}
		++argv;
	}
	return op;
}

/* Read options from file and process command line arguments */

char opts_file_name[1024] = "";

/* Program name */

char *pgm_name;

struct opt *proc_opts(char **argv)
{
	struct opt *op = 0;
	/* Parse option file */
	if ((pgm_name = strrchr(argv[0], '/')))
		pgm_name = pgm_name + 1;
	else
		pgm_name = argv[0];
	sprintf(opts_file_name, "%s/.%src", (getenv("HOME") ? getenv("HOME") : "/root"), pgm_name);

	op = proc_rc(opts_file_name);

	op = proc_args(op, argv + 1);

//	show_opts(stdout, op);

	return op;
}

/* Save options to file */

int save_opts(void)
{
	FILE *f;
	if (opts_file_name[0]) {
		f = fopen(opts_file_name, "w");
		if (!f) {
			perror("fopen");
			return 0;
		}
		show_opts(f, opts);
		fclose(f);
		printf("Wrote current options to %s\n", opts_file_name);
		return 1;
	}
	return 0;
}

#define DEFAULT_PORT "/dev/ttyS0"

/* Send break to serial port when we get this character.  Telnet "send brk" does this. */
#define BRK_CHAR ('\\' - '@')

/* Escape character */
#define ESC_CHAR '~'
/* #define ESC_CHAR (']' - '@') */

/* Set to zero for escape character to only work at beginning of line */
#define ESC_ANYWHERE 1

/* Set so that two escape characters in a row is converted into one: quoted escape */
#define ESC_DOUBLE 1

// Options
int no_ansi = 0; // Delete ANSI control sequences from remote device
int hex_dump = 0; // Display received data in hex
const char *port = 0; // Name of remote port
int remote_fd = -1; // fd of remote port
int local_fd; // fd of local port

// File to use for logging..
FILE *log_file = 0;
char log_file_name[1024];

// File we're sending...
FILE *send_file = 0;
char send_file_name[1024];

struct termios saved; // Original attributes of local system
int termios_good; // Set if 'saved' is valid.

/* Un-escape */

char *unesc(int c)
{
	static char buf[10];
	if (c < 32)
		sprintf(buf, "^%c", c + '@');
	else if (c < 127)
		sprintf(buf, "%c", c);
	else if (c == 127)
		sprintf(buf, "^?");
	else if (c < 160)
		sprintf(buf, "M-^%c", c + '@');
	else if (c < 255)
		sprintf(buf, "M-%c", c);
	else
		sprintf(buf, "M-^?");
	return buf;
}

/* Record current attributes */
void save_termios()
{
	if (!tcgetattr(local_fd, &saved))
		termios_good = 1;
	else
		termios_good = 0;
}

void got_break(int foo)
{
	printf("Got int (signal %d).. probably a break!\n", foo);
	fflush(stdout);
}

/* Change '/' in path to '+' */

char *sanitize(const char *name)
{
	static char buf[100];
	int x;
	strcpy(buf, name);
	for (x = 0; buf[x]; ++x)
		if (buf[x] == '/') buf[x] = '+';
	return buf;
}

/* Remove lock file because we're about to exit */

void removelock()
{
	char lockname[100];
	sprintf(lockname, "/tmp/lock..%s", sanitize(port));
	unlink(lockname);
}

/* Set local tty attributes for connected */
void cu_termios()
{
	struct termios attr;
	if (!tcgetattr(local_fd, &attr)) {
		attr.c_oflag &= ~ONLCR;
		attr.c_iflag &= ~ICRNL;
		attr.c_iflag &= ~(IXON|IXOFF);
		attr.c_lflag &= ~ICANON;
		attr.c_lflag &= ~ECHO;
		attr.c_lflag &= ~ISIG;
		attr.c_iflag &= ~IGNBRK;
		attr.c_iflag |= BRKINT;
		tcsetattr(local_fd, TCSADRAIN, &attr);
		signal(SIGINT, got_break);
	}
}

/* Restore original local tty attributes */

void restore_termios()
{
	if (termios_good)
		tcsetattr(local_fd, TCSADRAIN, &saved);
}

/* Baud rate table */

struct {
	const speed_t baud;
	const char *name;
} baud_table[] = 
{
	{ B50, "50" },
	{ B75, "75" },
	{ B110, "110" },
	{ B134, "134" },
	{ B150, "150" },
	{ B200, "200" },
	{ B300, "300" },
	{ B600, "600" },
	{ B1200, "1200" },
	{ B1800, "1800" },
	{ B2400, "2400" },
	{ B4800, "4800" },
	{ B9600, "9600" },
	{ B19200, "19200" },
	{ B38400, "38400" },
	{ B57600, "57600" },
	{ B115200, "115200" },
	{ B230400, "230400" },
	{ B460800, "460800" },
	{ B921600, "921600" },
	{ -1, 0 }
};

speed_t find_baud(char *s)
{
	int x;
	for (x = 0; baud_table[x].name; ++x)
		if (!strcmp(baud_table[x].name, s))
			return baud_table[x].baud;
	fprintf(stderr, "Unknown baud rate '%s', defaulting to 9600\n", s);
	return 9600;
}

const char *baud_string(speed_t b)
{
	int x;
	for (x = 0; baud_table[x].name; ++x)
		if (baud_table[x].baud == b)
			return baud_table[x].name;
	return "unknown";
}

void symctrl(int fd)
{
	char *s;
	speed_t baud;
	struct termios attr;
	if(!tcgetattr(fd, &attr)) { /* We have a tty */

		/* Force modes we always want */
		attr.c_iflag |= (IGNBRK | IGNPAR);
		attr.c_iflag &= ~(BRKINT | PARMRK | INPCK | ISTRIP | INLCR | IGNCR | IUCLC | IXANY);
		attr.c_oflag |= (OPOST | OFILL);
		attr.c_oflag &= ~(OLCUC | OCRNL | ONOCR | ONLRET | OFDEL | NLDLY | CRDLY | TABDLY | BSDLY | VTDLY | FFDLY);
		attr.c_cflag |= (CREAD);
		attr.c_lflag &= ~(XCASE | ECHONL | ECHOCTL | ECHOPRT | ECHOK | NOFLSH);
		attr.c_lflag |= (ECHOE | ECHOKE);

		/* Modes we usually want */
		attr.c_lflag &= ~(ECHO | ISIG | ICANON);
		attr.c_oflag &= ~(ONLCR);
		attr.c_iflag &= ~(ICRNL);

		/* Stop bits */
		attr.c_cflag &= ~CSTOPB;

		if ((s = get_opt(opts, "stop"))) {
			if (!strcmp(s, "1")) {
				attr.c_cflag &= ~CSTOPB;
			} else if (!strcmp(s, "2")) {
				attr.c_cflag |= CSTOPB;
			}
		}

		/* Data bits */
		attr.c_cflag = (attr.c_cflag & ~CSIZE) | CS8;

		if ((s = get_opt(opts, "bits"))) {
			if (!strcmp(s, "5")) {
				attr.c_cflag = (attr.c_cflag & ~CSIZE) | CS5;
			} else if (!strcmp(s, "6")) {
				attr.c_cflag = (attr.c_cflag & ~CSIZE) | CS6;
			} else if (!strcmp(s, "7")) {
				attr.c_cflag = (attr.c_cflag & ~CSIZE) | CS7;
			} else if (!strcmp(s, "8")) {
				attr.c_cflag = (attr.c_cflag & ~CSIZE) | CS8;
			}
		}

		/* Echo control */
		/* Should go to local fd, not remote */
		attr.c_lflag &= ~ECHO;

		if ((s = get_opt(opts, "echo"))) {
			if (!strcmp(s, "on")) {
				attr.c_lflag |= ECHO;
			} else if (!strcmp(s, "off")) {
				attr.c_lflag &= ~ECHO;
			}
		}

		/* Flow control */
		attr.c_iflag &= ~(IXON|IXOFF);
		attr.c_cflag |= CRTSCTS;

		if ((s = get_opt(opts, "flow"))) {
			if (!strcmp(s, "none")) {
				attr.c_iflag &= ~(IXON|IXOFF);
				attr.c_cflag &= ~CRTSCTS;
			} else if (!strcmp(s, "rtscts")) {
				attr.c_iflag &= ~(IXON|IXOFF);
				attr.c_cflag |= CRTSCTS;
			} else if (!strcmp(s, "xonxoff")) {
				attr.c_iflag |= (IXON|IXOFF);
				attr.c_cflag &= ~CRTSCTS;
			}
		}

		/* Parity */
		attr.c_cflag &= ~PARENB;

		if ((s = get_opt(opts, "parity"))) {
			if (!strcmp(s, "none")) {
				attr.c_cflag &= ~PARENB;
			} else if (!strcmp(s, "even")) {
				attr.c_cflag |= PARENB;
				attr.c_cflag &= ~PARODD;
			} else if (!strcmp(s, "odd")) {
				attr.c_cflag |= (PARENB | PARODD);
			}
		}

		/* Carrier detect */
		attr.c_cflag |= CLOCAL;

		if ((s = get_opt(opts, "cd"))) {
			if (!strcmp(s, "on")) {
				attr.c_cflag &= ~CLOCAL;
			} else if (!strcmp(s, "off")) {
				attr.c_cflag |= CLOCAL;
			}
		}

		/* Hangup */
		attr.c_cflag |= HUPCL;

		if ((s = get_opt(opts, "hup"))) {
			if (!strcmp(s, "on")) {
				attr.c_cflag |= HUPCL;
			} else if (!strcmp(s, "off")) {
				attr.c_cflag &= ~HUPCL;
			}
		}

		/* Baud rate */

		baud = B9600;

		if ((s = get_opt(opts, "baud")))
			baud = find_baud(s);

		cfsetospeed(&attr, baud);
		cfsetispeed(&attr, baud);

	tcsetattr(fd, TCSADRAIN, &attr);
	}
}

speed_t get_baud(int fd)
{
	struct termios attr;
	if(!tcgetattr(fd, &attr)) { /* We have a tty */
		return cfgetospeed(&attr);
	}
	return -1;
}

// Connect to some kind of port
// For example:
//    /dev/ttyS0:baud=115200
//    192.168.0.1:22
int symopen(const char *name, int mode)
{
	int fd;
	char buf[1024];
	char *ptr;
	strcpy(buf, name);
	if((ptr = strrchr(buf, ':')))
		*ptr = 0;
	fd = open(buf, mode);
	if(fd >= 0) {
		symctrl(fd);
	}
	return fd;
}

void show_esc_help()
{
	char buf[100];
	if (ESC_DOUBLE)
		sprintf(buf, "%s%s", unesc(ESC_CHAR), unesc(ESC_CHAR)); printf(" %-12s Send %s\n", buf, unesc(ESC_CHAR));
	sprintf(buf, "%s", unesc(BRK_CHAR)); printf(" %-12s Send break\n", buf);
	sprintf(buf, "%sb", unesc(ESC_CHAR)); printf(" %-12s Send break\n", buf);
	sprintf(buf, "%sq", unesc(ESC_CHAR)); printf(" %-12s Exit\n", buf);
	sprintf(buf, "%sh", unesc(ESC_CHAR)); printf(" %-12s Help\n", buf);
	sprintf(buf, "%sf", unesc(ESC_CHAR)); printf(" %-12s Turn machine off\n", buf);
	sprintf(buf, "%sn", unesc(ESC_CHAR)); printf(" %-12s Turn machine on\n", buf);
	sprintf(buf, "%s!cmd", unesc(ESC_CHAR)); printf(" %-12s Execute shell command\n", buf);
	sprintf(buf, "%s<file", unesc(ESC_CHAR)); printf(" %-12s Send file\n", buf);
	sprintf(buf, "%s<!cmd", unesc(ESC_CHAR)); printf(" %-12s Send command output\n", buf);
	sprintf(buf, "%s>file", unesc(ESC_CHAR)); printf(" %-12s Log to file\n", buf);
	sprintf(buf, "%s>", unesc(ESC_CHAR)); printf(" %-12s Close log file\n", buf);
	sprintf(buf, "%sm", unesc(ESC_CHAR)); printf(" %-12s Modify options (such as baud rate)\n", buf);
	sprintf(buf, "%so", unesc(ESC_CHAR)); printf(" %-12s Show current option values\n", buf);
	sprintf(buf, "%sw", unesc(ESC_CHAR)); printf(" %-12s Save current options as defaults\n", buf);
	sprintf(buf, "%ss", unesc(ESC_CHAR)); printf(" %-12s Print status information\n", buf);
	sprintf(buf, "%sx", unesc(ESC_CHAR)); printf(" %-12s Toggle hex-dump mode\n", buf);
	sprintf(buf, "%sa", unesc(ESC_CHAR)); printf(" %-12s Toggle no-ansi mode\n", buf);
}

void show_help()
{
	printf("Usage: %s [PORT] [OPTION]...\n", pgm_name);
	printf("Connect to a serial port\n");
	printf("\n");
	printf("PORT: Path to serial device.  For example /dev/ttyS0 or /dev/ttyUSB1\n");
	printf("\n");
	printf("OPTIONs:\n");
	show_valid_opts_cmd();
	printf("\n");
	// printf("After connection is established:\n");
	// show_esc_help();
}

/* Process data received from remote tty */

int remote_state = 0;

void remote_recv(char *buf, int len)
{
	char obuf[1024*6];
	unsigned optr = 0;
	while (len--) {
		int c = *(unsigned char *)buf++;
		switch (remote_state) {
			case 0: {
				if (hex_dump) {
					if (c < 32 || c > 126)
						obuf[optr++] = '.';
					else
						obuf[optr++] = c;
					obuf[optr++] = '<';
					obuf[optr++] = "0123456789ABCDEF"[c >> 4];
					obuf[optr++] = "0123456789ABCDEF"[c & 0xF];
					obuf[optr++] = '>';
					obuf[optr++] = ' ';
				} else if (no_ansi && c == 033) {
					// Skip until we get a letter
					remote_state = 1;
				} else if (no_ansi && !(c == 8 || c == 9 || c == 10 || c >= 32)) {
				} else {
					obuf[optr++] = c;
				}
				break;
			} case 1: {
				if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
					remote_state = 0;
				}
				break;
			}
		}
	}
	if (optr)
		write(local_fd, obuf, optr);
	if (log_file) {
		fwrite(obuf,optr,1,log_file);
		fflush(log_file);
	}
}

/* Process data received from local tty */

int local_state = 0;

void local_recv(char *buf, int len)
{
	char obuf[1024];
	unsigned optr = 0;
	while (len--) {
		int c = *(unsigned char *)buf++;
		/* printf("%x\n", c); */
		switch (local_state) {
			case 0: { /* At beginning of line */
				if (c == ESC_CHAR) {
					local_state = 1;
				} else if (c == BRK_CHAR) {
					if (!tcsendbreak(remote_fd, 0)) {
						printf("<Break sent>"); fflush(stdout);
					} else {
						printf("<Error sending break>"); fflush(stdout);
					}
					local_state = 0;
				} else if (c == 13) {
					obuf[optr++] = c;
				} else {
					obuf[optr++] = c;
					local_state = 2;
				}
				break;
			} case 2: { /* Not at beginning of line */
				if (c == ESC_CHAR && ESC_ANYWHERE) {
					local_state = 1;
				} else if (c == BRK_CHAR) {
					if (!tcsendbreak(remote_fd, 0)) {
						printf("<Break sent>"); fflush(stdout);
					} else {
						printf("<Error sending break>"); fflush(stdout);
					}
					local_state = 0;
				} else if (c == 13) {
					obuf[optr++] = c;
					local_state = 0;
				} else {
					obuf[optr++] = c;
					local_state = 2;
				}
				break;
			} case 1: {
				if (c == '.' || c == 'e' || c == 'q') { // Exit
					if (optr)
						write(remote_fd, obuf, optr);
					printf("\r\nConnection to %s closed.\r\n", port);
					restore_termios();
					removelock();
					exit(0);
				} else if (c == 'a') {
					no_ansi = !no_ansi;
					local_state = 0;
				} else if (c == 'x') {
					hex_dump = !hex_dump;
					local_state = 0;
				} else if (c == 'h' || c == '?') {
					restore_termios();
					printf("\r\n");
					show_esc_help();
					printf("\n");
					cu_termios();
					local_state = 0;
				} else if (c == 'f') { // Turn machine off
					char *off_str;
					restore_termios();
					printf("\r\nTurning machine off...\r\n");
					if ((off_str = get_opt(opts, "off"))) {
						printf("Executing: %s\n", off_str);
						system(off_str);
						sleep(1);
						printf("\r\nDone\r\n");
					} else {
						printf("Power off command not defined\n");
					}
					cu_termios();
					local_state = 0;
				} else if (c == 'n') { // Turn machine on
					char *on_str;
					restore_termios();
					printf("\r\nTurning machine on...\r\n");
					if ((on_str = get_opt(opts, "on"))) {
						printf("Executing: %s\n", on_str);
						system(on_str);
						sleep(1);
						printf("\r\nDone\r\n");
					} else {
						printf("Power on command not defined\n");
					}
					cu_termios();
					local_state = 0;
				} else if (c == 'm') {
					char name[100];
					char val[100];
					restore_termios();
					fcntl(local_fd, F_SETFL, O_SYNC);
					printf("\r\n");
					printf("Option to modify (enter for list): "); jgets(name, sizeof(name));
					if (check_opt(name)) {
						printf("Value to set it to (enter for list): "); jgets(val, sizeof(val));
						if (check_val(name, val)) {
							opts = set_opt(opts, name, val);
						} else {
							printf("Invalid value\n");
							printf("It must be one of:"); show_valid_vals(name);
						}
					} else {
						printf("Invalid option\n");
						printf("It must be one of:\n");
						show_valid_opts();
					}
					cu_termios();
					fcntl(local_fd, F_SETFL, O_NONBLOCK);
					symctrl(remote_fd);
					local_state = 0;
				} else if (c == 's') {
					int statv;
					/* Print status information */
					restore_termios();
					fcntl(local_fd, F_SETFL, O_SYNC);
					ioctl(remote_fd, TIOCMGET, &statv);
					printf("\r\n");
					printf("%s BAUD=%s outRTS=%d inCTS=%d outDTR=%d inDCD=%d inDSR=%d inRI=%d\r\n",
						port,
						baud_string(get_baud(remote_fd)),
						!!(statv & TIOCM_RTS), !!(statv & TIOCM_CTS),
						!!(statv & TIOCM_DTR), !!(statv & TIOCM_CD), 
						!!(statv & TIOCM_DSR), !!(statv & TIOCM_RI));
					printf("DB9 Male IBM PC Pins 1=DCD, 2=RD, 3=TD, 4=DTR, 5=GND, 6=DSR, 7=RTS, 8=CTS, 9=RI\n");
					cu_termios();
					local_state = 0;
				} else if (c == 'w') {
					/* Write current options to file */
					restore_termios();
					printf("\r\n");
					save_opts();
					cu_termios();
					local_state = 0;
				} else if (c == 'o') {
					/* Show options  */
					restore_termios();
					printf("\r\n");
					show_opts(stdout, opts);
					cu_termios();
					local_state = 0;
				} else if (c == 'b') { // Send break
//					if (!ioctl(remote_fd, TCSBRK, 0)) {
					if (!tcsendbreak(remote_fd, 0)) {
						printf("<Break sent>"); fflush(stdout);
					} else {
						printf("<Error sending break>"); fflush(stdout);
					}
					local_state = 0;
				} else if (c == '!') { // Execute shell command
					char buft[1024];
					restore_termios();
					fcntl(local_fd, F_SETFL, O_SYNC);
					printf("%s!", unesc(ESC_CHAR)); fflush(stdout);
					jgets(buft, sizeof(buft));
					system(buft);
					cu_termios();
					fcntl(local_fd, F_SETFL, O_NONBLOCK);
					local_state = 0;
				} else if (c == '<') { // Send contents of file or output of shell command to remote system
					char buft[1024];
					int lent;
					int flg;
					restore_termios();
					fcntl(local_fd, F_SETFL, O_SYNC);
					printf("%s<", unesc(ESC_CHAR)); fflush(stdout);
					jgets(buft, sizeof(buft));
					if (buft[0]=='!') {
						send_file = popen(buft + 1, "r");
					} else {
						send_file = fopen(buft, "r");
					}
					if (!send_file) {
						printf("Couldn't open %s\n", buft);
					} else {
						strcpy(send_file_name, buft);
					}
					cu_termios();
					fcntl(local_fd, F_SETFL, O_NONBLOCK);
					flg = 0;
					while (flg < 10) {
						while ((lent = read(fileno(send_file), buft, 100)) > 0) {
							int rtn;
							int n, a;
							a = 0;
							flg = 0;
							while (lent > 0) {
								n = write(remote_fd, buft + a, (unsigned)lent);
								if (n > 0) {
									a += n;
									lent -= n;
								}
							}
							// Read otherwise we might dead-lock
							while ((rtn = read(remote_fd, buft, 1024)) > 0) {
								remote_recv(buft, rtn);
							}
						}
						++flg;
					}
					fclose(send_file);
					local_state = 0;
				} else if (c == '>') { // Record everything from remote in a log file
					char buft[1024];
					restore_termios();
					fcntl(local_fd, F_SETFL, O_SYNC);
					printf("%s>", unesc(ESC_CHAR)); fflush(stdout);
					jgets(buft, sizeof(buft));
					if (log_file) {
						printf("Closing log file %s\n", log_file_name);
						fclose(log_file);
						log_file = 0;
					}
					if (buft[0]) {
						log_file = fopen(buft, "w");
						if (!log_file) {
							printf("Couldn't open %s\n", buft);
						} else {
							strcpy(log_file_name, buft);
							printf("Logging to %s\n", log_file_name);
						}
					}
					cu_termios();
					fcntl(local_fd, F_SETFL, O_NONBLOCK);
					local_state = 0;
				} else if (c == ESC_CHAR && ESC_DOUBLE) {
					obuf[optr++] = ESC_CHAR;
					local_state = 2;
				} else {
					obuf[optr++] = ESC_CHAR;
					obuf[optr++] = c;
					if (c == 13)
						local_state = 0;
					else
						local_state = 2;
				}
				break;
			}
		}
	}
	if (optr)
		write(remote_fd, obuf, optr);
}

const char *remote_host_name;

/* This should check /var/lock... */

char username[100];

struct passwd *pw;

int trylock(int is_tmp)
{
	int fd;
	char lockname[100];
	sprintf(lockname, "/tmp/lock..%s",sanitize(port));
	fd = open(lockname, O_EXCL|O_CREAT|O_WRONLY, 0);
	if (fd != -1) {
		FILE *f = fdopen(fd, "w");
#ifdef ASKNAME
		if (!is_tmp && pw && (!strcmp(pw->pw_name, "root") || !strcmp(pw->pw_name, "nobody") || !strcmp(pw->pw_name, "bin"))) {
			printf("\nUser name (for lock notice)> ");
			jgets(username, sizeof(username));
		} else {
			strcpy(username, pw->pw_name);
		}
#else
		strcpy(username, pw->pw_name);
#endif
		fprintf(f, "host=%s pid=%d user=%s\n", remote_host_name, getpid(), username);
		fclose(f);
		return 0;
	} else
		return -1;
}

void getlock()
{
	while (trylock(0)) {
		char lockname[100];
		char locked_by[1000];
		char locked_user[1000];
		int locked_pid;
		char buf[100];
		FILE *f;
		sprintf(lockname, "/tmp/lock..%s", sanitize(port));
		f = fopen(lockname, "r");
		if (!f) {
			printf("Couldn't open lock?  Try again..\n");
			exit(0);
		}
		strcpy(locked_by, "unknown");
		locked_pid = -1;
		strcpy(locked_user, "unknown");
		fscanf(f, "host=%s pid=%d user=%s", locked_by, &locked_pid, locked_user);
		fclose(f);
		if (locked_pid != -1 && !kill(locked_pid, 0)) {
			printf("\n%s in use by %s@%s (pid=%d).  Steal it (y,n)? ", port, locked_user, locked_by, locked_pid);
			jgets(buf, sizeof(buf));
			if (buf[0] == 'y') {
				kill(locked_pid, 9);
				unlink(lockname);
				sleep(1);
			} else
				exit(0);
		} else {
			/* Already gone */
			unlink(lockname);
		}
	}
}

void tmplock()
{
	int count = 0;
	while (trylock(1)) {
		char lockname[100];
		char locked_by[1000];
		char locked_user[1000];
		int locked_pid;
		FILE *f;
		sprintf(lockname, "/tmp/lock..%s", sanitize(port));
		f = fopen(lockname, "r");
		if (!f) {
			printf("Couldn't open lock?  Try again..\n");
			exit(0);
		}
		strcpy(locked_by, "unknown");
		locked_pid = -1;
		strcpy(locked_user, "unknown");
		fscanf(f, "host=%s pid=%d user=%s", locked_by, &locked_pid, locked_user);
		fclose(f);
		if (count == 3) {
			printf("Waited 3 times, so just take it\n");
			if (locked_pid != -1 && !kill(locked_pid, 0)) {
				printf("\n%s in use by %s@%s.  Taking... (killing pid %d)\n", port, locked_user, locked_by, locked_pid);
				kill(locked_pid, 9);
				unlink(lockname);
				sleep(1); // Need to wait?
			} else {
				/* Already gone */
				unlink(lockname);
			}
			count = 0;
		} else {
			printf("Someone has lock, wait a sec...\n");
			sleep(1);
			++count;
		}
	}
}

int main(int argc, char **argv)
{
	char *send_string = 0;
	int rtn;
	int parse_error = 0;
	local_fd = fileno(stdin);

	pw = getpwent(); // Multiple calls to this return different values: first root, next daemon...

	remote_host_name = getenv("HOSTNAME");
	if (!remote_host_name)
		remote_host_name = "localhost";

	opts = proc_opts(argv);

	if (get_opt(opts, "h"))
		remote_host_name = get_opt(opts, "h");

	send_string = get_opt(opts, "send");

	if (get_opt(opts, "noansi"))
		no_ansi = 1;

	if (get_opt(opts, "hexdump"))
		hex_dump = 1;

	if (get_opt(opts, "help")) {
		show_help();
		return 0;
	}

	if (!get_opt(opts, "baud")) {
		opts = set_opt(opts, "baud", "9600");
	}

	port = get_opt(opts, "port");
	if (!port) {
		/* parse_error = 1; */
		port = DEFAULT_PORT;
	}
	if (parse_error) {
		fprintf(stderr,"Syntax error\n");
		show_help();
		return -1;
	}
	if (send_string)
		tmplock();
	else
		getlock();
	remote_fd = symopen(port, 2);
	if (remote_fd < 0) {
		fprintf(stderr, "Couldn't open remote port\n");
		return -1;
	}
	printf("\n");
	if (ESC_CHAR == '~')
		printf("Hello %s.  Type ~h for help (~~h if connecting through ssh).\n", username, unesc(ESC_CHAR));
	else
		printf("Hello %s.  Type %sh for help.\n", username, unesc(ESC_CHAR));
	fcntl(remote_fd, F_SETFL, O_NONBLOCK);
	fcntl(local_fd, F_SETFL, O_NONBLOCK);
	save_termios();
	cu_termios();
	printf("\nConnected to %s at %s baud\r\n", port, baud_string(get_baud(remote_fd)));
	if (send_string) {
		write(remote_fd, send_string, strlen(send_string));
		restore_termios();
		removelock();
		exit(0);
	}
	for (;;) {
		char buf[1024];
		struct pollfd pollfds[2];
		pollfds[0].fd = remote_fd;
		pollfds[0].events = POLLIN;
		pollfds[0].revents = 0;
		pollfds[1].fd = local_fd;
		pollfds[1].events = POLLIN;
		pollfds[1].revents = 0;
		rtn = poll(pollfds, 2, 100);
		if (rtn < 0 && errno != EINTR) {
			perror("poll");
			restore_termios();
			removelock();
			exit(1);
		}
		rtn = read(remote_fd, buf, 1024);
		if (rtn > 0) {
			remote_recv(buf, rtn);
		}
		rtn = read(local_fd, buf, 1024);
		if (rtn > 0) {
			local_recv(buf, rtn);
		} else if (rtn == 0) {
			restore_termios();
			break;
		}
	}
	return 0;
}
