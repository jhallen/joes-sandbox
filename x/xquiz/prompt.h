struct prompt {
	char *prompt;		/* Prompt string */
	char *answer;		/* Answer string */
	int cursor;		/* Cursor offset in answer string */
	int y;			/* Position in box to display strings */
	int (*eachkey)(DSPOBJ *dspobj);
	int (*lastkey)(DSPOBJ *dspobj);
	DSPOBJ *keyobj;
	DSPOBJ *key;
};

DSPOBJ *mkprompt(DSPOBJ *in, int y, char *prmpt, int (*eachkey)(DSPOBJ *dspobj), int (*lastkey)(DSPOBJ *), DSPOBJ *kk);
void promptclr(DSPOBJ *dspobj);
DSPOBJ *mvprompt(DSPOBJ *dspobj, int y);
