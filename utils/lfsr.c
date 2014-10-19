#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *bits(int n,int nbits)
 {
 static char str[40];
 int x;
 for(x=0;x!=nbits;++x)
  if(n&(1<<x)) str[nbits-1-x]='1';
  else str[nbits-1-x]='0';
 str[nbits]=0;
 return str;
 }

int xor31(int num)
 {
 int q=0;
 int n;
 for(n=1;n!=(1<<31);n<<=1) q^=!!(num&n); 
 return q;
 }

int table[]=
 {
 0,0,0,0x3,0x3,0x5,0x3,0x3,0x1d,0x11,0x9,0x5,0x941,0x1601,0x2A01,0x3
 };

void showbits(int n,int nbits)
 {
 int x,y;
 for(x=1,y=1;y!=32;++y,x<<=1)
//  if(n&x) printf("%d ",nbits-y+1);
  if(n&x) printf("%d ",y - 1);
 }

int main(int argc,char *argv[])
 {
 int nbits = 4;
 int y = 0;
 int x;
 int accu=0;
 int seq=0;

 for (x = 1; argv[x]; ++x) {
  if (!strcmp(argv[x], "--help") || !strcmp(argv[x], "-h")) {
   printf("Linear Feedback Shift Registers\n\n");
   printf("syntax: lfsr --bits nn --count nn [--seq]\n\n");
   printf("Gives tap points which should feed an XNOR gate for an LFSR of\n");
   printf("the given no. of bits.  There will be 2^(bits)-1 states before the\n");
   printf("LFSR pattern repeats\n\n");
   printf("The XNOR gate should feed the left side of a right-shifting\n");
   printf("shift register.  The shift register should be reset to all zeros\n");
   printf("\n");
   printf("Also gives the state value of the specified count.\n");
   printf("For example, for a 3 bit LFSR, state value for count 0 will be 000,\n");
   printf("for count 1, will be 100, for count 2, will be 110, etc.\n");
   return 0;
  } else if (!strcmp(argv[x], "--bits")) {
   ++x;
   nbits = strtol(argv[x], NULL, 0);
   if(nbits>15 || nbits <1)
    {
    fprintf(stderr, "Sorry, this program only works up to 15 bits\n");
    return -1;
    }
  } else if (!strcmp(argv[x], "--count")) {
   ++x;
   y = strtol(argv[x], NULL, 0);
  } else if (!strcmp(argv[x], "--seq")) {
   seq = 1;
  } else {
   fprintf(stderr,"Unknown option\n");
   return -1;
  }
 }

 printf("LFSR of %d bits\n",nbits);
 printf("Tap points at: ");
 showbits(table[nbits],nbits);
 printf("\n");
 for(x=0;x!=y;++x)
  {
  int q=!xor31(accu&table[nbits]);
  accu=(accu>>1)+(q<<(nbits-1));
  }
 printf("Count %d is at state %d/$%x/%%%s\n",y,accu,accu,bits(accu,nbits));
 if (seq) {
  accu=0;
  for(x=0;x!=(1<<nbits);++x)
    {
    int q=!xor31(accu&table[nbits]);
    printf("%3.3d\t%4.4x\n",x,accu);
    accu=(accu>>1)+(q<<(nbits-1));
    }
 }
 return 0;
 }
