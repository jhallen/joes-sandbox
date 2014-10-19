#define SIZE 16

struct Bignum
  {
  unsigned char n[SIZE];

  Bignum(const Bignum &x);
  Bignum(const int &x);
  Bignum& operator =(const Bignum &x);
  Bignum& operator =(const int &x);
  Bignum operator +(const Bignum &x);
  Bignum operator -(const Bignum &x);
  Bignum operator &(const Bignum &x);
  Bignum operator |(const Bignum &x);
  Bignum operator ^(const Bignum &x);
  Bignum operator <<(const int &x);
  Bignum operator >>(const int &x);
  };
