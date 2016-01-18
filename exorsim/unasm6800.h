/* A known fact */

struct fact {
        struct fact *next;
	unsigned short addr;
	char *label;
	int type;
	int len;
	char *comment;
};

extern int targets[65536];
extern struct fact *facts[65536];

void unasm_line(unsigned char *mem, unsigned short *at_pc, char *outbuf, int *at_target, int flag);
int fdb_line(unsigned char *mem, unsigned short *at_pc, char *outbuf, int flag);

void parse_facts(FILE *f);
