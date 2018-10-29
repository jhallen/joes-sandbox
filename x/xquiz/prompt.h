struct prompt {
	char *prompt;		/* Prompt string */
	char *answer;		/* Answer string */
	int cursor;		/* Cursor offset in answer string */
	int y;			/* Position in box to display strings */
	void (*eachkey)(DSPOBJ *dspobj);
	void (*lastkey)(DSPOBJ *dspobj);
	DSPOBJ *keyobj;
	DSPOBJ *key;
};

DSPOBJ *mkprompt(DSPOBJ *in, int y, char *prmpt, void (*eachkey)(DSPOBJ *dspobj), void (*lastkey)(DSPOBJ *), DSPOBJ *kk);
void promptclr(DSPOBJ *dspobj);
DSPOBJ *mvprompt(DSPOBJ *dspobj, int y);
void promptforce(DSPOBJ *dspobj, char *s);

