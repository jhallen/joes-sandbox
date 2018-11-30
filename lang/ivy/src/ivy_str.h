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

#define IVY_STR_PROTECT_SIZE 128

extern struct ivy_str_protect {
	struct ivy_str_protect *next;
	Ivy_string *list[IVY_STR_PROTECT_SIZE];
} *ivy_str_protect_list;

extern struct ivy_str_protect *ivy_str_protect_ptr;
extern int ivy_str_protect_idx;

static inline void ivy_clear_protected_strs()
{
	ivy_str_protect_ptr = ivy_str_protect_list;
	ivy_str_protect_idx = 0;
}
void ivy_protect_str(Ivy_string *str);
