/* Math expression parser/unparser/evaluator */

#include <math.h>

struct node
 {
 int what;		/* What this node is */
 double value;		/* Value of number (or variable?) */
 char name[32];		/* Name of variable */
 struct node *right;	/* Right side of operator */
 struct node *left;	/* Left side of operator */
 int swaps;
 };

/* Node types */

#define wNUM	1	/* Number */
#define wVAR	2	/* Variable */
#define wFUNC	3	/* Function */

/* Precidence 40 */
#define wEXP	4

/* Precidence 30 */
#define wNEG	5

/* Precidence 20 */
#define wMUL	6
#define wDIV	7

/* Precidence 10 */
#define wADD	8
#define wSUB	9

/* Input string pointer */
char *str;

/* Error flag.  Is set if the expression is incomplete */
int err;

struct node *doparse(prec)
{
struct node *node=(struct node *)calloc(sizeof(struct node),1);
struct node *op;
int opprec, assoc;

ucloop:

/* Skip whitespace */
while(*str==' ' || *str=='\t') ++str;

if(*str>='0' && *str<='9' || *str=='.')
 {
 /* Found a number */
 node->what=wNUM;
 sscanf(str,"%lf",&(node->value));
 while(*str>='0' && *str<='9' || *str=='.' || *str=='e' || *str=='E') ++str;
 }
else if(*str>='A' && *str<='Z' || *str>='a' && *str<='z' || *str=='_')
 {
 /* Found a name */
 int x;
 node->what=wVAR;
 for(x=0;str[x]>='A' && str[x]<='Z' || str[x]>='a' && str[x]<='z' ||
         str[x]>='0' && str[x]<='9' || str[x]=='_';++x)
  node->name[x]=str[x];
 node->name[x]=0;
 str+=x;
 
 /* Check if this is a function call */
 while(*str==' ' || *str=='\t') ++str;
 if(*str=='(')
  {
  ++str;
  node->what=wFUNC;
  node->right=doparse(0);
  while(*str==' ' || *str=='\t') ++str;
  if(*str==')') ++str;
  else printf("Missing right parenthasis\n");
  }
 }
else if(*str=='(')
 {
 /* Found a parenthasis */
 free(node);
 ++str;
 node=doparse(0);
 while(*str==' ' || *str=='\t') ++str;
 if(*str==')') ++str;
 else printf("Missing right parenthasis\n");
 }
else if(*str=='-')
 {
 ++str;
 node->what=wNEG;
 node->right=doparse(30);
 }
else
 {
 err=1;
 printf("Missing expression\n");
 free(node);
 return 0;
 }

loop:
while(*str==' ' || *str=='\t') ++str;
op=(struct node *)calloc(sizeof(struct node),1);
assoc=0;

switch(*str)
 {
 case '^': opprec=40; op->what=wEXP; assoc=1; break;
 case '*': opprec=20; op->what=wMUL; break;
 case '/': opprec=20; op->what=wDIV; break;
 case '+': opprec=10; op->what=wADD; break;
 case '-': opprec=10; op->what=wSUB; break;
 default: free(op); return node;
 }

if(opprec>prec || assoc && opprec>=prec)
 {
 ++str;
 op->left=node;
 op->right=doparse(opprec);
 node=op;
 goto loop;
 }

free(op);
return node;
}

struct node *parse(s)
char *s;
{
str=s;
err=0;
return doparse(0);
}

double x;

double ev(node)
struct node *node;
{
switch(node->what)
 {
case wNUM: return node->value;
case wEXP: return pow(ev(node->left),ev(node->right));
case wMUL: return ev(node->left)*ev(node->right);
case wDIV: return ev(node->left)/ev(node->right);
case wADD: return ev(node->left)+ev(node->right);
case wSUB: return ev(node->left)-ev(node->right);
case wNEG: return -ev(node->right);
case wVAR: /* Variable lookup */
           if(!strcmp(node->name,"x")) return x;
           else
            {
            printf("Unknown variable\n");
            return 0.0;
            }
case wFUNC: /* Function call lookup */
           if(!strcmp(node->name,"sin")) return sin(ev(node->right));
           else if(!strcmp(node->name,"cos")) return cos(ev(node->right));
           else if(!strcmp(node->name,"tan")) return tan(ev(node->right));
           else
            {
            printf("Unknown function\n");
            return 0.0;
            }
case 0: return 0.0;
 }
}

rmnode(node)
struct node *node;
{
if(node)
 {
 rmnode(node->left);
 rmnode(node->right);
 free(node);
 }
}

unparselisp(node)
struct node *node;
{
switch(node->what)
 {
case wNUM: printf("%g",node->value); break;
case wEXP: printf("(^ "); unparselisp(node->left);
           printf(" "); unparselisp(node->right);
           printf(")"); break;
case wMUL: printf("(* "); unparselisp(node->left);
           printf(" "); unparselisp(node->right);
           printf(")"); break;
case wDIV: printf("(/ "); unparselisp(node->left);
           printf(" "); unparselisp(node->right);
           printf(")"); break;
case wADD: printf("(+ "); unparselisp(node->left);
           printf(" "); unparselisp(node->right);
           printf(")"); break;
case wSUB: printf("(- "); unparselisp(node->left);
           printf(" "); unparselisp(node->right);
           printf(")"); break;
case wNEG: printf("(- "); unparselisp(node->right);
           printf(")"); break;
case wVAR: printf("%s",node->name); break;
case wFUNC: printf("(%s ",node->name);
           unparselisp(node->right);
           printf(")");
           break;
case 0:    printf("?");
 }
}

unparseforth(node)
struct node *node;
{
switch(node->what)
 {
case wNUM: printf("%g",node->value); break;
case wEXP: unparseforth(node->left); printf(" "); unparseforth(node->right);
           printf(" ^"); break;
case wMUL: unparseforth(node->left); printf(" "); unparseforth(node->right);
           printf(" *"); break;
case wDIV: unparseforth(node->left); printf(" "); unparseforth(node->right);
           printf(" /"); break;
case wADD: unparseforth(node->left); printf(" "); unparseforth(node->right);
           printf(" +"); break;
case wSUB: unparseforth(node->left); printf(" "); unparseforth(node->right);
           printf(" -"); break;
case wNEG: printf("0 "); unparseforth(node->right);
           printf(" -"); break;
case wVAR: printf("%s",node->name); break;
case wFUNC: unparseforth(node->right); printf(" %s",node->name); break;
case 0:    printf("?");
 }
}

unparsefortran(node,prec)
struct node *node;
{
int opprec;

switch(node->what)
 {
case wNUM: printf("%g",node->value); return;
case wVAR: printf("%s",node->name); return;
case wFUNC: printf("%s(",node->name);
            unparsefortran(node->right,0);
            printf(")");
            return;
case 0: printf("?"); return;
 }

switch(node->what)
 {
case wEXP: opprec=40; break;
case wNEG: opprec=30; break;
case wMUL:
case wDIV: opprec=20; break;
case wADD:
case wSUB: opprec=10; break;
 }

if(opprec<prec) printf("(");
switch(node->what)
 {
case wEXP: unparsefortran(node->left,40);
           printf("^");
           unparsefortran(node->right,40);
           break;
case wMUL: unparsefortran(node->left,20);
           printf("*");
           unparsefortran(node->right,20);
           break;
case wDIV: unparsefortran(node->left,20);
           printf("/");
           unparsefortran(node->right,20);
           break;
case wADD: unparsefortran(node->left,10);
           printf("+");
           unparsefortran(node->right,10);
           break;
case wSUB: unparsefortran(node->left,10);
           printf("-");
           unparsefortran(node->right,10);
           break;
case wNEG: printf("-"); unparsefortran(node->right,30);
           break;
 }
if(opprec<prec) printf(")");
}

main()
{
char buf[128];
struct node *node;
while(printf("="), gets(buf))
 {
 node=parse(buf);
 unparselisp(node); printf("\n");
 unparseforth(node); printf("\n");
 unparsefortran(node,0); printf("\n");
 x=1.0; printf("Answer is: %g\n",ev(node));
 }
}
