int skipws(char **at_p);
int match_word(char **at_p, char *word);
int parse_word(char **at_p, char *buf);
int parse_hex(char **at_p, int *hex);
int parse_hex1(char **at_p, int *hex);
int parse_hex2(char **at_p, int *hex);
int parse_hex4(char **at_p, int *hex);
int parse_dec(char **at_p, int *dec);

int jgetline(FILE *f, char *buf);
int hatoi(unsigned char *buf);
void hd(FILE *out, unsigned char *mem, int start, int len);
int fields(char *buf, char *words[]);
char *jstrcpy(char *d, char *s);

void sim_termios(void);
void save_termios(void);
void restore_termios(void);
void sig_termios(void);
void nosig_termios(void);
int jstricmp(char *d, char *s);
