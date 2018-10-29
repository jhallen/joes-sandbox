void graph();

struct graph {
	LST *eqn;		/* Equation to graph */
	double left, right, top, bottom;	/* Dimensions */
	double npoints;		/* Number of points to graph */
	SYM *xvar;		/* Horizontal variable */
	SYM **vars;		/* List of variables */
	double *vals;		/* Values of the variables */
	char **vnames;		/* Variable name vector */
	I nvars;		/* Number of variables */
	int line;		/* Set for line mode */
	int nn;
	int help;		/* Set if help is on */
};

void showgraph(DSPOBJ *dspobj);

DSPOBJ *mkgraph(DSPOBJ *in, int xx, int y, int width, int height, LST *eqn);

