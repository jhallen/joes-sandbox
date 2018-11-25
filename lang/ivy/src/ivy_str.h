/* String */

struct ivy_string {
	Ivy_string *next_free;
	char *s;		/* Pointer to string */
	int len;		/* Size of string */
};

Ivy_string *ivy_alloc_str(char *s, size_t len);
void ivy_mark_str(Ivy_string *str);
void ivy_mark_protected_strs();
void ivy_sweep_strs();
void ivy_clear_protected_strs();
void ivy_protect_str(Ivy_string *str);
