/* FFT */

#include <math.h>

double fr[16384], fi[16384];	/* Data (real+imaginary) */
int n;				/* No. of data points */

main()
 {
 char buf[80];
 int x,y;
 double romega,iomega;
 double nromega,niomega;
 double raccu, iaccu;
 double a,b;

 /* Input data */
 for(n=0;gets(buf);++n) sscanf(buf,"%lf %lf",fr+n,fi+n);

 /* Compute omega=e^(-2*pi/n) */
 romega=cos(-2.0*M_PI/(double)n);
 iomega=sin(-2.0*M_PI/(double)n);

 /* Compute omega^(n/2) */
 nromega=1.0; niomega=0.0;
 for(y=0;y!=n/2+1;++y)
  {
  /* nomega*=omega */
  a=nromega*romega-niomega*iomega;
  b=niomega*romega+nromega*iomega;
  nromega=a; niomega=b;
  }

 /* Transform */
 for(y=-n/2+1;y!=n/2;++y)
  {
  for(x=n,raccu=0.0,iaccu=0.0;--x>=0;)
   {
   /* accu=accu*nomega+f */
   a=raccu*nromega-iaccu*niomega+fr[x];
   b=iaccu*nromega+raccu*niomega+fi[x];
   raccu=a; iaccu=b;
   }
  raccu/=(double)n;
  iaccu/=(double)n;

  printf("%lg %lg\n",raccu,iaccu);

  /* nomega*=omega */
  a=nromega*romega-niomega*iomega;
  b=niomega*romega+nromega*iomega;
  nromega=a; niomega=b;
  }
 }
