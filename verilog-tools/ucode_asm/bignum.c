#include <bignum.h>

Bignum::Bignum(const Bignum &x)
  {
  for(int q=0;q!=SIZE;++q) n[q]=x.n[x];
  }

Bignum::Bignum(const int &x)
  {
  if(x<0) for(int q=0;q!=SIZE;++q) n[q]=0xFF;
  else for(int q=0;q!=SIZE;++q) n[q]=0;
  n[0]=x;
  n[1]=(x>>8);
  n[2]=(x>>16);
  n[3]=(x>>24);
  }

Bignum& operator =(const Bignum &x)
  {
  for(int q=0;q!=SIZE;++q) n[q]=x.n[x];
  }

Bignum& operator =(const int &x)
  {
  if(x<0) for(int q=0;q!=SIZE;++q) n[q]=0xFF;
  else for(int q=0;q!=SIZE;++q) n[q]=0;
  n[0]=x;
  n[1]=(x>>8);
  n[2]=(x>>16);
  n[3]=(x>>24);
  }

Bignum operator +(const Bignum &x)
  {
  Bignum y;
  int carry=0;
  for(q=0;q!=SIZE;++q) y.n[q]=carry=n[q]+x.n[q]+(carry>>8);
  return y;
  }

Bignum operator -(const Bignum &x)
  {
  Bignum y;
  int carry=0;
  for(int q=0;q!=SIZE;++q) y.n[q]=carry=n[q]-x.n[q]+(carry>>8);
  return y;
  }

Bignum operator >>(const int &x)
  {
  Bignum rtn;
  if(x==0)
    rtn=this;
  else if(x<0)
    rtn=(this<<(-x));
  else if(x>=SIZE*8)
    {// set to zero or -1 depending on sign
    }
  else
    {
    rtn=this;
    int sgn=(rtn[SIZE-1]&128)?-1:0;
    int q;
    int nbytes=(x>>3);
    int nbits=(x&7);
    if(nbytes)
      {
      for(q=0;q<SIZE-x;++q)
        rtn.n[q]=rtn.n[q+nbytes];
      for(;q<SIZE;++q)
        rtn.n[q]=sgn;
      }
    if(nbits)
      {
      for(q=0;q<SIZE-1;++q)
        rtn.n[q]=(rtn.n[q]>>nbits)+(rtn.n[q+1]<<(8-nbits));
      rtn.n[SIZE-1]=(((char *)rtn.n)[SIZE-1]>>nbits);
      }
    }
  return rtn;
  }

Bignum operator <<(const int &x)
  {
  Bignum rtn;
  if(x==0)
    rtn=this;
  else if(x<0)
    rtn=(this>>(-x));
  else if(x>=SIZE*8)
    rtn=0;
  else
    {
    int q;
    int nbytes=(x>>3);
    int nbits=(x&7);
    rtn=this;
    /* Shift bytes */
    if(nbytes)
      {
      for(q=SIZE-1;q>=nbytes;--q)
        rtn.n[q]=rtn.n[q-nbytes];
      for(;q>=0;--q)
        rtn.n[q]=0;
      }
    /* Shift bits */
    if(nbits)
      {
      for(q=SIZE-1;q>0;--q)
        rtn.n[q]=(rtn.n[q]<<nbits)+(rtn.n[q-1]>>(8-nbits));
      rtn.n[0]=(rtn.n[0]<<nbits);
      }
    }
  return rtn;
  }
