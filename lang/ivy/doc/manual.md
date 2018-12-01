# Ivy

## Introduction

	Ivy is an embeddable language which is useful as both an extension
and a command language.  Its syntax is designed to be similar to
command-style languages like BASIC and UNIX shell, and is thus suitable for
simple command entry applications.  However, the awkwardness typically
associated with command languages is eliminated through Ivy's support of
full expressions, including function calls and assignments, and orthogonal
block structuring.

	Ivy compiles its source code into a compact byte code which is then
interpreted.  Six types are currently supported: integers, strings,
functions, void, floats, and objects.  Objects are late-binding storage
devices which take the role of arrays, structures and hash tables.  Type
declarations are not necessary, but type checking is not done until
run-time.

	Ivy comes packaged as an interactive language like BASIC and LISP. 
You can either execute language statements immediately from the keyboard or
run a program stored in a file.  Ivy is also easy to embed into another
program.  Calls are provided to execute Ivy code and to add C function
extensions to Ivy's interpreter.


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

Ivy uses '#' to introduce comments.  Everything from an unquoted # to the
end of the line is a comment.  '#' was chosen so that the very first line in
an Ivy program can be '#!/usr/bin/ivy', which makes the file into a script
in UNIX.

### Expression syntax

Everything in Ivy is an expression, meaning that statements and function
calls have the same syntax.  A consequence of this is that new statements
can be added to the language by defining new functions:

	print(square(10))	Call square function with 10 as arg, and
				print result.

	print(if(a==1,2,3))	If a==1, print 2.  Otherwise print 3.

### Lists of expressions

A list of expressions is expected within parenthesis and brackets:

	(list)			Argument list, parenthetical expression,
				or formal args for function definition.

	[list]			An object.

If a list is expected in the given context, the list is used as is.  If a
single expression is expected in the given context, all of the expressions
in the list are evaluated (left to right), and the value of the right-most
expression is returned:

	mul(2 3)		Pass two args to mul: 2 and 3

	3*(2 3)			Results in the value 9

The expressions within a list can be separated with whitespace, commas or
semicolons.  These are all identical in this context:

	mul(10 20 40)		3 args passed to mul
	mul(10,20;40)		3 args passed to mul
	mul(10;,20,,,40)	3 args passed to mul

Note that successive separators with nothing between them are ignored:

	mul(1,2) is exactly the same as mul(1,,,,2)

Note however, that an empty set of parenthesis has meaning:

	mul(() 20)	Passing two args to mul: void and 20.

Note that \ is a sequential evaluation infix operator which computes the
left side, discards the result and then computes the right side and returns
its result:

	mul(10\15\20 30)	Pass two args to mul: 20 and 30.

### Ambiguities

Since whitespace is a valid separator for expressions, ambiguities between
symbols which can be both infix and prefix operators result.  These
ambiguities are resolved by noting the physical distance between the
symbol and its potential arguments.  If the distance is balanced, the
operator is infix, otherwise it's prefix.

	a - b			One expression: subtract b from a
	a  -b			Two expressions: a and negated b
	f (7)			Two expressions: f and 7.
	f(7)			One expression: a call to f with arg 7
	f ( 7 )			One expression: a call to f with arg 7
	f  ( 7 )		Two expressions: f and 7.

Note that tabs occur every 8 spaces for this purpose.

## Command format function calls

There is a "command" format for function calls:

	function-name arg arg arg ; function-name arg arg arg ; ...

This is available at the top-level (non-enclosed) and within brackets:

	{commands}

In this context, semicolons and new-lines have the same low precedence: either
can be used to separate successive commands.  Commas and whitespace can be
used as argument separators, just as in lists.

"function-name" must be a single simple name or a member name.  If it is a
more complicated expression, the command is reduced to a list of expressions
until the next new-line or semicolon.

	print 10 20		# Print 10 and 20 (one command)

	print(20) print(1)	# Print 20 and 1 (two expressions)

Sometimes you may want to suppress the command interpretation of a single
simple name.  This can be done by surrounding the name with parenthesis:

	(print)			# The print function (no call)
	print			# Call print function with no arguments

### Blocks

Arguments to commands can be given as blocks or expressions.  These are all
equivalent:

	if(a==1,print("one"),a==2,print("two"),print("something else"))

	if a==1 print("one") a==2 print("two") print("something else")

	if a==1 { print "one" } a==2 { print "two" } { print "else" }

	if a==1 {
		print "one"
	} a==2 {
		print "two"
	} {
		print "something else"
	}

## Values

### Symbols

	There are symbols:

		`hello

	Symbols can be tested for equality:

		`hello==`hello		# True
		`hello==`goodbye	# False

	There is an interned string table for all symbols, so to check if
	two symbols are identical their addresses are compared.

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

	Floating point:

		x=.125
		x=.125e3
		x=0.3

### Strings

	String constants are enclosed in double-quotes:

		x="Hello there"

	Escape sequences may also be used inside of strings.

## Variables

	Variables set outside of functions are global variables.  They will
be visible in any block unless 'var' is used within the block to force the
creation of a new local variable with the same name.

	If a variable is set inside of a function and there is no global
variable of the same name, that variable will be local to the block it was
assigned in.


## Objects

	Objects are catch all data structures which may be indexed by
number, symbol or string.  Objects indexed by number are similar to
arrays.  Objects indexed by symbol are similar to structures.  Objects
indexed by string are similar to hash tables.

	A mixture of index types may be used on the same object, but always
refer to different locations (symbol foo and string "foo" can have different
values).

	An automatic array is used for numbered indexing.  This array
expands to accommodate whatever index is used.  If you store a value at
index 0 and also at index 999, then space for 1000 values will be allocated.

	Auto-expanding hash tables are used for symbol and string indexing. 
These hash tables expand to twice their size when they reach half full. 
Symbol indexing is faster than string indexing since, for symbols, the hash
value does not have to be recomputed on each access.  On the other hand,
there is a global hash table for symbols which is never compacted so symbols
should not be used for data input.

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

	Then the change will appear both in x and in y


## Expressions

	An expression is a single or dual operand operator, a constant, a
variable, a lambda function, a block enclosed within braces, a function
call, or an object.  Examples of each of these cases follow:

		~expr			Single operand

		expr+expr		Dual operand

		25			Constant

		(expressions)		Precedence
		{commands}

		expr(expr ...)		Function call

		[1 2 3 4]		An array object

		[`next=item `value=1]	A structure object

		(fn (x,y) x*y)(3,5)	Calling a lambda function


## Operators

	Ivy uses a superset of operators from C but with more intuitive
precedence.  Here are the operators grouped from highest precedence to
lowest:

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

### "`" Symbol quoting

		This operator can be used to prevent a symbol from being
replaced by its value.  Instead the symbol is used directly as the value. 
It is also used to explicitly state the argument or member name in a
command, function call or object.  For example:

		open `name="joe.c",  `mode="r"
		square(`x=5, `y=6)
		[`1=5 `0=7]			# Array object
		[`x=10 `y=10]			# Structure object

### "." Member selection

		This operator is used to select a named member from an
object.  For example:

		o=[`x=5, `y=10]		# Create an object
		pr o.x			# Print member x
		pr o.y			# Print member y
		pr o("y")		# Same as above

### "::" Modify environment

		This operator replaces the environment part of the closure
on the left side with the object on the right side.

		x = 2
		fn foo(n) { print x * n }

		# Call foo in its recorded environmment-
		# in this case, the global variables

		foo(7)			# Prints 14

		my_obj=[`mom=this, `x=3]

		# Call foo with my_obj as its environment

		foo::my_obj(7)		# Prints 21

### "( )" Function call

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


### "-" Negate

### "&" Address of.

		This operator converts its operand into a thunk (a zero
	argument nameless function thunk with no environment).  The thunk can
	be called with () or *.

### "~" Bit-wise one's complement

### "!" Logical not

### "++" Pre or post increment depending on whether it precedes or follows
	   a variable

### "--" Pre or post decrement

### "*" Indirection

		This prefix operator is used to call a zero argument
	function or thunk.

		fn set(&a b) {
			*a=b
		}

		x=3
		set x 10	# Set x to 10
		pr x		# Prints 10

### "<<" Bit-wise shift left

### ">>" Bit-wise shift right

### "*" Multiply

### "/" Divide

### "&" Bit-wise AND

### "%" Modulus (Remainder)

### "+" Add or concatenate

		In addition to adding integers, this operator concatenates
	strings if strings are passed to it.  For example:

		pr "Hello"+" There" 	# Prints "Hello There"

		'+' will also append an element on the right into an
	array object on the left:

		a=[1 2 3]
		a+=4			# a now is [ 1 2 3 4 ]

		pr []+1+2+3+4		# same as pr [1 2 3 4]
		pr []+1+2+3+[4 5]	# same as pr [1 2 3 [4 5]]

### "-" Subtract

### "|" Bit-wise OR or 

		If objects are given as arguments to OR, OR unions the
        objects together into a single object.  If the objects have
        numerically referenced members, OR will append the array on the
        right to the array on the left.  For example:

		a=[1 2 3]
		b=[4 5 6]
		a|=b			# a now is [1 2 3 4 5 6]

### "^" Bit-wise Exclusive OR

### "==" Equal

Returns 1 (true) if arguments are equal or 0 (false) if arguments are not
equal.  Can be used for strings, numbers, symbols and objects.  For objects,
"==" tests if the two arguments are the same object, not if the two
arguments have equivalent objects.

### ">" Greater than

### ">=" Greater than or equal to

### "<"  Less than

### "<=" Less than or equal to

### "!=" Not equal to

### "&&" Logical and

The right argument is only evaluated if the left argument is
true (non-zero).

### "||" Logical or

The right argument is evaluated only if the left argument is
false (zero).

### "=" Pre-assignment

The right side is evaluated and the result is stored in the variable
specified on the left side.  The right side's result is also returned.

### ":" Post-assignment

The right side is evaluated and the result is stored in the
variable specified on the left side.  The left side's original value
is returned.

### "X=" Pre-assignment group

These translate directly into: "left = left X right"

### "X:" Post-assignment group

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

		a:b:c:5
			':' is useful for shifting the value of variables
			around.  In this example, a gets b, b gets c, and
			c gets 5.

		a:b:a
			':' is also useful for swapping the values of
			variables.  In this example, a gets swapped with
			b.

### "\" Sequential evaluation

The left and then the right argument are evaluated and the
result of the right argument is returned.

### "," Argument separator

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

	Arguments may be passed by name.  When an argument is passed by
name, a local variable of that name is injected into the function's body. 
Variables may be created which are not in the argument list.  Named and
unnammed arguments may be mixed in the same function call.  The named
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

	Extra unnamed arguments may be collected into an object with the
following syntax:

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

			[this is not a variable, it's a special symbol that
			is replaced by the activation record object]

		mom
			The next outer lexical scope.  This is a normal
			variable.  If you assign it, the next outer lexical
			scope is changed to the specified object.

		dynamic
			This function's dynamic scope: the caller's
			activation record.

			This feature is optional and depends on how
			Ivy is compiled.  It is off by default.

	Functions may be assigned to variables and passed to other
functions.  For example you can define a function 'apply' which applies a
function to an argument:

		fn apply(x y) {
			return x(y)
		}

		fn square(x) {
			return x*x
		}

		print apply(square,5)	# Prints 25


	Functions can return other named or unnamed functions.  To return a
named function, 'return' must be used since it forces its argument to be an
expression (otherwise the returned function name would look like a function
call).  For example:

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

		pr bar()(4)		# Prints 16


	Functions can be declared inside of other functions.  This is
especially useful for manipulating the argument lists of pre-existing
functions.  Suppose you had an averaging function:

		fn avg(func from to) {
			var x, accu
			for x=from\ accu=0, x!=to, ++x {
				accu+=func(x)
			}
			return accu/(to-from)
		}

	And suppose that you have another function which takes two arguments
which you'd like to average, but with one argument set to a constant:

		fn add(a b) {
			return a+b
		}

	You could do this by using a function which creates a function
which calls add, but with one argument set to a constant:

		fn curry(y) {
			return fn((x),add(x,y))
		}

	Now 'avg' can be used on 'add':

		print avg(curry(20),0,10)

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

	foreach variable expr block

	foreach `label variable expr block

		Sets the variable to each element in the array resulting
		from expr and executes the block.

		'foreach' may optionally be labeled for matching
		with the argument to 'break' and 'continue'

	e.g.
	foreach a [1 2 3] {
		print a		# prints 123
	}


### Forindex statement

	forindex variable expr block

	forindex `label variable expr block

		Sets the variable to each valid index into the array
		resulting from expr and executes the block.

		'forindex' may optionally be labeled for matching
		with the argument to 'break' and 'continue'

	e.g.
	forindex a [10 20 30] {
		print a		# prints 0, 1 and 2.
	}


### Loop statement

	loop block
	loop `label block

		The block gets repeatedly executed until a 'break' or
                'until' statement within the block terminates the loop.


		'loop' may optionally be labeled for matching
		with the argument to 'break' and 'continue'

### While statement

	while expr block

	while `label expr block

		The block is repeatedly executed if the expression is true.


		'while' may optionally be labeled for matching
		with the argument to 'break' and 'continue'

### For statement

	for expr1, expr2, expr3 block

	for `label expr1, expr2, expr3 block

		This is a shorthand for the follow while statement:

			expr1
			while expr2 {
				block
				expr3
			}

		Thus,
        	   expr1 is usually used as an index variable initializer
		   expr2 is the loop test
	           expr3 is the index variable incrementer

		'for' may optionally be labeled for matching
		with the argument to 'break' and 'continue'

### Return statement

	return

	return expr

		This exits the function it is executed in with the given
                return value or with the value of the last expression
                preceding the return.

### Break statement

	break

	break LABEL

		This jumps out of the innermost or labeled loop

### Continue statement

	continue

	continue LABEL

		This jumps the beginning of the innermost or labeled loop.

### Until statement

	until expr

		This exits the loop it's in if the expression is true.

### Var statement

	var a, b, c

		Declare local variables.  The variables may also have
		initializers:

			var a=10, b=20

			fn raise(a) {
			  var x
			  for x=1, a, x<<=1\ --a
			  return x
			}


### Scope statement

	scope expr expr ...

		The expressions are evaluated in their own scope.  If they
	create local variables, they will not show in the outer scope.  The
	value of the last expression is returned.


## Builtin functions

### loadfile

	a = loadfile("name")

		Execute an Ivy source file within an empty global variable
	scope.  The executed code will only see Ivy's built-in functions. 
	The final value is returned.

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

## math functions

	The following functions from the standard C library are provided:

	sin() cos() tan() asin() acos() atan() atan2()
	sinh() cosh() tanh() asinh() acosh() atanh()
	exp() log() log10() pow() hypot() sqrt()
	floor() ceil() int() abs() min() max() erf() erfc()
	j0() j1() jn() y0() y1() yn()
