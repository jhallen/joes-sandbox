                                     Ivy

What is it?

Ivy 2005 is the latest version of the scripting language I wrote originally
in 1993.  This version adds an extensible syntax and is designed to be
embedded in my text editor JOE.

Extensible syntax

By "extensible syntax" I mean that you can define new statements in the
language just by declaring new functions.  The trick then is to make
functions powerful enough to do the same things that statements can: delayed
evaluation and messing around with scope.

For example, here is a function for a text editor which allows the body code
to move the cursor.  When the code is complete, the cursor is automatically
restored (akin to unwind-protect in LISP):

	fn excursion(&body)
		var save = cursor
		var rtn = *body
		cursor = save
		(rtn)

The '&' prefixing the formal argument indicates that the code for that
argument should not be evaluated immediately (as would normally be done in
this pass-by-value language), but to instead delay its evaluation (the
argument is packaged up as a function in its closure).  The '*' causes the
function to be called (evaluated).  In LISP, this would all be done with
quoting or macros.

Other features

Ivy's interpreter and parser are both event driven.  For example, when an
Ivy program calls a C-language function, it returns from the interpreter
and returns a pointer to the C-language function to the caller.  When the
C-function is done, the interpreter should be called again.

This means Ivy can be used in a co-operative multi-threaded environment:
think of a command interpreter for a MUD.

In Ivy, everything is a function, but to make statements look "normal" (and
to separate statements out from the parser as in LISP), there is a "command"
format for function calls.  Basically, these are the same:

	func(x,y,z)
	func x y z

Block structuring can be indicated by square brackets or indentation:

	# Indented blocks of commands
	foreach a {1 2 3}
		if a==2
			print "There's a 2"
			print "Hello"

	# square bracket blocks of commands
	[foreach a {1 2 3} [if a==2 [print "There's a 2"; print "Hello"]]]

	# Expressions
	foreach(a,{1 2 3},if(a==2,print("There's a 2")\print("Hello")))

Indentation is preferred for blocks, but square brackets are convenient for
when Ivy is used interactively.  A generic method of passing multiple
indented blocks to a function is provided.  This:

	if a==1
		print "It's a 1"
	next a==2
		print "It's a 2"
	last
		print "It's something else"

Is the same as this:

	if(a==1,print("It's a 1"),a==2,print("It's a 2"),print("It's something else"))

And can be used for your own functions:

	fred a==1
		print "It's a 1"
	next a==2
		print "It's a 2"
	last
		print "It's something else"

'fred' gets passed 5 arguments.

Ivy is compiled to byte-code, which is then interpreted.  Type checking is
done at run time.  It is a late binding language, meaning that variable name
to value bindings happen late (basically there's a hash table lookup
whenever a variable is accessed).  To help make this a little faster, all
names are converted to "atoms", which are unique strings which exist at only
one address.  The string's address is all that needs to be compared during
the hash table lookup.  The address is used as the hash value itself: so
most variable lookups reduce to a single array access followed by a single
word compare.

Ivy has closures: you can define functions inside others, and then return
them.  The returned function still have access to the creating function's
scope at the time of creation.

Ivy has lambda (nameless) functions.

	print fn(x,x*x)(7)	# Prints 49

Ivy has named argument passing and default argument values:

	fn square(a=10,b=10) a*b

	print square(`a=3)
	print square(`b=4)
	print square(3,5)

Ivy has command history and on-line help.

Ivy has garbage collection.

Ivy has "objects" which are a catch-all for array, structures, hashes and
tuples:

	a = {1 2 3}			# An array
	print a(1)

	a = {`a="hi" `b="there" }	# A structure
	print a.b

	a("first") = 1			# A hash table
	a("second") = 2
	print a("first")

	{x y z} = {1 2 3}		# A tuple: assign three variables at
					# once

	c = {`next=(), `value=0}	# A linked list
	b = {`next=c, `value=1}
	a = {`next=b, `value=2}

These objects are also used for closures.  They are the primary data
structure of the language.  When objects are used as closures, several
special members get defined:

	this	# The closure itself
	mom	# The next outer lexical scope
	dynamic	# The next outer dynamic scope

Ivy has a few other basic types:
	"hi"		# String
	123		# Integer
	12.3		# Double precision floating point
	fn((x),x*x)	# Function

Ivy has C-like operators.  It also has post-assignment operators, a
generalization of C post-increment.

	a=b=3		# 3 written to b, result (3) written to a
	a=b:3		# 3 written to b, previous value of b written to a

	a+=3		# pre-add a (returns a+3)
	a+:3		# post-add a (returns old value of a)

	a:b:a		# swap a and b

It has an assignment form of the '.' (member selection operator).  This is
convenient for traversing linked lists:

	c={`next=0, `value=3}
	b={`next=c, `value=2}
	a={`next=b, `value=1}

	for x=a, x, x.=next
		print x.value

Ivy has string-slicing built-in:

	a="Hello"
	print a(2,5) # Prints "llo"
	print a(0) # Prints "H"

Ivy has labeled break and continue:

	outer: for a=0, a!=10, ++a
	  for b=0, b!=10, ++b
	    if a+b==15
	      break outer

Other planned features:

	Exceptions.

	Continuations or co-routines (haven't decided yet: leaning towards
	co-routines, but designing for continuations).

	First-class regular expressions, as in perl.

	All UNIX system calls and library functions, as in perl.

	Object oriented Ivy (you can do it now with closures, but there
	is no inheritance, protection or module system).

	Widget library and X interface.  I'd like to use Ivy as a
	replacement for "gnu-plot" (and when built into JOE, as a free
	version of "mathcad").
