/* Regular expression parser */

/* An NFA state */

struct state
{
	struct state *f;	/* Match (ch==-1, or ch>=0) or Alternative (ch is -2) */
	struct state *e;	/* Alternative (only when ch is -2) */
	int ch;			/* f: -1==done, -2==alternative, >=0 match */
	int flg;		/* Print flag */
};

/* Print NFA */
void show(struct state *st);

/* Parse regular expression into NFA */
struct state *parse(unsigned char *s);

/* Set of NFA states */
struct list
{
	struct list *next;
	struct state *st;
};

/* DFA */
struct dfa {
	struct dfa *next; /* List of all dfa states */
	struct list *nfa; /* NFA closure that this is equivalent to */
	struct dfa *eof;
	struct dfa *nxt[256];		/* Transition */
	int no;		/* State number for printing */
	int flg;
};

/* Convert NFA into DFA */
struct dfa *nfa_to_dfa(struct state *st);

/* Print DFA */
void show_dfa(struct dfa *dfa);
