This is Notif- an object oriented widget library for the X Window System.

Preprocessor tricks are used to implement single inheritance in plain C.  A
class called 'Name' which inherits from 'Inherit' is declared like this in a
header file name.h:

	typedef struct name Name;

	/* Member functions and class variables */
	#define NAMEFUNCS \
		INHERITFUNC /* Functions we inherit */ \
		void (*st)(); /* A function of our own */ \
		Lith *(*gt)(); /* A function of our own */ \

	/* Inherit and declare instance variables */
	#define NAMEVARS \
		INHERITVARS /* Ones we inherit */ \
		Widget *target; /* Ones of our own */ \
		GC bknd; \

	/* All member functions and class variables */
	extern struct namefuncs { NAMEFUNCS } namefuncs;

	/* The type of an instance: notice that it has a pointer to the member functions */
	struct namevars { struct namefuncs *funcs; NAMEVARS };

	/* A constructor */
	Name *mkName(Name *t);

The constructor has a specific sequence of work to do:

	Name *mkName(Name *t)
	{
		mkInherit(t);	/* Call constructor from inherited class */
		if (!namefuncs.rm) { /* Set up member functions */
			memcopy(&namefuncs, &inheritfuncs, sizeof(struct inheritfuncs));
			namefuncs.st = namest; /* Install our member functions */
			namefuncs.gt = namegt;
			namefuncs.rm = namerm; /* Override inherited member functions */
		}
		t->funcs = &namefuncs;	/* Point to member functions */
		t->target = 0;		/* Initialize our member variables */
		t->bknd = stdbknd;
		return t;
	}

Notice that the constructor does not allocate space for the object.  You can call it like this:

	Name *t = mkName(malloc(sizeof(Name)));

A helper macro simplies this:

	Name *t = mk(Name);

By convention, the destructor is called rm.

	t->rm(); /* Call destructor */
	free(t); /* Free instance */

A helper macro simplifies this:

	rm(t); /* Call destructor and free */

