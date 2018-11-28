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

With this understanding, we can proceed towards implementing object-oriented
programming in Ivy.  There are two ways to do it: the direct method and the
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

	return this
}

My_class = create_My_class()
~~~~

We can add more member functions after the class has been created:

~~~~
fn My_class.increment() {
	x = x + 1
}
~~~~

We need a constructor to create class instances.  This constructor
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
We separate out a construct function from the instance allocator so that it
may be later be called by derived class constructors:

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

	fn increment() {
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

The instances are now ready and we can call their member functions:

~~~~
instance_1.show()   --> prints 10
instance_1.increment()
instance_1.show()   --> prints 11
instance_2.show()   --> prints 10
~~~~

Some magic must be going on here, since member functions are not in the
instance objects, and since you would expect the execution environment to be
the class, not the instance.

The member functions are found because we explicitly set **i.mom** to
**My_class** in the constructor with the direct method or it was implicitly
set this way in the closure method.  In either case, the symbol lookup
follows the mom chain as usual.  It finds the closure containing **show** or
**increment** with the recorded environment being the class object.

But the class object is not used for the execution environment.  This is
because the . operator replaces the environment part of the closure
retrieved from the symbol of its right side (**show** or **increment**) with the
object it began the symbol search with on the left side (**instance_1**), but
only if that object contains a mom.

If the object did not contain a mom, then the environment replacement does
not happen.  Instead the recorded environment is used.  This allows you to
make non-class objects as containers for other object's member functions:

~~~~
z=[]
z.show = instance_1.show
z.show()  --> prints 11
~~~~

The environement replacement is happening in the **instance_1.show** part of
the assignment above, so **instance_1** is the execution environment
for **show**.  Since **z** does not have a mom, **z** is not used as the environment
when we finally call show.

## Inheritance

We can create a new class based on an existing class like this:

~~~~
DerivedClass = [ `mom = MyClass ]
~~~~

Since **DerivedClass**'s mom is set to **MyClass**, symbol lookup for member
functions will find ones defined in **MyClass** if they are not directly
provided in **DerivedClass**.

It will need a new instance constructor:

~~~~
fn DerivedClass.construct(i=[]) {
	i = MyClass.construct(i)
	i.y = 20
	i.mom = DerivedClass
	return i
}
~~~~

Notice how we are calling the base class's constructor, but then elaborating
the instance by adding a new instance variable **y**.  Naturally the instance's
mom is replaced (it had been set to **MyClass** by **MyClass**'s constructor) so
that it is set to **DerivedClass**.

And we will override one of the member functions:

~~~~
fn DerivedClass.show() {
	print "Derived"
	print x
	print y
}
~~~~

Using the closure method, we provide a new class creation function and then
call it:

~~~~
fn MyClass.create_DerivedClass() {

	fn construct(i) {
		mom.mom.construct(i)
		i.y = 20
	}

	fn show() {
		print "Derived"
		print x
		print y
	}

	return this
}

DerivedClass = MyClass.create_DerivedClass()
~~~~

**create_DerivedClass** is defined as a member of **MyClass** so that when
it's called, **create_DerivedClass**'s execution environment's mom ends up
being **MyClass**.  *create_DerivedClass**'s execution environment becomes
**DerivedClass**.

An alternative way of defining **create_DerivedClass** which does not
involve modifying MyClass at all is as follows:

~~~~
fn create_DerivedClass() {

	mom = MyClass

	fn construct(i) {
		mom.mom.construct(i)
		i.y = 20
	}

	fn show() {
		print "Derived"
		print x
		print y
	}

	return this
}

DerivedClass = create_DerivedClass()
~~~~

Notice that we replaced **create_DerivedClass**'s execution environment's
mom during execution to connect it with its base class.  Since Ivy is a late
binding language, this is perfectly legal to do.

In either case, the new construction function adds a new instance variable,
**y**, as in the direct method.  It also calls the base class constructor. 
Notice that we follow mom twice to find it.  Remember that the construction
function will have its own execution environment when it's called, so one
"mom." is needed to traverse to **DerivedClass**.  The second "mom."
traversed back to **MyClass**, which has the construct function we want to
call.

Notice that we do not provide a new instance allocation function.  The one
in **MyClass** does the right thing, so there is no need to replace it.  It
will find **DerivedClass**'s **construct** function.

Now we can create an instance of the derived class:

~~~~
derived_instance_1 = DerivedClass.instance()

derived_instance_1.show()  --> Prints:

Derived
10
20
~~~~
