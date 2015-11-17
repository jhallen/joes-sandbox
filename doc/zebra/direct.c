#include <stdio.h>
#include <string.h>
#include "zstr.h"

struct {
	char *code;
	char *plant;
	char *variety;
	char *output;
} jindex[1024];
int indexi = 0;

int main(int argc, char *argv[])
{
	char buf[4096];
	char *ptr[5];
	FILE *f;
	int i;
	int w = 0;
	sprintf(buf, "./form %s", argv[1]);
	f = popen(buf, "r");
	while (fgets(buf, 4095, f)) {
		fields(buf, ptr, ':');
		jindex[indexi].code = strdup(ptr[0]);
		fprintf(stderr, "%s ", jindex[indexi].code);
		w += strlen(jindex[indexi].code) + 1;
		if (w >= 70)
			fprintf(stderr, "\n"), w = 0;
		jindex[indexi].plant = strdup(ptr[1]);
		jindex[indexi].variety = strdup(ptr[2]);
		fgets(buf, 4095, f);
		jindex[indexi].output = strdup(buf);
		++indexi;
	}
	if (w)
		fprintf(stderr, "\n");
	pclose(f);
      tt:
	fprintf(stderr, "Icode: ");
	if (!gets(buf))
		return;
	if (!buf[0])
		return;
	for (i = 0; i != indexi; ++i)
		if (!strcmp(buf, jindex[i].code)) {
			int n;
			fprintf(stderr, "%s %s\n", jindex[i].plant,
				jindex[i].variety);
		      t1:
			fprintf(stderr, "Amount: ");
			if (!gets(buf))
				return;
			n = 0;
			sscanf(buf, "%d", &n);
			if (n) {
				printf("^XA^PQ%d", n);
				printf("%s", jindex[i].output);
				goto t1;
			} else
				goto tt;
		}
	fprintf(stderr, "Not found\n");
	goto tt;
}
