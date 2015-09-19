# Arbitrary precision math routines

These routines take ASCII numbers in C strings and return ASCII string
results in malloc blocks.

A sample calculator is provided, compile it as follows:

	cc -o calc calc.c farb.c

	./a.out
	Used prec(25) to set the precision

	=1+2+3+4+5
	r0: 15

	=r0/3
	r1: 5

	=1/3
	r2: 333.3333333e-3

	=prec(250)
	r3: 250

	=1/7
	r4: 142.8571428571428571428571428571428571428571428571428571428571428571428571428571428571428571428571428571428571428571428571428571428571428571428571428571428571428571428571428571428571428571428571428571428571428571428571428571428571428571428571428571428e-3

	=


# Functions provided

## Unsigned arithmetic

These all take numbers of the form: [-]xxxxxx

Leading zeros must be suppressed.  If there is a leading minus sign, it is
ignored.  Leading zeros in the result will be suppressed.

### Unsigned addition: |a|+|b|

If 'n' is true, the result will be negative (even if it is zero).

	char *Uadd(char *a, char *b, int n);

### Unsigned subtraction: |a|-|b|

If 'n' is true, the result will be negative (even if it is zero).  |a| must
be larger or equal to |b| or this routine will go into an infinite loop.

	char *Usub(char *a, char *b, int n);

## Integer arithmetic

These all take numbers of the form [-]xxxxx

Leading zeros must be suppressed.  The result will have leading zeros
suppressed.

### Comparison: 1 if a>b, 0 if a=b, or -1 if a<b

	int Cmp(char *a, char *b);

### Magnitude comparison: 1 if |a|>|b|, 0 if |a|=|b|, or -1 if |a|<|b|

	int Ucmp(char *a, char *b);

### Negate: -a

	char *Neg(char *a);

### Absolute value: |a|

	char *Abs(char *a);

### Addition: a+b

	char *Add(char *a, char *b);

### Subtraction: a-b

	char *Sub(char *a, char *b);

### Multiplication: a*b

	char *Mul(char *a, char *b);

### Division: a/b

	char *Div(char *a, char *b);

### Remainder of a/b.

	char *Mod(char *a, char *b);

### Convert string to integer

	int Int(char *a);

### Convert integer to string

	char *Arb(int a);

### Add n zeros after a, even if 'a' is zero.

	char *Shift(char *a, int n);

## Money functions

These all take numbers in the form: [-]xxx.xxx
The decimal point is optional.  Leading and trailing zeros are permitted.
Commas may be interspersed throughout the number.  Leading dollar signs ($)
and asterics (*) are ignored.

These routines all output at least one digit before and two digits after
the decimal point.

Use the floating point routines for: comparison.

### Generate a money number from an integer and exponant

	char *money(char *s, int exp);

### Add

	char *Madd(char *a, char *b);

### Subtract

	char *Msub(char *a, char *b);

### Negate

	char *Mneg(char *a);

### Absolute value

	char *Mabs(char *a);

### Multiply

	char *Mmul(char *a, char *b);

### Divide

	char *Mdiv(char *a, char *b, int n);

## Floating point functions

These all take a number in the form: [-]xxx.xxx{E|e}[-]xxx
The decimal point and the exponent are optional, but there must be at least
one digit in the mantissa of the number.  Leading and trailing zeros are
acceptable.  No syntax checking is done: the numbers must be in the correct
format.

The exponent is limited to the machine size of an integer.

They each return a number in engineering format.  That is, a number
where the exponent is a multiple of 3 and there are 1 to 3 digits to
the left of the decimal point.  If the exponent is zero, it is not
included.  If there are no digits to the right of the decimal point,
the decimal point it is not included.


### Extract

Convert floating point number into a separate integer and exponent.
The integer is returned as a string, and the exponent is placed at the
address 'expa'.

	char *Extract(char *a, int *expa);

### Eng

Generate a floating point number from an integer and an exponent in
the engineering format.

	char *Eng(char *a, int exp);

### Floating point addition: a+b

	char *Fadd(char *a, char *b);

### Floating point subtraction: a-b

	char *Fsub(char *a, char *b);

### Comparison: 1 if a>b, 0 if a==b, or -1 if a<b

	int Fcmp(char *a, char *b);

### Magnitude comparison: 1 if |a|>|b|, 0 if |a|==|b|, or -1 if |a|<|b|

	int Fucmp(char *a, char *b);

### Negate: -a

	char *Fneg(char *a);

### Absolute value: |a|

	char *Fabs(char *);

### Floating point multiplication: a*b

	char *Fmul(char *a, char *b);

### Floating point division: a/b

At least n digits are generated.  No rounding takes place: the
number is simply truncated.

	char *Fdiv(char *a, char *b, int n);

### Truncate and digits after the decimal point

	char *Ftrunc(char *a);

### FtoArb

Truncate digits after the decimal point and add enough zeros so that the
exponent does not need to be generated (I.E., converts the number to an
arbitrary length integer).

	char *FtoArb(char *a);

### Convert to a double

	double Fdouble(char *a);

### Convert a double to a string

	char *Farb(double n);
