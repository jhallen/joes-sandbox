void graph();

struct graph {
	LST *eqn;		/* Equation to graph */
	double left, right, top, bottom;	/* Dimensions */
	double npoints;		/* Number of points to graph */
	SYM *xvar;		/* Horizontal variable */
	SYM **vars;		/* List of variables */
	double *vals;		/* Values of the variables */
	char **vnames;		/* Variable name vector */
	int nvars;		/* Number of variables */
	int line;		/* Set for line mode */
	int nn;
	int help;		/* Set if help is on */
};
