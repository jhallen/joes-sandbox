#include <stdio.h>
#include <stdlib.h>
#include "yytab.h"

/* Reduction functions */

yyVAL doadd(yyVAL left, yyVAL op, yyVAL right)
{
	return left + right;
}

yyVAL dosub(yyVAL left, yyVAL op, yyVAL right)
{
	return left - right;
}

yyVAL domul(yyVAL left, yyVAL op, yyVAL right)
{
	return left * right;
}

yyVAL dodiv(yyVAL left, yyVAL op, yyVAL right)
{
	return left / right;
}

yyVAL result(yyVAL e)
{
	printf("= %d\n", e);
	return 0;
}

yyVAL doparen(yyVAL lp, yyVAL exp, yyVAL rp)
{
	return exp;
}

yyVAL pass0(yyVAL exp)
{
	return exp;
}

yyVAL doneg(yyVAL op, yyVAL right)
{
	return -right;
}

/* Main: a simple scanner */

int main(int argc, char *argv[])
{
	char *s;
	struct yystack *stack = yymkstk();

	if (argc != 2)
		fprintf(stderr, "calc \'expressions\'\n"), exit(1);

	for (s = argv[1];; ++s)
		switch (*s) {
		case '+':
			yyparse(stack, add, 0);
			break;
		case '-':
			yyparse(stack, sub, 0);
			break;
		case '*':
			yyparse(stack, mul, 0);
			break;
		case '/':
			yyparse(stack, div, 0);
			break;

		case '(':
			yyparse(stack, lparen, 0);
			break;
		case ')':
			yyparse(stack, rparen, 0);
			break;

		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			{
				int d = 0;
				do
					d = d * 10 + *s++ - '0';
				while (*s >= '0' && *s <= '9');
				--s;
				yyparse(stack, constant, d);
				break;
			}

		case 0:
			yyparse(stack, yyEND, 0);
			return 0;
		}
}
