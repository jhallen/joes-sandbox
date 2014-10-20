#include <stdio.h>
#include <math.h>

double h=.006;		/* Height above ground (in.) */
double w=.008;		/* Trace width (in.) */
double t=.00137;	/* Trace thickness (in.) */
double er=4.5;		/* Permittivity */
double x=11.0;		/* Trace length (in.) */

double e_skny(h,w,er)
double h,w,er;
 {
 return (er+1.0)/2.0+(er-1.0)/2.0*(pow(1+12.0*h/w,-.5)+.04*(1.0-w/h)*(1.0-w/h));
 }

double e_wide(h,w,er)
double h,w,er;
 {
 return (er+1.0)/2.0+(er-1.0)/2.0*pow(1.0+12.0*h/w,-.5);
 }

double e_temp(h,w,er)
double h,w,er;
 {
 if(w>h) return e_wide(h,w,er);
 else return e_skny(h,w,er);
 }

double eeff(h,w,t,er)
double h,w,t,er;
 {
 double rtn=e_temp(h,w,er)-((er-1.0)*(t/h))/(4.6*sqrt(w/h));
 return rtn;
 }

double we_skny(h,w,t)
double h,w,t;
 {
 return w+1.25*t/M_PI*(1.0+log(4.0*M_PI*w/t));
 }

double we_wide(h,w,t)
double h,w,t;
 {
 return w+1.25*t/M_PI*(1.0+log(2.0*h/t));
 }

double we(h,w,t)
double h,w,t;
 {
 if(w>h/(2.0*M_PI)) return we_wide(h,w,t);
 else return we_skny(h,w,t);
 }

double zms_skny(h,w,t)
double h,w,t;
 {
 return 60.0*log(8.0*h/we(h,w,t)+we(h,w,t)/(4.0*h));
 }

double zms_wide(h,w,t)
double h,w,t;
 {
 return 120.0*M_PI/(we(h,w,t)/h+1.393+.667*log(we(h,w,t)/h+1.444));
 }

double zmstrip(h,w,t,er)
double h,w,t,er;
 {
 if(w>h) return zms_wide(h,w,t)/sqrt(eeff(h,w,t,er));
 else return zms_skny(h,w,t)/sqrt(eeff(h,w,t,er));
 }

double pmstrip(h,w,t,er)
double h,w,t,er;
 {
 return 84.72e-12*sqrt(eeff(h,w,t,er));
 }

double lmstrip(h,w,t,x)
double h,w,t,x;
 {
 return pmstrip(h,w,t,1.0)*zmstrip(h,w,t,1.0)*x;
 }

double cmstrip(h,w,t,er,x)
double h,w,t,er,x;
 {
 return pmstrip(h,w,t,er)/zmstrip(h,w,t,er)*x;
 }

main(argc,argv)
char *argv[];
 {
 int n;
 for(n=1;n!=argc;++n)
  if(!strcmp(argv[n],"-w")) sscanf(argv[n+1],"%lf",&w), ++n;
  else if(!strcmp(argv[n],"-h")) sscanf(argv[n+1],"%lf",&h), ++n;
  else if(!strcmp(argv[n],"-t")) sscanf(argv[n+1],"%lf",&t), ++n;
  else if(!strcmp(argv[n],"-x")) sscanf(argv[n+1],"%lf",&x), ++n;
  else if(!strcmp(argv[n],"-er")) sscanf(argv[n+1],"%lf",&er), ++n;

 printf("w=%lg, h=%lg, t=%lg, x=%lg, er=%lg\n",w,h,t,x,er);
 printf("impedance=%lg ohms\n",zmstrip(h,w,t,er));
 printf("inductance=%lg nH\n",lmstrip(h,w,t,x)*1.0e9);
 printf("inductance per inch=%lg nH\n",lmstrip(h,w,t,1.0)*1.0e9);
 printf("capacitance=%lg pF\n",cmstrip(h,w,t,er,x)*1.0e12);
 printf("capacitance per inch=%lg pF\n",cmstrip(h,w,t,er,1.0)*1.0e12);
 printf("delay=%lg ps/inch\n",pmstrip(h,w,t,er)*1.0e12);
 }
