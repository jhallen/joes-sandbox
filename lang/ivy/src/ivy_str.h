/* String */

struct str {
	Str *next_free;
	char *s;		/* Pointer to string */
	int len;		/* Size of string */
};

Str *alloc_str(char *s, size_t len);
void mark_str(Str *str);
void mark_protected_strs();
void sweep_strs();
void clear_protected_strs();
void protect_str(Str *str);
