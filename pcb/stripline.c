#include <stdio.h>
#include <math.h>

double h1=.006;		/* Height above ground (in.) */
double h2=.006;
double t=.00137;	/* Trace thickness (in.) */
double b=.020;
double w=.006;		/* Trace width (in.) */
double er=4.5;		/* Permittivity */
double x=11.0;		/* Trace length (in.) */

double zstr_k1(w,t)
double w,t;
 {
 return (w/2.0)*(1.0 + t/(M_PI*w)*(1.0+log(4.0*M_PI*w/t)) + .255*(t/w)*(t/w));
 }

double zstr_skny(b,w,t,er)
double b,w,t,er;
 {
 return 60.0/sqrt(er)*log(4.0*b/(M_PI*zstr_k1(w,t)));
 }

double zstr_k2(b,t)
double b,t;
 {
 return 2.0/(1.0-t/b)*log(1.0/(1.0-t/b)+1.0)-
        (1.0/(1.0-t/b)-1.0)*log(1.0/((1.0-t/b)*(1.0-t/b))-1.0);
 }

double zstr_wide(b,w,t,er)
double b,w,t,er;
 {
 return 94.15/(w/b/(1.0-t/b)+zstr_k2(b,t)/M_PI)/sqrt(er);
 }

double zstrip(b,w,t,er)
double b,w,t,er;
 {
 if(w>.35*b) return zstr_wide(b,w,t,er);
 else return zstr_skny(b,w,t,er);
 }

double zoffset(h1,h2,w,t,er)
double h1,h2,w,t,er;
 {
 return 2.0*zstrip(2.0*h1+t,w,t,er)*zstrip(2.0*h2+t,w,t,er)/
           (zstrip(2.0*h1+t,w,t,er)+zstrip(2.0*h2+t,w,t,er));
 }

double pstrip(er)
double er;
 {
 return 84.72e-12*sqrt(er);
 }

double lstrip(b,w,t,x)
double b,w,t,x;
 {
 return pstrip(1.0)*zstrip(b,w,t,1.0)*x;
 }

double lostrip(h1,h2,w,t,x)
double h1,h2,w,t,x;
 {
 return pstrip(1.0)*zoffset(h1,h2,w,t,1.0)*x;
 }

double cstrip(b,w,t,er,x)
double b,w,t,er,x;
 {
 return pstrip(er)/zstrip(b,w,t,er)*x;
 }

double costrip(h1,h2,w,t,er,x)
double h1,h2,w,t,er,x;
 {
 return pstrip(er)/zoffset(h1,h2,w,t,er)*x;
 }

main(argc,argv)
char *argv[];
 {
 int n;
 for(n=1;n!=argc;++n)
  if(!strcmp(argv[n],"-w")) sscanf(argv[n+1],"%lf",&w), ++n;
  else if(!strcmp(argv[n],"-b")) sscanf(argv[n+1],"%lf",&b), ++n;
  else if(!strcmp(argv[n],"-t")) sscanf(argv[n+1],"%lf",&t), ++n;
  else if(!strcmp(argv[n],"-x")) sscanf(argv[n+1],"%lf",&x), ++n;
  else if(!strcmp(argv[n],"-er")) sscanf(argv[n+1],"%lf",&er), ++n;
  else if(!strcmp(argv[n],"-h1")) sscanf(argv[n+1],"%lf",&h1), ++n;
  else if(!strcmp(argv[n],"-h2")) sscanf(argv[n+1],"%lf",&h2), ++n;

 printf("w=%lg, b=%lg, t=%lg, x=%lg, er=%lg\n",w,b,t,x,er);
 printf("impedance=%lg ohms\n",zstrip(b,w,t,er));
 printf("inductance=%lg nH\n",lstrip(b,w,t,x)*1.0e9);
 printf("inductance per inch=%lg nH\n",lstrip(b,w,t,1.0)*1.0e9);
 printf("capacitance=%lg pF\n",cstrip(b,w,t,er,x)*1.0e12);
 printf("capacitance per inch=%lg pF\n",cstrip(b,w,t,er,1.0)*1.0e12);
 printf("delay=%lg ps/inch\n",pstrip(er)*1.0e12);

 printf("\n");
 printf("w=%lg, h1=%lg, h2=%lg, t=%lg, x=%lg, er=%lg\n",w,h1,h2,t,x,er);
 printf("impedance=%lg ohms\n",zoffset(h1,h2,w,t,er));
 }
