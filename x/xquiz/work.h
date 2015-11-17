DSPOBJ *mkwork(DSPOBJ *in, int xx, int y, int width, int height, LST *eqn);

extern LST **sel;

/* Undo list */

struct undo {
	struct undo *next;
	struct undo *prev;
	LST *eqn;
};

struct work {
	LST *eqn;		/* Current equation */
	LST *ent;
	int selx, sely;
	int cury;
	struct undo *undo;	/* Undo list */
	char *sname;
	SYM **vars;
	char **vnames;
	int nvars, which;
};
