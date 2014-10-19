/* Compute column subtotals */

/* in:
 * a 1 0 0 0 3 
 * a 1 0 0 0 5
 * b 0 1 1 3 0
 * b 0 1 1 1 0
 *
 * out:
 * a 2 0 0 0 8
 * b 0 2 2 4 0
 *
 * Fields must be single-tab separated
 */
 

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Number of columns */
/* #define N 5 */
#define MAXN 50

int N=0;
/* Guess number of columns */
void guess(char *s)
{
/* Skip over word */
while(*s && *s!=' ' && *s!='\t') ++s;

/* Skip whitespace */
while(*s==' ' || *s=='\t') ++s;

while(*s>='0' && *s<='9')
  {
  ++N;
  /* Skip to first number */
  while(*s>='0' && *s<='9') ++s;
  /* Skip over whitespace */
  while(*s==' ' || *s=='\t') ++s;
  }

printf("Guessing %d\n",N);
}

char prev[1024];
char buf[1024];

int cmp(char *a,char *b)
{
while(*a && *b && *a==*b && *a!='\t' && *b!='\t')
  {
  ++a;
  ++b;
  }
if((!*a || *a=='\t') && (!*b || *b=='\t')) return 1;
else return 0;
}

char *field(char *a)
{
while(*a && *a!='\t') ++a;
if(*a=='\t') *a++=0;
return a;
}

int total[MAXN];

int main()
{
int x;
while(gets(buf))
  {
  if(!N) guess(buf);
  if(cmp(buf,prev))
    {
    char *q=buf;
    for(x=0;x!=N;++x)
      total[x]+=atoi(q=field(q));
    }
  else
    {
    char *q;
    if(prev[0])
      {
      printf("%s",prev);
      for(x=0;x!=N;++x)
        printf("	%d",total[x]);
      for(x=0;x!=N;++x) total[x]=0;
      printf("\n");
      }
    strcpy(prev,buf);
    q=prev;
    for(x=0;x!=N;++x)
      total[x]+=atoi(q=field(q));
    }
  }
if(prev[0])
  {
  printf("%s",prev);
  for(x=0;x!=N;++x)
    printf("	%d",total[x]);
  printf("\n");
  }
return 0;
}
