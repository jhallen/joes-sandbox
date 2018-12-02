# Ivy

- [Introduction](#Introduction)
- [Invocation](#Invocation)
- [Syntax](#Syntax)
- [Variables](#Variables)
- [Values](#Values)
- [Expressions](#Expressions)
- [Operators](#Operators)
- [Functions](#Functions)
- [Statements](#Statements)
- [Intrinsic functions](#Intrinsics)
- [Object Oriented Programming](#object-oriented-programming)

## Introduction

Ivy is an extensible, dynamically typed, late binding language intended to
be used as an embedded command language.  It can also be used stand-alone:
it can execute script files from the command line or presents a
read-eval-print loop (REPL) to the user if no files are given.

Ivy's extensibility is based on the fact that statements are syntactically
identical to function calls.  Also blocks (surrounded by braces) may be used
as function arguments.  Thus, new user-defined statements can be added just
by defining functions.  Function arguments are packaged up as thunks and may
have their evaluation delayed and execution environment modified.  This
allows user defined functions to do many of the things that traditional
language statements can do.

A number of features make Ivy suitable as a command language.  Commands in
Ivy are just function calls, but with a convenient lightweight syntax. 
Also, Ivy supports named arguments, default argument values and variadic
functions.

Ivy source code is compiled to byte-code which is then interpreted.  Ivy's
compiler and interpreter are both event driven (meaning that they return to
the top level when more input is needed).  This allows Ivy to be easily
embedded into other programs.

Ivy uses garbage collection for memory management.

## Invocation

	ivy [-u] [-t] [-c] [filenames...]

		If no filenames are given, ivy takes input from the keyboard

		-t  is a debugging aid which displays the parse tree as
		    commands are entered

		-u  is a debugging aid which unassembles the byte-code as
		    it's made

		-c  calculator mode.  prints the result of each command/
		    expression which is immediately executed


For example, here is "hello world" in Ivy interactive mode:

	ivy
	->print "Hello, world"
	Hello, world
	->

When calculator mode is used (-c option), the value of each command is
printed:

	ivy -c
	->10;20
	10
	20
	->

## Syntax

### Comments

Ivy uses *#* to introduce comments.  Everything from an unquoted *#* to the
end of the line is a comment.  *#* was chosen so that the very first line in
an Ivy program can be *#!/usr/bin/ivy*, which makes the file into a script
in UNIX.

### Commands

There is a command format for function calls, where one or more commands
are provided on a single logical line:

	command arg arg arg ; command arg arg arg ; ...

Newlines are permitted within parenthesis, brackets and braces and in this
way commands may span multiple physical lines.

The value of a list of commands is the value returned by the last command.

Arguments may be separated with whitespace or commas.  Arguments are
expressions.

Commands are simple names or several names separated with periods (for
member selection).  If anything other than this is provided, the command and
its arguments are treated as a list of expressions and they are sequentially
evaluated (and the final value is the value of the last expression).

	expression expression expression ; command arg arg arg ; ...

Sometimes you may want to suppress the treatment of a simple name as a
command (for example, to return a function).  To do this, enclose it within
parenthesis:

	(name)

Note that if you try to call a non-function value with zero arguments, the
result is the value itself.  This is relevant when you use Ivy
interactively (when invoked with the -c option):

	->a=10
	10
	->a
	10

*a* alone on a line is treated as a command, and is called with zero
arguments, and returns itself.

### Blocks

Commands may appear at the top-level (non-enclosed) and within braces to
make a block.

	{ commands }

The entire block is treated as a single expression and may be used as an
argument for a command.

Block structured statements such as *if* are just function calls but with
blocks given as arguments.  The traditional *if...else if...else* statement
looks like this in Ivy:

	if a==1 {
		print "A is 1"
	} a==2 {
		print "A is 2"
	} a==3 {
		print "A is 3"
	} {
		print "A is something else"
	}

It is equivalent to calling *if* as a function like this:

	if(a==1, print("A is 1"), a==2, print("A is 2"), a==3, print("A is
	3"), print("A is something else"))

### Lists

A list of expressions is expected within parenthesis and brackets:

	(list)			Argument list, parenthetical expression,
				or formal args for function definition.

	[list]			An object.

A list is expected for function arguments and objects.  A list may also be
provided for simple parenthetical expressions.  In this case, the
expressions of the list are evaluated in turn from left to right, and the
last one provides the value of the parenthetical expression.

	func(2 3)		Call func with two args: 2 and 3

	3*(2 3)			Results in the value 9

Expressions within a list can be separated with whitespace, commas or
semicolons.  These are all identical in this context:

	func(10 20 40)		Call func with three args
	func(10,20;40)		Call func with three args
	func(10;,20,,,40)	Call func with three args

Note that successive separators with nothing between them are ignored:

	func(1,2) is exactly the same as func(1,,,,2)

Note however, that an empty set of parenthesis has meaning:

	func(() 20)	Call func with two args: void and 20.

Since whitespace may be used to separate expressions, ambiguities between
symbols which can be both infix and prefix operators result.  These
ambiguities are resolved by noting the typographical distance between the
symbol and its potential arguments.  The rule is that if the distance is
balanced, the operator is infix, otherwise it's prefix:

	a - b			One expression: subtract b from a
	a  -b			Two expressions: a and negated b
	f (7)			Two expressions: f and 7.
	f(7)			One expression: a call to f with arg 7
	f ( 7 )			One expression: a call to f with arg 7
	f  ( 7 )		Two expressions: f and 7.

Note that tabs occur every 8 spaces for this purpose.

## Variables

### Scoping rules

Variables set (assigned to) outside of functions are global variables.  They
will be visible in any function or scope.

If a variable is assigned to inside of a function and there is no global
variable of the same name, a local variable will be created within the
function.

The *var* statement can be used to force the creation of local variables,
even if there are global variables with the same name.

Functions have their own scope.  Local variables created within a function
are only visible within the function.  Nested functions can see their
parent's variables.  

Statements and blocks do not have their own scope.  The only exception is
the *scope* statement.  Local variables created in its body will be visible
only within its body.

### Assignment

Ivy supports nested multi-assignment via objects:

	[a,b,[x,y]] = [1,2,[9,10]]

Functions may return an object for multi-assignment:

	fn multi() [1 2 3]

	[a,b,c]=multi()

Ivy is unlike most languages in that it does not know that a variable will
be used as an L-value until very late (not until the assignment takes
place).  Therefore more things are legal L-values than you would expect. 
For example, if a function returns a variable, it may be assigned to:

	->x=1
	->fn rtnvar() x
	->print x
	1
	->rtnvar()=10
	->print x
	10

This works for multi-assignment also:

	# Select a set of variables to assign to
	fn rtn(n) {
		if n==1 {
			return [x,y,z]
		} {
			return [i,j,k]
	}
	# Make sure they exist
	x=y=z=0
	i=j=l=0
	# Assign
	rtn(1)=[1,2,3]
	# x, y and z have been assigned to.

Note that the variables must already exist for this to work, otherwise they
will be created as local variables within the function.  They will still be
assigned to, but it is probably not what you want.

## Values

### Objects

Objects are catch-all data structures which may be indexed by number, symbol
or string.  Objects indexed by number are similar to arrays.  Objects
indexed by symbol are similar to structures.  Objects indexed by string are
similar to hash tables.

A mixture of index types may be used on the same object, but always
refer to different locations (symbol foo and string "foo" can have different
values).

An automatic array is used for numbered indexing.  This array expands to
accommodate whatever index is used.  If you store a value at index 0 and
also at index 999, then space for 1000 values will be allocated.

When arrays are created all at once, the first item will be at index 0:

	->o=[10 20 30]
	->print o[0]
	10
	->print o[1]
	20
	->print o[2]
	30
	->

Auto-expanding hash tables are used for symbol and string indexing.  These
hash tables expand to twice their size when they reach half full.  Symbol
indexing is faster than string indexing since, for symbols, the hash value
is just the address of the symbol.

Objects can be created either member by member:

	o=[]
	o.a=5
	o.b=6
	o(1)=7
	o("foo")=8

or all at once:

	o=[`a=5, `b=6, `1=7, `"foo"=8]

Objects are assigned by reference.  This means that if you have an
object in one variable:

	x=[1 2 3]

And you assign it to another:

	y=x

And then change one of the members of x:

	x(0)=5

Then the change will appear both in x and in y.

### void

The special value *void* is returned when you attempt to look up an object
member, but it's missing.  *void* is equivalent to false or 0 when used with
*if*.

Here is an example:

	->a=[]
	->if a.b print("it exists")
	->a.b=1
	->if a.b print("it exists")
	it exists
	->

### this

*this* is a read-only symbol that returns the current activation record.  An
activation record is an Ivy object which is used to hold local variables.

	->a=10
	->b=20
	->print this
	[ 1 at 0x0x1390498
		`a=10
		`mom=[ 0 at 0x0x13904e0 (globals) ]
		`b=20
	]
	->print this.a
	10
	->

### mom

*mom* is an object member present in objects used as activation records.  It
refers to the object used as the next outer scope.  *mom* can be used to
access variables in the next outer scope:

	->x=10
	->fn foo() {
	->	var x = 20
	->	print x
	->	print mom.x
	->}
	->foo
	20
	10
	->

### Closures

When functions are treated as data, they are always passed around as
closures.  A closure has the address of the code for the function and a
reference to its environment- the object which was the activation record at
the time when the function was defined.  This object will become the next
outer scope of the function (it's activation record's mom) when the function
is called.

The environment part of a closure value can be replaced in various ways. 
There is an operator to do this directly:

	modified = original::new_environment

When a closure is retrieved from an object with a member called mom via dot
notation, the object will replace the environment part of the closure:

	->x=10
	->fn p() print(x)
	->a=[`mom=this, `x=20, `p=p]
	->p()
	10
	->a.p()
	20

### Symbols

There are symbols:

	`hello

Symbols can be tested for equality:

	`hello==`hello		# True
	`hello==`goodbye	# False

There is an interned string table for all symbols, so within Ivy, the check
for symbol equality is fast: symbols are identical if their addresses match.

Symbols can be used as object indices.  Variables within a scope are
indexed by symbols.  These are equivalent:

	a(`hello)=5
	a.hello=5

### Integers

Integers may be entered in a variety of bases:

	x=0o127		# Octal
	x=0x80		# Hexadecimal
	x=0b11		# Binary
	x=123		# Decimal

Octal, binary and hexadecimal digits may be interspersed with
underscores (_) for enhanced readability:

	x=0xADDED_FEE

Also the ASCII value of a character may be taken as an integer:

	x='A'		# The value 65

The following "escape sequences" may also be used in place of a
character:

	x='\n'		# New-line
	x='\r'		# Return
	x='\b'		# Backspace
	x='\t'		# Tab
	x='\a'		# Alert (bell)
	x='\e'		# Escape
	x='\f'		# Form-feed
	x='\^A'		# Ctrl-A (works for ^@ to ^_ and ^?)
	x='\010'	# Octal for 8
	x='\xFF'	# Hexadecimal for 255
	x='\\'		# \
	x='\q'		# q (undefined characters return themselves)

### Floating point numbers

	x=.125
	x=.125e3
	x=0.3

### Strings

String constants are enclosed in double-quotes:

	x="Hello there"

Escape sequences may also be used inside of strings.

## Expressions

An expression is a single or dual operand operator with arguments, a
constant, a variable, a function definition, a block enclosed within braces,
a function call, an object or parenthetical expression.  Examples of each of
these cases follow:

	~expr			Single operand

	expr+expr		Dual operand

	25			Constant

	(expressions)		Precedence
	{commands}

	expr(expr ...)		Function call

	[1 2 3 4]		An array object

	[`next=item `value=1]	A structure object

	(fn (x,y) x*y)(3,5)	Calling an anonymous function


## Operators

Here are the operators grouped from highest precedence to lowest:

	`				# Named argument

	.				# Member selection

	::				# Modify environment part of closure

	( )				# Function call

	& - ~ ! ++ -- *			# Single operand

	<< >>				# Shift group

	* / & %				# Multiply group

	+ - | ^				# Add group

	== > < >= <= !=			# Comparison group

	&&				# Logical and

	||				# Logical or

	= <<= >>= *= /= %= += -= |= ^= .=	# Pre-assignments
	: <<: >>: *: /: %: +: -: |: ^: .:	# Post-assignments

	\				# Sequential evaluation: returns
					# result of right side.

	,				# Expression separation

	;				# Command separation

A detailed description of each operator follows:

### ` Symbol quoting

This operator can be used to prevent a symbol from being
replaced by its value.  Instead the symbol is used directly as the value. 
It is also used to explicitly state the argument or member name in a
command, function call or object.  For example:

	open `name="joe.c",  `mode="r"
	square(`x=5, `y=6)
	[`1=5 `0=7]			# Array object
	[`x=10 `y=10]			# Structure object

### . Member selection

This operator is used to select a named member from an
object.  For example:

	o=[`x=5, `y=10]		# Create an object
	pr o.x			# Print member x
	pr o.y			# Print member y
	pr o("y")		# Same as above

### :: Modify environment

This operator replaces the environment part of the closure
on the left side with the object on the right side.

	x = 2
	fn foo(n) { print x * n }

	# Call foo in its recorded environment-
	# in this case, the global variables

	foo(7)			# Prints 14

	my_obj=[`mom=this, `x=3]

	# Call foo with my_obj as its environment

	foo::my_obj(7)		# Prints 21

### ( ) Function call

This operator calls the function resulting from the
expression on the left with the arguments inside of the parenthesis.  This
operator can also be used for object member selection and for string
character selection and substring operations.  Examples of each of these
follow:

	x.y(5)			# Call function y in object x

	z(0)=1, z(1)=2		# Set numbered members of an object

	z("foo")=3, z("bar")=4	# Set string members of an object

	z(`foo)=3, z(`bar)=4	# Set symbol members of an object

	pr "Hello"(0)		# Prints 72

	pr "Hello"(1,3)		# Prints "el" (selects substring
				# beginning with first index and
				# end before second).


### - Negate

### & Address of.

This operator converts its operand into a thunk (a zero
argument nameless function thunk with no environment).  The thunk can
be called with () or *.

### ~ Bit-wise one's complement

### ! Logical not

### ++ Pre or post increment

Pre or post increment depending on whether it precedes or follows
a variable

### -- Pre or post decrement

### * Indirection

This prefix operator is used to call a zero argument
function or thunk.

	fn set(&a b) {
		*a=b
	}

	x=3
	set x 10	# Set x to 10
	print x		# Prints 10

### << Bit-wise shift left

### >> Bit-wise shift right

### * Multiply

### / Divide

### & Bit-wise AND

### % Modulus (Remainder)

### + Add or concatenate

In addition to adding integers, this operator concatenates
strings if strings are passed to it.  For example:

	print "Hello"+" There" 	# Prints "Hello There"

"+" will also append an element on the right into an
array object on the left:

	a=[1 2 3]
	a+=4			# a now is [ 1 2 3 4 ]

	print []+1+2+3+4	# same as print [1 2 3 4]
	print []+1+2+3+[4 5]	# same as print [1 2 3 [4 5]]

### - Subtract

### | Bit-wise or

If objects are given as arguments to OR, OR unions the
objects together into a single object.  If the objects have
numerically referenced members, OR will append the array on the
right to the array on the left.  For example:

	a=[1 2 3]
	b=[4 5 6]
	a|=b			# a now is [1 2 3 4 5 6]

### ^ Bit-wise Exclusive OR

### == Equal

Returns 1 (true) if arguments are equal or 0 (false) if arguments are not
equal.  Can be used for strings, numbers, symbols and objects.  For objects,
"==" tests if the two arguments are the same object, not if the two
arguments have equivalent objects.

### > Greater than

### >= Greater than or equal to

### <  Less than

### <= Less than or equal to

### != Not equal to

### && Logical and

The right argument is only evaluated if the left argument is
true (non-zero).

### || Logical or

The right argument is evaluated only if the left argument is
false (zero).

### = Pre-assignment

The right side is evaluated and the result is stored in the variable
specified on the left side.  The right side's result is also returned.

### : Post-assignment

The right side is evaluated and the result is stored in the
variable specified on the left side.  The left side's original value
is returned.

### X= Pre-assignment group

These translate directly into: "left = left X right"

### X: Post-assignment group

These translate directly into: "left : left X right"

Notes on assignment groups:

	.=	translates into "left = left . right" and is
		useful for traversing linked lists.  For example:

		for list=void\ x=0, x!=10, ++x {	# Build list
		  list=[`next=list, `value=x]
		}

		(note, this builds the list in reverse
		order.  9,8,7...)
				
		for a=list, a, a.=next {	# Print list  (second
						# expr evals for 0)
			print a.value
		}

	x+:1	Is the same as x++
	x+=1	Is the same as ++x


":" is useful for shifting the value of variables around.  In this example,
a gets b, b gets c, and c gets 5:

	a:b:c:5

":" is also useful for swapping the values of variables.  In this example, a
gets swapped with b:

	a:b:a

### \ Sequential evaluation

The left and then the right argument are evaluated and the
result of the right argument is returned.

### , Argument separator

When this is used in statements which require only a single
expression, it has the same effect as \


## Functions

### Function declaration syntax:

Command format named function declarations:

	fn name(args) expr

	fn name(args) {
	       	body
	}

Expression format named function declarations:

	fn(name, (args), body-expr)

Command format anonymous function:

	{ fn (args) body-expr }

Expression format anonymous function:

	fn((args), body-expr)

These are all equivalent:

	fn square(x) x*x

	fn square (x) {
		x*x
	}

	square = { fn (x) x*x }

	square = fn((x), x*x)

The last forms define so-called "Lambda" (anonymous) functions.  You
can call anonymous functions without assigning them:

	x=fn((x),x*x)(6)	# x gets assigned 36

You can also define named functions right in the middle of an expression,
and immediately call them:

	y=fn(square,(x),x*x)(5)	# y gets assigned 25
	print square(6)		# Prints 36

### Argument lists

You may specify zero or more formal arguments.  Each argument must be
provided during a function call, or an error occurs.

	fn mm(x,y) x*y

	mm(1)      --> "Error: Missing arguments"

	mm(1,2,3)  --> "Error: Too many arguments"

	mm(3,4)    --> returns 12.

However, default values may be specified.  If an argument with a
default value is missing, no error occurs and the default value is used
instead:

	fn mm(x=5,y=6) x*y

	mm()       --> x=5,y=6 --> 30

	mm(6)      --> x=6,y=6 --> 36

	mm(6,2)    --> x=6,y=2 --> 12

Expressions may be used for the default values.  The expressions are
evaluated when the function is called (not when defined).  The evaluation
happens in the body of the function (where the expression may declare local
variables).  The order is left to right, and expressions on the right may
use arguments to their left.

	fn zz(x=5,y={ var q=5; x }) x*y+q

	q=10

	zz()      --> x=5,q=5,y=5  --> 25

	zz(3)     --> x=3,q=5,y=3  --> 14

	zz(3, 3)  --> x=3,q=10,y=3 --> 19

Arguments may be passed by name.  When an argument is passed by name, a
local variable of that name is injected into the function's body.  Variables
may be created which are not in the formal argument list.  Named and
unnamed arguments may be mixed in the same function call.  The named
arguments have no effect on how the unnamed arguments are processed: the
unnamed arguments are matched up left-to-right with the formal argument list
as if there were no provided named arguments.  This means that an unnamed
argument may overwrite a named argument if it occurs after the named
argument, or vice-versa.  If there were fewer unnamed arguments than in the
formal argument list, and the missing ones were declared with default
values, and they were not provided with a named argument, then the default
value is used.  If arguments without default values are missing, and they
were not provided by a named argument, an error occurs.

	fn zz(x,y,z=10) x+y+z+e

	zz(`e=1,`y=2,3) --> x=3, y=2, z=10, e=1 --> 16

Extra unnamed arguments may be collected into an object with your choice of
name with the following syntax:

	fn zz(x,extras...) {
		print "x = ", x
		forindex z extras {
			print "extras(", z, ") = ", extras(z)
		}
	}

	zz(5,6,7,8) --> prints

	x = 5
	extras(0) = 6
	extras(1) = 7
	extras(2) = 8

### Function execution

When the body of a function gets control, its activation record (the
object used for the function's local variables) gets several variables:



	this	The function's activation record itself as an object

		print this	Prints all local variables

*this* is not a variable, it's a special symbol that is replaced by the
activation record object.

	mom	The next outer lexical scope.

Mom is a normal variable.  If you assign it, the next outer lexical scope is
changed to the specified object.

Functions may be assigned to variables and passed to other functions.  For
example you can define a function *apply* which applies a function to an
argument:

	fn apply(x y) {
		return x(y)
	}

	fn square(x) {
		return x*x
	}

	print apply(square,5)	# Prints 25


Functions can return other named or unnamed functions.  (Remember to enclose
the function name in parenthesis to suppress command interpretation, or use
return).  For example:

	fn square(x) {
		return x*x
	}

	fn foo() {
		return square
	}

	print foo()(4)		# Prints 16

	fn bar() {
		(fn((x),x*x))	# Return lambda function
	}

	print bar()(4)		# Prints 16

Functions can be declared inside of other functions.  This is
especially useful for manipulating the argument lists of pre-existing
functions.  Suppose you had an averaging function:

	fn avg(func from to) {
		var x, accu = 0
		for x = from, x != to, ++x {
			accu += func(x)
		}
		return accu / (to - from)
	}

And suppose that you have another function which takes two arguments which
you'd like its average, but with one argument set to a constant:

	fn add(a b) {
		return a+b
	}

You could do this by using a function which creates a function
which calls add, but with one argument set to a constant:

	fn curry(y) {
		return fn((x), add(x,y))
	}

	Now 'avg' can be used on 'add':

	print avg(curry(20),0,10) # Prints 24

### Delayed evaluation of arguments

Arguments may be prefixed with ampersands to prevent them from being
immediately evaluated.  The marked argument is packaged up as a "thunk"- a
zero argument function with the environment set as the calling function's
activation record.  The function may call the thunk whenever it wants,
either by using the normal function call syntax or by using the indirection
operator, '*'.

For example, here is a function which sets a specified variable to
a value:

	fn set(&x,y) { *x = y }

	set q 10

	print q	--> prints 10

## Statements

### If statement

	if test-expr-1 {
		expr-1
	} test-expr-2 {
		expr-2
	} test-expr-3 {
		expr-3
	} {
		otherwise-expr
	}

	if(test-expr-1,expr-1,test-expr-2,expr-2,...,otherwise-expr)

### Foreach statement

	foreach name expr block

	foreach `label name expr block

Sets the variable *name* to each element in the object resulting from *expr*
and executes the *block*.

*foreach* may optionally be labeled for matching with the argument to
*break* and *continue*

	->foreach a [1 2 3] print(a)
	1
	2
	3
	->

### Forindex statement

	forindex name expr block

	forindex `label name expr block

Sets the variable *name* to each valid index into the object resulting from
*expr* and executes the block.

*forindex* may optionally be labeled for matching with the argument to
*break* and *continue*

	->forindex a [10 20 30] print(a)
	0
	1
	2
	->

### Loop statement

	loop block
	loop `label block

The block gets repeatedly executed until a 'break' or 'until' statement
within the block terminates the loop.

*loop* may optionally be labeled for matching with the argument to *break*
and *continue*.

### While statement

	while expr block

	while `label expr block

The block is repeatedly executed if the expression is true.

*while* may optionally be labeled for matching with the argument to *break*
and *continue*.

### For statement

	for init, test, incr block

	for `label init, test, incr block

This is a shorthand for the following while statement:

	init
	while test {
		block
		incr
	}

Thus,

*init* is usually used as an index variable initializer

*test* is the loop test

*incr* is the index variable incrementer

*for* may optionally be labeled for matching with the argument to *break*
and *continue*

### Return statement

	return

	return expr

*return* exits the function it is executed in with the given return value or
with *void* if no value is given.

### Break statement

	break

	break LABEL

*break* jumps out of the innermost or labeled loop

### Continue statement

	continue

	continue LABEL

*continue* jumps to the beginning of the innermost or labeled loop.

### Until statement

	until expr

*until* exits the loop it's in if *expr* is true.

### Var statement

	var a, b, c

Declare local variables.  The variables may also have initializers:

	var a=10, b=20

	fn raise(a) {
		var x
		for x=1, a, x<<=1\ --a
		return x
	}

### Scope statement

	scope expr expr ...

The expressions are evaluated in their own scope.  If they create local
variables, they will not show up in the outer scope.  The value of the last
expression is returned.

## Intrinsics

Here are functions that are always built-in.

### loadfile

	a = loadfile("name")

Execute an Ivy source file within an empty global variable scope.  The
executed code will only see Ivy's built-in functions.  The final value is
returned.

### len

	len(a)

Returns the length of string 'a' or number of elements in
array 'a'

### print

	print(...)

Prints the arguments

### printf

	printf(...)

C printf

	printf "%d\n", 17

### get

	a=get()

Get a line of input as a string.  Returns void if there is
no more input.

	# Add line numbers to input
	n=1
	while a=get() {
		print n++, " ", a
	}

### atoi

	x=atoi("2")

Converts a string to a number

### itoa

	s=itoa(20)

Convert a number to a string

### clear

	clear(...)

Frees the values of the listed variables and sets the
variables to VOID.

### dup

	b=dup(a)

Make a duplicate of an array/object

### match

	match(string,pattern,result-variables...)

Regular expression pattern matching

Return true if string matches pattern (which must be a
regular expression string).

If there is a match, each spanned area is stored in the
corresponding result variable:

	match "fooAbar" ".*A.*" a b

		a now has "foo" and b has "bar".

It is ok to supply fewer result variables than there are
spanning areas.

	The regular expression string may be made of:

                    .      matches any character.
                    *      matches zero or more of the previous character
                           (generates a result string).
                    +      matches one or more of the previous character
                           (generates a result string).
                    [...]  matches one character in the list ...
                           ranges may be specified with the list, such
                           as 0-9, a-z, etc.
                    x      other characters match themselves only.

Note that the entire string must be spanned for a match to
occur:  It is as if the pattern always begins with ^ and
ends with $.

## Math functions

The following functions from the standard C library are provided:

	sin() cos() tan() asin() acos() atan() atan2()
	sinh() cosh() tanh() asinh() acosh() atanh()
	exp() log() log10() pow() hypot() sqrt()
	floor() ceil() int() abs() min() max() erf() erfc()
	j0() j1() jn() y0() y1() yn()

## Object-Oriented Programming

Besides being available for use by the programmer, Ivy's objects are used
internally for activation records.  This means that a function's local
variables are implemented as object members.

The only difference between regular objects and objects used for activation
records is the presence of a member called *`mom*.  This member refers to
the next outer scoping level.  Ivy uses lexical scoping, so the chain of
next outer levels include (for the case of nested functions) the parent
function's (*not* the calling function's) activation record, then the global
variables (when modules are loaded, they each get their own object for
global variables), then finally the object containing Ivy's built-in
functions, such as *print*.  During symbol lookup, the chain of moms is
searched for the symbol.

When functions are passed around, they always come in closures.  A closure
contains a pointer to the function's code and a pointer to the environment
where it was defined, which is the activation record in effect at that time. 
The environment is the object that becomes the function's activation
record's mom when the function is called.

With this understanding, we can proceed towards implementing object-oriented
programming in Ivy.  There are two ways to do it: the direct method and the
closure method.  They are equivalent, and will be shown side by side.

First we need to define a class to hold member functions and static
variables (variables shared by all instances of the class).  In the direct
method, we just create an object, but with *`mom* set to the current
activation record, in this case the one containing the global variables:

	My_class=[`mom=this]

The special symbol *this* always refers to the object being used as the
current activation record.

We can add a member function by assigning a lambda (nameless) function to a
member name (*show* in this case):

	My_class.show = fn((), {
		print x
	})

Or we can do this same thing by using dot notation in the function
declaration:

	fn My_class.show() {
		print x
	}
	
	fn My_class.increment() {
		x = x + 1
	}

Or we could even have included them when we created the object in the first
place:

	My_class = [
		`mom = this
	
		`show = fn((), {
			print x
		})
	
		`increment = fn((), {
			x = x + 1
		})
	]

In the closure method, we write a function which returns its activation
record.  This will be used as the class.  Any nested functions will become
member functions:

	fn create_My_class() {
	
		fn show() {
			print x
		}
	
		return this
	}

	My_class = create_My_class()

The closure method has the advantage of not requiring you to explicitly set
*mom*, in case that bothers you.  *My_class.mom* will still exist, however. 
It was set in the activation record when then function was invoked.

We can add more member functions after the class has been created (by either
method: assigning lambda functions to member names or by declaring named
functions with the dot notation):

	fn My_class.increment() {
		x = x + 1
	}

Notice that member functions refer to instance variables as in C++ or Java. 
There is no need to prefix each instance variable with *self* or *this* as
in most languages with prototype based object systems.  On the other hand,
calls to sibling member functions should use dot notation:

	fn My_class.inc_and_show() {
		this.increment()
		this.show()
	}

We need a constructor to create class instances.  This constructor
should be a class member.  For the direct method, we write this:

	fn My_class.instance(i=[]) {
		i.x = 10
		i.mom = My_class
		return i
	}

Notice that an empty object is provided as the default value for *i*.  If
the *i* argument is missing, this default empty object will become the
instance.  If the argument is provided, then the caller provided object is
used for the instance.  We will use this capability later for derived
classes, where we want to allow the derived class constructor to call the
base class constructor.

Next, we create an instance variable x and set it to a default value 10.

Finally, we set the mom of the instance to the class so that if we call
member functions on the instance, the ones defined in the class will be
found.

For the closure method, the instance creation function is a nested function
of *create_My_class* which returns its activation record as the instance. 
We separate out a construction function from the instance allocator so that
it may be later called by derived class constructors:

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

Now we create some instances of the class:

	instance_1 = My_class.construct()
	instance_2 = My_class.construct()

The instances are now ready and we can call their member functions:

	instance_1.show()   --> prints 10
	instance_1.increment()
	instance_1.show()   --> prints 11
	instance_2.show()   --> prints 10

But, the member functions are not in the instance objects, and even then you
would expect the called function's activation record's mom to be the class,
not the instance.  So what is going on?

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

	z=[]
	z.show = instance_1.show
	z.show()  --> prints 11

The environment replacement is happening in the *instance_1.show* part of
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

	fn My_class.inc_and_show() {
		increment()
		show()
	}

But it will not work.  The increment will look up *x* starting in the
environment where it was defined.  This is either in the global environment
for the direct method or in the class object for the closure method.  Either
way, it's not accessing the *x* in the instance.

The correct way to write this function is as follows:

	fn My_class.inc_and_show() {
		mom.increment()
		mom.show()
	}

*Mom* will refer to the instance object when inc_and_show is called.  In
this case, you could replace *mom* with *this*.  But it is better to use
*mom*, since the member function should not be modifying the activation
record of *inc_and_show* itself.

### Inheritance

We can create a new class based on an existing class like this:

	DerivedClass = [ `mom = MyClass ]

Since *DerivedClass*'s mom is set to *MyClass*, symbol lookup for member
functions will find ones defined in *MyClass* if they are not directly
provided in *DerivedClass*.

It will need a new instance constructor:

	fn DerivedClass.construct(i=[]) {
		i = MyClass.construct(i)
		i.y = 20
		i.mom = DerivedClass
		return i
	}

Notice how we are calling the base class's constructor, but then elaborating
the instance by adding a new instance variable *y*.  Naturally the instance's
mom is replaced (it had been set to *MyClass* by *MyClass*'s constructor) so
that it is set to *DerivedClass*.

And we will override one of the member functions:

	fn DerivedClass.show() {
		print "Derived"
		print x
		print y
	}

Using the closure method, we provide a new class creation function and then
call it:

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

*create_DerivedClass* is defined as a member of *MyClass* so that when
it's called, *create_DerivedClass*'s activation record's mom ends up being
*MyClass*.

An alternative way of defining *create_DerivedClass* which does not
involve modifying *MyClass* at all is as follows:

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

	derived_instance_1 = DerivedClass.instance()

	derived_instance_1.show()  --> Prints:

	Derived
	10
	20

