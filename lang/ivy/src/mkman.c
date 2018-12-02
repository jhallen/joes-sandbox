/* Convert manual into on-line help */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct section {
	struct section *next;
	char *name;
} *sections;

void tolowers(char *d, char *s)
{
	int x;
	for (x = 0; s[x]; ++x)
		if (s[x] >= 'A' && s[x] <= 'Z')
			d[x] = s[x] + 'a' - 'A';
		else
			d[x] = s[x];
	d[x] = 0;
}

struct section *find_section(char *name)
{
	struct section *s;
	for (s = sections; s; s = s->next)
		if (!strcmp(s->name, name)) {
			fprintf(stderr,"Duplicate sections '%s'\n",name);
			exit(-1);
		}
	s = (struct section *)malloc(sizeof(struct section));
	s->name = strdup(name);
	s->next = sections;
	sections = s;
	return s;
}

int main(int argc, char *argv[])
{
	char lower[80];
	char buf[1024];
	struct section *current = 0;
	int count = 0;
	int line = 0;
	printf("static struct section { const char *name; const char *text; } sections[]=\n");
	printf("{\n");
	while(fgets(buf, sizeof(buf) - 1, stdin)) {
		if (strlen(buf) && buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = 0;
		++line;
		/* if (buf[0] == '#' && buf[1] == ' ') {
			if (current)
				printf("  },\n");
			tolowers(lower, buf + 2);
			current=find_section(lower);
			++count;
			printf("  { \"%s\",\n", lower);
		} else */ if (buf[0] == '#' && buf[1] == '#' && buf[2] == ' ') {
			if (current)
				printf("  },\n");
			tolowers(lower, buf + 3);
			current=find_section(lower);
			++count;
			printf("  { \"%s\",\n", lower);
		} /* else if (buf[0] == '#' && buf[1] == '#' && buf[2] == '#' && buf[3] == ' ') {
			if (current)
				printf("  },\n");
			tolowers(lower, buf + 4);
			current=find_section(lower);
			++count;
			printf("  { \"%s\",\n", lower);
		} */ else if (current) {
			int x;
			int col = 0;
			printf("    \"");
			for(x = 0; buf[x]; ++x)
				if (buf[x] == '"')
					printf("\\\""), ++col;
				else if (buf[x] == '\\')
					printf("\\\\"), ++col;
				else if (buf[x] == '\t')
					while(++col & 7)
						printf(" ");
				else
					printf("%c", buf[x]), ++col;
			printf("\\n\"\n");
			if (col >= 79) {
				fprintf(stderr, "Warning: 80 columns exceeded on line %d\n", line);
			}
		}
	}
	if (current)
		printf("  },\n");
	printf("  { 0, 0 }\n");
	printf("};\n");
	fprintf(stderr, "Created %d sections\n", count);
	return 0;
}
