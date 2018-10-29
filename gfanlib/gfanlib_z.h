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

#define OLD 1
#if OLD
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
  Integer(mpz_t const value_)
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

#else
namespace gfan{
  typedef signed long int word;//processor type for integers
  typedef int64_t LimbWord;//memory word. Assumed to be at least as big as word
  typedef uint64_t uLimbWord;//memory word. Assumed to be at least as big as word
  const int limbSizeInBytes=8;
#include <stdint.h>
  struct spec64malloc{
    int64_t data;
    bool hasLimbs()
    {
      return data&1;
    }
    word fitsMask()
    {
      return 0xc000000000000000;
    }
    void assign(word value)//assuming data fits
    {
      data=value<<1;
    }
    void alloc(int nlimbs)//one limb is added since that will tell the number of remaining limbs
    {
      int64_t temp=(int64_t)malloc((nlimbs+1)*limbSizeInBytes);
      assert(temp);
      data=temp+1;
    }
    LimbWord *limbs()//assuming that limbs exist
    {
      return (LimbWord*)(data-1);
    }
    word nlimbs()//assuming limbs exist
    {
      return (word)*limbs();
    }
    void copy(spec64malloc b)
    {
      if(hasLimbs())
        {
          word n2=b.nlimbs()+1;
          int64_t temp=(int64_t)malloc((n2)*limbSizeInBytes);
          assert(temp);

          data=temp+1;
          memcpy((LimbWord*)temp,limbs(),n2*limbSizeInBytes);
        }
      else
        {
          data=b.data;
        }
    }
    void doFree()//assuming that limbs exist
    {
      free((void*)(data-1));
    }
    word valueToWord()//assume no limbs and that word is big enough to contain int64_t
    {
      return data>>1;
    }
  };
  template <struct spec> struct IntegerTemplate : public spec
  {
  private:
    bool fits(word v)
    {
      return !((value_&fitsMask())^((value_<<1)&fitsMask));
    }
  public:
    static bool isField()
    {
      return false;
    }
    IntegerTemplate()
    {
      spec.data=0;
    }
    void assignWordNoFree()
    {
      if(fits(value_))
        {
          assign(value);
        }
      else
        {
          alloc(1);
          limbs()[0]=1;
          limbs()[1]=value_;
        }
    }
    IntegerTemplate(word value_)
    {
      assignWordNoFree(value_);
    }
    IntegerTemplate(IntegerTemplate const & value_)
    {
      if(value_.hasLimbs())
        {
          copy(value_);
        }
      else
        data=value.data;
    }
/*    Integer(mpz_t value_)
    {
      mpz_init_set(value,value_);
    }*/
    ~IntegerTemplate()
    {
      if(hasLimbs())doFree();
    }
    IntegerTemplate& operator=(const IntegerTemplate& a)
      {
        if(this!=&a)
          {
            if(hasLimps())doFree();
            copy(a);
          }
        return *this;
      }
    bool isZero()const{
      return data==0;
    }
    friend std::ostream &operator<<(std::ostream &f, IntegerTemplate const &a)
    {
      if(hasLimps())
        {
          LimpWord *p=limbs();
          int l=*p++;
          for(int i=0;i<l;i++)
            f<<":"<<p[i];
        }
      else
        {
          f<<valueToWord();
        }
      return f;
    }
    LimbWord signExtension(LimbWord a)
    {
      return 0-(a<0);
    }
    void addWordToLimbs(word v)
    {
      int n=nlimbs();
      LimbWord V=v;
      LimbWord *p=limbs()+1;
      LimbWord s=signExtension(V);
      for(int i=0;i<n;i++)
        {
          LimbWord r=V+*p;
          bool carry=(uLimbWord)r<(uLimbWord)V;
          V=carry+s;
        }

    }


    IntegerTemplate& operator+=(const IntegerTemplate& a)
      {
        if(hasLimbs()||a.hasLimbs())
          {
            if(!hasLimbs())
              {

              }
            else
              {
              }

          }
        else
          {
            word C=valueToWord();
            word A=a.valueToWord();
            word D=A+C;
            assignWordNoFree(D);
          }
        return *this;
      }

  };

  typedef IntegerTemplate<spec64malloc> Integer;
};
#endif

#endif /* LIB_Z_H_ */

