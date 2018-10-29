struct button {
	char *name;		/* Name displayed in button */
	int x, y;		/* Position to display name string */
};

DSPOBJ *mkbutton(DSPOBJ *in, int x, int y, char *name, void (*press)(DSPOBJ *obj, XButtonEvent *ev));
