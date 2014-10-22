## Notif

This is Notif- an object oriented widget library for the X Window System.

## Object Oriented C

Preprocessor tricks are used to implement single inheritance in plain C.

Each object is composed of two structures.  One is the variable,
per-instance structure.  Each instance of the same class has one of these
structures, and it is allocated with malloc().  When we refer to an object,
we usually mean this structure's address.  One of the members of this
structure (called 'func'), points to the other structure- the per-class
structure.  Each class has one of these structures, usually allocated as
static data (I.E., as a global variable: malloc is not used).  Each object
of a class points to that class's per-class structure.  The per-object
structure contains data which may be different for each object, such as
screen x/y position.  The per-class structure contains data which depends
only on class, such as event handler function pointers and other so-called
member functions.

Member functions could be called with C pointer operators as follows
(supposing x is a pointer to an object):

	x->funcs->rm(x);	/* Delete object */

Note that the first argument for every member function is always the
object's address (it's equivalent to the C++ 'this' pointer).  It would
quickly become very tiring to have to use so many pointer operators every
time we wanted to call a member function, so a shorthand is called for.  C++
provides a convenient shorthand for calling member functions:

	x->rm();

This will not work in C, so instead we define a "polymorphic
function dispatch macro" for every member function (except event handlers,
which are not typically called directly by the programmer).  These macros
are all defined in notif.h.  The one for 'rm' is as follows:

	#define rm(x) ((x)->funcs->rm(x))

These macros make calling a member function look like calling a
any normal function:

	rm(x);			/* Delete object */


Incidentally, there is always a member function called 'rm' defined
for each class for the purpose of disposing of objects of that class.

The per-object and per-class structures are not defined directly. 
Instead, macros are used to defined the contents of these structures, and
the structures are defined by refering to these macros.  For example, these
macros define the class Foo:

``` C
	typedef struct foovars Foo;

        /* Per class structure members */
	#define FOOFUNCS \
	  void (*rm)(); \
	  void (*st)(); \
	  void (*gt)();

	/* Per object structure member */
	#define FOOVARS \
	  int x; \
	  int y; \

	extern struct foofuncs { FOOFUNCS } foofuncs;
						/* per-class structure */

	struct foovars { struct foofuncs *funcs; FOOVARS };
						/* per-object structure */

```

The purpose of defining these structures in this unusual way is to
provide a simple single-inheritance scheme.  Lets say we want to define a
class Bar which is to inherit Foo.  Bar must have all the same structure
members as Foo in the same places, so that a Bar object can pass for a Foo
object to code which doesn't know otherwise.  This is simply done by naming
the Foo macro as the first element of each of the Bar macros:

``` C
	typedef struct barbars Bar;

	#define BARFUNCS \
	  FOOFUNCS \
	  void (*clr)();

	#define BARVARS \
	  FOOVARS \
	  int z;

	extern struct barfuncs { BARFUNCS } barfuncs;
						/* per-class structure */

	struct barvars { struct barfuncs *funcs; BARVARS };
						/* per-object structure */
```

Note that the only per-object member which does not go in the VARS
macro is the 'funcs' pointer.  We always want the 'funcs' pointer's type to
match the per-class structure so that we can see all of the member functions
through this pointer without the need for any casting.

Each class has a "constructor" function for creating an object of
that class.  Because a class might want to call the constructor function of
a class which it inherits, the constructor itself can not do the actual
malloc for the object (the constructor from the inherited class would not
allocate enough space for the inheriting class).  The programmer must
therefore call malloc:

``` C
	Bar *x=mkBar(malloc(sizeof(Bar)));
```

This can get tedious, so a macro in notif.h is provided to shorten it:

``` C
	Bar *x=mk(Bar);		/* Construct a Bar object */
```

The constructor function is a good place to initialize the per-class
structure for that class.  For example, the constructor for Bar might look
like this:

``` C
	struct barfuncs barfuncs;	/* The Per-class structure */
	
	static bargt(Bar *x)		/* Member functions */
	{
	}

        static barclr(Bar *x)
	{
	}

	/* Minimal constructor function */
	Bar *mkBar(Bar *x)
	{
		mkFoo(x);		/* Initialize inherited per-object variables */
		if(!barfuncs.rm) {	/* Init. per-class variables if not done */
			memcpy(&barfuncs,&foofuncs,sizeof(struct foofuncs)); /* Inherit Foo's per-class members by copying */
			barfuncs.clr=barclr;	/* Initialize our new member */
			barfuncs.gt=bargt;	/* Change inherited member to one of our own */
		}
		x->funcs= &barfuncs;	/* Change funcs ptr to our per-class struct */
		x->z;			/* Initialize our per-object variables */
		return x;	/* Return the object */
	}
```

One final note about object oriented C:  sometimes it becomes
necessary to call an inherited class's member functions, even though they
have been replaced.  For example, the 'gt' member function was replaced in
Bar above.  If you want to call the original 'gt' member function from Foo
on an object of class Bar, you can not use the polymorphic function dispatch
macro:

``` C
	gt((Foo *)x);		/* Does not work! */
```

Instead you must refer to Foo's per-class structure directly and use
parenthasis to prevent invokation of the dispatch macro:

``` C
	(foofuncs.gt)(x);	/* This works */
```

This trick is most often used in replacement member functions such
as 'bargt' in Bar.  Perhaps 'bargt' wants the same behaviour as 'foogt', but
with only a minor change at the beginning or end.  It could do this by
calling foogt:

``` C
	static bargt(Bar *x)
	{
		(foofuncs.gt)(x);	/* Same as foogt */
		++x->z;			/* Plus this */
	}
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
		Text *txt;
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

## Widgets

Widgets correspond to 1 - 3 actual X Windows.  A widget is always composed
of a primary drawing window, but it may also be composed of an optional
border window (if stborder() is set) and a scroll-mask window (if stscroll()
has been called).  The coordinates of a widget (controlled with stx() and
sty()) are relative to the top left corner of the outermost window composing
the widget.  Coordinates for drawing in a widget are always relative to the
top left corner of the drawing window, so you never have to compensate for
the existence of borders.

Any widget can be made to be scrollable by calling stscroll().  When this is
done, the drawing window size (that returned by gtwidth() and gtheight())
will no longer be the same as the inside widget size (that returned by
gtiw() and gtih()) and the drawing window can be scrolled with stlofst() and
sttofst().  Stscroll() creates a scroll mask window which has the inside
widget size.

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

## Widget states

A widget can be in one of two primary states.  Widgets which are on
have X windows which are mapped onto the display and will receive events. 
Widgets which are off have no X windows and receive no events.  Dangling
widgets- those which are not part of the window hierarchy (for example, a
new widget which has not been added to anything yet) are always off.

All widgets maintain a list of widgets which they contain (the widget's
kids).  If a widget is added to this list while the widget is on, the added
widget will be turned on as well (if it is enabled(), which is the default
for new widgets).  When a widget is turned on, all of its enabled kids will
be turned on as well.

## Child positions

Usually a widget's position within another is either explicity given (with
stx() and sty()), or the placement of the widget is controled by the
parent's placement manager.  Manual placement information has precidence
over the placement manager- I.E., the placement manager will not position
the widget if it stx() or sty() have been called before the widget is added. 
The placement manager works on the x and y coordinates independantly, so you
could set a window's x position explicity, but leave the y coordinate unset
for the placement manager to handle.

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

## Main widgets

Widgets which are added to the root widget are special- they are the main
widget of the application and communicate setup information with the window
manager.  sttitle() and the window manager hint functions are valid for
these windows.  Sttitle() and the hint function must be called before the
widget is added to the root widget.

The root widget matches the screen size, so gtw() and gth() may be used on
it to get the screen size.

## Widget class hierarchy

The base class for all Notif objects is Lith.  Both the Widget and
the Wind class inherit Lith.  Anything which inherits Lith can receive
events, contain other objects, have a placement manager for controlling the
positioning of other objects and have a screen position and size.  See
widget.man and lith.h for a fuller description of these properties.

Notif programs create containment hierarchies of Widgets which
correspond to X windows displayed on the screen.  For example, the main
application widget will contain menu bar widgets, and menu bar widgets
contain menu entry widgets and so on.  Now widgets in Notif (see widget.man)
are themselves composed of 1-3 Winds (Winds are Liths with an associated X
window).  A typical stand alone button widget is composed of a Wind for
drawing the contents of the button, and a border Wind for pretty shadow
effects.  Each Wind corresponds to an actual X window (each has a Window
ID), and since Wind inherits Lith, they can contain other Winds.  Thus in
addition to the apparent hierarchy of Widgets, there is also a hidden
parallel hierarchy of Winds.  This is important to understand when you are
trying to search the hierarchy for a matching Window ID.  The Wind hierachy
must be searched, since that is what directly corresponds to actual X
windows.  Once you have the Wind, you can usually find the corresponding
Widget by looking at the 'target' member of the Wind's per-object structure.

## Event Delivery

ws() flushes output and then if there are any pending X events, delivers
them to Notif widgets.  For the simplest case (for an Expose event for
example), the process works as follows.  The X-event to be delivered is
placed in the global variable 'ev'.  ws() then looks up the corresponding
Wind object for the Window number given in 'ev' (Notif maintains an XContext
database of Window to Wind equivalents).  Next, ws() checks the Wind
object's target variable.  If it is set and there is no event handler
defined (if the event function pointer is NULL) for the event in Wind, the
event will be delivered to the target Lith instead of the actual Wind.  This
is used to redirect events from the componant Winds which make up a widget
to the widget itself.  Remember that this only happens if the Wind did
define an event handler for the particular event being delivered: for
example Shadow border objects do define an Expose event handler, so Expose
events for the Border will actually be delivered to the Border, not the
Widget.  Finally, the event handler is called as follows:

``` C
	int handler(Lith *x,XEvent *ev);
```

Where:
* x is the object receiving the event (if the event was redirected from a  Wind to a widget, x will be the widget's address).
* ev is the address of XEvent (usually the address of the global variable 'ev').

The per-class structure for Widgets and Winds contains a function
pointer entry for each X Event.  The names for these functions are simply
the X name for the event in all lower case.  So the Expose event is called
'expose' in the per-class structure.

If you create your own widget and want to be able to capture a
particular X event, you must inherit the standard Widget and set the
corresponding function pointer entry in the new per-class structure from
within the constructor function to point to your event handler.

## Userevent

The default event handler for Widget for the ButtonPress,
ButtonRelease, MotionNotify and KeyPress events is userevent().  This
function translates these events into calls to the member function 'user'
and basically makes button presses have the same format as keypresses.  In
addition, userevent provides for parents to intercept these events before
they are delivered to their kids.  Userevent will search all of the
destination Widget's parents, from root upward and call each non-NULL 'user'
function until one returns with a zero result.  Thus it is up to the parent
to decide if any particular event is to be delivered to a child.

``` C
	'user' is called as follows:

	int user(Widget *x,int key,int xbuttonstate,int x,int y,Time time,Widget *org);
```

Where:
* x is the widget receiving the event
* key is the key code.  mouse clicks and motion are translated into XK_Press1 - XK_Press3, XK_Release1 - XK_Release3 and XK_Motion.
* xbuttonstate contains the shift key status and mouse button status (I.E., MouseLeft, MouseMiddle, MouseRight, MouseShift, MouseCtrl, and MouseAlt).
* x, y and time give the position of the mouse click or keypress.
* org is the original destination of the event before it was intercepted by the parent.

The default function for 'user' provided by Widget is widgetuser().
This function provides basic focus handling (where the tab key switches
which widget receives key press events).  Also it sends the event to the
KMAP defined for the widget (see kbd.man).  If a key binding is detected in
the KMAP, the key function bound to that key binding will be called as
follows:

``` C
	int ufunction(int stat,Widget *z,void *arg,int key,int state,int x,int y,Widget *org);
```

Where:
* stat is zero.
* arg is the arg setting placed by kadd().
* key, state, x, y, and org are the same as above.
* z is the object containing the key binding.

See edit.c for example code on how to set up KMAP.  Normally this is the
final destination of the event, but the key function can optionally call
doevent() with either a simulated event or even the same event which was
delivered to the key function.  The 'root' argument for doevent() selects
how close to root userevent() will look for an intercepting parent.  This
can be useful for sending an event to a child which is normal intercepted by
the parent.

## Callbacks

The family of functions fnN() are used to create a callback structure TASK,
which contains a function to be called along with its arguments.  N can be 0 - 5,
and specifies how many arguments are passed to the function.  fnN() is
typically used to package up a callback function which is to be installed
into a widget.

The family of functions contN() are used to schedule the execution
of a callback function previously packaged up with fnN().  The function will
be called on the next return to the event loop.  contN() allows additional
arguments to be appended to the callback function's argument list.  contN()
is typically called by a widget event handler when a certain event occurs,
for example, the Edit widget will use contN() on an installed callback
function when the return key is pressed.

``` C
		TASK cb[1];
		fn0(cb,fred);		/* Install fred into cb */

		cont0(cb);		/* callback will be executed */
```

The callback function itself always looks like this:

``` C
		void func(stat,arg1,arg2,arg3,argA,argB,argC)
		{
			if(!stat) { /* Normal execution */
			} else { /* cancel() was called- free resources */
			}
		}
```

Where arg1..arg3 are those passed with fnN(), argA...argC are those
passed with contN(), and stat gives execution status.  stat will be zero if
the callback function is executed normally with contN().  stat will be
non-zero if the callback has been aborted with cancel().  cancel()
is typically used when a widget is deleted before the callback function is
normally executed.

Callback functions can only be executed one time.  The variable in
the widget containing the callback is automatically cleared by contN() to
ensure that the widget does not mistakenly execute a callback function more
than once.  If you want a callback function to be repeatedly executable
(I.E., so that every time you hit a button widget a function is called), you
must have the callback function reinstall itself (I.E., by calling fnN()
again and passing its result to the widget's installer function).

Callback functions always execute in their own stack.  The stack is
created when the callback function is executed, and it is deleted when the
function returns.  This creates a cooperative multi-threaded environment-
cooperative because thread switching only can occur during the return to the
event handler.  Locking of critical sections is usually never needed in a
cooperative multi-threading environment, but you must keep in mind that
variables can be changed by another thread whenever the event loop is
executed.

cfn() is used to package up the current thread for later
continuation.  It is used in conjunction with go() in a two-step process:

``` C
		int args[3];		/* Return arguments */
		TASK x[1];		/* Continuation TASK */
		Button *b=mk(Button);	/* Button widget */
			stfn(b,x=cfn(args));	/* Create TASK and install it */
		add(root,b);

		go(io,x);		/* Package up thread in x and return
					   to event loop.  Returns when
					   button is pressed. */
```


cfn() simply creates the TASK structure so that it can be installed
into a widget or otherwise be easily passed around.  go() fills in the
structure with the current stack pointer and program counter values and then
runs the event loop (which exists in its own stack).  When the return value
of cfn() is passed as an argument to contN() (usually by a widget), go()
will return and the thread can continue its execution (this switch happens
at the next return to the event loop).

The above continuation feature is useful for creating non-modal
dialog windows in a function call style.  The user calls a function which
places the dialog on the screen.  The function returns when the user clicks
"OK".  In the meantime other non-modal dialog windows can be created
(ultimately by other callbacks which are executed by other events).  The
dialog would be non-modal, and since each thread exists in its own stack,
one does not have to worry about which dialog is finished first.

The continuation feature is also useful for wholly internal scripts
or sequences which must wait for intervening event returns.  The advantage
here is purely that of code structuring: the event wait can be in the middle
of a for-loop and perhaps be several subroute levels deep.  There is no need
to save all the local variables in an object struture for an actual return
to the event loop, which is usual practice in many event-driven
environments.

go() with a NULL argument should be used in main() for the initial
execution of the event loop.  I.E., the last line of main() should be
go(NULL).  go() currently executes serv() (see serv.h) and ws() (see
event.man in Notif).
