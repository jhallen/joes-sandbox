#include <stdio.h>
#include "re.h"

int dfa_match(struct dfa *dfa, char *s)
{
	while (dfa && *s) {
		dfa = dfa->nxt[*(unsigned char *)s++];
	}
	if (dfa && dfa->eof)
		return 1;
	else
		return 0;
}

int main(int argc,char *argv[])
{
	struct state *st;
	struct dfa *dfa;
	if (argc < 2 || argc > 3) {
		printf("re 'regular-expression' [string-to-check]\n");
		return -1;
	}
//	printf("Parse...\n");
	st = parse((unsigned char *)argv[1]);
	printf("\nHere is NFA:\n");
	show(st);
//	printf("\nConvert NFA to DFA...\n");
	dfa = nfa_to_dfa(st);
//	printf("\nHere is state machine:\n\n");
	show_dfa(dfa);
	if (argc == 3) {
		printf("Try to match string:\n");
		printf("match=%s\n",dfa_match(dfa,argv[2]) ? "yes": "no");
	}
	return 0;
}
