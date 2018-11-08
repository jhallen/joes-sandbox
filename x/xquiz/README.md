# Math Quiz / Computer Algebra System for X

This program gives a math quiz, but includes a computer algebra system.  It
was written in 1990.

This was work for my [IQP at WPI](https://www.wpi.edu/academics/undergraduate/interactive-qualifying-project). 
This was a two person project: I wrote the code and my partner wrote the
thesis.

The advisor's original idea was to use
[Prolog](https://en.wikipedia.org/wiki/Prolog) and to try to symbolically
prove that any variation of the answer that the student might enter matched
the correct answer.  But I did not think Prolog was appropriate for the mundane
tasks of an X Windowing system program, so I ended up writing it in C.  LISP
would have been a better choice, but I don't think LISP's integration with X
was so great at that time, and in any case I knew C and did not know LISP.

At the time I was fascinated by
[Macsyma](https://en.wikipedia.org/wiki/Macsyma) and wanted to learn how to
make one.

## Features

* As you type the answer in standard FORTRAN formula notation, it is typeset in standard math notation on the screen.

* An equation editor is provided which allows you to select parts of the equation with the mouse, and perform common operations on them such as simplification, factoring and distributing.

* The simplifier will combine terms, reduce fractions, take derivatives, and remove identity elements.

* You can graph the equation you are working on at the click of a button.

## Build Instructions

For Ubuntu 16.04:

	apt-get install xorg-dev

	make

	./quiz data
