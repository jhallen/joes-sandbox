/*
 *	Simple hash table
 *	Copyright
 *		(C) 1992 Joseph H. Allen
 *
 *	This file is part of JOE (Joe's Own Editor)
 */

typedef struct entry HENTRY;
typedef struct hash HASH;

struct entry {
	char	*name;
	HENTRY	*next;
	void	*val;
};

struct hash {
	int	len;
	HENTRY	**tab;
};

unsigned long hash(char *s);
HASH *htmk(int len);
void htrm(HASH *ht);
void *htadd(HASH *ht, char *name, void *val);
void *htfind(HASH *ht, char *name);
