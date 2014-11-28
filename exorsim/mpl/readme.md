# MPL - Motorola's M6800 Programming Language

MPL is a PL/I-like language for Motorola's 8-bit
[M6800](http://en.wikipedia.org/wiki/Motorola_6800) development system, the
[Motorola Exorciser](http://exorciser.net/excorciser1_en.htm) (running
[MDOS](http://exorsim.sourceforge.net/mdos-intro.html)).  I assume it's Motorola's answer to
[PL/M](http://www.cpm.z80.de/randyfiles/plm/MCS-8_PLMprogGuide1973.pdf) for
Intel's 8080 microprocessor.  The language seems to have been created before 1976.

It's fun to pretend it's 1978 and try to write some programs with this
language.  It's certainly quirky by today's standards:

 * Supports fixed-point decimal
 * Supports multidimensional arrays
 * Block structuring and sub-procedures, but no scoping!
 * Severely limited identifiers
 * Severely limited strings
 * Supports structures and pointers (kind of..)
 * Pass by reference arguments
 * No functions, no recursion

I have not been able to find the manual for MPL, but version 2.1 of the
compiler is available on some MDOS disk images in
[Bitsavers](http://bitsavers.informatik.uni-stuttgart.de/bits/Motorola/). 
These can be run on an [Exorciser
simulator](http://exorsim.sourceforge.net/).

I have been able to find two sample programs in the [Motorola Users Group
Library](http://test.dankohn.info/~myhome/projects/68HC11/AXIOM_HC11/Source/Users%20Group/index.txt),
specifically:

[61 - MPL CORE SORT PROGRAM](http://test.dankohn.info/~myhome/projects/68HC11/AXIOM_HC11/Source/Users%20Group/UG061)

[154 - MPLUTIL](http://test.dankohn.info/~myhome/projects/68HC11/AXIOM_HC11/Source/Users%20Group/UG154)

The disk image provides a few more hints.  There is a file with MPL 2.1
[release notes](mplnews.sa).  Also the compiler binary can be inspected for
likely keywords.  I find these:

	LE GE LT GT NE EQ IEOR IAND IOR SHIFT NOT AND OR ADDR DECLARE DCL
	BASED SIGNED BY INITIAL INIT MAIN GO GOTO TO IF THEN ELSE DO END
	CALL RETURN CONSTANT CONST EXT OPTS OPTIONS DEFINE DEF PROCEDURE
	PROC WHILE BIT LABEL EXTERNAL GLOBAL SHORT LONG BSCT CSCT DSCT PSCT
	GIVING

There are certainly more keywords, but they are not obvious from the
compiler binary- they must be encoded in some way.  I know of these:

	BIT BIN DEC CHAR

There are two libraries which come with the compiler: MPLULIB.RO and
MPLSLIB.RO.  MPLULIB has these procedures:

	MDOS DSPLY KEYIN PRINT PUSH2 PULL2

Finally, there is an ad for the [Resident MPL Compiler](mplad.md).

## The compiler

The compiler, MPL.CM, is 39K and with no overlays.  This is quite large for
the Exorciser, which had only 56K of RAM.  8K is reserved for MDOS, so this
leaves just 9K of free RAM for the compiler to use.  By contrast, the
assembler, RASM is 16K and the linker, RLOAD is 8.6K.

A typical run looks like this (= is the command prompt):

```
MDOS  3.05

=MPL TEST;LO=TEST.AI

MDOS MPL COMPILER  2.1
COPYRIGHT BY MOTOROLA 1978

PAGE    1   MPL  2.1

  10 TEST: PROC OPTIONS(MAIN)
  20   DCL HELLO CHAR(13) INIT('HELLO, WORLD!!')
  30   DCL CR BIN INIT(13)
  40   CALL DSPLY<1,1,ADDR(HELLO)>
  50   CALL MDOS
  60   END
TOTAL ERRORS    0

TOTAL ERRORS    0

=RASM TEST.AI
MDOS MACROASSEMBLER  3.01 
COPYRIGHT BY MOTOROLA 1977


=RLOAD
MDOS LINKING LOADER REV  3.02
COPYRIGHT BY MOTOROLA 1977
?IF=TEST
?BASE
?LOAD=TEST.RO
?LIB=MPLSLIB,MPLULIB
?OBJA=TEST.CM
?MAPF
  NO UNDEFINED SYMBOLS

MEMORY MAP

  S SIZE  STR  END COMN
  B 0000 0020 0020 0000 
  C 0000 2000 2000 0000 
  D 007C 2000 207B 000A 
  P 0018 207C 2093 0000 

MODULE NAME BSCT DSCT PSCT
            0020 2000 207C
  MDOS      0020 2072 208E
  DSPLY     0020 2072 2091

COMMON SECTIONS

   NAME  S SIZE  STR 
  T$     D 000A 2072

DEFINED SYMBOLS
  

MODULE NAME:       
  TEST   P 207C    

MODULE NAME: MDOS  
  MDOS   P 208E    

MODULE NAME: DSPLY 
  DSPLY  P 2091    
?EXIT

=TEST
HELLO, WORLD!
=

```

It helps to make a "chain" file (which is like an MDOS batch file) to
automate this:

```
TEST.CF:
  @SET 800
  DEL TEST.AI
  DEL TEST.RO
  DEL TEST.CM
  @SET 0
  MPL TEST.SA;LO=TEST.AI
  RASM TEST.AI
  RLOAD
  IF=TEST
  BASE
  LOAD=TEST.RO
  LIB=MPLSLIB,MPLULIB
  OBJA=TEST.CM
  MAPF
  EXIT
```

Then you can compile, assemble and link with a single command:

```
  =CHAIN TEST
```

## The language

Here is what I've been able to learn about the language from the information
available.

## Comments

MPL uses C-like comments:

```
/* THIS IS
  A COMMENT */
```

You can not have a comment within a comment.

MPL also has line comments beginning with !:

```
  A=5 ! ASSIGN 5 TO A
```

## Statements

Unlike other versions of PL/I, MPL does not require semicolons at the ends
of statements.

## Character set

Like other software on the Exorciser, MPL only allows uppercase letters,
except in strings.

Identifiers start with a letter and may contain numbers.  No other character
is allowed.

MPL seems to allow long identifiers (I'm not sure how long), but almost all
identifiers end up as labels in the generated assembly code.  RASM is
limited to just 6 character labels, so the same limit applies to MPL.  This is
a severe limitation.

MPL truncates MPL labels to 6 characters, but it does not truncate variable
names.  If you try to use a longer variable name, RASM will give an error.

There is one exception to this: normal pass by reference parameter names do
not end up in the generated assembly, so they can be long.

## Operators

These operators are available in assignment expressions, listed in
precedence order:

1. % or SHIFT
  * `A = B % -1` Shifts B left 1 bit (right side must be a constant)
2. IOR, IEOR, and IAND.  & can be used for IAND.
  * `A = B & C` Bitwise AND.
3. *, /
4. +, -

The expression for `IF` allows these additional operators:

1. LE (or <=), GE (or >=), LT (or <), GT (or >), NE (or #), EQ (or =)
2. NOT
3. AND, OR

## Constants

MPL has integer constants:

```
 I=123
 J=$FF
```

MPL has fixed point decimal constants:

```
 K=1.23
```

MPL has string constants:

```
 STR='HELLO'
 STR='JOE''S STUFF!!'
```

Use two single quotes to include a single quote.  ! is special also, use !!
to get !.  I was hoping ! was some kind of quoting mechanism, but if so I
haven't been able to figure it out.

String constants are limited to just 30 characters (string variables can be
longer).  I have not found any way to concatenate string constants.  Taken
together, these are really terrible limitations.

Unlike C, there is no automatic NUL appended to the ends of string
constants.  When you assign a short string to a long string (string lengths
known at compile time), it is left-justified and the balance is filled with
spaces.  Similarly, when you initialize a string variable and the
initializer is shorter than the string, the balance is filled with spaces.

Immediately there is a problem to write even "Hello, world!".  The provided
library procedure DSPLY takes an address of a carriage-return terminated
string.  How can we provide this carriage return?

One way is to assume that adjacent variable declarations are contiguous in
memory:

```
    DCL STR CHAR(13) INIT('Hello, world!!')
    DCL CR BIN(1) INIT(13)
```

This does work, but seems like bad practice.  You might think that you could
just write the carriage return with code:

```
    DCL STR CHAR(14) INIT('Hello, world!!')

    STR(13) = 13
```

But this is not allowed!  Strings can not be indexed like arrays.  Another
way which does work is to use an array of characters instead of a string:

```
    DCL STR(40) CHAR INIT('H', 'e', 'l', 'l', 'o', ',', ' ', 'w',
                          'o', 'r', 'l', 'd', '!!', 13)

```

It's OK that the initializer is shorter than an array, the balance is left
uninitialized.  Even so, this is kind of awkward to type..

### Named constants

Constants can be named:

```
   DCL ONE CONST(1)
   DCL HELLO CONST('HELLO')
```

## Basic types

MPL has these basic types:

```
  /* A STRING WITH LENGTH 25 */
  DCL S CHAR(25)

  /* A BIT FIELD */
  DCL E BIT	! ONE BIT
  DCL F BIT(1)  ! ONE BIT
  DCL G BIT(3)

  /* AN UNSIGNED 8-BIT INTEGER */
  DCL I         ! ONE BYTE
  DCL J BIN     ! ONE BYTE
  DCL K BIN(1)  ! ONE BYTE

  /* A SIGNED 16-BIT INTEGER */
  DCL Y SIGNED BIN(2)	

  /* AN UNSIGNED ASCII DECIMAL INTEGER, 7 DIGITS */
  DCL Z DEC(7)

  /* A SIGNED ASCII DECIMAL INTEGER, 8 DIGITS */
  DCL Z SIGNED DEC(8)

  /* 7 DIGIT ASCII DECIMAL WITH DECIMAL POINT ASSUMED 2 PLACES FROM RIGHT */
  DCL MONEY DEC(7,2) ! 00000.00 - 99999.99
```

### BIT type

BITs do not work very well.  You can assign a constant to and compare single
bits with constants (as in `IF E = 1 THEN`).  You can assign constants to
fields larger than 1 bit, but you can not compare them.  You can not assign
an integer to a bit.  You can assign a bit to an integer, but the result is
weird: you will discover that the bits are left-justified in their bytes,
and no proper conversion is done.  It means a 1 bit when converted to an
integer is 128.

Adjacent bits in structures are packed together.  Adjacent bits in stand-alone
declarations like above are not packed: each bit uses 1 byte.

### BIN type

Two binary sizes are supported: BIN(1) and BIN(2)- one byte or two.

If an operator's arguments are both BIN(1)s, then an 8-bit operation is
performed.  If either argument is BIN(2), then a 16-bit operation is
performed.

Integer constants are 8-bits, unless they are above 255, then they are
16-bits.  This is true even if the other argument is signed.

Negating an integer argument does not change its size.  So -255 is the same
as 1 if the other argument is a byte.  -255 if 0xFF01 if the other argument
is 16-bits.

Integer constants are promoted to 16 bits when passed in procedure
arguments.

## Automatic Conversions

Here is a summary of what happens when you assign one basic type to another:

* string = integer: the string is filled with spaces, the integer is
converted to ASCII decimal and it right justified in the string.

* string = decimal: the ASCII decimal is right justified in the string and
the balance to the left is space-filled.  If there is an assumed decimal point,
a decimal point is inserted into the string so that the string version of the
number is one larger than the decimal version.

* integer = decimal: the ASCII decimal is converted to an integer. 
Unfortunately, the decimal point is ignored during this conversion- the
entire decimal number is treated as an integer even if there is an assumed
decimal point.

* decimal = decimal: it seems to truncate properly.  For example if you
convert a DEC(3,9) to a DEC(3), just the integer part is taken.

* decimal = integer: the integer is converted to ASCII decimal.  The decimal point is ignored during
this conversion.

* integer = string: not allowed.

* string = integer: not allowed.

* bit = integer: not allowed except for integer constants.  Even this does
not work for the case of a larger than a single bit field in a structure.

* integer = bit: the byte containing the bit is directly loaded into the
integer.

## Complex Types

### Arrays

MPL supports multi-dimensional arrays, up to 3 dimensions.  Two dimensional
arrays are stored in row-major order in memory (each row is stored in
contiguous memory).  Initializers for arrays are row-major also.

Array indexing is 1 based (the first element is located at index 1):

```
    DCL ARY(2, 3) BIN INIT(1, 2, 3, /* FIRST ROW */
                           4, 5, 6) /* SECOND ROW */

    DCL I, J

    DCL NUM DEC(3)
    DCL CR INIT(13)

    DO J = 1 TO 2
      DO I = 1 TO 3
        NUM = ARY(J, I) /* CONVERT TO ASCII DECIMAL */
        CALL DSPLY<1,1,ADDR(NUM)>
        END
      END
        
```

Whole arrays can not be assigned.  If you try it, only the first element is
copied.  I find this surprising since MPL will copy strings and large ASCII
decimal numbers.

### Pointers and Structures

MPL has pointers and structures:

```
    DCL BUF(20) CHAR    /* AN ARRAY OF CHARACTERS */
    DCL P BIN(2)        /* THE POINTER */
    DCL Z CHAR BASED    /* DEFINE TYPE IT'S POINTING TO */
    DCL Q CHAR

    P = ADDR(BUF)       /* SET POINTER */
    P->Z = 'A'		/* WRITE */
    P = P + 1		/* INCREMENT POINTER */
    Q = P->Z		/* READ */
```

Pointers do not have a special type: just use BIN(2).  It means the language
will not automatically take into account the size of the object you're
pointing to when performing pointer arithmetic.

The only dereference operator is -> and a name from a __BASED__ declaration
must be supplied on the right side (even if you are not accessing a
structure) to indicate the type.  It's weird, but it works.

I found a bug involving pointers:  The left side of -> must not be
a by-reference parameter.  A work-around is to copy the parameter to a
another variable.

__ADDR__ returns the address of the variable in its argument.  Only variable
names are allowed for __ADDR__, not complex expressions.  Therefore there is
no direct way to get the address of a particular array item.

__BASED__ means you are defining a structure or a simple type which can be
deferenced with ->.  Structures are declared like this:

```
    DCL 1 MYSTRUCT BASED,
          2 X BIN,
          2 Y,
            3 I BIN,
            3 J BIN,
          2 Z BIN(2)

    P->X = 1
    P->I = 2
    P->J = 3
    P->Z = 512
          
```

The structure is 5 bytes in this case.  Notice that you provide the level
number for each item in the structure, similar to COBOL.  For example X is
at level 2.  Y is also at level 2, but it's a sub-structure including I and
J at level 3.

## Declarations

Declarations can be placed anywhere- within procedures at the top or in the
middle of code, or outside of procedures.  The only restriction is that
symbols must be declared before they are used.

The default type is BIN(1) (one byte unsigned binary) if none is specified.
This is a little odd since integer constants are two bytes when passed as arguments
with CALL.

There are a few more things to know about declarations:

### Scoping? What's that?

There is no scoping!  All symbols in a file are in the same
namespace no matter where they are.  It makes no difference if a variable
is declared inside or outside of a block or inside or outside of a procedure.
Structure members are also in this same namespace.

It gets worse! The symbols are passed directly to the assembler, so you need
to worry about which symbols have special meaning to the assembler.  For
example, A, B and X can not be used since they're 6800 register names.
The assembler only supports 6 character names, so you are doomed to use short
obscure names.

### GLOBAL and EXTERNAL

Declarations marked __GLOBAL__ are visible to other modules (even if they
are located within a procedure).  They are marked in the assembler with the
__XDEF__ directive so that the symbol is visible to the linker.

To use an externally defined declaration, put a copy of the declaration in
your file, but replace the __GLOBAL__ with __EXTERNAL__.  No space will be
allocated for the declaration, instead an __XREF__ directive is emitted to
inform the linker to find the corresponding __XDEF__.

All procedures are marked with __XDEF__ except for sub-procedures
(procedures define within other procedures)- they are local to the file. 
All CALLs emit an __XREF__ in case their target is external.

### FORTRAN named common blocks

A declaration can be labeled to use a named common block.  All declarations
in the same common block use the same space.

```
   FRED: DCL Y BIN
   FRED: DCL Z BIN(2)
```

Y and Z above use the same space.

### Sections

RASM and RLOAD understand four sections:

1. BSCT - zero page (base page) data section
2. CSCT - FORTRAN "blank common" section
3. DSCT - data section
4. PSCT - code (program) section

MPL normally puts code into PSCT and data into DSCT.  Declarations can be
forced into other sections by specifying the section name right after the
DCL statement:

```
    DCL PSCT Z BIN(2) INIT(12345)
    DCL BSCT Y BIN(2)
```

Z will be placed in the code section (perhaps in ROM).  Y will be paced in the
zero page.  Y may not have initial data (there is no way to load it).

### Force location of a declaration

DEF (or DEFINED) can be used to provide the absolute address of the memory
to be used by a declartion.  This feature can be used to locate hardware
devices:

```
	DCL ACIA0 DEF $FCF4 ! SERIAL PORT STATUS BITS
	DCL ACIA1 DEF $FCF5 ! SERIAL PORT DATA BYTE
```

DEF can also be used to create a kind of union: to indicate that one
decaration does not allocate space, but instead uses the space provided
by another declaration.

```
    DCL 1 FRED,
          2 I BIN,
          2 J BIN

    DCL BOB BIN(2) DEF FRED
```

Structure FRED containing two one-byte members I and J uses the same space
as two-byte integer BOB.  Note that it is not allowed for the size of BOB to
be larger than the size of FRED.

## Procedures

MPL does not have user defined functions.  Instead it has procedures which
can be CALLed.

As in PL/I, one procedure should be marked with `OPTIONS (MAIN)` to indicate
the program's starting point.  MAIN is a keyword, so you can not label the
main procedure with MAIN.

Arguments to procedures are normally passed by reference.  Arguments can not
be complex expressions.  Arguments are supposed to be variables, but
constants are permitted also.  In this case, space is allocated for the
constant so that it can be passed by reference.

By-reference arguments have some further limitations that I've discovered:

 * You can not use a pass by reference argument as a parameter for another subroutine call.  You will get a compile error.

 * You can not apply the dereference operator to pass by reference arguments.  There is no error, but the result is wrong.

```
   /* PASS BY REFERENCE */

    ADDEM: PROC(A,B,C)
      DCL A BIN(2), B BIN(2), C BIN(2)
      A = B + C
    END

    DCL RESULT BIN(2)
    CALL ADDEM(RESULT,1,2)
```

The generated code looks like this:

```
* MADE BY MPL  2.1
       OPT  REL		* Relocatable..
*  10 ADDEM: PROC(A,B,C)
       XDEF ADDEM	* Allow ADDEM to be accessed outside of this module
ADDEM  EQU  *
*  20   DCL A BIN(2), B BIN(2), C BIN(2)
*  30   A = B + C
       TSX		* Get stack pointer into X
       LDX  0,X		* Get return address into X
       LDX  4,X		* Get address of B into X
       LDAA 1,X		* Get B into registers A / B
       LDAB 0,X
       TSX
       LDX  0,X
       LDX  6,X
       ADDA 1,X		* Add C into registers A / B
       ADCB 0,X
       TSX
       LDX  0,X
       LDX  2,X
       STAB 0,X		* Save result into A
       STAA 1,X
*  40   END
*  50 CALLIT: PROC
       XDEF CALLIT
CALLIT EQU  *
*  60   DCL RESULT BIN(2)
       DSCT		* Data section
RESULT RMB  2		* Space for result
       PSCT		* Program section
*  70   CALL ADDEM(RESULT,3,4)
       JSR  ADDEM	* Call addem..
       BRA  .002	* Skip argument list
       FDB  RESULT	* Address of A
       FDB  .392	* Address of B
       FDB  .399	* Address of C
*  80   END
.002   EQU  *
.399   FDB  4		* Argument C
.392   FDB  3		* Argument B
T$     COMM DSCT
.T     RMB     10
       END
```

But there is a special form to force arguments and return values into the
6800 registers A, B and X.  When this form is used, CALLs are pass by value:

```
   /* PASS BY VALUE IN REGISTERS */

    ADDEM: PROC<I,J,K>
      DCL I, J, K BIN(2)
      K = I + J + K
      RETURN<,,K>
    END

    DCL RESULT BIN
    CALL ADDEM<1,2,3> GIVING<,,RESULT>
```

The generated code looks like this:

```
* MADE BY MPL  2.1
       OPT  REL
*  10 ADDEM: PROC<I,J,K>
       XDEF ADDEM
ADDEM  EQU  *
       STAA I
       STAB J
       STX  K
*  20  DCL I, J, K BIN(2)
       DSCT
I      RMB  1
J      RMB  1
K      RMB  2
       PSCT
*  30  K = I + J + K
       LDAA I
       ADDA J
       CLRB
       ADDA K+1
       ADCB K
       STAB K
       STAA K+1
*  40  RETURN<,,K>
       LDX  K
       RTS
*  50  END
*  60 TRY: PROC
       XDEF TRY
TRY    EQU  *
*  70  DCL RESULT BIN(2)
       DSCT
RESULT RMB  2
       PSCT
*  80  CALL ADDEM<1,2,3> GIVING<,,RESULT>
       LDAA #1
       LDAB #2
       LDX  #3
       JSR  ADDEM
       STX  RESULT
*  90  END
T$     COMM DSCT
.T     RMB     11
       END
```

### RETURN statement

The __RETURN__ statement is required at the end of each procedure, otherwise
the RTS instruction will be missing.  You can see this in the by
reference version of ADDEM above.  You are free to __RETURN__ at any other
point of the procedure.

### Embedded assembly

Lines beginning with __$__ are passed directy to the assembler.  As far as I
can tell you are not required to maintain register contents between
statements, so it's safe for the assemby code trash the A, B and X
registers.

Embedded assembly can access procedure arguments- just follow the generated
code examples above.

### No recursion

It should come as no surprise that recursion is not allowed since variables
are not stored on the stack.

Procedures can be defined in other procedures.  This doesn't help you very
much since there is no scoping.  Sub-procedures are local to the file at
least (like C static variables).

## Structured programming

MPL supports typical structured programming constructs, such as loops.

I've discovered that the DO loops can not use complex expressions.  Also negative numbers
can not be used.  Finally, you can only use binary numbers.  Decimal is not allowed.

```
    DCL ARY(10), SUM, PROD, I

    DO I = 1 TO 10
      ARY(I) = I
      END

    DO I = 1 TO 10 BY 2
      ARY(I) = ARY(I) - 1
      END

    I = 10
    PROD = 1
    DO WHILE I # 0 /* # means 'not equal' */
      PROD = PROD * ARY(I)
      I = I - 1
      END
```

DO and END can be used to make a block which can be placed anywhere you
expect to have a statement:

```
    IF I EQ B THEN
        CALL FRED(I)
    ELSE
        CALL BOB(I)

    IF I NE B THEN
      DO
        I = I + 1
        CALL FRED(I)
      END
    ELSE
      DO
        I = I + 3
        CALL FRED(I)
      END
```

MPL also has labels and GOTO, and computed GOTO:

```
    GO TO SKIP
  FIRST:
    I = I + 1
  SKIP:
    CALL DOIT(I)

    GOTO (A,B,C,D), I
  A: J = I * 5
    GOTO DONE
  B: J = I * 6
    GOTO DONE
  C: J = I * 7
    GOTO DONE
  D: J = I * 8
  DONE:
    CALL PRINT(J)

```

## Run Time Library

There are only a few procedures provided:

```
  CALL DSPLY<,,ptr> ! Display carriage return terminated string to console

  CALL KEYIN<,len,ptr> ! Enter a string of a specified length from console
  
  CALL MDOS ! Exit program, restore MDOS command interpreter

  CALL PRINT ! Write to printer?

  CALL PUSH2<,,X> ! Push X on stack

  CALL PULL2 GIVING<,,X> ! Pop X from stack
```

Here is an example program showing some of these procedures.

``` C

  /* COMPUTE FACTORIAL */
  FACT: PROC(N, P)
    DCL N BIN(2)
    DCL P BIN(2)
    P = 1
    DO WHILE N NE 1
      P = P * N
      N = N - 1
      END
    END

  /* COMPUTE FACTORIAL OF ENTERED NUMBERS */
  TRYIT: PROC OPTIONS(MAIN)
    DCL N BIN(2)
    DCL M BIN(2)
    DCL P BIN(2)
    DCL OD DEC(5)
    DCL CR1 BIN INIT(13)
    DCL ID DEC(2)
    DCL MSG CHAR(20) INIT('ENTER 2 DIGIT NUMBER')
    DCL CR BIN INIT(13)
    
    /* DISPLAY WELCOME MESSAGE */
    CALL DSPLY<1,1,ADDR(MSG)>
   
    M=1
    DO WHILE M NE 0
      /* ENTER NUMBER */
      CALL KEYIN<1,2,ADDR(ID)>
      /* CONVERT TO BINARY */
      M = ID
      N = M

      /* COMPUTE FACTORIAL */
      CALL FACT(N, P)

      /* PRINT RESULT */
      OD = P
      CALL DSPLY<1,1,ADDR(OD)>
      END
    /* BYE */
    CALL MDOS
    END

```

## Support functions

MPLSLIB contains language support subroutines for operations which are not
native to the 6800.  I have figured out some of them:

* .F0F  Copy string from T to X.  If source is short, fill with space.  Left justify
  * .T has source
  * X has dest
  * B has source size
  * A has no. spaces to fill
  *  A + B should be same size as dest

* .F4A 8-BIT MULTIPLY
  *  A * B -> A

* .F0A Copy fixed width ASCII decimal
  * .T has source
  * X has dest
  * B has length

* .F01 Convert decimal number to integer
  * .T has source
  * X has dest
  * A has source length
  * B has dest length

* .f02 convert integer to string (space filled, right justified)
  * .T has source
  * X has dest
  * A has source length
  * B has dest length

* .F03 copy decimal to string
  * .T has source
  * X has dest   
  * A source length
  * B dest length 

__.T__ is a common (shared) area used for arguments for some of these
functions.  __.T__ is also used for expression temporaries.

The module with the main procedure allocates space for the stack,
called __.S__.

## Final example

Here is a program which computes 64-point discrete fourier transform of a square wave.  It prints
the time domain input and frequency domain output using ASCII graphics.  This is an example of how
you can write a numeric program using MPL's fixed point decimal support.

```
DCL RSTR CHAR(5) INIT('REAL ')
DCL RSTRZ INIT(0)

DCL ISTR CHAR(5) INIT('IMAG ')
DCL ISTRZ INIT(0)

/* Print single character to console */

PUTCH: PROC<CHOUT,,>
  DCL CHOUT
$ JSR $F018
  RETURN
  END

/* Print $ or NUL terminated string to console */
/* If string ends with $, print newline */

PUTS: PROC<,,PTSSTR>
  DCL PTSSTR BIN(2)
  DCL RDC BIN BASED
  DO WHILE PTSSTR->RDC NE $24 AND PTSSTR->RDC NE 0
    CALL PUTCH<PTSSTR->RDC>
    PTSSTR = PTSSTR + 1
    END
  IF PTSSTR->RDC = $24 THEN CALL PUTCH<10,,>
  RETURN
  END

/* Print string then number */

PRDEC: PROC(PSTR,DVAL)
  DCL PSTR BIN(2)
  DCL DVAL SIGNED DEC(9,6)
  DCL BUF CHAR(10)
  DCL TERM INIT($24)
  CALL PUTS<,,PSTR>
  BUF = DVAL
  CALL PUTS<,,ADDR(BUF)>
  RETURN
  END

/* Print a graph */

SHOW: PROC(R, I)
    DCL R SIGNED DEC(9,6)
    DCL I SIGNED DEC(9,6)
    DCL RA SIGNED DEC(9,6)
    DCL IA SIGNED DEC(9,6)
    DCL HM SIGNED DEC(3)
    DCL Q SIGNED BIN(2)
    DCL QS CHAR(20)
    DCL QCR INIT($24)
    RA = R ! Copy args so we can pass them again
    IA = I
    ! CALL PRDEC(ADDR(RSTR), RA)
    ! CALL PRDEC(ADDR(ISTR), IA)
    RA = RA * 20.0 + 40.0
    HM = RA ! Truncate
    Q = HM ! Convert to binary
    ! QS = Q
    ! CALL PUTS<,,ADDR(QS)>
    IF Q > 79 THEN Q = 79
    /* Print the graph */
    DO WHILE Q > 0
        CALL PUTCH<$20,,>
        Q = Q - 1
        END
    CALL PUTCH<$2A,,>
    CALL PUTCH<10,,>
    RETURN
    END

/* Discrete fourier transform */

DFT: PROC(FR, FI)
    /* Real and imaginary parts of input data */
    DCL FR(64) SIGNED DEC(9,6)
    DCL FI(64) SIGNED DEC(9,6)

    /* omega = e ^ (-2*pi/n) */
    DCL ROMEGA SIGNED DEC(9,6) INIT(0.995184)  ! cos(-2*pi/n)
    DCL IOMEGA SIGNED DEC(9,6) INIT(-0.098017) ! sin(-2*pi/n)

    /* Compute nomega = omega^(n/2) */
    DCL NROMEG SIGNED DEC(9,6) INIT(1.0)
    DCL NIOMEG SIGNED DEC(9,6) INIT(0.0)
    DCL TR SIGNED DEC(9,6)
    DCL TI SIGNED DEC(9,6)
    DCL N SIGNED BIN
    DO N = 0 TO 32
        TR = NROMEG * ROMEGA - NIOMEG * IOMEGA
        TI = NIOMEG * ROMEGA + NROMEG * IOMEGA
        NROMEG = TR
        NIOMEG = TI
        END

    ! CALL PRDEC(ADDR(RSTR), NROMEG)
    ! CALL PRDEC(ADDR(ISTR), NIOMEG)

    /* Transform */
    DO N = 1 TO 64
        DCL RACCU SIGNED DEC(9,6)
        DCL IACCU SIGNED DEC(9,6)
        DCL Z
        DCL ZN
        RACCU = 0.0
        IACCU = 0.0
        DO Z = 1 TO 64
            /* ACCU = ACCU * NOMEGA + F */
            ZN = 65 - Z
            TR = RACCU * NROMEG - IACCU * NIOMEG + FR(ZN)
            TI = IACCU * NROMEG + RACCU * NIOMEG + FI(ZN)
            RACCU = TR
            IACCU = TI
            END
        CALL SHOW(RACCU, IACCU)
        RACCU = RACCU / 64.0
        IACCU = IACCU / 64.0

        
        /* NOMEGA *= OMEGA */
        TR = NROMEG * ROMEGA - NIOMEG * IOMEGA
        TI = NIOMEG * ROMEGA + NROMEG * IOMEGA
        NROMEG = TR
        NIOMEG = TI
        END
    RETURN
    END

/* Main */

TRY: PROC OPTIONS(MAIN)
    DCL REAL(64) SIGNED DEC(9,6)
    DCL IMAG(64) SIGNED DEC(9,6)
    DCL XR SIGNED DEC(9,6)
    DCL XI SIGNED DEC(9,6)
    DCL Y
    DCL INPSTR CHAR(7) INIT('Input:$')
    CALL PUTS<,,ADDR(INPSTR)>
    /* Initialize with a square wave */
    DO Y = 1 TO 64
        IF ((Y-1)&4) = 0 THEN
            REAL(Y) = 1.0
        ELSE
            REAL(Y) = -1.0
        IMAG(Y) = 0.0
        XR = REAL(Y)
        XI = IMAG(Y)
        CALL SHOW(XR, XI)
        END
    /* Compute and print DFT */
    DCL OUTSTR CHAR(8) INIT('Output:$')
    CALL PUTS<,,ADDR(OUTSTR)>
    CALL DFT(REAL, IMAG)
    CALL MDOS
    END
```

Here is the result when you run the program.  Rotate left 90 degrees to the see the graph properly.

```
=DFT
Input:
                                                            *
                                                            *
                                                            *
                                                            *
                    *
                    *
                    *
                    *
                                                            *
                                                            *
                                                            *
                                                            *
                    *
                    *
                    *
                    *
                                                            *
                                                            *
                                                            *
                                                            *
                    *
                    *
                    *
                    *
                                                            *
                                                            *
                                                            *
                                                            *
                    *
                    *
                    *
                    *
                                                            *
                                                            *
                                                            *
                                                            *
                    *
                    *
                    *
                    *
                                                            *
                                                            *
                                                            *
                                                            *
                    *
                    *
                    *
                    *
                                                            *
                                                            *
                                                            *
                                                            *
                    *
                    *
                    *
                    *
                                                            *
                                                            *
                                                            *
                                                            *
                    *
                    *
                    *
                    *
Output:
                                        *
                                        *
                                        *
                                        *
                                        *
                                        *
                                       *
                                                           *
                                        *
                                        *
                                        *
                                        *
                                        *
                                        *
                                        *
                                        *
                                       *
                                       *
                                       *
                                       *
                                       *
                                       *
                                       *
                                                          *
                                        *
                                        *
                                        *
                                        *
                                        *
                                        *
                                        *
                                        *
                                        *
                                        *
                                        *
                                        *
                                        *
                                        *
                                         *
                                                           *
                                       *
                                       *
                                       *
                                       *
                                       *
                                       *
                                       *
                                        *
                                        *
                                        *
                                        *
                                        *
                                        *
                                        *
                                        *
                                                          *
                                       *
                                       *
                                       *
                                       *
                                       *
                                       *
                                       *
                                        *
=
```
