/*
 * lib_z.h
 *
 *  Created on: Sep 28, 2010
 *      Author: anders
 */

#ifndef LIB_Z_H_
#define LIB_Z_H_

#include <string.h>
#include <ostream>
#include "gmp.h"

namespace gfan{
  class Rational;
class Integer
{
  friend class Rational;
  mpz_t value;
public:
  static bool isField()
  {
    return false;
  }
  Integer()
  {
    mpz_init(value);
  }
  Integer(signed long int value_)
  {
    mpz_init(value);
    mpz_set_si(value,value_);
  }
  Integer(Integer const & value_)
  {
    mpz_init_set(value,value_.value);
  }
  Integer(mpz_t value_)
  {
    mpz_init_set(value,value_);
  }
  ~Integer()
  {
    mpz_clear(value);
  }
  Integer& operator=(const Integer& a)
    {
      const Integer *A=(const Integer*)&a;
      if (this != A) {
        mpz_clear(value);
        mpz_init_set(value, a.value);
      }
      return *this;
    }
  bool isZero()const{
    return mpz_sgn(value)==0;
  }
  friend std::ostream &operator<<(std::ostream &f, Integer const &a)
  {
    void (*freefunc)(void *, size_t);
    mp_get_memory_functions(0,0,&freefunc);
    char *str=mpz_get_str(0,10,a.value);
    f<<str;
    freefunc(str,strlen(str)+1);
    return f;
  }
  Integer& operator+=(const Integer& a)
    {
      mpz_add(value,value,a.value);
      return *this;
    }
  Integer& operator-=(const Integer& a)
    {
      mpz_sub(value,value,a.value);
      return *this;
    }
  Integer& operator*=(const Integer& a)
    {
      mpz_mul(value,value,a.value);
      return *this;
    }
  Integer& operator/=(const Integer& a)
    {
      mpz_div(value,value,a.value);
      return *this;
    }
  friend Integer operator-(const Integer &b)
  {
    Integer ret;
    ret-=b;
    return ret;
  }
  Integer operator+(const Integer &a)const
  {
    Integer ret(*this);
    ret+=a;
    return ret;
  }
  Integer operator-(const Integer &a)const
  {
    Integer ret(*this);
    ret-=a;
    return ret;
  }
  Integer operator*(const Integer &a)const
  {
    Integer ret(*this);
    ret*=a;
    return ret;
  }
  Integer operator/(const Integer &a)const
  {
    Integer ret(*this);
    ret/=a;
    return ret;
  }
  void madd(const Integer &a,const Integer &b)
    {
      mpz_t temp;
      mpz_init(temp);
      mpz_mul(temp,a.value,b.value);
      mpz_add(value,value,temp);
      mpz_clear(temp);
    }
  bool operator<(const Integer &a)const
  {
    return mpz_cmp(value,a.value)<0;
  }
  bool operator==(const Integer &a)const
  {
    return mpz_cmp(value,a.value)==0;
  }
  bool operator!=(const Integer &a)const
  {
    return mpz_cmp(value,a.value)!=0;
  }
  int sign()const
  {
    return mpz_sgn(value);
  }
  static Integer gcd(Integer const &a, Integer const &b, Integer &s, Integer &t)
  {
    mpz_t r;
    mpz_init(r);
    mpz_gcdext(r,s.value,t.value,a.value,b.value);
    Integer ret(r);
    mpz_clear(r);
    return ret;
  }
  /**
   * Assigns the value to z. z must have been initialized as a gmp variable.
   */
  void setGmp(mpz_t z)const
  {
    mpz_set(z,value);
  }
  /**
   * Returns a value which is useful for computing hash functions.
   */
  signed long int hashValue()const
  {
    return mpz_get_si(value);
  }
  bool fitsInInt()const
  {
    mpz_t v;
    mpz_init(v);
    this->setGmp(v);
    bool ret=(mpz_fits_sint_p(v)!=0);
    mpz_clear(v);
    return ret;
  }
  int toInt()const
  {
    mpz_t v;
    mpz_init(v);
    this->setGmp(v);
    int ret=0;
    if(mpz_fits_sint_p(v))
      ret=mpz_get_si(v);
//    else
//      ok=false;
    mpz_clear(v);
    return ret;
  }
};


}



#endif /* LIB_Z_H_ */

