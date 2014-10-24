#ifndef _Iutil
#define _Iutil 1

#include <string>
using namespace std;

// Count no. of fields in a record.
int nfields(char *);

// Break up line into zero terminated fields.  A pointer to each
// field is left in an array.
void fields(char *,char **);

void mset(char *,char,int);
void msetI(int *,int,int);
void mcpy(void *,void *,int);
int mcnt(const char *,char,int);
int Iabs(int);
long Labs(long);
int toup(int c);
bool crest(int c);
bool cwhite(int c);
long ston(char *s);
char *ntos(long n);

/* int val(string); */

#endif
