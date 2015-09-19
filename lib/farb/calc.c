/* Tiny little calculator program */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "farb.h"

int precision = 10;

char *err;

struct var {
	char *name;
	int set;
	char *val;
	struct var *next;
	char *(*func)(char *arg);
} *vars = 0;

char *set_precision(char *v)
{
	precision = Int(v);
	return v;
}

struct var *get(char *str)
{
	struct var *v;
	for (v = vars; v; v = v->next)
		if (!strcmp(v->name, str))
			return v;
	v = (struct var *) malloc(sizeof(struct var));
	v->set = 0;
	v->next = vars;
	vars = v;
	v->name = strdup(str);
	v->func = 0;
	return v;
}

char *ptr;
struct var *dumb;

char *expr(int prec, struct var **rtv)
{
	char *x = "0";
	struct var *v = 0;
	while (*ptr == ' ' || *ptr == '\t')
		++ptr;
	if ((*ptr >= 'a' && *ptr <= 'z') || (*ptr >= 'A' && *ptr <= 'Z') || *ptr == '_') {
		char *s = ptr, c;
		while ((*ptr >= 'a' && *ptr <= 'z') || (*ptr >= 'A' && *ptr <= 'Z') || *ptr == '_' || (*ptr >= '0' && *ptr <= '9'))
			++ptr;
		c = *ptr;
		*ptr = 0;
		v = get(s);
		x = v->val;
		*ptr = c;
		if (v->func) {
			while (*ptr == ' ' || *ptr == '\t')
				++ptr;
			if (*ptr == '(') {
				char *arg;
				++ptr;
				arg = expr(0, &dumb);
				if (*ptr == ')')
					++ptr;
				else {
					if (!err)
						err = "Missing )";
				}
				x = v->func(arg);
			} else {
				if (!err)
					err = "Missing argument";
			}
		}
	} else if ((*ptr >= '0' && *ptr <= '9') || *ptr == '.') {
		char *s = ptr;
		while ((*ptr >= '0' && *ptr <= '9') || *ptr == '.' || *ptr == 'e' || *ptr == 'E')
			++ptr;
		x = (char *)malloc(ptr - s + 1);
		x[ptr - s] = 0;
		memcpy(x, s, ptr - s);
	} else if (*ptr == '(') {
		++ptr;
		x = expr(0, &v);
		if (*ptr == ')')
			++ptr;
		else {
			if (!err)
				err = "Missing )";
		}
	} else if (*ptr == '-') {
		++ptr;
		x = Fneg(expr(10, &dumb));
	}
      loop:
	while (*ptr == ' ' || *ptr == '\t')
		++ptr;
/*	if (*ptr == '^' && 6 >= prec) {
		++ptr;
		x = pow(x, expr(6, &dumb));
		goto loop;
	} else */ if (*ptr == '*' && 5 > prec) {
		++ptr;
		x = Fmul(x, expr(5, &dumb));
		goto loop;
	} else if (*ptr == '/' && 5 > prec) {
		++ptr;
		x = Fdiv(x, expr(5, &dumb), precision);
		goto loop;
	} else if (*ptr == '+' && 4 > prec) {
		++ptr;
		x = Fadd(x, expr(4, &dumb));
		goto loop;
	} else if (*ptr == '-' && 4 > prec) {
		++ptr;
		x = Fsub(x, expr(4, &dumb));
		goto loop;
	} else if (*ptr == '=' && 2 >= prec) {
		++ptr;
		x = expr(2, &dumb);
		if (v)
			v->val = x, v->set = 1;
		else {
			if (!err)
				err = "Left side of = is not an l-value";
		}
		goto loop;
	}
	*rtv = v;
	return x;
}

int main(int argc, char *argv[])
{
	char buf[1024];
	int eqn = 0;
	printf("Used prec(25) to set the precision\n");
	get("prec")->func = set_precision;
	while (printf("="), gets(buf)) {
		char *result;
		ptr = buf;
		err = 0;
		while (*ptr == ' ' || *ptr == '\t')
			++ptr;
		if (*ptr && *ptr != '#') {
			struct var *v;
			result = expr(0, &dumb);
			while (*ptr == ' ' || *ptr == '\t')
				++ptr;
			if (*ptr && *ptr != '#') {
				if (!err)
					err = "Extra junk after end of expr";
			}
			if (!err)
				printf("r%d: %s\n", eqn, result);
			else
				printf("%s\n", err);
			sprintf(buf, "r%d", eqn++);
			v = get(buf);
			v->val = result;
			v->set = 1;
		}
	}
	return 0;
}
