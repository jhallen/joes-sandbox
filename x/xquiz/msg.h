DSPOBJ *mkmsg();
struct msg {
	char **items;
	char *text;
	int nitems;
	int h;			/* Height of each line */
	int x1;			/* Starting x position of message */
	int w2;			/* Width of each selection */
	int x2;			/* Starting x position of selections */
	void (*func)(int y);
};
