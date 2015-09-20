struct token {
	TOKEN *next;
	I type;
	I prec;
	I ass;
	SYM *sym;
	NUM *num;
};

void setscan(C *s);
TOKEN *gettok();
void ungettok(TOKEN *t);
void rmtok(TOKEN *t);
