## Noitf

This is Notif- an object oriented widget library for the X Window System.

## Object oriented C

Preprocessor tricks are used to implement single inheritance in plain C.  A
class called 'Name' which inherits from 'Inherit' is declared like this in a
header file name.h:

``` C

	/* Note case of every instance of 'name', 'Name' and 'NAME'.
	   Also, 'inherit', 'Inherit' and 'INHERIT'. */

	typedef struct name Name;

	/* Member functions and class variables */
	#define NAMEFUNCS \
		INHERITFUNCS /* Functions we inherit */ \
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
```

The constructor has a specific sequence of work to do:

``` C
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
```

Notice that the constructor does not allocate space for the object.  You can call it like this:

``` C
	Name *t = mkName(malloc(sizeof(Name)));
```

A helper macro simplifies this:

``` C
	Name *t = mk(Name);
```

By convention, the destructor is called rm.

``` C
	t->rm(); /* Call destructor */
	free(t); /* Free instance */
```

A helper macro simplifies this:

``` C
	rm(t); /* Call destructor and free */
```

## Hello world!

Here is a simple program which displays a button.  When you press the button, the program exits.

``` C
	#include "notif.h"

	void press()
	{
		exit(1);
	}

	int main()
	{
		Widget *mainw;
		Button *b;
		TASK task[1];

		izws(); /* Initialize windowing system */

		mainw = mk(Widget); /* Create main window */

		stwidth(mainw, 640); /* Set inside size of window */
		stheight(mainw, 480);

		b = mk(Button); /* Create a button */
			stw(b, 80); /* Set outside size of button */
			sth(b, 34);
			txt = mk(Text); /* Create a text box */
				st(txt, "Bye!");
			add(b, txt); /* Install it in the button */
			stfn(b, fn0(task, press)); /* Callback */
		add(mainw, b); /* Install button in main window */

		add(root, mainw); /* Add main window to desktop */

		go(flshio, NULL); /* Process events */
	}
```

## Placement manager

Each widget inherits the placement manager.  It controls where child widgets
added to the parent widget are placed.  Before adding a child widget, set
the placement mode for the next added width.  Placement modes are sticky, so
once you set "left to right" mode, each added widget will be placed next to
the previous one.

Unless a size has been set, the outer widget will be sized to hold its child
widgets.

Child widgets can be made to be sized to fit the parent by attaching two
anchor points in a particular dimension.  For example,

``` C
		stwidth(parent, 100);	/* Parent is 100 pixels wide */
		ledge(parent, 0);	/* Next child attached to left edge */
		auxredge(parent, 0);	/* Next child also attached to right edge */

		add(parent, child);	/* Child sized to fit in 100 pixel width */

```

	    Horizontal placement:

``` C
		void ltor(Widget *w,Widget *rel,int ofst);	/* Left to right */
		void ledge(Widget *w,int ofst);			/* Left edge */
		void lsame(Widget *w,Widget *rel,int ofst);	/* Left same */
		void rtol(Widget *w,Widget *rel,int ofst);	/* Right to left */
		void redge(Widget *w,int ofst);			/* Right edge */
		void rsame(Widget *w,Widget *rel,int ofst);	/* Right same */
		void hcenter(Widget *w);			/* Horz. center */
		void auxltor(Widget *w,Widget *rel,int ofst);	/* Left to right */
		void auxledge(Widget *w,int ofst);		/* Left edge */
		void auxlsame(Widget *w,Widget *rel,int ofst);	/* Left same */
		void auxrtol(Widget *w,Widget *rel,int ofst);	/* Right to left */
		void auxredge(Widget *w,int ofst);		/* Right edge */
		void auxrsame(Widget *w,Widget *rel,int ofst);	/* Right same */
```

	    Vertical placement:

``` C
		void ttob(Widget *w,Widget *rel,int ofst);	/* Top to bottom */
		void tedge(Widget *w,int ofst);			/* Top edge */
		void tsame(Widget *w,Widget *rel,int ofst);	/* Top same */
		void btot(Widget *w,Widget *rel,int ofst);	/* Bottom to top */
		void bedge(Widget *w,int ofst);			/* Bottom edge */
		void bsame(Widget *w,Widget *rel,int ofst);	/* Bottom same */
		void vcenter(Widget *w);			/* Vertical center */
		void auxttob(Widget *w,Widget *rel,int ofst);	/* Top to bottom */
		void auxtedge(Widget *w,int ofst);		/* Top edge */
		void auxtsame(Widget *w,Widget *rel,int ofst);	/* Top same */
		void auxbtot(Widget *w,Widget *rel,int ofst);	/* Bottom to top */
		void auxbedge(Widget *w,int ofst);		/* Bottom edge */
		void auxbsame(Widget *w,Widget *rel,int ofst);	/* Bottom same */
```

## Sizes

Generally a widget has several sizes:

``` C
		void stwidth(Widget *w, int width);	/* Drawing area width */
		int gtwidth(Widget *w);

		void stiw(Widget *w, int width);	/* Inside width */
		int gtiw(Widget *w);

		void stw(Widget *w, int width);		/* Outside width */
		int gtw(Widget *w);
```

The difference between the inside size and outside size is the space
reserved for the border.  The drawing area size can be larger than the inside
size.  In this case, the drawing area is usually scrollable.

