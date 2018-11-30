static inline void ivy_clear_protected()
{
	ivy_clear_protected_objs();
	ivy_clear_protected_strs();
}
void ivy_collect();
Ivy_val *ivy_mark_val(Ivy_val *val);

extern int ivy_alloc_count; /* Number of times an allocation function has been called */
#define GC_COUNT 999 /* Collect garbage after this many calls */
