/// BigInt.cc

#ifndef BIG_INT_CC
#define BIG_INT_CC

#include "BigInt.h"

///
/// Konstruktoren fuer die Klasse BigInt
///

/// Default-Konstruktor

BigInt::BigInt()
{
  mpz_init(value);
}

BigInt::BigInt(int a)
{
  mpz_init(value);
  mpz_set_si(value,(long)a);
}

/// Copy-Konstruktor

BigInt::BigInt(const BigInt& a)
{
  mpz_init(value);
  mpz_set(value,a.value);
}

/// Destruktor
BigInt::~BigInt()
{
  mpz_clear(value);
}

///
/// Zuweisungsoperatoren
///

BigInt& BigInt::operator=(int a)
{
  mpz_set_si(value,(long)a);
  return *this;
}

BigInt& BigInt::operator=(const BigInt& a)
{
  mpz_set(value,a.value);
  return *this;
}

///
/// Type-Conversion
///

BigInt::operator bool()
{
  if (mpz_sgn(value)) return true;
  return false;
}

BigInt::operator int()
{
  long int ret_val;
  ret_val=mpz_get_si(value);
  return (int)ret_val;
}

BigInt::operator short()
{
  long int ret_val;
  ret_val=mpz_get_si(value);
  return (short)ret_val;
}

///
/// unary arithmetic operators
///

/// unary Minus
BigInt BigInt::operator-()
{
  BigInt erg;
  mpz_neg(erg.value,value);
  return erg;
}

/// += Operator
BigInt& BigInt::operator+=(const BigInt &a)
{
  BigInt aux;
  mpz_set(aux.value,value);
  mpz_add(value,aux.value,a.value);
  return *this;
}

/// -= Operator
BigInt& BigInt::operator-=(const BigInt &a)
{
  BigInt aux;
  mpz_set(aux.value,value);
  mpz_sub(value,aux.value,a.value);
  return *this;
}

/// *= Operator
BigInt& BigInt::operator*=(const BigInt &a)
{
  BigInt aux;
  mpz_set(aux.value,value);
  mpz_mul(value,aux.value,a.value);
  return *this;
}

/// /= Operator
BigInt& BigInt::operator/=(const BigInt &a)
{
  BigInt aux;
  mpz_set(aux.value,value);
  mpz_fdiv_q(value,aux.value,a.value);
  return *this;
}

/// prefix ++
BigInt& BigInt::operator++()
{
  BigInt aux;
  mpz_set(aux.value,value);
  mpz_add(value,aux.value,BigInt(1).value);
  return *this;
}

/// postfix ++
BigInt BigInt::operator++(int)
{
  BigInt erg;
  mpz_add(erg.value,value,BigInt(1).value);
  return erg;
}

/// prefix --
BigInt& BigInt::operator--()
{
  BigInt aux;
  mpz_set(aux.value,value);
  mpz_sub(value,aux.value,BigInt(1).value);
  return *this;
}

/// postfix --
BigInt BigInt::operator--(int)
{
  BigInt erg;
  mpz_add(erg.value,value,BigInt(1).value);
  return erg;
}

BigInt operator-(const BigInt& r)
{
  BigInt erg;
  mpz_neg(erg.value,r.value);
  return erg;
}

///
/// Vergleichsoperatorn
///

bool operator<(const BigInt& a,const BigInt& b)
{
  if (mpz_cmp(a.value,b.value)<0) return true;
  return false;
}

bool operator<=(const BigInt& a,const BigInt& b)
{
  if (mpz_cmp(a.value,b.value)>0) return false;
  return true;
}

bool operator>(const BigInt& a,const BigInt& b)
{
  if (mpz_cmp(a.value,b.value)>0) return true;
  return false;
}

bool operator>=(const BigInt& a,const BigInt& b)
{
  if (mpz_cmp(a.value,b.value)<0) return false;
  return true;
}

bool operator==(const BigInt& a,const BigInt& b)
{
  if (!mpz_cmp(a.value,b.value)) return true;
  return false;
}

bool operator!=(const BigInt& a,const BigInt& b)
{
  if (!mpz_cmp(a.value,b.value)) return false;
  return true;
}

bool operator<(const int& a,const BigInt& b)
{
  if (mpz_cmp(BigInt(a).value,b.value)<0) return true;
  return false;
}

bool operator<=(const int& a,const BigInt& b)
{
  if (mpz_cmp(BigInt(a).value,b.value)>0) return false;
  return true;
}

bool operator>(const int& a,const BigInt& b)
{
  if (mpz_cmp(BigInt(a).value,b.value)>0) return true;
  return false;
}

bool operator>=(const int& a,const BigInt& b)
{
  if (mpz_cmp(BigInt(a).value,b.value)<0) return false;
  return true;
}

bool operator==(const int& a,const BigInt& b)
{
  if (!mpz_cmp(BigInt(a).value,b.value)) return true;
  return false;
}

bool operator!=(const int& a,const BigInt& b)
{
  if (!mpz_cmp(BigInt(a).value,b.value)) return false;
  return true;
}

bool operator<(const BigInt& a,const int& b)
{
  if (mpz_cmp(a.value,BigInt(b).value)<0) return true;
  return false;
}

bool operator<=(const BigInt& a,const int& b)
{
  if (mpz_cmp(a.value,BigInt(b).value)>0) return false;
  return true;
}

bool operator>(const BigInt& a,const int& b)
{
  if (mpz_cmp(a.value,BigInt(b).value)>0) return true;
  return false;
}

bool operator>=(const BigInt& a,const int& b)
{
  if (mpz_cmp(a.value,BigInt(b).value)<0) return false;
  return true;
}

bool operator==(const BigInt& a,const int& b)
{
  if (!mpz_cmp(a.value,BigInt(b).value)) return true;
  return false;
}

bool operator!=(const BigInt& a,const int& b)
{
  if (!mpz_cmp(a.value,BigInt(b).value)) return false;
  return true;
}

///
/// die Grundoperationen
///

BigInt operator+(const BigInt& a,const BigInt &b)
{
  BigInt erg(a);
  return erg+=b;
}

BigInt operator-(const BigInt& a,const BigInt &b)
{
  BigInt erg(a);
  return erg-=b;
}

BigInt operator*(const BigInt& a,const BigInt &b)
{
  BigInt erg(a);
  return erg*=b;
}

BigInt operator/(const BigInt& a,const BigInt &b)
{
  BigInt erg(a);
  return erg/=b;
}

BigInt operator+(const int& a,const BigInt &b)
{
  BigInt erg(a);
  return erg+=b;
}

BigInt operator-(const int& a,const BigInt &b)
{
  BigInt erg(a);
  return erg-=b;
}

BigInt operator*(const int& a,const BigInt &b)
{
  BigInt erg(a);
  return erg*=b;
}

BigInt operator/(const int& a,const BigInt &b)
{
  BigInt erg(a);
  return erg/=b;
}

BigInt operator+(const BigInt& a,const int &b)
{
  BigInt erg(a);
  return erg+=BigInt(b);
}

BigInt operator-(const BigInt& a,const int &b)
{
  BigInt erg(a);
  return erg-=BigInt(b);
}

BigInt operator*(const BigInt& a,const int &b)
{
  BigInt erg(a);
  return erg*=BigInt(b);
}

BigInt operator/(const BigInt& a,const int &b)
{
  BigInt erg(a);
  return erg/=BigInt(b);
}

/// liefert das Vorzeichen
int sgn(const BigInt& a)
{
  return mpz_sgn(a.value);
}

/// liefert den Absolutbetrag
BigInt abs(const BigInt& a)
{
  BigInt erg;
  if (mpz_sgn(a.value)<0)
    mpz_neg(erg.value,a.value);
  else
    mpz_set(erg.value,a.value);
  return erg;
}

#endif  /// BIG_INT_CC
