
typedef struct bitset Bitset;

struct bitset {
	int size;
	unsigned char *map;
};

Bitset *mk_bitset(int prealloc);
void rm_bitset(Bitset *set);
void bitset_expand(Bitset *set, int size);
void bitset_set(Bitset *set, int bit);
void bitset_clr(Bitset *set, int bit);
int bitset_tst(Bitset *set, int bit);
int bitset_cmp(Bitset *a, Bitset *b);
Bitset *bitset_dup(Bitset *a);
