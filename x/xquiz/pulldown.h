struct pulldown {
	char *name;		/* Name of pulldown menu */
	int x, y;		/* Position to display name string */
	char **items;		/* Array of strings */
	int nitems;		/* Number of things in pulldown menu */
	int h;			/* Height of each item */
	int w;			/* Width of widest item */
	DSPOBJ *sub;		/* Set if submenu is on */
	int (*gtitems)(DSPOBJ *dspobj, char ***items, int *nitems);	/* Get items */
	int (*select)(DSPOBJ *dspobj, int n);	/* Function to execute when item is selected */
	int which;		/* Line which is currently highlighted */
};

DSPOBJ *mkpulldown(DSPOBJ *in, int x, int y, char *name, void (*gtitems)(DSPOBJ *obj, char ***items, int *nitems), void (*select)(DSPOBJ *obj, int n));
