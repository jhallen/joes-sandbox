# Object-Oriented Progamming

Besides being available for use by the programmer, Ivy's objects are used
internally for function execution contexts (also known as scoping levels or
environments).  This means that a function's local variables, including
nested functions are implemented as object members.

The only difference between regular objects and objects used for execution
contexts is the presence of a member called `mom.  This member refers to the
next outer scoping level.  During symbol lookup, the chain of moms is
searched for the symbol.  Always the outermost level is the object
containing Ivy's built-in functions, such as print.

With this understanding, we can proceed towards an understand of how
object-oriented programming is supported by Ivy.  There are two ways to
implement object-oriented programming in Ivy: the direct method and the
closure method.  They are equivalent, and will be shown side by side.

First we need to define a class to hold member functions and static
variables (variables shared by all instances of the class).  In the direct
method, we just create an object, but with `mom set to the current execution
environment:

~~~~
My_class=[`mom=this]
~~~~

The special symbol **this** always refers to the object being used as the
current execution environment.

We can add a member function by assigning a lambda (nameless) function:

~~~~
My_class.show = fn((), {
	print x
})
~~~~

Or by using the dot notation in the function declaration:

~~~~
fn My_class.show() {
	print x
}

fn My_class.increment() {
	x = x + 1
}
~~~~

In the closure method, we write a function which returns its execution
environment.  This will be used as the class.  Any nested functions will
become member functions:

~~~~
fn create_My_class() {

	fn show() {
		print x
	}

	fn inc() {
		x = x + 1
	}

	return this
}

My_class = create_My_class()
~~~~

Now we need a constructor to create class instances.  This constructor
should be a class member.  For the direct method, we add this:

~~~~
fn My_class.instance(i=[]) {
	i.x = 10
	i.mom = My_class
	return i
}
~~~~

Notice that we create the object for the instance as the default value for
i.  If i is missing, the object is automatically created.  If the argument
is provided, then the caller provided the object for the instance.  We will
use this later for derived classes, where we want to allow the derived
class constructor to call the base class constructor.

We create an instance variable x and set it to a default value 10.

We set the mom of the instance to the class so that if the instance is used
as an execution environment, then the called function will have access to
the class variables and other member functions.

For the closure method, the instance creation function is a nested function
of create_My_class which returns its execution environment as the instance. 
We provide a separate construct function so that it can be called by derived
class constructors.

~~~~
fn create_My_class() {

	fn construct(i) {
	        i.x = 10
	}

	fn instance() {
		construct(this)
		return this
	}

	fn show() {
		print x
	}

	fn inc() {
		x = x + 1
	}

	return this
}
~~~~

Now we create instances of the class:

~~~~
instance_1 = My_class.construct()
instance_2 = My_class.construct()
~~~~

The instance is now ready and we can call member functions:

~~~~
instance_1.show()   --> prints 10
instance_1.inc()
instance_1.show()   --> prints 11
instance_2.show()   --> prints 10
~~~~

Some magic must be going on here, since member functions are not in the
instance objects, and since you would expect the execution environment to be
the class, not the instance.

The member functions are found because we explicitly set i.mom to My_class
in the constructor with the direct method or it was implicitly set this way
in the closure method.  In either case, the symbol lookup follows the mom
chain as usual.  It finds the closure containing show or inc with the
recorded environment being the class object.

But the class object is not used for the execution environment.  The . 
operator replaces the environment part of the closure retrieved from the
symbol of its right side (show or inc) with the object it began the symbol
search with on the left side (instance_1), but only if that object contains
a mom.

If the object did not contain a mom, then the environment replacement does
not happen.  Instead the recorded environment is used.  This allows you to
make objects that just refere to other object's member functions:

~~~~
z=[]
z.show = instance_1.show
z.show()  --> prints 11
~~~~

Note that the environement replacement is happening in the instance_1.show
part of the assignment above, so instance_1 is still the execution
environment for show.  Since z does not have a mom, z is not used as the
environment when we finally call show.

## Inheritance

