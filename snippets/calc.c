/* Tiny little calculator program */

#include <math.h>
#include <stdio.h>
#include <string.h>

int err;

struct var
 {
 char *name;
 int set;
 double val;
 struct var *next;
 } *vars=0;

struct var *get(str)
char *str;
 {
 struct var *v;
 for(v=vars;v;v=v->next) if(!strcmp(v->name,str)) return v;
 v=(struct var *)malloc(sizeof(struct var));
 v->set=0;
 v->next=vars;
 vars=v;
 v->name=strdup(str);
 return v;
 }

char *ptr;
struct var *dumb;

double expr(prec,rtv)
struct var **rtv;
 {
 double x=0.0;
 struct var *v=0;
 while(*ptr==' ' || *ptr=='\t') ++ptr;
 if(*ptr>='a' && *ptr<='z' || *ptr>='A' && *ptr<='Z' || *ptr=='_')
  {
  char *s=ptr, c;
  while(*ptr>='a' && *ptr<='z' || *ptr>='A' && *ptr<='Z' || *ptr=='_' ||
        *ptr>='0' && *ptr<='9') ++ptr;
  c= *ptr; *ptr=0;
  v=get(s);
  x=v->val;
  *ptr=c;
  }
 else if(*ptr>='0' && *ptr<='9' || *ptr=='.')
  {
  sscanf(ptr,"%lg",&x);
  while(*ptr>='0' && *ptr<='9' || *ptr=='.' || *ptr=='e' || *ptr=='E') ++ptr;
  }
 else if(*ptr=='(')
  {
  ++ptr;
  x=expr(0,&v);
  if(*ptr==')') ++ptr;
  else { printf("Missing )\n"); if(!err) err=1; }
  }
 else if(*ptr=='-')
  {
  ++ptr;
  x= -expr(10,&dumb);
  }
 loop:
 while(*ptr==' ' || *ptr=='\t') ++ptr;
 if(*ptr=='^' && 6>=prec)
  {
  ++ptr;
  x=pow(x,expr(6,&dumb));
  goto loop;
  }
 else if(*ptr=='*' && 5>prec)
  {
  ++ptr;
  x*=expr(5,&dumb);
  goto loop;
  }
 else if(*ptr=='/' && 5>prec)
  {
  ++ptr;
  x/=expr(5,&dumb);
  goto loop;
  }
 else if(*ptr=='+' && 4>prec)
  {
  ++ptr;
  x+=expr(4,&dumb);
  goto loop;
  }
 else if(*ptr=='-' && 4>prec)
  {
  ++ptr;
  x-=expr(4,&dumb);
  goto loop;
  }
 else if(*ptr=='=' && 2>=prec)
  {
  ++ptr;
  x=expr(2,&dumb);
  if(v) v->val=x, v->set=1;
  else { printf("Left side of = is not an l-value\n"); err=2; }
  goto loop;
  }
 *rtv=v;
 return x;
 }

main()
 {
 char buf[1024];
 int eqn=0;
 while(printf("="), gets(buf))
  {
  double result;
  ptr=buf;
  err=0;
  while(*ptr==' ' || *ptr=='\t') ++ptr;
  if(*ptr && *ptr!='#')
   {
   struct var *v;
   result=expr(0,&dumb);
   while(*ptr==' ' || *ptr=='\t') ++ptr;
   if(*ptr && *ptr!='#') { printf("Extra junk after end of expr\n"); err=2; }
   if(!err) printf("r%d: %g\n",eqn,result);
   sprintf(buf,"r%d",eqn++);
   v=get(buf);
   v->val=result;
   v->set=1;
   }
  }
 }
