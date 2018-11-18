void clear_protected();
void collect();
Val *mark_val(Val *val);

extern int alloc_count; /* Number of times an allocation function has been called */
#define GC_COUNT 10240 /* Collect garbage after this many calls */
