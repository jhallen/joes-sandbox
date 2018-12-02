# Ivy

- [Introduction](#Introduction)
- [Invocation](#Invocation)
- [Syntax](#Syntax)
- [Values](#Values)
- [Variables](#Variables)
- [Objects](#Objects)
- [Expressions](#Expressions)
- [Operators](#Operators)
- [Functions](#Functions)
- [Statements](#Statements)
- [Built in functions](#Built)

## Introduction

Ivy is an extensible, dynamically typed, late binding language intended to
be used as an embedded command language.  It can also be used stand-alone:
it can execute script files from the command line or presents a
read-eval-print loop (REPL) to the user if no files are given.

Ivy's extensibility is based on the fact that statements are syntactically
identical to funciton calls.  Also blocks (surrounded by braces) may be used
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

## Values

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

### Floating point

	x=.125
	x=.125e3
	x=0.3

### Strings

String constants are enclosed in double-quotes:

	x="Hello there"

Escape sequences may also be used inside of strings.

## Variables

Variables set outside of functions are global variables.  They will be
visible in any function or scope unless 'var' is used to force the creation
of a new local variable with the same name.

If a variable is assigned to inside of a function and there is no global
variable of the same name, that variable will be local to the block it was
assigned in.


## Objects

Objects are catch all data structures which may be indexed by number, symbol
or string.  Objects indexed by number are similar to arrays.  Objects
indexed by symbol are similar to structures.  Objects indexed by string are
similar to hash tables.

A mixture of index types may be used on the same object, but always
refer to different locations (symbol foo and string "foo" can have different
values).

An automatic array is used for numbered indexing.  This array
expands to accommodate whatever index is used.  If you store a value at
index 0 and also at index 999, then space for 1000 values will be allocated.

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
		pr x		# Prints 10

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

		for list=0\ x=0, x!=10, ++x	# Build list
		  list=[`next=list, `value=x]

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
argument, or vice-verse.  If there were fewer unnamed arguments than in the
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

This is a shorthand for the follow while statement:

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

## Built in functions

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
