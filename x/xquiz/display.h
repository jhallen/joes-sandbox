/* Display objects */

struct dspobj
 {
 /* Object relation information */
 LINK link;			/* Doubly linked list of these things */
 LINK children;			/* Linked list of objects in this thing */
 DSPOBJ *in;			/* Object this one is in or 0 for top level */
 DSPOBJ *top;			/* Top-level object this is in.  If this is a
                                   top-level object, 'top' refers to itself */

 /* X window associated with this object */
 Window win;			/* Window this thing gets displayed in */
 GC gc;				/* Default graphics context */
 int width, height;		/* Size of this window */
 int x,y;			/* Position of this window relative to 'in' */

 /* Member functions */
 void (*press)(DSPOBJ *obj, XButtonEvent *ev);		/* Button press */
 void (*type)(DSPOBJ *obj, char c);		/* Accept a keyboard char (see 'keyobj') */
 void (*enter)(DSPOBJ *obj, XCrossingEvent *ev);		/* Mouse enter */
 void (*leave)(DSPOBJ *obj, XCrossingEvent *ev);		/* Mouse exit */
 void (*release)(DSPOBJ *obj, XButtonEvent *ev);		/* Button release */
 void (*move)(DSPOBJ *obj, XMotionEvent *ev);			/* Button motion */
 void (*show)(DSPOBJ *obj);			/* Display this object */
 void (*close)(DSPOBJ *obj);			/* Close this object - this handles whatever
                                   is in 'extend' */

 /* This is set if the size of the window changed.  I.E., so 'show' doesn't
  * have to figure this out itself
  */
 int resize;

 /* If 'trap' is set and if this is a top-level window, then keypress
  * and button press events which occur in the top-level window or in
  * any of its children go here instead.  This has higher precidence
  * than keyobj.
  */
 DSPOBJ *trap;
 /* If 'keyobj' is set and if this is a top-level window, then any keypresses
  * which occur in the top-level window or in any of its children go to the
  * 'type' function of the object this points to.  If 'keyobj' is clear,
  * keypresses go nowhere.
  */
 DSPOBJ *keyobj;

 /* Extension to object
  * This usually points to a structure containing the working variables for
  * whatever is in the window
  */
 void *extend;
 };

/* void dspopen(DSPOBJ *in,int x,int y,int width,int height);
 * Open a new window.  'in' gives the parent of this window.  If 'in' is zero,
 * a top-level window is created.  (x,y) give the position of the new window
 * relative to the top-left corner of 'in'.  (width,height) give the size of
 * the new window.
 *
 * 'enter', 'leave', 'press', 'release', 'move', 'type', 'trap', 'show',
 * 'close', 'resize', 'keyobj' and 'extend' all start out clear.  Whatever
 * function calls this must set these (except for 'resize') itself.
 */
DSPOBJ *dspopen();

/* void dspclose(DISPOBJ *dspobj);  Close a window and all of its children */
void dspclose();

/* DSPOBJ *dspfind(Window win);  Find the DSPOBJ for the given X-window */
DSPOBJ *dspfind();

/* void dspshw();  Redisplay all windows */
void dspshw();
