#include <stdio.h>
#include <stdlib.h>
#include "yytab.h"

/* The value of a token or nonterminal */

struct value {
	/* Starting and ending column where terminal/nonterminal appeared */
	int start, end;

	/* Numeric value of it */
	double val;
};

/* Error flag */
char *err = 0;
int errstart, errend;

/* Function to construct a value */

struct value *cons(int start, int end, double val)
{
	struct value *v = (struct value *) malloc(sizeof(struct value));
	v->start = start;
	v->end = end;
	v->val = val;
	return v;
}

/* Function to free a value */

void rm(struct value *v)
{
	if (v)
		free(v);
}

/* Reduction functions */

yyVAL doadd(yyVAL left, yyVAL op, yyVAL right)
{
	left->end = right->end;	/* Propagate start/end */

	left->val += right->val;	/* Operate */

	rm(right);
	rm(op);			/* Free */
	return left;
}

yyVAL dosub(yyVAL left, yyVAL op, yyVAL right)
{
	left->end = right->end;	/* Propagate start/end */

	left->val -= right->val;	/* Operate */

	rm(right);
	rm(op);			/* Free */
	return left;
}

yyVAL domul(yyVAL left, yyVAL op, yyVAL right)
{
	left->end = right->end;	/* Propagate start/end */

	left->val *= right->val;	/* Operate */

	rm(right);
	rm(op);			/* Free */
	return left;
}

yyVAL dodiv(yyVAL left, yyVAL op, yyVAL right)
{
	left->end = right->end;	/* Propagate start/end */

	if (right->val == 0.0) {
		err = "Divide by zero:";
		errstart = right->start;
		errend = right->end;
	} else
		left->val /= right->val;	/* Operate */

	rm(right);
	rm(op);			/* Free */
	return left;
}

yyVAL doneg(yyVAL op, yyVAL right)
{
	right->start = op->start;

	right->val = -right->val;

	rm(op);
	return right;
}

yyVAL doparen(yyVAL lp, yyVAL exp, yyVAL rp)
{
	exp->start = lp->start;
	exp->end = rp->end;

	rm(lp);
	rm(rp);
	return exp;
}

yyVAL pass(yyVAL exp)
{
	return exp;
}

yyVAL result(yyVAL e)
{
	if (!err) {
		printf("= %g\n", e->val);
		rm(e);
	}
	return 0;
}

/* Input buffer and stack */

char *s;
int col;
struct yystack *stack;

/* Conflict resolution function */

int decide(yyVAL lookahead)
{
	/* Value from top of stack */
	yyVAL top = stack->stack[stack->sp].value;

	/* Skip whitespace */
	while (s[++col] == ' ');

	if (lookahead->start - top->end <= col - lookahead->end) {	/* If '-' is closer to thing on left */
		--col;
		return 0;	/* Shift */
	} else {		/* If '-' is closer to thing on right */
		--col;
		return 1;	/* Reduce */
	}
}

/* Scanner */

int main(int argc, char *argv[])
{
	int stat;

	if (argc != 2)
		fprintf(stderr, "calc \'expressions\'\n"), exit(1);

	stack = yymkstk();
	s = argv[1];

	for (col = 0;; ++col) {
		switch (s[col]) {
		case '+':
			stat =
			    yyparse(stack, add, cons(col, col + 1, 0.0));
			break;
		case '-':
			stat =
			    yyparse(stack, sub, cons(col, col + 1, 0.0));
			break;
		case '*':
			stat =
			    yyparse(stack, mul, cons(col, col + 1, 0.0));
			break;
		case '/':
			stat =
			    yyparse(stack, div, cons(col, col + 1, 0.0));
			break;

		case '(':
			stat =
			    yyparse(stack, lparen,
				    cons(col, col + 1, 0.0));
			break;
		case ')':
			stat =
			    yyparse(stack, rparen,
				    cons(col, col + 1, 0.0));
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
		case '.':
			{
				int d = col;
				double v = 0.0;
				sscanf(s + col, "%lg", &v);
				while (s[col] >= '0' && s[col] <= '9'
				       || s[col] == '.')
					++col;
				stat =
				    yyparse(stack, constant,
					    cons(d, col, v));
				--col;
				break;
			}

		case 0:
			stat = yyparse(stack, yyEND, cons(col, col, 0.0));
			break;

		default:
			stat = 1;
			break;
		}
		if (stat == -1) {
			int y;
			printf("Syntax error:\n%s\n", s);
			for (y = 0; y < col; ++y)
				printf("-");
			printf("^\n");
			return 1;
		} else if (err) {
			int y;
			printf("%s\n%s\n", err, s);
			for (y = 0; y < errstart; ++y)
				printf(" ");
			for (; y < errend; ++y)
				printf("-");
			printf("\n");
			return 1;
		} else if (stat == 0)
			return 0;
	}
}
