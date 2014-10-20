/*************************************/
/* Compute the roots of a polynomial */
/*************************************/

#include <bios.h>
#include <fg.h>
#include <stdio.h>
#include "cplx.c"

C one={1.0,0.0};
C zero={0.0,0.0};

typedef struct node NODE;
struct node
 {
 char type;
 union
  {
  double fp;
  struct
   {
   NODE *r;
   NODE *l;
   }
    lst;
  }
  v;
 };

#define tX 1
#define tK 2
#define tCONST 0

#define tADD 3
#define tMUL 4
#define tSUB 5
#define tDIV 6
#define tNEG 7
#define tEXP 8

NODE *new(void)
{
NODE *it=(NODE *)malloc(sizeof(NODE));
it->v.lst.r=0;
it->type=0;
it->v.lst.l=0;
return it;
}

void kill(NODE *it)
{
if(!it) return;
if(it->type>=3)
 {
 kill(it->v.lst.r);
 kill(it->v.lst.l);
 }
free(it);
}

NODE *parse(char **s,int);

NODE *thing(char **s)
{
double num;
NODE *n;
while(**s==' ' || **s=='\t') (*s)++;
switch(*((*s)++))
 {
case 'x':
case 'X':
 n=new();
 n->type=tX;
 return n;
case 'k':
case 'K':
 n=new();
 n->type=tK;
 return n;
case '(':
 n=parse(s,0);
 if(**s!=')')
  {
  kill(n);
  return 0;
  }
 (*s)++;
 return n;
case '-':
 n=new();
 n->type=tNEG;
 n->v.lst.r=thing(s);
 return n;
case '.':
case '0':
case '1':
case '2':
case '3':
case '4':
case '5':
case '6':
case '7':
case '8':
case '9':
 if(!sscanf(*s-1,"%lf",&num)) return 0;
 n=new();
 n->type=tCONST;
 n->v.fp=num;
 while(**s>='0' && **s<='9' || **s=='.' || **s=='e' || **s=='E') (*s)++;
 return n;
case 0:
default:
 (*s)--;
 return 0;
 }
}

NODE *parse(char **s,int lvl)
{
NODE *l;
NODE *t;
l=thing(s);
if(!l) return 0;
loop:
while(**s==' ' || **s=='\t') (*s)++;
switch(*((*s)++))
 {
case '^':
 if(lvl>2) goto down;
 t=new();
 t->type=tEXP;
 t->v.lst.l=l;
 t->v.lst.r=parse(s,3);
 if(!t->v.lst.r)
  {
  kill(t);
  return 0;
  }
 l=t;
 break;
case '*':
 if(lvl>1) goto down;
 t=new();
 t->type=tMUL;
 t->v.lst.l=l;
 t->v.lst.r=parse(s,2);
 if(!t->v.lst.r)
  {
  kill(t);
  return 0;
  }
 l=t;
 break;
case '/':
 if(lvl>1) goto down;
 t=new();
 t->type=tDIV;
 t->v.lst.l=l;
 t->v.lst.r=parse(s,2);
 if(!t->v.lst.r)
  {
  kill(t);
  return 0;
  }
 l=t;
 break;
case '+':
 if(lvl) goto down;
 t=new();
 t->type=tADD;
 t->v.lst.l=l;
 t->v.lst.r=parse(s,1);
 if(!t->v.lst.r)
  {
  kill(t);
  return 0;
  }
 l=t;
 break;
case '-':
 if(lvl) goto down;
 t=new();
 t->type=tSUB;
 t->v.lst.l=l;
 t->v.lst.r=parse(s,1);
 if(!t->v.lst.r)
  {
  kill(t);
  return 0;
  }
 l=t;
 break;
default:
down:
 (*s)--;
 return l;
 }
goto loop;
}

C eval(NODE *list,C,C);

int max(int x,int y)
{
if(x>=y) return x;
else return y;
}

int corder(NODE *list)
{
int order;
switch(list->type)
 {
 case tX:
  return 1;
 case tEXP:
  return corder(list->v.lst.l)*(eval(list->v.lst.r,zero,zero).r);
 case tMUL:
  return corder(list->v.lst.l)+corder(list->v.lst.r);
 case tDIV:
  return corder(list->v.lst.l)-corder(list->v.lst.r);
 case tADD:
 case tSUB:
  return max(corder(list->v.lst.l),corder(list->v.lst.r));
 case tNEG:
  return corder(list->v.lst.r);
 default:
  return 0;
 }
return order;
}

C eval(NODE *list,C x,C k)
{
C vv;
C cnt;
C mm;
switch(list->type)
 {
case tX:
 return x;
case tK:
 return k;
case tCONST:
 vv.r=list->v.fp;
 vv.i=0.0;
 return vv;
case tADD:
 return add(eval(list->v.lst.l,x,k),eval(list->v.lst.r,x,k));
case tSUB:
 return sub(eval(list->v.lst.l,x,k),eval(list->v.lst.r,x,k));
case tMUL:
 return mul(eval(list->v.lst.l,x,k),eval(list->v.lst.r,x,k));
case tDIV:
 return div(eval(list->v.lst.l,x,k),eval(list->v.lst.r,x,k));
case tNEG:
 return neg(eval(list->v.lst.r,x,k));
case tEXP:
 vv=one;
 cnt=eval(list->v.lst.r,x,k);
 mm=eval(list->v.lst.l,x,k);
 while(cnt.r>=1.0)
  {
  vv=mul(vv,mm);
  cnt.r-=1.0;
  }
 return vv;
 }
}

/* Test it */
/*
main()
{
char str[80];
char *s;
NODE *list;
C v;
while(gets(str))
 {
 s=str;
 list=parse(&s,0);
 if(!list || *s!=0)
  {
  for(;s>str;s--) printf("-");
  printf("^\n");
  printf("Syntax error\n");
  continue;
  };
 printf("Ok\n");
 v=eval(list,one,one);
 printf("%f %f\n",v.r,v.i);
 kill(list);
 }
}
*/


/**************************/
/* This is the polynomial */
/**************************/

C k;

C array[60];
NODE *list;
C func(x1)
C x1;
{
int x;
C ret;
ret=eval(list,x1,k);
for(x=0;array[x].r!=0.0 || array[x].i!=0.0;x++)
 ret=div(ret,sub(x1,array[x]));
return ret;
}

/**************/
/* The method */
/**************/

C two={2.0,0.0};
C ntwo={-2.0,0.0};
C four={4.0,0.0};
C x;
C ix;
C iix;

C muller()
{
C xi;

C fi;
C f;
C iff;
C iif;

C g;

C gamma;
C gammai;

C h;
C hi;

C bot;
C top;
C a;

f=func(x);
iff=func(ix);
iif=func(iix);

h=sub(x,ix);
gamma=div(h,sub(ix,iix));
goto into;
loop:
g=
sub(mul(add(one,mul(two,gamma)),sub(f,iff)),
    mul(mul(gamma,gamma),sub(iff,iif)));
bot=csqrt(
sub(mul(g,g),
    mul(four,
     mul(f,
      mul(add(one,gamma),
       mul(gamma,
        sub(f,
         sub(iff,
          mul(gamma,sub(iff,iif))))))))));
top=mul(ntwo,mul(f,add(one,gamma)));
a=add(g,bot);
if(mag(top)<mag(a)) gammai=div(top,a);
else gammai=div(top,sub(g,bot));

hi=mul(h,gammai);
xi=add(x,hi);
fi=func(xi);

iix=ix; ix=x; x=xi;
iif=iff; iff=f; f=fi;
h=hi; gamma=gammai;
into:
if(mag(f)<0.01) return;
goto loop;
}

main()
{
char str[80];
char olds[80];
char *ss;
C have[60];
C have1[60];
fg_line_t lin;
double s;
int xxx;
int flag=0;
int q;
int yyy;
int z;
int order;

mmmm:
printf("Enter function in complex variable X and real parameter K\n");
mmm1:
if(!gets(str)) return 0;
ss=str;
if(!*ss) goto mmm1;
list=parse(&ss,0);
if(!list || *ss)
 {
 for(;ss>str;ss--) printf("-");
 printf("^\n");
 printf("Syntax error.\n");
 goto mmm1;
 }
order=corder(list);
printf("Order is %d\n",order);
if(order<3)
 {
 printf("Order of equation must be 3 or greater\n");
 goto mmm1;
 }
bioskey(0);
s=.01;
flag=0;
fg_init_all();
xxx=fg_displaybox[FG_X2]/2;
yyy=fg_displaybox[FG_Y2]/2;
lin[FG_X1]=xxx;
lin[FG_X2]=xxx;
lin[FG_Y1]=0;
lin[FG_Y2]=yyy*2;
fg_drawline(FG_WHITE,FG_MODE_SET,~0,FG_LINE_SOLID,lin);
lin[FG_X1]=0;
lin[FG_X2]=xxx*2;
lin[FG_Y1]=yyy;
lin[FG_Y2]=yyy;
fg_drawline(FG_WHITE,FG_MODE_SET,~0,FG_LINE_SOLID,lin);
for(z=0;z<order;z++)
 {
 have[z*3].r=0.0;
 have[z*3].i=0.0;
 have[z*3+1].r= -1.0;
 have[z*3+1].i=0.0;
 have[z*3+2].r=1.0;
 have[z*3+2].i=0.0;
 }
for(k.r=.1,k.i=0.0;1;k.r=k.r+s)
 {
 array[0]=zero;
 for(z=0;z<order;z++)
  {
  x=have[z*3];
  ix=have[z*3+1];
  iix=have[z*3+2];
  muller();
  lin[FG_X1]=xxx+(int)(40.0*x.r);
  lin[FG_Y1]=yyy+(int)(40.0*x.i);
  if(flag)
   {
   int z;
   C cc;
   cc=have[0];
   for(z=0;z<order;z++) if(mag(sub(x,cc))>mag(sub(x,have[3*z]))) cc=have[3*z];
   lin[FG_X2]=xxx+(int)(40.0*cc.r);
   lin[FG_Y2]=yyy+(int)(40.0*cc.i);
   }
  else
   {
   lin[FG_X2]=lin[FG_X1];
   lin[FG_Y2]=lin[FG_Y1];
   }
/*  printf("%f %f\n",have[z*3].i,x.i);*/
  have1[z*3]=x;
  have1[z*3+1]=ix;
  have1[z*3+2]=iix;
  array[z]=x;
  array[z+1]=zero;
  fg_drawthickline(FG_YELLOW,FG_MODE_SET,~0,FG_LINE_SOLID,lin,fg_displaybox,2);
  }
 sprintf(str,"K=%f",k.r);
 if(flag)
  fg_puts(FG_BLACK,FG_MODE_SET,~0,FG_ROT0,0,0,olds,fg_displaybox);
 fg_puts(FG_YELLOW,FG_MODE_SET,~0,FG_ROT0,0,0,str,fg_displaybox);
 strcpy(olds,str);
 flag=1;
 if(bioskey(1))
  {
  fg_term();
  return;
  }
 for(q=0;q<order*3;q++) have[q]=have1[q];
 }
}

