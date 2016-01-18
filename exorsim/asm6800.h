unsigned assemble(unsigned char *mem, unsigned addr, char *buf);

void show_syms(FILE *f);
void clr_syms(void);
char *find_label(unsigned short val);
