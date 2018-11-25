void ivy_clear_protected();
void ivy_collect();
Ivy_val *ivy_mark_val(Ivy_val *val);

extern int ivy_alloc_count; /* Number of times an allocation function has been called */
#define GC_COUNT 10240 /* Collect garbage after this many calls */
