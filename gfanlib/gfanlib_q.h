/*
 * lib_q.h
 *
 *  Created on: Sep 29, 2010
 *      Author: anders
 */

#ifndef LIB_Q_H_
#define LIB_Q_H_

#include <string.h>
#include <ostream>
#include "gmp.h"

#include "gfanlib_z.h"

namespace gfan{
class Rational
{
  mpq_t value;
public:
  static bool isField()
  {
    return true;
  }
  Rational()
  {
    mpq_init(value);
  }
  Rational(signed long int value_)
  {
    mpq_init(value);
//    mpz_init_set_si(mpq_numref(value), value_);
//    mpz_init_set_ui(mpq_denref(value), 1);
    mpz_set_si(mpq_numref(value), value_);
    mpz_set_ui(mpq_denref(value), 1);
    mpq_canonicalize(value);
  }
  Rational(Rational const & value_)
  {
    mpq_init(value);
    mpq_set(value,value_.value);
  }
  Rational(mpq_t value_)
  {
    mpq_init(value);
    mpq_set(value,value_);
  }
  explicit Rational(Integer const & value_)
  {
    mpq_init(value);
//    mpz_init_set(mpq_numref(value), value_.value);
//    mpz_init_set_ui(mpq_denref(value), 1);
    mpz_set(mpq_numref(value), value_.value);
    mpz_set_ui(mpq_denref(value), 1);
    mpq_canonicalize(value);
  }
  ~Rational()
  {
    mpq_clear(value);
  }
  Rational& operator=(const Rational& a)
    {
      const Rational *A=(const Rational*)&a;
      if (this != A) {
        mpq_clear(value);
        mpq_init(value);
        mpq_set(value,a.value);
      }
      return *this;
    }
  bool isZero()const{
    return mpz_sgn(mpq_numref(value))==0;
  }
  friend std::ostream &operator<<(std::ostream &f, Rational const &a)
  {
    void (*freefunc)(void *, size_t);
    mp_get_memory_functions(0,0,&freefunc);
    char *str=mpq_get_str(0,10,a.value);
    f<<str;
    freefunc(str,strlen(str)+1);
    return f;
  }
  Rational& operator+=(const Rational& a)
    {
      mpq_add(value,value,a.value);
      return *this;
    }
  Rational& operator-=(const Rational& a)
    {
      mpq_sub(value,value,a.value);
      return *this;
    }
  Rational& operator*=(const Rational& a)
    {
      mpq_mul(value,value,a.value);
      return *this;
    }
  Rational& operator/=(const Rational& a)
    {
      assert(!a.isZero());
      mpq_div(value,value,a.value);
      return *this;
    }
  friend Rational operator-(const Rational &b)
  {
    Rational ret;
    ret-=b;
    return ret;
  }
  Rational operator+(const Rational &a)const
  {
    Rational ret(*this);
    ret+=a;
    return ret;
  }
  Rational operator-(const Rational &a)const
  {
    Rational ret(*this);
    ret-=a;
    return ret;
  }
  Rational operator*(const Rational &a)const
  {
    Rational ret(*this);
    ret*=a;
    return ret;
  }
  Rational operator/(const Rational &a)const
  {
    Rational ret(*this);
    ret/=a;
    return ret;
  }
  void madd(const Rational &a,const Rational &b)
    {
      mpq_t temp;
      mpq_init(temp);
      mpq_mul(temp,a.value,b.value);
      mpq_add(value,value,temp);
      mpq_clear(temp);
    }
  bool operator<(const Rational &a)const
  {
    return mpq_cmp(value,a.value)<0;
  }
  bool operator==(const Rational &a)const
  {
    return mpq_cmp(value,a.value)==0;
  }
  bool operator!=(const Rational &a)const
  {
    return mpq_cmp(value,a.value)!=0;
  }
  /**
   * Returns +1,-1, or 0.
   */
  int sign()const
  {
    return mpq_sgn(value);
  }
  static Rational gcd(Rational const &a, Rational const &b, Rational &s, Rational &t)
  {
/*    mpz_t r;
    mpz_init(r);
    mpz_gcdext(r,s.value,t.value,a.value,b.value);
    Integer ret(r);
    mpz_clear(r);*/
    assert(0);
    return a;
  }
  /**
   * Assigns the value to q. q must have been initialized as a gmp variable.
   */
  void setGmp(mpq_t q)const
  {
    mpq_set(q,value);
  }
};


}

#endif /* LIB_Q_H_ */
