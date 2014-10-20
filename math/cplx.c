#include <math.h>

typedef struct c C;
struct c
{
double r;
double i;
};

C csqrt(x)
C x;
{
C ret;
if(x.i<0.000001)
 {
 if(x.r>=0.0)
  {
  ret.r=sqrt(x.r);
  ret.i=0.0;
  }
 else
  {
  ret.r=0.0;
  ret.i= -sqrt(-x.r);
  }
 }
else
 {
 double tmp=sqrt(x.r*x.r+x.i*x.i);
 ret.r=sqrt((x.r+tmp)/2.0);
 ret.i=sqrt((tmp-x.r)/2.0);
 if(x.i<0.0) ret.i= -ret.i;
 }
return ret;
}

C mul(x,y)
C x;
C y;
{
C ret;
ret.r=x.r*y.r-x.i*y.i;
ret.i=x.i*y.r+x.r*y.i;
return ret;
}

C add(x,y)
C x;
C y;
{
C ret;
ret.r=x.r+y.r;
ret.i=x.i+y.i;
return ret;
}

C neg(x)
C x;
{
C ret;
ret.r= -x.r;
ret.i= -x.i;
return ret;
}

C sub(x,y)
C x;
C y;
{
C ret;
ret.r=x.r-y.r;
ret.i=x.i-y.i;
return ret;
}

C div(x,y)
C x;
C y;
{
C ret;
double mags=y.r*y.r+y.i*y.i;
ret.r=(x.r*y.r+x.i*y.i)/mags;
ret.i=(x.i*y.r-x.r*y.i)/mags;
return ret;
}

double mag(x)
C x;
{
return sqrt(x.r*x.r+x.i*x.i);
}

