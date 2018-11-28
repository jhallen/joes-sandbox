# Object-Oriented Progamming

Besides being available for use by the programmer, Ivy's objects are used
internally for activation records.  This means that a function's local
variables are implemented as object members.

The only difference between regular objects and objects used for activation
records is the presence of a member called `mom.  This member refers to the
next outer scoping level.  Ivy uses lexical scoping, so the next outer level
is usually (for the case of nested functions) the parent function's (*not*
the calling function's) activation record, the global variables (when
modules are loaded, they each get their own object for global variables), or
finally the object containing Ivy's built-in functions, such as *print*. 
During symbol lookup, the chain of moms is searched for the symbol.

When functions are passed around, they always come in closures.  A closure
contains a pointer to the function's code and a pointer to the environment
where it was defined (the activation record at that time).  The environment
is the object that becomes the function's activation record's mom when the
function is called.

With this understanding, we can proceed towards implementing object-oriented
programming in Ivy.  There are two ways to do it: the direct method and the
closure method.  They are equivalent, and will be shown side by side.

First we need to define a class to hold member functions and static
variables (variables shared by all instances of the class).  In the direct
method, we just create an object, but with `mom set to the current
activation record, in this case the one containing the global variables:

~~~~
My_class=[`mom=this]
~~~~

The special symbol *this* always refers to the object being used as the
current activation record.

We can add a member function by assigning a lambda (nameless) function to a
member name (*show* in this case):

~~~~
My_class.show = fn((), {
	print x
})
~~~~

Or we can do this same thing by using dot notation in the function
declaration:

~~~~
fn My_class.show() {
	print x
}

fn My_class.increment() {
	x = x + 1
}
~~~~

Or we could even have included them when we created the object in the first
place:

~~~~
My_class = [
	`mom = this

	`show = fn((), {
		print x
	})

	`increment = fn((), {
		x = x + 1
	})
]
~~~~

In the closure method, we write a function which returns its activation
record.  This will be used as the class.  Any nested functions will become
member functions:

~~~~
fn create_My_class() {

	fn show() {
		print x
	}

	return this
}

My_class = create_My_class()
~~~~

The closure method has the advantage of not requiring explicit setting of *mom*,
in case that bothers you.  *My_class.mom* will still exist, however.  It was
set when then function was invoked.

We can add more member functions after the class has been created (by either
method: assigning lambda functions to member names or by declaring named
functions with the dot notation):

~~~~
fn My_class.increment() {
	x = x + 1
}
~~~~

Notice that member functions refer to instance variables as in C++ or Java. 
There is no need to prefix each instance variable with *self* or *this* as
in most languages with prototype based object systems.  On the other hand,
calls to sibling member functions should use dot notation:

~~~~
fn My_class.inc_and_show() {
	this.increment()
	this.show()
}
~~~~

We need a constructor to create class instances.  This constructor
should be a class member.  For the direct method, we write this:

~~~~
fn My_class.instance(i=[]) {
	i.x = 10
	i.mom = My_class
	return i
}
~~~~

Notice that we create the object for the instance as the default value for
*i*.  If *i* is missing, the object is automatically created.  If the
argument is provided, then the caller provided the object for the instance. 
We will use this later for derived classes, where we want to allow the
derived class constructor to call the base class constructor.

We create an instance variable x and set it to a default value 10.

We set the mom of the instance to the class so that if we call member
functions on the instance, the ones defined in the class will be found.

For the closure method, the instance creation function is a nested function
of *create_My_class* which returns its activation record as the instance. 
We separate out a construction function from the instance allocator so that
it may be later called by derived class constructors:

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

You might think some magic must be going on here, since member functions are
not in the instance objects, and even then you would expect the called
function's activation record's mom to be the class, not the instance.

The member functions are found because we explicitly set *i.mom* to
*My_class* in the constructor with the direct method or it was implicitly
set this way in the closure method.  In either case, the symbol lookup
follows the mom chain as usual.  It finds the closure for *show* or
*increment* with the recorded environment being the class object.

But the class object is not used for the member function's environment (and
here we come to the heart of Ivy's object system).  This is because the . 
operator replaces the environment part of the closure retrieved from the
symbol on its right side (*show* or *increment*) with the object it
began the symbol search in on its left side (*instance_1*), but only if
that object contains a mom.

[If the object did not contain a mom, then the environment replacement does
not happen.  Instead the recorded environment is used.  This allows you to
use non-class objects as simple containers for other object's member
functions:

~~~~
z=[]
z.show = instance_1.show
z.show()  --> prints 11
~~~~

The environement replacement is happening in the *instance_1.show* part of
the assignment above, so *instance_1* is the mom for *show*'s activation
record.  Since *z* does not have a mom, *z* is not used as the
environment when we finally call show in *z.show()*.]

The bottom line is that a function does not know that it is a member
function and certainly not which instance to operate on until it has been
accessed via the dot notation.  A non-obivious consequence of this is that
member functions must use dot notation when calling sibling member
functions, even though the dot notation is not required to find them.

For example, we could have a function which increments and shows.  We might
try writing it like this:

~~~~
fn My_class.inc_and_show() {
	increment()
	show()
}
~~~~

But it will not work.  The increment will look up *x* starting in the
environment where it was defined.  This is either in the global environment
for the direct method or in the class object for the closure method.  Either
way, it's not accessing the *x* in the instance.

The correct way to write this function is as follows:

~~~~
fn My_class.inc_and_show() {
	mom.increment()
	mom.show()
}
~~~~

*Mom* will refer to the instance object when inc_and_show is called.  In
this case, you could replace *mom* with *this*.  But it is better to use
*mom*, since the member function should never be modifying the activation
record of inc_and_show.

## Inheritance

We can create a new class based on an existing class like this:

~~~~
DerivedClass = [ `mom = MyClass ]
~~~~

Since *DerivedClass*'s mom is set to *MyClass*, symbol lookup for member
functions will find ones defined in *MyClass* if they are not directly
provided in *DerivedClass*.

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
the instance by adding a new instance variable *y*.  Naturally the instance's
mom is replaced (it had been set to *MyClass* by *MyClass*'s constructor) so
that it is set to *DerivedClass*.

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

*create_DerivedClass* is defined as a member of *MyClass* so that when
it's called, *create_DerivedClass*'s activation record's mom ends up being
*MyClass*.

An alternative way of defining *create_DerivedClass* which does not
involve modifying *MyClass* at all is as follows:

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

Notice that we replaced *create_DerivedClass*'s activation record's mom
during execution to connect it with its base class.  Since Ivy is a late
binding language, this is perfectly legal to do.

In either case, the new construction function adds a new instance variable,
*y*, as in the direct method.  It also calls the base class constructor. 
Notice that we follow mom twice to find it.  Remember that the construction
function will have its own activation record when it's called, so one "mom."
is needed to traverse to *DerivedClass*.  The second "mom." traversed back
to *My_class*, which has the construct function we want to call.

Notice that we do not provide a new instance allocation function.  The one
in *My_class* does the right thing, so there is no need to replace it.  It
will find *DerivedClass*'s *construct* function.

Now we can create an instance of the derived class:

~~~~
derived_instance_1 = DerivedClass.instance()

derived_instance_1.show()  --> Prints:

Derived
10
20
~~~~

## Multiple Inheritance

Multiple inheritance is possible in Ivy.  First define some base
classes:

~~~~
First_class = [ `mom=this ]

fn First_class.construct(i=[]) {
        i.x = 10
        i.mom = First_class
        return i
}

fn First_class.first_show() {
        print "x = ", x
}

fn First_class.first_inc() {
        x = x + 1
}

Second_class = [ `mom=this ]

fn Second_class.construct(i=[]) {
        i.y = 20
        i.mom = Second_class
        return i
}

fn Second_class.second_show() {
        print "y = ", y
}

fn Second_class.second_inc() {
        y = y + 1
}
~~~~

For the derived class, we can not rely on the automatic scope traversal
mechanism for symbol lookup, since it does not handle branches.  In any case
it is often ambiguous- there could be member functions with the same name in
both base classes.

Instead, we must bring in all of the base class member functions manually. 
In this case, there are no name conflicts, so you can union the base classes
together:

~~~~
Derived_class = First_class | Second_class
Derived_class.mom = this
~~~~

Much more realistically you will have to fix conflicting names.  Suppose the
base classes both used *show* and *inc*:

~~~~
First_class = [ `mom=this ]

fn First_class.instance(i=[]) {
        i.x = 10
        i.mom = First_class
        return i
}

fn First_class.show() {
        print "x = ", x
}

fn First_class.inc() {
        x = x + 1
}

Second_class = [ `mom=this ]

fn Second_class.instance(i=[]) {
        i.y = 20
        i.mom = Second_class
        return i
}

fn Second_class.show() {
        print "y = ", y
}

fn Second_class.inc() {
        y = y + 1
}
~~~~

Then we might construct the derived class as follows:

~~~~
Derived_class = [
	`mom = this

	`first_show = First_class.show
	`second_show = Second_class.show
	`first_inc = First_class.inc
	`second_inc = Second_class.inc

	`instance = fn((i=[]) {
		First_class.instance(i)
		Second_class.instance(i)
		i.z = 30
		i.mom = Derived_class
		return i
	}

	`show = fn(() {
		mom.first_show
		mom.second_show
		print "z = ", z
	}

	`third_inc = fn(() {
		z = z + 1
	}
]
~~~~

We can try it as follows:

~~~~
inst = Derived_class.instance()
inst.first_inc()
inst.third_inc()
inst.show()  --> Prints

x = 11
y = 20
z = 31
~~~~

Here is the closure version:

~~~~
fn create_First_class() {

        fn construct(i) {
                i.x = 10
        }

        fn instance() {
                construct(i)
                return i
        }

        fn show() {
                print "x = ", x
        }

        fn inc() {
                x = x + 1
        }

        return this
}

First_class = create_First_class()

fn create_Second_class() {

        fn construct(i) {
                i.y = 20
        }

        fn instance() {
                construct(i)
                return i
        }

        fn show() {
                print "y = ", y
        }

        fn inc() {
                y = y + 1
        }

        return this
}

Second_class = create_Second_class()

fn create_Derived_class() {

        first_show = First_class.show
        second_show = Second_class.show
        first_inc = First_class.inc
        second_inc = Second_class.inc

        fn construct(i) {
                First_class.construct(i)
                Second_class.construct(i)
                i.z = 30
        }

        fn instance() {
                construct(this)
                return this
        }

        fn show() {
                mom.first_show()
                mom.second_show()
                print "z = ", z
        }

        fn third_inc() {
                z = z + 1
        }

        return this
}

Derived_class = create_Derived_class()
~~~~
